/* Moving dot torus effect: http://insolitdust.sourceforge.net/code.html */

#include <stdio.h>
#include <math.h>
#include "sys.h"

#define K       15            // Intensity of single big torus block
#define COLORS  163           // Number of color used

u16 *fb;

unsigned char Form1[5][5] = { // Big 5x5 torus block
  { K*0, K*3, K*3, K*3, K*0 },
  { K*2, K*4, K*4, K*4, K*2},
  { K*3, K*4, K*5, K*4, K*3},
  { K*2, K*4, K*4, K*4, K*2},
  { K*0, K*3, K*3, K*3, K*0}
};

static int Point[100][3] = {  // Points of torus
  {   19,    0,  -13},
  {    8,    0,  -21},
  {   -7,    0,  -22},
  {  -18,    0,  -13},
  {  -22,    0,    1},
  {  -18,    0,   14},
  {   -7,    0,   22},
  {    7,    0,   22},
  {   19,    0,   14},
  {   23,    0,    1},
  {    4,   47,  -13},
  {   -6,   41,  -21},
  {  -17,   32,  -22},
  {  -26,   26,  -13},
  {  -30,   23,    0},
  {  -26,   26,   14},
  {  -17,   32,   22},
  {   -6,   41,   22},
  {    3,   47,   14},
  {    7,   50,    0},
  {  -37,   77,  -13},
  {  -40,   66,  -21},
  {  -45,   53,  -22},
  {  -48,   42,  -13},
  {  -50,   37,    0},
  {  -48,   42,   14},
  {  -45,   52,   22},
  {  -40,   66,   22},
  {  -37,   77,   14},
  {  -35,   81,    0},
  {  -87,   77,  -13},
  {  -83,   66,  -21},
  {  -79,   53,  -22},
  {  -75,   42,  -13},
  {  -74,   37,    0},
  {  -75,   42,   14},
  {  -79,   52,   22},
  {  -83,   66,   22},
  {  -87,   77,   14},
  {  -88,   81,    0},
  { -127,   47,  -13},
  { -118,   41,  -21},
  { -106,   32,  -22},
  {  -97,   26,  -13},
  {  -94,   23,    0},
  {  -97,   26,   14},
  { -106,   32,   22},
  { -118,   41,   22},
  { -127,   47,   14},
  { -130,   50,    0},
  { -142,    0,  -13},
  { -131,    0,  -21},
  { -117,    0,  -22},
  { -105,    0,  -13},
  { -101,    0,    0},
  { -105,    0,   14},
  { -117,    0,   22},
  { -131,    0,   22},
  { -142,    0,   14},
  { -147,    0,    0},
  { -127,  -47,  -13},
  { -118,  -41,  -21},
  { -106,  -32,  -22},
  {  -97,  -26,  -13},
  {  -94,  -23,    0},
  {  -97,  -26,   14},
  { -106,  -32,   22},
  { -118,  -41,   22},
  { -127,  -47,   14},
  { -130,  -50,    0},
  {  -87,  -77,  -13},
  {  -83,  -66,  -21},
  {  -79,  -53,  -22},
  {  -75,  -42,  -13},
  {  -74,  -37,    0},
  {  -75,  -42,   14},
  {  -79,  -52,   22},
  {  -83,  -66,   22},
  {  -87,  -77,   14},
  {  -88,  -81,    0},
  {  -37,  -77,  -13},
  {  -40,  -66,  -21},
  {  -45,  -53,  -22},
  {  -48,  -42,  -13},
  {  -50,  -37,    0},
  {  -48,  -42,   14},
  {  -45,  -52,   22},
  {  -40,  -66,   22},
  {  -37,  -77,   14},
  {  -35,  -81,    0},
  {    4,  -47,  -13},
  {   -6,  -41,  -21},
  {  -17,  -32,  -22},
  {  -26,  -26,  -13},
  {  -30,  -23,    0},
  {  -26,  -26,   14},
  {  -17,  -32,   22},
  {   -6,  -41,   22},
  {    3,  -47,   14},
  {    7,  -50,    0}};

