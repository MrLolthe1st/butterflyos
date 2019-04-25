#include "../includes/storage/msd.h"
#include "../includes/interrupts.h"

const int cbw_sig = 0x43425355;
//#define DEBUG 1
template<typename T>
void printMem(T aa, size_t c)
{
	unsigned char * a = (unsigned char *)aa;
	for (int i = 0; i < c; i++)
	{
		if (a[i] < 0x10)
			printf("0%x", (int)a[i]);
		else
			printf("%x", (int)a[i]);
		if ((i % 24) != 23)
			printf("-");
		else if (i != c - 1)
			printf("\n");
	}

}

unsigned int msd_reset(UsbDevice * dev, UsbEndpoint * out)
{
	//MassStorage Reset command
	if (!UsbDevRequest(dev, RT_HOST_TO_DEV | RT_CLASS | RT_INTF, 0xff, 0, dev->intfDesc->intfIndex, 0, 0)) {
		printf("Can't reset mass storage, exiting....\n");
		return 1;
	}
	//Clear halt for out endpoint
	if (!UsbDevRequest(dev, 0x2, 0x01, 0, out->desc->addr & 0xf, 0, 0)) {
		printf("Can't clear halt for endpoint %x, exiting...\n", out->desc->addr & 0xf);
		return 1;
	}
	return 0;
}


bool clear_endpoint_halt(usb_mass_storage * s)
{
	if (!UsbDevRequest(s->d, 0x2, 0x01, 0, s->endpointOut->desc->addr & 0xf, 0, 0)) {
		printf("Can't clear halt for endpoint %x, exiting...\n", s->endpointOut->desc->addr & 0xf);
		return 0;
	}
	return 1;
}

void prepare_transfer(UsbTransfer * t, UsbEndpoint * e, int len, void * data)
{
	t->w = 1;
	t->complete = false;
	t->success = false;
	t->endp = e;
	t->len = len;
	t->data = data;
	t->req = 0;
}

void usb_mass_storage::start_storage()
{
	usb_mass_storage * s = this;
	UsbDevice * dev = this->d;

	// Prepare CBW
	memset(cbw, 0, 31);
	cbw->lun = 0; cbw->tag = s->tag;
	s->tag++; cbw->sig = cbw_sig;
	cbw->wcb_len = 10;	cbw->flags = 0x80;
	cbw->xfer_len = 0;

	// and command, also
	*((u8*)((size_t)cbw + 15)) = 0x1B;
	*((u8*)((size_t)cbw + 18)) = 0;
	*((u8*)((size_t)cbw + 19)) = 1 | (1 << 4);

	prepare_transfer(t, endpointOut, 0x1F, cbw);
	dev->hcIntr(dev, t);

	if (!t->success) { clear_endpoint_halt(s);  return; }

	prepare_transfer(t, endpointIn, 13, cbw);
	dev->hcIntr(dev, t);
	if (!t->success) { clear_endpoint_halt(s);  return; }

}

typedef struct PACKED {
	u8 op;
	u8 res;
	u8 pgoropcode;
	u8 res2;
	u16 alloc;
	u8 ctrl;
} inquiry_cmd;

typedef struct __attribute__((__packed__)) __cmd_rdcap_10_t {
	u8 op;  // 0x25
	u8 reserved;
	unsigned int lba;
	u16 reserved2;
	u8 pmi;
	u8 control;
} cmd_rdcap_10_t;



long long usb_mass_storage::read_capacity()
{
	usb_mass_storage * s = this;
	UsbDevice * dev = s->d;

	memset(cbw, 0, 31);	cbw->lun = 0;
	cbw->sig = cbw_sig; cbw->wcb_len = 10;
	cbw->tag = s->tag;	s->tag++;
	cbw->flags = 0x80;	cbw->xfer_len = 8;
	cmd_rdcap_10_t * cmd = (cmd_rdcap_10_t *)((size_t)cbw + 15);	cmd->op = 0x25;

	prepare_transfer(t, endpointOut, 0x1F, cbw);

#ifdef DEBUG
	printf("ReadCapacity CBW\n");
	printMem(cbw, 13);
	printf("\n");
#endif // DEBUG

	dev->hcIntr(dev, t);

	if (!t->success) { clear_endpoint_halt(s);  return 0; }


	unsigned char res[8] = {};

	prepare_transfer(t, endpointIn, 0x08, &res);
	dev->hcIntr(dev, t);

	if (!t->success) { clear_endpoint_halt(s);  return 0; }

#ifdef DEBUG
	printf("ReadCapacity DATA\n");
	printMem(&res, 8);
	printf("\n");
#endif // DEBUG

	prepare_transfer(t, endpointIn, 13, cbw);
	dev->hcIntr(dev, t);
	if (!t->success) { clear_endpoint_halt(s);  return 0; }

#ifdef DEBUG
	printf("ReadCapacity CSW\n");
	printMem(cbw, 13);
	printf("\n");
#endif // DEBUG

	s->bytesPerBlock = (res[4] << 24) + (res[5] << 16) + (res[6] << 8) + res[7];
	return (res[0] << 24) + (res[1] << 16) + (res[2] << 8) + res[3];
}

