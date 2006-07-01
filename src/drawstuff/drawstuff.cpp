/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001-2003 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

/*

simple graphics.

the following command line flags can be used (typically under unix)
	-notex		Do not use any textures
	-noshadow[s]	Do not draw any shadows
	-pause		Start the simulation paused

TODO
----

manage openGL state changes better

*/

#ifdef WIN32
#include <windows.h>
#endif

#include "drawstuff/drawstuff.h"
#include "drawstuff/internal.h"

/// //////////////////////////////////////////////////////////////////////// ///

// constants to convert degrees to radians and the reverse
#define RAD_TO_DEG (180.0/M_PI)
#define DEG_TO_RAD (M_PI/180.0)

// light vector. (light position)
// LIGHTZ is implicitly 1
#define LIGHTX (1.0f)
#define LIGHTY (0.4f)

bool use_textures = true;
GLfloat transform_matrix[16];

GLuint Texture_Ground;
GLuint Texture_Sky;

#define DAY 1
#define NIGHT 2

int DayOrNight = DAY;

//***************************************************************************
// misc mathematics stuff

#define dCROSS(a,op,b,c) \
  (a)[0] op ((b)[1]*(c)[2] - (b)[2]*(c)[1]); \
  (a)[1] op ((b)[2]*(c)[0] - (b)[0]*(c)[2]); \
  (a)[2] op ((b)[0]*(c)[1] - (b)[1]*(c)[0]);

inline float dDOT (const float *a, const float *b)
  { return ((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2]); }


static void normalizeVector3 (float v[3])
{
  float len = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
  if (len <= 0.0f) {
    v[0] = 1;
    v[1] = 0;
    v[2] = 0;
  }
  else {
    len = 1.0f / (float)sqrt(len);
    v[0] *= len;
    v[1] *= len;
    v[2] *= len;
  }
}


//***************************************************************************
// the current drawing state (for when the user's step function is drawing)

static float color[4] = {0,0,0,0};	// current r,g,b,alpha color

///***************************************************************************
/// OpenGL utility stuff

static void setCamera (float x, float y, float z, float h, float p, float r)
{
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  glRotatef (90, 0,0,1);
  glRotatef (90, 0,1,0);
  glRotatef (r, 1,0,0);
  glRotatef (p, 0,1,0);
  glRotatef (-h, 0,0,1);
  glTranslatef (-x,-y,-z);
}

// sets the material color, not the light color

static void setMaterialColor (float r, float g, float b, float alpha)
{
  GLfloat light_ambient[4],light_diffuse[4],light_specular[4];

  light_ambient[0] = r*0.3f;
  light_ambient[1] = g*0.3f;
  light_ambient[2] = b*0.3f;
  light_ambient[3] = alpha;

  light_diffuse[0] = r*0.7f;
  light_diffuse[1] = g*0.7f;
  light_diffuse[2] = b*0.7f;
  light_diffuse[3] = alpha;

  light_specular[0] = r*0.2f;
  light_specular[1] = g*0.2f;
  light_specular[2] = b*0.2f;
  light_specular[3] = alpha;

  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, light_ambient);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, light_diffuse);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, light_specular);
  glMaterialf  (GL_FRONT_AND_BACK, GL_SHININESS, 5.0f);
}


static void setTransform (const float pos[3], const float R[12])
{
  transform_matrix[0]=R[0];
  transform_matrix[1]=R[4];
  transform_matrix[2]=R[8];
  transform_matrix[3]=0;
  transform_matrix[4]=R[1];
  transform_matrix[5]=R[5];
  transform_matrix[6]=R[9];
  transform_matrix[7]=0;
  transform_matrix[8]=R[2];
  transform_matrix[9]=R[6];
  transform_matrix[10]=R[10];
  transform_matrix[11]=0;
  transform_matrix[12]=pos[0];
  transform_matrix[13]=pos[1];
  transform_matrix[14]=pos[2];
  transform_matrix[15]=1;
}


