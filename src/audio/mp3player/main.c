#include <stdio.h>
#include <stdlib.h>
#include "mp3dec.h"
#include "sys.h"
#include "ff.h"

int rnd_mode = 0;
int play_dir (char *dname);
void play_file (char *dname, char *fname);
void play_buf (u8 *iptr, int len);

int check_events (void)
{
  dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  return sd_card_detect();
}

void __attribute__((interrupt("IRQ"))) irq_handler (void)
{
  aud_handler();
}

int main (void)
{
  FATFS fs;
  puts(CURSOR_HIDE FG_CYAN  "F1C100S MP3 Player" ATTR_RESET);
  puts(FG_YELLOW "Usage:\n"
       "  'm' mute\n"
       "  '+' volume up\n"
       "  '-' volume down\n"
       "  '0' pause\n"
       "  '9' next track\n"
       "  's' enable/disable random mode" ATTR_RESET);
  sd_init();
  disk_init(0, &sd_read, &sd_write);
  ac.dac.volume = 50;
  while(1)
  {
    if(check_events())
    {
      printf("Card inserted: %uMB\n", sd_card_init() / 2048);
      printf("SD-disk mount: ");
      if(f_mount(&fs, (TCHAR*)"0:", 1) != FR_OK) puts("error");
      else
      {
        printf("%s\n", fs.fs_type == 2 ? "FAT16" : fs.fs_type == 3 ? "FAT32" : "exFAT");
        play_dir("0:/mp3");
        puts("Card removed" CLR_EOL);
      }
    }
  }
}

void play_buf (u8 *iptr, int len)
{
  int res, c, oidx = 0, ilen = len;
  s16 outbuf[2][1152 * 2 * 2];
  HMP3Decoder mp3dec;
  MP3FrameInfo mp3inf;
  mp3dec = MP3InitDecoder();
  while(ilen)
  {
    if(MP3GetNextFrameInfo(mp3dec, &mp3inf, iptr) < 0)
    {
      ilen--; iptr++;
      res = MP3FindSyncWord(iptr, ilen);
      if(res < 0) return;
      ilen -= res; iptr += res;
    }
    else
    {
      res = MP3Decode(mp3dec, &iptr, &ilen, &outbuf[oidx & 1][0], 0);
      led_set(LED_ENABLE);
      while(ac.dac.head != ac.dac.tail) IRQ_WAIT();
      led_set(LED_DISABLE);
      IRQ_DISABLE();
      ac.dac.buf = &outbuf[oidx][0];
      ac.dac.size = -1;
      ac.dac.tail = 0;
      ac.dac.head = mp3inf.outputSamps;
      IRQ_ENABLE();
      ac_enable(mp3inf.samprate, mp3inf.nChans);
      oidx ^= 1;
      printf("%05dHz %dkBps %03d%%\r", mp3inf.samprate, mp3inf.bitrate / 1000,
        (len - ilen) / (len / 100));
      c = kbhit() ? getchar() : 0;
      if(c == '0') while(getchar() != '0');
      if(c == '=' || c == '+') { ac.dac.volume++; ac_mixer_init(); };
      if(c == '-' || c == '_') { ac.dac.volume--; ac_mixer_init(); };
      if(c == 'm' || c == 'M') { ac.dac.mute ^= 1; ac_mixer_init(); };
      if(c == 's' || c == 'S') { rnd_mode ^= 1; c = '9'; }
      if(c == '9' || !check_events())
      {
        while(ac.dac.head != ac.dac.tail) IRQ_WAIT();
        return;
      }
    }
  }
}

void play_file (char *dname, char *fname)
{
  FIL fil;
  u8  *fbuf;
  char path[256];
  UINT res, fsize;
  sprintf(path, "%s/%s", dname, fname);
  if(f_open(&fil, path, FA_READ) == FR_OK)
  {
    fsize = f_size(&fil);
    fbuf = malloc(fsize);
    if(fbuf)
    {
      if(f_read(&fil, fbuf, fsize, &res) == FR_OK)
      {
        printf(CLR_LINE "Playback: " ATTR_RESET "%s\n", fname);
        play_buf(fbuf, fsize);
        printf(CLR_LINE CURSOR_UP CLR_LINE);
      }
      free(fbuf);
    }
    f_close(&fil);
  }
}

int play_dir (char *dname)
{
  DIR dir;
  FILINFO fno;
  static int i = 0;
  int fnum, j;
  if(f_findfirst(&dir, &fno, dname, "*.mp3") != FR_OK) return -1;
  if(!fno.fname[0]) return -2;
  for(fnum = 1; f_findnext(&dir, &fno) == FR_OK && fno.fname[0]; fnum++);
  printf("Found %d files\n", fnum);
  while(check_events())
  {
    i %= fnum;
    f_findfirst(&dir, &fno, dname, "*.mp3");
    for(j = i; j; j--) f_findnext(&dir, &fno);
    play_file(dname, fno.fname);
    for(j = i; j == i % fnum;)
    {
      if(rnd_mode)
      {
        srand(ctr_ms);
        i = rand();
      }
      else i++;
    }
  }
  return 0;
}
