/* Gouraud Shade effect: http://insolitdust.sourceforge.net/code.html */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sys.h"

#define XOFFSET         (layer[0].width / 2)
#define YOFFSET         (layer[0].height / 2)
#define ZOFFSET         250
#define SCALING_FACTOR1 10
#define SCALING_FACTOR2 50
#define VERTICES        SCALING_FACTOR1 * SCALING_FACTOR2
#define PALSIZE         128
#define RADIUS1         25
#define RADIUS2         50
#define RADIUS3         25

typedef struct {
  int X, Y;
} Point;

typedef struct {
  int X, Y, Z;
} Point3D;

u16 palette[256];

void SetColour(unsigned char n, unsigned char r, unsigned char g, unsigned char b)
{
  palette[n] = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
}

void PutPixel(int x, int y, unsigned char col)
{
  ((u16*)layer[0].addr)[display->width * y + x] = palette[col];
}

// 3D object and rotation table
Point3D Shape[VERTICES], RotatedShape[VERTICES];

// Store the 2D object from 3D object
Point Shape2D[VERTICES], Face2D[3];

// List of face to draw on screens
int DrawOrder[VERTICES];
int MaxZ = RADIUS1 + RADIUS2 + RADIUS3, Vertices = 0;
int PolygonCoords[VERTICES][4], VertexCount, ZValue;
unsigned char Colours[3];

// Draw on screen a triangle in Gouraud Shade mode 
static void GouraudShade(Point Vertices[3], unsigned char Colours[3])
{
  unsigned char StartColour, EndColour;
  int MinY = 0, MaxY = 0, ColourIncSign, ColourIncCount, ColourDiff;
  register int Count, Y;
  int LineWidth, EndVertex1, EndVertex2, StartVertex1, StartVertex2;
  int XDiff1, XDiff2, YDiff1, YDiff2, X1, X2, ColourDiff1, ColourDiff2;
  int XCalc1 = 0, XCalc2 = 0, ColourCalc1 = 0, ColourCalc2 = 0;
  for(Count = 1; Count < 3; Count++)
  {
    if(Vertices[Count].Y < Vertices[MinY].Y) MinY = Count;
    else if(Vertices[Count].Y > Vertices[MaxY].Y) MaxY = Count;
  }
  StartVertex1 = StartVertex2 = MinY;
  EndVertex1 = MinY + 2;
  if(EndVertex1 >= 3) EndVertex1 -= 3;
  EndVertex2 = MinY + 1;
  if(EndVertex2 >= 3) EndVertex2 -= 3;
  XDiff1 = Vertices[EndVertex1].X - Vertices[StartVertex1].X;
  YDiff1 = Vertices[EndVertex1].Y - Vertices[StartVertex1].Y;
  XDiff2 = Vertices[EndVertex2].X - Vertices[StartVertex1].X;
  YDiff2 = Vertices[EndVertex2].Y - Vertices[StartVertex1].Y;
  ColourDiff1 = Colours[EndVertex1] - Colours[StartVertex1];
  ColourDiff2 = Colours[EndVertex2] - Colours[StartVertex2];
  if(YDiff1 == 0) YDiff1 = 1;
  if(YDiff2 == 0) YDiff2 = 1;
  for(Y = Vertices[MinY].Y; Y <= Vertices[MaxY].Y; Y++)
  {
    X2 = Vertices[StartVertex1].X + XCalc1 / YDiff1;
    XCalc1 += XDiff1;
    X1 = Vertices[StartVertex2].X + XCalc2 / YDiff2;
    XCalc2 += XDiff2;
    EndColour = Colours[StartVertex1] + ColourCalc1 / YDiff1;
    ColourCalc1 += ColourDiff1;
    StartColour = Colours[StartVertex2] + ColourCalc2 / YDiff2;
    ColourCalc2 += ColourDiff2;
    if(EndColour > StartColour) ColourIncSign = 1;
    else ColourIncSign = -1;
    ColourDiff = abs(StartColour - EndColour);
    LineWidth = X2 - X1;
    ColourIncCount = ColourDiff - (LineWidth >> 1);
    for(Count = X1; Count < X2; Count++) 
    {
      PutPixel(Count, Y, StartColour);
      while(ColourIncCount >= 0)
      {
        StartColour += ColourIncSign;
        ColourIncCount -= LineWidth;
      }
      ColourIncCount += ColourDiff;
    }
    if(Y == Vertices[EndVertex1].Y)
    {
      StartVertex1 = EndVertex1;
      EndVertex1 = EndVertex2;
      XDiff1 = Vertices[EndVertex1].X - Vertices[StartVertex1].X;
      YDiff1 = Vertices[EndVertex1].Y - Vertices[StartVertex1].Y;
      ColourDiff1 = Colours[EndVertex1] - Colours[StartVertex1];
      if(YDiff1 == 0) YDiff1 = 1;
      XCalc1 = XDiff1;
      ColourCalc1 = ColourDiff1;
    }
    if(Y == Vertices[EndVertex2].Y)
    {
      StartVertex2 = EndVertex2;
      EndVertex2 = EndVertex1;
      XDiff2 = Vertices[EndVertex2].X - Vertices[StartVertex2].X;
      YDiff2 = Vertices[EndVertex2].Y - Vertices[StartVertex2].Y;
      ColourDiff2 = Colours[EndVertex2] - Colours[StartVertex2];
      if(YDiff2 == 0) YDiff2 = 1;
      XCalc2 = XDiff2;
      ColourCalc2 = ColourDiff2;
    }
  }
}

