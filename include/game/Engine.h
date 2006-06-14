#ifndef CLASS_ENGINE
#define CLASS_ENGINE

#include <ode/ode.h>
#include <drawstuff/drawstuff.h>

#include <SDL/SDL_TTF.h>

#include <game/Car.h>
#include <game/Ground.h>
#include <game/Camera.h>

#include <game/utils.h>

/// //////////////////////////////////////////////////////////////////////// ///

#define MAX_CONTACTS 10		// maximum number of contact points per body

/// //////////////////////////////////////////////////////////////////////// ///

class Engine {

  public:

    static void Start();
    static void SimLoop (int pause);

    static dWorldID World;
    static dSpaceID Space;
    static dJointGroupID ContactGroup;

    void Ini();
    static void GetCarCell();

    Car *car;
    Ground *ground;

/// //////////////////////////////////////////////////////////////////////// ///

  private:

    static void Engine::nearCallback (void *data, dGeomID o1, dGeomID o2);

    static float xyz[3];
    static float hpr[3];

    static int car_Cell_X;
    static int car_Cell_Y;

    static int car_Cell_X_Last;
    static int car_Cell_Y_Last;

    static float Step_Sensibility;

    static dGeomID AUX1,AUX2;
    static int update;
};

#endif