//static int Planes[100][4]={		//Faces of torus
int Planes[100][4]={		//Faces of torus
{0,1,11,10},
{1,2,12,11},
{2,3,13,12},
{3,4,14,13},
{4,5,15,14},
{5,6,16,15},
{6,7,17,16},
{7,8,18,17},
{8,9,19,18},
{9,0,10,19},
{10,11,21,20},
{11,12,22,21},
{12,13,23,22},
{13,14,24,23},
{14,15,25,24},
{15,16,26,25},
{16,17,27,26},
{17,18,28,27},
{18,19,29,28},
{19,10,20,29},
{20,21,31,30},
{21,22,32,31},
{22,23,33,32},
{23,24,34,33},
{24,25,35,34},
{25,26,36,35},
{26,27,37,36},
{27,28,38,37},
{28,29,39,38},
{29,20,30,39},
{30,31,41,40},
{31,32,42,41},
{32,33,43,42},
{33,34,44,43},
{34,35,45,44},
{35,36,46,45},
{36,37,47,46},
{37,38,48,47},
{38,39,49,48},
{39,30,40,49},
{40,41,51,50},
{41,42,52,51},
{42,43,53,52},
{43,44,54,53},
{44,45,55,54},
{45,46,56,55},
{46,47,57,56},
{47,48,58,57},
{48,49,59,58},
{49,40,50,59},
{50,51,61,60},
{51,52,62,61},
{52,53,63,62},
{53,54,64,63},
{54,55,65,64},
{55,56,66,65},
{56,57,67,66},
{57,58,68,67},
{58,59,69,68},
{59,50,60,69},
{60,61,71,70},
{61,62,72,71},
{62,63,73,72},
{63,64,74,73},
{64,65,75,74},
{65,66,76,75},
{66,67,77,76},
{67,68,78,77},
{68,69,79,78},
{69,60,70,79},
{70,71,81,80},
{71,72,82,81},
{72,73,83,82},
{73,74,84,83},
{74,75,85,84},
{75,76,86,85},
{76,77,87,86},
{77,78,88,87},
{78,79,89,88},
{79,70,80,89},
{80,81,91,90},
{81,82,92,91},
{82,83,93,92},
{83,84,94,93},
{84,85,95,94},
{85,86,96,95},
{86,87,97,96},
{87,88,98,97},
{88,89,99,98},
{89,80,90,99},
{90,91,1,0},
{91,92,2,1},
{92,93,3,2},
{93,94,4,3},
{94,95,5,4},
{95,96,6,5},
{96,97,7,6},
{97,98,8,7},
{98,99,9,8},
{99,90,0,9}};

float SinTab[721], CosTab[721]; // Precalculated sin, cos table

unsigned char D_Buf[320 * 240]; // Pointer to video memory
unsigned int YTable[240];       // Pointer to yoffset table

// Blur the whole screen
static void Blur_Screen(void)
{
  unsigned char *Row1, *Row2, *Row3;
  int X, Y, Color;
  for (Y = 1; Y < 240 - 2; Y++)
  {
    Row1 = D_Buf + YTable[Y - 1];
    Row2 = D_Buf + YTable[Y];
    Row3 = D_Buf + YTable[Y + 1];
    *Row1++ = 0;
    *Row2++ = 0;
    *Row3++ = 0;
    for (X = 1; X < 319; X++)
    {
      Color = (*Row1 + *Row3 + *(Row2 - 1) + *(Row2 + 1)) >> 2;
      Color -= 2;
      if(Color < 0) Color = 0;
      *Row2 = Color;
      Row1++;
      Row2++;
      Row3++;
    }
    *Row1++ = 0;
    *Row2++ = 0;
    *Row3++ = 0;
  }
}

// Rotate on X
void RotX(register float *Y,register float *Z,unsigned int Phi)
{
  float Tmp1,Tmp2;
  Tmp1 = (*Y * CosTab[Phi] - *Z * SinTab[Phi]);
  Tmp2 = (*Y * SinTab[Phi] + *Z * CosTab[Phi]);
  *Y = Tmp1;
  *Z = Tmp2;
}

