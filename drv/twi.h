#ifndef TWI_H
#define TWI_H

/* TWI */
enum TWI_TYPE   { TWI_MASTER, TWI_SLAVE };
enum TWI0_PORT  { TWI0_PD, TWI0_PE };
enum TWI1_PORT  { TWI1_PD };
enum TWI2_PORT  { TWI2_PD, TWI2_PE };
#define TWI_100kHz  ((2 << 3) | 5)
#define TWI_400kHz  ((2 << 3) | 3)

struct TWI_CFG {
  u32 type  : 1;
  u32 port  : 2;
  u32 clkmn : 8;
};

struct TWI_DEV {
  TWI_T *bus;
  u8    addr;
  void  *data;
};

extern int twi_error;

int twi_init (TWI_T *TWI, struct TWI_CFG cfg);
void twi_stop (TWI_T *TWI);
void twi_start (TWI_T *TWI, char addr);
void twi_restart (TWI_T *TWI, char addr);
void twi_send (TWI_T *TWI, void *ptr, int len);
void twi_recv (TWI_T *TWI, void *ptr, int len);
void twi_write_start (TWI_T *TWI, char dev_addr, char reg_addr);
void twi_read_start (TWI_T *TWI, char dev_addr, char reg_addr);

/* RTC PCF8563 (BM8563) */
#define PCF8563_ADDR 0xA2

struct TM {
  u8  sec;      // 00:59
  u8  min;      // 00:59
  u8  hour;     // 00:23
  u8  mday;     // 01:31
  u8  wday;     // 00:06 (Sunday, Monday, .. , Saturday)
  u8  mon;      // 01:12
  u8  year;     // 00:99
  u8  res;      // reserved
};

int pcf8563_init (struct TWI_DEV *dev);
int pcf8563_read_tm (struct TWI_DEV *dev);
int pcf8563_write_tm (struct TWI_DEV *dev);

/* TS NS2009 */
#define NS2009_ADDR 0x90
#define NS2009_TS_INIT { 150, 3960, 140, 3800, 80 }

struct TS_NS2009 {
  u32 x1, x2;   // X - borders
  u32 y1, y2;   // Y - borders
  u32 dz;       // Z - threshold
  u32 bufx[5];  // X - fifo buffer
  u32 bufy[5];  // Y - fifo buffer
  u32 x, y, z;  // X, Y - real coordinates, Z - touch history
};

int ns2009_read (struct TWI_DEV *dev);

/* TS GT911 */
#if 1
#define GT911_ADDR 0xBA
#else
#define GT911_ADDR 0x28
#endif
#define GT911_TS_INIT { 0, 799, 0, 479 };

struct TS_GT911 {
  u32 x1, x2;   // X - borders
  u32 y1, y2;   // Y - borders
  struct {
    u32 x, y, z;// X, Y - real coordinates, Z - point size
  } pt[5];
};

int gt911_rd (struct TWI_DEV *dev, u16 addr, void *dat, u32 len);
int gt911_wr (struct TWI_DEV *dev, u16 addr, void *dat, u32 len);
int gt911_read (struct TWI_DEV *dev);

#endif
