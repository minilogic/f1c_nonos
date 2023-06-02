#ifndef USBH_NET_H
#define USBH_NET_H

#include "usbh.h"

extern u8 ip_addr[4];
extern u8 ip_mask[4];
extern u8 ip_gate[4];
extern u8 ip_mac[6];

int usbh_net_init (DSC_DEV *dev_dsc);
void usbh_net_deinit (void);
void usbh_net_handler (u32 isr);

#endif
