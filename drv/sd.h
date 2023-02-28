#ifndef SD_H
#define SD_H

void sd_init (void);
void sd_deinit (void);
int sd_card_detect (void);
int sd_card_init (void);
int sd_read (void *ptr, u32 addr, u32 cnt);
int sd_write (void *ptr, u32 addr, u32 cnt);

#endif