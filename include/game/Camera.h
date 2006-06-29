#ifndef CLASS_CAMERA
#define CLASS_CAMERA

#include <ode/ode.h>
#include <drawstuff/drawstuff.h>

#include <game/Car.h>

class Camera
{
  public:

    static void Update();
    static void SetView(int view);

    static dReal seguir[3];
    static bool move;
    static bool interior;
    static bool vista_micro;

  private:

    static float camera_height;
    static float distance_actual;
    static float distance_view;

    static float cam_speed;

};

#endif
