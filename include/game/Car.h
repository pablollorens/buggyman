#ifndef CLASS_CAR
#define CLASS_CAR

#include <list>
#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include <game/MilkshapeModel.h>
#include <game/utils.h>
#include <unistd.h>
#include <string>
using namespace std;

#define DENSITY (0.5)		// density of all objects

#define LENGTH 1.6	// chassis length
#define WIDTH  0.5	// chassis width
#define HEIGHT 0.5	// chassis height
#define STARTZ 1.05	// starting height of chassis

#define CMASS 120	// chassis mass
#define WMASS 50     // wheel mass

#define WHEELS 4
#define RADIUS 0.2
#define WHEEL_WIDTH 0.1f

//#define MAX_SPEED 18
#define MIN_SPEED 10
#define MAX_STEER 0.7

#define STEER_SENSIBILITY 0.05
#define SPEED_ACCEL_SENSIBILITY 0.5
#define SPEED_BREAK_SENSIBILITY 0.4

#define DIR_VEHICLES "vehicles"

/// //////////////////////////////////////////////////////////////////////// ///

class Car {

  public:

    Car(dWorldID world, dSpaceID space,int x,int y,int z,int rotation);

    static void LoadTextures();
    static void Draw();
    static void Sim();
    static void setPosCar(int x, int y);

    static dSpaceID Car_Space;

    static dBodyID Chassis_BodyID;
    static dGeomID Chassis_GeomID;

    static dBodyID Wheel_BodyID[WHEELS];
    static dGeomID Wheel_GeomID[WHEELS];   // wheels
    static dJointID Wheel_JointID[WHEELS];	// joint[0] is the front wheel

//    static dBodyID Stuff_BodyID[4];
//    static dGeomID Stuff_GeomID[4];
//    static dJointID Stuff_joint[4];

    // things that the user controls

    static void SpeedIncrease(int sign);
    static void SpeedDecrease();
    static void SteerIncrease(int sign);
    static void SteerDecrease();
    static void DoAction(int action);

    static dReal speed,steer;	// user commands

    static int CheckpointList[1024];

    ///PRUEBAS
    static int MAX_SPEED;

/// //////////////////////////////////////////////////////////////////////// ///

  private:

    static Model *car_Model;												// Holds The Model Data
    static Model *wheel_Model;
//    static Model *stuff1_Model;
//    static Model *stuff2_Model;
};

#endif
