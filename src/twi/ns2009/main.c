#include <stdio.h>
#include <string.h>
#include "sys.h"

struct TS ts = NS2009_TS_INIT;
struct TWI_DEV ts_dev = { .bus = SYS_TWI_NUM, .addr = NS2009_ADDR0, .data = &ts };
struct TWI_CFG twi = { .type = TWI_MASTER, .port = SYS_TWI_PORT, .clkmn = TWI_400kHz };

int def_col[] = { RGB565_GRAY, RGB565_BLACK, RGB565_AZURE, RGB565_BLUE,
  RGB565_BROWN, RGB565_CYAN, RGB565_GREEN, RGB565_MAGENTA, RGB565_ORANGE,
  RGB565_PINK, RGB565_PURPLE, RGB565_RED, RGB565_VIOLET, RGB565_WHITE,
  RGB565_YELLOW };

u16 *fb;

u8 bitmap[6] = {
  0b00111100,
  0b01111110,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100
};

void plot (int x, int y, int z)
{
  u16 *addr = fb + display->width * y + x;
  for(int cy = 0; cy < sizeof(bitmap); cy++)
    if((cy + y) < display->height)
      for(int cx = 0; cx < 8; cx++)
        if((cx + x) < display->width && bitmap[cy] & (1 << cx))
          addr[cy * display->width + cx] = z;
}

int main (void)
{
  int i = 0;
  puts(CURSOR_HIDE FG_CYAN  "F1C100S & NS2009" ATTR_RESET);
  disp_init(&TFT_800x480, 0);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  memset(fb, 0xff, layer[0].stride / 8 * layer[0].height);
  lay_update(0);
  delay(100);
  twi_init(SYS_TWI_NUM, twi);
  disp_backlight(75);
  while(1)
  {
    delay(1);
    for(int y = 0; y < 25; y++)
      for(int x = 770; x < 800; x++)
        plot(x, y, def_col[i]);
    if(kbhit() && getchar() == '0')
      memset(fb, 0xff, layer[0].stride / 8 * layer[0].height);
    if(ns2009_read(&ts_dev) == OK)
    {
      printf("%d %d %03d %03d\r", i, ts.z & 3, ts.x, ts.y);
      if(ts.x > 770 && ts.y < 25)
      {
        i++;
        if(i == sizeof(def_col) / sizeof(int)) i = 0;
        while(ns2009_read(&ts_dev) == OK);
      }
      else plot(ts.x, ts.y, def_col[i]);
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
