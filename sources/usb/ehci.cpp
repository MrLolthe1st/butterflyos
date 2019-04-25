
#include "ehci.h"
#include "usbd.h"
#include "../includes/string.h"
#include "../includes/interrupts.h"

//Turn off optimizations: MMIO
#pragma GCC push_options
#pragma GCC optimize("O0")
#define uint unsigned int


// ------------------------------------------------------------------------------------------------
// Port Status and Control Registers. There are defines for PORTSC register, to don't override PORT status/control for hubs, etc.

#define PORT_CONNECTION                 (1 << 0)    // Current Connect Status
#define PORT_CONNECTION_CHANGE          (1 << 1)    // Connect Status Change
#define PORT_ENABLE                     (1 << 2)    // Port Enabled
#define PORT_ENABLE_CHANGE              (1 << 3)    // Port Enable Change
#define PORT_OVER_CURRENT               (1 << 4)    // Over-current Active
#define PORT_OVER_CURRENT_CHANGE        (1 << 5)    // Over-current Change
#define PORT_FPR                        (1 << 6)    // Force Port Resume
#define PORT_SUSPEND                    (1 << 7)    // Suspend
#define PORT_RESET                      (1 << 8)    // Port Reset
#define PORT_LS_MASK                    (3 << 10)   // Line Status
#define PORT_LS_SHIFT                   10
#define PORT_POWER                      (1 << 12)   // Port Power
#define PORT_OWNER                      (1 << 13)   // Port Owner
#define PORT_IC_MASK                    (3 << 14)   // Port Indicator Control
#define PORT_IC_SHIFT                   14
#define PORT_TC_MASK                    (15 << 16)  // Port Test Control
#define PORT_TC_SHIFT                   16
#define PORT_WKCNNT_E                   (1 << 20)   // Wake on Connect Enable
#define PORT_WKDSCNNT_E                 (1 << 21)   // Wake on Disconnect Enable
#define PORT_WKOC_E                     (1 << 22)   // Wake on Over-current Enable
#define PORT_RWC                        (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE | PORT_OVER_CURRENT_CHANGE)
#define EHCI_DEVICE_QUERIES_MAX			3

// ------------------------------------------------------------------------------------------------
bit32u mmio_read(const size_t base, const size_t offset) {
	return *((volatile bit32u *)(base + offset));
}

// ------------------------------------------------------------------------------------------------
void mmio_write(const size_t base, const size_t offset, const bit32u val) {
	volatile bit32u *ptr = (volatile bit32u *)(base + offset);
	*ptr = val;

}

// ------------------------------------------------------------------------------------------------
void ehci_write_register(EhciController * hc, uint reg, uint val)
{
	mmio_write(hc->ehcibase, reg, val);
}

// ------------------------------------------------------------------------------------------------
uint ehci_read_register(EhciController * hc, uint reg)
{
	return mmio_read(hc->ehcibase, reg);
}

// ------------------------------------------------------------------------------------------------
void WCR(EhciController * hc, uint reg, uint val)
{
	mmio_write(hc->ehcibase2, reg, val);
}

// ------------------------------------------------------------------------------------------------
uint RCR(EhciController * hc, uint reg)
{
	return mmio_read(hc->ehcibase2, reg);
}

// ------------------------------------------------------------------------------------------------
static EhciTD *ehci_alloc_TD(EhciController *hc)
{
	// TODO - better memory management
	EhciTD *end = hc->tdPool + MAX_TD;
	for (EhciTD *td = hc->tdPool; td != end; ++td)
	{
		if (!td->active)
		{
			td->active = 1;
			return td;
		}
	}

	return 0;
}

// ------------------------------------------------------------------------------------------------
static EhciQH *ehci_alloc_QH(EhciController *hc)
{
	// TODO - better memory management
	EhciQH *end = hc->qhPool + MAX_QH;
	for (EhciQH *qh = hc->qhPool; qh != end; ++qh)
	{
		if (!qh->active)
		{
			qh->active = 1;
			return qh;
		}
	}

	return 0;
}

