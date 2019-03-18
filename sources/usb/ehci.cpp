#include "ehci.h"
#include "usbd.h"
#include "../includes/string.h"
#include "../includes/interrupts.h"
#pragma GCC push_options
#pragma GCC optimize("O0")

#define uint unsigned int
bit32u MMIOREAD(const size_t base, const size_t offset) {
	return *((volatile bit32u *)(base + offset));
}

void MWIR(const size_t base, const size_t offset, const bit32u val) {
	volatile bit32u *ptr = (volatile bit32u *)(base + offset);
	*ptr = val;
}

void WRITEREG(EhciController * hc, uint reg, uint val)
{
	MWIR(hc->ehcibase, reg, val);
}

uint ROR(EhciController * hc, uint reg)
{
	return MMIOREAD(hc->ehcibase, reg);
}

void WCR(EhciController * hc, uint reg, uint val)
{
	MWIR(hc->ehcibase2, reg, val);
}
uint RCR(EhciController * hc, uint reg)
{
	return MMIOREAD(hc->ehcibase2, reg);
}

// ------------------------------------------------------------------------------------------------
static EhciTD *EhciAllocTD(EhciController *hc)
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

	printf("EhciAllocTD failed\n");
	return 0;
}

// ------------------------------------------------------------------------------------------------
static EhciQH *EhciAllocQH(EhciController *hc)
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

	printf("EhciAllocQH failed\n");
	return 0;
}

// ------------------------------------------------------------------------------------------------
static void EhciFreeTD(EhciTD *td)
{
	td->active = 0;
}

// ------------------------------------------------------------------------------------------------
static void EhciFreeQH(EhciQH *qh)
{
	qh->active = 0;
}
extern "C" {
	// ------------------------------------------------------------------------------------------------
	static void EhciInsertAsyncQH(EhciQH *list, EhciQH *qh)
	{
		EhciQH *end = LinkData(list->qhLink.prev, EhciQH, qhLink);

		qh->qhlp = (u32)(uintptr_t)list | PTR_QH;
		end->qhlp = (u32)(uintptr_t)qh | PTR_QH;

		LinkBefore(&list->qhLink, &qh->qhLink);
	}

	// ------------------------------------------------------------------------------------------------
	static void EhciInsertPeriodicQH(EhciQH *list, EhciQH *qh)
	{
		EhciQH *end = LinkData(list->qhLink.prev, EhciQH, qhLink);

		qh->qhlp = PTR_TERMINATE;
		end->qhlp = (u32)(uintptr_t)qh | PTR_QH;

		LinkBefore(&list->qhLink, &qh->qhLink);
	}

	// ------------------------------------------------------------------------------------------------
	static void EhciRemoveQH(EhciQH *qh)
	{
		EhciQH *prev = LinkData(qh->qhLink.prev, EhciQH, qhLink);

		prev->qhlp = qh->qhlp;
		LinkRemove(&qh->qhLink);
	}
}
// ------------------------------------------------------------------------------------------------
static void EhciPortSet(volatile u32 *portReg, u32 data)
{
	u32 status = *portReg;
	status |= data;
	//status &= ~PORT_RWC;
	*portReg = status;
}

// ------------------------------------------------------------------------------------------------
static void EhciPortClr(volatile u32 *portReg, u32 data)
{
	u32 status = *portReg;
	status &= ~data;
	*portReg = status;
}

// ------------------------------------------------------------------------------------------------
static uint EhciResetPort(EhciController *hc, uint port)
{
	volatile u32 *reg = &hc->opRegs->ports[port];
	// Reset the port
	*reg |= (1 << 12);
	Wait(102);
	EhciPortSet(reg, PORT_RESET);
	Wait(102);
	EhciPortClr(reg, PORT_RESET);

	// Wait 100ms for port to enable (TODO - what is appropriate length of time?)
	uint status = 0;
	int oo = 0;
	for (uint i = 0; i < 5; ++i)
	{
		// Delay
		Wait(31);

		// Get current status
		status = *reg;
		if (status & (1 << 10) && oo == 0)
		{
			*reg |= ((1 << 13));
			oo = 1;
			i = 0;
		}
		// Check if device is attached to port
		if (~status & PORT_CONNECTION)
			break;
		// Acknowledge change in status
		if (status & (PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE))
		{
			EhciPortClr(reg, PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE);
			continue;
		}

		// Check if device is enabled
		if (status & PORT_ENABLE)
			break;
	}
	return status;
}

