#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "usb_dsc.h"
#include "usb_msc.h"

#if 0
#define PUTC(ch)  putchar(ch)
#define PUTS(str) puts(str)
#define PUTF(...) printf(__VA_ARGS__)
#else
#define PUTC(ch)
#define PUTS(str)
#define PUTF(...)
#endif

static u8 vendor[] = "MiniLogic";
static u8 device[] = "USB Mass Storage Device";
static u8 serial[] = "000000000000";

#define EP_BULK_IN  1
#define EP_BULK_OUT 1

static struct
{
  DSC_DEV   dev;
  DSC_QUAL  qual;
  DSC_CFG   cfg;
  DSC_IF    itf;
  DSC_EP    ep1;
  DSC_EP    ep2;
  DSC_LNID  str0;
} dsc = {
  { /* Device Descriptor */
    sizeof(DSC_DEV),  // bLength
    1,                // bDescriptorType
    0x0200,           // bcdUSB
    0,                // bDeviceClass
    0,                // bDeviceSubClass
    0,                // bDeviceProtocol
    64,               // bMaxPacketSize0
    0x1111,           // idVendor
    0x0000,           // idProduct
    0x0100,           // bcdDevice
    1,                // iManufacturer
    2,                // iProduct
    3,                // iSerialNumber
    1                 // bNumConfigurations
  },{ /* Qualifier Descriptor */
    sizeof(DSC_QUAL), // bLength
    6,                // bDescriptorType
    0x0200,           // bcdUSB
    0,                // bDeviceClass
    0,                // bDeviceSubClass
    0,                // bDeviceProtocol
    64,               // bMaxPacketSize0
    1,                // bNumConfigurations
    0                 // bReserved
  },{ /* Configuration Descriptor */
    sizeof(DSC_CFG),  // bLength
    2,                // bDescriptorType
    sizeof(DSC_CFG) + // wTotalLength
    sizeof(DSC_IF)  +
    sizeof(DSC_EP)  +
    sizeof(DSC_EP),
    1,                // bNumInterfaces
    1,                // bConfigurationValue
    0,                // iConfiguration
    128,              // bmAttributes:        Bus powered
    100 / 2           // bMaxPower:           100 mA
  },{ /*  Interface Descriptor */
    sizeof(DSC_IF),   // bLength
    4,                // bDescriptorType
    0,                // bInterfaceNumber
    0,                // bAlternateSetting
    2,                // bNumEndpoints
		8,                // bInterfaceClass      Mass Storage Class
		6,                // bInterfaceSubClass   SCSI
		80,               // bInterfaceProtocol   Bulk Only Protocol
		0                 // iInterface
  },{ /* Endpoint 1 Descriptor */
    sizeof(DSC_EP),   // bLength
    5,                // bDescriptorType
    128 | EP_BULK_IN, // bEndpointAddress:    In
    2,                // bmAttributes:        Bulk
    64,               // wMaxPacketSize
    0                 // bInterval
  },{ /* Endpoint 2 Descriptor */
    sizeof(DSC_EP),   // bLength
    5,                // bDescriptorType:     Out
    0 | EP_BULK_OUT,  // bEndpointAddress:    Bulk
    2,                // bmAttributes
    64,               // wMaxPacketSize
    0                 // bInterval
  },{ /* String Descriptor 0 */
    sizeof(DSC_LNID), // bLength
    3,                // bDescriptorType
    0x0409            // wLANGID:             English (US)
  }
};

/* USB Mass Storage Responses */
INQUIRY_RES inq = {
  0,      // Connected direct access block device
  128,    // Device is removable
  0,      // Version is not claim conformance to any standard
  2,      // Response Data Format
  36 - 5, // Additional Length
  128,    // SCSI target device contains an embedded storage array controller
  0,      // Reserved
  0,      // Reserved
	// Vendor ID (8 bytes)
	{'F','1','C','1','0','0','S',' '},
	// Product ID (16 bytes)
  {'S','D','H','C',' ','C','a','r','d',' ','R','e','a','d','e','r'},
	// Product Revision Level (4 bytes)
  {'0','0','0','1'}
};

static union {
  u32 dat[65536 / 4];
  RD_CAPACITIES_RES res_fmt_cap;
  RD_CAPACITY_RES res_cap;
  REQUEST_SENSE_RES res_reqsense;
  MODE_SENSE6_RES res_sense6;
} buf;

static u8 USB_Config;
static SETUP_PACKET setup;
static u32 cbw_tag, cbw_len, cbw_cmd, cbw_addr, bulk_len, bulk_idx;

/*******************************************************************************
                                      USB
*******************************************************************************/
static void phy_write(u8 addr, u8 data, u8 len)
{
  for(u32 i = 0; i < len; i++)
  {
    USB->PHYCTL = (USB->PHYCTL & 0xFFFF0000) | (addr + i) * 256 | (data & 1) * 128;
    USB->PHYCTL |= 1;
    USB->PHYCTL &= ~1;
    data >>= 1;
  }
}

