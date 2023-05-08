#include <stdio.h>
#include "sys.h"
#include "usbd.h"

int main (void)
{
  dev_enable(0);
  puts("\033[36mF1C100S - USBD MSC\033[0m");
  sd_init();
  while(1)
  {
    if(sd_card_detect())
    {
      printf("Card inserted: %uMB\n", sd_card_init() / 2048);
      usb_mux(USB_MUX_DEVICE);
      usbd_init();
      while(sd_card_detect()) usbd_handler();
      usb_deinit();
    }
    else
    {
      puts("Card removed");
      while(!sd_card_detect()) {};
    }
  }
}