// Rotate a point from center by SinAngle and CosAngle
static void RotatePoint(int X, int Y, int XCenter, int YCenter, float SinAngle,
                        float CosAngle, int *EndX, int *EndY)
{
  *EndX = (X - XCenter) * CosAngle - (Y - YCenter) * SinAngle + XCenter;
  *EndY = (Y - YCenter) * CosAngle + (X - XCenter) * SinAngle + YCenter;
}

//Precalculate the object on screen
void Build_Shape(void)
{
  double dx, dy, dz, Alpha, Beta, Modulus, Value, X, Y, Z;
  double Distance, MinDistance;
  int Count1, Count2, Index1, Index2, Rotation;
  for(Alpha = 0, Count2 = 0; Count2 < SCALING_FACTOR2; Count2++, Alpha += 2 * M_PI / SCALING_FACTOR2)
  {
    X = RADIUS2 * cos(2 * Alpha) + RADIUS1 * sin(Alpha);
    Y = RADIUS2 * sin(2 * Alpha) + RADIUS1 * cos(Alpha);
    Z = RADIUS2 * cos(3 * Alpha);
    dx = -2 * RADIUS2 * sin(2 * Alpha) + RADIUS1 * cos(Alpha);
    dy = 2 * RADIUS2 * cos(2 * Alpha) - RADIUS1 * sin(Alpha);
    dz = -3 * RADIUS2 * sin(3 * Alpha);
    Value = sqrt(dx * dx + dz * dz);
    Modulus = sqrt(dx * dx + dy * dy + dz * dz);
    for(Beta = 0, Count1 = 0; Count1 < SCALING_FACTOR1; Count1++, Beta += 2 * M_PI / SCALING_FACTOR1)
    {
      Shape[Vertices].X =	X - RADIUS3 * (cos(Beta) * dz - sin(Beta) *	dx * dy / Modulus) / Value;
      Shape[Vertices].Y =	Y - RADIUS3 * sin(Beta) * Value / Modulus;
      Shape[Vertices].Z =	Z + RADIUS3 * (cos(Beta) * dx + sin(Beta) * dy * dz / Modulus) / Value;
      Vertices++;
    }
  }
  for(Count1 = 0; Count1 < SCALING_FACTOR2; Count1++)
  {
    Index1 = Count1 * SCALING_FACTOR1;
    Index2 = Index1 + SCALING_FACTOR1;
    Index2 %= Vertices;
    Rotation = 0;
    MinDistance = (Shape[Index1].X - Shape[Index2].X) * (Shape[Index1].X - Shape[Index2].X) +
                  (Shape[Index1].Y - Shape[Index2].Y) * (Shape[Index1].Y - Shape[Index2].Y) +
                  (Shape[Index1].Z - Shape[Index2].Z) * (Shape[Index1].Z - Shape[Index2].Z);
    for(Count2 = 1; Count2 < SCALING_FACTOR1; Count2++)
    {
      Index2 = Count2 + Index1 + SCALING_FACTOR1;
      if(Count1 == SCALING_FACTOR2 - 1) Index2 = Count2;
      Distance = (Shape[Index1].X - Shape[Index2].X) * (Shape[Index1].X - Shape[Index2].X) +
                 (Shape[Index1].Y - Shape[Index2].Y) * (Shape[Index1].Y - Shape[Index2].Y) +
                 (Shape[Index1].Z - Shape[Index2].Z) * (Shape[Index1].Z - Shape[Index2].Z);
      if(Distance < MinDistance)
      {
        MinDistance = Distance;
        Rotation = Count2;
      }
    }
    for(Count2 = 0; Count2 < SCALING_FACTOR1; Count2++)
    {
      Index2 = (SCALING_FACTOR1 + Count2 + Rotation) % SCALING_FACTOR1;
      PolygonCoords[Index1 + Count2][0] = Index1 + Count2;
      Index2 = Count2 + 1;
      Index2 %= SCALING_FACTOR1;
      PolygonCoords[Index1 + Count2][1] = Index1 + Index2;
      Index2 = Count2 + Rotation + 1;
      Index2 %= SCALING_FACTOR1;
      PolygonCoords[Index1 + Count2][2] = (Index1 + Index2 + SCALING_FACTOR1) % Vertices;
      Index2 = Count2 + Rotation;
      Index2 %= SCALING_FACTOR1;
      PolygonCoords[Index1 + Count2][3] = (Index1 + Index2 + SCALING_FACTOR1) % Vertices;
    }
  }
  for(Count1 = 0; Count1 < Vertices; Count1++) DrawOrder[Count1] = Count1;
}

