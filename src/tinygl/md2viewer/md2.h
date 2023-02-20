#ifndef MD2_H
#define MD2_H

typedef struct
{
    int     ident;              // magic number. must be equal to "IPD2"
    int     version;            // md2 version. must be equal to 8

    int     skinwidth;          // width of the texture
    int     skinheight;         // height of the texture
    int     framesize;          // size of one frame in bytes

    int     num_skins;          // number of textures
    int     num_vertices;       // number of vertices
    int     num_st;             // number of texture coordinates
    int     num_tris;           // number of triangles
    int     num_glcmds;         // number of opengl commands
    int     num_frames;         // total number of frames

    int     offset_skins;       // offset to skin names (64 bytes each)
    int     offset_st;          // offset to s-t texture coordinates
    int     offset_tris;        // offset to triangles
    int     offset_frames;      // offset to frame data
    int     offset_glcmds;      // offset to opengl commands
    int     offset_end;         // offset to end of file

} md2_header_t;

/* vector */
typedef float vec3_t[3];

/* texture name */
typedef struct
{
  char name[64];   /* texture file name */
} md2_skin_t;

/* texture coords */
typedef struct
{
  short s;
  short t;

} md2_texCoord_t;

/* triangle data */
typedef struct
{
  unsigned short vertex[3];   /* vertex indices of the triangle */
  unsigned short st[3];       /* tex. coord. indices */

} md2_triangle_t;

/* vertex data */
typedef struct
{
  unsigned char v[3];         /* position */
  unsigned char normalIndex;  /* normal vector index */

} md2_vertex_t;

/* frame data */
typedef struct
{
  vec3_t          scale;      /* scale factor */
  vec3_t          translate;  /* translation vector */
  char            name[16];   /* frame name */
  md2_vertex_t    *verts;     /* list of frame's vertices */

} md2_frame_t;

/* gl command packet */
typedef struct
{
  float s;
  float t;
  int index;

} md2_glcmd_t;


/* md2 model structure */
typedef struct
{
  md2_header_t* header;
  md2_skin_t *skins;
  md2_texCoord_t *texcoords;
  md2_triangle_t *triangles;
  md2_frame_t *frames;
  int *glcmds;
  GLuint tex_id;
} md2_model_t;

typedef struct
{
  int xsize;
	int ysize;
	char* image;
} irgb_t;

//int ReadMD2Model(const char *filename, md2_model_t *mdl);
//void RenderFrameItpWithGLCmds (int n, float interp, md2_model_t *mdl);

#endif
