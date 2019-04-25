#include "../includes/storage/ata.h"
#include "../includes/storage/storage.h"
#include "../includes/video.h"
#include "../includes/memory/memory.h"
#include "../includes/interrupts.h"

#pragma GCC push_options
#pragma GCC optimize("Ofast")
ATA_Drive ata_disks[4];

void wait_for_set(volatile int * a, int c)
{
	while (!(*a & (1 << c)));
}

void wait_for_clear(volatile short * a, int c)
{
	while (*a & (1 << c));
}


int ATA_Drive::wait_ready()
{
	if (inportb(this->port + ATA_STATUS_OFFSET) & 1) return 0;
	while (inportb(this->port + ATA_STATUS_OFFSET) & (1 << STATUS_BSY_SHIFT));
	return 1;
}

int ATA_Drive::wait_data()
{
	if (inportb(this->port + ATA_STATUS_OFFSET) & 1) return 0;
	while (!(inportb(this->port + ATA_STATUS_OFFSET) & (1 << STATUS_DRQ_SHIFT)));
	return 1;
}

int ATA_Drive::identify_drive(unsigned short * buf)
{
	wait_for_clear(&this->busy, 1);
	this->busy |= 1;
	if (!this->wait_ready()) return 0;
	outportb(this->port + ATA_DRIVE_OFFSET, DRIVE_REG_INITIAL | (this->drive << DRIVE_DRIVE_SHIFT));
	inportb(this->port + ATA_STATUS_OFFSET);
	inportb(this->port + ATA_STATUS_OFFSET);
	inportb(this->port + ATA_STATUS_OFFSET);
	inportb(this->port + ATA_STATUS_OFFSET);
	inportb(this->port + ATA_STATUS_OFFSET);
	int cl = inportb(this->port + ATA_LBAMI_OFFSET);
	int ch = inportb(this->port + ATA_LBAHI_OFFSET);
	this->wait_ready();
	outportb(this->port + ATA_COUNT_OFFSET, 0);
	outportb(this->port + ATA_LBALO_OFFSET, 0);
	outportb(this->port + ATA_LBAMI_OFFSET, 0);
	outportb(this->port + ATA_LBAHI_OFFSET, 0);
	outportb(this->port + ATA_DRIVE_OFFSET, DRIVE_REG_INITIAL | (this->drive << DRIVE_DRIVE_SHIFT));
	outportb(this->port + ATA_COMMAND_OFFSET, ATA_IDENTIFY);
	this->wait_ready();
	char result = inportb(this->port + ATA_STATUS_OFFSET);
	if (!result || (result | 1)) return 0;
	this->wait_data();
	for (int i = 0; i < 256; i++)
		buf[i] = inportw(this->port + ATA_DATA_OFFSET);
	for (int i = 0; i < 20; i++)
		this->serial[i] = (buf[10 + (i >> 1)] >> ((!(i & 1)) << 3)) & 0xFF;
	this->serial[20] = 0;
	for (int i = 0; i < 8; i++)
		this->firmware[i] = (buf[23 + (i >> 1)] >> ((!(i & 1)) << 3)) & 0xFF;
	this->firmware[8] = 0;
	for (int i = 0; i < 40; i++)
		this->model[i] = (buf[27 + (i >> 1)] >> ((!(i & 1)) << 3)) & 0xFF;
	this->model[40] = 0;
	for (int i = 0; i < 60; i++)
		this->media_serial[i] = (buf[176 + (i >> 1)] >> ((!(i & 1)) << 3)) & 0xFF;
	this->media_serial[60] = 0;
	this->multiple_count = buf[47] & 0xFF;
	this->lba48_support = buf[83] & (1 << 10);
	if (this->lba48_support)
		this->sectors_count = (((unsigned long long) buf[103]) << 48) +
		(((unsigned long long) buf[102]) << 32) +
		(((unsigned long long) buf[101]) << 16) +
		(((unsigned long long) buf[100]));
	else
		this->sectors_count = (((unsigned long long) buf[61]) << 16) +
		(((unsigned long long) buf[60]));
	this->present = 1;
	this->busy &= ~1;
	return 1;
}

