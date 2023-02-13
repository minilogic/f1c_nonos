#include "sys.h"

/******************************************************************************/
/*                                SPI0 (FLASH)                                */
/******************************************************************************/
#define spi_flash_select() SPI0->TC = 0x44
#define spi_flash_deselect() SPI0->TC = 0xC4

void spi_init (void)
{
  CCU->BUS_CLK_GATING0 |= (1 << 20);  // SPI0
  CCU->BUS_SOFT_RST0 &= ~(1 << 20);
  CCU->BUS_SOFT_RST0 |= (1 << 20);
  PC->CFG0 = 0x2222;                  // PC0-CLK, PC1-CS, PC2-MISO, PC3-MOSI
  SPI0->CC = 0x1001;                  // AHB / 4 = 48MHz
  for(SPI0->GC = 0x80000083; SPI0->GC & 0x80000000;) {};
  spi_flash_deselect();
  for(SPI0->FC = 0x80408001; SPI0->FC & 0x80008000;) {};
}

void spi_deinit (void)
{
  CCU->BUS_CLK_GATING0 &= ~(1 << 20);
  PC->CFG0 = 0x7777;
}

int spi_flash_read (u32 addr, void *buf, u32 len)
{
  u8 i, *rx = (u8*)buf;
  if(!buf || !len) return 1;
  spi_flash_select();
  SPI0->MBC = 4;
  SPI0->MTC = 4;
  SPI0->BCC = 4;
  SPI0->TX.word = __builtin_bswap32((addr & 0x00FFFFFF) | 0x03000000);
  for(SPI0->TC |= (1U << 31); SPI0->TC & (1U << 31); ) {};
  SPI0->RX.word;
  do
  {
    i = len > 64 ? 64 : len;
    SPI0->MBC = i;
    SPI0->TC |= (1U << 31);
    len -= i;
    do
    {
      if(SPI0->FS & 0xFF)
      {
        *rx++ = SPI0->RX.byte;
        i--;
      }
    } while(i);
  } while(len);
  spi_flash_deselect();
  return 0;
}
