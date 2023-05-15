#ifndef USBH_H
#define USBH_H

#include "usb_dsc.h"

#define OUT 0
#define IN  128

extern u8 dev_usb;

void usbh_init (void);
void usbh_handler (void);

int set_cfg (u8 cfg);
int ctrl_msg (u32 w1, u32 w2, void *data);
int wait_csr (u8 ep, u32 mask, u32 event, u32 timeout);

int msc_init (DSC_CFG *cfg_dsc);
int msc_read (void *ptr, u32 addr, u32 cnt);
int msc_write (void *ptr, u32 addr, u32 cnt);

#endif