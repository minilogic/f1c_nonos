#include <stdio.h>
#include "sys.h"

#if 1
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
TWI_T* TWI;

int twi_error;

static void wait_status (int event)
{
  TWI->CTRL &= ~8;
  for(ctr_us = 0; !(TWI->CTRL & 8); )
  {
    if(ctr_us > 5000)
    {
      PRINT("twi timeout(stat:%x ctrl:%x)\n", TWI->STAT, TWI->CTRL);
      break;
    }
  }
  if(TWI->STAT != event)
  {
    PRINT("twi event error(stat:%x ctrl:%x)\n", TWI->STAT, TWI->CTRL);
    twi_error = 1;
  }
}

static void twi_reset (void)
{
  TWI->SRST = 1;
  delay(1);
  TWI->SRST = 0;
}

void twi_enable (void)
{
  TWI = TWI0;
  #if 1
  for(int i = 0; i < 128; i++)
  {
    PE->CFG1 = (PE->CFG1 & ~(0xFF << 12)) | (0x00 << 12); // PE11-SCL, PE12-SDA
    if((PE->DAT & (3 << 11)) == (3 << 11)) break;
    PRINT("HWR:%d ", i);  // hw twi reset
    PE->CFG1 = (PE->CFG1 & ~(0xFF << 12)) | (0x11 << 12); // PE11-SCL, PE12-SDA
    PE->DAT &= ~(3 << 11);
    delay(1);
    PE->DAT |= (3 << 11);
    delay(1);
  }
  #endif
  CCU->BUS_CLK_GATING2 |= (1 << 16);
  CCU->BUS_SOFT_RST2 &= ~(1 << 16);
  CCU->BUS_SOFT_RST2 |= (1 << 16);
  PE->CFG1 = (PE->CFG1 & ~(0xFF << 12)) | (0x33 << 12); // PE11-SCL, PE12-SDA
  #if 1
  TWI->CCR = (2 << 3) | 3;  // 96MHz / (2^3 * (2 + 1) * 10) = 400KHz
  #else
  TWI->CCR = (2 << 3) | 5;  // 96MHz / (2^5 * (2 + 1) * 10) = 100KHz
  #endif
  twi_reset();
}

void twi_disable (void)
{
  // ........
}

void twi_stop (void)
{
  TWI->CTRL = (TWI->CTRL & ~8) | 16;
  for(ctr_us = 0; TWI->CTRL & 16; )
  {
    if(ctr_us > 5000)
    {
      twi_error = 1;
      PRINT("twistp tout(stat:%x ctrl:%x)\n", TWI->STAT, TWI->CTRL);
      break;
    }
  }
  //if(twi_error) twi_reset();
  if(twi_error) twi_enable();
}

void twi_start (char addr)
{
  twi_error = 0;            // send start
  TWI->CTRL = 64 | 32 | 4;
  wait_status(0x08);
  TWI->DATA = addr;         // send dev_addr
  wait_status(addr & 1 ? 0x40 : 0x18);
}

void twi_restart (char addr)
{
  TWI->CTRL = 64 | 32 | 4;  // send restart
  wait_status(0x10);
  TWI->DATA = addr;         // send dev_addr
  wait_status(addr & 1 ? 0x40 : 0x18);
}

void twi_send (void *ptr, int len)
{
  u8 *p = (u8 *)ptr;
  do
  {
    TWI->DATA = *p++;         // send data
    wait_status(0x28);
  } while(--len);
}

void twi_recv (void *ptr, int len)
{
  u8 *p = (u8 *)ptr;
  do
  {
    if(len == 1)
    {
      TWI->CTRL &= ~4;        // nak
      wait_status(0x58);
    }
    else wait_status(0x50);   // ack
    *p++ = TWI->DATA;
  } while(--len);
}

//static
void twi_write_start (char dev_addr, char reg_addr)
{
  twi_start(dev_addr & 0xFE);
  twi_send(&reg_addr, 1);
}

//static
void twi_read_start (char dev_addr, char reg_addr)
{
  twi_write_start(dev_addr, reg_addr);
  twi_restart(dev_addr | 1);
}

/*******************************************************************************
                              RTC PCF8563 (BM8563)
*******************************************************************************/
#define PCF8563 0xA2

enum { RTC_CTRL1, RTC_CTRL2, RTC_SEC, RTC_MIN, RTC_HOUR, RTC_DAY,
       RTC_WDAY, RTC_MON, RTC_YEAR, RTC_AMIN, RTC_AHOUR, RTC_ADAY,
       RTC_AWDAY, RTC_CLK, RTC_TCTRL, RTC_TIMER };

static void rtc_write_reg(char addr, char dat)
{
  twi_write_start(PCF8563, addr);
  twi_send(&dat, 1);
  twi_stop();
}

static char rtc_read_reg (char addr)
{
  char res;
  twi_read_start(PCF8563, addr);
  twi_recv(&res, 1);
  twi_stop();
  return res;
}

int rtc_read_tm (struct TM *ptr)
{
  struct TM tm;
  twi_read_start(PCF8563, RTC_SEC);
  twi_recv(&tm, sizeof(struct TM) - 1);
  twi_stop();
  tm.min &= 0x7F;
  tm.hour &= 0x3F;
  tm.mday &= 0x3F;
  tm.wday &= 0x07;
  tm.mon &= 0x1F;
  if(twi_error || tm.sec > 0x60 || tm.min > 0x60 || tm.hour > 0x23 || tm.mday > 0x31 || tm.mon > 0x12)
  {
    //printf("\e[31mtm_inv(%d):%02x/%02x/%02x %02x:%02x:%02x\e[0m\n", twi_error,
    //  tm.year, tm.mon, tm.mday, tm.hour, tm.min, tm.sec);
    return 1;
  }
  ptr->sec = tm.sec & 0x7F;
  ptr->min = tm.min;
  ptr->hour = tm.hour;
  ptr->mday = tm.mday;
  ptr->wday = tm.wday;
  ptr->mon = tm.mon;
  ptr->year = tm.year;
  return 0;
}

void rtc_write_tm (struct TM *ptr)
{
  twi_write_start(PCF8563, RTC_SEC);
  twi_send(ptr, sizeof(struct TM) - 1);
  twi_stop();
}

int rtc_enable (void)
{
  struct TM tm = { 0x80, 0x00, 0x00, 0x01, 0x06, 0x01, 0x00, 0x00 };
  rtc_write_reg(RTC_TCTRL, 0x82);
  if(twi_error) return 1;
  if(!(rtc_read_reg(RTC_SEC) & 128)) return 0;
  rtc_write_tm(&tm);
  return 2;
}
