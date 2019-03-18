#include "usbd.h"
#include "../includes/memory/memory.h"
int s_nextUsbAddr;
UsbDevice *g_usbDeviceList;
UsbController *g_usbControllerList;

// ------------------------------------------------------------------------------------------------
UsbDevice *UsbDevCreate()
{
	// Initialize structure
	UsbDevice *dev = (UsbDevice*)malloc(sizeof(UsbDevice));
	if (dev)
	{
		dev->parent = 0;
		dev->next = g_usbDeviceList;
		dev->hc = 0;
		dev->drv = 0;

		dev->port = 0;
		dev->speed = 0;
		dev->addr = 0;
		dev->maxPacketSize = 0;
		dev->hcControl = 0;
		dev->hcIntr = 0;
		dev->drvPoll = 0;

		g_usbDeviceList = dev;
	}

	return dev;
}

// ------------------------------------------------------------------------------------------------
bool UsbIntrRequest(UsbDevice *dev,
	unsigned int type, unsigned int request,
	unsigned int value, unsigned int index,
	unsigned int len, void *data)
{
	UsbDevReq req;
	req.type = type;
	req.req = request;
	req.value = value;
	req.index = index;
	req.len = len;

	UsbTransfer t;
	t.endp = 0;
	t.req = &req;
	//t.toggle
	t.data = data;
	t.len = len;
	t.complete = false;
	t.success = false;

	dev->hcControl(dev, &t);

	return t.success;
}

// ------------------------------------------------------------------------------------------------
bool UsbEndpqRequest(UsbDevice *dev,
	unsigned int type, unsigned int request,
	unsigned int value, unsigned int index,
	unsigned int len, void *data)
{
	UsbDevReq req;
	req.type = type;
	req.req = request;
	req.value = value;
	req.index = index;
	req.len = len;

	UsbTransfer t;
	t.endp = 0;
	t.req = &req;
	t.data = data;
	t.len = len;
	t.complete = false;
	t.success = false;

	dev->hcIntr(dev, &t);

	return t.success;
}
// ------------------------------------------------------------------------------------------------
bool UsbDevRequest(UsbDevice *dev,
	unsigned int type, unsigned int request,
	unsigned int value, unsigned int index,
	unsigned int len, void *data)
{
	UsbDevReq req;
	req.type = type;
	req.req = request;
	req.value = value;
	req.index = index;
	req.len = len;

	UsbTransfer t;
	t.endp = 0;
	t.req = &req;
	t.data = data;
	t.len = len;
	t.complete = false;
	t.success = false;

	dev->hcControl(dev, &t);

	return t.success;
}

// ------------------------------------------------------------------------------------------------
bool UsbDevGetLangs(UsbDevice *dev, u16 *langs)
{
	langs[0] = 0;

	u8 buf[256];
	UsbStringDesc *desc = (UsbStringDesc *)&buf;

	// Get length
	if (!UsbDevRequest(dev,
		RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
		REQ_GET_DESC, (USB_DESC_STRING << 8) | 0, 0,
		1, desc))
	{
		return false;
	}
	// Get lang data
	if (!UsbDevRequest(dev,
		RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
		REQ_GET_DESC, (USB_DESC_STRING << 8) | 0, 0,
		desc->len, desc))
	{
		return false;
	}


	unsigned int langLen = (desc->len - 2) / 2;
	for (unsigned int i = 0; i < langLen; ++i)
	{
		langs[i] = desc->str[i];
	}
	langs[langLen] = 0;
	return true;
}

// ------------------------------------------------------------------------------------------------
bool UsbDevGetString(UsbDevice *dev, char *str, unsigned int langId, unsigned int strIndex)
{
	str[0] = '\0';
	u8 buf[256];
	UsbStringDesc *desc = (UsbStringDesc *)buf;

	// Get string length
	if (!UsbDevRequest(dev,
		RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
		REQ_GET_DESC, (USB_DESC_STRING << 8) | strIndex, langId,
		1, desc))
	{
		return false;
	}

	// Get string data
	if (!UsbDevRequest(dev,
		RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
		REQ_GET_DESC, (USB_DESC_STRING << 8) | strIndex, langId,
		desc->len, desc))
	{
		return false;
	}

	// Dumb Unicode to ASCII conversion
	unsigned int strLen = (desc->len - 2) / 2;
	for (unsigned int i = 0; i < strLen; ++i)
	{
		str[i] = desc->str[i];
	}
	str[strLen] = '\0';
	return true;
}

// ------------------------------------------------------------------------------------------------
bool UsbDevClearHalt(UsbDevice *dev)
{

}

