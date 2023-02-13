#ifndef SPI_H
#define SPI_H

void spi_init (void);
void spi_deinit (void);

int spi_flash_read (u32 addr, void *buf, u32 len);

#endif