// Rotate on Y
void RotY(register float *X,register float *Z,unsigned int Phi)
{
  float Tmp1,Tmp2;
  Tmp1 = (*X * CosTab[Phi] + *Z * SinTab[Phi]);
  Tmp2 = (- *X * SinTab[Phi] + *Z * CosTab[Phi]);
  *X = Tmp1;
  *Z = Tmp2;
}

// Rotate on Z
void RotZ(register float *X,register float *Y,unsigned int Phi)
{
  float Tmp1,Tmp2;
  Tmp1 = (*X * CosTab[Phi] - *Y * SinTab[Phi]);
  Tmp2 = (*X * SinTab[Phi] + *Y * CosTab[Phi]);
  *X = Tmp1;
  *Y = Tmp2;
}

// Draw a single big torus block
static void Draw_Blob(register int X,register int Y)
{
  int I,J;
  unsigned char Pixel;
  unsigned int App;
  for (I = 0; I < 5; I++) for (J = 0; J < 5; J++)
  {
    App = YTable[Y] + (X + J);
    Pixel = D_Buf[App] + Form1[I][J];
    if (Pixel >= COLORS - 1) Pixel = COLORS - 1;
    D_Buf[App] = Pixel;
  }
}

// Draw torus on screen, update rotation and blur screen
void Show_Torus(void)
{
  static int A = 318, XOff = 160, YOff = 120;
  static long Phi = 0;
  int I, Px, Py;
  float X ,Y, Z;
  long Phi2, Pa;
  Phi2 = Phi;
  for(Phi2 = Phi; Phi2 <= (Phi + 16); Phi2++) for (I = 0; I < 100; I++)
  {
    X = (float)(Point[I][0] + 77);
    Y = (float)(Point[I][1]);
    Z = (float)(Point[I][2] + 50);
    Pa = Phi2 % 720;
    RotZ(&X, &Y, Pa);
    RotX(&Y, &Z, Pa);
    RotY(&X, &Z ,Pa);
    Px = ((int)(X * A / (Z - A))) + XOff;
    Py = ((int)(Y * A / (Z - A))) + YOff;
    Draw_Blob(Px, Py);
  }
  Phi += 6;
  Blur_Screen();
}

u16 palette[256];

void SetColour(unsigned char n, unsigned char r, unsigned char g, unsigned char b)
{
  palette[n] = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
}

void fb_update (void)
{
  for(int y = 0; y < 240; y++)
  {
    for(int x= 0; x < 320; x++)
    {
      u16 col = palette[D_Buf[y * 320 + x]];
      int offset = (display->height - 480) / 2 * display->width +
        (display->width - 640) / 2;
      offset += (y * display->width + x) * 2;
      fb[offset] = col;
      fb[offset + 1] = col;
      fb[offset + display->width] = col;
      fb[offset + display->width + 1] = col;
    }
  }
}

int main (void)
{
  puts("\033[36mF1C100S - Torus Demo Effect\033[0m");

  //disp_init(&TV_PAL, 0);
  //disp_init(&TV_NTSC, 0);
  disp_init(&TFT_800x480, 0);
  fb = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb, 0, 5 << 8);
  lay_update(0);
  delay(100);
  disp_backlight(75);

  for(int i = 0; i < 721; i++)
  {
    SinTab[i] = sin(i * 2 * M_PI / 720) * 0.7;
    CosTab[i] = cos(i * 2 * M_PI / 720);
  }
  for(int i = 0; i < COLORS; i++)
  {
    unsigned char r, g, b;
    if(i == 0) r = g = b = 0;
    else
    {
      r = (unsigned char)(163 * exp(7 * log((double)i/(double)(COLORS - 10))));
      g = (unsigned char)(163 * exp(2 * log((double)i/(double)(COLORS - 10))));
      b = (unsigned char)(163 * exp(2 * log((double)i/(double)(COLORS - 10))));
    }
    if(r > 163) r = 163;
    if(g > 163) g = 163;
    if(b > 163) b = 163;
    SetColour(i, r, g, b);
  }
  for(int i = 0; i < 240; i++) YTable[i] = i * 320;
  while(1)
  {
    Show_Torus();
    fb_update();
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
    delay(20);
  }
}