// ------------------------------------------------------------------------------------------------
static void ehci_free_TD(EhciTD *td)
{
	td->active = 0;
}

// ------------------------------------------------------------------------------------------------
static void ehci_free_QH(EhciQH *qh)
{
	qh->active = 0;
}

extern "C" {
	// ------------------------------------------------------------------------------------------------
	static void ehci_insert_async_qh(EhciQH *list, EhciQH *qh)
	{
		if (!list) return;
		if (!qh) return;
		EhciQH *end = LinkData(list->qhLink.prev, EhciQH, qhLink);

		qh->qhlp = (size_t)list | PTR_QH;
		end->qhlp = (size_t)qh | PTR_QH;

		LinkBefore(&list->qhLink, &qh->qhLink);
	}

	// -----------------------------s------------------------------------------------------------------
	static void ehci_insert_periodic_qh(EhciQH *list, EhciQH *qh)
	{
		EhciQH *end = LinkData(list->qhLink.prev, EhciQH, qhLink);

		qh->qhlp = PTR_TERMINATE;
		end->qhlp = (u32)(uintptr_t)qh | PTR_QH;

		LinkBefore(&list->qhLink, &qh->qhLink);
	}

	// ------------------------------------------------------------------------------------------------
	static void ehci_remove_from_qh(EhciQH *qh)
	{
		EhciQH *prev = LinkData(qh->qhLink.prev, EhciQH, qhLink);

		prev->qhlp = qh->qhlp;
		LinkRemove(&qh->qhLink);
	}
}

// ------------------------------------------------------------------------------------------------
static void EhciPortSet(volatile u32 *portReg, u32 data)
{
	volatile u32 status = *portReg;

	status |= data;

	*portReg = status;
}

// ------------------------------------------------------------------------------------------------
static void ehci_port_clear_bit(volatile u32 *portReg, u32 data)
{
	volatile u32 status = *portReg;

	status &= ~data;

	*portReg = status;

}

// ------------------------------------------------------------------------------------------------
static void ehci_init_td(EhciTD *td, EhciTD *prev,
	uint toggle, uint packetType,
	uint len, const void *data)
{
	if (prev)
	{
		prev->link = (size_t)td | PTR_QH;
		prev->tdNext = (size_t)td;
	}

	td->link = PTR_TERMINATE;
	td->altLink = PTR_TERMINATE;
	td->tdNext = 0;

	td->token =
		(toggle << TD_TOK_D_SHIFT) |
		(len << TD_TOK_LEN_SHIFT) |
		(3 << TD_TOK_CERR_SHIFT) |
		(packetType << TD_TOK_PID_SHIFT) |
		TD_TOK_ACTIVE;

	// Data buffer (not necessarily page aligned)
	void * p = (void *) data;
	td->buffer[0] = (u32) p;
	td->extBuffer[0] = 0;
	p = (void *) ((size_t) p & ~((size_t)0xfff));

	// Remaining pages of buffer memory must be aligned
	// to 12-bits boundary
	for (uint i = 1; i < 4; ++i)
	{
		p = (void*)((size_t) p + (size_t)0x1000);
		td->buffer[i] = (u32)(p);
		td->extBuffer[i] = (u32)((long long) p >> 32);
	}
}

