#ifndef _STORAGE_H
#define _STORAGE_H (1)
#include "../ports.h"
#include "ata.h"
#include "ahci.h"
#define STORAGE_ATA		1
#define STORAGE_AHCI	2
#define STORAGE_USB		3
#define STORAGE_UNKNOWN 4
#define STORAGES_MAX 256
void storages_init();
void storage_add(void * device, int devtype);
class disk_storage {
	void * device;
	int type = 0;
public:
	void init(void *, int);
	int read(long long, short, void *);
private:

};
#endif