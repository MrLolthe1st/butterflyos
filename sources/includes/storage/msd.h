#ifndef _MSD_H
#define _MSD_H (1)
#include "storage.h"
#include "../../usb/usbd.h"

typedef struct  __attribute__((__packed__)) _q1__q {
	uint32_t sig;
	uint32_t tag;
	uint32_t xfer_len;
	uint8_t flags;
	uint8_t lun;
	uint8_t wcb_len;
	u8 cmd;
	unsigned char command[15];
} cbw_t;
class usb_mass_storage {
public:
	UsbDevice * d;
	UsbTransfer * t;
	unsigned long long sectorsCount;
	unsigned char lunCount;
	unsigned int bytesPerBlock;
	UsbEndpoint * endpointIn, *endpointOut;
	unsigned int tag;
	bool inquiry_request();
	bool request_sense();
	void start_storage();
	bool read(long long, short, void *);
	bool write(long long, short, void *);
	long long read_capacity();
	bool test_unit_ready();
	cbw_t * cbw;
};
bool usb_storage_init(UsbDevice * dev);
#endif