static void drawBox (const float sides[3])
{
  float lx = sides[0]*0.5f;
  float ly = sides[1]*0.5f;
  float lz = sides[2]*0.5f;

  // sides
  glBegin (GL_TRIANGLE_STRIP);
  glNormal3f (-1,0,0);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,-ly,lz);
  glVertex3f (-lx,ly,-lz);
  glVertex3f (-lx,ly,lz);
  glNormal3f (0,1,0);
  glVertex3f (lx,ly,-lz);
  glVertex3f (lx,ly,lz);
  glNormal3f (1,0,0);
  glVertex3f (lx,-ly,-lz);
  glVertex3f (lx,-ly,lz);
  glNormal3f (0,-1,0);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,-ly,lz);
  glEnd();

  // top face
  glBegin (GL_TRIANGLE_FAN);
  glNormal3f (0,0,1);
  glVertex3f (-lx,-ly,lz);
  glVertex3f (lx,-ly,lz);
  glVertex3f (lx,ly,lz);
  glVertex3f (-lx,ly,lz);
  glEnd();

  // bottom face
  glBegin (GL_TRIANGLE_FAN);
  glNormal3f (0,0,-1);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,ly,-lz);
  glVertex3f (lx,ly,-lz);
  glVertex3f (lx,-ly,-lz);
  glEnd();
}


// This is recursively subdivides a triangular area (vertices p1,p2,p3) into
// smaller triangles, and then draws the triangles. All triangle vertices are
// normalized to a distance of 1.0 from the origin (p1,p2,p3 are assumed
// to be already normalized). Note this is not super-fast because it draws
// triangles rather than triangle strips.

static void drawPatch (float p1[3], float p2[3], float p3[3], int level)
{
  int i;
  if (level > 0) {
    float q1[3],q2[3],q3[3];		 // sub-vertices
    for (i=0; i<3; i++) {
      q1[i] = 0.5f*(p1[i]+p2[i]);
      q2[i] = 0.5f*(p2[i]+p3[i]);
      q3[i] = 0.5f*(p3[i]+p1[i]);
    }
    float length1 = (float)(1.0/sqrt(q1[0]*q1[0]+q1[1]*q1[1]+q1[2]*q1[2]));
    float length2 = (float)(1.0/sqrt(q2[0]*q2[0]+q2[1]*q2[1]+q2[2]*q2[2]));
    float length3 = (float)(1.0/sqrt(q3[0]*q3[0]+q3[1]*q3[1]+q3[2]*q3[2]));
    for (i=0; i<3; i++) {
      q1[i] *= length1;
      q2[i] *= length2;
      q3[i] *= length3;
    }
    drawPatch (p1,q1,q3,level-1);
    drawPatch (q1,p2,q2,level-1);
    drawPatch (q1,q2,q3,level-1);
    drawPatch (q3,q2,p3,level-1);
  }
  else {
    glNormal3f (p1[0],p1[1],p1[2]);
    glVertex3f (p1[0],p1[1],p1[2]);
    glNormal3f (p2[0],p2[1],p2[2]);
    glVertex3f (p2[0],p2[1],p2[2]);
    glNormal3f (p3[0],p3[1],p3[2]);
    glVertex3f (p3[0],p3[1],p3[2]);
  }
}

