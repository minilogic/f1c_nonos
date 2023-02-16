#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "sys.h"
#include "gl.h"
#include "glu.h"
#include "zbuffer.h"

static GLint gear1, gear2, gear3;

static void gear( GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
  GLint teeth, GLfloat tooth_depth )
{
  GLint i;
  GLfloat r0, r1, r2;
  GLfloat angle, da;
  GLfloat u, v, len;
  r0 = inner_radius;
  r1 = outer_radius - tooth_depth/2.0;
  r2 = outer_radius + tooth_depth/2.0;
  da = 2.0*M_PI / teeth / 4.0;
  glShadeModel( GL_FLAT );
  glNormal3f( 0.0, 0.0, 1.0 );
  /* draw front face */
  glBegin( GL_QUAD_STRIP );
  for (i=0;i<=teeth;i++)
  {
    angle = i * 2.0*M_PI / teeth;
    glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
    glVertex3f( r1*cos(angle), r1*sin(angle), width*0.5 );
    glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
    glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
  }
  glEnd();
  /* draw front sides of teeth */
  glBegin( GL_QUADS );
  da = 2.0*M_PI / teeth / 4.0;
  for (i=0;i<teeth;i++)
  {
    angle = i * 2.0*M_PI / teeth;
    glVertex3f( r1*cos(angle),      r1*sin(angle),      width*0.5 );
    glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   width*0.5 );
    glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), width*0.5 );
    glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
  }
  glEnd();
  glNormal3f( 0.0, 0.0, -1.0 );
  /* draw back face */
  glBegin( GL_QUAD_STRIP );
  for (i=0;i<=teeth;i++)
  {
    angle = i * 2.0*M_PI / teeth;
    glVertex3f( r1*cos(angle), r1*sin(angle), -width*0.5 );
    glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
    glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
    glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
  }
  glEnd();
  /* draw back sides of teeth */
  glBegin( GL_QUADS );
  da = 2.0*M_PI / teeth / 4.0;
  for (i=0;i<teeth;i++)
  {
    angle = i * 2.0*M_PI / teeth;
    glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
    glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
    glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
    glVertex3f( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
  }
  glEnd();
   /* draw outward faces of teeth */
  glBegin( GL_QUAD_STRIP );
  for (i=0;i<teeth;i++)
  {
    angle = i * 2.0*M_PI / teeth;
    glVertex3f( r1*cos(angle),      r1*sin(angle),       width*0.5 );
    glVertex3f( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
    u = r2*cos(angle+da) - r1*cos(angle);
    v = r2*sin(angle+da) - r1*sin(angle);
    len = sqrt( u*u + v*v );
    u /= len;
    v /= len;
    glNormal3f( v, -u, 0.0 );
    glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),    width*0.5 );
    glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
    glNormal3f( cos(angle), sin(angle), 0.0 );
    glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da),  width*0.5 );
    glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
    u = r1*cos(angle+3*da) - r2*cos(angle+2*da);
    v = r1*sin(angle+3*da) - r2*sin(angle+2*da);
    glNormal3f( v, -u, 0.0 );
    glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da),  width*0.5 );
    glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
    glNormal3f( cos(angle), sin(angle), 0.0 );
  }
  glVertex3f( r1*cos(0.0), r1*sin(0.0), width*0.5 );
  glVertex3f( r1*cos(0.0), r1*sin(0.0), -width*0.5 );
  glEnd();
  glShadeModel( GL_SMOOTH );
  /* draw inside radius cylinder */
  glBegin( GL_QUAD_STRIP );
  for (i=0;i<=teeth;i++)
  {
    angle = i * 2.0*M_PI / teeth;
    glNormal3f( -cos(angle), -sin(angle), 0.0 );
    glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 );
    glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 );
  }
  glEnd();
}

void init( void )
{
   static GLfloat pos[4] = {5.0, 5.0, 10.0, 1.0 };
   static GLfloat red[4] = {0.8, 0.1, 0.0, 1.0 };
   static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0 };
   static GLfloat blue[4] = {0.2, 0.2, 1.0, 1.0 };
   glLightfv( GL_LIGHT0, GL_POSITION, pos );
   glEnable( GL_CULL_FACE );
   glEnable( GL_LIGHTING );
   glEnable( GL_LIGHT0 );
   glEnable( GL_DEPTH_TEST );
   /* make the gears */
   gear1 = glGenLists(1);
   glNewList(gear1, GL_COMPILE);
   glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red );
   gear( 1.0, 4.0, 1.0, 20, 0.7 );
   glEndList();
   gear2 = glGenLists(1);
   glNewList(gear2, GL_COMPILE);
   glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green );
   gear( 0.5, 2.0, 2.0, 10, 0.7 );
   glEndList();
   gear3 = glGenLists(1);
   glNewList(gear3, GL_COMPILE);
   glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue );
   gear( 1.3, 2.0, 0.5, 10, 0.7 );
   glEndList();
   glEnable( GL_NORMALIZE );
   glViewport(0, 0, (GLint)500, (GLint)480);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum( -1.0, 1.0, -1, 1, 5.0, 60.0 );
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, -40.0 );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );   
}

int main (void)
{
  u16 *fb;
  int fps = 0;
  ZBuffer* frameBuffer = NULL;
  GLfloat view_rotx=20.0, view_roty=30.0, view_rotz=0.0, angle = 0.0;
  puts("\033[36mF1C100S - TinyGl Gears Demo\033[0m");

  disp_init(&TV_NTSC, 0);
  //disp_init(&TFT_800x480, 0);
  disp_backlight(100);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);

  frameBuffer = ZB_open(display->width, display->height, ZB_MODE_5R6G5B, 0, NULL, NULL, NULL);
  glInit(frameBuffer);
  init();
  glViewport(0, 0, display->width, display->height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLfloat)display->width / display->height, 1.0, 60.0);
  glTranslatef( 0.0, 0.0, 20.0 );
  glMatrixMode(GL_MODELVIEW);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     
  while(1)
  {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();
    glRotatef( view_rotx, 1.0, 0.0, 0.0 );
    glRotatef( view_roty, 0.0, 1.0, 0.0 );
    glRotatef( view_rotz, 0.0, 0.0, 1.0 );
    glPushMatrix();
    glTranslatef( -2.0, -2.0, 0.0 );
    glRotatef( angle, 0.0, 0.0, 1.0 );
    glCallList(gear1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 4.1, -2.0, 0.0 );
    glRotatef( -2.0*angle-9.0, 0.0, 0.0, 1.0 );
    glCallList(gear2);
    glPopMatrix();
    glPushMatrix();
    glTranslatef( -2.1, 4.2, 0.0 );
    glRotatef( -2.0*angle-25.0, 0.0, 0.0, 1.0 );
    glCallList(gear3);
    glPopMatrix();
    glPopMatrix();
    angle += 0.5;
    if(angle >= 360.0) angle = 0.0; 
    ZB_copyFrameBuffer(frameBuffer, fb, display->width * sizeof(PIXEL));
    //view_rotx += 0.3;
    //view_roty += 0.3;
    view_rotz += 0.5;
    fps++;
    if(ctr_ms > 1000)
    {
      printf("fps:%d \r", fps);
      fps = 0;
      ctr_ms = 0;
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
