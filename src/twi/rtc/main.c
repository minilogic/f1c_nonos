#include <stdio.h>
#include "sys.h"
#include "mcufont.h"

#if 1
extern const struct mf_rlefont_s mf_rlefont_comic72;
const struct mf_font_s *font = &mf_rlefont_comic72.font;
#else
extern const struct mf_rlefont_s mf_rlefont_times72;
const struct mf_font_s *font = &mf_rlefont_times72.font;
#endif

#define R_SHIFT 11
#define G_SHIFT 5
#define B_SHIFT 0
#define COLOR(r,g,b) attr = (r << R_SHIFT) + (g << G_SHIFT) + (b << B_SHIFT)

u16 *fb;
u16 attr;

void drawPixel (u16 x, u16 y)
{
  u16 *addr = fb + display->width * y + x;
  *addr = attr;
}

static void pixel_cb (s16 x, s16 y, u8 cnt, u8 alpha, void *state)
{
  while(cnt--)
  {
    drawPixel(x, y);
    x++;
  }
}

static uint8_t char_cb (s16 x0, s16 y0, mf_char character, void *state)
{
  return mf_render_character(font, x0, y0, character, &pixel_cb, state);
}

int main (void)
{
  char str[32];
  u8 sec = 255;
  struct TM tm;
  struct TWI_CFG twi = { .type = TWI_MASTER, .port = SYS_TWI_PORT, .clkmn = TWI_400kHz };
  struct TWI_DEV rtc = { .bus = SYS_TWI_NUM, .addr = PCF8563_ADDR, .data = &tm };
  puts(CURSOR_HIDE FG_CYAN "F1C100S TWI Example" ATTR_RESET);
  disp_init(&TFT_800x480, 0);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);
  delay(100);
  disp_backlight(50);
  twi_init(SYS_TWI_NUM, twi);
  COLOR(31, 31, 31);
  mf_render_aligned(font, display->width / 2, 50, MF_ALIGN_CENTER,
    "TWI RTC Example", 0, &char_cb, NULL);
  if(pcf8563_init(&rtc)) puts("RTC error");
  #if 0
  tm.sec = 0x00;
  tm.min = 0x45;
  tm.hour = 0x13;
  tm.mday = 0x10;
  tm.wday = 0x03;
  tm.mon = 0x04;
  tm.year = 0x24;
  pcf8563_write_tm(&rtc);
  #endif
  while(1)
  {
    while(1)
    {
      if(pcf8563_read_tm(&rtc) == OK && sec != tm.sec) break;
      delay(100);
    }
    printf("\r\e[33m%02x/%02x/20%02x %02x:%02x:%02x \e[0m",
      tm.mday, tm.mon, tm.year, tm.hour, tm.min, tm.sec);
    disp_sync();
    COLOR(0, 0, 0);
    mf_render_aligned(NULL, 50, 200, MF_ALIGN_LEFT, str, 0, &char_cb, NULL);
    sprintf(str, "%02x/%02x/20%02x %02x:%02x:%02x", 
      tm.mday, tm.mon, tm.year, tm.hour, tm.min, tm.sec);
    COLOR(31, 63, 0);
    mf_render_aligned(NULL, 50, 200, MF_ALIGN_LEFT, str, 0, &char_cb, NULL);
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
