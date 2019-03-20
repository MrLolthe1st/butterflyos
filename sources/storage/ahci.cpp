#include "../includes/storage/storage.h"
#define uint unsigned int

void *memcpy(void *dest, const void *source, size_t count);
void *memset(void *buf, int ch, size_t count);
HBA_MEM * abar;
// ------------------------------------------------------------------------------------------------
void port_rebase(HBA_PORT *port, int portno)
{
	stop_cmd(port);	// Stop command engine

					// Command list offset: 1K*portno
					// Command list entry size = 32
					// Command list entry maxim count = 32
					// Command list maxim size = 32*32 = 1K per port
	port->clb = (uint) malloc(1024 + 4196);
	port->clb = (((uint)port->clb) / 2048) * 2048 + 2048;
	port->clbu = 0;
	memset((void*)(port->clb), 0, 1024);

	// FIS offset: 32K+256*portno
	// FIS entry size = 256 unsigned chars per port
	port->fb = (uint)malloc(256 + 4196);
	port->fb = (((uint)port->fb) / 2048) * 2048 + 2048;
	port->fbu = 0;
	memset((void*)(port->fb), 0, 256);

	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
	for (int i = 0; i < 32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
								// 256 unsigned chars per command table, 64+16+48+16*8
								// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = (uint)malloc(256 + 4196);
		cmdheader[i].ctba = (((uint)cmdheader[i].ctba) / 2048) * 2048 + 2048;
		cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}

	start_cmd(port);	// Start command engine
}

// ------------------------------------------------------------------------------------------------
// Start command engine
void start_cmd(HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR)
		////print_string("cmd: %d %d\n", port->cmd, port->cmd & HBA_PxCMD_CR);
		;
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

// ------------------------------------------------------------------------------------------------
// Stop command engine
void stop_cmd(HBA_PORT *port)
{
	//	//print_string("%x\n", port->cmd);
		// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
	port->cmd &= ~HBA_PxCMD_FRE;

	//port->cmd &= ~HBA_PxCMD_FRE;
	// Wait until FR (bit14), CR (bit15) are cleared
	while (1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}

	// Clear FRE (bit4)
}

#define HBA_PxIS_TFES   (1 << 30)

// ------------------------------------------------------------------------------------------------
bool ahci_read_sectors(HBA_MEM * abar, HBA_PORT *port, unsigned long long starth, unsigned short count, uint16_t *buf)
{
	port->is = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(abar, port);
	if (slot == -1)
		return false;

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count - 1) >> 4) + 1;	// PRDT entries count

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
		(cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));
	int i = 0;
	// 8K u8s (16 sectors) per PRDT
	for (i = 0; i < cmdheader->prdtl - 1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8 * 1024 - 1;	//8K u8s (this value should always 
													// be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4 * 1024;	// 4K u16s
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t)buf;
	cmdtbl->prdt_entry[i].dbc = (count << 9) - 1;	// 512 u8s per sector
	cmdtbl->prdt_entry[i].i = 1;

	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = 0x25;

	cmdfis->lba0 = (uint8_t)starth;
	cmdfis->lba1 = (uint8_t)(starth >> 8);
	cmdfis->lba2 = (uint8_t)(starth >> 16);
	cmdfis->device = 1 << 6;	// LBA mode

	cmdfis->lba3 = (uint8_t)(starth >> 24);
	cmdfis->lba4 = (uint8_t)(starth >> 32);
	cmdfis->lba5 = (uint8_t)(starth >> 40);

	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;

	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		//print_string("Port is hung\n");
		return 0;
	}

	port->ci = 1 << slot;	// Issue command

							// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		//Wait(1);
		if ((port->ci & (1 << slot)) == 0)
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			//print_string("Read disk error %x\n", (uint)starth);
			return 0;
		}
	}

	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		//print_string("Read disk error %x\n", (uint)starth);
		return 0;
	}

	return true;
}

// ------------------------------------------------------------------------------------------------
bool AHCI_identify(HBA_MEM * abar, HBA_PORT *port, uint16_t *buf)
{
	port->is = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(abar, port);
	if (slot == -1)
		return false;

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = 1;	// PRDT entries count

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
		(cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));
	int i = 0;
	cmdtbl->prdt_entry[0].dba = (uint32_t)buf;
	cmdtbl->prdt_entry[0].dbc = 511;	// 512 u8s per sector
	cmdtbl->prdt_entry[0].i = 1;
	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
	memset(cmdfis, 0, sizeof(FIS_REG_H2D));
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = 0xEC;

	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		//print_string("Port is hung\n");
		return 0;
	}

	port->ci = 1 << slot;	// Issue command

							// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		//Wait(1);
		if ((port->ci & (1 << slot)) == 0)
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			//print_string("Read disk error\n");
			return 0;
		}
	}

	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		//print_string("Read disk error\n");
		return 0;
	}

	return true;
}