// draw a sphere of radius 1
static int sphere_quality = 1;
static void drawSphere()
{
  // icosahedron data for an icosahedron of radius 1.0
# define ICX 0.525731112119133606f
# define ICZ 0.850650808352039932f
  static GLfloat idata[12][3] = {
    {-ICX, 0, ICZ},
    {ICX, 0, ICZ},
    {-ICX, 0, -ICZ},
    {ICX, 0, -ICZ},
    {0, ICZ, ICX},
    {0, ICZ, -ICX},
    {0, -ICZ, ICX},
    {0, -ICZ, -ICX},
    {ICZ, ICX, 0},
    {-ICZ, ICX, 0},
    {ICZ, -ICX, 0},
    {-ICZ, -ICX, 0}
  };

  static int index[20][3] = {
    {0, 4, 1},	  {0, 9, 4},
    {9, 5, 4},	  {4, 5, 8},
    {4, 8, 1},	  {8, 10, 1},
    {8, 3, 10},   {5, 3, 8},
    {5, 2, 3},	  {2, 7, 3},
    {7, 10, 3},   {7, 6, 10},
    {7, 11, 6},   {11, 0, 6},
    {0, 1, 6},	  {6, 1, 10},
    {9, 0, 11},   {9, 11, 2},
    {9, 2, 5},	  {7, 2, 11},
  };

  static GLuint listnum = 0;
  if (listnum==0) {
    listnum = glGenLists (1);
    glNewList (listnum,GL_COMPILE);
    glBegin (GL_TRIANGLES);
    for (int i=0; i<20; i++) {
      drawPatch (&idata[index[i][2]][0], &idata[index[i][1]][0], &idata[index[i][0]][0], sphere_quality);
    }
    glEnd();
    glEndList();
  }
  glCallList (listnum);
}

static void drawTriangle (const float *v0, const float *v1, const float *v2, int solid)
{
  float u[3],v[3],normal[3];
  u[0] = v1[0] - v0[0];
  u[1] = v1[1] - v0[1];
  u[2] = v1[2] - v0[2];
  v[0] = v2[0] - v0[0];
  v[1] = v2[1] - v0[1];
  v[2] = v2[2] - v0[2];
  dCROSS (normal,=,u,v);
  normalizeVector3 (normal);

  glBegin(solid ? GL_TRIANGLES : GL_LINE_STRIP);
  glNormal3fv (normal);
  glVertex3fv (v0);
  glVertex3fv (v1);
  glVertex3fv (v2);
  glEnd();
}

// draw a capped cylinder of length l and radius r, aligned along the x axis
static int capped_cylinder_quality = 3;

static void drawCappedCylinder (float l, float r)
{
  int i,j;
  float tmp,nx,ny,nz,start_nx,start_ny,a,ca,sa;
  // number of sides to the cylinder (divisible by 4):
  const int n = capped_cylinder_quality*4;

  l *= 0.5;
  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,l);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,-l);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw first cylinder cap
  start_nx = 0;
  start_ny = 1;
  for (j=0; j<(n/4); j++) {
    // get start_n2 = rotated start_n
    float start_nx2 =  ca*start_nx + sa*start_ny;
    float start_ny2 = -sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    nx = start_nx; ny = start_ny; nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    glBegin (GL_TRIANGLE_STRIP);
    for (i=0; i<=n; i++) {
      glNormal3d (ny2,nz2,nx2);
      glVertex3d (ny2*r,nz2*r,l+nx2*r);
      glNormal3d (ny,nz,nx);
      glVertex3d (ny*r,nz*r,l+nx*r);
      // rotate n,n2
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
      tmp = ca*ny2- sa*nz2;
      nz2 = sa*ny2 + ca*nz2;
      ny2 = tmp;
    }
    glEnd();
    start_nx = start_nx2;
    start_ny = start_ny2;
  }

  // draw second cylinder cap
  start_nx = 0;
  start_ny = 1;
  for (j=0; j<(n/4); j++) {
    // get start_n2 = rotated start_n
    float start_nx2 = ca*start_nx - sa*start_ny;
    float start_ny2 = sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    nx = start_nx; ny = start_ny; nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    glBegin (GL_TRIANGLE_STRIP);
    for (i=0; i<=n; i++) {
      glNormal3d (ny,nz,nx);
      glVertex3d (ny*r,nz*r,-l+nx*r);
      glNormal3d (ny2,nz2,nx2);
      glVertex3d (ny2*r,nz2*r,-l+nx2*r);
      // rotate n,n2
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
      tmp = ca*ny2- sa*nz2;
      nz2 = sa*ny2 + ca*nz2;
      ny2 = tmp;
    }
    glEnd();
    start_nx = start_nx2;
    start_ny = start_ny2;
  }
}