// ------------------------------------------------------------------------------------------------
static void EhciInitTD(EhciTD *td, EhciTD *prev,
	uint toggle, uint packetType,
	uint len, const void *data)
{
	if (prev)
	{
		prev->link = (u32)(uintptr_t)td;
		prev->tdNext = (u32)(uintptr_t)td;
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
	uintptr_t p = (uintptr_t)data;
	td->buffer[0] = (u32)p;
	td->extBuffer[0] = 0;
	p &= ~0xfff;

	// Remaining pages of buffer memory.
	for (uint i = 1; i < 4; ++i)
	{
		p += 0x1000;
		td->buffer[i] = (u32)(p);
		td->extBuffer[i] = 0;
	}
}

// ------------------------------------------------------------------------------------------------
static void EhciInitQH(EhciQH *qh, UsbTransfer *t, EhciTD *td, UsbDevice *parent, bool interrupt, uint speed, uint addr, uint endp, uint maxSize)
{
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

	qh->tdHead = (u32)(uintptr_t)td;
	qh->nextLink = (u32)(uintptr_t)td;
	qh->token = 0;
}

// ------------------------------------------------------------------------------------------------
static void EhciProcessQH(EhciController *hc, EhciQH *qh)
{
	UsbTransfer *t = qh->transfer;
	char z = *(char*)(VIDEO_SEG);
	*(char*)(VIDEO_SEG) = z ^ 0;
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
				printf(" Data Buffer Error\n");
			}
			if (qh->token & TD_TOK_BABBLE)
			{
				printf(" Babble Detected\n");
			}
			if (qh->token & TD_TOK_XACT)
			{
				printf(" Transaction Error\n");
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
		EhciRemoveQH(qh);

		// Free transfer descriptors
		EhciTD *td = (EhciTD *)(uintptr_t)qh->tdHead;
		while (td)
		{
			EhciTD *next = (EhciTD *)(uintptr_t)td->tdNext;
			EhciFreeTD(td);
			td = next;
		}

		// Free queue head
		EhciFreeQH(qh);
	}
}


// ------------------------------------------------------------------------------------------------
static void EhciWaitForQH(EhciController *hc, EhciQH *qh)
{

	UsbTransfer *t = qh->transfer;
	while ((!t->complete))
	{
		EhciProcessQH(hc, qh);
	}

}
// ------------------------------------------------------------------------------------------------
static void EhciDevControl(UsbDevice *dev, UsbTransfer *t)
{
	EhciController *hc = (EhciController *)dev->hc;
	UsbDevReq *req = t->req;

	// Determine transfer properties
	uint speed = dev->speed;
	uint addr = dev->addr;
	uint maxSize = dev->maxPacketSize;
	uint type = req->type;
	uint len = req->len;

	// Create queue of transfer descriptors
	EhciTD *td = EhciAllocTD(hc);
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
	EhciInitTD(td, prev, toggle, packetType, packetSize, req);
	prev = td;

	// Data in/out packets
	packetType = type & RT_DEV_TO_HOST ? USB_PACKET_IN : USB_PACKET_OUT;

	u8 *it = (u8 *)t->data;
	u8 *end = it + len;
	while (it < end)
	{
		td = EhciAllocTD(hc);
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

		EhciInitTD(td, prev, toggle, packetType, packetSize, it);

		it += packetSize;
		prev = td;
	}

	// Status packet
	td = EhciAllocTD(hc);
	if (!td)
	{
		return;
	}

	toggle = 1;
	packetType = type & RT_DEV_TO_HOST ? USB_PACKET_OUT : USB_PACKET_IN;
	EhciInitTD(td, prev, toggle, packetType, 0, 0);

	// Initialize queue head
	EhciQH *qh = EhciAllocQH(hc);
	EhciInitQH(qh, t, head, dev->parent, false, speed, addr, 0, maxSize);

	// Wait until queue has been processed
	EhciInsertAsyncQH(hc->asyncQH, qh);
	EhciWaitForQH(hc, qh);
}

