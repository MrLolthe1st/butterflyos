#include "../includes/storage/drives.h"

int name_compare(struct rb_tree * z, struct rb_node * a, struct rb_node * b)
{
	return strcmp((const char *)a->value, (const char *)b->value);
}
#define dnames(f) device_names->operator[]((char*)f)
map<char *, void *> * device_names;
void drives_init()
{
	device_names = (map<char *, void *>*)malloc(sizeof(map<char *, void *>));
	device_names->set_cmp(name_compare);
}

int logical_drive::read(long long LBA, short count, void * buffer)
{
	if (LBA < this->disk_offset || this->disk_offset + this->size_in_sectors < LBA + count)
		return 0;
	return this->strg->read(this->disk_offset + LBA, count, buffer);
}

int logical_drive::write(long long LBA, short count, void * buffer)
{
	if (LBA < this->disk_offset || this->disk_offset + this->size_in_sectors < LBA + count)
		return 0;
	return this->strg->write(this->disk_offset + LBA, count, buffer);
}

int logical_read(char * path, long long LBA, short count, void * buffer)
{
	logical_drive * drive = (logical_drive*) dnames(path);
	if (!drive) return 0;
	return drive->read(LBA, count, buffer);
}

int logical_write(char * path, long long LBA, short count, void * buffer)
{
	logical_drive * drive = (logical_drive*) dnames(path);
	if (!drive) return 0;
	return drive->write(LBA, count, buffer);
}

int add_device(char * name, disk_storage * str, long long offset, long long size)
{
	logical_drive * d = (logical_drive*)dnames(name);
	if (d) //Device alredy exists, go away from there!
		return 0;
	d = new logical_drive(str, offset, size, name);
	dnames(name) = (void*) d;
	return 1;
}

int remove_device(char * name)
{
	logical_drive * d = (logical_drive*)dnames(name);
	if (!d)
		return 0;
	device_names->erase(name);
	return 1;
}
