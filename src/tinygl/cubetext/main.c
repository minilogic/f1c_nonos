#include <stdio.h>
#include "sys.h"
#include "gl.h"
#include "glu.h"
#include "zbuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#include "stb_image.h"
#include "girl.h"

#define CUBE_STEP 0.1
#define ZOOM      0.05

float angle = 0.0;

struct V3 {
  float v1;
  float v2;
  float v3;
} ptrv[8] = {
  {-1.0,1.0,1.0},
  {-1.0,-1.0,1.0},
  {1.0,-1.0,1.0},
  {1.0,1.0,1.0},
  {-1.0,1.0,-1.0},
  {-1.0,-1.0,-1.0},
  {1.0,-1.0,-1.0},
  {1.0,1.0,-1.0}
};

struct T2 {
  float t1;
  float t2;
} ptrt[4] = {
  {0.0, 0.0},
  {1.0, 0.0},
  {1.0, 1.0},
  {0.0, 1.0}
};                

void DrawQUADS(struct V3* ptr, int iv1, int iv2, int iv3, int iv4, struct T2* ptrt, int it1, int it2, int it3, int it4)
{
  glBegin(GL_QUADS);
  glTexCoord2fv((float*)&ptrt[it1]);
  glVertex3fv((float*)&ptr[iv1]);
  glTexCoord2fv((float*)&ptrt[it2]);
  glVertex3fv((float*)&ptr[iv2]);
  glTexCoord2fv((float*)&ptrt[it3]);
  glVertex3fv((float*)&ptr[iv3]);
  glTexCoord2fv((float*)&ptrt[it4]);
  glVertex3fv((float*)&ptr[iv4]);
  glEnd();
}
void DrawGL (float i) 
{
  glLoadIdentity();
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glTranslatef(0.0, 0.0, i);
  glRotatef(angle, 1.0, 0.0, 0.0);
  glRotatef(2.0*angle, 0.0, 1.0, 0.0);
  glRotatef(3.0*angle, 0.0, 0.0, 1.0);
  DrawQUADS((struct V3*)&ptrv,0,1,2,3,(struct T2*)&ptrt,3,0,1,2);
  DrawQUADS((struct V3*)&ptrv,0,3,7,4,(struct T2*)&ptrt,1,2,3,0);
  DrawQUADS((struct V3*)&ptrv,4,7,6,5,(struct T2*)&ptrt,2,3,0,1);
  DrawQUADS((struct V3*)&ptrv,5,6,2,1,(struct T2*)&ptrt,3,0,1,2);
  DrawQUADS((struct V3*)&ptrv,7,3,2,6,(struct T2*)&ptrt,3,0,1,2);
  DrawQUADS((struct V3*)&ptrv,5,1,0,4,(struct T2*)&ptrt,3,0,1,2);
 } 

int main (void)
{
  u16 *fb;
  GLuint TexObj;
  float i = -15.0, j = ZOOM;
  ZBuffer* frameBuffer = NULL;
  u32 ctr_zoom, ctr_1s, fps = 0;
  int x, y, ch;
  unsigned char* texture;
  puts("\033[36mF1C100S - TinyGl Textured Cube Demo\033[0m");

  //disp_init(&TV_NTSC, 0);
  disp_init(&TFT_800x480, 0);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);
  delay(100);
  disp_backlight(100);

  frameBuffer = ZB_open(display->width, display->height, ZB_MODE_5R6G5B, 0,
    NULL, NULL, NULL);
  glInit(frameBuffer);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearDepth(1.0);
  glEnable( GL_CULL_FACE );
  glEnable(GL_DEPTH_TEST);  
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glGenTextures(1, &TexObj);
  glBindTexture(GL_TEXTURE_2D, TexObj);
  texture = stbi_load_from_memory(girl_jpg, sizeof(girl_jpg), &x, &y, &ch, 3);
  glTexImage2D(GL_TEXTURE_2D, 0, ch, x, y, 0,  GL_RGB, GL_UNSIGNED_BYTE, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, TexObj);
  glEnable(GL_TEXTURE_2D);
  glViewport(0, 0, display->width, display->height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(35.0, (GLfloat)display->width / display->height, 1.0, 300.0);
  glMatrixMode(GL_MODELVIEW);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     
  ctr_zoom = ctr_ms + 75;
  ctr_1s = ctr_ms + 1000;
  while(1)
  {
    if(angle < 360.0) angle += 0.2;
    else angle = 0.0;  
    DrawGL(i);
    ZB_copyFrameBuffer(frameBuffer, fb, display->width * sizeof(PIXEL));
    fps++;
    if(ctr_ms >= ctr_zoom)
    {
      if(i <= -10.0) j = ZOOM;
      if(i >= -3.0) j = -ZOOM;
      i += j;
      ctr_zoom = ctr_ms + 75;
    }
    if(ctr_ms >= ctr_1s)
    {
      printf("fps:%d \r", fps);
      fps = 0;
      ctr_1s = ctr_ms + 1000;
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