bool usb_mass_storage::request_sense()
{
	usb_mass_storage * s = this;
	UsbDevice * dev = s->d;

	memset(cbw, 0, 31);		cbw->lun = 0;
	cbw->tag = s->tag;		s->tag++;
	cbw->sig = cbw_sig;		cbw->wcb_len = 10;
	cbw->flags = 0x80;		cbw->xfer_len = 0x12;

	*((u8*)((size_t)cbw + 15)) = 0x03;
	*((u8*)((size_t)cbw + 16)) = 0x01;
	*((u8*)((size_t)cbw + 19)) = 0x12;

	prepare_transfer(t, endpointOut, 0x1F, cbw);

#ifdef DEBUG
	printf("RequsetSense CBW\n");
	printMem(cbw, 13);
	printf("\n");
#endif // DEBUG

	dev->hcIntr(dev, t);
	if (!t->success) { clear_endpoint_halt(s);  return 0; }

	char z[32];

	prepare_transfer(t, endpointIn, 0x12, &z);
	dev->hcIntr(dev, t);
	if (!t->success) { clear_endpoint_halt(s);  return 0; }

#ifdef DEBUG
	printf("RequsetSense Data\n");
	printMem(&z, 0x12);
	printf("\n");
#endif // DEBUG


	prepare_transfer(t, endpointIn, 13, cbw);

	if (!t->success) { clear_endpoint_halt(s);  return 0; }
#ifdef DEBUG
	printf("RequestSense CSW\n");
	printMem(cbw, 13);
	printf("\n");
#endif // DEBUG
	return 1;
}

bool usb_mass_storage::test_unit_ready()
{
	usb_mass_storage * s = this;
	UsbDevice * dev = s->d;
	cbw->lun = 0;		cbw->tag = s->tag;
	s->tag++;			cbw->sig = cbw_sig;
	cbw->wcb_len = 10;	cbw->flags = 0x00;
	cbw->xfer_len = 0;

	prepare_transfer(t, endpointOut, 0x1F, cbw);

#ifdef DEBUG
	printf("TUR CBW\n");
	printMem(cbw, 13);
	printf("\n");
#endif // DEBUG

	dev->hcIntr(dev, t);
	if (!t->success) { clear_endpoint_halt(s);  return 0; }

	t->endp = endpointIn;
	t->req = 0;
	t->data = cbw;
	t->len = 13;
	t->complete = false;
	t->success = false;
	t->w = 1;
	dev->hcIntr(dev, t);
#ifdef DEBUG
	printf("TUR CSW\n");
	printMem(cbw, 13);
	printf("\n");
#endif // DEBUG
	//free(cbw);
	return *((u8*)((size_t)cbw + 12));

}

bool usb_mass_storage::inquiry_request()
{
	usb_mass_storage * s = this;
	if (!clear_endpoint_halt(s)) return false;
	UsbDevice * dev = s->d;
	UsbTransfer *t = s->t;
	UsbEndpoint * endpointIn = s->endpointIn, *endpointOut = s->endpointOut;
	cbw_t * cbw = this->cbw; // (cbw_t*)malloc(33);
	memset(cbw, 0, 31);
	cbw->lun = 0;
	cbw->tag = s->tag;
	s->tag++;
	cbw->sig = cbw_sig;
	cbw->wcb_len = 10;
	cbw->flags = 0x80;
	cbw->xfer_len = 0x24;
	inquiry_cmd * cmd = (inquiry_cmd *)((size_t)cbw + 15);
	cmd->op = 0x12;
	cmd->alloc = 0x24;
	t->endp = endpointOut;
	t->req = 0;
	t->data = cbw;
	t->len = 0x1F;
	t->complete = false;
	t->success = false;
	t->w = 1;
#ifdef DEBUG
	printf("Inquiry CBW\n");
	printMem(cbw, 31); printf("\n");
	printf("\n");
#endif // DEBUG
	dev->hcIntr(dev, t);
	t->endp = endpointIn;
	t->req = 0;
	t->data = cbw;
	t->len = 0x24;
	t->complete = false;
	t->success = false;
	t->w = 1;
	dev->hcIntr(dev, t);
	t->endp = endpointIn;
	t->req = 0;
	t->data = cbw;
	t->len = 13;
	t->complete = false;
	t->success = false;
	t->w = 1;
	dev->hcIntr(dev, t);
#ifdef DEBUG
	printf("Inquiry CSW\n");
	printMem(cbw, 13); printf("\n");
	printf("\n");
#endif // DEBUG
	//free(cbw);
	return false;

}


