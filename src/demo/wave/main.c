/* Graphical demo based on https://www.youtube.com/watch?v=wyuJ-dqS2to */

#include <stdio.h>
#include <math.h>
#include "sys.h"

#define RED_COLORS    32
#define GREEN_COLORS  64
#define BLUE_COLORS   32
#define REDSHIFT 11
#define GREENSHIFT 5
#define BLUESHIFT 0
#define N 1024 // Number of dots
#define SCALE 8192
#define INCREMENT 512 // INCREMENT = SCALE / sqrt(N) * 2
#define SPEED 10
#define PI2 6.283185307179586476925286766559

u16 *fb;
int16_t sine[SCALE];
int16_t cosi[SCALE];

void initialize() {
 uint16_t i;
 for(i = 0; i < SCALE; i++) {
  sine[i] = (int)(sin(PI2 * i / SCALE) * SCALE);
  cosi[i] = (int)(cos(PI2 * i / SCALE) * SCALE);
 }
}

void matrix(int xyz[3][N], char rgb[3][N]) {
 static uint32_t t = 0;
 uint16_t i;
 int16_t x = -SCALE;
 int16_t y = -SCALE;
 uint16_t d;
 uint16_t s;
 for(i = 0; i < N; i++) {
  xyz[0][i] = x;
  xyz[1][i] = y;
  d = sqrt(x * x + y * y);
  s = sine[(t * 30) % SCALE] + SCALE;
  xyz[2][i] = sine[(d + s) % SCALE] *
              sine[(t * 10) % SCALE] / SCALE / 2;
  rgb[0][i] = (cosi[xyz[2][i] + SCALE / 2] + SCALE) *
              (RED_COLORS - 1) / SCALE / 2;
  rgb[1][i] = (cosi[(xyz[2][i] + SCALE / 2 + 2 * SCALE / 3) % SCALE] + SCALE) *
              (GREEN_COLORS - 1) / SCALE / 2;
  rgb[2][i] = (cosi[(xyz[2][i] + SCALE / 2 + SCALE / 3) % SCALE] + SCALE) *
              (BLUE_COLORS - 1) / SCALE / 2;
  x += INCREMENT;
  if(x >= SCALE) {
   x = -SCALE;
   y += INCREMENT;
  }
 }
 t++;
}

void rotate(int xyz[3][N], char rgb[3][N],
            uint16_t angleX, uint16_t angleY, uint16_t angleZ) {
 uint16_t i;
 int16_t tmpX;
 int16_t tmpY;
 int16_t sinx = sine[angleX];
 int16_t cosx = cosi[angleX];
 int16_t siny = sine[angleY];
 int16_t cosy = cosi[angleY];
 int16_t sinz = sine[angleZ];
 int16_t cosz = cosi[angleZ];

 for(i = 0; i < N; i++) {
  tmpX      = (xyz[0][i] * cosx - xyz[2][i] * sinx) / SCALE;
  xyz[2][i] = (xyz[0][i] * sinx + xyz[2][i] * cosx) / SCALE;
  xyz[0][i] = tmpX;

  tmpY      = (xyz[1][i] * cosy - xyz[2][i] * siny) / SCALE;
  xyz[2][i] = (xyz[1][i] * siny + xyz[2][i] * cosy) / SCALE;
  xyz[1][i] = tmpY;

  tmpX      = (xyz[0][i] * cosz - xyz[1][i] * sinz) / SCALE;
  xyz[1][i] = (xyz[0][i] * sinz + xyz[1][i] * cosz) / SCALE;
  xyz[0][i] = tmpX;
 }
}


void plot(int cx, int cy, int radius, u16 color)
{
  u16 *addr = fb + display->width * cy + cx;
  for(int j = 0; j < radius; j++)
  {
    if((cy + j) < display->height)
    {
      for(int i = 0; i < radius; i++) if((cx + i) < display->width)
        addr[j * display->width + i] = color;
    }
  }
}

static uint8_t pix_size = 2;

void draw(int xyz[3][N], char rgb[3][N]) {
  static uint16_t oldProjX[N] = {0};
  static uint16_t oldProjY[N] = {0};
  static uint8_t oldDotSize[N] = {0};
  uint16_t i, projX, projY, projZ, dotSize;
  if(pix_size == 255) pix_size = 0;
  else if(pix_size > 7) pix_size = 7;
  for(i = 0; i < N; i++)
  {
    projZ   = SCALE - (xyz[2][i] + SCALE) / 4;
    projX   = display->width / 2 + (xyz[0][i] * projZ / SCALE) / 25;
    projY   = display->height / 2 + (xyz[1][i] * projZ / SCALE) / 25;
    dotSize = pix_size + 3 - (xyz[2][i] + SCALE) * 2 / SCALE;
    plot(oldProjX[i], oldProjY[i], oldDotSize[i], 0);
    if(projX > dotSize && projY > dotSize && projX < display->width - dotSize &&
       projY < display->height - dotSize)
    {
      plot(projX, projY, dotSize, (rgb[0][i] << REDSHIFT) +
                                  (rgb[1][i] << GREENSHIFT) +
                                  (rgb[2][i] << BLUESHIFT));
      oldProjX[i] = projX;
      oldProjY[i] = projY;
      oldDotSize[i] = dotSize;
    }
  }
}

int main (void)
{
  char rgb[3][N];
  int key, xyz[3][N], angleX = 0, angleY = 7500, angleZ = 7500,
    speedX = 0, speedY = 0, speedZ = 0;
  puts("\033[36mF1C100S - wave demo ("__DATE__" "__TIME__")\033[0m");
  led_set(LED_ENABLE);

  //disp_init(&TV_NTSC, 0);
  disp_init(&TFT_800x480, 0);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);
  delay(100);
  disp_backlight(100);

  initialize();
  while(1)
  {
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
    matrix(xyz, rgb);
    rotate(xyz, rgb, angleX, angleY, angleZ);
    draw(xyz, rgb);
    if(kbhit())
    {
      key = getchar();
      if(key == '4') speedX -= SPEED;
      if(key == '6') speedX += SPEED;
      if(key == '8') speedY -= SPEED;
      if(key == '2') speedY -= SPEED;
      if(key == '5') speedZ -= SPEED;
      if(key == '7') pix_size++;
      if(key == '1') pix_size--;
      if(key == '*') angleX = angleY = angleZ = speedX = speedY = speedZ = 0;
      printf("X=%d Y=%d Z=%d\n", angleX, angleY, angleZ);
    }
    else
    {
      if(speedX > 0) speedX -= SPEED;
      else if(speedX < 0) speedX += SPEED;
      if(speedY > 0) speedY -= SPEED;
      else if(speedY < 0) speedY += SPEED;
      if(speedZ > 0) speedZ -= SPEED;
      else if(speedZ < 0) speedZ += SPEED;
    }
    angleX += speedX;
    angleY += speedY;
    angleZ += speedZ;
    if(angleX >= SCALE) angleX -= SCALE;
    else if(angleX < 0) angleX += SCALE;
    if(angleY >= SCALE) angleY -= SCALE;
    else if(angleY < 0) angleY += SCALE;
    if(angleZ >= SCALE) angleZ -= SCALE;
    else if(angleZ < 0) angleZ += SCALE;
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
    delay(25);
  }
}