// ------------------------------------------------------------------------------------------------
int ahci_write_sectors(HBA_MEM * abar, HBA_PORT *port, unsigned long long starth, unsigned short count,
	short * buf)
{
	port->is = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0;					// Spin lock timeout counter
	int slot = find_cmdslot(abar, port);
	if (slot == -1)	return false;
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 1;		// Read from device
	cmdheader->prdtl = (uint16_t)((count - 1) >> 4) + 1;	// PRDT entries count
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
		(cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));
	int i = 0;
	// 8K u8s (16 sectors) per PRDT
	for (i = 0; i < cmdheader->prdtl - 1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8 * 1024 - 1;	//8K u8s (this value should always
													// be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4 * 1024;	// 4K u16s
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t)buf;
	cmdtbl->prdt_entry[i].dbc = (count << 9) - 1;	// 512 u8s per sector
	cmdtbl->prdt_entry[i].i = 1;

	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = 0x35;
	cmdfis->lba0 = (uint8_t)starth;
	cmdfis->lba1 = (uint8_t)(starth >> 8);
	cmdfis->lba2 = (uint8_t)(starth >> 16);
	cmdfis->device = 1 << 6;	// LBA mode
	cmdfis->lba3 = (uint8_t)(starth >> 24);
	cmdfis->lba4 = (uint8_t)(starth >> 32);
	cmdfis->lba5 = (uint8_t)(starth >> 40);
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		//print_string("Port is hung\n");
		return 0;
	}
	port->ci = 1 << slot;	// Issue command
							// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		//Wait(1);
		if ((port->ci & (1 << slot)) == 0)
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			//print_string("Read disk error\n");
			return 0;
		}
	}
	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		//print_string("Read disk error\n");
		return 0;
	}
	return true;
}


// ------------------------------------------------------------------------------------------------
// Find a free command list slot
int find_cmdslot(HBA_MEM *abar, HBA_PORT *m_port)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (m_port->sact | m_port->ci);
	unsigned int qq = (abar->cap & 0x0f00) >> 8;
	for (int i = 0; i < qq; i++)
	{
		if ((slots & 1) == 0)
			return i;
		slots >>= 1;
	}
	return -1;
}

// ------------------------------------------------------------------------------------------------
// Check device type
int check_type(HBA_PORT *port)
{
	uint32_t ssts = port->ssts;
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;

	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

void AHCI_Drive::init(void * ptr, long long sect_cnt)
{
	this->port = ptr;
	this->serctors_count = sect_cnt;
}

int AHCI_Drive::read(long long LBA, short count, void * buf)
{
	return ahci_read_sectors((HBA_MEM*)this->abar, (HBA_PORT*)this->port, LBA, (unsigned short)count, (short unsigned int *)buf);
}

int AHCI_Drive::write(long long LBA, short count, void * buf)
{
	return ahci_write_sectors((HBA_MEM*)this->abar, (HBA_PORT*)this->port, LBA, (unsigned short)count, (short *)buf);
}
#include "../includes/interrupts.h"
// ------------------------------------------------------------------------------------------------
//Probe AHCI port
void ahci_probe_port(void *abar_temp1)
{
	HBA_MEM *abar_temp = (HBA_MEM *)abar_temp1;
	// Search disk in impelemented ports
	uint pi = abar_temp->pi;
	int i = 0;
	while (i < 32)
	{
		if (pi & 1)
		{
			uint cmd_st = (abar_temp->ports[i].cmd) & 0x1;
			uint cmd_fre = (abar_temp->ports[i].cmd) & 0x10;
			int dt = check_type((HBA_PORT *)&abar_temp->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				//print_string("SATA drive found at port...");
				abar = abar_temp;
				unsigned short * buf = (unsigned short *)malloc(16 * 512);
				buf[0] = 1;
				int res = 0;
				int jj = 0;
				port_rebase(&abar_temp->ports[i], i);
				abar_temp->ports[i].cmd |= 0x10;
				abar_temp->ports[i].cmd |= 0x1;
				res = ahci_read_sectors(abar, &abar->ports[i], 0, 16, buf);
				if (res != 0) {
					AHCI_identify(abar, &abar->ports[i], buf);
					AHCI_Drive * ad = (AHCI_Drive*)malloc(sizeof(AHCI_Drive));
					ad->init((void*)&abar->ports[i], (long long)((identify_data*)buf)->sectors_48);
					ad->abar = (void*)abar;
					storage_add(ad, STORAGE_AHCI);
					memcpy((void*)ad->model, (const void*)&((identify_data*)buf)->model, 40);
					for (int j = 0; j < 20; j++)
					{
						int aa = ad->model[j * 2 + 1];
						ad->model[j * 2 + 1] = ad->model[j * 2];
						ad->model[j * 2] = aa;
					}
					ad->model[40] = 0;
					printf("AHCI: %dmbytes, %s\n", (int)ad->serctors_count >> 11, ad->model);
				}
				free(buf);
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				////print_string("\nSATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				////print_string("\nSEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				////print_string("\nPM drive found at port %d\n", i);
			}
			else
			{
				////print_string("\nNo drive found at port %d\n", i);
			}
		}
		pi >>= 1;
		i++;
	}

}