// draw a cylinder of length l and radius r, aligned along the z axis
static void drawCylinder2 (float l, float radius1, float radius2,float zoffset)
{
  int i;
  float tmp,ny,nz,a,ca,sa;
  const int n = 24;	// number of sides to the cylinder (divisible by 4)

  l *= 0.5;
  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*radius1,nz*radius1,l+zoffset);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*radius2,nz*radius2,-l+zoffset);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw top cap
  glShadeModel (GL_FLAT);
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,1);
  glVertex3d (0,0,l+zoffset);
  for (i=0; i<=n; i++) {
    if (i==1 || i==n/2+1)
      setMaterialColor (color[0]*0.75f,color[1]*0.75f,color[2]*0.75f,color[3]);
    glNormal3d (0,0,1);
    glVertex3d (ny*radius1,nz*radius1,l+zoffset);
    if (i==1 || i==n/2+1)
      setMaterialColor (color[0],color[1],color[2],color[3]);

    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw bottom cap
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,-1);
  glVertex3d (0,0,-l+zoffset);
  for (i=0; i<=n; i++) {
    if (i==1 || i==n/2+1)
      setMaterialColor (color[0]*0.75f,color[1]*0.75f,color[2]*0.75f,color[3]);
    glNormal3d (0,0,-1);
    glVertex3d (ny*radius2,nz*radius2,-l+zoffset);
    if (i==1 || i==n/2+1)
      setMaterialColor (color[0],color[1],color[2],color[3]);

    // rotate ny,nz
    tmp = ca*ny + sa*nz;
    nz = -sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();
}

static void drawCone(float l, float r, float zoffset)
{
	drawCylinder2(l,0,r,zoffset);
}

// draw a cylinder of length l and radius r, aligned along the z axis
static void drawCylinder (float l, float r, float zoffset)
{
	drawCylinder2(l,r,r,zoffset);
}

///***************************************************************************
/// motion model

// current camera position and orientation
static float view_xyz[3];	// position x,y,z
static float view_hpr[3];	// position x,y,z

static void wrapCameraAngles()
{
  for (int i=0; i<3; i++) {
    while (view_hpr[i] > 180) view_hpr[i] -= 360;
    while (view_hpr[i] < -180) view_hpr[i] += 360;
  }
}


// call this to update the current camera position. the bits in `mode' say
// if the left (1), middle (2) or right (4) mouse button is pressed, and
// (deltax,deltay) is the amount by which the mouse pointer has moved.

void dsMotion (int mode, int deltax, int deltay)
{
  float side = 0.01f * float(deltax);
  float fwd = (mode==4) ? (0.01f * float(deltay)) : 0.0f;
  float s = (float) sin (view_hpr[0]*DEG_TO_RAD);
  float c = (float) cos (view_hpr[0]*DEG_TO_RAD);

  if (mode==1) {
    view_hpr[0] += float (deltax) * 0.5f;
    view_hpr[1] += float (deltay) * 0.5f;
  }
  else {
    view_xyz[0] += -s*side + c*fwd;
    view_xyz[1] += c*side + s*fwd;
    if (mode==2 || mode==5) view_xyz[2] += 0.01f * float(deltay);
  }
  wrapCameraAngles();
}

/// //////////////////////////////////////////////////////////////////////// ///
/// drawing loop stuff