// ------------------------------------------------------------------------------------------------
static void ehci_init_qh(EhciQH *qh, UsbTransfer *t, EhciTD *td, UsbDevice *parent, bool interrupt, uint speed, uint addr, uint endp, uint maxSize)
{
	if (!qh) return;
	qh->transfer = t;

	uint ch =
		(maxSize << QH_CH_MPL_SHIFT) |
		QH_CH_DTC |
		(speed << QH_CH_EPS_SHIFT) |
		(endp << QH_CH_ENDP_SHIFT) |
		addr;
	uint caps =
		(1 << QH_CAP_MULT_SHIFT);

	if (!interrupt)
	{
		ch |= 5 << QH_CH_NAK_RL_SHIFT;
	}

	if (speed != USB_HIGH_SPEED && parent)
	{
		if (interrupt)
		{
			// split completion mask - complete on frames 2, 3, or 4
			caps |= (0x1c << QH_CAP_SPLIT_C_SHIFT);
		}
		else
		{
			ch |= QH_CH_CONTROL;
		}

		caps |=
			(parent->port << QH_CAP_PORT_SHIFT) |
			(parent->addr << QH_CAP_HUB_ADDR_SHIFT);
	}

	if (interrupt)
	{
		// interrupt schedule mask - start on frame 0
		caps |= (0x01 << QH_CAP_INT_SCHED_SHIFT);
	}

	qh->ch = ch;
	qh->caps = caps;
	qh->tdHead = (size_t)td;
	qh->nextLink = (size_t)td;
	qh->token = 0;
}


// ------------------------------------------------------------------------------------------------
static void ehci_process_qh(EhciController *hc, EhciQH *qh)
{
	if (!hc) return;
	if (!qh) return;
	UsbTransfer *t = qh->transfer;
	// Clear L1 chache
	char * mem = (char *) VIDEO_SEG;
	*mem ^= 0;

	if (qh->token & TD_TOK_HALTED)
	{
		t->success = false;
		t->complete = true;
	}
	else if (qh->nextLink & PTR_TERMINATE)
	{
		if (~qh->token & TD_TOK_ACTIVE)
		{
			if (qh->token & TD_TOK_DATABUFFER)
			{
				printf(" Data Buffer Erehci_read_register\n");
			}
			if (qh->token & TD_TOK_BABBLE)
			{
				printf(" Babble Detected\n");
			}
			if (qh->token & TD_TOK_XACT)
			{
				printf(" Transaction Erehci_read_register\n");
			}
			if (qh->token & TD_TOK_MMF)
			{
				printf(" Missed Micro-Frame\n");
			}

			t->success = true;
			t->complete = true;
		}
	}

	if (t->complete)
	{
		// Clear transfer from queue
		qh->transfer = 0;

		// Update endpoint toggle state
		if (t->success && t->endp)
		{
			t->endp->toggle ^= 1;
		}

		// Remove queue from schedule
		ehci_remove_from_qh(qh);

		// Free transfer descriptors
		EhciTD *td = (EhciTD *)(uintptr_t)qh->tdHead;
		while (td)
		{
			EhciTD *next = (EhciTD *)(uintptr_t)td->tdNext;
			ehci_free_TD(td);
			td = next;
		}

		// Free queue head
		ehci_free_QH(qh);
	}
}


