#ifndef USB_DSC_H
#define USB_DSC_H

typedef struct PACKED
{
  u8  bLength;
  u8  bDescriptorType;
  u16 bcdUSB;
  u8  bDeviceClass;
  u8  bDeviceSubClass;
  u8  bDeviceProtocol;
  u8  bMaxPacketSize0;
  u16 idVendor;
  u16 idProduct;
  u16 bcdDevice;
  u8  iManufacturer;
  u8  iProduct;
  u8  iSerialNumber;
  u8  bNumConfigurations;
} DSC_DEV;

typedef struct PACKED
{
  u8  bLength;
  u8  bDescriptorType;
  u16 bcdUSB;
  u8  bDeviceClass;
  u8  bDeviceSubClass;
  u8  bDeviceProtocol;
  u8  bMaxPacketSize0;
  u8  bNumConfigurations;
  u8  bReserved;
} DSC_QUAL;

typedef struct PACKED
{
  u8  bLength;
  u8  bDescriptorType;
  u16 wTotalLength;
  u8  bNumInterfaces;
  u8  bConfigurationValue;
  u8  iConfiguration;
  u8  bmAttributes;
  u8  bMaxPower;
} DSC_CFG;

typedef struct PACKED
{
  u8  bLength;
  u8  bDescriptorType;
  u8  bInterfaceNumber;
  u8  bAlternateSetting;
  u8  bNumEndpoints;
  u8  bInterfaceClass;
  u8  bInterfaceSubClass;
  u8  bInterfaceProtocol;
  u8  iInterface;
} DSC_IF;

typedef struct PACKED
{
  u8  bLength;
  u8  bDescriptorType;
  u8  bEndpointAddress;
  u8  bmAttributes;
  u16 wMaxPacketSize;
  u8  bInterval;    
} DSC_EP;

typedef struct PACKED
{
  u8  bLength;
  u8  bDescriptorType;
  u16 wLANGID;
} DSC_LNID;

typedef union PACKED
{
  u32 dat[2];
  struct {
    u8  bmRequestType;
    u8  bRequest;
    u8  wValue_l;
    u8  wValue_h;
    u8  wIndex_l;
    u8  wIndex_h;
    u8  wLength_l;
    u8  wLength_h;
  };
  struct {
    u16 wRequest;
    u16 wValue;
    u16 wIndex;
    u16 wLength;
  };
} SETUP_PACKET;

#endif