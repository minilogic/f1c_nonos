#include <string.h>
#include <malloc.h>
#include "sys.h"

struct LAY layer[4];
struct DISP *display;

struct DISP TV_PAL  = { 720, 576, 625, 21, 864, 141, 0x81006207, 0, 0, 0 };
struct DISP TV_NTSC = { 720, 480, 525, 19, 858, 120, 0x81006207, 0, 0, 1 };
struct DISP TFT_800x480 = { 800, 480, 2, 23, 4, 46, 0x81004107, 5, 0, 2 }; // 198 / 6 = 33.0MHz

int disp_init (struct DISP *cfg, u32 bg)
{
  if(cfg)
  {
    display = cfg;
    for(CCU->PLL_VIDEO_CTRL = cfg->pll; !(CCU->PLL_VIDEO_CTRL & (1 << 28));) {};
    CCU->BE_CLK = 1UL << 31;
    CCU->FE_CLK = 1UL << 31;
    CCU->TCON_CLK = 1UL << 31;
    CCU->BUS_CLK_GATING1 |= (1 << 14) | (1 << 12) | (1 << 4);
    CCU->BUS_SOFT_RST1 &= ~((1 << 14) | (1 << 12) | (1 << 4));
    CCU->BUS_SOFT_RST1 |= (1 << 14) | (1 << 12) | (1 << 4);
    memset(DEBE, 0, 2048);
    TCON->CTRL = 0;
    TCON->INT0 = 0;
    if(cfg->ctrl < 2)
    { // TV
      CCU->TVE_CLK = 0x80008100 | 10;
      CCU->BUS_CLK_GATING1 |= (1 << 10);
      CCU->BUS_SOFT_RST1 &= ~(1 << 10);
      CCU->BUS_SOFT_RST1 |= (1 << 10);
      TCON->T1_CTRL = 0x80100000 | (((cfg->vsp - cfg->height) / 2) << 4);
      TCON->T1_TIMING[0] = ((cfg->width - 1) << 16) | (cfg->height / 2 - 1);
      TCON->T1_TIMING[1] = ((cfg->width - 1) << 16) | (cfg->height / 2 - 1);
      TCON->T1_TIMING[2] = ((cfg->width - 1) << 16) | (cfg->height / 2 - 1);
      TCON->T1_TIMING[3] = ((cfg->hsp - 1) << 16) | (cfg->hbp - 1);
      TCON->T1_TIMING[4] = (cfg->vsp << 16) | (cfg->vbp - 1);
      if(!cfg->height)
      { // PAL
        TVE->CFG1 = 0x07030001;
        TVE->CHROMA_FREQ = 0x2A098ACB;
        TVE->FB_PORCH = 0x008A0018;
        TVE->HD_VS = 0x00000016;
        TVE->LINE_NUM = 0x00160271;
        TVE->LEVEL = 0x00FC00FC;
        TVE->CB_RESET = 0x00000000;
        TVE->VS_NUM = 0x00000001;
        TVE->FILTER = 0x00000005;
        TVE->CBCR_LEVEL = 0x00002828;
        TVE->CBCR_GAIN = 0x0000E0E0;
        TVE->RESYNC = 0x800D000C;
      }
      else
      { // NTSC
        TVE->CFG1 = 0x07030000;
        TVE->CHROMA_FREQ = 0x21F07C1F;
        TVE->FB_PORCH = 0x00760020;
        TVE->HD_VS = 0x00000016;
        TVE->LINE_NUM = 0x0016020D;
        TVE->LEVEL = 0x00F0011A;
        TVE->CB_RESET = 0x00000001;
        TVE->VS_NUM = 0x00000000;
        TVE->FILTER = 0x00000002;
        TVE->CBCR_LEVEL = 0x0000004F;
        TVE->CBCR_GAIN = 0x0000A0A0;
        TVE->RESYNC = 0x000E000C;
      }
      TVE->NOTCH_DELAY = 0x00000120;
      TVE->TINT_PHASE = 0x00000000;
      TVE->B_WIDTH = 0x0016447E;
      TVE->SYNC_LEVEL = 0x001000F0;
      TVE->WHITE_LEVEL = 0x01E80320;
      TVE->ACT_LINE = 0x000005A0;
      TVE->CHROMA_BW = 0x00000000;
      TVE->CFG2 = 0x00000101;
      TVE->SLAVE = 0x00000000;
      TVE->CFG3 = 0x00000000;
      TVE->CFG4 = 0x00000000;
      TVE->DAC1 = 0x433810A1;
      TVE->ENABLE = 1;
      TCON->T1_TIMING[5] = (1 << 16) | 1;
      TCON->T1_IO_CTRL[0] = 0;
      TCON->T1_IO_CTRL[1] = 0x0FFFFFFF;
      TCON->CTRL = 0x80000001;
      DEBE->COLOR_COEF[0] = 0x02040000;
      DEBE->COLOR_COEF[1] = 0x01070000;
      DEBE->COLOR_COEF[2] = 0x00640000;
      DEBE->COLOR_COEF[3] = 0x01000000;
      DEBE->COLOR_COEF[4] = 0x1ed60000;
      DEBE->COLOR_COEF[5] = 0x1f680000;
      DEBE->COLOR_COEF[6] = 0x01c20000;
      DEBE->COLOR_COEF[7] = 0x08000000;
      DEBE->COLOR_COEF[8] = 0x1e870000;
      DEBE->COLOR_COEF[9] = 0x01c20000;
      DEBE->COLOR_COEF[10] = 0x1fb70000;
      DEBE->COLOR_COEF[11] = 0x08000000; //rgb2yuv
      DEBE->MODE_CTRL = 3 | (1 << 5);
      DEBE->BACKCOLOR = bg;
      return OK;
    }
    else
    { // TFT
      PD->CFG0 = 0x22222222;  // PD0..PD7: LCD D2..D7, D10, D11
      PD->CFG1 = 0x22222222;  // PD8..PD15: LCD D12..D15, D18..D21
      PD->CFG2 = 0x00222222;  // PD16..PD21: LCD D22..D23, CLK, DE, HS, VS
      PD->DRV0 = 0xFFFFFFFF;  // Drive strong (level 3)
      PD->DRV1 = 0x00000FFF;
      PD->PUL0 = 0;           // Pull-up/down disable
      PD->PUL1 = 0;
      TCON->T0_CLK &= 0x0FFFFFFF;
      TCON->T0_IO_CTRL[1] = 0xFFFFFFFF;
      TCON->T1_IO_CTRL[1] = 0xFFFFFFFF;
      //DEBE->REG_CTRL = 3;
      DEBE->MODE_CTRL = 3;
      DEBE->BACKCOLOR = bg;
      TCON->T0_CTRL = (1UL << 31) | (1UL << 21) | ((cfg->vbp + cfg->vsp) << 4);
      TCON->T0_CLK = (1UL << 28) | cfg->div;
      TCON->T0_TIMING[0] = ((cfg->width - 1) << 16) | (cfg->height - 1);
      TCON->T0_TIMING[1] = ((cfg->width + cfg->hbp + cfg->hsp - 1) << 16) | (cfg->hbp - 1);
      TCON->T0_TIMING[2] = ((cfg->height + cfg->vbp + cfg->vsp) << 17) | (cfg->vbp - 1);
      TCON->T0_TIMING[3] = ((cfg->hsp - 1) << 16) | (cfg->vsp - 1);
      TCON->T0_HV_TIMING = 0;
      TCON->T0_CPU_IF = 0;
      TCON->T0_IO_CTRL[0] = cfg->inv << 24;
      TCON->T0_IO_CTRL[1] = 0;
      TCON->CTRL = 0x80000000;
      while(TCON->T0_CTRL & (1 << 21));
      return OK;
    }
  }
  return KO;
}

