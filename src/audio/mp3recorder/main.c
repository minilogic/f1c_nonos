#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "layer3.h"
#include "mp3dec.h"
#include "sys.h"
#include "ff.h"

#define MPEG_SN     576
#define MP3_PART    8192

void rec_enable (int sr, int br, int ch);
void rec_disable (FIL *fil);
void rec_handler (FIL *fil);

void play_buf (u8 *iptr, int len);
void play_file (char *dname, char *fname);

void __attribute__((interrupt("IRQ"))) irq_handler (void)
{
  aud_handler();
}

int check_events (void)
{
  dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  return sd_card_detect();
}

const int mp3sr[9] = { 48000,44100,32000,24000,22050,16000,12000,11025,8000 };

const int mp3br[9][16] = {
  { 0, 32, 40, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 }, /* 48000 */
  { 0, 32, 40, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 }, /* 44100 */
  { 0, 32, 40, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 }, /* 32000 */
  { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 }, /* 24000 */
  { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 }, /* 22050 */
  { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 }, /* 16000 */
  { 0, 8, 16, 24, 32, 40, 48, 56, 64, 0, 0, 0, 0, 0, 0, 0 },           /* 12000 */
  { 0, 8, 16, 24, 32, 40, 48, 56, 64, 0, 0, 0, 0, 0, 0, 0 },           /* 11025 */
  { 0, 8, 16, 24, 32, 40, 48, 56, 64, 0, 0, 0, 0, 0, 0, 0 }            /* 8000  */
};

