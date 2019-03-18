#include "includes/PCI.h"
#include "includes/video.h"
// ------------------------------------------------------------------------------------------------
const char *PciDeviceName(unsigned int vendorId, unsigned int deviceId)
{
	return "Unknown Device";
}

// ------------------------------------------------------------------------------------------------
const char *PciClassName(unsigned int classCode, unsigned int subclass, unsigned int progIntf)
{
	switch ((classCode << 8) | subclass)
	{
	case PCI_VGA_COMPATIBLE:            return "VGA-Compatible Device";
	case PCI_STORAGE_SCSI:              return "SCSI Storage Controller";
	case PCI_STORAGE_IDE:               return "IDE Interface";
	case PCI_STORAGE_FLOPPY:            return "Floppy Disk Controller";
	case PCI_STORAGE_IPI:               return "IPI Bus Controller";
	case PCI_STORAGE_RAID:              return "RAID Bus Controller";
	case PCI_STORAGE_ATA:               return "ATA Controller";
	case PCI_STORAGE_SATA:              return "SATA Controller";
	case PCI_STORAGE_OTHER:             return "Mass Storage Controller";
	case PCI_NETWORK_ETHERNET:          return "Ethernet Controller";
	case PCI_NETWORK_TOKEN_RING:        return "Token Ring Controller";
	case PCI_NETWORK_FDDI:              return "FDDI Controller";
	case PCI_NETWORK_ATM:               return "ATM Controller";
	case PCI_NETWORK_ISDN:              return "ISDN Controller";
	case PCI_NETWORK_WORLDFIP:          return "WorldFip Controller";
	case PCI_NETWORK_PICGMG:            return "PICMG Controller";
	case PCI_NETWORK_OTHER:             return "Network Controller";
	case PCI_DISPLAY_VGA:               return "VGA-Compatible Controller";
	case PCI_DISPLAY_XGA:               return "XGA-Compatible Controller";
	case PCI_DISPLAY_3D:                return "3D Controller";
	case PCI_DISPLAY_OTHER:             return "Display Controller";
	case PCI_MULTIMEDIA_VIDEO:          return "Multimedia Video Controller";
	case PCI_MULTIMEDIA_AUDIO:          return "Multimedia Audio Controller";
	case PCI_MULTIMEDIA_PHONE:          return "Computer Telephony Device";
	case PCI_MULTIMEDIA_AUDIO_DEVICE:   return "Audio Device";
	case PCI_MULTIMEDIA_OTHER:          return "Multimedia Controller";
	case PCI_MEMORY_RAM:                return "RAM Memory";
	case PCI_MEMORY_FLASH:              return "Flash Memory";
	case PCI_MEMORY_OTHER:              return "Memory Controller";
	case PCI_BRIDGE_HOST:               return "Host Bridge";
	case PCI_BRIDGE_ISA:                return "ISA Bridge";
	case PCI_BRIDGE_EISA:               return "EISA Bridge";
	case PCI_BRIDGE_MCA:                return "MicroChannel Bridge";
	case PCI_BRIDGE_PCI:                return "PCI Bridge";
	case PCI_BRIDGE_PCMCIA:             return "PCMCIA Bridge";
	case PCI_BRIDGE_NUBUS:              return "NuBus Bridge";
	case PCI_BRIDGE_CARDBUS:            return "CardBus Bridge";
	case PCI_BRIDGE_RACEWAY:            return "RACEway Bridge";
	case PCI_BRIDGE_OTHER:              return "Bridge Device";
	case PCI_COMM_SERIAL:               return "Serial Controller";
	case PCI_COMM_PARALLEL:             return "Parallel Controller";
	case PCI_COMM_MULTIPORT:            return "Multiport Serial Controller";
	case PCI_COMM_MODEM:                return "Modem";
	case PCI_COMM_OTHER:                return "Communication Controller";
	case PCI_SYSTEM_PIC:                return "PIC";
	case PCI_SYSTEM_DMA:                return "DMA Controller";
	case PCI_SYSTEM_TIMER:              return "Timer";
	case PCI_SYSTEM_RTC:                return "RTC";
	case PCI_SYSTEM_PCI_HOTPLUG:        return "PCI Hot-Plug Controller";
	case PCI_SYSTEM_SD:                 return "SD Host Controller";
	case PCI_SYSTEM_OTHER:              return "System Peripheral";
	case PCI_INPUT_KEYBOARD:            return "Keyboard Controller";
	case PCI_INPUT_PEN:                 return "Pen Controller";
	case PCI_INPUT_MOUSE:               return "Mouse Controller";
	case PCI_INPUT_SCANNER:             return "Scanner Controller";
	case PCI_INPUT_GAMEPORT:            return "Gameport Controller";
	case PCI_INPUT_OTHER:               return "Input Controller";
	case PCI_DOCKING_GENERIC:           return "Generic Docking Station";
	case PCI_DOCKING_OTHER:             return "Docking Station";
	case PCI_PROCESSOR_386:             return "386";
	case PCI_PROCESSOR_486:             return "486";
	case PCI_PROCESSOR_PENTIUM:         return "Pentium";
	case PCI_PROCESSOR_ALPHA:           return "Alpha";
	case PCI_PROCESSOR_MIPS:            return "MIPS";
	case PCI_PROCESSOR_CO:              return "CO-Processor";
	case PCI_SERIAL_FIREWIRE:           return "FireWire (IEEE 1394)";
	case PCI_SERIAL_SSA:                return "SSA";
	case PCI_SERIAL_USB:
		switch (progIntf)
		{
		case PCI_SERIAL_USB_UHCI:       return "USB (UHCI)";
		case PCI_SERIAL_USB_OHCI:       return "USB (OHCI)";
		case PCI_SERIAL_USB_EHCI:       return "USB2";
		case PCI_SERIAL_USB_XHCI:       return "USB3";
		case PCI_SERIAL_USB_OTHER:      return "USB Controller";
		default:                        return "Unknown USB Class";
		}
		break;
	case PCI_SERIAL_FIBER:              return "Fiber Channel";
	case PCI_SERIAL_SMBUS:              return "SMBus";
	case PCI_WIRELESS_IRDA:             return "iRDA Compatible Controller";
	case PCI_WIRLESSS_IR:               return "Consumer IR Controller";
	case PCI_WIRLESSS_RF:               return "RF Controller";
	case PCI_WIRLESSS_BLUETOOTH:        return "Bluetooth";
	case PCI_WIRLESSS_BROADBAND:        return "Broadband";
	case PCI_WIRLESSS_ETHERNET_A:       return "802.1a Controller";
	case PCI_WIRLESSS_ETHERNET_B:       return "802.1b Controller";
	case PCI_WIRELESS_OTHER:            return "Wireless Controller";
	case PCI_INTELLIGENT_I2O:           return "I2O Controller";
	case PCI_SATELLITE_TV:              return "Satellite TV Controller";
	case PCI_SATELLITE_AUDIO:           return "Satellite Audio Controller";
	case PCI_SATELLITE_VOICE:           return "Satellite Voice Controller";
	case PCI_SATELLITE_DATA:            return "Satellite Data Controller";
	case PCI_CRYPT_NETWORK:             return "Network and Computing Encryption Device";
	case PCI_CRYPT_ENTERTAINMENT:       return "Entertainment Encryption Device";
	case PCI_CRYPT_OTHER:               return "Encryption Device";
	case PCI_SP_DPIO:                   return "DPIO Modules";
	case PCI_SP_OTHER:                  return "Signal Processing Controller";
	}

	return "Unknown PCI Class";
}