#include "../includes/interrupts.h"
// ------------------------------------------------------------------------------------------------
bool UsbDevInit(UsbDevice *dev)
{
	// Get first 8 bytes of device descriptor
	UsbDeviceDesc devDesc;
	if (!UsbDevRequest(dev,
		RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
		REQ_GET_DESC, (USB_DESC_DEVICE << 8) | 0, 0,
		8, &devDesc))
	{
		return false;
	}//UsbPrintDeviceDesc(&devDesc);
	dev->maxPacketSize = devDesc.maxPacketSize;
	// Set address
	unsigned int addr = ++s_nextUsbAddr;
	if (!UsbDevRequest(dev,
		RT_HOST_TO_DEV | RT_STANDARD | RT_DEV,
		REQ_SET_ADDR, addr, 0,
		0, 0))
	{
		return false;
	}

	dev->addr = addr;
	Wait(102);    // Set address recovery time

		// Read entire descriptor
	if (!UsbDevRequest(dev,
		RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
		REQ_GET_DESC, (USB_DESC_DEVICE << 8) | 0, 0,
		sizeof(UsbDeviceDesc), &devDesc))
	{
		return false;
	}

	// Dump descriptor
	//UsbPrintDeviceDesc(&devDesc);

	// String Info
	UsbDevGetLangs(dev, (short unsigned int*)&dev->langs);

	unsigned int langId = dev->langs[0];

	//printTextToWindow(1, mywin, "%x",(int)langs[0]);
	if (langId)
	{
		UsbDevGetString(dev, (char*)&dev->productStr, langId, devDesc.productStr);
		UsbDevGetString(dev, (char*)&dev->vendorStr, langId, devDesc.vendorStr);
		UsbDevGetString(dev, (char*)&dev->serialStr, langId, devDesc.serialStr);
		printf("Product='%s' Vendor='%s' Serial=%s\n", &dev->productStr, &dev->vendorStr, &dev->serialStr);
	}
	// Pick configuration and interface - grab first for now
	u8 configBuf[256];
	unsigned int pickedConfValue = 0;
	UsbIntfDesc *pickedIntfDesc = 0;
	UsbEndpDesc *pickedEndpDesc = 0;
	for (unsigned int confIndex = 0; confIndex < devDesc.confCount; ++confIndex)
	{
		// Get configuration total length
		if (!UsbDevRequest(dev,
			RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
			REQ_GET_DESC, (USB_DESC_CONF << 8) | confIndex, 0,
			4, configBuf))
		{
			continue;
		}

		// Only static size supported for now
		UsbConfDesc *confDesc = (UsbConfDesc *)configBuf;
		if (confDesc->totalLen > sizeof(configBuf))
		{
			printf("  Configuration length %d greater than %d bytes",
				confDesc->totalLen, sizeof(configBuf));


			continue;
		}

		// Read all configuration data
		if (!UsbDevRequest(dev,
			RT_DEV_TO_HOST | RT_STANDARD | RT_DEV,
			REQ_GET_DESC, (USB_DESC_CONF << 8) | confIndex, 0,
			confDesc->totalLen, configBuf))
		{
			continue;
		}

		//UsbPrintConfDesc(confDesc);

		if (!pickedConfValue)
		{
			pickedConfValue = confDesc->confValue;
		}

		// Parse configuration data
		u8 *data = configBuf + confDesc->len;
		u8 *end = configBuf + confDesc->totalLen;

		while (data < end)
		{
			u8 len = data[0];
			u8 type = data[1];
			//Assume, that data are sorted like:
			//..., Interface, endp, endp, ..., interface, endp, endp, ...
			switch (type)
			{
			case USB_DESC_INTF:
			{
				//printTextToWindow(1, mywin, "  De:\n");
				UsbIntfDesc *intfDesc = (UsbIntfDesc*)malloc(sizeof(UsbIntfDesc));
				memcpy(intfDesc, data, sizeof(UsbIntfDesc) - 8);
				intfDesc->next = 0;
				intfDesc->endpoints = 0;
				if (!pickedIntfDesc)
				{
					dev->intfDesc = intfDesc;
					//Set first

				}
				else {
					//Set next to last;
					UsbIntfDesc *last = dev->intfDesc;
					while (last->next) last = last->next;
					last->next = intfDesc;
				}
				pickedIntfDesc = intfDesc;
			}
			break;

			case USB_DESC_ENDP:
			{
				//Currently I'm checking last interface, add that endpoint to it.
				UsbEndpDesc *endp_desc = (UsbEndpDesc*)malloc(sizeof(UsbEndpDesc));
				memcpy(endp_desc, data, sizeof(UsbEndpDesc) - 4);
				endp_desc->next = 0;
				//UsbPrintEndpDesc(endp_desc);

				if (!pickedIntfDesc)
				{
					//Wtf is going on????
				//	printTextToWindow(4, mywin, "WTF??? We aren't receive interface first!");
					return 0;
					//pickedEndpDesc = endp_desc;
				}
				if (pickedEndpDesc)
				{
					pickedEndpDesc->next = endp_desc;
					pickedEndpDesc = endp_desc;
				}
				else
				{
					pickedIntfDesc->endpoints = endp_desc;
				}
				pickedEndpDesc = endp_desc;
			}
			break;
			}

			data += len;
		}
	}
	// Configure device
	if (pickedConfValue)
	{
		if (!UsbDevRequest(dev,
			RT_HOST_TO_DEV | RT_STANDARD | RT_DEV,
			REQ_SET_CONF, pickedConfValue, 0,
			0, 0))
		{
			return false;
		}
		UsbIntfDesc *cur = dev->intfDesc;
		while (cur)
		{
			//	UsbPrintIntfDesc(cur);
			UsbEndpDesc * cd = cur->endpoints;
			while (cd)
			{
				//	UsbPrintEndpDesc(cd);
				cd = cd->next;
			}
			cur = cur->next;
		}
		//_usbhubinit(dev);
		//_UsbMouseInit(dev); _UsbKbdInit(dev);
		usb_hub_init(dev);
		if (pickedIntfDesc->intfClass == 8 && pickedEndpDesc->attributes)
		{
			usb_storage_init(dev);
		}
	}

	return true;
}
void UsbService()
{
	while (1)
	{
		UsbPoll();
	}
}
void UsbPoll()
{
	//if (!pcidone) return;
	//usbPoll = 0;

	for (UsbController *c = g_usbControllerList; c; c = c->next)
	{
		if (c->poll)
		{
			c->poll(c);
		}
	}

	for (UsbDevice *dev = g_usbDeviceList; dev; dev = dev->next)
	{
		if (dev->drvPoll)
		{
			dev->drvPoll(dev);
		}
	}
}