/*******************************************************************************
                                      USBD
*******************************************************************************/
void usbd_init (void)
{
  PUTS("\e[32mEnable USB Device\e[0m");
  usb_mux(USB_MUX_DEVICE);
  CCU->USBPHY_CFG |= 3;
  CCU->BUS_CLK_GATING0 |= (1 << 24);
  CCU->BUS_SOFT_RST0 &= ~(1 << 24);
  CCU->BUS_SOFT_RST0 |= (1 << 24);
  phy_write(0x0c, 0x01, 1);
  phy_write(0x20, 0x14, 5);
  phy_write(0x2A, 0x03, 2);
  SYS->CTRL[1] |= 1;
  USB->ISCR = (USB->ISCR & ~0x70) | (0x3F << 12);  
  USB->VEND0 = 0;
  USB->BUS_IE = 0;
  USB->BUS_IS = 0xFF;
  USB->EP_IE = 0;
  USB->EP_IS = 0xFFFFFFFF;
  USB->POWER &= ~((1 << 7) | (1 << 6));
  USB->POWER |= ((1 << 6) | (1 << 5));
}

/*******************************************************************************
                                    USB DEV
*******************************************************************************/
static void ep0_send_buf (void *buf, u16 len)
{
  u16 ctr;
  u8 *ptr = buf;
  USB->TXCSR = 0x40; // Serviced RxPktRdy
  while(len)
  {
    while(USB->TXCSR & 2);
    ctr = len < 64 ? len : 64;
    len -= ctr;
    do USB->FIFO[0].byte = *ptr++;
    while(--ctr);
    USB->TXCSR = 2; // TxPktRdy
  }
  USB->TXCSR = 8;   // DataEnd
}

static void ep0_send_dsc (void *ptr)
{
  u8 *dsc = ptr;
  u16 len = setup.wLength, dlen = dsc[0];
  if(dsc[1] == 2) dlen = dsc[2];
  if(dlen < len) len = dlen;
  ep0_send_buf(dsc, len);
}

static void ep0_send_str (void *ptr)
{
  u8 *dsc = ptr;
  u16 dlen, len = setup.wLength;
  for(dlen = 0; dsc[dlen]; dlen++);
  dlen = dlen * 2 + 2;
  if(dlen < len) len = dlen;
  USB->TXCSR = 0x40; // Serviced RxPktRdy
  USB->FIFO[0].word16 = 0x0300 | dlen;
  while(len -= 2) USB->FIFO[0].word16 = *dsc++;
  USB->TXCSR = 0x0A; // TxPktRdy | DataEnd
}

