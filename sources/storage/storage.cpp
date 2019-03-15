#include "../includes/storage/storage.h"
#include "../includes/video.h"
disk_storage storages[STORAGES_MAX];
int storages_count = 0;
void disk_storage::init(void * device, int devtype)
{
	this->device = device;
	this->type = devtype;
}

int disk_storage::read(long long LBA, short count, void * buffer)
{
	switch (this->type)
	{
	case STORAGE_ATA:
		return ((ATA_Drive*)this->device)->read(LBA, count, buffer);

	default:
		break;
	}
}

void storages_init()
{
	storages_count = 0;
	ata_init();
	print_string("Storages ready.\n");

}

void storage_add(void * device, int devtype)
{
	storages[storages_count++].init(device, devtype);
}