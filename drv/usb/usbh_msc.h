#ifndef USBH_MSC_H
#define USBH_MSC_H

#include "usbh.h"

int usbh_msc_init (DSC_CFG *cfg_dsc);
int usbh_msc_read (void *ptr, u32 addr, u32 cnt);
int usbh_msc_write (void *ptr, u32 addr, u32 cnt);

#endif
