#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sys.h"
#include "gl.h"
#include "glu.h"
#include "zbuffer.h"
#include "md2.h"

vec3_t anorms_table[162] = {
#include "anorms.h"
};

void RenderFrameItpWithGLCmds (int n, float interp, md2_model_t *mdl)
{
  int i, *pglcmds;
  vec3_t v_curr, v_next, v, norm;
  float *n_curr, *n_next;
  md2_frame_t *pframe1, *pframe2;
  md2_vertex_t *pvert1, *pvert2;
  md2_glcmd_t *packet;
  /* check if n is in a valid range */
  if((n < 0) || (n > (mdl->header->num_frames - 2))) return;
  /* enable model's texture */
  glBindTexture (GL_TEXTURE_2D, mdl->tex_id);
  //glEnable(GL_TEXTURE_2D);
  /* pglcmds points at the start of the command list */
  pglcmds = mdl->glcmds;
  /* draw the model */
  while((i = *(pglcmds++)) != 0)
  {
    if(i < 0)
    {
      glBegin (GL_TRIANGLE_FAN);
      i = -i;
    }
    else glBegin (GL_TRIANGLE_STRIP);
    /* draw each vertex of this group */
    for(/* nothing */; i > 0; --i, pglcmds += 3)
    {
      packet = (md2_glcmd_t *)pglcmds;
      pframe1 = &mdl->frames[n];
      pframe2 = &mdl->frames[n + 1];
      pvert1 = &pframe1->verts[packet->index];
      pvert2 = &pframe2->verts[packet->index];
      /* pass texture coordinates to OpenGL */
      glTexCoord2f (packet->s, packet->t);
      /* interpolate normals */
      n_curr = anorms_table[pvert1->normalIndex];
      n_next = anorms_table[pvert2->normalIndex];
      norm[0] = n_curr[0] + interp * (n_next[0] - n_curr[0]);
      norm[1] = n_curr[1] + interp * (n_next[1] - n_curr[1]);
      norm[2] = n_curr[2] + interp * (n_next[2] - n_curr[2]);
      glNormal3fv (norm);
      /* interpolate vertices */
      v_curr[0] = pframe1->scale[0] * pvert1->v[0] + pframe1->translate[0];
      v_curr[1] = pframe1->scale[1] * pvert1->v[1] + pframe1->translate[1];
      v_curr[2] = pframe1->scale[2] * pvert1->v[2] + pframe1->translate[2];
      v_next[0] = pframe2->scale[0] * pvert2->v[0] + pframe2->translate[0];
      v_next[1] = pframe2->scale[1] * pvert2->v[1] + pframe2->translate[1];
      v_next[2] = pframe2->scale[2] * pvert2->v[2] + pframe2->translate[2];
      v[0] = v_curr[0] + interp * (v_next[0] - v_curr[0]);
      v[1] = v_curr[1] + interp * (v_next[1] - v_curr[1]);
      v[2] = v_curr[2] + interp * (v_next[2] - v_curr[2]);
      glVertex3fv (v);
    }
    glEnd();
  }
}

#define MAXLOADMODEL 2

md2_model_t mdl[MAXLOADMODEL];
irgb_t      irgb[MAXLOADMODEL];

int load_md2 (const unsigned char *md2, md2_model_t *mdl)
{
  unsigned char *ptr;
  mdl->header = (md2_header_t*)md2;
  if((mdl->header->ident != 844121161) || (mdl->header->version != 8)) return 0;
  mdl->skins = (md2_skin_t*)&md2[mdl->header->offset_skins];
  mdl->texcoords = (md2_texCoord_t*)&md2[mdl->header->offset_st];
  mdl->triangles = (md2_triangle_t*)&md2[mdl->header->offset_tris];
  mdl->glcmds = (int*)&md2[mdl->header->offset_glcmds];
  mdl->frames = malloc(sizeof(md2_frame_t) * mdl->header->num_frames);
  ptr = (unsigned char*)&md2[mdl->header->offset_frames];
  for(int i = 0; i < mdl->header->num_frames; i++)
  {
    memcpy(&mdl->frames[i].scale, ptr, sizeof(vec3_t));
    ptr += sizeof(vec3_t);
    memcpy(&mdl->frames[i].translate, ptr, sizeof(vec3_t));
    ptr += sizeof(vec3_t);
    memcpy(&mdl->frames[i].name, ptr, sizeof(char) * 16);
    ptr += sizeof(char) * 16;
    mdl->frames[i].verts = malloc(sizeof(md2_vertex_t) * mdl->header->num_vertices);
    memcpy(mdl->frames[i].verts, ptr, sizeof(md2_vertex_t) * mdl->header->num_vertices);
    ptr += sizeof(md2_vertex_t) * mdl->header->num_vertices;
  }
  return 1;
}