#define uint unsigned int
// ------------------------------------------------------------------------------------------------
void PciWrite32(unsigned int id, unsigned int reg, unsigned int data)
{
	unsigned int address = 0x80000000 | id | (reg & 0xfc);
	outportd(PCI_CONFIG_ADDR, address);
	outportd(PCI_CONFIG_DATA, data);
}

unsigned char PciRead8(unsigned int id, unsigned int reg)
{
	unsigned int addr = 0x80000000 | id | (reg & 0xfc);
	outportd(PCI_CONFIG_ADDR, addr);
	return inportb(PCI_CONFIG_DATA + (reg & 0x03));
}

// ------------------------------------------------------------------------------------------------
unsigned short PciRead16(unsigned int id, unsigned int reg)
{
	unsigned int addr = 0x80000000 | id | (reg & 0xfc);
	outportd(PCI_CONFIG_ADDR, addr);
	return inportw(PCI_CONFIG_DATA + (reg & 0x02));
}

// ------------------------------------------------------------------------------------------------
unsigned int PciRead32(unsigned int id, unsigned int reg)
{
	unsigned int addr = 0x80000000 | id | (reg & 0xfc);
	outportd(PCI_CONFIG_ADDR, addr);
	return inportd(PCI_CONFIG_DATA);
}
// ------------------------------------------------------------------------------------------------
void PciWrite16(unsigned int id, unsigned int reg, unsigned short data)
{
	unsigned int address = 0x80000000 | id | (reg & 0xfc);
	outportd(PCI_CONFIG_ADDR, address);
	outportw(PCI_CONFIG_DATA + (reg & 0x02), data);
}

