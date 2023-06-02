#include <stdio.h>
#include "sys.h"
#include "usbh_msc.h"
#include "usbh_net.h"

#if 0
#define PUTC(ch)  putchar(ch)
#define PUTS(str) puts(str)
#define PUTF(...) printf(__VA_ARGS__)
#else
#define PUTC(ch)
#define PUTS(str)
#define PUTF(...)
#endif

u8 dev_usb;

static struct USBH_DEV {
  struct {
    DSC_DEV dev;
    union {
      DSC_CFG cfg;
      u8 raw_cfg[512];
    };
  } dsc;
  u8 cfg;
} dev;

/*******************************************************************************
                                     USB PHY
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
                                     USB HOST
*******************************************************************************/
void usbh_init (void)
{
  usb_mux(USB_MUX_HOST);
  CCU->USBPHY_CFG |= 3;
  CCU->BUS_CLK_GATING0 |= (1 << 24);
  CCU->BUS_SOFT_RST0 &= ~(1 << 24);
  CCU->BUS_SOFT_RST0 |= (1 << 24);
  #if 1
  phy_write(0x0c, 0x01, 1);
  phy_write(0x20, 0x14, 5);
  //phy_write(0x2A, 0x02, 2);
  phy_write(0x2A, 0x03, 2);
  #endif
  SYS->CTRL[1] |= 1;
  USB->ISCR = (USB->ISCR & ~0x70) | (1 << 16);    // USBC_BP_ISCR_DPDM_PULLUP_EN
  USB->ISCR = (USB->ISCR & ~0x70) | (1 << 17);
  USB->ISCR = (USB->ISCR & ~0xC070) | (2 << 14);  // Force internal ID to low
  USB->ISCR = (USB->ISCR & ~0x70) | (3 << 12);    // Force internal VBUS to high
  USB->BUS_IE = 0;
  USB->BUS_IS = 0xFF;
  USB->EP_IE = 0;
  USB->EP_IS = 0xFFFFFFFF;
  USB->DEVCTL = 0;
  USB->EP_IDX = 0;
  USB->VEND0 = 0;
  USB->POWER = 0xA0;
  USB->DEVCTL = 0x81;
  USB->POWER = 0xF0;
  USB->DEVCTL = 0x99;
  dev.cfg = 0;
  dev_usb = 0;
}

int wait_csr (u8 ep, u32 mask, u32 event, u32 timeout)
{
  u8  cur_ep = USB->EP_IDX;
  int res = OK;
  USB->EP_IDX = ep & 127;
  for(ctr_us = 0; res == OK; )
  {
    if(ep & IN)
    { // ep in
      if((USB->RXCSR & mask) == event) break;
      res = USB->RXCSR & 0x14C; // RXERR_INCMP, RXERR_STALL, RX_NAKTOD, RXERR_TO
    }
    else if(ep & 127)
    { // ep out
      if((USB->TXCSR & mask) == event) break;
      res = USB->TXCSR & 0xC4;  // TXERR_NAKTOINCMP, TXERR_STALL, 
    }
    else
    { // ep0
      if((USB->TXCSR & mask) == event) break;
      res = USB->TXCSR & 0x94;  // EP0ERR_NAKTO, EP0ERR_TO, EP0ERR_STALL
    }
    if(USB->BUS_IS & 0xF7) res |= 0x4000;
    if(ctr_us >= timeout) res |= 0x8000;
  }
  USB->EP_IDX = cur_ep;
  return res;
}

int ctrl_msg (u32 w1, u32 w2, void *data)
{
  int res = 0;
  u8  *ptr = data, ep = USB->EP_IDX;
  USB->EP_IDX = 0;
  PUTF("CTRL -> %08X %08X ", __builtin_bswap32(w1), __builtin_bswap32(w2));
  USB->FIFO[0].word32 = w1;
  USB->FIFO[0].word32 = w2;
  w2 >>= 16;
  USB->TXCSR = 0x0A;            // SetupPkt | TxPktRdy
  if(wait_csr(0, 2, 0, 500000) == OK)
  {
    if(w1 & 0x80) // Device-to-host
    {
      if(w2) PUTF("<-");
      while(w2)
      {
        USB->TXCSR |= 0x20;     // ReqPkt
        if(wait_csr(0, 0x20, 0, 500000) != OK) return 0;
        w2 -= USB->RXCOUNT;
        PUTF("%d ", USB->RXCOUNT);
        PUTF(" (%02X,%d)", USB->TXCSR, USB->RXCOUNT);
        if(!USB->RXCOUNT) break;
        while(USB->RXCOUNT) ptr[res++] = USB->FIFO[0].byte;
      }
      USB->TXCSR &= ~1;
      PUTF("csr_%02X ", USB->TXCSR);
      USB->TXCSR |= 0x42;       // StatusPkt | TxPktRdy
      if(wait_csr(0, 2, 0, 500000) != OK) return 0;
      USB->EP_IDX = ep;
      for(int i = 0; i < res; i++) PUTF(" %02X", ptr[i]);
      PUTC('\n');
      return res;
    }
    else  // Host-to-device
    {
      while(res < w2)
      {
        USB->TXCSR &= ~2;
        for(int i = 0; i < 64 && res < w2; i++) USB->FIFO[0].byte = ptr[res++];
        USB->TXCSR |= 2;        // TxPktRdy
        if(wait_csr(0, 2, 0, 500000) != OK)
        {
          puts("txpkt error");
          while(1);
        }
      }
      PUTF("csr_%02X ", USB->TXCSR);
      USB->TXCSR |= 0x60;       // StatusPkt | ReqPkt
      if(wait_csr(0, 1, 1, 500000) == OK)
      {
        PUTF("csr_%02X ", USB->TXCSR);
        USB->TXCSR &= ~0x41;
        USB->EP_IDX = ep;
        PUTC('\n');
        return 1;
      }
      else PUTS("stat error");
    }
  }
  USB->EP_IDX = ep;
  return 0;
}