void dsStartGraphics (int width, int height, dsFunctions *fn)
{
    CFG_File config;

    int result = CFG_OpenFile(GAME_CONFIG_NAME, &config );

    if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
    {
        dsPrint("Unable to load file: %s\n", SDL_GetError());
        exit(1);
    }
    DayOrNight = CFG_ReadInt("hour_to_game",HOUR_RANDOM);

    if ( DayOrNight == HOUR_RANDOM )
    {
        // random day or nitgh
        srand(SDL_GetTicks()/100);		/*initialize random number generator*/
        double random = rand()%10;
        DayOrNight = random>4 ? DAY : NIGHT ;
    }

    // load textures
    if (DayOrNight == DAY)
    {
       Texture_Sky = dsLoadGLTexture( "./textures/skyDay.bmp" );
       Texture_Ground = dsLoadGLMipMap( "./textures/groundDay.bmp" );
    }
    else
    {
       Texture_Sky = dsLoadGLTexture( "./textures/skyNight.bmp" );
       Texture_Ground = dsLoadGLMipMap( "./textures/groundNight.bmp" );
    }

  // setup viewport
  glViewport (0,0,width,height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  const float vnear = 0.1f;
  const float vfar = 10000.0f;
  const float k = 0.8f;     // view scale, 1 = +/- 45 degrees
  if (width >= height) {
    float k2 = float(height)/float(width);
    glFrustum (-vnear*k,vnear*k,-vnear*k*k2,vnear*k*k2,vnear,vfar);
  }
  else {
    float k2 = float(width)/float(height);
    glFrustum (-vnear*k*k2,vnear*k*k2,-vnear*k,vnear*k,vnear,vfar);
  }

  // setup lights. it makes a difference whether this is done in the
  // GL_PROJECTION matrix mode (lights are scene relative) or the
  // GL_MODELVIEW matrix mode (lights are camera relative, bad!).
  /// DAY
  static GLfloat light_ambient_day[] = { 0.5, 0.5, 0.5, 1.0 };
  static GLfloat light_diffuse_day[] = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat light_specular_day[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient_day);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse_day);
  glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular_day);
  /// NIGHT
  static GLfloat light_ambient_night[] = { 0.25, 0.25, 0.25, 1.0 };
  static GLfloat light_diffuse_night[] = { 0.25, 0.0, 1.0, 1.0 };
  static GLfloat light_specular_night[] = { 0.25, 0.0, 1.0, 1.0 };
  glLightfv (GL_LIGHT1, GL_AMBIENT, light_ambient_night);
  glLightfv (GL_LIGHT1, GL_DIFFUSE, light_diffuse_night);
  glLightfv (GL_LIGHT1, GL_SPECULAR, light_specular_night);
//  glColor3f (1.0, 1.0, 1.0);

  // clear the window
//  glClearColor (0.0, 0.0, 0.0, 0.0);
//  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // snapshot camera position
//  float view2_xyz[3];
//  float view2_hpr[3];
//  memcpy (view2_xyz,view_xyz,sizeof(float)*3);
//  memcpy (view2_hpr,view_hpr,sizeof(float)*3);

  // go to GL_MODELVIEW matrix mode and set the camera
//  glMatrixMode (GL_MODELVIEW);
//  glLoadIdentity();
//  setCamera (view2_xyz[0],view2_xyz[1],view2_xyz[2],view2_hpr[0],view2_hpr[1],view2_hpr[2]);

  // set the light position (for some reason we have to do this in model view.
  static GLfloat light_position[] = { LIGHTX, LIGHTY, 1.0, 0.0 };
  glLightfv (GL_LIGHT0, GL_POSITION, light_position);
  glLightfv (GL_LIGHT1, GL_POSITION, light_position);

  // leave openGL in a known state - flat shaded white, no textures
    // setup stuff
  glEnable (GL_LIGHTING);
  if (DayOrNight==DAY)  glEnable (GL_LIGHT0);
  else                  glEnable (GL_LIGHT1);

  glDisable (GL_TEXTURE_2D);
  glDisable (GL_TEXTURE_GEN_S);
  glDisable (GL_TEXTURE_GEN_T);

  glShadeModel (GL_SMOOTH);

  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);

  glEnable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  glFrontFace (GL_CCW);
}


void dsDrawFrame (dsFunctions *fn, int pause)
{
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  if (fn->step) fn->step (pause);
}


void dsStopGraphics()
{
    if ( Texture_Ground ) {
      glDeleteTextures( 1, &Texture_Ground );
      Texture_Ground = 0;
    }
    if ( Texture_Sky ) {
      glDeleteTextures( 1, &Texture_Sky );
      Texture_Sky = 0;
    }
}

///***************************************************************************
/// C interface

