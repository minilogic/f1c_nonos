#include <stdio.h>
#include <math.h>
#include "sys.h"
#include "gl.h"
#include "glu.h"
#include "zbuffer.h"

#define SPEED 0.025

GLfloat lightpos[4] = { 2.0, 0.0, -2.5, 1.0 };
GLfloat lightdirect[3] = { -0.0, -0.0, -0.7 };

float angle = 0.0;
float dangle = 0.0;

char logo[20][25] = {
  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0 },
  { 0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0 },
  { 2,2,2,2,2,2,2,2,1,2,1,2,1,2,1,2,1,2,2,2,2,2,2,2,2 },
  { 2,2,2,2,2,2,2,2,1,1,2,2,1,2,2,1,1,2,2,2,2,2,2,2,2 },
  { 2,2,2,2,2,2,2,2,1,2,1,2,1,2,1,2,1,2,2,2,2,2,2,2,2 },
  { 2,2,2,2,2,2,2,2,1,2,2,1,1,1,2,2,1,2,2,2,2,2,2,2,2 },
  { 2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2 },
  { 2,2,2,2,2,2,2,2,2,2,2,1,1,1,2,2,2,2,2,2,2,2,2,2,2 },
  { 2,2,2,2,2,2,2,2,2,2,2,1,1,1,2,2,2,2,2,2,2,2,2,2,2 },
  { 2,2,2,2,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,2,2,2,2 },
  { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
  { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 }
};

void DrawGL(float i) 
{
  float z;
  glLoadIdentity();
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glTranslatef(0.0, 0.0, i);
  glRotatef(10.0, 0.0, 0.0, 1.0);
  glRotatef(43.0, 0.0, 1.0, 0.0);
  glTranslatef(-0.4, -0.4, -0.1);
  for(int i=0;i<25;i++)
  {
    glTranslatef(0.045, 0.0, 0.0);
    glPushMatrix();
    z = 0.08*sin(angle);
    angle += 0.2;
    for(int j=19;j>=0;j--)
    {
      if(logo[j][i] == 1)	glColor3f(0.0, 0.0, 0.0);
      else if(logo[j][i] == 2) glColor3f(0.0, 0.9, 0.5);
      else glColor3f(0.0, 0.0, 0.9);
      glTranslatef(0.0, 0.045, 0.0);
      glBegin(GL_QUADS);
      glVertex3f(0.0,  -0.04, z);
      glVertex3f(0.04, -0.04, z);
      glVertex3f(0.04,  0.0, z);
      glVertex3f(0.0, 0.0, z);
      glEnd();
    }
    glPopMatrix();
  }
  angle = dangle;
}

int main (void)
{
  u16 *fb;
  float i = -5.0, j = SPEED;
  ZBuffer* frameBuffer = NULL;
  puts("\033[36mF1C100S - TinyGl Ukrainian Flag Demo\033[0m");

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
  glClearColor ( 40.0/255.0, 40.0/255.0, 40.0/255.0, 1.0);
  glEnable(GL_LIGHTING);
  glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.0);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightdirect);
  glEnable(GL_COLOR_MATERIAL);
  glClearDepth(1.0);
  glEnable( GL_CULL_FACE );
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, display->width, display->height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLfloat)display->width / display->height, 1.0, 40.0);
  glMatrixMode(GL_MODELVIEW);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     
  while(1)
  {
    DrawGL(i);
    dangle -= 0.15;
    ZB_copyFrameBuffer(frameBuffer, fb, display->width * sizeof(PIXEL));
    if(ctr_ms > 75)
    {
      ctr_ms = 0;
      if(i <= -3.0) j = SPEED;
      if(i >= -1.3) j = -SPEED;
      i += j;
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
