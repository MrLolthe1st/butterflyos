#include "ehci.h"
#include "usbd.h"
#include "../includes/string.h"
#include "../includes/interrupts.h"
//Turn off optimizations: MMIO
#pragma GCC push_options
#pragma GCC optimize("O0")
#define uint unsigned int

// ------------------------------------------------------------------------------------------------
bit32u MMIOREAD(const size_t base, const size_t offset) {
	return *((volatile bit32u *)(base + offset));
}

// ------------------------------------------------------------------------------------------------
void MWIR(const size_t base, const size_t offset, const bit32u val) {
	volatile bit32u *ptr = (volatile bit32u *)(base + offset);
	*ptr = val;
}

// ------------------------------------------------------------------------------------------------
void WRITEREG(EhciController * hc, uint reg, uint val)
{
	MWIR(hc->ehcibase, reg, val);
}

// ------------------------------------------------------------------------------------------------
uint ROR(EhciController * hc, uint reg)
{
	return MMIOREAD(hc->ehcibase, reg);
}

// ------------------------------------------------------------------------------------------------
void WCR(EhciController * hc, uint reg, uint val)
{
	MWIR(hc->ehcibase2, reg, val);
}

// ------------------------------------------------------------------------------------------------
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

		qh->qhlp = (size_t)list | PTR_QH;
		end->qhlp = (size_t)qh | PTR_QH;

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
	EhciPortSet(reg, PORT_POWER);
	Wait(10);
	EhciPortSet(reg, PORT_RESET);
	Wait(103);
	EhciPortClr(reg, PORT_RESET);
	unsigned int status = 0;
	char powner_red = 0;
	Wait(10);
	for (uint i = 0; i < 5; ++i)
	{
		// Get current status
		status = *reg;
		if (status & (3 << 10) && !powner_red)
		{
			*reg |= ((1 << 13));
			powner_red = 1;
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
		// Delay
		Wait(21);
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
	//MWIR((size_t)&qh->tdHead, 0, (unsigned int)td | PTR_ITD);
	//MWIR((size_t)&qh->nextLink, 0, (unsigned int)td);
	qh->tdHead = (size_t)td;
	//qh->altLink = (size_t)td;
	//qh->curLink = (size_t)td;
	qh->nextLink = (size_t)td;
	qh->token = 0;
}


void EHCIPRINTTD(EhciTD * td)
{
	while (td)
	{
		printf("[%x:%x]", td, td->token);
		EhciTD *next = (EhciTD *)(uintptr_t)td->tdNext;
		td = next;
	}
	printf("\n");
}
// ------------------------------------------------------------------------------------------------
static void EhciProcessQH(EhciController *hc, EhciQH *qh)
{
	UsbTransfer *t = qh->transfer;
	char z = *(char*)(VIDEO_SEG);
	*(char*)(VIDEO_SEG) = z ^ 0;
	//WRITEREG(hc, usbStsO, 0);
//	Wait(200);
	//printf("[%x,%x,%x]", hc->opRegs->usbSts, hc->opRegs->asyncListAddr, qh->nextLink);
	//Wait(90);

	//t->success = true;
	//t->complete = true;
	//EHCIPRINTTD((EhciTD*)qh->tdHead);
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
	long long start_time = time1024;
	UsbTransfer *t = qh->transfer;
	while (!t->complete && time1024 - start_time < 510)
	{
		EhciProcessQH(hc, qh);
	}
	if (time1024 - start_time >= 512)
	{
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
	EHCIPRINTTD((EhciTD*)qh->tdHead);
	//	printf(" nextLink=0x%08x\n", qh->nextLink);
	//	printf(" altLink=0x%08x\n", qh->altLink);
		//printf(" token=0x%08x\n", qh->token);
		//printf(" buffer=0x%08x\n", qh->buffer[0]);
	EhciPrintQH((EhciQH*)qh->qhlp, strt, c + 1);
}
// ------------------------------------------------------------------------------------------------
static void EhciDevControl(UsbDevice *dev, UsbTransfer *t)
{
	EhciController *hc = (EhciController *)dev->hc;
	UsbDevReq *req = t->req;

	// Determine transfer properties
	uint speed = dev->speed;
	uint addr = dev->addr;
	//printf("~@@@%d@", addr);
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
	//printf("*******%d**", len);
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
zww:
	// Initialize queue head
	EhciQH *qh = EhciAllocQH(hc);
	EhciInitQH(qh, t, head, dev->parent, false, speed, addr, 0, maxSize);
	//unsigned int cmd = ROR(hc, usbCmdO);
	//WRITEREG(hc, usbCmdO, cmd | 0x40);
	// Wait until queue has been processed
	EhciInsertAsyncQH(hc->asyncQH, qh);
	//hc->asyncQH->nextLink = qh->nextLink;
//	EhciPrintQH(hc->asyncQH, hc->asyncQH);

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
		Wait(1);
		printf("USB: ");
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
				printf("Stall.\n");
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

			UsbDevice * cur = g_usbDeviceList, *ls = 0;
			//Clear Connection change bit
			EhciPortClr(reg, PORT_CONNECTION_CHANGE);
			while (cur)
			{
				if (cur->port == port && (uint)cur->hc == (uint)hc)
				{
					printf("USB device has disconnected from EHCI!\n");
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

void reset_hc(EhciController * hc)
{
	// Read the Command register
	uint cmd = ROR(hc, usbCmdO);
	// Write it back, setting bit 2 (the Reset bit) 
	//  and making sure the two schedule Enable bits are clear.
	WRITEREG(hc, usbCmdO, 2 | cmd & ~(CMD_ASE | CMD_PSE | CMD_RS));
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
	Wait(11);

}

// ------------------------------------------------------------------------------------------------
void _ehci_init(uint id, PciDeviceInfo *info)
{
	if (!(((info->classCode << 8) | info->subclass) == PCI_SERIAL_USB &&
		info->progIntf == PCI_SERIAL_USB_EHCI))
	{
		return;
	}
	/*if (sizeof(EhciQH) != 128)
	{
		printf("Unexpected EhciQH size: %d\n", sizeof(EhciQH));
		return;
	}*/
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
	//WRITEREG(hc, usbCmdO, 0);
	int align = 4096;
	hc->frameList = (u32 *)malloc(1024 * sizeof(u32) + align * 2);
	hc->frameList = (unsigned int *)((((size_t)hc->frameList) / align) * align + align);
	hc->qhPool = (EhciQH *)malloc(sizeof(EhciQH) * MAX_QH + align * 2);
	hc->tdPool = (EhciTD *)malloc(sizeof(EhciTD) * MAX_TD + align * 2);
	hc->qhPool = (EhciQH *)((((size_t)hc->qhPool) / align) * align + align);
	hc->tdPool = (EhciTD *)((((size_t)hc->tdPool) / align) * align + align);
	memset(hc->qhPool, 0, sizeof(EhciQH) * MAX_QH);
	memset(hc->tdPool, 0, sizeof(EhciTD) * MAX_TD);
	reset_hc(hc);
	// Asynchronous queue setup
	EhciQH *qh = EhciAllocQH(hc);
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

	//reset_hc(hc);
	// Disable interrupts
	MWIR(hc->ehcibase, usbIntrO, 0x3f);
	// Setup frame list
	WRITEREG(hc, frameIndexO, 0);
	WRITEREG(hc, periodicListBaseO, (u32)(uintptr_t)hc->frameList);
	WRITEREG(hc, asyncListAddrO, (size_t)hc->asyncQH);
	WRITEREG(hc, ctrlDsSegmentO, 0);
	Wait(10);
	//Clear status
	WRITEREG(hc, usbStsO, 0x1020);
	// Enable controller
	WRITEREG(hc, usbCmdO, CMD_RS);
	WRITEREG(hc, usbCmdO, (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS);
	while (ROR(hc, usbStsO) & STS_HCHALTED);
	//unsigned int sts = ROR(hc, usbStsO);
	//WRITEREG(hc, usbStsO, sts | 0x20);
	// Configure all devices to be managed by the EHCI
	WRITEREG(hc, configFlagO, 0);
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