// ------------------------------------------------------------------------------------------------
void PciWrite8(unsigned int id, unsigned int reg, unsigned char data)
{
	unsigned int address = 0x80000000 | id | (reg & 0xfc);
	outportd(PCI_CONFIG_ADDR, address);
	outportb(PCI_CONFIG_DATA + (reg & 0x03), data);
}




// ------------------------------------------------------------------------------------------------
static void PciReadBar(unsigned int id, unsigned int index, unsigned int *address, unsigned int *mask)
{
	unsigned int reg = PCI_CONFIG_BAR0 + index * sizeof(unsigned int);

	// Get address
	*address = PciRead32(id, reg);

	// Find out size of the bar
	PciWrite32(id, reg, 0xffffffff);
	*mask = PciRead32(id, reg);

	// Restore adddress
	PciWrite32(id, reg, *address);
}
// ------------------------------------------------------------------------------------------------
static uint
pcireadl(unsigned char bus, unsigned  char dev, unsigned char func, unsigned char offset)
{
	unsigned long addr;

	addr = ENABLE | (bus << 16) | (dev << 11) | (func << 8) | (offset & 0xFC);

	outportd(ADDR, addr);
	return inportd(DATA);
}

struct pci_func pci_e1000, pci_ahci;

uint
pcibar(PciFunction *f, unsigned char bar)
{
	return pcireadl(f->bus, f->dev, f->func, 0x10 + 4 * bar);
}
// ------------------------------------------------------------------------------------------------
void PciGetBar(PciBar *bar, unsigned int id, unsigned int index)
{
	// Read pci bar register
	unsigned int addressLow;
	unsigned int maskLow;
	PciReadBar(id, index, &addressLow, &maskLow);

	if (addressLow & PCI_BAR_64)
	{
		// 64-bit mmio
		unsigned int addressHigh;
		unsigned int maskHigh;
		PciReadBar(id, index + 1, &addressHigh, &maskHigh);

		bar->u.address = (void *)((0) | (addressLow & ~0xf));
		bar->size = ~((0) | (maskLow & ~0xf)) + 1;
		bar->flags = addressLow & 0xf;
	}
	else if (addressLow & PCI_BAR_IO)
	{
		// i/o register
		bar->u.port = (unsigned short)(addressLow & ~0x3);
		bar->size = (unsigned short)(~(maskLow & ~0x3) + 1);
		bar->flags = addressLow & 0x3;
	}
	else
	{
		// 32-bit mmio
		bar->u.address = (void *)(addressLow & ~0xf);
		bar->size = ~(maskLow & ~0xf) + 1;
		bar->flags = addressLow & 0xf;
	}
}


