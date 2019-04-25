#ifndef _DRIVES_H
#define _DRIVES_H (1)
#include "../ports.h"
#include "ata.h"
#include "ahci.h"
#include "storage.h"
#include "../../std/map.h"
//extern map<char *, void * > device_names;
void drives_init();
#define DEVTYPE_HDD 0x02
#define DEVTYPE_MSD 0x03

class logical_drive {
	int dev_type = 0;
	disk_storage * strg;
public:
	long long disk_offset = 0;
	long long size_in_sectors = 0;
	int read(long long, short, void *);
	int write(long long, short, void *);
	char * name = 0;
	logical_drive(disk_storage * str, long long offset, long long size, char * name = 0, int dev_type = 0) { this->dev_type = dev_type; this->disk_offset = offset; this->size_in_sectors = size; this->strg = str; this->name = name; };

private:
};

int remove_device(char * name);
int add_device(char * name, disk_storage * str, long long offset, long long size);
int logical_write(char * path, long long LBA, short count, void * buffer);
int logical_read(char * path, long long LBA, short count, void * buffer);
#endif