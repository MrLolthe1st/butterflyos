#ifndef _AHCI_H
#define _AHCI_H (1)
#include "../target.h"
#include "storage.h"
#include "../memory/memory.h"
#include "../video.h"
class AHCI_Drive {
public:
	long long serctors_count;
	void * port, *abar;
	char model[41];
	void init(void *, long long);
	int read(long long, short, void *);
	int write(long long, short, void *);

};
#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000


#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define ATA_SECTSIZE 512

#define ATA_LBA_MODE (1<<6)

#define ATA_MAXLBA28 ((1 << 28) - 1)
#define ATA_MAXLBA48 ((1l << 48) - 1)

// status
#define ATA_STS_ERR (1 << 0)    // error
#define ATA_STS_DRQ (1 << 3)    // data transfer requested
#define ATA_STS_BSY (1 << 7)    // busy

// commands
#define ATA_CMD_READ_SECTORS 0x20
#define ATA_CMD_READ_DMA_EXT 0x25

// ------------------------------------------------------------------------------------------------
typedef enum
{
	FIS_TYPE_REG_H2D = 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H = 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT = 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP = 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA = 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST = 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP = 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS = 0xA1,	// Set device bits FIS - device to host
} FIS_TYPE;

// ------------------------------------------------------------------------------------------------
typedef struct tagFIS_REG_H2D
{
	uint8_t  fis_type;		// FIS_TYPE_REG_H2D

	uint8_t  pmport : 4;	// Port multiplier
	uint8_t  rsv0 : 3;		// Reserved
	uint8_t  c : 1;			// 1: Command, 0: Control

	uint8_t  command;		// Command register
	uint8_t  featurel;		// Feature register, 7:0

							// unsigned int 1
	uint8_t  lba0;			// LBA low register, 7:0
	uint8_t  lba1;			// LBA mid register, 15:8
	uint8_t  lba2;			// LBA high register, 23:16
	uint8_t  device;		// Device register

							// unsigned int 2
	uint8_t  lba3;			// LBA register, 31:24
	uint8_t  lba4;			// LBA register, 39:32
	uint8_t  lba5;			// LBA register, 47:40
	uint8_t  featureh;		// Feature register, 15:8

							// unsigned int 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  icc;			// Isochronous command completion
	uint8_t  control;		// Control register

							// unsigned int 4
	uint8_t  rsv1[4];		// Reserved
} FIS_REG_H2D;

// ------------------------------------------------------------------------------------------------
typedef struct tagFIS_REG_D2H
{
	uint8_t  fis_type;    // FIS_TYPE_REG_D2H

	uint8_t  pmport : 4;    // Port multiplier
	uint8_t  rsv0 : 2;      // Reserved
	uint8_t  i : 1;         // Interrupt bit
	uint8_t  rsv1 : 1;      // Reserved

	uint8_t  status;      // Status register
	uint8_t  error;       // Error register

						  // unsigned int 1
	uint8_t  lba0;        // LBA low register, 7:0
	uint8_t  lba1;        // LBA mid register, 15:8
	uint8_t  lba2;        // LBA high register, 23:16
	uint8_t  device;      // Device register

						  // unsigned int 2
	uint8_t  lba3;        // LBA register, 31:24
	uint8_t  lba4;        // LBA register, 39:32
	uint8_t  lba5;        // LBA register, 47:40
	uint8_t  rsv2;        // Reserved

						  // unsigned int 3
	uint8_t  countl;      // Count register, 7:0
	uint8_t  counth;      // Count register, 15:8
	uint8_t  rsv3[2];     // Reserved

						  // unsigned int 4
	uint8_t  rsv4[4];     // Reserved
} FIS_REG_D2H;

// ------------------------------------------------------------------------------------------------
typedef struct tagFIS_DATA
{
	uint8_t  fis_type;		// FIS_TYPE_DATA

	uint8_t  pmport : 4;	// Port multiplier
	uint8_t  rsv0 : 4;		// Reserved

	uint8_t  rsv1[2];		// Reserved

							// unsigned int 1 ~ N
	uint32_t data[1];		// Payload
} FIS_DATA;

