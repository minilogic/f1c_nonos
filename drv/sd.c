#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "sys.h"

#define CMD_LOAD          (1U << 31)
#define CMD_PRG_CLK       (1U << 21)
#define CMD_SEND_INIT_SEQ (1U << 15)
#define CMD_WAIT_PRE_OVER (1U << 13)
#define CMD_STOP_CMD_FLAG (1U << 12)
#define CMD_STREAM (1U << 11)
#define CMD_TRANS_WRITE   (1U << 10)
#define CMD_DATA_TRANS    (1U << 9)
#define CMD_CHK_RESP_CRC  (1U << 8)
#define CMD_LONG_RESP     (1U << 7)
#define CMD_RESP_RCV      (1U << 6)

#define RES_R1            (CMD_CHK_RESP_CRC | CMD_RESP_RCV)
#define RES_R2            (CMD_CHK_RESP_CRC | CMD_LONG_RESP | CMD_RESP_RCV)
#define RES_R3            (CMD_RESP_RCV)
#define RES_R4            (CMD_RESP_RCV)
#define RES_R5            (CMD_CHK_RESP_CRC | CMD_RESP_RCV)
#define RES_R6            (CMD_CHK_RESP_CRC | CMD_RESP_RCV)
#define RES_R7            (CMD_CHK_RESP_CRC | CMD_RESP_RCV)

struct {
  u32 rca;
  u32 cap;
  u32 ccs : 1;
  u32 det : 1;
} card;

void sd_deinit (void)
{
  CCU->SDMMC0_CLK = 0;
  PF->CFG0 = 0;
  card.cap = 0;
  card.det = 0;
  delay(50);
}

void sd_init (void)
{
  sd_deinit();
  PF->CFG0 = 0x222222;
  CCU->BUS_CLK_GATING0 |= 0x100;
  CCU->BUS_SOFT_RST0 |= 0x100;
  CCU->SDMMC0_CLK = 0x8003000F;
  SD0->GCTL = 0x107;
  delay(50);
}

int sd_card_detect (void)
{
  u32 ris = SD0->RIS & ((1 << 31) | (1 << 30));
  SD0->RIS = ris;
  if(ris & (1 << 31)) { sd_init(); }
  if(ris & (1 << 30)) card.det = 1;
  return card.det;
}

static int wait_status (u32 mask, u32 event)
{
  for(ctr_us = 0; ctr_us < 3000000; )
  {
    if(!sd_card_detect()) return -1;
    if((SD0->STA & mask) == event) return 0;
  }
  return -2;
}

static int wait_event (u32 event)
{
  u32 isr;
  for(ctr_us = 0; ctr_us < 100000; )
  {
    isr = SD0->RIS;
    if(isr & (1 << 31)) { sd_init(); return -1; }
    if(isr & event) return 0;
  }
  return -2;
}

static int cmd (u32 cmd, u32 arg)
{
  u32 res;
  if(!card.det) return -1;
  SD0->ARG = arg;
  SD0->CMD = cmd | CMD_LOAD;
  res = wait_event(4);
  SD0->RIS = 0xFFFFFFFF;
  return res;
}

int sd_card_init (void)
{
  int timeout;
  u32 arg;
  if(!card.cap)
  {
    SD0->CKC = 0;
    cmd(CMD_PRG_CLK | CMD_WAIT_PRE_OVER, 0);
    delay(10);
    SD0->CKC = 0x10000;
    cmd(CMD_PRG_CLK | CMD_WAIT_PRE_OVER, 0);
    SD0->BWD = 0;
    card.rca = 0;
    card.ccs = 0;
    timeout = 200;
    do
    {
      delay(1);
      cmd(CMD_SEND_INIT_SEQ, 0);  // GO_IDLE_STATE
      cmd(8 + RES_R7, 0x1AA);     // SEND_IF_COND
    } while(card.det && --timeout && (SD0->RESP0 & 0xFF) != 0xAA);
    arg = (SD0->RESP0 & 0xFF) == 0xAA ? 0x40FF8000 : 0x00FF8000;
    timeout = 1300;
    do
    {
      delay(1);
      cmd(55 + RES_R1, card.rca);
      cmd(41 + RES_R3, arg);      // SD_SEND_OP_COND
      if(--timeout == 0) return 0;//{ puts("timeout"); return 0; }
    } while(card.det && !(SD0->RESP0 & 0x80000000));
    if(SD0->RESP0 & 0x40000000 && arg & 0x40000000) card.ccs = 1;
    cmd(2 + RES_R2, 0);           // ALL_SEND_CID
    cmd(3 + RES_R6, 0);           //SEND_RELATIVE_ADDR
    card.rca = SD0->RESP0 & 0xFFFF0000;
    cmd(9 + RES_R2, card.rca);    // SEND_CSD
    if(SD0->RESP3 & 0x40000000)
    {
      arg = (SD0->RESP1 >> 16) | ((SD0->RESP2 & 0x3F) << 16);
      arg = (arg + 1) << 10;
    }
    else
    {
      arg = (SD0->RESP1 >> 30) | ((SD0->RESP2 & 0x3FF) << 2);
      arg = (arg + 1) << (((SD0->RESP1 & 0x38000) >> 15) + 2);
      arg <<= (((SD0->RESP2 >> 16) & 15) - 9);
    }
    cmd(7 + RES_R1, card.rca);    // SELECT/DESELECT_CARD (Standby to Transfer State)
    cmd(16 + RES_R1, 512);        // SET_BLOCKLEN
    cmd(55 + RES_R1, card.rca);
    cmd(6 + RES_R1, 2);           // SET_BUS_WIDTH (4bit SD bus)
    SD0->BWD = 1;
    CCU->SDMMC0_CLK = 0x80000000; // Speed up
    card.cap = card.det ? arg : 0;
    delay(10);
  }
  return card.cap;
}

int sd_read (void *ptr, u32 addr, u32 cnt)
{
  u32 ctr = cnt * 128, *buf = (u32*)ptr;
  if(!card.cap) return cnt;
  if((u32)ptr & 3)
  {
    printf("sd_read err: ptr is not aligned (%p %u %u)\n", ptr, addr, cnt);
    buf = malloc(cnt * 512);
    sd_read(buf, addr, cnt);
    memcpy(ptr, buf, cnt * 512);
    free(buf);
    return cnt;
  }
  SD0->GCTL &= ~0x100;
  SD0->BYC = cnt * 512;
  SD0->ARG = card.ccs ? addr : addr * 512;
  SD0->GCTL |= (1U << 31);
  SD0->CMD = cnt == 1 ? 17 | CMD_DATA_TRANS | CMD_WAIT_PRE_OVER | CMD_LOAD | RES_R1 :
    18 | CMD_DATA_TRANS | CMD_STOP_CMD_FLAG | CMD_WAIT_PRE_OVER | CMD_LOAD | RES_R1;
  do {
    if(wait_status(4, 0)) break;
    *buf++ = SD0->FIFO;
  } while(--ctr);
  wait_event(4);
  wait_event(cnt == 1 ? 1 << 3 : 1 << 14);
  SD0->RIS = 0xFFFFFFFF;
  SD0->GCTL |= 0x100;
  return cnt;
}

int sd_write (void *ptr, u32 addr, u32 cnt)
{
  puts("sd_write");
  while(1);
}
