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
#include <time.h>

#include <game/utils.h>

using namespace std;

/// //////////////////////////////////////////////////////////////////////// ///

class Ground {

  public:

    Ground(dWorldID world, dSpaceID space);

    static void LoadTextures();
    static void Draw(int cell_x, int cell_y);

    static Ground_Cell Cell_Matrix[X][Y];
    static int Checkpoints_Total;

    static Model * ModelosMilky[K_MODEL];
    static dTriMeshDataID MeshesData[K_MODEL];
    static Model * ground_Model;

/// //////////////////////////////////////////////////////////////////////// ///

  private:

};

#endif
