#ifndef TWI_H
#define TWI_H

struct TM {
  uint8_t   sec;          // 00:59
  uint8_t   min;          // 00:59
  uint8_t   hour;         // 00:23
  uint8_t   mday;         // 01:31
  uint8_t   wday;         // 00:06 (Sunday, Monday, .. , Saturday)
  uint8_t   mon;          // 01:12
  uint8_t   year;         // 00:99
  uint8_t   res;          // reserved
};

/* TWI */
extern int twi_error;
void twi_enable (void);

/* RTC */
int rtc_enable (void);
int rtc_read_tm (struct TM *ptr);
void rtc_write_tm (struct TM *ptr);

#endif
