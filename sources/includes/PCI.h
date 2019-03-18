#ifndef _PCI_H
#define _PCI_H (1)
#include "ports.h"

#define PCI_MAKE_ID(bus, dev, func)     ((bus) << 16) | ((dev) << 11) | ((func) << 8)

#define PCI_C_STORAGE 0x1

#define PCI_SC_AHCI   0x6
// I/O Ports
#define PCI_CONFIG_ADDR                 0xcf8
#define PCI_CONFIG_DATA                 0xcfC

// Header Type
#define PCI_TYPE_MULTIFUNC              0x80
#define PCI_TYPE_GENERIC                0x00
#define PCI_TYPE_PCI_BRIDGE             0x01
#define PCI_TYPE_CARDBUS_BRIDGE         0x02

// PCI Configuration Registers
#define PCI_CONFIG_VENDOR_ID            0x00
#define PCI_CONFIG_DEVICE_ID            0x02
#define PCI_CONFIG_COMMAND              0x04
#define PCI_CONFIG_STATUS               0x06
#define PCI_CONFIG_REVISION_ID          0x08
#define PCI_CONFIG_PROG_INTF            0x09
#define PCI_CONFIG_SUBCLASS             0x0a
#define PCI_CONFIG_CLASS_CODE           0x0b
#define PCI_CONFIG_CACHELINE_SIZE       0x0c
#define PCI_CONFIG_LATENCY              0x0d
#define PCI_CONFIG_HEADER_TYPE          0x0e
#define PCI_CONFIG_BIST                 0x0f

// Type 0x00 (Generic) Configuration Registers
#define PCI_CONFIG_BAR0                 0x10
#define PCI_CONFIG_BAR1                 0x14
#define PCI_CONFIG_BAR2                 0x18
#define PCI_CONFIG_BAR3                 0x1c
#define PCI_CONFIG_BAR4                 0x20
#define PCI_CONFIG_BAR5                 0x24
#define PCI_CONFIG_CARDBUS_CIS          0x28
#define PCI_CONFIG_SUBSYSTEM_VENDOR_ID  0x2c
#define PCI_CONFIG_SUBSYSTEM_DEVICE_ID  0x2e
#define PCI_CONFIG_EXPANSION_ROM        0x30
#define PCI_CONFIG_CAPABILITIES         0x34
#define PCI_CONFIG_INTERRUPT_LINE       0x3c
#define PCI_CONFIG_INTERRUPT_PIN        0x3d
#define PCI_CONFIG_MIN_GRANT            0x3e
#define PCI_CONFIG_MAX_LATENCY          0x3f

// ------------------------------------------------------------------------------------------------
// PCI BAR

#define PCI_BAR_IO                      0x01
#define PCI_BAR_LOWMEM                  0x02
#define PCI_BAR_64                      0x04
#define PCI_BAR_PREFETCH                0x08

#define VENDOR_INTEL                    0x8086

// PCI Classes
#define PCI_CLASS_LEGACY                0x00
#define PCI_CLASS_STORAGE               0x01
#define PCI_CLASS_NETWORK               0x02
#define PCI_CLASS_DISPLAY               0x03
#define PCI_CLASS_MULTIMEDIA            0x04
#define PCI_CLASS_MEMORY                0x05
#define PCI_CLASS_BRIDGE_DEVICE         0x06
#define PCI_CLASS_COMMUNICATION         0x07
#define PCI_CLASS_PERIHPERALS           0x08
#define PCI_CLASS_INPUT_DEVICES         0x09
#define PCI_CLASS_DOCKING_STATION       0x0a
#define PCI_CLASS_PROCESSOR             0x0b
#define PCI_CLASS_SERIAL_BUS            0x0c
#define PCI_CLASS_WIRELESS              0x0d
#define PCI_CLASS_INTELLIGENT_IO        0x0e
#define PCI_CLASS_SATELLITE             0x0f
#define PCI_CLASS_CRYPT                 0x10
#define PCI_CLASS_SIGNAL_PROCESSING     0x11
#define PCI_CLASS_UNDEFINED             0xff

// Undefined Class
#define PCI_UNCLASSIFIED                0x0000
#define PCI_VGA_COMPATIBLE              0x0001

// Mass Storage Controller
#define PCI_STORAGE_SCSI                0x0100
#define PCI_STORAGE_IDE                 0x0101
#define PCI_STORAGE_FLOPPY              0x0102
#define PCI_STORAGE_IPI                 0x0103
#define PCI_STORAGE_RAID                0x0104
#define PCI_STORAGE_ATA                 0x0105
#define PCI_STORAGE_SATA                0x0106
#define PCI_STORAGE_OTHER               0x0180

