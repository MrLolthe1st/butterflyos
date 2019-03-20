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
const size_t CACHE_MAX_RECORDS = 4097; //~10ns access, much better, than 400-900ns, but it takes about 2.5mbyte of RAM.
void storages_init();
void storage_add(void * device, int devtype);
#include "../../std/map.h"
typedef struct {
	long long LBA;
	long long la = -1e18 - 1;
	short count;
	void * buf;
} _storage_cache_block;

class disk_storage {
	void * device;
	int type = 0;
	map<long long, _storage_cache_block *> * cache;
	map<long long, long long> * last_access;
	long long la_val = -1e18;
public:
	void init(void *, int);
	void cleanup();
	int read(long long, short, void *);
	int write(long long, short, void *);
private:

};
#include "msd.h"
#endif