// ------------------------------------------------------------------------------------------------
static void EhciDevIntr(UsbDevice *dev, UsbTransfer *t)
{
	EhciController *hc = (EhciController *)dev->hc;
	// Determine transfer properties
	uint speed = dev->speed;
	uint addr = dev->addr;
	uint maxSize = t->endp->desc->maxPacketSize;
	uint endp = t->endp->desc->addr & 0xf;
	// Create queue of transfer descriptors
	EhciTD *td = EhciAllocTD(hc);
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
	uint packetType = USB_PACKET_IN;
	if (t->endp->desc->addr & 0x80)
		packetType = USB_PACKET_IN;
	else
		packetType = USB_PACKET_OUT;


	uint packetSize = t->len;

	EhciInitTD(td, prev, toggle, packetType, packetSize, t->data);
	// Initialize queue head
	EhciQH *qh = EhciAllocQH(hc);
	EhciInitQH(qh, t, head, dev->parent, true, speed, addr, endp, maxSize);
	// Schedule queue
	if (t->w)
		EhciInsertAsyncQH(hc->asyncQH, qh);
	else
		EhciInsertPeriodicQH(hc->periodicQH, qh);
	if (t->w)
		EhciWaitForQH(hc, qh);
}

void probeEhciPort(EhciController *hc, uint port)
{
	uint status = EhciResetPort(hc, port);
	if (status & PORT_ENABLE)
	{
		uint speed = USB_HIGH_SPEED;
		printf("Detected device on port #%x\n", port);
		Wait(102);
		UsbDevice *dev = UsbDevCreate();
		if (dev)
		{
			dev->parent = 0;
			dev->hc = hc;
			dev->port = port;
			dev->speed = speed;
			dev->maxPacketSize = 8;


			dev->hcControl = EhciDevControl;
			dev->hcIntr = EhciDevIntr;

			if (!UsbDevInit(dev))
			{
				// TODO - cleanup
			}
		}
	}
}
// ------------------------------------------------------------------------------------------------
static void EhciProbe(EhciController *hc)
{
	// Port setup
	uint portCount = RCR(hc, hcsParamsO) & HCSPARAMS_N_PORTS_MASK;
	for (uint port = 0; port < portCount; ++port)
	{
		probeEhciPort(hc, port);
	}
}