// ------------------------------------------------------------------------------------------------
typedef struct tagFIS_PIO_SETUP
{
	uint8_t  fis_type;		// FIS_TYPE_PIO_SETUP

	uint8_t  pmport : 4;	// Port multiplier
	uint8_t  rsv0 : 1;		// Reserved
	uint8_t  d : 1;			// Data transfer direction, 1 - device to host
	uint8_t  i : 1;			// Interrupt bit
	uint8_t  rsv1 : 1;

	uint8_t  status;		// Status register
	uint8_t  error;			// Error register

							// unsigned int 1
	uint8_t  lba0;			// LBA low register, 7:0
	uint8_t  lba1;			// LBA mid register, 15:8
	uint8_t  lba2;			// LBA high register, 23:16
	uint8_t  device;		// Device register

							// unsigned int 2
	uint8_t  lba3;			// LBA register, 31:24
	uint8_t  lba4;			// LBA register, 39:32
	uint8_t  lba5;			// LBA register, 47:40
	uint8_t  rsv2;			// Reserved

							// unsigned int 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  rsv3;			// Reserved
	uint8_t  e_status;		// New value of status register

							// unsigned int 4
	uint16_t tc;			// Transfer count
	uint8_t  rsv4[2];		// Reserved
} FIS_PIO_SETUP;

// ------------------------------------------------------------------------------------------------
typedef struct tagFIS_DMA_SETUP
{
	// unsigned int 0
	uint8_t  fis_type;		// FIS_TYPE_DMA_SETUP

	uint8_t  pmport : 4;	// Port multiplier
	uint8_t  rsv0 : 1;		// Reserved
	uint8_t  d : 1;			// Data transfer direction, 1 - device to host
	uint8_t  i : 1;			// Interrupt bit
	uint8_t  a : 1;         // Auto-activate. Specifies if DMA Activate FIS is needed

	uint8_t  rsved[2];      // Reserved

							//unsigned int 1&2

	uint64_t DMAbufferID;   //DMA Buffer Identifier. Used to Identify DMA 
							// buffer in host memory. SATA Spec says host specific and not in 
							// Spec. Trying AHCI spec might work.

							//unsigned int 3
	uint32_t rsvd;          //More reserved

							//unsigned int 4
	uint32_t DMAbufOffset;  //unsigned char offset into buffer. First 2 bits must be 0

							//unsigned int 5
	uint32_t TransferCount; //Number of unsigned chars to transfer. Bit 0 must be 0

							//unsigned int 6
	uint32_t resvd;         //Reserved

} FIS_DMA_SETUP;