// Network Controller
#define PCI_NETWORK_ETHERNET            0x0200
#define PCI_NETWORK_TOKEN_RING          0x0201
#define PCI_NETWORK_FDDI                0x0202
#define PCI_NETWORK_ATM                 0x0203
#define PCI_NETWORK_ISDN                0x0204
#define PCI_NETWORK_WORLDFIP            0x0205
#define PCI_NETWORK_PICGMG              0x0206
#define PCI_NETWORK_OTHER               0x0280

// Display Controller
#define PCI_DISPLAY_VGA                 0x0300
#define PCI_DISPLAY_XGA                 0x0301
#define PCI_DISPLAY_3D                  0x0302
#define PCI_DISPLAY_OTHER               0x0380

// Multimedia Controller
#define PCI_MULTIMEDIA_VIDEO            0x0400
#define PCI_MULTIMEDIA_AUDIO            0x0401
#define PCI_MULTIMEDIA_PHONE            0x0402
#define PCI_MULTIMEDIA_AUDIO_DEVICE     0x0403
#define PCI_MULTIMEDIA_OTHER            0x0480

// Memory Controller
#define PCI_MEMORY_RAM                  0x0500
#define PCI_MEMORY_FLASH                0x0501
#define PCI_MEMORY_OTHER                0x0580

// Bridge Device
#define PCI_BRIDGE_HOST                 0x0600
#define PCI_BRIDGE_ISA                  0x0601
#define PCI_BRIDGE_EISA                 0x0602
#define PCI_BRIDGE_MCA                  0x0603
#define PCI_BRIDGE_PCI                  0x0604
#define PCI_BRIDGE_PCMCIA               0x0605
#define PCI_BRIDGE_NUBUS                0x0606
#define PCI_BRIDGE_CARDBUS              0x0607
#define PCI_BRIDGE_RACEWAY              0x0608
#define PCI_BRIDGE_OTHER                0x0680

// Simple Communication Controller
#define PCI_COMM_SERIAL                 0x0700
#define PCI_COMM_PARALLEL               0x0701
#define PCI_COMM_MULTIPORT              0x0702
#define PCI_COMM_MODEM                  0x0703
#define PCI_COMM_GPIB                   0x0704
#define PCI_COMM_SMARTCARD              0x0705
#define PCI_COMM_OTHER                  0x0780

// Base System Peripherals
#define PCI_SYSTEM_PIC                  0x0800
#define PCI_SYSTEM_DMA                  0x0801
#define PCI_SYSTEM_TIMER                0x0802
#define PCI_SYSTEM_RTC                  0x0803
#define PCI_SYSTEM_PCI_HOTPLUG          0x0804
#define PCI_SYSTEM_SD                   0x0805
#define PCI_SYSTEM_OTHER                0x0880

// Input Devices
#define PCI_INPUT_KEYBOARD              0x0900
#define PCI_INPUT_PEN                   0x0901
#define PCI_INPUT_MOUSE                 0x0902
#define PCI_INPUT_SCANNER               0x0903
#define PCI_INPUT_GAMEPORT              0x0904
#define PCI_INPUT_OTHER                 0x0980

// Docking Stations
#define PCI_DOCKING_GENERIC             0x0a00
#define PCI_DOCKING_OTHER               0x0a80

// Processors
#define PCI_PROCESSOR_386               0x0b00
#define PCI_PROCESSOR_486               0x0b01
#define PCI_PROCESSOR_PENTIUM           0x0b02
#define PCI_PROCESSOR_ALPHA             0x0b10
#define PCI_PROCESSOR_POWERPC           0x0b20
#define PCI_PROCESSOR_MIPS              0x0b30
#define PCI_PROCESSOR_CO                0x0b40

// Serial Bus Controllers
#define PCI_SERIAL_FIREWIRE             0x0c00
#define PCI_SERIAL_ACCESS               0x0c01
#define PCI_SERIAL_SSA                  0x0c02
#define PCI_SERIAL_USB                  0x0c03
#define PCI_SERIAL_FIBER                0x0c04
#define PCI_SERIAL_SMBUS                0x0c05

#define PCI_SERIAL_USB_UHCI             0x00
#define PCI_SERIAL_USB_OHCI             0x10
#define PCI_SERIAL_USB_EHCI             0x20
#define PCI_SERIAL_USB_XHCI             0x30
#define PCI_SERIAL_USB_OTHER            0x80