extern "C" {
	static void EhciControllerPollList(EhciController *hc, Link *list)
	{
		EhciQH *qh;
		EhciQH *next;/*
		//lockTaskSwitch(1);
		ListForEachSafe(qh, next, *list, qhLink)
		{
			if (qh->transfer && (qh->transfer->w == 0) && (!qh->transfer->complete))
			{
				EhciProcessQH(hc, qh);
			}
		}*/
		//unlockTaskSwitch();
	}

}
// ------------------------------------------------------------------------------------------------
static void EhciControllerPoll(UsbController *controller)
{
	EhciController *hc = (EhciController *)controller->hc;
	EhciControllerPollList(hc, &hc->asyncQH->qhLink);
	EhciControllerPollList(hc, &hc->periodicQH->qhLink);
	uint portCount = RCR(hc, hcsParamsO) & HCSPARAMS_N_PORTS_MASK;
	for (uint port = 0; port < portCount; ++port)
	{
		volatile u32 *reg = &hc->opRegs->ports[port];
		uint status = *reg;
		if (status & 2 && !(status & 1))
		{
			//Device disconnected
			printf("USB device has disconnected from EHCI!\n");
			UsbDevice * cur = g_usbDeviceList, *ls = 0;
			//Clear Connection change bit
			EhciPortClr(reg, PORT_CONNECTION_CHANGE);
			while (cur)
			{
				if (cur->port == port && (uint)cur->hc == (uint)hc)
				{
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
		else if (status & 2 && (status & 1)) {
			printf("USB device connected to EHCI!\n");
			EhciPortClr(reg, PORT_CONNECTION_CHANGE);
			probeEhciPort(hc, port);
		}
	}
}
// ------------------------------------------------------------------------------------------------
void _ehci_init(uint id, PciDeviceInfo *info)
{
	if (!(((info->classCode << 8) | info->subclass) == PCI_SERIAL_USB &&
		info->progIntf == PCI_SERIAL_USB_EHCI))
	{
		return;
	}
	if (sizeof(EhciQH) != 128)
	{
		printf("Unexpected EhciQH size: %d\n", sizeof(EhciQH));
		return;
	}
	printf("Initializing EHCI\n");
	// Base I/O Address
	PciBar bar;
	PciGetBar(&bar, id, 0);
	if (bar.flags & PCI_BAR_IO)
	{
		// Only Memory Mapped I/O supported
		return;
	}
	// Controller initialization
	EhciController *hc = (EhciController *)malloc(sizeof(EhciController));
	hc->capRegs = (EhciCapRegs *)(uintptr_t)bar.u.address;
	hc->ehcibase2 = (uint)bar.u.address;
	hc->opRegs = (EhciOpRegs *)(uintptr_t)((size_t)bar.u.address + (size_t)hc->capRegs->capLength);
	hc->ehcibase = (uint)&hc->opRegs->usbCmd;
	// Read the Command register
	uint cmd = ROR(hc, usbCmdO);
	// Write it back, setting bit 2 (the Reset bit) 
	//  and making sure the two schedule Enable bits are clear.
	WRITEREG(hc, usbCmdO, 2 | cmd & ~(CMD_ASE | CMD_PSE));
	// A small delay here would be good.  You don't want to read
	//  the register before it has a chance to actually set the bit
	ROR(hc, usbCmdO);
	// Now wait for the controller to clear the reset bit.
	// Note: A timeout would be a good idea too in case the bit
	//  never becomes clear.  (Bad controller: Bad address: etc.) (See note 1 below)
	while (ROR(hc, usbCmdO) & 2);
	// Again, a small delay here would be good to allow the
	//  reset to actually become complete.
	ROR(hc, usbCmdO);
	// wait for the halted bit to become set
	// again, see note 1 below)
	while (!(ROR(hc, usbStsO) & STS_HCHALTED));
	hc->frameList = (u32 *)malloc(1024 * sizeof(u32) + 8192 * 4);
	hc->frameList = (unsigned int *)((((size_t)hc->frameList) / 16384) * 16384 + 16384);
	hc->qhPool = (EhciQH *)malloc(sizeof(EhciQH) * MAX_QH + 8192 * 4);
	hc->tdPool = (EhciTD *)malloc(sizeof(EhciTD) * MAX_TD + 8192 * 4);
	hc->qhPool = (EhciQH *)((((size_t)hc->qhPool) / 16384) * 16384 + 16384);
	hc->tdPool = (EhciTD *)((((size_t)hc->tdPool) / 16384) * 16384 + 16384);
	// Asynchronous queue setup
	EhciQH *qh = EhciAllocQH(hc);
	qh->qhlp = (u32)(uintptr_t)qh | PTR_QH;
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
	qh = EhciAllocQH(hc);
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
	MWIR(hc->ehcibase, usbIntrO, 0);
	// Setup frame list
	WRITEREG(hc, frameIndexO, 0);
	WRITEREG(hc, periodicListBaseO, (u32)(uintptr_t)hc->frameList);
	WRITEREG(hc, asyncListAddrO, (u32)(uintptr_t)hc->asyncQH);
	WRITEREG(hc, ctrlDsSegmentO, 0);
	Wait(100);
	//Clear status
	WRITEREG(hc, usbStsO, ~0x3F);
	// Enable controller
	WRITEREG(hc, usbCmdO, (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS);
	while (ROR(hc, usbStsO)&STS_HCHALTED);

	// Configure all devices to be managed by the EHCI
	WRITEREG(hc, configFlagO, 1);
	printf("Device configured. Probing ports...\n");
	// Probe devices
	EhciProbe(hc);

	// Register controller
	UsbController *controller = (UsbController *)malloc(sizeof(UsbController));
	controller->next = g_usbControllerList;
	controller->hc = hc;
	//controller->poll = &epoll;
	controller->poll = &EhciControllerPoll;
	g_usbControllerList = controller;
}

#pragma GCC pop_options