// ------------------------------------------------------------------------------------------------
typedef volatile struct tagHBA_PORT
{
	uint32_t clb;		// 0x00, command list base address, 1K-unsigned char aligned
	uint32_t clbu;		// 0x04, command list base address upper 32 bits
	uint32_t fb;		// 0x08, FIS base address, 256-unsigned char aligned
	uint32_t fbu;		// 0x0C, FIS base address upper 32 bits
	uint32_t is;		// 0x10, interrupt status
	uint32_t ie;		// 0x14, interrupt enable
	uint32_t cmd;		// 0x18, command and status
	uint32_t rsv0;		// 0x1C, Reserved
	uint32_t tfd;		// 0x20, task file data
	uint32_t sig;		// 0x24, signature
	uint32_t ssts;		// 0x28, SATA status (SCR0:SStatus)
	uint32_t sctl;		// 0x2C, SATA control (SCR2:SControl)
	uint32_t serr;		// 0x30, SATA error (SCR1:SError)
	uint32_t sact;		// 0x34, SATA active (SCR3:SActive)
	uint32_t ci;		// 0x38, command issue
	uint32_t sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	uint32_t fbs;		// 0x40, FIS-based switch control
	uint32_t rsv1[11];	// 0x44 ~ 0x6F, Reserved
	uint32_t vendor[4];	// 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

// ------------------------------------------------------------------------------------------------
typedef volatile struct tagHBA_MEM
{
	// 0x00 - 0x2B, Generic Host Control
	uint32_t cap;		// 0x00, Host capability
	uint32_t ghc;		// 0x04, Global host control
	uint32_t is;		// 0x08, Interrupt status
	uint32_t pi;		// 0x0C, Port implemented
	uint32_t vs;		// 0x10, Version
	uint32_t ccc_ctl;	// 0x14, Command completion coalescing control
	uint32_t ccc_pts;	// 0x18, Command completion coalescing ports
	uint32_t em_loc;	// 0x1C, Enclosure management location
	uint32_t em_ctl;	// 0x20, Enclosure management control
	uint32_t cap2;		// 0x24, Host capabilities extended
	uint32_t bohc;		// 0x28, BIOS/OS handoff control and status
						// 0x2C - 0x9F, Reserved
	uint8_t  rsv[0xA0 - 0x2C];

	// 0xA0 - 0xFF, Vendor specific registers
	uint8_t  vendor[0x100 - 0xA0];

	// 0x100 - 0x10FF, Port control registers
	HBA_PORT	ports[1];	// 1 ~ 32
} HBA_MEM;

// ------------------------------------------------------------------------------------------------
typedef struct tagHBA_CMD_HEADER
{
	// DW0
	uint8_t  cfl : 5;	// Command FIS length in unsigned intS, 2 ~ 16
	uint8_t  a : 1;		// ATAPI
	uint8_t  w : 1;		// Write, 1: H2D, 0: D2H
	uint8_t  p : 1;		// Prefetchable

	uint8_t  r : 1;		// Reset
	uint8_t  b : 1;		// BIST
	uint8_t  c : 1;		// Clear busy upon R_OK
	uint8_t  rsv0 : 1;	// Reserved
	uint8_t  pmp : 4;	// Port multiplier port

	uint16_t prdtl;		// Physical region descriptor table length in entries

						// DW1
	volatile
		uint32_t prdbc;	// Physical region descriptor unsigned char count transferred

						// DW2, 3
	uint32_t ctba;		// Command table descriptor base address
	uint32_t ctbau;		// Command table descriptor base address upper 32 bits

						// DW4 - 7
	uint32_t rsv1[4];	// Reserved
} HBA_CMD_HEADER;

// ------------------------------------------------------------------------------------------------
typedef struct tagHBA_PRDT_ENTRY
{
	uint32_t dba;		// Data base address
	uint32_t dbau;		// Data base address upper 32 bits
	uint32_t rsv0;		// Reserved

						// DW3
	uint32_t dbc : 22;	// unsigned char count, 4M max
	uint32_t rsv1 : 9;	// Reserved
	uint32_t i : 1;		// Interrupt on completion
} HBA_PRDT_ENTRY;

// ------------------------------------------------------------------------------------------------
typedef struct tagHBA_CMD_TBL
{
	// 0x00
	uint8_t  cfis[64];				// Command FIS

									// 0x40
	uint8_t  acmd[16];				// ATAPI command, 12 or 16 unsigned chars

									// 0x50
	uint8_t  rsv[48];				// Reserved

									// 0x80
	HBA_PRDT_ENTRY	prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
} HBA_CMD_TBL;


// ------------------------------------------------------------------------------------------------
typedef struct  identify_data
{
	uint16_t	flags; // 0
	uint16_t	unused1[9]; // 9
	char		serial[20]; // 19
	uint16_t	unused2[3]; // 22
	char		firmware[8]; // 26
	char		model[40]; // 46
	uint16_t	sectors_per_int; // 47
	uint16_t	unused3; // 48
	uint16_t	capabilities[2]; // 50
	uint16_t	unused4[2]; // 52
	uint16_t	valid_ext_data; // 53
	uint16_t	unused5[5]; // 58
	uint16_t	size_of_rw_mult; // 59
	uint32_t	sectors_28; // 61
	uint16_t	unused6[38]; // 99
	uint64_t	sectors_48; // 103
	uint16_t	unused7[2]; // 105
	uint16_t	phys_log_size; // 106
	uint16_t	unused8[10]; // 116
	uint32_t	sector_size; // 118
	uint16_t	unused9[137];
} identify_data;
int find_cmdslot(HBA_MEM * abar, HBA_PORT *m_port);
void ahci_probe_port(void *abar_temp1);
bool AHCI_identify(HBA_PORT *port, uint16_t *buf);
bool ahci_read_sectors(HBA_MEM*abar, HBA_PORT *port, unsigned long long starth, unsigned short count, uint16_t *buf);
void start_cmd(HBA_PORT *port);
void stop_cmd(HBA_PORT *port);
void port_rebase(HBA_PORT *port, int portno);
int check_type(HBA_PORT *port);
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
int ahci_write_sectors(HBA_MEM*abar, HBA_PORT *port, unsigned long long starth, unsigned short count,
	short * buf);
#endif