static void ep0_handler (void)
{
  USB->EP_IDX = 0;
  u16 csr = USB->TXCSR;
  if(csr & 8) {USB->TXCSR = csr & ~8; return;}  // Status stage is not complete?
  if(csr & 4) USB->TXCSR = csr & ~4;            // EP0 Sent Stall
  if(csr & 16) USB->TXCSR = 0x80;               // EP0 Serviced Setup End
  if(csr & 1)                                   // EP0 RxPkt Ready
  {
    PUTF("EP0 ");
    if(USB->RXCOUNT != 8)
    {
      PUTS("\e[31mInvalid Packet Length\e[0m");
      while(1); // Wait for watchdog!
    }
    PUTF("(%d) ", USB->RXCOUNT);
    setup.dat[0] = USB->FIFO[0].word32;
    setup.dat[1] = USB->FIFO[0].word32;
    PUTF("%08x %08x ", setup.dat[0], setup.dat[1]);
    if(setup.wRequest == 0xFEA1)
		{
      USB->TXCSR = 0x40; // Serviced RxPktRdy
      USB->FIFO[0].byte = 0;
      USB->TXCSR = 0x0A; // TxPktRdy | DataEnd
			PUTS("Get Max LUN");
		}
    else if(setup.wRequest == 0x0500)
    {
      dsc.ep1.wMaxPacketSize = dsc.ep2.wMaxPacketSize = USB->POWER & 16 ? 512 : 64;
      setup.wValue_l &= 127;
      USB->TXCSR = 0x48;
      while(USB->TXCSR & 0x08);
      USB->TXFUNCADDR = setup.wValue_l;
      PUTF("Set Addr(%d) %cS-mode\n", setup.wValue, dsc.ep1.wMaxPacketSize > 64 ? 'H' : 'F');
    }
    else if(setup.wRequest == 0x0900)
    {
      USB_Config = setup.wValue_l;
      //if(USB_Config)
      {
        USB->EP_IDX = EP_BULK_IN;         // in ep: device -> host
        USB->TXFIFOSZ = 6;                // 2^(size + 3): 2^(6+3)=512
        USB->TXFIFOADDR = 64 / 8;         // Offset(addr * 8)
        USB->TXMAXP = dsc.ep1.wMaxPacketSize;
        USB->TXCSR = 0x2048;              // fifo flush, clr data toggle, auto set, mode in
        #if EP_BULK_IN != EP_BULK_OUT
        USB->EP_IDX = EP_BULK_OUT;        // out ep: host -> device
        #endif
        USB->RXFIFOSZ = 6;                // 2^(size + 3): 2^(6+3)=512
        USB->RXFIFOADDR = (512 + 64) / 8; // Offset(addr * 8)
        USB->RXMAXP = dsc.ep2.wMaxPacketSize;
        USB->RXCSR = 0x0090;              // fifo flush, clr data toggle, auto clr, ?
        USB->EP_IDX = 0;
        USB->TXCSR = 0x48;                // Serviced RxPktRdy | DataEnd
        //USB->EP_IE = 0xFFFFFFFF;
        //USB->EP_TX_IE = 0xFFFF;
        //USB->EP_RX_IE = 0xFFFF;
      }
      //else
      //{
      //}
      PUTF("Set Cfg(%d)\n", setup.wValue_l);
    }
    else if(setup.wRequest == 0x0880)
    {
      USB->TXCSR = 0x40; // Serviced RxPktRdy
      USB->FIFO[0].byte = USB_Config;
      USB->TXCSR = 0x0A; // TxPktRdy | DataEnd
      PUTF("Get Cfg(%d)\n", USB_Config);
    }
    else
    {
      if(setup.wRequest == 0x0680)
      {
        if(setup.wValue_h == 1)
        {
          PUTS("Get Dev Dsc");
          ep0_send_dsc(&dsc.dev);
          return;
        }
        else if(setup.wValue_h == 2)
        {
          PUTS("Get Cfg Dsc");
          ep0_send_dsc(&dsc.cfg);
          return;
        }
        else if(setup.wValue_h == 3)
        {
          PUTF("Get Str_%u Dsc\n", setup.wValue_l);
          if(setup.wValue_l == 0) { ep0_send_dsc(&dsc.str0); return; }
          else if(setup.wValue_l == 1) { ep0_send_str(vendor); return; }
          else if(setup.wValue_l == 2) { ep0_send_str(device); return; }
          else if(setup.wValue_l == 3) { ep0_send_str(serial); return; }
        }
        else if(setup.wValue_h == 6)
        {
          PUTS("Get Qual Dsc");
          ep0_send_dsc(&dsc.qual);
          return;
        }
      }
      PUTS("\e[31mInvalid Request\e[0m");
      USB->TXCSR = 0x60;   // Serviced RxPktRdy | SendStall 
    }
  }
}

void ums_csw (void)
{
  #if EP_BULK_IN != EP_BULK_OUT
  USB->EP_IDX = EP_BULK_IN;
  #endif
  while(USB->TXCSR & 3);
  USB->FIFO[EP_BULK_IN].word32 = 0x53425355;  // 'USBS'
  USB->FIFO[EP_BULK_IN].word32 = cbw_tag;
  USB->FIFO[EP_BULK_IN].word32 = cbw_len;
  USB->FIFO[EP_BULK_IN].byte = 0;             // bCSWStatus
  USB->TXCSR |= 1;                            // TxPktRdy
}

static void bulk_init (void)
{
  bulk_idx = 0;
  bulk_len = cbw_len > sizeof(buf.dat) ? sizeof(buf.dat) : cbw_len;
  cbw_len -= bulk_len;
}