extern "C" void dsSimulationLoop (int argc, char **argv, int window_width, int window_height, bool fullscreen, dsFunctions *fn, dsInterfaces *in)
{
  // look for flags that apply to us
  int initial_pause = 0;
  for (int i=1; i<argc; i++) {
    if (strcmp(argv[i],"-notex")==0) use_textures = 0;
    if (strcmp(argv[i],"-pause")==0) initial_pause = 1;
  }

  if (fn->version > DS_VERSION)
    dsError ("bad version number in dsFunctions structure");

  dsPlatformSimLoop (window_width,window_height,fullscreen,fn,in,initial_pause);
}


extern "C" void dsSetViewpoint (float xyz[3], float hpr[3])
{
  if (xyz) {
    view_xyz[0] = xyz[0];
    view_xyz[1] = xyz[1];
    view_xyz[2] = xyz[2];
  }
}


extern "C" void dsGetViewpoint (float xyz[3], float hpr[3])
{
  if (xyz) {
    xyz[0] = view_xyz[0];
    xyz[1] = view_xyz[1];
    xyz[2] = view_xyz[2];
  }
}


extern "C" void dsSetCameraLookAt(float eyex, float eyey, float eyez,
                                  float posx, float posy, float posz,
                                  float upx,  float upy,  float upz)
{
    gluLookAt( posx,posy,posz, eyez,eyex,eyey, upx,upy,upz);
}


extern "C" void dsSetColor (float red, float green, float blue)
{
  color[0] = red;
  color[1] = green;
  color[2] = blue;
  color[3] = 1;

  glColor4f(color[0],color[1],color[2],color[3]);
}


extern "C" void dsSetColorAlpha (float red, float green, float blue, float alpha)
{
  color[0] = red;
  color[1] = green;
  color[2] = blue;
  color[3] = alpha;

  glColor4f(color[0],color[1],color[2],color[3]);
}


extern "C" void dsDrawBox (const float pos[3], const float R[12], const float sides[3])
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
  setMaterialColor (color[0],color[1],color[2],color[3]);

  glPushMatrix();
    setTransform (pos,R);
    glMultMatrixf (transform_matrix);
    drawBox (sides);
  glPopMatrix();

  glDisable(GL_BLEND);
}


extern "C" void dsDrawSphere (const float pos[3], const float R[12], float radius)
{
  glEnable (GL_NORMALIZE);
  glPushMatrix();
    setTransform (pos,R);
    glMultMatrixf (transform_matrix);
    glScaled (radius,radius,radius);
    drawSphere();
  glPopMatrix();
  glDisable (GL_NORMALIZE);
}


extern "C" void dsDrawTriangle (const float pos[3], const float R[12], const float *v0, const float *v1, const float *v2, int solid)
{
  glPushMatrix();
    setTransform (pos,R);
    glMultMatrixf (transform_matrix);
    drawTriangle (v0, v1, v2, solid);
  glPopMatrix();
}

void dsDrawCone(const float pos[3], const float R[12], float length, float radius)
{
  glPushMatrix();
    setTransform (pos,R);
    glMultMatrixf (transform_matrix);
    drawCone(length,radius,0);
  glPopMatrix();
}

void dsDrawCylinder2 (const float pos[3], const float R[12], float length, float radius1,float radius2)
{
  glPushMatrix();
    setTransform (pos,R);
    glMultMatrixf (transform_matrix);
    drawCylinder2 (length,radius1,radius2,0);
  glPopMatrix();
}


extern "C" void dsDrawCylinder (const float pos[3], const float R[12], float length, float radius)
{
  glPushMatrix();
    setTransform (pos,R);
    glMultMatrixf (transform_matrix);
    drawCylinder (length,radius,0);
  glPopMatrix();
}


extern "C" void dsDrawCappedCylinder (const float pos[3], const float R[12], float length, float radius)
{
  glPushMatrix();
    setTransform (pos,R);
    glMultMatrixf (transform_matrix);
    drawCappedCylinder (length,radius);
  glPopMatrix();
}