// ------------------------------------------------------------------------------------------------
static void EhciWaitForQH(EhciController *hc, EhciQH *qh)
{
	long long start_time = time1024;

	UsbTransfer *t = qh->transfer;
	// Wait with timeout
	int timeout = 4000;
	while (!t->complete && time1024 - start_time < timeout + 1)
	{
		ehci_process_qh(hc, qh);
	}

	if (time1024 - start_time >= timeout)
	{
		log_debug(DEBUG_ERROR, "transaction timeot");
		t->complete = true;
		t->success = false;
	}

}
void EhciPrintQH(EhciQH *qh, EhciQH * strt, int c = 0)
{
	if (!qh) return;
	qh = (EhciQH*)((size_t)qh&(~7));
	if (c > 0 && qh == strt) return;
	printf("qh=0x%08x;", qh);
	printf(" qhlp=0x%08x\n", qh->qhlp);
	printf(" ch=0x%08x;", qh->ch);
	printf(" caps=0x%08x\n", qh->caps);
	printf(" curLink=0x%08x\n", qh->curLink);
	//	printf(" nextLink=0x%08x\n", qh->nextLink);
	//	printf(" altLink=0x%08x\n", qh->altLink);
		//printf(" token=0x%08x\n", qh->token);
		//printf(" buffer=0x%08x\n", qh->buffer[0]);
	EhciPrintQH((EhciQH*)qh->qhlp, strt, c + 1);
}
// ------------------------------------------------------------------------------------------------
static void ehci_device_control(UsbDevice *dev, UsbTransfer *t)
{
	if (!dev) return;
	if (!t) return;
	EhciController *hc = (EhciController *)dev->hc;
	UsbDevReq *req = t->req;

	// Determine transfer properties
	uint speed = dev->speed;
	uint addr = dev->addr;
	uint maxSize = dev->maxPacketSize;
	uint type = req->type;
	uint len = req->len;

	// Create queue of transfer descriptors
	EhciTD *td = ehci_alloc_TD(hc);
	if (!td)
	{
		return;
	}

	EhciTD *head = td;
	EhciTD *prev = 0;

	// Setup packet
	uint toggle = 0;
	uint packetType = USB_PACKET_SETUP;
	uint packetSize = sizeof(UsbDevReq);
	ehci_init_td(td, prev, toggle, packetType, packetSize, req);
	prev = td;

	// Data in/out packets
	packetType = type & RT_DEV_TO_HOST ? USB_PACKET_IN : USB_PACKET_OUT;

	u8 *it = (u8 *)t->data;
	u8 *end = it + len;

	while (it < end)
	{
		td = ehci_alloc_TD(hc);
		if (!td)
		{
			return;
		}

		toggle ^= 1;
		packetSize = end - it;
		if (packetSize > maxSize)
		{
			packetSize = maxSize;
		}

		ehci_init_td(td, prev, toggle, packetType, packetSize, it);

		it += packetSize;
		prev = td;
	}

	// Status packet
	td = ehci_alloc_TD(hc);
	if (!td)
	{
		return;
	}

	toggle = 1;
	packetType = type & RT_DEV_TO_HOST ? USB_PACKET_OUT : USB_PACKET_IN;
	ehci_init_td(td, prev, toggle, packetType, 0, 0);
	// Initialize queue head
	EhciQH *qh = ehci_alloc_QH(hc);
	ehci_init_qh(qh, t, head, dev->parent, false, speed, addr, 0, maxSize);
	// Wait until queue has been processed
	ehci_insert_async_qh(hc->asyncQH, qh);

	EhciWaitForQH(hc, qh);
}

// ------------------------------------------------------------------------------------------------
static void ehci_dev_interface(UsbDevice *dev, UsbTransfer *t)
{
	// Bulk request
	// Get HC
	EhciController *hc = (EhciController *)dev->hc;

	// Determine transfer properties
	uint speed = dev->speed;
	uint addr = dev->addr;
	uint maxSize = t->endp->desc->maxPacketSize;
	// Set endpoint address
	uint endp = t->endp->desc->addr & 0xf;

	// Create queue of transfer descriptors
	EhciTD *td = ehci_alloc_TD(hc);
	if (!td)
	{
		t->success = false;
		t->complete = true;
		return;
	}

	EhciTD *head = td;
	EhciTD *prev = 0;

	// Data in/out packets
	uint toggle = t->endp->toggle;
	uint packetType = t->endp->desc->addr & 0x80 ? USB_PACKET_IN : USB_PACKET_OUT;

	uint packetSize = t->len;

	ehci_init_td(td, prev, toggle, packetType, packetSize, t->data);

	// Initialize queue head
	EhciQH *qh = ehci_alloc_QH(hc);
	ehci_init_qh(qh, t, head, dev->parent, true, speed, addr, endp, maxSize);

	// Schedule queue
	if (t->w)
		ehci_insert_async_qh(hc->asyncQH, qh);
	else
		ehci_insert_periodic_qh(hc->periodicQH, qh);
	
	if (t->w)
		EhciWaitForQH(hc, qh);

}


