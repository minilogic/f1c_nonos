#include <stdio.h>
#include "sys.h"
#include "usbh.h"
#include "usb_dsc.h"
#include "usb_msc.h"

#if 1
#define PUTC(ch)  putchar(ch)
#define PUTS(str) puts(str)
#define PUTF(...) printf(__VA_ARGS__)
#else
#define PUTC(ch)
#define PUTS(str)
#define PUTF(...)
#endif

CSW csw;
u8 msc_bulk_in, msc_bulk_out;

#define _CBW_(_cmd, _dir) \
  .signature = CBW_SIGNATURE, .tag = _cmd, \
  .total_bytes = sizeof(_cmd##_RES), \
  .dir = _dir, .lun = 0, .cmd_len = sizeof(_cmd##_CMD)

static CBW inquiry_cbw = {
  _CBW_(INQUIRY, IN),
  .cb.inquiry.opcode = INQUIRY,
  .cb.inquiry.alen = sizeof(INQUIRY_RES)
};

static CBW rd_capacity_cbw = {
  _CBW_(RD_CAPACITY, IN),
  .cb.rd_capacity.opcode = RD_CAPACITY
};

static CBW reqsense_cbw = {
  _CBW_(REQUEST_SENSE, IN),
  .cb.request_sense.opcode = REQUEST_SENSE,
  .cb.request_sense.alen = sizeof(REQUEST_SENSE_RES)
};

int msc_reset (void)
{
  int res = ctrl_msg(0x0000FF21, 0x00000000, NULL);
  USB->TXCSR = 0x0048;
  USB->RXCSR = 0x0090;  // clr data toggle
  ctrl_msg(0x00000102, msc_bulk_in, NULL),
  ctrl_msg(0x00000102, msc_bulk_out, NULL);
  return res;
}

int bulk_out (void *ptr, u32 len)
{
  int i, res = OK;
  while(len && res == OK)
  {
    i = len > 512 ? 512 : len;
    len -= i;
    while(i--) USB->FIFO[1].byte = *(u8*)ptr++;
    USB->TXCSR = 1;       // TxPktRdy
    res = wait_csr(OUT | 1, 1, 0, 5000000);
  }
  return res;
}

int bulk_in (void *ptr, u32 len)
{
  while(len)
  {
    USB->RXCSR = 32;
    if(wait_csr(IN | 1, 1, 1, 5000000) != OK) return KO;
    len -= USB->RXCOUNT;
    if(USB->RXCOUNT & 3 || (u32)ptr & 3)
      while(USB->RXCOUNT--) *(u8*)ptr++ = USB->FIFO[1].byte;
    else
    {
      while(USB->RXCOUNT)
      {
        *(u32*)ptr = USB->FIFO[1].word32;
        ptr += 4;
        USB->RXCOUNT -= 4;
      }
    }
  }
  return OK;
}

int msc_xfer (CBW *cbw, void *dat)
{
  if(bulk_out(cbw, sizeof(CBW)) == OK)
  {
    if(cbw->dir == IN)
    {
      if(bulk_in(dat, cbw->total_bytes) != OK) msc_reset();
    }
    else
    {
      if(bulk_out(dat, cbw->total_bytes) != OK) msc_reset();
    }
    if(bulk_in(&csw, sizeof(csw)) == OK && !csw.status) return OK;
  }
  msc_reset();
  return KO;
}

int msc_cmd (CBW *cbw, void *dat)
{
  REQUEST_SENSE_RES  reqsense_res;
  for(int i = 0; i < 5; i++)
  {
    if(msc_xfer(cbw, dat) == OK) return OK;
    msc_xfer(&reqsense_cbw, &reqsense_res);
  }
  return KO;
}

int msc_read (void *ptr, u32 addr, u32 cnt)
{
  CBW cbw = { CBW_SIGNATURE, RD10, cnt * 512, IN, 0, sizeof(RD10_CMD),
    .cb.rd10 = { RD10, 0, __builtin_bswap32(addr), 0, __builtin_bswap16(cnt), 0 }};
  msc_cmd(&cbw, ptr);
  return cnt;
}

int msc_write (void *ptr, u32 addr, u32 cnt)
{
  CBW cbw = { CBW_SIGNATURE, WR10, cnt * 512, OUT, 0, sizeof(WR10_CMD),
    .cb.wr10 = { WR10, 0, __builtin_bswap32(addr), 0, __builtin_bswap16(cnt), 0 }};
  msc_cmd(&cbw, ptr);
  return cnt;
}

int msc_init (DSC_CFG *cfg_dsc)
{
  u8 maxlun;
  INQUIRY_RES        inquiry_res;
  RD_CAPACITY_RES    rdcap_res;
  DSC_IF *if_dsc = (DSC_IF*)(&cfg_dsc->bMaxPower + 1);
  DSC_EP *ep_dsc = (DSC_EP*)(&if_dsc->iInterface + 1);
  if(if_dsc->bInterfaceClass != 8 || if_dsc->bInterfaceProtocol != 80) return KO;
  if(ep_dsc->bEndpointAddress & IN) msc_bulk_in = ep_dsc->bEndpointAddress;
  else msc_bulk_out = ep_dsc->bEndpointAddress;
  ep_dsc++;
  if(ep_dsc->bEndpointAddress & IN) msc_bulk_in = ep_dsc->bEndpointAddress;
  else msc_bulk_out = ep_dsc->bEndpointAddress;
  set_cfg(1);
  ctrl_msg(0x0000FEA1, 0x00010000, &maxlun);
  USB->EP_IDX = 1;      // out ep: host -> device
  USB->TXFUNCADDR = 1;  // address @ bus
  USB->TXTYPE = (1 << 6) | (2 << 4) | (msc_bulk_out & 127); // !!! setup speed
  USB->TXMAXP = 512;    //dsc.ep1.wMaxPacketSize;
  USB->TXFIFOADDR = 64 / 8;
  USB->TXFIFOSZ = 6;
  USB->TXCSR = 0x0048;
  USB->RXFUNCADDR = 1;  // address @ bus
  USB->RXTYPE = (1 << 6) | (2 << 4) | (msc_bulk_in & 127);
  USB->RXMAXP = 512;    //dsc.ep1.wMaxPacketSize;
  USB->RXFIFOADDR = (512 + 64) / 8;  // addr * 8
  USB->RXFIFOSZ = 6;    // 2 ^ (size + 3)
  USB->RXCSR = 0x0080;  // clr data toggle
  if(msc_cmd(&inquiry_cbw, &inquiry_res) != OK ||
     msc_cmd(&rd_capacity_cbw, &rdcap_res) != OK)
  {
    PUTS("MSC Init: error");
    return KO;
  }
  if(!(__builtin_bswap32(rdcap_res.last_lba) / (2 * 1024))) return KO;
  PUTF("Capacity: %dMB\n",
    __builtin_bswap32(rdcap_res.last_lba) / (2 * 1024));
  return OK;
}

