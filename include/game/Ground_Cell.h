#ifndef CLASS_GROUND_CELL
#define CLASS_GROUND_CELL

#include <ode/ode.h>
#include <game/milkshapemodel.h>
#include <string>
#include <utils.h>
using namespace std;

class Ground_Cell {

  public:

    Ground_Cell();

    string name;
    dGeomID geomID;
    dGeomID road_geom;
    Model * road_model;
    int id;
    int i_checkpoint;
    int start;
    int speed_factor;

  private:

};

#endif
