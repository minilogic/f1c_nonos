#include <stdio.h>
#include "sys.h"
#include "lvgl.h"
#include "lv_demo_stress.h"
#include "lv_demo_widgets.h"
#include "lv_demo_benchmark.h"

static void display_flush (lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
  layer[0].addr = color_p;
  lay_update(0);
  lv_disp_flush_ready(disp_drv);
}

int main (void)
{
  u8  bl = 100;
  u16 *fb[2];
  lv_disp_drv_t disp_drv;
  lv_disp_buf_t draw_buf;
  puts("\033[36mF1C100S - LVGL Demo ("__DATE__" "__TIME__")\033[0m");
  //disp_init(&TV_NTSC, 0);
  disp_init(&TFT_800x480, 0);
  ctr_ms = 0;
  #if 1
  lay_config(0, display->width, display->height, 0, 0, 16, 0, 0, 5 << 8);
  #else
  lay_config(0, 700, 400, 0, 0, 16, 0, 0, 5 << 8);
  #endif
  delay(100);
  disp_backlight(bl);
  fb[0] = fb_alloc(layer[0].width, layer[0].height, 16);
  fb[1] = fb_alloc(layer[0].width, layer[0].height, 16);
  lv_init();
  lv_disp_buf_init(&draw_buf, fb[0], fb[1], layer[0].width * layer[0].height);
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = layer[0].width;
  disp_drv.ver_res = layer[0].height;
  disp_drv.flush_cb = display_flush;
  disp_drv.buffer = &draw_buf;
  lv_disp_drv_register(&disp_drv);
  #if 1
  lv_demo_widgets();
  #else
  lv_demo_stress();
  #endif
  while(1)
  {
    lv_task_handler();
    if(kbhit())
    {
      int c = getchar();
      if(c == '=' && bl < 100) bl++;
      if(c == '-' && bl > 0) bl--;
      if(c == '=' || c == '-') disp_backlight(bl);
      if(c == '6' && layer[0].posx < display->width) layer[0].posx++;
      if(c == '4' && layer[0].posx > 0) layer[0].posx--;
      if(c == '2' && layer[0].posy < display->height) layer[0].posy++;
      if(c == '8' && layer[0].posy > 0) layer[0].posy--;
      if(c == '2' || c == '4' || c == '6' || c == '8') lay_update(0);
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
