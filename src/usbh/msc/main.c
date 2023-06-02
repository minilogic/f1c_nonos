#include <stdio.h>
#include <malloc.h>
#include "sys.h"
#include "ff.h"
#include "usbh_msc.h"

static int file_read (unsigned char **fbuf, char *name)
{
  FIL fil;
  unsigned int i, j, fsize = 0;
  printf("Read file %s", name);
  if(f_open(&fil, name, FA_READ) == FR_OK)
  {
    fsize = f_size(&fil);
    *fbuf = malloc(fsize);
    printf("(%d): ", fsize);
    ctr_ms = 0;
    for(i = 0; i < fsize; )
    {
      usbh_handler();
      if(!dev_usb) break;
      j = fsize - i > 32768 ? 32768 : fsize - i;
      f_read(&fil, *fbuf + i, j, &j);
      i += j;
      printf("%3d%%\b\b\b\b", i * 100 / fsize);
    }
    if(dev_usb) printf("OK (%dmS %d.%dMB/S)\n", ctr_ms,
      (fsize >> 10) / ctr_ms, ((fsize >> 10) % ctr_ms) * 1000 / ctr_ms);
    f_close(&fil);
  }
  else puts(": error");
  return fsize;
}

static int file_write (unsigned char **fbuf, char *name, int fsize)
{
  FIL fil;
  unsigned int i, j;
  printf("Write file %s", name);
  if(f_open(&fil, name, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
  {
    printf("(%d): ", fsize);
    ctr_ms = 0;
    for(i = 0; i < fsize; )
    {
      usbh_handler();
      if(!dev_usb) break;
      j = fsize - i > 32768 ? 32768 : fsize - i;
      f_write(&fil, *fbuf + i, j, &j);
      i += j;
      printf("%3d%%\b\b\b\b", i * 100 / fsize);
    }
    if(dev_usb) printf("OK (%dmS %d.%dMB/S)\n", ctr_ms,
      (fsize >> 10) / ctr_ms, ((fsize >> 10) % ctr_ms) * 1000 / ctr_ms);
    f_close(&fil);
  }
  else puts(": error");
  return fsize;
}

int main (void)
{
  FATFS   fs;
  unsigned char *fbuf;
  int fsize;
  puts("\033[36mF1C100S - USBH MSC\033[0m");
  usb_mux(USB_MUX_HOST);
  usbh_init();
  while(1)
  {
    usbh_handler();
    if(dev_usb ==  1)
    {
      printf("USB disk: ");
      disk_init(0, &usbh_msc_read, &usbh_msc_write);
      if(f_mount(&fs, (TCHAR*)"0:", 1) != FR_OK) puts("not support or error");
      else
      {
        printf("%s\n", fs.fs_type == 2 ? "FAT16" : fs.fs_type == 3 ? "FAT32" : "exFAT");
        fsize = file_read(&fbuf, "0:/vid/avatar.mjpg");
        if(fsize && dev_usb)
        {
          //dump(fbuf, 128);
          file_write(&fbuf, "0:/vid/clone.mjpg", fsize);
          free(fbuf);
        }
      }
      dev_usb = 255;
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
