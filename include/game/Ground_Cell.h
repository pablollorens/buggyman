#ifndef CLASS_GROUND_CELL
#define CLASS_GROUND_CELL

#include <ode/ode.h>
#include <game/milkshapemodel.h>

class Ground_Cell {

  public:

    Ground_Cell();

    dGeomID geomID;
    dGeomID geomPista;
    Model * modelo;
    int num_modelo;

    bool arbol;

    int Checkpoint;

  private:

};

#endif