int ATA_Drive::write_lba28(long long start, short count, void * buffer)
{
	wait_for_clear(&this->busy, 1);
	this->busy |= 1;
	int LBA0 = (start >> 0) & 0xFFFF, LBA1 = (start >> 16) & 0xFFFF, LBA2 = (start >> 32) & 0xFFFF;
	outportb(this->port + ATA_COUNT_OFFSET, count);
	outportb(this->port + ATA_LBALO_OFFSET, (LBA0 >> 0) & 0xFF);
	outportb(this->port + ATA_LBAMI_OFFSET, (LBA0 >> 8) & 0xFF);
	outportb(this->port + ATA_LBAHI_OFFSET, (LBA1 >> 0) & 0xFF);
	outportb(this->port + ATA_DRIVE_OFFSET, DRIVE_REG_INITIAL | DRIVE_LBA_ENABLE | (this->drive << DRIVE_DRIVE_SHIFT));
	outportb(this->port + ATA_COMMAND_OFFSET, ATA_WRITE);
	this->wait_ready();
	this->wait_data();
	short * buf = (short*)buffer;
	for (int i = 0; i < 256 * count; i++)
		outportw(this->port + ATA_DATA_OFFSET, buf[i]);
	this->busy &= ~1;
	return 1;
}

int ATA_Drive::write_lba48(long long start, short count, void * buffer)
{
	wait_for_clear(&this->busy, 1);
	this->busy |= 1;
	int LBA0 = (start >> 0) & 0xFFFF, LBA1 = (start >> 16) & 0xFFFF, LBA2 = (start >> 32) & 0xFFFF;
	outportb(this->port + ATA_COUNT_OFFSET, count >> 8);
	outportb(this->port + ATA_LBALO_OFFSET, (LBA1 >> 8) & 0xFF);
	outportb(this->port + ATA_LBAMI_OFFSET, (LBA2) & 0xFF);
	outportb(this->port + ATA_LBAHI_OFFSET, (LBA2 >> 8) & 0xFF);
	outportb(this->port + ATA_COUNT_OFFSET, count & 0xFF);
	outportb(this->port + ATA_LBALO_OFFSET, (LBA0) & 0xFF);
	outportb(this->port + ATA_LBAMI_OFFSET, (LBA0 >> 8) & 0xFF);
	outportb(this->port + ATA_LBAHI_OFFSET, (LBA1) & 0xFF);
	outportb(this->port + ATA_DRIVE_OFFSET, DRIVE_REG_INITIAL | DRIVE_LBA_ENABLE | (this->drive << DRIVE_DRIVE_SHIFT));
	outportb(this->port + ATA_COMMAND_OFFSET, ATA_WRITE_EXT);
	this->wait_ready();
	this->wait_data();
	short * buf = (short*)buffer;
	for (int i = 0; i < 256 * count; i++)
		outportw(this->port + ATA_DATA_OFFSET, buf[i]);
	this->busy &= ~1;
	return 1;
}


int ATA_Drive::write(long long start, short count, void * buffer)
{
	if (this->lba48_support)
		return this->write_lba48(start, count, buffer);
	else
		return this->write_lba28(start, count, buffer);
}

int ATA_Drive::read_lba28(long long start, short count, void * buffer)
{
	wait_for_clear(&this->busy, 1);
	this->busy |= 1;
	int LBA0 = (start >> 0) & 0xFFFF, LBA1 = (start >> 16) & 0xFFFF, LBA2 = (start >> 32) & 0xFFFF;
	outportb(this->port + ATA_COUNT_OFFSET, count);
	outportb(this->port + ATA_LBALO_OFFSET, (LBA0 >> 0) & 0xFF);
	outportb(this->port + ATA_LBAMI_OFFSET, (LBA0 >> 8) & 0xFF);
	outportb(this->port + ATA_LBAHI_OFFSET, (LBA1 >> 0) & 0xFF);
	outportb(this->port + ATA_DRIVE_OFFSET, DRIVE_REG_INITIAL | DRIVE_LBA_ENABLE | (this->drive << DRIVE_DRIVE_SHIFT));
	outportb(this->port + ATA_COMMAND_OFFSET, ATA_READ);
	this->wait_ready();
	this->wait_data();
	short * buf = (short*)buffer;
	for (int i = 0; i < 256 * count; i++)
		buf[i] = inportw(this->port + ATA_DATA_OFFSET);
	this->busy &= ~1;
	return 1;
}

