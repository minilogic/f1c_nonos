#ifndef DISP_H
#define DISP_H

struct DISP {
  u16 width;      // Horizontal resolution
  u16 height;     // Vertical resolution
  u16 vsp;        // Vertical sync pulse width
  u16 vbp;        // Vertical back porch width
  u16 hsp;        // Horizontal sync pulse width
  u16 hbp;        // Horizontal back porch width
  u32 pll;        // PLL_VIDEO_CTRL
  u8  div;        // DCLKDIV: dot clock divider
  u8  inv;        // IO_CTRL_0 (1-invert)
  u16 ctrl;       // 0-tv_pal, 1-tv_ntsc, 2-tft
};

struct LAY {
  u16 width;
  u16 height;
  u16 posx;
  u16 posy;
  u32 stride;
  void *addr;
  u32 attr0;
  u32 attr1;
};

extern struct LAY layer[4];
extern struct DISP *display;
extern struct DISP TV_PAL;
extern struct DISP TV_NTSC;
extern struct DISP TFT_800x480;

int disp_init (struct DISP *cfg, u32 bg);
u8 disp_backlight (u8 x);
void disp_sync (void);

void lay_config (int i, int width, int height, int posx, int posy, int stride,
                void *addr, int attr0, int attr1);
void lay_update (int i);

void *fb_alloc (int width, int height, int stride);

#endif