// Compare the Z distance of two shape 
int Compare(const void *Number1, const void *Number2)
{
  register int N1 = *((int *)Number1);
  register int N2 = *((int *)Number2);
  return RotatedShape[N2].Z - RotatedShape[N1].Z;
}

// Rotare the 3D object
void RotateShape(void)
{
  static float Phi = 0, Theta = 0;
  float SinePhi = sin(Phi), CosinePhi = cos(Phi), SineTheta = sin(Theta), CosineTheta = cos(Theta);
  for(VertexCount = 0; VertexCount < Vertices; VertexCount++)
  {
    RotatePoint(Shape[VertexCount].Y, Shape[VertexCount].Z, 0, 0, SinePhi,
                CosinePhi, &RotatedShape[VertexCount].Y, &RotatedShape[VertexCount].Z);
    RotatePoint(Shape[VertexCount].X, RotatedShape[VertexCount].Z, 0, 0,
                SineTheta, CosineTheta, &RotatedShape[VertexCount].X, &RotatedShape[VertexCount].Z);
  }
  Phi += 0.12;
  Theta += 0.08;
}

// From 3D coord, draw on screen one triangle
static void DrawFace(int Index1, int Index2, int Index3)
{
  if((RotatedShape[Index2].X - RotatedShape[Index1].X) * (RotatedShape[Index1].Y - RotatedShape[Index3].Y) <
    (RotatedShape[Index2].Y - RotatedShape[Index1].Y) * (RotatedShape[Index1].X - RotatedShape[Index3].X))
  return;
  Face2D[0].X = Shape2D[Index1].X;
  Face2D[0].Y = Shape2D[Index1].Y;
  Face2D[1].X = Shape2D[Index2].X;
  Face2D[1].Y = Shape2D[Index2].Y;
  Face2D[2].X = Shape2D[Index3].X;
  Face2D[2].Y = Shape2D[Index3].Y;
  Colours[0] = 1 + (PALSIZE - 1) * (RotatedShape[Index1].Z + MaxZ) / (MaxZ << 1);
  Colours[1] = 1 + (PALSIZE - 1) * (RotatedShape[Index2].Z + MaxZ) / (MaxZ << 1);
  Colours[2] = 1 + (PALSIZE - 1) * (RotatedShape[Index3].Z + MaxZ) / (MaxZ << 1);
  if(PolygonCoords[DrawOrder[VertexCount]][0] & 2)
  {
    Colours[0] += PALSIZE;
    Colours[1] += PALSIZE;
    Colours[2] += PALSIZE;
  }
  GouraudShade(Face2D, Colours);
}

void Draw2DFaces(void)
{
  for(VertexCount = 0; VertexCount < Vertices; VertexCount++)
  {
    ZValue = (ZOFFSET + RotatedShape[VertexCount].Z) >> 2;
    Shape2D[VertexCount].X = XOFFSET + ((RotatedShape[VertexCount].X) << 7) / ZValue;
    Shape2D[VertexCount].Y = YOFFSET + ((RotatedShape[VertexCount].Y) << 7) / ZValue;
  }
  for(VertexCount = 0; VertexCount < Vertices; VertexCount++)
  {
    DrawFace(PolygonCoords[DrawOrder[VertexCount]][0],
             PolygonCoords[DrawOrder[VertexCount]][1],
             PolygonCoords[DrawOrder[VertexCount]][2]);
    DrawFace(PolygonCoords[DrawOrder[VertexCount]][0],
             PolygonCoords[DrawOrder[VertexCount]][2],
             PolygonCoords[DrawOrder[VertexCount]][3]);
  }
}

int main (void)
{
  u16 *fb[2];
  puts("\033[36mF1C100S - Gouraud Shade Effect ("__DATE__" "__TIME__")\033[0m");

  //disp_init(&TV_NTSC, 0);
  disp_init(&TFT_800x480, 0);
  fb[0] = fb_alloc(display->width, display->height, 16);
  fb[1] = fb_alloc(display->width, display->height, 16);
  lay_config(0, display->width, display->height, 0, 0, 16, fb[0], 0, 5 << 8);
  delay(100);
  disp_backlight(100);

  for(int i = 1; i <= PALSIZE; i++)
  {
    unsigned char r, g, b;
    r = 63 - 63 * i / PALSIZE;
    g = 10;
    b = 0;
    SetColour(i, r * 4, g * 4, b * 4);
    r = 0;
    g = 10;
    b = 63 - 63 * i / PALSIZE;
    SetColour(PALSIZE + i, r * 4, g * 4, b * 4);
  }
  Build_Shape();
  while(1)
  {
    ctr_ms = 0;
    RotateShape();
    qsort((void *)DrawOrder, VERTICES, sizeof(DrawOrder[0]), Compare);
    Draw2DFaces();
    disp_sync();
    lay_update(0);
    layer[0].addr = layer[0].addr == fb[0] ? fb[1] : fb[0];
    memset(layer[0].addr, 0, layer[0].height * layer[0].stride / 8);
    printf("%d fps\r", 1000 / ctr_ms);
    dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
  }
}
