#include <stdio.h>
#include <math.h>
#include "sys.h"
#include "gl.h"
#include "glu.h"
#include "zbuffer.h"

float angle = 0.0;

GLfloat lightpos[4] = {0.0, 2.5, 0.0, 1.0};
GLfloat lightdirect[3] = {0.0, 0.0, 0.0};

#define DIM    6
#define DIM2   (1.0/(2.0*DIM))
#define T      1

float rad[2*DIM+1][2*DIM];

void DrawPlan(int zoom)
{
  float v1,v2,v3,v4, ang;
  int i,j,n;
  n = 1;
  i = 0;
  j = 0;
  glLoadIdentity();
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glTranslatef(0.0, 0.0, zoom);
  glRotatef(30.0, 1.0, 0.0, 0.0);
  glRotatef(angle, 0.0, 1.0, 0.0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightdirect);
  for (i = -DIM; i<DIM; i++)
  for (j = -DIM; j<=DIM; j++)
  {
    if (n ==1)
    {
      glColor3f(200.0/255.0, 40.0/255.0, 240.0/255.0); 
      n = 0;
    }
    else
    {
      glColor3f(0.8, 0.8, 0.8);
      n =1;
    }
    ang = 0.25*angle;
    v1 = 0.7*sin(rad[i+DIM][j+DIM+1]-ang);
    v2 = 0.7*sin(rad[i+DIM+1][j+DIM+1]-ang);
    v3 = 0.7*sin(rad[i+DIM+1][j+DIM]-ang);
    v4 = 0.7*sin(rad[i+DIM][j+DIM]-ang);
    glBegin(GL_QUADS);
    glTexCoord2f(DIM2*(DIM+i), DIM2*(DIM+j));
    glVertex3f ((float)i, v1, (float)j+1);
    glTexCoord2f(DIM2*(DIM+i+1), DIM2*(DIM+j));
    glVertex3f ((float)i+1, v2, (float)j+1);
    glTexCoord2f(DIM2*(DIM+i+1), DIM2*(DIM+j-1));
    glVertex3f ((float)i+1, v3, (float)j);
    glTexCoord2f(DIM2*(DIM+i), DIM2*(DIM+j-1));
    glVertex3f ((float)i, v4, (float)j);
    glEnd(); 
  }  
} 

#define ZOOM 0.2

int main (void)
{
  u16 *fb;
  float i = -50.0, j = ZOOM;
  ZBuffer* frameBuffer = NULL;
  puts("\033[36mF1C100S - TinyGl Wave Demo\033[0m");

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
  glClearColor (0, 0.0, 0.0, 1.0); 
  glEnable(GL_LIGHTING);
  glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.0);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
  glEnable(GL_LIGHT0);
  glNormal3f(0.0, 1.0, 0.0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glClearDepth(1.0);
  glEnable( GL_CULL_FACE );
  glEnable(GL_DEPTH_TEST);
  for (int i = -DIM; i<DIM; i++)
  {
    for (int j = -DIM; j<=DIM; j++)
    {
      rad[i+DIM][j+DIM] = T*sqrt(i*i+j*j);
      rad[i+DIM][j+DIM+1] = T*sqrt(i*i+(j+1)*(j+1));
    }
  }
  glViewport(0, 0, display->width, display->height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (GLfloat)display->width / display->height, 1.0, 300.0);
  glMatrixMode(GL_MODELVIEW);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );  
  glPolygonMode(GL_FRONT,GL_FILL);
  //glPolygonMode(GL_FRONT,GL_LINE);
  //glPolygonMode(GL_FRONT,GL_POINT);
  while(1)
  {
    angle += 0.5;
    if(angle == 360.0) angle = 0.0;  
    DrawPlan(i);
    ZB_copyFrameBuffer(frameBuffer, fb, display->width * sizeof(PIXEL));
    if(ctr_ms > 50)
    {
      ctr_ms = 0;
      if(i <= -50.0) j = ZOOM;
      if(i >= -10.0) j = -ZOOM;
      i += j;
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
