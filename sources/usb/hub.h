#ifndef _HUB_H
#define _HUB_H (1)
#include "usbd.h"

// ------------------------------------------------------------------------------------------------
typedef struct UsbHub
{
	UsbDevice *dev;
	UsbHubDesc desc;
} UsbHub;
unsigned int UsbHubResetPort(UsbHub *hub, unsigned int port);
void UsbHubProbe(UsbHub *hub);
void UsbHubPoll(UsbDevice *dev);
bool usb_hub_init(UsbDevice *dev);

#endif