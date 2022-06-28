/********************************************************************
 * COPYRIGHT --  
 ********************************************************************
 * Programm: File
 * Datei: usb.c
 * Autor: Aden
 * Erstellt: 10. Dezember 2014
 *******************************************************************/

#include <bur/plctypes.h>
#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

#define _REPLACE_CONST 
#include <asusb.h>
#include <fileio.h>
#include <string.h>
#include <AsBrStr.h>
#include "auxfunc.h"

/* //////////////////////////////////////////////////////////////////
// DEFINES
////////////////////////////////////////////////////////////////// */
#define TRUE 1
#define FALSE 0
#define MAXUSBDEVICES 32
#define SANDISK_VENDOR_ID 0x0781
#define SANDISK_PRODUCT_ID 0x7105
#define SANDISK_BCD 0x1033
#define USB_GETNODELIST 1
#define USB_SEARCHDEVICE 2
#define USB_DEVICELINK 3
#define USB_DEVICEUNLINK 4
#define USB_FILEACCESS 5
#define MSDEVICE "USBStick0"
#define PARAMDEVICE "/DEVICE="
#define MAXLOGMESSAGELENGTH (250)
/* //////////////////////////////////////////////////////////////////
// DECLARATION
////////////////////////////////////////////////////////////////// */
_LOCAL UsbNodeListGet_typ UsbNodeListGetFub;
_LOCAL UsbNodeGet_typ UsbNodeGetFub;
_LOCAL usbNode_typ usbDevice;
_LOCAL DevLink_typ DevLinkFub;
_LOCAL DevUnlink_typ DevUnlinkFub;
_LOCAL BOOL deviceLinked;
_LOCAL UDINT usbAttachDetachCount;
_LOCAL UINT usbAction,usbNodeIx;
_LOCAL UDINT usbNodeList[MAXUSBDEVICES];
_LOCAL UDINT usbNodeId;
//_LOCAL char szDevParamName[asusb_DEVICENAMELENGTH+sizeof(PARAMDEVICE)];
_LOCAL char szDevParamName[150];
extern _LOCAL UINT FloppyInvisible;
extern _LOCAL char USBDeviceStr[32];

static    char                tmpMsg[MAXLOGMESSAGELENGTH+1];
static    char                lclposstr[30];
static    UINT                USBDevCnt;