void dsDrawLine (const float pos1[3], const float pos2[3])
{
  bool change = glIsEnabled(GL_LIGHTING);
  int old_shade_model;
  glGetIntegerv(GL_SHADE_MODEL, &old_shade_model);

  glDisable (GL_LIGHTING);
  glColor3f (color[0],color[1],color[2]);
  glLineWidth (2);
  glShadeModel (GL_FLAT);
  glBegin (GL_LINES);
    glVertex3f (pos1[0],pos1[1],pos1[2]);
    glVertex3f (pos2[0],pos2[1],pos2[2]);
  glEnd();

  if (change) glEnable(GL_LIGHTING);
  glShadeModel(old_shade_model);
}


void dsSetSphereQuality (int n)
{
  sphere_quality = n;
}


void dsSetCappedCylinderQuality (int n)
{
  capped_cylinder_quality = n;
}

void drawCap(float l, float r)
{
  const int n = capped_cylinder_quality*4;

  l *= 0.5;
  float a = float(M_PI*2.0)/float(n);
  float sa = (float) sin(a);
  float ca = (float) cos(a);

  float start_nx = 0;
  float start_ny = 1;
  for (int j=0; j<(n/4); j++)
  {
    // get start_n2 = rotated start_n
    float start_nx2 =  ca*start_nx + sa*start_ny;
    float start_ny2 = -sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    float nx = start_nx;
    float ny = start_ny;
    float nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    glBegin (GL_TRIANGLE_STRIP);
        for (int i=0; i<=n; i++)
        {
          glNormal3d (ny2,nz2,nx2);
          glTexCoord2f (1,1);
          glVertex3d   (ny2*r,nz2*r,l+nx2*r);
          glNormal3d (ny,nz,nx);
          glTexCoord2f (1,1);
          glVertex3d   (ny*r,nz*r,l+nx*r);
          // rotate n,n2
          float tmp = ca*ny - sa*nz;
          nz = sa*ny + ca*nz;
          ny = tmp;
          tmp = ca*ny2- sa*nz2;
          nz2 = sa*ny2 + ca*nz2;
          ny2 = tmp;
        }
    glEnd();
    start_nx = start_nx2;
    start_ny = start_ny2;
  }
}

extern "C" void dsDrawSkyDome (const float pos[3], const float R[12], const float length, const float radius)
{
  dsSetColor (0.0, 0.5, 1.0);

  glDisable (GL_LIGHTING);
  glCullFace (GL_FRONT);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, Texture_Sky);

      glEnable (GL_TEXTURE_GEN_S);
      glTexGeni (GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
      static GLfloat s_params[4] = {1.0f,1.0f,1.0f,1};
      glTexGenfv (GL_S,GL_OBJECT_PLANE,s_params);

      glEnable (GL_TEXTURE_GEN_T);
      glTexGeni (GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
      static GLfloat t_params[4] = {0.817f,-0.817f,0.817f,1};
      glTexGenfv (GL_T,GL_OBJECT_PLANE,t_params);

  glPushMatrix();
    setTransform (pos,R);
    glMultMatrixf (transform_matrix);
    drawCap(length,radius);
  glPopMatrix();

  glCullFace (GL_BACK);
  glEnable (GL_LIGHTING);

      glDisable (GL_TEXTURE_GEN_S);
      glDisable (GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_2D);
}

extern "C" void dsDrawFakeGround()
{
  glPushAttrib( GL_ENABLE_BIT );
  //glDisable (GL_LIGHTING);

glEnable(GL_LIGHTING);
setMaterialColor(0.0,0.0,1.0,1.0);

  const float gsize = 10000.0f;
  const float offset = 0; // -0.001f; ... polygon offsetting doesn't work well

  const float ground_scale = 1.0f/10.0f;	// ground texture scale (1/size)
  const float ground_ofsx = 10.5;		// offset of ground texture
  const float ground_ofsy = 0.5;

  glEnable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, Texture_Ground );

  glBegin (GL_QUADS);
      glNormal3f (0,0,1);
      glTexCoord2f (-gsize*ground_scale + ground_ofsx,-gsize*ground_scale + ground_ofsy);
      glVertex3f (-gsize,-gsize,offset);
      glTexCoord2f (gsize*ground_scale + ground_ofsx,-gsize*ground_scale + ground_ofsy);
      glVertex3f (gsize,-gsize,offset);
      glTexCoord2f (gsize*ground_scale + ground_ofsx,gsize*ground_scale + ground_ofsy);
      glVertex3f (gsize,gsize,offset);
      glTexCoord2f (-gsize*ground_scale + ground_ofsx,gsize*ground_scale + ground_ofsy);
      glVertex3f (-gsize,gsize,offset);
  glEnd();

  glPopAttrib();
}