#define bswap_32_m(n) \
    (((n) >> 24) | \
    (((n) >> 8) & 0xFF00U) | \
    (((n) << 8) & 0xFF0000U) | \
    (((n) << 24) & 0xFF000000U))
typedef struct PACKED _cmd_rw10_t {
	uint8_t op;     // read=0x28
	uint8_t flags;  // bits: 1=fua_nv, 3=fua, 4=dpo, 7:5=rdprotect
	uint32_t lba;
	uint8_t group;
	uint16_t len;
	uint8_t control;
} cmd_rw10_t;

void storageDisconnect(UsbDevice *d)
{
	usb_mass_storage * s = (usb_mass_storage *)d->drv;
	free(s->endpointIn);
	free(s->t);
	free(s->endpointOut);
	free(s);
	printf("Storage successfully disconected!\n");
}

bool usb_mass_storage::write(long long lba, short count, void * buf)
{
	usb_mass_storage * s = this;
	//Get device and endpoints
	UsbDevice * dev = s->d;
	UsbTransfer *t = s->t;
	UsbEndpoint * endpointIn = s->endpointIn, *endpointOut = s->endpointOut;
	cbw_t * cbw = s->cbw;
	memset(cbw, 0, 31);
	cbw->tag = s->tag;
	s->tag++;
	cbw->sig = cbw_sig;
	cbw->wcb_len = 10;
	cbw->flags = 0x00;
	//Transfer length
	cbw->xfer_len = count * s->bytesPerBlock;
	cmd_rw10_t * cmd = (cmd_rw10_t*)((size_t)cbw + 15);
	cmd->op = 0x2A;
	*((unsigned int*)((size_t)cbw + 15 + 2)) = bswap_32_m(lba);//LBA for CBW is big-endian
	*((u16*)((size_t)cbw + 15 + 7)) = ((count & 0xFF) << 8);//Count of sectors also
	*((u8*)((size_t)cbw + 15 + 1)) = 0;//Count of sectors also
	t->endp = endpointOut;//bulk Out
	t->req = 0;
	t->data = cbw;
	t->len = 0x1F;
	t->complete = false;
	t->success = false;
	t->w = 1;
	dev->hcIntr(dev, t);
	//	printf("{*%x}", lba);
	for (int i = 0; i < count*(s->bytesPerBlock / endpointOut->desc->maxPacketSize); i++)
	{
		//Read by one sector
		t->endp = endpointOut;
		t->req = 0;
		t->data = buf;
		t->len = endpointOut->desc->maxPacketSize;
		t->complete = false;
		t->success = false;
		dev->hcIntr(dev, t);
		buf = (void*)((size_t)buf + endpointOut->desc->maxPacketSize);
	}
	t->endp = endpointIn;
	t->req = 0;
	t->data = cbw;
	t->len = 13;
	t->complete = false;
	t->success = false;
	//Read CSW
	dev->hcIntr(dev, t);
	//Invalid signature - soft reset
	if (cbw->sig != 0x53425355) {
		return 0;
	}
	return 1;

}
void poll()
{

}


