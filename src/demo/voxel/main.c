/* Voxel effect: http://insolitdust.sourceforge.net/code.html */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sys.h"

u16 *fb;
unsigned char DBuf[320 * 240], HMap[256 * 256], CMap[256 * 256];
float FOV = M_PI / 3.5;     //Focal length
int LastY[320], LastC[320]; //temp tables
int tdelay = 90;            //Time to change direction

int Clamp(int x) { return (x < 0 ? 0 : (x > 255 ? 255 : x)); }

// Calculate the maps...
void MakeMap (void)
{
  int p, p2, k, k2, i, j;
  int a, b, c , d;
  HMap[0] = 128;
  for(p = 256; p > 1; p=p2)
  {
    p2 = p >> 1;
    k = p * 8 + 20;
    k2 = k >> 1;
    for(i = 0; i < 256; i += p) for(j = 0; j < 256; j+= p)
    {
      a = HMap[(i << 8) + j];
      b = HMap[(((i + p) & 255) << 8) + j];
      c = HMap[(i << 8) + ((j + p) & 255)];
      d = HMap[(((i + p) & 255) << 8) + ((j + p) & 255)];
      HMap[(i << 8) + ((j + p2) & 255)] = Clamp(((a + c) >> 1) + (rand() % k - k2));
      HMap[(((i + p2) & 255) << 8) + ((j + p2) & 255)] = Clamp(((a + b + c + d) >> 2) + (rand() % k - k2));
      HMap[(((i + p2) & 255) << 8) + j] = Clamp(((a + b) >> 1) + (rand() % k - k2));
    }
  }
  for(k = 0; k < 3; k++) for(i = 0; i < 256 * 256; i += 256) for(j = 0; j < 256; j++)
    HMap[i + j] = (HMap[((i + 256) & 0xFF00) + j] + HMap[i + ((j + 1) & 0xFF)] +
                  HMap[((i - 256) & 0xFF00) + j] + HMap[i + ((j - 1) & 0xFF)]) >> 2;
  for(i = 0; i < 256 * 256; i += 256) for(j = 0; j < 256; j++)
  {
    k = 128 + (HMap[((i + 256) & 0xFF00) + ((j + 1) & 255)] - HMap[i + j]) * 6;
    if(k < 0) k = 0;
    if(k > 255) k = 255;
    CMap[i + j] = k;
  }
}

// Draw a voxel line
void Line (int x0, int y0, int x1, int y1, int hy, int s)
{
  int i;
  int sx, sy;
  int c,y,h,u0,v0,u1,v1,a,b,h0,h1,h2,h3;
  sx = (x1 - x0) / 320;
  sy = (y1 - y0) / 320;
  for(i = 0; i < 320; i++)
  {
    u0 = (x0 >> 16) & 0xFF;
    a = (x0 >> 8) & 255;
    v0 = ((y0 >> 8) & 0xFF00);
    b = (y0 >> 8) & 255;
    u1 = (u0 + 1) & 0xFF;
    v1 = (v0 + 256) & 0xFF00;
    h0 = HMap[u0 + v0];
    h2 = HMap[u0 + v1];
    h1 = HMap[u1 + v0];
    h3 = HMap[u1 + v1];
    h0 = (h0 << 8) + a * (h1 - h0);
    h2 = (h2 << 8) + a * (h3 - h2);
    h = ((h0 << 8) + b * (h2 - h0)) >> 16;
    h0 = CMap[u0 + v0];
    h2 = CMap[u0 + v1];
    h1 = CMap[u1 + v0];
    h3 = CMap[u1 + v1];
    h0 = (h0 << 8) + a * (h1 - h0);
    h2 = (h2 << 8) + a * (h3 - h2);
    c = ((h0 << 8) + b * (h2 - h0));
    y = (((h - hy) * s) >> 11) + (240 / 2);
    if(y < (a = LastY[i]))
    {
      unsigned char *b = DBuf + (a << 8) + (a << 6) + i;
      int sc,cc;
      if(LastC[i] == -1) LastC[i] = c;
      sc = (c - LastC[i]) / (a - y);
      cc = LastC[i];
      if(a > 240 - 1)
      {
        b -= (a - (240 - 1)) * 320;
        cc += (a - (240 - 1)) * sc;
        a = 240 - 1;
      }
      if(y < 0) y = 0;
      while(y < a)
      {
        *b = cc >> 18;
        cc += sc;
        b -= 320;
        a--;
      }
      LastY[i] = y;
    }
    LastC[i] = c;
    x0 += sx;
    y0 += sy;
  }
}

// Show single frame
void Do_Voxel (int x0, int y0, float aa)
{
  int d;
  memset(DBuf, 0, sizeof(DBuf)); //	Video->Clear();
  for(d = 0; d < 320; d++) LastY[d] = 240;
  memset(&LastC[0], -1, 320 * sizeof(int));
  for(d = 0; d < 120; d += 1 + (d >> 6)) Line(x0 + d * 65536 * cos(aa - FOV),
    y0 + d * 65536 * sin(aa - FOV), x0 + d * 65536 * cos(aa + FOV),
    y0 + d * 65536 * sin(aa + FOV), 0 - 30, (int)(120 * 256 / (d + 1)));
}

u16 palette[256];

void SetColour(unsigned char n, unsigned char r, unsigned char g, unsigned char b)
{
  palette[n] = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
}

void fb_update (void)
{
  for(int y = 0; y < 240; y++)
  {
    for(int x= 0; x < 320; x++)
    {
      u16 col = palette[DBuf[y * 320 + x]];
      int offset = (display->height - 480) / 2 * display->width +
        (display->width - 640) / 2;
      offset += (y * display->width + x) * 2;
      fb[offset] = col;
      fb[offset + 1] = col;
      fb[offset + display->width] = col;
      fb[offset + display->width + 1] = col;
    }
  }
}

int main (void)
{
  static int x0 = 0, y0 = 0, cnt = 0, mul = -1;
  static float ss = 14000/*speed*/, sa = 0.004/*rotation*/, a = 0;
  puts("\033[36mF1C100S - Voxel Effect ("__DATE__" "__TIME__")\033[0m");

  disp_init(&TV_NTSC, 0);
  //disp_init(&TFT_800x480, 0);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);
  delay(100);
  disp_backlight(100);

  for (int i = 0; i < 256; i++) SetColour(i, i * 4, i * 4, i * 4);
  MakeMap();
  while(1)
  {
    Do_Voxel(x0, y0, a);
    fb_update();
    x0 += ss * cos(a);
    y0 += ss * sin(a);
    a += sa;
    cnt++;
    if(cnt >= tdelay)
    {
      // Change direction
      sa *= mul;
      mul *= -1;
      fb_update();
      cnt = 0;
      tdelay = 90 + (rand() % 60);
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