int load_ppm (char *ppm, irgb_t *irgb)
{
  if(*ppm++ != 'P' || *ppm++ != '6' || *ppm++ != '\n') return 0;
  if(!atoi(ppm)) while(*ppm++ != '\n');
  irgb->xsize = atoi(ppm);
  while(*ppm++ != ' ');
  irgb->ysize = atoi(ppm);
  while(*ppm++ != '\n');
  if(atoi((char*)ppm) != 255) return 0;
  while(*ppm++ != '\n');
  irgb->image = ppm;
  return irgb->xsize != 0 && irgb->ysize != 0;
}

int load_model (int i, const unsigned char *md2, const unsigned char *ppm)
{
  if(!load_md2(md2, &mdl[i])) return 0;
  if(!load_ppm((char*)ppm, &irgb[i])) return 0;
  glGenTextures(1, &mdl[i].tex_id);	
  glBindTexture(GL_TEXTURE_2D, mdl[i].tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, irgb[i].xsize, irgb[i].ysize, 0,
    GL_RGB, GL_UNSIGNED_BYTE, irgb[i].image);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  return 1;
}

GLfloat lightpos[4] = {0.0, 0.0, 0.2, 1.0};
GLfloat lightdirect[3] = {-0.5, -0.6, -0.7};

extern const unsigned char buf0_md2[];
extern const unsigned char buf0_ppm[];

extern const unsigned char buf1_md2[];
extern const unsigned char buf1_ppm[];

extern const unsigned char buf_md2[];
extern const unsigned char buf_ppm[];

int main (void)
{
  u16 *fb;
  int fps = 0, pre_ms = 0, text = 0, poly = 0, anim = 0, anim_en = 1, anim_fps = 16;
  float dist = 10.0, dy = -5.0, interp=0.2, angle_light = 0.0, angle = 45.0;
  ZBuffer* frameBuffer = NULL;
  puts("\033[36mF1C100S - TinyGl MD2 Viewer\033[0m");

  //disp_init(&TV_NTSC, 0);
  disp_init(&TFT_800x480, 0);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);
  delay(100);
  disp_backlight(100);

  frameBuffer = ZB_open(display->width, display->height, ZB_MODE_5R6G5B, 0, NULL, NULL, NULL);
  glInit(frameBuffer);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glClearColor (0.1, 0.1, 0.3, 1.0); 
  glEnable(GL_LIGHTING);
  glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.0);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0);//110.0);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightdirect); 
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glClearDepth(1.0);
  glFrontFace(GL_CW);
  glEnable( GL_CULL_FACE );
  glEnable(GL_DEPTH_TEST);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glEnable(GL_TEXTURE_2D);
  if(!load_model(0, buf0_md2, buf0_ppm) || !load_model(1, buf1_md2, buf1_ppm))
  {
    puts("Model_0 loading error!");
    while(1);
  }
  glViewport(0, 0, display->width, display->height); // -20
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50.0, (GLfloat)display->width / display->height, 1.0, 400.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );   
  glPolygonMode(GL_FRONT,GL_FILL);
  while(1)
  {
    glLoadIdentity();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glTranslatef(0.0, dy, dist);
    glPushMatrix(); 
    glRotatef(-6.0*angle_light, 0.0, 1.0, 0.0);
    angle_light += 1.0 / (float)(fps + 1);
    if(angle_light >= 360.0) angle_light = 0.0;
    glTranslatef(0.0, 45.0, -90.0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glPopMatrix();
    glPushMatrix(); 
    glTranslatef(0.0, 0.0, -90.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0); 
    glRotatef(-90.0, 0.0, 0.0, 1.0); 
    glRotatef(angle, 0.0, 0.0, 1.0);
    RenderFrameItpWithGLCmds(anim, interp, &mdl[0]);
    RenderFrameItpWithGLCmds(anim, interp, &mdl[1]);
    if(anim_en)
    {
      interp += (float)(ctr_ms - pre_ms) / (float)(1000 / anim_fps);
      if(interp>=1.0)
      {
        interp = 0.0;
        if(++anim >= mdl[0].header->num_frames-1) anim = 0;
      }
    }
    glPopMatrix();
    ZB_copyFrameBuffer(frameBuffer, fb, display->width * sizeof(PIXEL));
    fps++;
    if(ctr_ms > 1000)
    {
      printf("fps:%d \r", fps);
      fps = 0;
      ctr_ms = 0;
    }
    pre_ms = ctr_ms;
    if(kbhit())
    {
      char c = getchar();
      if(c == '+') dist += 1.0;
      if(c == '-') dist -= 1.0;
      if(c == '2') dy += 1.0;
      if(c == '8') dy -= 1.0;
      if(c == '6') angle += 2.0;
      if(c == '4') angle -= 2.0;
      if(c == '5') anim_en ^= 1;
      if(c == '3') if(anim_fps < 20) anim_fps++;
      if(c == '1') if(anim_fps > 1) anim_fps--;
      if(c == '7')
      {
        if(++text & 1) glDisable(GL_TEXTURE_2D);
        else glEnable(GL_TEXTURE_2D);
      }
      if(c == '9')
      {
        if(++poly & 1) glPolygonMode(GL_FRONT,GL_LINE);
        else glPolygonMode(GL_FRONT,GL_FILL);
      }
    }
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