bool usb_mass_storage::read(long long lba, short count, void * buf)
{
	usb_mass_storage * s = this;
	//clear_endpoint_halt(s);
	//Get device and endpoints
	UsbDevice * dev = s->d; char *ut = (char*)buf;
	UsbTransfer *t = s->t;
	UsbEndpoint * endpointIn = s->endpointIn, *endpointOut = s->endpointOut;
	//Allocate Control Block Wrapper
	cbw_t * cbw = s->cbw;
retry_read:;
	memset(cbw, 0, 32);
	cbw->tag = s->tag;
	s->tag++;
	cbw->sig = cbw_sig;
	cbw->wcb_len = 10;
	cbw->flags = 0x80;
	//Transfer length
	cbw->xfer_len = s->bytesPerBlock * count;
	cmd_rw10_t * cmd = (cmd_rw10_t*)((size_t)cbw + 15);
	cmd->op = 0x28;
	*((unsigned int*)((size_t)cbw + 15 + 2)) = bswap_32_m(lba);//LBA for CBW is big-endian
	*((u16*)((size_t)cbw + 15 + 7)) = ((count & 0xFF) << 8);//Count of sectors also
	*((u8*)((size_t)cbw + 15 + 1)) = 0;//Count of sectors also
	t->endp = endpointOut;//bulk Out
	t->req = 0;
	t->data = cbw;
	t->len = 0x1F;
	t->complete = false;
	t->success = false;
	t->w = 1;// printf("{^%x}", lba);
/*#ifdef DEBUG
	printf("Read CBW\n");
	printMem(cbw, 13);
	printf("\n");
#endif // DEBUG*/
	dev->hcIntr(dev, t);

	for (int i = 0; i < count*(s->bytesPerBlock / endpointIn->desc->maxPacketSize); i++)
	{
		//Read by one sector
		t->endp = endpointIn;
		t->req = 0;
		t->data = buf;
		t->len = endpointIn->desc->maxPacketSize;
		t->complete = false;
		t->success = false;
		dev->hcIntr(dev, t);/*
#ifdef DEBUG
		printf("Read Data\n");
		printMem(buf, 13);
		printf("\n");
#endif // DEBUG*/
		buf = (void*)((size_t)buf + endpointOut->desc->maxPacketSize);
	}
	t->endp = endpointIn;
	t->req = 0;
	t->data = cbw;
	t->len = 13;
	t->complete = false;
	t->success = false;
	//Read CSW
	dev->hcIntr(dev, t);/*
#ifdef DEBUG
	printf("Read CSW\n");
	printMem(cbw, 13);
	printf("\n");
#endif // DEBUG*/
//Invalid signature - soft reset
	if (cbw->sig != 0x53425355) {
		return 0;
	}
	return 1;

}

bool usb_storage_init(UsbDevice * dev)
{
	// Check out for endpoints for in/out

	UsbEndpoint * endpointIn, *endpointOut;
	endpointIn = (UsbEndpoint*)malloc(sizeof(UsbEndpoint));
	memset(endpointIn, 0, sizeof(UsbEndpoint));
	endpointIn->toggle = 0;
	// Initial toggle state is zero(In spec no information about(i'm haven't found),
	// but it works and do not touch it!

	if (dev->intfDesc->endpoints->addr & 0x80)
		endpointIn->desc = dev->intfDesc->endpoints;
	else
		endpointIn->desc = dev->intfDesc->endpoints->next;


	endpointOut = (UsbEndpoint*)malloc(sizeof(UsbEndpoint));
	endpointOut->toggle = 0;
	// Initial toggle state is zero(In spec no information about(i'm haven't found),
	// but it works and do not touch it!

	if (dev->intfDesc->endpoints->addr & 0x80)
		endpointOut->desc = dev->intfDesc->endpoints->next;
	else
		endpointOut->desc = dev->intfDesc->endpoints;
	//Reset MassStorage
	if (msd_reset(dev, endpointOut))
		return false; // Haven't reseted, get out from here!

	u8 lunCnt = 0;
	if (!UsbDevRequest(dev, 0b10100001, 0xfe, 0, dev->intfDesc->intfIndex, 1, &lunCnt)) {
		printf("Can't get LUN count!\n"); return false;
	}
	dev->drvPoll = (void(*)(UsbDevice*)) &poll;

	// Preapare transfer
	UsbTransfer *t = (UsbTransfer*)malloc(sizeof(UsbTransfer));
	// Allocate memory for storage structures
	usb_mass_storage * storage = (usb_mass_storage*)malloc(sizeof(usb_mass_storage));
	dev->drv = storage;
	dev->onDisconnect = &storageDisconnect;
	t->w = 1;
	storage->t = t;
	storage->tag = 0x2000;
	storage->d = dev;
	storage->endpointIn = endpointIn;
	storage->endpointOut = endpointOut;
	storage->cbw = (cbw_t*)malloc(512);
	storage->cbw = (cbw_t*)(((((size_t)storage->cbw) / 64) + 1) * 64);
	long long sectorCount = 0;
	for (int lun = 0; lun <= lunCnt; ++lun) {
		if (storage->inquiry_request()) return false;

		//Start storage
		storage->start_storage();
		//Wait for ready
		while (storage->test_unit_ready()) {

			//Request sense from device
			storage->request_sense();
		}
		sectorCount = storage->read_capacity();
	}
	log_debug(DEBUG_OK, "MSD sectors count: %lld, %lld MBytes", (int)sectorCount, (int)(sectorCount >> 11) + !!(sectorCount % (1 << 11)));
	storage->sectorsCount = sectorCount;
	//Try read to clear chache
	char * b = (char*)malloc(1024);
	storage->read((long long)0, (short)2, (void *)b);
	printf("Read ok. : %x", *(unsigned short*)b);
	free(b);
	storage_add(storage, STORAGE_USB);
}