int ATA_Drive::read_lba48(long long start, short count, void * buffer)
{
	//count = 1;
	wait_for_clear(&this->busy, 1);
	this->busy |= 1;
	this->wait_ready();
	int LBA0 = (start >> 0) & 0xFFFF, LBA1 = (start >> 16) & 0xFFFF, LBA2 = (start >> 32) & 0xFFFF;
	outportb(this->port + ATA_COUNT_OFFSET, count >> 8);
	outportb(this->port + ATA_LBALO_OFFSET, (LBA1 >> 8) & 0xFF);
	outportb(this->port + ATA_LBAMI_OFFSET, (LBA2) & 0xFF);
	outportb(this->port + ATA_LBAHI_OFFSET, (LBA2 >> 8) & 0xFF);
	outportb(this->port + ATA_COUNT_OFFSET, count & 0xFF);
	outportb(this->port + ATA_LBALO_OFFSET, (LBA0) & 0xFF);
	outportb(this->port + ATA_LBAMI_OFFSET, (LBA0 >> 8) & 0xFF);
	outportb(this->port + ATA_LBAHI_OFFSET, (LBA1) & 0xFF);
	outportb(this->port + ATA_DRIVE_OFFSET, DRIVE_REG_INITIAL | DRIVE_LBA_ENABLE | (this->drive << DRIVE_DRIVE_SHIFT));
	outportb(this->port + ATA_COMMAND_OFFSET, ATA_READ_EXT);
	this->wait_ready();
	this->wait_data();
	short * buf = (short*)buffer;
	for (int i = 0; i < 256 * count; i++)
		buf[i] = inportw(this->port + ATA_DATA_OFFSET);
	this->busy &= ~1;
	return 1;
}


int ATA_Drive::read(long long start, short count, void * buffer)
{
	if (this->lba48_support)
		return this->read_lba48(start, count, buffer);
	else
		return this->read_lba28(start, count, buffer);
}

int ATA_Drive::init(int port, int drive)
{
	this->port = port;
	this->present = false;
	this->busy = 0;
	this->drive = drive;
	unsigned short identify[256];
	if (this->identify_drive((unsigned short *)&identify)) {
		this->busy = 0;
		this->present = true;
		print_string("Detected ATA disk: ");
		print_string(this->model);
		print_string("\n");
		return 1;
	}
	else return 0;
}

void ata_init()
{
	ata_disks[0].init(PRIMARY_BASE, 0);
	ata_disks[1].init(PRIMARY_BASE, 1);
	ata_disks[2].init(SECONDARY_BASE, 0);
	ata_disks[3].init(SECONDARY_BASE, 1);
	for (int i = 0; i < 3; i++)
		if (ata_disks[i].present)
		{
			storage_add(&ata_disks[i], STORAGE_ATA);
			/*
			print_string("Testing read speed....\n");
			print_string("Single-sector mode: ");
			char * buf = (char*)malloc(512 * 16);
			long long t = time;
			for (int j = 0; j < 2048; j++)
			{
				ata_disks[i].read((long long)j, 1, (void*)buf);
			}
			print_int((int)(time - t), 10);
			print_string(" ticks/megabyte.\nMultisector mode: ");
			t = time;
			for (int j = 0; j < 2048 / 16; j++)
			{
				ata_disks[i].read((long long)(2048 + j * 16), (short)16, (void*)buf);
			}
			print_int((int)(time - t), 10);
			print_string(" ticks/megabyte.\n");
			free(buf);*/
		}
	//print_string("ATA Ready.\n");
}
#pragma GCC pop_options