int main (void)
{
  FIL fil;
  FATFS fs;
  u32 c, ctr_load, ctr_temp, sr = 0, br = 8;
  puts(CURSOR_HIDE FG_CYAN  "F1C100S MP3 Recorder\n"
       FG_YELLOW "Usage:\n"
       "  'r' change sample rate\n"
       "  'b' change MP3 bitrate\n"
       "  's' start/stop recording" ATTR_RESET);
  sd_init();
  disk_init(0, &sd_read, &sd_write);
  ac.adc.size = MPEG_SN * 256;
  ac.adc.buf = malloc(ac.adc.size * 2);
  ac.adc.mute_micin = 0;
  ac.adc.gain = 7;
  ac.adc.gain_micin = 7;
  ac.dac.volume = 50;
  while(!check_events())
  {
    printf(CLR_LINE "Please insert memory card\r");
    delay(100);
  }
  printf(CLR_LINE "Card inserted: %uMB\n", sd_card_init() / 2048);
  printf("SD-disk mount: ");
  if(f_mount(&fs, (TCHAR*)"0:", 1) != FR_OK)
  {
    puts("error");
    while(1);
  }
  else
  {
    printf("%s\n", fs.fs_type == 2 ? "FAT16" : fs.fs_type == 3 ? "FAT32" : "exFAT");
    while(1)
    {
      do
      {
        printf(FG_GREEN "SampleRate:%5dHz BitRate:%2dkbit/sec\r" ATTR_RESET,
          mp3sr[sr], mp3br[sr][br]);
        c = getchar();
        if(c == 'r' || c == 'R') if(++sr > 8) sr = 0;
        if(c == 'b' || c == 'B') if(++br > 15) br = 0;
        if(!mp3br[sr][br]) br = 1;
      } while(c != 's' && c != 'S');
      ac_enable(mp3sr[sr], 1);
      if(f_open(&fil, "record.mp3", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
      {
        ctr_ms = 0;
        ctr_load = 0;
        rec_enable(mp3sr[sr], mp3br[sr][br], 1);
        printf(CLR_LINE FG_RED "Recording %5dHz %2dkbit/sec\n" ATTR_RESET,
          mp3sr[sr], mp3br[sr][br]);
        while(!kbhit())
        {
          IRQ_WAIT();
          ctr_temp = ctr_ms;
          rec_handler(&fil);
          ctr_load += ctr_ms - ctr_temp;
          printf("CPU_Load:%02d%% %02d:%02d\r", (ctr_load * 100) / ctr_ms,
            ctr_ms / 60000, ctr_ms / 1000);
        }
        printf(CLR_LINE CURSOR_UP CLR_LINE);
        printf("SampleRate:%5dHz BitRate:%2dkbit/sec CPU_Load:%d%%\n",
          mp3sr[sr], mp3br[sr][br], (ctr_load * 100) / ctr_ms);
        rec_disable(&fil);
        f_close(&fil);
        getchar();
        puts(FG_RED "Playback" ATTR_RESET);
        play_file("0:", "record.mp3");
        printf(CLR_LINE CURSOR_UP CLR_LINE);
      }
    }
  }
}

struct {
  shine_t enc;
  u8 buf[MP3_PART * 8];
  int head;
  int tail;
} mp3;

void rec_disable (FIL *fil)
{
  if(mp3.enc == NULL) return;
  shine_close(mp3.enc);
  int res = mp3.head >= mp3.tail ? mp3.head - mp3.tail :
    sizeof(mp3.buf) - mp3.tail + mp3.head;
  led_set(LED_ENABLE);
  f_write(fil, &mp3.buf[mp3.tail], res, NULL);
  led_set(LED_DISABLE);
  mp3.enc = NULL;
}

void rec_enable (int sr, int br, int ch)
{
  shine_config_t config;
  //rec_disable();
  shine_set_config_mpeg_defaults(&config.mpeg);
  config.mpeg.mode = ch == 1 ? MONO : STEREO;
  config.mpeg.bitr = br;
  config.mpeg.emph = NONE;
  config.mpeg.copyright = 0;
  config.mpeg.original = 1;
  config.wave.channels = 1;
  config.wave.samplerate = sr;
  shine_check_config(config.wave.samplerate, config.mpeg.bitr);
  IRQ_DISABLE();
  ac.adc.head = 0;
  ac.adc.tail = 0;
  IRQ_ENABLE();
  mp3.head = 0;
  mp3.tail = 0;
  mp3.enc = shine_initialise(&config);
}

void rec_handler (FIL *fil)
{
  unsigned char *ptr;
  int head, tail, sn, res;
  IRQ_DISABLE();
  head = ac.adc.head;
  tail = ac.adc.tail;
  IRQ_ENABLE();
  res = head >= tail ? head - tail : ac.adc.size - tail + head;
  sn = ac.rate > 24000 ? MPEG_SN * 2 : MPEG_SN; // mpeg1 sn = 576 * 2
  if(res >= sn)
  {
    ptr = shine_encode_buffer_interleaved(mp3.enc, &ac.adc.buf[ac.adc.tail], &res);
    tail += sn;
    if(tail == ac.adc.size) tail = 0;
    IRQ_DISABLE();
    ac.adc.tail = tail;
    IRQ_ENABLE();
    if(mp3.head + res > sizeof(mp3.buf))
    {
      memcpy(&mp3.buf[mp3.head], ptr, sizeof(mp3.buf) - mp3.head);
      res -= (sizeof(mp3.buf) - mp3.head);
      ptr += (sizeof(mp3.buf) - mp3.head);
      memcpy(&mp3.buf[0], ptr, res);
      mp3.head = res;
    }
    else
    {
      memcpy(&mp3.buf[mp3.head], ptr, res);
      mp3.head += res;
      if(mp3.head == sizeof(mp3.buf)) mp3.head = 0;
    }
    res = mp3.head >= mp3.tail ? mp3.head - mp3.tail :
      sizeof(mp3.buf) - mp3.tail + mp3.head;
    if(res >= MP3_PART)
    {
      led_set(LED_ENABLE);
      f_write(fil, &mp3.buf[mp3.tail], MP3_PART, NULL);
      mp3.tail += MP3_PART;
      if(mp3.tail == sizeof(mp3.buf)) mp3.tail = 0;
      led_set(LED_DISABLE);
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
      //if(c == 'm' || c == 'M') { ac.dac.mute ^= 1; ac_mixer_init(); };
      //if(c == 's' || c == 'S') { rnd_mode ^= 1; c = '9'; }
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
        //printf(CLR_EOL "%s\n", fname);
        play_buf(fbuf, fsize);
      }
      free(fbuf);
    }
    f_close(&fil);
  }
}
