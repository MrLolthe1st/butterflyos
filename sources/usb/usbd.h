#ifndef _USBD_H
#define _USBD_H (1)
#include "../link.h"
#include "../includes/target.h"
#define PACKED __attribute__((__packed__))

#define USB_CLASS_INTF                  0x00
#define USB_CLASS_AUDIO                 0x01
#define USB_CLASS_COMM                  0x02
#define USB_CLASS_HID                   0x03
#define USB_CLASS_PHYSICAL              0x05
#define USB_CLASS_IMAGE                 0x06
#define USB_CLASS_PRINTER               0x07
#define USB_CLASS_STORAGE               0x08
#define USB_CLASS_HUB                   0x09
#define USB_CLASS_CDC_DATA              0x0a
#define USB_CLASS_SMART_CARD            0x0b
#define USB_CLASS_SECURITY              0x0d
#define USB_CLASS_VIDEO                 0x0e
#define USB_CLASS_HEALTHCARE            0x0f
#define USB_CLASS_DIAGNOSTIC            0xdc
#define USB_CLASS_WIRELESS              0xe0
#define USB_CLASS_MISC                  0xef
#define USB_CLASS_APP                   0xfe
#define USB_CLASS_VENDOR                0xff

// ------------------------------------------------------------------------------------------------
// USB Request Type

#define RT_TRANSFER_MASK                0x80
#define RT_DEV_TO_HOST                  0x80
#define RT_HOST_TO_DEV                  0x00

#define RT_TYPE_MASK                    0x60
#define RT_STANDARD                     0x00
#define RT_CLASS                        0x20
#define RT_VENDOR                       0x40

#define RT_RECIPIENT_MASK               0x1f
#define RT_DEV                          0x00
#define RT_INTF                         0x01
#define RT_ENDP                         0x02
#define RT_OTHER                        0x03

// ------------------------------------------------------------------------------------------------
// USB Device Requests

#define REQ_GET_STATUS                  0x00
#define REQ_CLEAR_FEATURE               0x01
#define REQ_SET_FEATURE                 0x03
#define REQ_SET_ADDR                    0x05
#define REQ_GET_DESC                    0x06
#define REQ_SET_DESC                    0x07
#define REQ_GET_CONF                    0x08
#define REQ_SET_CONF                    0x09
#define REQ_GET_INTF                    0x0a
#define REQ_SET_INTF                    0x0b
#define REQ_SYNC_FRAME                  0x0c

// ------------------------------------------------------------------------------------------------
// USB Hub Class Requests

#define REQ_CLEAR_TT_BUFFER             0x08
#define REQ_RESET_TT                    0x09
#define REQ_GET_TT_STATE                0x0a
#define REQ_STOP_TT                     0x0b

// ------------------------------------------------------------------------------------------------
// USB HID Interface Requests

#define REQ_GET_REPORT                  0x01
#define REQ_GET_IDLE                    0x02
#define REQ_GET_PROTOCOL                0x03
#define REQ_SET_REPORT                  0x09
#define REQ_SET_IDLE                    0x0a
#define REQ_SET_PROTOCOL                0x0b

// ------------------------------------------------------------------------------------------------
// USB Standard Feature Selectors

#define F_DEVICE_REMOTE_WAKEUP          1   // Device
#define F_ENDPOINT_HALT                 2   // Endpoint
#define F_TEST_MODE                     3   // Device

// ------------------------------------------------------------------------------------------------
// USB Hub Feature Seletcors

#define F_C_HUB_LOCAL_POWER             0   // Hub
#define F_C_HUB_OVER_CURRENT            1   // Hub
#define F_PORT_CONNECTION               0   // Port
#define F_PORT_ENABLE                   1   // Port
#define F_PORT_SUSPEND                  2   // Port
#define F_PORT_OVER_CURRENT             3   // Port
#define F_PORT_RESET                    4   // Port
#define F_PORT_POWER                    8   // Port
#define F_PORT_LOW_SPEED                9   // Port
#define F_C_PORT_CONNECTION             16  // Port
#define F_C_PORT_ENABLE                 17  // Port
#define F_C_PORT_SUSPEND                18  // Port
#define F_C_PORT_OVER_CURRENT           19  // Port
#define F_C_PORT_RESET                  20  // Port
#define F_PORT_TEST                     21  // Port
#define F_PORT_INDICATOR                22  // Port

// ------------------------------------------------------------------------------------------------
// USB Device Request

typedef struct UsbDevReq
{
	unsigned char type;
	unsigned char req;
	unsigned short value;
	unsigned short index;
	unsigned short len;
}  UsbDevReq;
// ------------------------------------------------------------------------------------------------
// USB HID Subclass Codes

#define USB_SUBCLASS_BOOT               0x01

// ------------------------------------------------------------------------------------------------
// USB HID Protocol Codes

#define USB_PROTOCOL_KBD                0x01
#define USB_PROTOCOL_MOUSE              0x02
#define USB_DESC_DEVICE                 0x01
#define USB_DESC_CONF                   0x02
#define USB_DESC_STRING                 0x03
#define USB_DESC_INTF                   0x04
#define USB_DESC_ENDP                   0x05

// ------------------------------------------------------------------------------------------------
// USB HID Descriptor Types

#define USB_DESC_HID                    0x21
#define USB_DESC_REPORT                 0x22
#define USB_DESC_PHYSICAL               0x23

// ------------------------------------------------------------------------------------------------
// USB HUB Descriptor Types

#define USB_DESC_HUB                    0x29

// ------------------------------------------------------------------------------------------------
// USB Device Descriptor

