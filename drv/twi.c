#include <stdio.h>
#include "sys.h"

#if 0
#define PUTC(ch)        putchar(ch)
#define PUTS(str)       puts(str)
#define PRINT(...)      printf(__VA_ARGS__)
#else
#define PUTC(ch)
#define PUTS(str)
#define PRINT(...)
#endif

/*******************************************************************************
                                      TWI
*******************************************************************************/
int twi_error;

static void wait_status (TWI_T *TWI, int event)
{
  TWI->CTRL &= ~8;
  for(ctr_us = 0; !(TWI->CTRL & 8); )
  {
    if(ctr_us > 5000)
    {
      PRINT("twi_tout(stat:%x ctrl:%x)\n", TWI->STAT, TWI->CTRL);
      break;
    }
  }
  if(TWI->STAT != event)
  {
    PRINT("twi_event_err(stat:%x ctrl:%x)\n", TWI->STAT, TWI->CTRL);
    twi_error = 1;
  }
}

static void twi_reset (TWI_T *TWI)
{
  TWI->SRST = 1;
  delay(1);
  TWI->SRST = 0;
}

static int twi_idx (TWI_T *TWI)
{
  if(TWI == TWI0) return 1 << 16;
  else if(TWI == TWI1) return 1 << 17;
  else if(TWI == TWI2) return 1 << 18;
  else return KO;
}

int twi_init (TWI_T *TWI, struct TWI_CFG cfg)
{
  int mask;
  if(TWI == TWI0)
  {
    if(cfg.port == TWI0_PD)       // PD12-TWI0_SCL, PD0-TWI0_SDA
    {
      PD->CFG0 = (PD->CFG0 & 0xFFFFFFF0) | 0x00000003;
      PD->CFG1 = (PD->CFG1 & 0xFFF0FFFF) | 0x00030000;
    }
    else if(cfg.port == TWI0_PE)  // PE11-TWI0_SCL, PE12-TWI0_SDA
      PE->CFG1 = (PE->CFG1 & 0xFFF00FFF) | 0x00033000;
    else return KO;
  }
  else if(TWI == TWI1)
  {
    if(cfg.port == TWI1_PD)       // PD5-TWI1_SCL, PD6-TWI1_SDA
      PD->CFG0 = (PD->CFG0 & 0xF00FFFFF) | 0x03300000;
    else return KO;
  }
  else if(TWI == TWI2)
  {
    if(cfg.port == TWI2_PD)       // PD15-TWI2_SCL, PD16-TWI2_SDA
    {
      PD->CFG1 = (PD->CFG1 & 0x0FFFFFFF) | 0x40000000;
      PD->CFG2 = (PD->CFG2 & 0xFFFFFFF0) | 0x00000004;
    }
    else if(cfg.port == TWI2_PE)  // PE0-TWI2_SCL, PE1-TWI2_SDA
      PE->CFG0 = (PE->CFG0 & 0xFFFFFFFF) | 0x00000044;
    else return KO;
  }
  else return KO;
  mask = twi_idx(TWI);
  CCU->BUS_CLK_GATING2 |= mask;
  CCU->BUS_SOFT_RST2 &= ~mask;
  CCU->BUS_SOFT_RST2 |= mask;
  TWI->CCR = cfg.clkmn;
  twi_reset(TWI);
  return OK;
}

void twi_deinit (TWI_T *TWI)
{
  //.........
}

void twi_stop (TWI_T *TWI)
{
  TWI->CTRL = (TWI->CTRL & ~8) | 16;
  for(ctr_us = 0; TWI->CTRL & 16; )
  {
    if(ctr_us > 5000)
    {
      twi_error = 1;
      PRINT("twi_stp_tout(stat:%x ctrl:%x)\n", TWI->STAT, TWI->CTRL);
      break;
    }
  }
  if(twi_error) twi_reset(TWI);
}

void twi_start (TWI_T *TWI, char addr)
{
  twi_error = 0;            // send start
  TWI->CTRL = 64 | 32 | 4;
  wait_status(TWI, 0x08);
  TWI->DATA = addr;         // send dev_addr
  wait_status(TWI, addr & 1 ? 0x40 : 0x18);
}

void twi_restart (TWI_T *TWI, char addr)
{
  TWI->CTRL = 64 | 32 | 4;  // send restart
  wait_status(TWI, 0x10);
  TWI->DATA = addr;         // send dev_addr
  wait_status(TWI, addr & 1 ? 0x40 : 0x18);
}

void twi_send (TWI_T *TWI, void *ptr, int len)
{
  u8 *p = (u8 *)ptr;
  do
  {
    TWI->DATA = *p++;       // send data
    wait_status(TWI, 0x28);
  } while(--len);
}

void twi_recv (TWI_T *TWI, void *ptr, int len)
{
  u8 *p = (u8 *)ptr;
  do
  {
    if(len == 1)
    {
      TWI->CTRL &= ~4;            // nak
      wait_status(TWI, 0x58);
    }
    else wait_status(TWI, 0x50);  // ack
    *p++ = TWI->DATA;
  } while(--len);
}

void twi_write_start (TWI_T *TWI, char dev_addr, char reg_addr)
{
  twi_start(TWI, dev_addr & 0xFE);
  twi_send(TWI, &reg_addr, 1);
}

void twi_read_start (TWI_T *TWI, char dev_addr, char reg_addr)
{
  twi_write_start(TWI, dev_addr, reg_addr);
  twi_restart(TWI, dev_addr | 1);
}