unsigned long long *pages_for_ahci_start;
unsigned long long *pages_for_ahci_end;

// ------------------------------------------------------------------------------------------------
static inline void sysOutLong(unsigned short port, uint32_t val)
{
	__asm__("outl %0, %1"
		: : "a"(val), "Nd"(port));
}

// ------------------------------------------------------------------------------------------------
static inline uint32_t sysInLong(unsigned short port)
{
	uint32_t ret;
	__asm__("inl %1, %0"
		: "=a"(ret) : "Nd"(port));
	return ret;
}

// ------------------------------------------------------------------------------------------------
unsigned short pciConfigReadWord(unsigned short bus, unsigned short slot,
	unsigned short func, unsigned short offset)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	unsigned short tmp = 0;
	address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8)
		| (offset & 0xfc) | ((uint32_t)0x80000000));
	sysOutLong(0xCF8, address);
	tmp = (unsigned short)((sysInLong(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	return (tmp);
}

// ------------------------------------------------------------------------------------------------
uint32_t ReadWord(unsigned short bus, unsigned short slot, unsigned short func,
	unsigned short offset)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint32_t tmp = 0;
	address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8)
		| (offset & 0xfc) | ((uint32_t)0x80000000));
	sysOutLong(0xCF8, address);
	tmp = (uint32_t)(sysInLong(0xCFC) /* & 0xffff*/);
	return (tmp);
}
#define AHCI_MMIO_BUFFER 0xfebf0000
#define e1000_MMIO_BUFFER 0xfebc0000

// ------------------------------------------------------------------------------------------------
void get_mmio_space_size(struct pci_func *pci_device)
{
	uint32_t address;
	uint32_t lbus = pci_device->bus_num;
	uint32_t lslot = pci_device->slot_num;
	uint32_t lfunc = 0;
	uint32_t tmp = 0;
	unsigned short offset = (unsigned short)(pci_device->mmio_reg | 0x0);
	address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8)
		| (offset & 0xfc) | ((uint32_t)0x80000000));
	pci_device->start_virtual_address = address;
	sysOutLong(0xCF8, address);
	sysOutLong(0xCFC, 0xffffffff);
	tmp = (ReadWord(pci_device->bus_num, pci_device->slot_num, 0,
		(pci_device->mmio_reg | 0x0)));
	pci_device->mmio_reg_size = (~tmp) + 1;
	sysOutLong(0xCF8, address);
	sysOutLong(0xCFC, pci_device->mmio_reg_addr);
}

