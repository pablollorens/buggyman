
#include <game/Engine.h>

/// //////////////////////////////////////////////////////////////////////// ///
/// STATICS MEMBERS DEFINITION

dWorldID Engine::World;
dSpaceID Engine::Space;
dJointGroupID Engine::ContactGroup;

float Engine::xyz[3] = { ((X-1)*7)-3.5f, -1.5f, 3.0f };
float Engine::hpr[3] = { 75.0f, -27.5f, 0.0f };

int Engine::car_Cell_X = 0;
int Engine::car_Cell_Y = 0;

int Engine::car_Cell_X_Last = 0;
int Engine::car_Cell_Y_Last = 0;

float Engine::Step_Sensibility = 0.05;

dGeomID Engine::AUX1 = 0;
dGeomID Engine::AUX2 = 0;

int Engine::update = 2;

Uint32 Engine::ticks_total;
Uint32 Engine::ticks_sim;
Uint32 Engine::ticks_draw;

/// //////////////////////////////////////////////////////////////////////// ///
/// ENGINE FUNCTIONS

// start simulation - set viewpoint

void Engine::Start()
{
  dsSetViewpoint (xyz,hpr);

  dsPrint (".. Loading Textures\n");

  Car::LoadTextures();
  Ground::LoadTextures();

  dsPrint ("# Initializing ODE\n");
}

/// //////////////////////////////////////////////////////////////////////// ///

// simulation loop

void Engine::SimLoop (int pause)
{
    Uint32 ticks_start = SDL_GetTicks();

    /// SIM ///

    if (!pause)
    {
        Car::Sim();

        switch(Ground::Cell_Matrix[car_Cell_X][car_Cell_Y].num_modelo)
        {
            case RAMP_B: Car::MAX_SPEED = 50; break;
            case STRAIGHT: Car::MAX_SPEED = 200; break;
            default: if (Car::MAX_SPEED>15) Car::MAX_SPEED--; break;
        }

        dSpaceCollide (Space,0,&nearCallback);

        dWorldStep (World,Step_Sensibility);

        dJointGroupEmpty (ContactGroup);
    }

    for(int i=0;i<X;i++)
        for(int j=0; j<Y; j++)
        {
            dGeomDisable(Ground::Cell_Matrix[i][j].geomID);
            if (Ground::Cell_Matrix[i][j].modelo)
                dGeomDisable(Ground::Cell_Matrix[i][j].geomPista);
        }

    Camera::Update();

    Uint32 ticks_start2 = SDL_GetTicks();
    ticks_sim += ticks_start2 - ticks_start;

    /// DRAW ///

    GetCarCell(); // update Car_Cell_X, Car_Cell_Y
    Ground::Draw(car_Cell_X, car_Cell_Y);

    Car::Draw();

    // importante escribir al final de los draw
    dsGLPrint(100,100,"%d",Ground::Cell_Matrix[car_Cell_X][car_Cell_Y].num_modelo);

    /// DEBUG INFO
    Uint32 ticks_end = SDL_GetTicks();
    ticks_draw += ticks_end - ticks_start2;
    ticks_total += ticks_end - ticks_start;
}

/// //////////////////////////////////////////////////////////////////////// ///

// this is called by dSpaceCollide when two objects in space are potentially colliding.
void Engine::nearCallback (void *data, dGeomID o1, dGeomID o2)
{

  dContact contact[MAX_CONTACTS];   // up to MAX_CONTACTS contacts per box-box
  int n = dCollide (o1,o2,MAX_CONTACTS,&contact[0].geom,sizeof(dContact));

  if (n > 0)
   for (int i=0; i<n; i++)
   {
      contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
      contact[i].surface.mu = dInfinity;
      contact[i].surface.slip1 = 0.1;
      contact[i].surface.slip2 = 0.1;
      //contact[i].surface.bounce = 0.1;
      //contact[i].surface.bounce_vel = 0.1;
      contact[i].surface.soft_erp = 0.5;
      contact[i].surface.soft_cfm = 0.3;
      dJointID c = dJointCreateContact (World,ContactGroup,&contact[i]);
      dJointAttach (c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
   }

   /// collide exceptions ///

  // car stuff //

//  if ( dAreConnected (Car::Chassis_BodyID, Ground::BodyID) )
//  {
//      if ( dAreConnected (Car::Chassis_BodyID, Car::Stuff_BodyID[0]) )
//      {
//        dJointDestroy(Car::Stuff_joint[0]);
//        dBodyAddForce(Car::Stuff_BodyID[0],2,2,2);
//      }
//      if ( dAreConnected (Car::Chassis_BodyID, Car::Stuff_BodyID[1]) )
//      {
//        dJointDestroy(Car::Stuff_joint[1]);
//        dBodyAddForce(Car::Stuff_BodyID[1],2,-2,2);
//      }
//  }
}

/// //////////////////////////////////////////////////////////////////////// ///

void Engine::GetCarCell()
{
  // detecta la celda sobre la cual se encuentra el coche (celdas de tamaño 7)
  car_Cell_X_Last = car_Cell_X;
  car_Cell_Y_Last = car_Cell_Y;

  dReal car_x = dBodyGetPosition(Car::Chassis_BodyID)[0] + 3.5; // correccion para esquina celda 1 en (0,0)
  dReal car_y = dBodyGetPosition(Car::Chassis_BodyID)[1] + 3.5; // 7/2

  car_Cell_X = (int)(car_x / 7.0);
  car_Cell_Y = (int)(car_y / 7.0);
}

/// //////////////////////////////////////////////////////////////////////// ///
