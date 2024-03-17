#ifndef SYS_H
#define SYS_H

#include "f1c100s.h"
#include "display.h"
#include "vt100.h"
#include "spi.h"
#include "twi.h"
#include "aud.h"
#include "sd.h"

#define ctr_us  (TIM->AVS_CNT0)
#define ctr_ms  (TIM->AVS_CNT1)
void delay (u32 ms);
void udelay (u32 us);

enum USB_MUX_STATE { USB_MUX_DEVICE, USB_MUX_DISABLE, USB_MUX_HOST };
extern enum USB_MUX_STATE usb_mux_state;
void usb_mux (enum USB_MUX_STATE i);
void usb_deinit (void);

int kbhit(void);
void dump (void *ptr, uint16_t len);

int state_vsys (void);
int state_switch (void);
void dev_enable (int state);

enum LED_STATE { LED_DISABLE, LED_ENABLE, LED_TOGGLE };
void led_set (enum LED_STATE state);

void disk_init ( u8 pdrv, int (*cbrd) (void *ptr, u32 addr, u32 cnt),
  int (*cbwr) (void *ptr, u32 addr, u32 cnt));

static inline void IRQ_ENABLE (void)
{
  u32 tmp;
  __asm__ __volatile__(
    "mrs %0, cpsr\n"
    "bic %0, %0, #(1<<7)\n"
    "msr cpsr_cxsf, %0"
    : "=r" (tmp)
    :
    : "memory");
}

static inline void IRQ_DISABLE (void)
{
  u32 tmp;
  __asm__ __volatile__(
    "mrs %0, cpsr\n"
    "orr %0, %0, #(1<<7)\n"
    "msr cpsr_cxsf, %0"
    : "=r" (tmp)
    :
    : "memory");
}

static inline void IRQ_WAIT (void)
{
  __asm__ __volatile__("mcr p15, 0, %0, c7, c0, 4" :: "r"(0));
}

#endif
