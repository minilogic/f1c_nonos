#include <stdio.h>
#include "sys.h"
#include "lvgl.h"
#include "lv_demos.h"

struct TS ts = NS2009_TS_INIT;
struct TWI_DEV ts_dev = { .bus = SYS_TWI_NUM, .addr = NS2009_ADDR0, .data = &ts };
struct TWI_CFG twi = { .type = TWI_MASTER, .port = SYS_TWI_PORT, .clkmn = TWI_400kHz };

u16 *fb;

static void ts_read (lv_indev_t *indev, lv_indev_data_t *data)
{
  if(ns2009_read(&ts_dev) == OK)
  {
    data->point.x = ts.x;
    data->point.y = ts.y;
    data->state = LV_INDEV_STATE_PRESSED;
  }
  else data->state = LV_INDEV_STATE_RELEASED;
}

static void display_flush (lv_display_t *disp, const lv_area_t *area, u8 *buf)
{
  u32 w = lv_area_get_width(area), h = lv_area_get_height(area);
  u16 *dst = &fb[area->x1 + area->y1 * display->width], *src = (u16*)buf;
  while(h--)
  {
    for(int i = 0; i < w; i++) *dst++ = *src++;
    dst += display->width - w;
  }
  lv_disp_flush_ready(disp);
}

static u32 get_ms (void)
{
  return ctr_ms;
}

int main (void)
{
  u8 *lv_buf[2];
  u32 lv_buf_size;
  lv_display_t  *disp_drv;
  lv_indev_t    *indev_drv;
  printf(FG_CYAN "F1C100S - LVGL_%d.%d.%d ("__DATE__" "__TIME__")" ATTR_RESET,
         LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
  //disp_init(&TV_NTSC, 0);
  disp_init(&TFT_800x480, 0);
  ctr_ms = 0;
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);
  delay(100);
  twi_init(SYS_TWI_NUM, twi);
  disp_backlight(90);

  lv_init();
  lv_tick_set_cb(get_ms);
  disp_drv = lv_display_create(display->width, display->height);
  lv_display_set_flush_cb(disp_drv, display_flush);
  lv_buf_size = display->width * display->height * lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);
  lv_buf[0] = malloc(lv_buf_size);
  lv_display_set_buffers(disp_drv, lv_buf[0], NULL, lv_buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

  indev_drv = lv_indev_create();
  lv_indev_set_type(indev_drv, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev_drv, ts_read);

  lv_demo_widgets();
  //lv_demo_music();
  //lv_demo_stress();
  //lv_demo_benchmark();
  while(1)
  {
    lv_task_handler();
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