u8 disp_backlight (u8 x)
{
  if(display->ctrl < 2) x = 0;
  if(x > 100) x = 100;
  PA->CFG0 = (PA->CFG0 & ~0x0F00) | 0x0300;
  PWM->CTRL = 0x070;
  PWM->CH0_PERIOD = (100 << 16) | x;
  return x;
}

void disp_sync (void)
{
  u32 shift = display->ctrl < 2 ? 28 : 29, dbg = TCON->DBG_INFO & (1 << shift);
  while(dbg == (TCON->DBG_INFO & (1 << shift))) {};
}


void lay_config (int i, int width, int height, int posx, int posy, int stride,
                void *addr, int attr0, int attr1)
{
  layer[i].width = width;
  layer[i].height = height;
  layer[i].posx = posx;
  layer[i].posy = posy;
  layer[i].stride = width * stride;
  layer[i].addr = addr;
  layer[i].attr0 = attr0;
  layer[i].attr1 = attr1;
}

void lay_update (int i)
{
  DEBE->LAY_SIZE[i] = ((layer[i].height - 1) << 16) | (layer[i].width - 1);
  DEBE->LAY_STRIDE[i] = layer[i].stride;
  DEBE->LAY_POS[i] = (layer[i].posy << 16) | layer[i].posx;
  DEBE->LAY_ATTR0[i] = layer[i].attr0;
  DEBE->LAY_ATTR1[i] = layer[i].attr1;
  //DEBE->REG_CTRL = 3;
  DEBE->LAY_FB_ADDRL[i] = (u32)layer[i].addr << 3;
  DEBE->LAY_FB_ADDRH[i] = (u32)layer[i].addr >> 29;
  DEBE->MODE_CTRL |= (1 << 8);
}

void *fb_alloc (int width, int height, int stride)
{
  int size = width * height * stride / 8;
  void *ptr = memalign(4096, size);
  if(ptr) memset(ptr, 0, size);
  return ptr;
}