AUX_RGBImageRec *dsLoadBMP(const char *Filename)						// Loads A Bitmap Image
{
	FILE *File=NULL;												// File Handle

	if (!Filename)													// Make Sure A Filename Was Given
	{
		return NULL;												// If Not Return NULL
	}

	File=fopen(Filename,"r");										// Check To See If The File Exists

	if (File)														// Does The File Exist?
	{
		fclose(File);												// Close The Handle
		return auxDIBImageLoad(Filename);							// Load The Bitmap And Return A Pointer
	}

	return NULL;													// If Load Failed Return NULL
}

GLuint dsLoadGLTexture( const char *filename )						// Load Bitmaps And Convert To Textures
{
	AUX_RGBImageRec *pImage;										// Create Storage Space For The Texture
	GLuint texture = 0;												// Texture ID

	pImage = dsLoadBMP( filename );									// Loads The Bitmap Specified By filename

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ( pImage != NULL && pImage->data != NULL )					// If Texture Image Exists
	{
		glGenTextures(1, &texture);									// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, pImage->sizeX, pImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		free(pImage->data);											// Free The Texture Image Memory
		free(pImage);												// Free The Image Structure
	}
	else
        dsPanic(400,"Error loading BMP image '%s'.",filename);

	return texture;													// Return The Status
}

GLuint dsLoadGLMipMap( const char *filename )						// Load Bitmaps And Convert To Textures
{
	AUX_RGBImageRec *pImage;										// Create Storage Space For The Texture
	GLuint texture = 0;												// Texture ID

	pImage = dsLoadBMP( filename );									// Loads The Bitmap Specified By filename

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ( pImage != NULL && pImage->data != NULL )					// If Texture Image Exists
	{
		glGenTextures(1, &texture);									// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, pImage->sizeX, pImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, pImage->sizeX, pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_NEAREST);

		free(pImage->data);											// Free The Texture Image Memory
		free(pImage);												// Free The Image Structure
	}
	else
        dsPanic(400,"Error loading BMP image '%s'.",filename);

	return texture;													// Return The Status
}

/*
//***************************************************************************
// Texture object.

class Texture {
  Image *image;
  GLuint name;
public:
  Texture (char *filename);
  ~Texture();
  void bind (int modulate);
};


Texture::Texture (char *filename)
{
  image = new Image (filename);
  glGenTextures (1,&name);
  glBindTexture (GL_TEXTURE_2D,name);

  // set pixel unpacking mode
  glPixelStorei (GL_UNPACK_SWAP_BYTES, 0);
  glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);

  // glTexImage2D (GL_TEXTURE_2D, 0, 3, image->width(), image->height(), 0,
  //		   GL_RGB, GL_UNSIGNED_BYTE, image->data());
  gluBuild2DMipmaps (GL_TEXTURE_2D, 3, image->width(), image->height(), GL_RGB, GL_UNSIGNED_BYTE, image->data());

  // set texture parameters - will these also be bound to the texture???
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}


Texture::~Texture()
{
  delete image;
  glDeleteTextures (1,&name);
}


void Texture::bind (int modulate)
{
  glBindTexture (GL_TEXTURE_2D,name);
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, modulate ? GL_MODULATE : GL_DECAL);
}

*/