/*******************************************************************************
                              RTC PCF8563 (BM8563)
*******************************************************************************/
enum { PCF_CTRL1, PCF_CTRL2, PCF_SEC, PCF_MIN, PCF_HOUR, PCF_DAY,
       PCF_WDAY, PCF_MON, PCF_YEAR, PCF_AMIN, PCF_AHOUR, PCF_ADAY,
       PCF_AWDAY, PCF_CLK, PCF_TCTRL, PCF_TIMER };

static int pcf8563_write_reg (struct TWI_DEV *dev, u8 addr, u8 dat)
{
  twi_write_start(dev->bus, dev->addr, addr);
  twi_send(dev->bus, &dat, 1);
  twi_stop(dev->bus);
  return twi_error ? KO : OK;
}

static u8 pcf8563_read_reg (struct TWI_DEV *dev, u8 addr)
{
  char res;
  twi_read_start(dev->bus, dev->addr, addr);
  twi_recv(dev->bus, &res, 1);
  twi_stop(dev->bus);
  return res;
}

int pcf8563_read_tm (struct TWI_DEV *dev)
{
  struct TM tmp, *tm = (struct TM *)dev->data;
  twi_read_start(dev->bus, dev->addr, PCF_SEC);
  twi_recv(dev->bus, &tmp, sizeof(struct TM) - 1);
  twi_stop(dev->bus);
  tmp.min &= 0x7F;
  tmp.hour &= 0x3F;
  tmp.mday &= 0x3F;
  tmp.wday &= 0x07;
  tmp.mon &= 0x1F;
  if(twi_error || tmp.sec > 0x60 || tmp.min > 0x60 || tmp.hour > 0x23 ||
     tmp.mday > 0x31 || tmp.mon > 0x12) return KO;
  tm->sec = tmp.sec & 0x7F;
  tm->min = tmp.min;
  tm->hour = tmp.hour;
  tm->mday = tmp.mday;
  tm->wday = tmp.wday;
  tm->mon = tmp.mon;
  tm->year = tmp.year;
  return OK;
}

int pcf8563_write_tm (struct TWI_DEV *dev)
{
  twi_write_start(dev->bus, dev->addr, PCF_SEC);
  twi_send(dev->bus, dev->data, sizeof(struct TM) - 1);
  twi_stop(dev->bus);
  return twi_error ? KO : OK;
}

int pcf8563_init (struct TWI_DEV *dev)
{
  if(pcf8563_write_reg(dev, PCF_TCTRL, 0x82) == KO) return KO;
  if(!(pcf8563_read_reg(dev, PCF_SEC) & 128)) return OK;
  struct TM *tm = (struct TM *)dev->data; // 00:00:00 01/01/2000
  tm->sec = 0; tm->min = 0; tm->hour = 0; tm->mday = 1; tm->wday = 6;
  tm->mon = 1; tm->year = 0; tm->res = 0;
  pcf8563_write_tm(dev);
  return 1;
}

/*******************************************************************************
                                TS NS2009
*******************************************************************************/
#define NS2009_RD_X   0xC0
#define NS2009_RD_Y   0xD0
#define NS2009_RD_Z1  0xE0
#define NS2009_RD_Z2  0xF0

static void ts_buf_update (u32 *buf, u32 val)
{
  buf[4] = buf[3];
  buf[3] = buf[2];
  buf[2] = buf[1];
  buf[1] = buf[0];
  buf[0] = val;
}

static u32 ts_get_median (u32 *buf)
{
  int a[3] = { 0xFFFF, 0xFFFF, 0xFFFF };
  for(int i = 0; i < 5; i++)
  {
    if(buf[i] < a[0]) { a[2] = a[1]; a[1] = a[0]; a[0] = buf[i]; }
    else if(buf[i] < a[1]) { a[2] = a[1]; a[1] = buf[i]; }
    else if(buf[i] < a[2]) a[2] = buf[i];
  }
  return a[2];
}

static int ns2009_rd (struct TWI_DEV *dev, u8 cmd, u32 *dat)
{
  char res[2];
  twi_start(dev->bus, dev->addr & 0xFE);
  twi_send(dev->bus, &cmd, 1);
  twi_stop(dev->bus);
  if(!twi_error)
  {
    twi_start(dev->bus, dev->addr | 1);
    twi_recv(dev->bus, &res, 2);
    twi_stop(dev->bus);
    *dat = (res[0] << 4) | (res[1] >> 4);
  }
  return twi_error ? KO : OK;
}

int ns2009_read (struct TWI_DEV *dev)
{
  u32 x, y, z;
  struct TS *ts = (struct TS *)dev->data;
  if(ns2009_rd(dev, NS2009_RD_Z1, &z) == KO) return KO;
  if(ns2009_rd(dev, NS2009_RD_X, &x) == KO) return KO;
  if(ns2009_rd(dev, NS2009_RD_Y, &y) == KO) return KO;
  ts->z <<= 1;
  if(z < ts->dz) return KO;
  ts->z |= 1;
  if(x < ts->x1) x = ts->x1; else if(x > ts->x2) x = ts->x2;
  if(y < ts->y1) y = ts->y1; else if(y > ts->y2) y = ts->y2;
  x = (x - ts->x1) * (display->width - 1) / (ts->x2 - ts->x1);
  y = (y - ts->y1) * (display->height - 1) / (ts->y2 - ts->y1);
  ts_buf_update(ts->bufx, x);
  ts_buf_update(ts->bufy, y);
  if((ts->z & 0x1F) != 0x1F) return KO;
  ts->x = ts_get_median(ts->bufx);
  ts->y = display->height - ts_get_median(ts->bufy);
  return OK;
}