void LogMessage(char *ptr)
{
	return;
}
/* //////////////////////////////////////////////////////////////////
// INIT UP
////////////////////////////////////////////////////////////////// */
void usbinit(void)
{
	usbAttachDetachCount = 0;
	usbNodeIx = 0;
	usbAction = USB_GETNODELIST;
	strcpy(USBDeviceStr,MSDEVICE);
	USBDevCnt = 0;
}
/* //////////////////////////////////////////////////////////////////
// CYCLIC TASK
////////////////////////////////////////////////////////////////// */
void usbcyclic(void)
{
	switch (usbAction)
	{
		case USB_GETNODELIST:
			FloppyInvisible = TRUE;
			UsbNodeListGetFub.enable = 1;
			UsbNodeListGetFub.pBuffer = (UDINT)&usbNodeList;
			UsbNodeListGetFub.bufferSize = sizeof(usbNodeList);
			UsbNodeListGetFub.filterInterfaceClass = asusb_CLASS_MASS_STORAGE;
			UsbNodeListGetFub.filterInterfaceSubClass = asusb_SUBCLASS_SCSI_COMMAND_SET;

			UsbNodeListGet(&UsbNodeListGetFub);
			if (UsbNodeListGetFub.status != ERR_FUB_BUSY)
				usbAttachDetachCount = UsbNodeListGetFub.attachDetachCount;

			if (UsbNodeListGetFub.status == ERR_OK /*&& UsbNodeListGetFub.listNodes*/)
			{
				/* USB Device Attach or detach */
				usbAction = USB_SEARCHDEVICE;
				usbNodeIx = 0;
			}
			else 
			if (UsbNodeListGetFub.status == asusbERR_BUFSIZE
				|| UsbNodeListGetFub.status == asusbERR_NULLPOINTER)
			{
				/* Error Handling */
				strcpy(tmpMsg,"FILE: Error during UsbNodeListGet: ");
				brsitoa(UsbNodeListGetFub.status,(UDINT)lclposstr);
				strcat(tmpMsg,lclposstr);
				LogMessage(tmpMsg);
			}
			break;
		case USB_SEARCHDEVICE:
			UsbNodeGetFub.enable = 1;
			UsbNodeGetFub.nodeId = usbNodeList[usbNodeIx];
			UsbNodeGetFub.pBuffer = (UDINT)&usbDevice;
			UsbNodeGetFub.bufferSize = sizeof(usbDevice);
			UsbNodeGet(&UsbNodeGetFub);
			if (UsbNodeGetFub.status == ERR_OK )
			{
				/* MASS_STORAGE on 1st ? */
				if ((strcmp (usbDevice.ifName, "/bd0") == 0))
				{
					/* Mass Storage on Port IF5 found */
					strcpy(szDevParamName,PARAMDEVICE);
					strcat (szDevParamName,usbDevice.ifName);
					usbNodeId = usbNodeList[usbNodeIx];
					usbAction = USB_DEVICELINK;
					LogMessage("FILE: USB device detected ");
					
					strcpy(tmpMsg,"Class= ");
					brsitoa(usbDevice.interfaceClass,(UDINT)lclposstr);
					strcat(tmpMsg,lclposstr);
					strcat(tmpMsg,", Subclass= ");
					brsitoa(usbDevice.interfaceSubClass,(UDINT)lclposstr);
					strcat(tmpMsg,lclposstr);
					strcat(tmpMsg,", Protocol= ");
					brsitoa(usbDevice.interfaceProtocol,(UDINT)lclposstr);
					strcat(tmpMsg,lclposstr);
					strcat(tmpMsg,", VendorID= ");
					brsitoa(usbDevice.vendorId,(UDINT)lclposstr);
					strcat(tmpMsg,lclposstr);
					strcat(tmpMsg,", ProductID= ");
					brsitoa(usbDevice.productId,(UDINT)lclposstr);
					strcat(tmpMsg,lclposstr);
					strcat(tmpMsg,", ReleaseVersion= ");
					brsitoa(usbDevice.bcdDevice,(UDINT)lclposstr);
					strcat(tmpMsg,lclposstr);
					strcat(tmpMsg,", if-name= ");
					strcat(tmpMsg,usbDevice.ifName);
					LogMessage(tmpMsg);
					
					
				}
				else 
				{
					usbNodeIx++;
					if (usbNodeIx >= UsbNodeListGetFub.allNodes) 
					{
						/* USB Device not found */
						usbAction = USB_GETNODELIST;
					}
				}
			}
			else if (UsbNodeGetFub.status == asusbERR_USB_NOTFOUND)
			{
				/* USB Device not found */
				usbAction = USB_GETNODELIST;
			}
			else if (UsbNodeGetFub.status == asusbERR_BUFSIZE
				|| UsbNodeGetFub.status == asusbERR_NULLPOINTER)
			{
				/* Error Handling */
				strcpy(tmpMsg,"FILE: Error during UsbNodeGet: ");
				brsitoa(UsbNodeGetFub.status,(UDINT)lclposstr);
				strcat(tmpMsg,lclposstr);
				LogMessage(tmpMsg);

			}
			break;
		case USB_DEVICELINK:
			DevLinkFub.enable = 1;
			DevLinkFub.pDevice = (UDINT)USBDeviceStr;
			DevLinkFub.pParam = (UDINT)szDevParamName;
			DevLink(&DevLinkFub);
			if (DevLinkFub.status == ERR_OK)
			{		    
				LogMessage("FILE: USB device linked ");
				usbAction = USB_FILEACCESS;
				deviceLinked = TRUE;
			}
			else
			if (DevLinkFub.status == fiERR_DEVICE_ALREADY_EXIST)
			{
				USBDevCnt++;
				if(USBDevCnt > 9)
					USBDevCnt = 0;
				USBDeviceStr[strlen(USBDeviceStr)-1] = USBDevCnt + '0';
			}	    
			else
			if (DevLinkFub.status != ERR_FUB_BUSY)
			{		    
				strcpy(tmpMsg,"FILE: Error during DevLink: ");
				brsitoa(DevLinkFub.status,(UDINT)lclposstr);
				strcat(tmpMsg,lclposstr);
				LogMessage(tmpMsg);
			}
			break;
		case USB_DEVICEUNLINK:
			DevUnlinkFub.enable = 1;
			DevUnlinkFub.handle = DevLinkFub.handle;
			DevUnlink(&DevUnlinkFub);
			if (DevUnlinkFub.status == ERR_OK)
			{		    
				LogMessage("FILE: USB device un-linked ");
				usbAction = USB_GETNODELIST;
				deviceLinked = FALSE;
			}
			else
			if (DevUnlinkFub.status != ERR_FUB_BUSY)
			{		    
				strcpy(tmpMsg,"FILE: Error during DevUnLink: ");
				brsitoa(DevUnlinkFub.status,(UDINT)lclposstr);
				strcat(tmpMsg,lclposstr);
				LogMessage(tmpMsg);
			}
			break;
		case USB_FILEACCESS:
			/* Check USB Device */
			UsbNodeGetFub.enable = 1;
			UsbNodeGetFub.nodeId = usbNodeId;
			UsbNodeGetFub.pBuffer = (UDINT)&usbDevice;
			UsbNodeGetFub.bufferSize = sizeof(usbDevice);
			UsbNodeGet(&UsbNodeGetFub);
			if (UsbNodeGetFub.status == ERR_OK )
			{
				if(FloppyInvisible)
					LogMessage("FILE: USB device ready ");
				FloppyInvisible = FALSE;
			}
			else 
			if (UsbNodeGetFub.status == asusbERR_USB_NOTFOUND)
			{
				FloppyInvisible = TRUE;
				strcpy(tmpMsg,"FILE: USB device removed ");
				brsitoa(UsbNodeGetFub.status,(UDINT)lclposstr);
				strcat(tmpMsg,lclposstr);
				LogMessage(tmpMsg);
				usbAction = USB_DEVICEUNLINK;
			}
			else 
			if (UsbNodeGetFub.status != ERR_FUB_BUSY)
			{
				strcpy(tmpMsg,"FILE: Error during UsbNodeGet: ");
				brsitoa(UsbNodeGetFub.status,(UDINT)lclposstr);
				strcat(tmpMsg,lclposstr);
				LogMessage(tmpMsg);
			}
		default:
			break;	
	}
} 