int get_dev_dsc (DSC_DEV *ptr)
{
  return ctrl_msg(0x01000680, sizeof(DSC_DEV) << 16, ptr);
}

int get_cfg_dsc (DSC_CFG *ptr, u8 idx)
{
  return ctrl_msg(0x02000680 | (idx << 16), sizeof(DSC_CFG) << 16, ptr) ?
    ctrl_msg(0x02000680 | (idx << 16), ptr->wTotalLength << 16, ptr) : 0;
}

int get_str_dsc (u8 idx, u16 lid, u8 *ptr)
{
  return ctrl_msg(0x03000680 | (idx << 16), 0x00020000 | lid, ptr) ?
    ctrl_msg(0x03000680 | (idx << 16), (ptr[0] << 16) | lid, ptr) : 0;
}

int set_addr (u8 addr)
{
  if(!ctrl_msg(0x00000500 | (addr << 16), 0x00000000, NULL)) return 0;
  delay(2);
  USB->TXFUNCADDR = addr;
  return 1;
}

int set_cfg (u8 cfg)
{
  return ctrl_msg(0x00000900 | (cfg << 16), 0x00000000, NULL);
}

int set_interface (u8 i)
{
  return i;
}

static void usb_reset (void)
{
  USB->POWER = 0xE8;
  delay(25);
  phy_write(0x3c, 2, 2);
  USB->POWER = 0xF0;
  phy_write(0x3c, 0, 2);
  delay(25);
  USB->EP_IDX = 0;
  USB->TXCSR = 0;
  USB->TXFUNCADDR = 0;
  USB->TXHUBADDR = 0;
  USB->TXHUBPORT = 0;
  USB->TXINTERVAL = 0;
}

static void print_str_dsc (char *str, u8 *ptr)
{
  u8 i, j, *dst = ptr;
  for(i = 2, j = *ptr; i < j; i += 2) *dst++ = ptr[i];
  *dst = 0;
  if(ptr[0] > ' ') printf("%s: %s\n", str, ptr);
}

void usbh_handler (void)
{
  u8  buf[256];
  u32 isr;
  isr = USB->BUS_IS;
  USB->BUS_IS = isr;
  if(isr & 0xF7) PUTF("bus: %x\n", isr);
  if(isr & 0x10)
  {
    delay(100);
    usb_reset();
    delay(100);
    USB->TXTYPE = (USB->POWER & 0x10) ? 0x40 : (USB->DEVCTL & 0x40) ? 0x80 : 0xC0;
    if(get_dev_dsc(&dev.dsc.dev) && get_cfg_dsc(&dev.dsc.cfg, 0) && set_addr(1))
    {
      printf("\e[33mUSB device connected (vid=0x%04x pid=0x%04X)\e[0m\n",
        dev.dsc.dev.idVendor, dev.dsc.dev.idProduct);
      if(dev.dsc.dev.iProduct && get_str_dsc(dev.dsc.dev.iProduct, 0x0409, buf))
        print_str_dsc("Product", buf);
      if(dev.dsc.dev.iManufacturer && get_str_dsc(dev.dsc.dev.iManufacturer, 0x0409, buf))
        print_str_dsc("Manufacturer", buf);
      if(usbh_msc_init(&dev.dsc.cfg) == OK) dev_usb = 1; else
      #ifdef USBH_NET
      if(usbh_net_init(&dev.dsc.dev) == OK) dev_usb = 2; else
      #endif
      puts("Unknown USB-device"), dev_usb = 0xff;
    }
    else isr |= 64;
  }
  if(isr & 0x64)
  {
    if(dev_usb) puts("USB device disconnected");
    USB->DEVCTL &= ~1;
    USB->ISCR = (USB->ISCR & ~0x3070) | (2 << 12);  // Internal VBUS low
    delay(250);
    USB->ISCR = (USB->ISCR & ~0x70) | (3 << 12);    // Internal VBUS high
    delay(250);
    USB->DEVCTL |= 1;
    USB->BUS_IS = 0xFF;
    #ifdef USBH_NET
    if(dev_usb == 2) usbh_net_deinit();
    #endif
    dev_usb = 0;
  }
  isr = USB->EP_IS;
  if(isr) USB->EP_IS = isr;
  #ifdef USBH_NET
  if(dev_usb == 2) usbh_net_handler(isr);
  #endif
}
