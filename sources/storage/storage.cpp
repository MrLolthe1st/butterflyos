#include "../includes/storage/storage.h"
#include "../includes/video.h"
disk_storage storages[STORAGES_MAX];
int storages_count = 0;
void disk_storage::init(void * device, int devtype)
{
	this->device = device;
	this->type = devtype;
	this->cache = (map<long long, _storage_cache_block *>*) malloc(sizeof(map<long long, _storage_cache_block *>));
	this->cache->set_cmp(memcmp<long long, _storage_cache_block *>);
	this->last_access = (map<long long, long long>*) malloc(sizeof(map<long long, long long>));
	this->last_access->set_cmp(memcmp<long long, long long>);
}

void disk_storage::cleanup()
{

}

int disk_storage::read(long long LBA, short count, void * buffer)
{
	//Try read from cache.
	_storage_cache_block * &scb = this->cache->operator[](LBA);
	if (this->la_val > 1e9)
	{
		//Iterate over map, and free all instances.
		//map<long long, _storage_cache_block *>::iterator it = ((*this->cache).begin());

		this->cache->clear();
		this->last_access->clear();
		this->la_val = -1e18;
	}
	if (scb)
	{
		memcpy(buffer, scb->buf, count * 512);
		//And say, that we're lastacessed it
		if (scb->la != -(1e18 + 1))
			this->last_access->erase(scb->la);
		this->last_access->operator[](this->la_val) = LBA;
		scb->la = this->la_val;
		this->la_val++;
		return true;
	}
	int res = 0;
	switch (this->type)
	{
	case STORAGE_ATA:
		res = ((ATA_Drive*)this->device)->read(LBA, count, buffer);
		break;
	case STORAGE_AHCI:
		res = ((AHCI_Drive*)this->device)->read(LBA, count, buffer);
		break;
	case STORAGE_USB:
		res = ((usb_mass_storage*)this->device)->read(LBA, count, buffer);
		break;
	default:
		break;
	}
	if (res)
	{
		this->cleanup();
		_storage_cache_block * scb = (_storage_cache_block*)malloc(sizeof(_storage_cache_block));
		scb->buf = malloc(count * 512);
		memcpy(scb->buf, buffer, count * 512);
		scb->LBA = LBA;
		scb->count = count;
		scb->la = this->la_val;
		this->last_access->operator[](this->la_val) = LBA;
		this->la_val++;
		this->cache->operator[](LBA) = scb;
		return res;
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
	case STORAGE_USB:
		return ((usb_mass_storage*)this->device)->write(LBA, count, buffer);
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