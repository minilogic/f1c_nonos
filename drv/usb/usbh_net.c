#ifdef USBH_NET

#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "r8152.h"
#include "usbh_net.h"
#include "netif/etharp.h"

#if 1
#define PUTC(ch)  putchar(ch)
#define PUTS(str) puts(str)
#define PUTF(...) printf(__VA_ARGS__)
#else
#define PUTC(ch)
#define PUTS(str)
#define PUTF(...)
#endif

u8 ip_buf[32768];
struct netif netif;
u32 lan_tim, lan_con = 0, ip_idx = 0, eth_len = 0, usb_len = 0;

static err_t net_snd (struct netif *netif, struct pbuf *p)
{
  u32 i, j, len = p->tot_len;
  pbuf_copy_partial(p, ip_buf, len, 0);
  if(lan_con)
  {
    USB->EP_IDX = 1;
    USB->FIFO[1].word32 = len | TX_FS | TX_LS;
    USB->FIFO[1].word32 = 0;
    i = 512 - 8;
    for(j = 0; j < len; )
    {
      if((len - j) > 3)
      {
        USB->FIFO[1].word32 = *(u32*)&ip_buf[j];
        j += 4;
        i -= 4;
      }
      else
      {
        USB->FIFO[1].byte = ip_buf[j++];
        i--;
      }
      if(!i || j == len)
      {
        USB->TXCSR = 1;
        i = 512;
        wait_csr(1, 1, 0, 5000000);
      }
    }
  }
  return ERR_OK;
}

static void net_rcv (void)
{
  struct pbuf* p;
  int len = USB->RXCOUNT;
  while(len)
  {
    if(usb_len == 0)
    {
      if(USB->RXCOUNT < 4)
      {
        PUTF("rxcount err: %d", USB->RXCOUNT);
        while(1);
      }
      eth_len = USB->FIFO[1].word32 & RX_LEN_MASK;
      usb_len = eth_len + 20;
      if(eth_len & (RX_ALIGN - 1))
        usb_len += (RX_ALIGN - (eth_len & (RX_ALIGN - 1)));
      len -= 4;
    }
    if(len)
    {
      if(len > 3 && usb_len > 3 && !((u32)ip_buf & 3))
      {
        *(u32*)&ip_buf[ip_idx] = USB->FIFO[1].word32;
        ip_idx += 4;
        len -= 4;
        usb_len -= 4;
      }
      else
      {
        len--;
        usb_len--;
        ip_buf[ip_idx++] = USB->FIFO[1].byte;
      }
      if(usb_len == 0)
      {
        p = pbuf_alloc(PBUF_RAW, eth_len, PBUF_POOL);
        if(p != NULL) pbuf_take(p, &ip_buf[20], eth_len);
        if(netif.input(p, &netif) != ERR_OK) pbuf_free(p);
        ip_idx = 0;
      }
    }
  }
}

u32 sys_now (void) { return ctr_ms; }

static err_t ethernetif_init(struct netif *netif)
{
  netif->linkoutput = net_snd;
  netif->output = etharp_output;
  netif->mtu = 1500;
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
  netif->hwaddr_len = 6;
  memcpy(netif->hwaddr, ip_mac, 6);
  return ERR_OK;
}

void net_deinit (void)
{
  netif_remove(&netif);
}

void net_init (void)
{
  ip4_addr_t ipaddr;
  ip4_addr_t netmask;
  ip4_addr_t gw;
  IP4_ADDR(&ipaddr, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
  IP4_ADDR(&netmask, ip_mask[0], ip_mask[1], ip_mask[2], ip_mask[3]);
  IP4_ADDR(&gw, ip_gate[0], ip_gate[1], ip_gate[2], ip_gate[3]);
  netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, ethernet_input);
  netif_set_default(&netif);
  netif_set_up(&netif);
}

void usbh_net_handler (u32 isr)
{
  if(isr & 0x20000)
  {
    USB->EP_IDX = 1;
    net_rcv();
    USB->RXCSR = 32;
  }
  if((lan_tim - sys_now()) > 100)
  {
    lan_tim = sys_now() + 100;
    if(rtl8152_get_speed() & 2)
    {
      if(!lan_con)
      {
        lan_con = 1;
        PUTS("LAN connect");
        rtl8152_enable();
        USB->EP_IDX = 1;
        USB->RXCSR = 32;
        ip_idx = 0, eth_len = 0, usb_len = 0;
        net_init();
      }
    }
    else
    {
      if(lan_con)
      {
        lan_con = 0;
        PUTS("LAN disconnect");
        rtl8152_disable();
        net_deinit();
      }
    }
  }
}

void usbh_net_deinit (void)
{
  lan_con = 0;
  PUTS("LAN disconnect");
  net_deinit();
}

int usbh_net_init (DSC_DEV *dev_dsc)
{
  if(dev_dsc->idVendor != 0x0BDA || dev_dsc->idProduct != 0x8152) return KO;
  set_cfg(1);
  if(!r8152_probe(ip_mac)) return KO;
  PUTF("MAC: %02X-%02X-%02X-%02X-%02X-%02X\n",
    ip_mac[0], ip_mac[1], ip_mac[2], ip_mac[3], ip_mac[4], ip_mac[5]);
  PUTF("IP: %d.%d.%d.%d\n", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
  USB->EP_IDX = 1;
  USB->RXFUNCADDR = 1;                    // address @ bus
  USB->RXTYPE = (1 << 6) | (2 << 4) | 1;
  USB->RXMAXP = 512;                      //dsc.ep1.wMaxPacketSize;
  USB->RXFIFOADDR = 64 / 8;               // addr * 8
  USB->RXFIFOSZ = 6;                      // 2 ^ (size + 3)
  USB->RXCSR = 0x0080;                    // clr data toggle
  USB->TXFUNCADDR = 1;                    // address @ bus
  USB->TXTYPE = (1 << 6) | (2 << 4) | 2;  // !!! setup   speed
  USB->TXMAXP = 512;                      //dsc.ep1.wMaxPacketSize;
  USB->TXFIFOADDR = (512 + 64) / 8;
  USB->TXFIFOSZ = 6;
  USB->TXCSR = 0x0048;
  USB->EP_IDX = 0;
  lan_tim = sys_now() + 100;
  return OK;
}

#endif