// ------------------------------------------------------------------------------------------------
static uint ehci_reset_port(EhciController *hc, uint port)
{
	volatile u32 *reg = &hc->opRegs->ports[port];
	// Reset the port

	EhciPortSet(reg, PORT_POWER);
	Wait(10);
	EhciPortSet(reg, PORT_RESET);
	Wait(10);
	ehci_port_clear_bit(reg, PORT_RESET);

	unsigned int status = 0;

	for (int i = 0; i < EHCI_DEVICE_QUERIES_MAX; ++i)
	{

		// Get current status
		status = *reg;

		// Check for line status bits:
		// 01b: Low-speed device, release ownership of port,
		// 00b: everything is ok
		// another states are unreachable: we've set 1 to config register
		if (status & (3 << 10))
		{
			// Release ownership by setting 'Port owner' bit
			*reg |= ((1 << 13));

			// Now, we can't manipulate device, just return zero
			return 0;
		}

		// Check if device is attached to port
		// If connection bit isn't set, device isn't present
		if (~status & PORT_CONNECTION)
			break;

		// Acknowledge change in status
		if (status & (PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE))
		{
			// Clear status change bits
			ehci_port_clear_bit(reg, PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE);
			continue;
		}

		// Check if device is enabled
		if (status & PORT_ENABLE)
			break;

		// Delay
		Wait(10);
	}

	return status;
}



void probeEhciPort(EhciController *hc, uint port)
{
	// Reset port, and get status
	uint status = ehci_reset_port(hc, port);

	// if port is enabled, create descriptor,
	// and try to detect device characteristcs

	if (status & PORT_ENABLE)
	{
		uint speed = USB_HIGH_SPEED;
		log_debug(DEBUG_INFO, "Detected USB device on port %d", port);
		UsbDevice *dev = UsbDevCreate();
		if (dev)
		{
			dev->parent = 0;
			dev->hc = hc;
			dev->port = port;
			dev->speed = speed;
			dev->maxPacketSize = 8;
			dev->hcControl = ehci_device_control;
			dev->hcIntr = ehci_dev_interface;

			if (!UsbDevInit(dev))
			{
				printf("stalled.\n");
				// TODO - cleanup
			}
		}
	}
}


// ------------------------------------------------------------------------------------------------
static void ehci_probe_ports(EhciController *hc)
{
	// Port setup:
	// read port count, iterate over all ports, and
	// enable, if needed
	uint portCount = RCR(hc, hcsParamsO) & HCSPARAMS_N_PORTS_MASK;
	for (uint port = 0; port < portCount; ++port)
	{
		probeEhciPort(hc, port);
	}
}

extern "C" {
	static void ehci_controller_pollList(EhciController *hc, Link *list)
	{
		EhciQH *qh;
		EhciQH *next;/*
		//lockTaskSwitch(1);
		ListForEachSafe(qh, next, *list, qhLink)
		{
			if (qh->transfer && (qh->transfer->w == 0) && (!qh->transfer->complete))
			{
				ehci_process_qh(hc, qh);
			}
		}*/
		//unlockTaskSwitch();
	}

}
// ------------------------------------------------------------------------------------------------
static void ehci_controller_poll(UsbController *controller)
{
	EhciController *hc = (EhciController *)controller->hc;
	ehci_controller_pollList(hc, &hc->asyncQH->qhLink);
	ehci_controller_pollList(hc, &hc->periodicQH->qhLink);
	uint portCount = RCR(hc, hcsParamsO) & HCSPARAMS_N_PORTS_MASK;
	for (uint port = 0; port < portCount; ++port)
	{
		volatile u32 *reg = &hc->opRegs->ports[port];
		uint status = *reg;
		if (status & PORT_CONNECTION_CHANGE && !(status & PORT_CONNECTION))
		{
			//Device disconnected

			UsbDevice * cur = g_usbDeviceList, * ls = 0;
			//Clear Connection change bit
			ehci_port_clear_bit(reg, PORT_CONNECTION_CHANGE);
			while (cur)
			{
				if (cur->port == port && (uint)cur->hc == (uint)hc)
				{
				//	printf("USB device has disconnected from EHCI!\n");
					if (cur->onDisconnect)
						cur->onDisconnect(cur);
					if (ls)
					{
						ls->next = cur->next;
						free(cur); break;
					}
					else
					{
						g_usbDeviceList = cur->next;
						free(cur); break;
					}
				}
				ls = cur;
				cur = cur->next;
			}
		}
		else if (status & PORT_CONNECTION_CHANGE && (status & PORT_CONNECTION)) {
			//printf("USB device connected to EHCI!\n");
			ehci_port_clear_bit(reg, PORT_CONNECTION_CHANGE);
			probeEhciPort(hc, port);
		}
	}
}

