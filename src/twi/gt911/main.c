#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys.h"

struct TS_GT911 ts = GT911_TS_INIT;
struct TWI_DEV ts_dev = { .bus = SYS_TWI_NUM, .addr = GT911_ADDR, .data = &ts };
struct TWI_CFG twi = { .type = TWI_MASTER, .port = SYS_TWI_PORT, .clkmn = TWI_400kHz };

int palette[] = { RGB565_PURPLE, RGB565_GREEN, RGB565_ORANGE, RGB565_BLUE, RGB565_RED };

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
  u16 *addr = (u16*)layer[0].addr + display->width * y + x;
  for(int cy = 0; cy < sizeof(bitmap); cy++)
    if((cy + y) < display->height)
      for(int cx = 0; cx < 8; cx++)
        if((cx + x) < display->width && bitmap[cy] & (1 << cx))
          addr[cy * display->width + cx] = z;
}

void line (int x1, int y1, int x2, int y2, int color)
{
  int i, deltax, deltay, numpixels;
  int d, dinc1, dinc2;
  int xa, xb, xinc1, xinc2;
  int ya, yb, yinc1, yinc2;
  deltax = abs(x2 - x1);
  deltay = abs(y2 - y1);
  if(deltax >= deltay)
  {
    numpixels = deltax + 1;
    d = (2 * deltay) - deltax;
    dinc1 = deltay << 1;
    dinc2 = (deltay - deltax) << 1;
    xinc1 = 1;
    xinc2 = 1;
    yinc1 = 0;
    yinc2 = 1;
  }
  else
  {
    numpixels = deltay + 1;
    d = (2 * deltax) - deltay;
    dinc1 = deltax << 1;
    dinc2 = (deltax - deltay) << 1;
    xinc1 = 0;
    xinc2 = 1;
    yinc1 = 1;
    yinc2 = 1;
  }
  if(x1 > x2)
  {
    xinc1 = -xinc1;
    xinc2 = -xinc2;
  }
  if(y1 > y2)
  {
    yinc1 = -yinc1;
    yinc2 = -yinc2;
  }
  xa = x1;
  ya = y1;
  xb = x2;
  yb = y2;
  numpixels = (numpixels+3)>>1;
  for(i = 1; i < numpixels; i++)
  {
    plot(xa, ya, color);
    plot(xb, yb, color);
    if(d < 0)
    {
      d = d + dinc1;
      xa = xa + xinc1;
      xb = xb - xinc1;
      ya = ya + yinc1;
      yb = yb - yinc1;
    }
    else
    {
      d = d + dinc2;
      xa = xa + xinc2;
      xb = xb - xinc2;
      ya = ya + yinc2;
      yb = yb - yinc2;
    }
  }
}

int main (void)
{
  u16 *fb[2];
  int i, j;
  puts(CURSOR_HIDE FG_CYAN  "F1C100S & GT911" ATTR_RESET);
  disp_init(&TFT_800x480, 0);
  fb[0] = fb_alloc(display->width, display->height, 16);
  fb[1] = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb[0], 0, 5 << 8);
  memset(layer[0].addr, 0xFF, layer[0].height * layer[0].stride / 8);
  lay_update(0);
  delay(100);
  twi_init(SYS_TWI_NUM, twi);
  disp_backlight(100);
  gt911_rd(&ts_dev, 0x8140, &i, 4);
  printf("TS ID: %c%c%c\n", i, i & 255, (i >> 8) & 255, (i >> 16) & 255);
  while(1)
  {
    delay(20);
    i = gt911_read(&ts_dev);
    printf("Points: %d\r", i);
    if(i > 1)
    {
      layer[0].addr = layer[0].addr == fb[0] ? fb[1] : fb[0];
      memset(layer[0].addr, 0xFF, layer[0].height * layer[0].stride / 8);
      for(j = 0; j < i - 1; j++)
        line(ts.pt[j].x, ts.pt[j].y, ts.pt[j + 1].x, ts.pt[j + 1].y, palette[j]);
      line(ts.pt[0].x, ts.pt[0].y, ts.pt[i - 1].x, ts.pt[i - 1].y, palette[j]);
      disp_sync();
      lay_update(0);
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