void bulk_out_handler (void)
{
  u8  *ptr;
  u32 i;
  USB->EP_IDX = EP_BULK_OUT;
  if(cbw_cmd == 0x2A)
  {
    i = USB->RXCOUNT / 4;
    while(i--) buf.dat[bulk_idx++] = USB->FIFO[EP_BULK_OUT].word32;
    USB->RXCSR &= ~1; // RxPktRdy
    if(bulk_idx == bulk_len / 4)
    {
      sd_write(buf.dat, cbw_addr, bulk_len / 512);
      cbw_addr += bulk_len / 512;
      if(cbw_len) bulk_init();
      else
      {
        cbw_cmd = 0;
        ums_csw();
      }
    }
  }
  else if(USB->RXCOUNT != 31 || USB->FIFO[EP_BULK_OUT].word32 != CBW_SIGNATURE)
  {
    PUTS("\e[31mInvalid CBW\e[0m");
    while(1);         // Wait for watchdog!
  }
  else
  {
    cbw_tag = USB->FIFO[EP_BULK_OUT].word32;        // dCBWTag
    cbw_len = USB->FIFO[EP_BULK_OUT].word32;        // dCBWDataTransferLength
    cbw_cmd = USB->FIFO[EP_BULK_OUT].word32 >> 24;  // CBWCB, bCBWCBLength, bCBWLUN, bmCBWFlags
    cbw_addr = USB->FIFO[EP_BULK_OUT].word32 >> 8;
    cbw_addr = __builtin_bswap32(cbw_addr | (USB->FIFO[EP_BULK_OUT].word32 << 24));
    USB->FIFO[EP_BULK_OUT].word32;
    USB->FIFO[EP_BULK_OUT].word16;
    USB->FIFO[EP_BULK_OUT].byte;
    USB->RXCSR &= ~1; // RxPktRdy
    PUTF("EP%d CMD%02X ", EP_BULK_OUT, cbw_cmd);
    if(cbw_cmd == WR10)
    {
      PUTF("WR10 0x%08lX %u\n", cbw_addr, cbw_len);
      bulk_init();
    }
    else if(cbw_cmd == RD10)
    {
      PUTF("RD10 0x%08lX %u\n", cbw_addr, cbw_len);
      #if EP_BULK_IN != EP_BULK_OUT
      USB->EP_IDX = EP_BULK_IN;
      #endif
      do
      {
        bulk_init();
        sd_read(buf.dat, cbw_addr, bulk_len / 512);
        cbw_addr += bulk_len / 512;
        for(bulk_len /= dsc.ep2.wMaxPacketSize; bulk_len; bulk_len--)
        {
          for(i = 0; i < dsc.ep2.wMaxPacketSize; i += 4)
            USB->FIFO[EP_BULK_IN].word32 = buf.dat[bulk_idx++];
          for(USB->TXCSR |= 1; USB->TXCSR & 3; ) {};
        }
      } while(cbw_len);
      ums_csw();
    }
    else
    {
      i = 0;
      ptr = (u8*)&buf;
      memset(buf.dat, 0, 512);
      switch(cbw_cmd)
      {
        case INQUIRY:
          PUTS("Inquiry");
          i = sizeof(inq);
          ptr = (u8*)&inq;
          break;
        case RD_CAPACITIES:
          PUTS("Read Format Capacity");
          buf.res_fmt_cap.list_len = 8 << 24;
          buf.res_fmt_cap.block_num = __builtin_bswap32(sd_card_init());
          buf.res_fmt_cap.dsc_type = 0x0002;
          buf.res_fmt_cap.block_size = 0x0002;
          i = sizeof(buf.res_fmt_cap);
          break;
        case RD_CAPACITY:
          PUTS("Read Capacity");
          buf.res_cap.last_lba = __builtin_bswap32(sd_card_init() - 1);
          buf.res_cap.block_size = 0x00020000;
          i = sizeof(buf.res_cap);
          break;
        case TEST_UNIT_READY:
          PUTS("Test Unit Ready");
          break;
        case 0x1E:
          PUTS("Pre Allow Medium Removal");
          break;
        case MODE_SENSE6:
          PUTS("Mode Sense(6)");
          buf.res_sense6.data_len = 3;
          i = sizeof(buf.res_sense6);
          break;
        case REQUEST_SENSE:
          PUTS("Request Sense");
          buf.res_reqsense.res_code = 0x70;
          i = sizeof(buf.res_reqsense);
          break;
        default:
          PUTS("\e[31mCommand Not Support\e[0m");
          USB->EP_IDX = EP_BULK_IN;
          USB->TXCSR = 0x10;  // SendStall
          return;
      }
      if(i)
      {
        cbw_len -= i;
        #if EP_BULK_IN != EP_BULK_OUT
        USB->EP_IDX = EP_BULK_IN;
        #endif
        do
        {
          USB->FIFO[EP_BULK_IN].byte = *ptr++;
        } while(--i);
        USB->TXCSR |= 1;  // TxPktRdy
      }
      ums_csw();
    }
  }
}

void usbd_handler (void)
{
  u32 isr;
  isr = USB->BUS_IS;
  USB->BUS_IS = isr;
  // bus events
  if(isr & 1)
  {
    PUTS("USB Suspend");
    //USB->BUS_IS |= 1;
  }
  if(isr & 2)
  {
    PUTS("USB Resume");
    //USB->BUS_IS |= 2;
  }
  if(isr & 4)
  {
    PUTS("USB Reset");
    //USB->BUS_IS = 0xFF;
    USB->EP_IS = 0xFFFFFFFF;
    USB->EP_IDX = 0;
    USB->TXFUNCADDR = 0;
    USB_Config = 0;
  }
  isr = USB->EP_IS;
  USB->EP_IS = isr;
  if(isr & 1) ep0_handler();
  if(isr & (1 << (EP_BULK_OUT + 16))) bulk_out_handler();
}
