#ifndef CLASS_GROUND
#define CLASS_GROUND

#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include <game/milkshapemodel.h>
#include <game/Ground_Cell.h>
#include <SDL_Config/SDL_Config.h>

#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <list>
#include <functions.h>
#include <utils.h>
#include <paths.h>

using namespace std;

typedef struct TrackInfo {
  int id;
  string name;
  Model * roadmodel;
  dTriMeshDataID roadmesh;
} TrackInfo;

/// //////////////////////////////////////////////////////////////////////// ///

class Ground {

  public:

    Ground(dWorldID world, dSpaceID space);

    static void LoadTextures();
    static void Draw(int cell_x, int cell_y);

    static Ground_Cell Cell_Matrix[GRID_X][GRID_Y];
    static int Checkpoints_Total;

    static Model * ground_Model;
    static dGeomID walls[4];

    static map< string, TrackInfo* > track_map;

/// //////////////////////////////////////////////////////////////////////// ///

  private:

};

#endif