void reset_hc(EhciController * hc)
{
	// Read the Command register
	uint cmd = ehci_read_register(hc, usbCmdO);
	// Write it back, setting bit 2 (the Reset bit) 
	//  and making sure the two schedule Enable bits are clear.
	ehci_write_register(hc, usbCmdO, 2 | cmd & ~(CMD_ASE | CMD_PSE | CMD_RS));
	// A small delay here would be good.  You don't want to read
	//  the register before it has a chance to actually set the bit
	ehci_read_register(hc, usbCmdO);
	// Now wait for the controller to clear the reset bit.
	// Note: A timeout would be a good idea too in case the bit
	//  never becomes clear.  (Bad controller: Bad address: etc.) (See note 1 below)
	while (ehci_read_register(hc, usbCmdO) & 2);
	// Again, a small delay here would be good to allow the
	//  reset to actually become complete.
	ehci_read_register(hc, usbCmdO);
	// wait for the halted bit to become set
	// again, see note 1 below)
	while (!(ehci_read_register(hc, usbStsO) & STS_HCHALTED));

}

template<typename T>
T align_address(T addr, int align)
{
	return (T)((((size_t)addr) / align) * align + align);
}

// ------------------------------------------------------------------------------------------------
void _ehci_init(uint id, PciDeviceInfo *info)
{
	// Check for class
	if (!(((info->classCode << 8) | info->subclass) == PCI_SERIAL_USB &&
		info->progIntf == PCI_SERIAL_USB_EHCI))
		return;
	
	if (sizeof(EhciQH) != 256)
	{
		log_debug(DEBUG_ERROR, "Unexpected EhciQH size: %d", sizeof(EhciQH));
		return;
	}

	log_debug(DEBUG_INFO, "Initializing EHCI");

	// Get base I/O Address
	PciBar bar;
	PciGetBar(&bar, id, 0);

	if (bar.flags & PCI_BAR_IO)
	{
		log_debug(DEBUG_WARNING, "IO. Working only with MMIO");
		// Only Memory Mapped I/O supported
		return;
	}

	// Controller initialization
	EhciController *hc = (EhciController *)malloc(sizeof(EhciController));

	// Set addresses
	hc->capRegs = (EhciCapRegs *)(uintptr_t)bar.u.address;
	hc->ehcibase2 = (uint)bar.u.address;
	hc->opRegs = (EhciOpRegs *)(uintptr_t)((size_t)bar.u.address + (size_t)hc->capRegs->capLength);
	hc->ehcibase = (uint)&hc->opRegs->usbCmd;

	//Allocate pools and framelist and make it 12-bit aligned
	int align = 1 << 12;

	hc->frameList = (u32 *)malloc(1024 * sizeof(u32) + align * 2);
	hc->frameList = (unsigned int *)((((size_t)hc->frameList) / align) * align + align);

	hc->qhPool = (EhciQH *)malloc(sizeof(EhciQH) * MAX_QH + align * 2);
	hc->tdPool = (EhciTD *)malloc(sizeof(EhciTD) * MAX_TD + align * 2);
	hc->qhPool = align_address(hc->qhPool, align);
	hc->tdPool = align_address(hc->tdPool, align);

	// Zero memory
	memset(hc->qhPool, 0, sizeof(EhciQH) * MAX_QH);
	memset(hc->tdPool, 0, sizeof(EhciTD) * MAX_TD);
	log_debug(DEBUG_OK, "Pools are allocated. Resetting HC.");

	// Now reset the hc
	reset_hc(hc);
	log_debug(DEBUG_OK, "EHCI host controller has resetted.");

	// Asynchronous queue setup
	EhciQH *qh = ehci_alloc_QH(hc);

	qh->qhlp = (size_t)qh | PTR_QH;
	qh->ch = QH_CH_H;
	qh->caps = 0;
	qh->curLink = 0;
	qh->nextLink = PTR_TERMINATE;
	qh->altLink = 0;
	qh->token = 0;

	for (uint i = 0; i < 5; ++i)
	{
		qh->buffer[i] = 0;
		qh->extBuffer[i] = 0;
	}

	qh->transfer = 0;
	qh->qhLink.prev = &qh->qhLink;
	qh->qhLink.next = &qh->qhLink;
	hc->asyncQH = qh;

	// Periodic list queue setup
	qh = ehci_alloc_QH(hc);
	qh->qhlp = PTR_TERMINATE;
	qh->ch = 0;
	qh->caps = 0;
	qh->curLink = 0;
	qh->nextLink = PTR_TERMINATE;
	qh->altLink = 0;
	qh->token = 0;
	for (uint i = 0; i < 5; ++i)
	{
		qh->buffer[i] = 0;
		qh->extBuffer[i] = 0;
	}
	qh->transfer = 0;
	qh->qhLink.prev = &qh->qhLink;
	qh->qhLink.next = &qh->qhLink;

	hc->periodicQH = qh;
	for (uint i = 0; i < 1024; ++i)
	{
		hc->frameList[i] = PTR_QH | (u32)(uintptr_t)qh;
	}
	// Check extended capabilities
	uint eecp = (RCR(hc, hccParamsO) & HCCPARAMS_EECP_MASK) >> HCCPARAMS_EECP_SHIFT;
	if (eecp >= 0x40)
	{
		// Disable BIOS legacy support
		uint legsup = PciRead32(id, eecp + USBLEGSUP);
		if (legsup & USBLEGSUP_HC_BIOS)
		{
			PciWrite32(id, eecp + USBLEGSUP, legsup | USBLEGSUP_HC_OS);
			for (;;)
			{
				legsup = PciRead32(id, eecp + USBLEGSUP);
				if (~legsup & USBLEGSUP_HC_BIOS && legsup & USBLEGSUP_HC_OS)
				{
					break;
				}
			}
		}
	}

	// Disable interrupts
	ehci_write_register(hc, usbIntrO, 0);

	// Setup frame list
	ehci_write_register(hc, frameIndexO, 0);
	ehci_write_register(hc, periodicListBaseO, (u32)(uintptr_t)hc->frameList);
	ehci_write_register(hc, asyncListAddrO, (size_t)hc->asyncQH);
	ehci_write_register(hc, ctrlDsSegmentO, 0);
	
	
	// Clear status
	ehci_write_register(hc, usbStsO, 0x3F);

	// Enable controller
	ehci_write_register(hc, usbCmdO, (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS);

	// And wait for halt
	while (ehci_read_register(hc, usbStsO) & STS_HCHALTED);

	// Configure all devices to be managed by the EHCI:
	// if PC has another companion controller, make EHCI
	// is 'manager' for EHCI devices. For an example:
	// we have EHCI and xHCI, firstly, we need to configure EHCI,
	// and, if can, manage devices by xHCI
	// bla-bla-bla
	ehci_write_register(hc, configFlagO, 1);

	log_debug(DEBUG_OK, "EHCI setup done. Probing ports.");

	// Probe devices
	ehci_probe_ports(hc);

	// Register controller
	// TODO: make it simplier
	UsbController *controller = (UsbController *) malloc(sizeof(UsbController));

	controller->next = g_usbControllerList;
	controller->hc = hc;

	controller->poll = &ehci_controller_poll;
	g_usbControllerList = controller;
}

#pragma GCC pop_options