// Wireless Controllers
#define PCI_WIRELESS_IRDA               0x0d00
#define PCI_WIRLESSS_IR                 0x0d01
#define PCI_WIRLESSS_RF                 0x0d10
#define PCI_WIRLESSS_BLUETOOTH          0x0d11
#define PCI_WIRLESSS_BROADBAND          0x0d12
#define PCI_WIRLESSS_ETHERNET_A         0x0d20
#define PCI_WIRLESSS_ETHERNET_B         0x0d21
#define PCI_WIRELESS_OTHER              0x0d80

// Intelligent I/O Controllers
#define PCI_INTELLIGENT_I2O             0x0e00

// Satellite Communication Controllers
#define PCI_SATELLITE_TV                0x0f00
#define PCI_SATELLITE_AUDIO             0x0f01
#define PCI_SATELLITE_VOICE             0x0f03
#define PCI_SATELLITE_DATA              0x0f04

// Encryption/Decryption Controllers
#define PCI_CRYPT_NETWORK               0x1000
#define PCI_CRYPT_ENTERTAINMENT         0x1001
#define PCI_CRYPT_OTHER                 0x1080

// Data Acquisition and Signal Processing Controllers
#define PCI_SP_DPIO                     0x1100
#define PCI_SP_OTHER                    0x1180

const char *PciDeviceName(unsigned int vendorId, unsigned int deviceId);
const char *PciClassName(unsigned int classCode, unsigned int subclass, unsigned int progIntf);
typedef struct PciBar
{
	union
	{
		void *address;
		unsigned short port;
	} u;
	unsigned int size;
	unsigned int flags;
} PciBar;

// ------------------------------------------------------------------------------------------------
typedef struct PciDeviceInfo
{
	unsigned short vendorId;
	unsigned short deviceId;
	unsigned char classCode;
	unsigned char subclass;
	unsigned char progIntf;
} PciDeviceInfo;
typedef struct PciFunction {
	unsigned short vendorid;
	unsigned char bus;
	unsigned char dev;
	unsigned char func;
	unsigned char cclass;
	unsigned char subclass;
} PciFunction;
typedef struct PciDriver
{
	void(*init)(unsigned int id, PciDeviceInfo *info);

} PciDriver;
static void PciReadBar(unsigned int id, unsigned int index, unsigned int *address, unsigned int *mask);
void PciWrite8(unsigned int id, unsigned int reg, unsigned char data);
void PciWrite16(unsigned int id, unsigned int reg, unsigned short data);
unsigned int PciRead32(unsigned int id, unsigned int reg);
unsigned short PciRead16(unsigned int id, unsigned int reg);
unsigned char PciRead8(unsigned int id, unsigned int reg);
void PciWrite32(unsigned int id, unsigned int reg, unsigned int data);

#define ADDR 0xCF8
#define DATA 0xCFC
#define ENABLE (1 << 31)


struct mem_req {
	uint64_t phys_addr, dest_addr;
	size_t len;
};
void PciInit();
static void PciVisit(unsigned int bus, unsigned int dev, unsigned int func);
int pci_scan_bus(struct pci_func *pci_device);
void get_mmio_space_size(struct pci_func *pci_device);
uint32_t ReadWord(unsigned short bus, unsigned short slot, unsigned short func,
	unsigned short offset);
unsigned short pciConfigReadWord(unsigned short bus, unsigned short slot,
	unsigned short func, unsigned short offset);
static inline uint32_t sysInLong(unsigned short port);
static inline void sysOutLong(unsigned short port, uint32_t val);
extern unsigned long long *pages_for_ahci_start;
extern unsigned long long *pages_for_ahci_end;

struct pci_func
{
	uint32_t vendor_id;
	uint32_t device_id;
	uint32_t bus_num;
	uint32_t slot_num;
	uint32_t mmio_reg;
	uint32_t interrupt_reg;
	uint32_t mmio_reg_addr;
	uint32_t mmio_reg_size;
	uint32_t irq_line;
	uint64_t start_virtual_address;
};
void PciGetBar(PciBar *bar, unsigned int id, unsigned int index);


unsigned int
pcibar(PciFunction *f, unsigned char bar);
static unsigned int
pcireadl(unsigned char bus, unsigned char dev, unsigned char func, unsigned char offset);
static void PciReadBar(unsigned int id, unsigned int index, unsigned int *address, unsigned int *mask);








#include "storage/storage.h"
#include "storage/ahci.h"
#include "../usb/usbd.h"
#endif // !_PCI_H