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
	case STORAGE_AHCI:
		return ((AHCI_Drive*)this->device)->read(LBA, count, buffer);
		break;
	default:
		break;
	}
	return 0;
}

int disk_storage::write(long long LBA, short count, void * buffer)
{
	AHCI_Drive * d = (AHCI_Drive*)this->device;
	switch (this->type)
	{
	case STORAGE_ATA:
		return ((ATA_Drive*)this->device)->write(LBA, count, buffer);
	case STORAGE_AHCI:
		return ((AHCI_Drive*)this->device)->write(LBA, count, buffer);
	default:
		break;
	}
	return 0;
}

void storages_init()
{
	storages_count = 0;
	ata_init();
}

void make_patritions(disk_storage &disk)
{

}

void storage_add(void * device, int devtype)
{
	int cnt = storages_count++;
	storages[cnt].init(device, devtype);
	make_patritions(storages[cnt]);
}