typedef struct UsbDeviceDesc
{
	unsigned char len;
	unsigned char type;
	unsigned short usbVer;
	unsigned char devClass;
	unsigned char devSubClass;
	unsigned char devProtocol;
	unsigned char maxPacketSize;
	unsigned short vendorId;
	unsigned short productId;
	unsigned short deviceVer;
	unsigned char vendorStr;
	unsigned char productStr;
	unsigned char serialStr;
	unsigned char confCount;
} PACKED UsbDeviceDesc;

// ------------------------------------------------------------------------------------------------
// USB Configuration Descriptor

typedef struct UsbConfDesc
{
	unsigned char len;
	unsigned char type;
	unsigned short totalLen;
	unsigned char intfCount;
	unsigned char confValue;
	unsigned char confStr;
	unsigned char attributes;
	unsigned char maxPower;
} PACKED UsbConfDesc;

// ------------------------------------------------------------------------------------------------
// USB String Descriptor

typedef struct UsbStringDesc
{
	unsigned char len;
	unsigned char type;
	unsigned short str[];
} PACKED UsbStringDesc;

// ------------------------------------------------------------------------------------------------
// USB Interface Descriptor

typedef struct UsbEndpDesc
{
	unsigned char len;
	unsigned char type;
	unsigned char addr;
	unsigned char attributes;
	unsigned short maxPacketSize;
	unsigned char interval;
	struct UsbEndpDesc * next;
} PACKED UsbEndpDesc;

typedef struct __attribute__((packed))  UsbIntfDesc
{
	unsigned char len;
	unsigned char type;
	unsigned char intfIndex;
	unsigned char altSetting;
	unsigned char endpCount;
	unsigned char intfClass;
	unsigned char intfSubClass;
	unsigned char intfProtocol;
	unsigned char intfStr;
	struct UsbIntfDesc * next;
	UsbEndpDesc * endpoints;
} PACKED UsbIntfDesc;

// ------------------------------------------------------------------------------------------------
// USB Endpoint Descriptor

// ------------------------------------------------------------------------------------------------
// USB HID Desciptor

typedef struct __attribute__((packed))  UsbHidDesc
{
	unsigned char len;
	unsigned char type;
	unsigned short hidVer;
	unsigned char countryCode;
	unsigned char descCount;
	unsigned char descType;
	unsigned short descLen;
} PACKED UsbHidDesc;

// ------------------------------------------------------------------------------------------------
// USB Hub Descriptor

typedef struct __attribute__((packed)) UsbHubDesc
{
	unsigned char len;
	unsigned char type;
	unsigned char portCount;
	unsigned short chars;
	unsigned char portPowerTime;
	unsigned char current;
	// removable/power control bits vary in size
} PACKED UsbHubDesc;

// Hub Characteristics
#define HUB_POWER_MASK                  0x03        // Logical Power Switching Mode
#define HUB_POWER_GLOBAL                0x00
#define HUB_POWER_INDIVIDUAL            0x01
#define HUB_COMPOUND                    0x04        // Part of a Compound Device
#define HUB_CURRENT_MASK                0x18        // Over-current Protection Mode
#define HUB_TT_TTI_MASK                 0x60        // TT Think Time
#define HUB_PORT_INDICATORS             0x80        // Port Indicators


#define USB_STRING_SIZE                 127



// ------------------------------------------------------------------------------------------------
// USB Speeds

#define USB_FULL_SPEED                  0x00
#define USB_LOW_SPEED                   0x01
#define USB_HIGH_SPEED                  0x02
typedef struct UsbController
{
	struct UsbController *next;
	void *hc;

	void(*poll)(struct UsbController *controller);
} UsbController;

// ------------------------------------------------------------------------------------------------
// Globals

extern UsbController *g_usbControllerList;

// ------------------------------------------------------------------------------------------------
// USB Endpoint

typedef struct UsbEndpoint
{
	UsbEndpDesc * desc;
	unsigned int toggle;
} UsbEndpoint;

// ------------------------------------------------------------------------------------------------
// USB Transfer

typedef struct UsbTransfer
{
	UsbEndpoint *endp;
	UsbDevReq *req;
	void *data;
	size_t len;
	bool complete;
	bool success;
	char w;
} UsbTransfer;

// ------------------------------------------------------------------------------------------------
// USB Device

typedef struct UsbDevice
{
	struct UsbDevice *parent;
	struct UsbDevice *next;
	void *hc;
	void *drv;

	unsigned int port;
	unsigned int speed;
	unsigned int addr;
	unsigned int maxPacketSize;

	UsbIntfDesc *intfDesc;

	void(*hcControl)(struct UsbDevice *dev, UsbTransfer *t);
	void(*hcIntr)(struct UsbDevice *dev, UsbTransfer *t);
	void(*drvPoll)(struct UsbDevice *dev);
	void(*onDisconnect)(struct UsbDevice *dev);

	unsigned short langs[USB_STRING_SIZE];
	char productStr[USB_STRING_SIZE];
	char vendorStr[USB_STRING_SIZE];
	char serialStr[USB_STRING_SIZE];
} UsbDevice;
typedef struct UsbDriver
{
	bool(*init)(UsbDevice *dev);
} UsbDriver;


extern int s_nextUsbAddr;
extern UsbDevice *g_usbDeviceList;
void UsbService();
void UsbPoll();
 bool UsbDevInit(UsbDevice *dev);
UsbDevice *UsbDevCreate();
bool UsbDevRequest(UsbDevice *dev,
	unsigned int type, unsigned int request,
	unsigned int value, unsigned int index,
	unsigned int len, void *data);
#include "ehci.h"
#include "hub.h"
#endif