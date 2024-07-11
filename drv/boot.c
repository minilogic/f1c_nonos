#include "sys.h"

void __attribute__((naked)) _boot (void)
{
  __asm__ __volatile__ (
    /* Loader structure */
    "b      _res\n"                   // [00] jump to reset vector
    ".long  0x4E4F4765\n"             // [04] eGON
    ".long  0x3054422E\n"             // [08] .BT0
    ".long  0x00000000\n"             // [12] checksum for boot0
    ".long  __spl_size\n"             // [16] length for boot0
    ".long  0x024c5053\n"             // [20] SPL
    ".long  0, 0\n"                   // [24], [28]
    ".long  0, 0, 0, 0, 0, 0, 0, 0\n" // [32]..[60]
    ".long  0, 0, 0, 0\n"             // [64]..[76]
    /* Save boot params */
    "_res:  mov r0, #64\n"
    "mrc    p15, 0, r1, c1, c0, 0\n"  // [64] = CP15 SCTLR Register
    "mrc    p15, 0, r2, c1, c0, 0\n"  // [68] = CP15 Control Register
    "stmia  r0, {r1-r2, sp, lr}\n"    // [72] = SP, [76] = LR
    "bl     boot\n"
    /* Return to FEL */
    "mov    r0, #64\n"
    "ldmia  r0, {r1-r4}\n"
    "mov    lr, r4\n"
    "mov    sp, r3\n"
    "mcr    p15, 0, r2, c1, c0, 0\n"
    "mcr    p15, 0, r1, c1, c0, 0\n"
    "bx     lr\n"
    );
}

static inline void sdelay(int loops)
{
  __asm__ __volatile__ (
    "1:subs %0, %1, #1\n"
    "bne    1b":"=r" (loops):"0"(loops));
}

void put_char (char c)
{
  if(c == '\n') uart_put(SYS_UART_NUM, '\r');
  uart_put(SYS_UART_NUM, c);
}

void put_string (char *ptr)
{
  while(*ptr != 0) put_char(*ptr++);
}

void put_num (unsigned int num)
{
  int i;
  char str[16];
  if(num)
  {
    for(i = 0; num; num /= 10) str[i++] = num % 10 + '0';
    while(i) put_char(str[--i]);
  }
  else put_char('0');
  put_char('\n');
}

void sys_dram_init(void);

void boot (void)
{
  dev_enable(1);
  /* System clock initialization */
  CCU->PLL_STABLE0 = 0x1FF;
  CCU->PLL_STABLE1 = 0x1FF;
  CCU->CPU_CLK_SRC = 0x10000;
  sdelay(100);
  CCU->PLL_PERIPH_CTRL = 0x80041700;  // PERIPH: 576MHz
  sdelay(100);
  CCU->AHB_APB_CFG = 0x00003180;      // AHB:192MHz, APB:96MHz
  sdelay(100);
  CCU->DRAM_GATING |= (1 << 26) | (1 << 24);
  sdelay(100);
  for(CCU->PLL_CPU_CTRL = 0x80001700; !(CCU->PLL_CPU_CTRL & (1 << 28)); ) {};
  CCU->CPU_CLK_SRC = 0x20000;         // CPU:576MHz
  sdelay(100);
  /* UART initialization */
  uart_init(SYS_UART_NUM, (struct UART_CFG) { .port = SYS_UART_PORT,
    .bitrate = UART_BR(115200), .parity = UART_PAR_NO, .stop = UART_STP_1,
    .lenght = UART_8b });
  /* DDR initialization */
  sys_dram_init();
  /* Firmware loading */
  if(*(unsigned int*)8 != 0x4c45462e)
  {
    /* SPI NOR initialization */
    put_string("\n\n\033[36mSPI-boot\033[0m\nImage size: ");
    spi_init();
    spi_flash_read(8192, (void*)0x80000000, 32);
    put_num(*(int*)0x80000014);
    spi_flash_read(8192, (void*)0x80000000, *(int*)0x80000014);
    ((void(*)())0x80000000)();
  }
  else put_string("\n\n\033[36mUSB-boot\033[0m\n");
}
