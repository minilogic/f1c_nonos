#include <stdio.h>
#include "sys.h"

volatile int timer_event = 0;

void __attribute__((interrupt("IRQ"))) irq_handler (void)
{
  static int timer_count = 1000;
  if(TIM->IS)
  {
    TIM->IS = 1;
    if(--timer_count == 0)
    {
      timer_count = 1000;
      timer_event = 1;
    }
  }
}

void tim_init (void)
{
  INT->BASE_ADDR = 0;
  INT->MASK[0] = ~(1 << IRQ_TIM0);
  INT->MASK[1] = 0xFFFFFFFF;
  INT->EN[0] = (1 << IRQ_TIM0);
  INT->EN[1] = 0;
  TIM->T0_INTV = 3000;  // 24MHz / 8 = 3MHz / 3000 = 1000Hz(1mS)
  TIM->T0_CURV = 0;
  TIM->T0_CTRL = 0x35;
  TIM->IE = 1;
}

int main (void)
{
  puts("\e[36mF1C100S IRQ Example\e[0m");
  tim_init();
  __asm__ __volatile__ ("msr cpsr_c,#0x53\n");  // SVC mode, IRQ enabled
  while(1)
  {
    if(timer_event)
    {
      timer_event = 0;
      putchar('*');
    }
  }
}