int pci_scan_bus(struct pci_func *pci_device)
{
	int bus;
	int slot;
	unsigned short vendor, device;
	for (bus = 0; bus < 256; bus++)
	{
		for (slot = 0; slot < 32; slot++)
		{
			unsigned int id = PCI_MAKE_ID(bus, slot, 0);
			vendor = PciRead8(id, PCI_CONFIG_CLASS_CODE);
			device = PciRead8(id, PCI_CONFIG_SUBCLASS);
			if (vendor == 0x1
				&& device == 0x06)
			{
				pci_device->bus_num = bus;
				pci_device->slot_num = slot;
				pci_device->mmio_reg_addr = ReadWord(bus, slot, 0,
					(pci_device->mmio_reg | 0x0));
				pci_device->irq_line = (ReadWord(bus, slot, 0,
					(pci_device->interrupt_reg | 0x0)) & 0xff);
				return 1;
			}
		}
	}
	return 0;
}
unsigned int iddd;
// ------------------------------------------------------------------------------------------------
static void PciVisit(unsigned int bus, unsigned int dev, unsigned int func)
{


	unsigned int id = PCI_MAKE_ID(bus, dev, func);

	PciDeviceInfo info;
	PciFunction p;
	info.vendorId = PciRead16(id, PCI_CONFIG_VENDOR_ID);

	if (info.vendorId == 0xffff)
	{
		return;
	}

	info.deviceId = PciRead16(id, PCI_CONFIG_DEVICE_ID);
	info.progIntf = PciRead8(id, PCI_CONFIG_PROG_INTF);
	info.subclass = PciRead8(id, PCI_CONFIG_SUBCLASS);
	info.classCode = PciRead8(id, PCI_CONFIG_CLASS_CODE);
	if (info.classCode == 0x1 && info.subclass == 0x6)
	{
		pci_ahci.vendor_id = info.vendorId;
		pci_ahci.device_id = info.deviceId;
		pci_ahci.bus_num = bus;
		pci_ahci.slot_num = dev;
		pci_ahci.mmio_reg_addr = PciRead16(id,
			(pci_ahci.mmio_reg | 0x0));
		pci_ahci.irq_line = (PciRead16(id,
			(pci_ahci.interrupt_reg | 0x0)) & 0xff);
		iddd = id;
	}
	p.cclass = info.classCode;
	p.subclass = info.subclass;
	p.bus = bus;
	p.vendorid = info.vendorId;
	p.dev = dev;
	p.func = func;
	if (info.classCode == 0x01 && info.subclass == 0x01 &&
		(info.progIntf == 0x8A || info.progIntf == 0x80)) {

		outportd((1 << 31) | (bus << 16) | (dev << 11)
			| (func << 8) | 8, 0xCF8); // Send the parameters.
		if ((inportd(0xCFC) >> 16) != 0xFFFF) { // If device exists (class isn't 0xFFFF)
												// Check if this device needs an IRQ assignment:
			outportd((1 << 31) | (bus << 16) | (dev << 11)
				| (func << 8) | 0x3C, 0xCF8); // Read the interrupt line field
			outportb(0xCFC, 0xFE); // Change the IRQ field to 0xFE
			outportd((1 << 31) | (bus << 16) | (dev << 11)
				| (func << 8) | 0x3C, 0xCF8); // Read the interrupt line field
			if ((inportd(0xCFC) & 0xFF) == 0xFE) {
				// This device needs an IRQ assignment.
			}
			else {
				// The device doesn't use IRQs, check if this is an Parallel IDE:
					// This is a Parallel IDE Controller which uses IRQs 14 and 15.
				//inst(0x3E, &irq_ideMast, 0x8e);
				//inst(0x3E, &irq_ideSlave, 0x8e);
				//ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
			}
		}
	}
	_ehci_init(id, &info);
	/*print_int(info.classCode, 10);
	print_char(':');
	print_int(info.subclass, 10);
	print_char(':');

	print_string(PciClassName(info.classCode, info.subclass, info.progIntf));
	print_string("\n");
	**//*_ehci_init(id, &info);
	EthIntelInit(id, &info);
	_uhci_init(id, &info);*/
	//_rtl39_init(id, &info);
}
void _probe_port(void *abar_temp);

// ------------------------------------------------------------------------------------------------
void PciInit()
{
	//print_string("PCI initialization...\n");
	for (unsigned int bus = 0; bus < 256; ++bus)
	{
		for (unsigned int dev = 0; dev < 32; ++dev)
		{
			unsigned int baseId = PCI_MAKE_ID(bus, dev, 0);
			unsigned char headerType = PciRead8(baseId, PCI_CONFIG_HEADER_TYPE);
			unsigned int funcCount = headerType & PCI_TYPE_MULTIFUNC ? 8 : 1;

			for (unsigned int func = 0; func < funcCount; ++func)
			{
				PciVisit(bus, dev, func);
			}
		}
	}
	unsigned int q = PciRead32(iddd, 0x24);
	ahci_probe_port((void*)q);
}

