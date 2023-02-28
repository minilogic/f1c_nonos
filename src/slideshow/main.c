#include <stdio.h>
#include <malloc.h>
#include "sys.h"
#include "ff.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#include "stb_image.h"

u16 *fb;

/* Image Scaling With Bresenham: www.ddj.com/184405045 */
void ScaleLine(u16 *Target, u16 *Source, int SrcWidth, int TgtWidth)
{
  int NumPixels = TgtWidth;
  int IntPart = SrcWidth / TgtWidth;
  int FractPart = SrcWidth % TgtWidth;
  int E = 0;
  while (NumPixels-- > 0) {
    *Target++ = *Source;
    Source += IntPart;
    E += FractPart;
    if (E >= TgtWidth) {
      E -= TgtWidth;
      Source++;
    }
  }
}

void ScaleRect(u16 *Target, u16 *Source, int SrcWidth, int SrcHeight,
               int TgtWidth, int TgtHeight)
{
  int NumPixels = TgtHeight;
  int IntPart = (SrcHeight / TgtHeight) * SrcWidth;
  int FractPart = SrcHeight % TgtHeight;
  int E = 0;
  u16 *PrevSource = NULL;
  while (NumPixels-- > 0) {
    if (Source == PrevSource) {
      memcpy(Target, Target-TgtWidth, TgtWidth*sizeof(*Target));
    } else {
      ScaleLine(Target, Source, SrcWidth, TgtWidth);
      PrevSource = Source;
    }
    Target += TgtWidth;
    Source += IntPart;
    E += FractPart;
    if (E >= TgtHeight) {
      E -= TgtHeight;
      Source += SrcWidth;
    }
  }
}

int jpg_decode (u8* src, int len, u16* dst, int width, int height)
{
  int x, y, ch;
  u8  *raw = stbi_load_from_memory(src, len, &x, &y, &ch, 3);
  printf("Decode(%p): %dx%d\n", raw, x, y);
  if(raw == NULL) return 0;
  for(int i = 0; i < x * y; i++)
    ((u16*)raw)[i] = ((raw[i * 3 + 0] & 0xF8) << 8) |
             ((raw[i * 3 + 1] & 0xFC) << 3) | (raw[i * 3 + 2] >> 3);
  if(x == width && y == height) memcpy(dst, raw, x * y * 2);
  else ScaleRect(dst, (u16*)raw, x, y, width, height);
  stbi_image_free(raw);
  return 1;
}

int slideshow (char *path)
{
  u8  *fbuf;
  u32 fsize;
  DIR dir;
  FIL fil;
  FILINFO fno;
  UINT res;
  char name[256];
  sprintf(name, "%s", path);
  if(f_findfirst(&dir, &fno, name, "*.jpg") != FR_OK) return 1;
  if(!fno.fname[0]) return 2;
  do
  {
    sprintf(name, "%s/%s", path, fno.fname);
    if(f_open(&fil, name, FA_READ) == FR_OK)
    {
      fsize = f_size(&fil);
      fbuf = malloc(fsize);
      if(fbuf)
      {
        if(f_read(&fil, fbuf, fsize, &res) == FR_OK)
        {
          printf("File(%d): %s\n", fsize, fno.fname);
          if(jpg_decode(fbuf, fsize, fb, display->width, display->height))
            delay(500);
        }
        free(fbuf);
      }
      f_close(&fil);
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  } while(f_findnext(&dir, &fno) == FR_OK && fno.fname[0] && sd_card_detect());
  f_closedir(&dir);
  return 0;
}

int main (void)
{
  FATFS   fs;
  puts("\033[36mF1C100S - Slideshow ("__DATE__" "__TIME__")\033[0m");

  //disp_init(&TV_PAL, 0);
  //disp_init(&TV_NTSC, 0);
  disp_init(&TFT_800x480, 0);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);
  delay(100);
  disp_backlight(75);
  sd_init();
  disk_init(0, &sd_read, &sd_write);
  while(1)
  {
    if(sd_card_detect())
    {
      printf("Card inserted: %uMB\n", sd_card_init() / 2048);
      printf("SD-disk mount: ");
      if(f_mount(&fs, (TCHAR*)"0:", 1) != FR_OK) puts("error");
      else
      {
        printf("%s\n", fs.fs_type == 2 ? "FAT16" : fs.fs_type == 3 ? "FAT32" : "exFAT");
        while(sd_card_detect()) slideshow("0:/wallpapers");
        puts("Card removed");
      }
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
