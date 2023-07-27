/* Plasma effect: https://github.com/retrofw/gpmark */

#include <stdio.h>
#include <math.h>
#include "sys.h"

int fsin[3][4096];
unsigned short rgb[256];

void InitPlasma()
{
  float l = 4.0;
  for(int i = 0; i < 4096; i++)
  {
    fsin[0][i] = (int)(sin(i / (l * 15.0)) * 96.0 + 96.0);
    fsin[1][i] = (int)(sin(i / (l * 20.0)) * 112.0 + 112.0);
    fsin[2][i] = (int)(sin(i / (l * 35.0)) * 128.0 + 128.0);
  }
  for(int i = 0; i < 64; i++)
  {
    rgb[i] = ((i >> 1) << 11) | ((i >> 1) << 5);
    rgb[64 + i] = ((31-(i >> 2)) << 11) | (((i >> 1) + 32) << 5) | (i >> 1);
    rgb[128 + i] = ((16 - (i >> 2)) << 11) | ((63 - (i >> 1)) << 5) | 31;
    rgb[192 + i] = ((31 - (i >> 1)) << 5) | (31 - (i >> 1));
  }
}

void RunPlasma(int ntime, unsigned short *vram)
{
  int k1 = ntime;
  int k3 = 3 * ntime;
  if (k1 > 503) k1 = k1 - (k1 / 503) * 503;
  if (k3 > 880) k3 = k3 - (k3 / 880) * 880;
  for(int y=0; y < display->height; y++)
  {
    for(int x=0; x < display->width; x++)
    {
      unsigned char c = fsin[0][x] + fsin[1][y + k1] + fsin[2][x + y + k3];
      *vram++ = rgb[c];
    }
  }
}

int main (void)
{
  u16 *fb;
  int  ntime = 0;
  puts("\e[36mF1C100S - Plasma Effect\e[0m");
  //disp_init(&TV_PAL, 0);
  disp_init(&TFT_800x480, 0);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);
  delay(100);
  disp_backlight(60);
  InitPlasma();
  while(1)
  {
    RunPlasma(ntime++, fb);
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
