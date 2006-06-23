#include <game/Car.h>

/// //////////////////////////////////////////////////////////////////////// ///
/// STATICS MEMBERS DEFINITION

// buggy

dSpaceID Car::Car_Space;

dBodyID Car::Chassis_BodyID;
dGeomID Car::Chassis_GeomID;

dGeomID Car::platform;
dGeomID Car::camera_box;
dGeomID Car::camera_view_box;

dBodyID Car::Wheel_BodyID[WHEELS];
dGeomID Car::Wheel_GeomID[WHEELS];
dJointID Car::Wheel_JointID[WHEELS];


const dReal * Car::car_wheel_right;
const dReal * Car::car_wheel_left;
dVector3 Car::car_prey;

//dBodyID Car::Stuff_BodyID[4];
//dGeomID Car::Stuff_GeomID[4];
//dJointID Car::Stuff_joint[WHEELS];

// things that the user controls

dReal Car::speed = 0;
dReal Car::steer = 0;

Model *Car::car_Model = NULL;												// Holds The Model Data
Model *Car::wheel_Model = NULL;												// Holds The Model Data
//Model *Car::stuff1_Model = NULL;
//Model *Car::stuff2_Model = NULL;

int Car::CheckpointList[1024];
int Car::MAX_SPEED = 16;

/// //////////////////////////////////////////////////////////////////////// ///
/// CONSTRUCTOR

Car::Car(dWorldID world, dSpaceID space,int x1,int y1,int z1,int rotation)
{
  x1=X-1-x1;

  /// CHECKPOINTS
  for (int i=0; i<32; i++)
    CheckpointList[i] = false;


  /// CARGA DE MODELOS
  car_Model = new MilkshapeModel();
  wheel_Model = new MilkshapeModel();
  // entramos dentro del directorio de los coches
  char fcadena[300];
  string ruta = getcwd(fcadena,300);
  string ruta_nueva = ruta + "\\";
  ruta_nueva += DIR_VEHICLES;
  chdir(ruta_nueva.c_str());
  if ( car_Model->loadModelData( "car.ms3d" ) == false )		// Loads The Model And Checks For Errors
    dsPanic("Couldn't load the model car.ms3d");
  if ( wheel_Model->loadModelData( "wheel.ms3d" ) == false )
    dsPanic("Couldn't load the model wheel.ms3d");
  // salimos al directorio raiz
  chdir(ruta.c_str());

  /// INITIAL PLATFORM
  platform = dCreateBox (0,6,6,1);
  dGeomSetPosition(platform,x1*7,y1*7,STARTZ-3);
  dSpaceAdd(space,platform);

  /// CHASSIS
  Chassis_BodyID = dBodyCreate (world);
  dBodySetPosition (Chassis_BodyID,x1*7,y1*7,STARTZ);

  // geom
  Chassis_GeomID = dCreateBox (0, LENGTH, WIDTH, HEIGHT);
  dGeomSetBody (Chassis_GeomID, Chassis_BodyID);
  // mass
  dMass m;
  dMassSetBox (&m, DENSITY, LENGTH, WIDTH, HEIGHT);
  //dMassAdjust (&m,CMASS);
  dBodySetMass (Chassis_BodyID, &m);

  /// WHEELS
  for (int i=0; i<WHEELS; i++)
  {
      // body
    Wheel_BodyID[i] = dBodyCreate (world);
    dQuaternion q;
    dQFromAxisAndAngle (q,1,0,0,M_PI*0.5);
    dBodySetQuaternion (Wheel_BodyID[i],q);

      // geom
    Wheel_GeomID[i] = dCreateSphere (0,RADIUS);
    dGeomSetBody (Wheel_GeomID[i],Wheel_BodyID[i]);

      // mass
    dMassSetSphere (&m,DENSITY,RADIUS);
    //dMassAdjust (&m,WMASS);
    dBodySetMass (Wheel_BodyID[i],&m);
  }

    // Para Desert apc
    dBodySetPosition (Wheel_BodyID[0],(x1*7)+( 0.25*2.0),(y1*7)+0.35,STARTZ-0.2);
    dBodySetPosition (Wheel_BodyID[1],(x1*7)+( 0.25*2.0),(y1*7)-0.35,STARTZ-0.2);
    dBodySetPosition (Wheel_BodyID[2],(x1*7)+(-0.25*1.7),(y1*7)+0.35,STARTZ-0.2);
    dBodySetPosition (Wheel_BodyID[3],(x1*7)+(-0.25*1.7),(y1*7)-0.35,STARTZ-0.2);
    // Para golgotha
//    dBodySetPosition (Wheel_BodyID[0], 0.25*2.0, 0.45,0.81);
//    dBodySetPosition (Wheel_BodyID[1], 0.25*2.0,-0.45,0.81);
//    dBodySetPosition (Wheel_BodyID[2],-0.25*1.7, 0.45,0.81);
//    dBodySetPosition (Wheel_BodyID[3],-0.25*1.7,-0.45,0.81);


  // front and back wheel hinges
  for (int i=0; i<WHEELS; i++) {
    Wheel_JointID[i] = dJointCreateHinge2 (world,0);
    dJointAttach (Wheel_JointID[i],Chassis_BodyID,Wheel_BodyID[i]);
    const dReal *a = dBodyGetPosition (Wheel_BodyID[i]);
    dJointSetHinge2Anchor (Wheel_JointID[i],a[0],a[1],a[2]);
    dJointSetHinge2Axis1 (Wheel_JointID[i],0,0,1);
    dJointSetHinge2Axis2 (Wheel_JointID[i],0,1,0);
  }

  // set joint suspension
  for (int i=0; i<WHEELS; i++) {
    dJointSetHinge2Param (Wheel_JointID[i],dParamSuspensionERP,0.4);
    dJointSetHinge2Param (Wheel_JointID[i],dParamSuspensionCFM,0.8);
  }

  // lock back wheels along the steering axis
  for (int i=2; i<WHEELS; i++) {
    // set stops to make sure wheels always stay in alignment
    dJointSetHinge2Param (Wheel_JointID[i],dParamLoStop,0);
    dJointSetHinge2Param (Wheel_JointID[i],dParamHiStop,0);
  }


//  // Car Stuff //
//
//
//  //milkshape
//  stuff1_Model = new MilkshapeModel();
//  stuff1_Model->loadModelData( "data/Stuff1.ms3d" );
//  stuff2_Model = new MilkshapeModel();
//  stuff2_Model->loadModelData( "data/Stuff2.ms3d" );
//
//  for (i=0; i<4; i++) {
//    Stuff_BodyID[i] = dBodyCreate (world);
//  }
//
//  dBodySetPosition (Stuff_BodyID[0], 0.25*2.0, 0.30,0.98);
//  dBodySetPosition (Stuff_BodyID[1], 0.25*2.0,-0.33,0.98);
//  dBodySetPosition (Stuff_BodyID[2],-0.25*1.9, 0.30,0.98);
//  dBodySetPosition (Stuff_BodyID[3],-0.25*1.9,-0.33,0.98);
//
//  for (i=0; i<4; i++) {
//    dMassSetBox (&m, DENSITY, 0.4, 0.3, 0.2);
////    dMassAdjust (&m,CMASS);
//    dBodySetMass (Stuff_BodyID[i], &m);
//
//    Stuff_GeomID[i] = dCreateBox (0, 0.4, 0.3, 0.2);
//    dGeomSetBody (Stuff_GeomID[i], Stuff_BodyID[i]);
//  }
//
//  for (i=0; i<4; i++) {
//    Stuff_joint[i] = dJointCreateFixed (world,0);
//    dJointAttach (Stuff_joint[i], Stuff_BodyID[i], Chassis_BodyID);
//    dJointSetFixed (Stuff_joint[i]);
//  }

  // SPACE //

  // create car space and add it to the top level space
  Car_Space = dSimpleSpaceCreate (space);
  dSpaceSetCleanup (Car_Space, 0);
  dSpaceAdd (Car_Space, Chassis_GeomID);
  for (int i=0; i<WHEELS; i++)
  {
    dSpaceAdd (Car_Space, Wheel_GeomID[i]);
    //dSpaceAdd (Car_Space, Stuff_GeomID[i]);
  }


  dMatrix3 R;
  if (rotation==180) dRFromAxisAndAngle(R,0,0,1,+M_PI);
  if (rotation==270) dRFromAxisAndAngle(R,0,0,1,+(3*M_PI)/2);
  if (rotation==0)   dRFromAxisAndAngle(R,0,0,1,0);
  if (rotation==90)  dRFromAxisAndAngle(R,0,0,1,+M_PI/2);
  dBodySetRotation(Chassis_BodyID,R);

  /// CAMERA BOX
  camera_box = dCreateBox (0,0.1,0.1,0.1);
  camera_view_box = dCreateBox (0,0.1,0.1,0.1);

  dGeomSetPosition(camera_box,x1*7,y1*7,dBodyGetPosition(Chassis_BodyID)[2]+0.4);

  car_wheel_right = dBodyGetPosition(Wheel_BodyID[0]);
  car_wheel_left  = dBodyGetPosition(Wheel_BodyID[1]);

  for(int coord=0; coord < 3; coord++)
    car_prey[coord] = (car_wheel_right[coord]+car_wheel_left[coord])/2.0;
  dGeomSetPosition(camera_view_box,car_prey[0],car_prey[1],car_prey[2]+0.4);

  // A�adimos camaras al espacio de colisiones del coche
  dSpaceAdd (Car_Space, camera_box);
  dSpaceAdd (Car_Space, camera_view_box);
}

void Car::setPosCar(int i, int j, int z, int rotation)
{
    i=X-1-i;

    // Ponemos la variable z temporalmente al valor de STARTZ
    z = (int)STARTZ;
    dGeomSetPosition (platform,i*7,j*7,z-3);

    dBodySetPosition (Chassis_BodyID,i*7,j*7,z);

    dBodySetPosition (Wheel_BodyID[0],(i*7)+( 0.25*2.0),(j*7)+0.35,z-0.2);
    dBodySetPosition (Wheel_BodyID[1],(i*7)+( 0.25*2.0),(j*7)-0.35,z-0.2);
    dBodySetPosition (Wheel_BodyID[2],(i*7)+(-0.25*1.7),(j*7)+0.35,z-0.2);
    dBodySetPosition (Wheel_BodyID[3],(i*7)+(-0.25*1.7),(j*7)-0.35,z-0.2);

    dMatrix3 R;
    if (rotation==180) dRFromAxisAndAngle(R,0,0,1,+M_PI);
    if (rotation==270)  dRFromAxisAndAngle(R,0,0,1,+(3*M_PI)/2);
    if (rotation==0)  dRFromAxisAndAngle(R,0,0,1,0);
    if (rotation==90)  dRFromAxisAndAngle(R,0,0,1,+M_PI/2);
    dBodySetRotation(Chassis_BodyID,R);


}

void Car::Update_Camera_Box()
{
    dGeomSetPosition(camera_box,
                        dBodyGetPosition(Chassis_BodyID)[0],
                        dBodyGetPosition(Chassis_BodyID)[1],
                        dBodyGetPosition(Chassis_BodyID)[2]+0.3);
    const dReal * rotation =  dBodyGetRotation(Chassis_BodyID);
    dGeomSetRotation(camera_box,rotation);


    car_wheel_right = dBodyGetPosition(Wheel_BodyID[0]);
    car_wheel_left  = dBodyGetPosition(Wheel_BodyID[1]);

    for(int coord=0; coord < 3; coord++)
    car_prey[coord] = (car_wheel_right[coord]+car_wheel_left[coord])/2.0;
    dGeomSetPosition(camera_view_box,car_prey[0],car_prey[1],car_prey[2]+0.4);
    dGeomSetRotation(camera_view_box,rotation);

}

/// //////////////////////////////////////////////////////////////////////// ///
/// DRAW

void Car::LoadTextures()
{
    char fcadena[300];
    string ruta = getcwd(fcadena,300);
    string ruta_nueva = ruta + "\\vehicles";
    chdir(ruta_nueva.c_str());
    car_Model->reloadTextures();
    wheel_Model->reloadTextures();
    chdir(ruta.c_str());
}

void Car::Draw()
{
  // chassis //

	car_Model->draw(1,dBodyGetPosition(Chassis_BodyID), dBodyGetRotation(Chassis_BodyID));

  // wheels //

    wheel_Model->draw(2,dBodyGetPosition(Wheel_BodyID[0]), dBodyGetRotation(Wheel_BodyID[0]));
    wheel_Model->draw(2,dBodyGetPosition(Wheel_BodyID[1]), dBodyGetRotation(Wheel_BodyID[1]));
    wheel_Model->draw(2,dBodyGetPosition(Wheel_BodyID[2]), dBodyGetRotation(Wheel_BodyID[2]));
    wheel_Model->draw(2,dBodyGetPosition(Wheel_BodyID[3]), dBodyGetRotation(Wheel_BodyID[3]));

  // car stuff //

//    stuff1_Model->draw( dBodyGetPosition(Stuff_BodyID[0]), dBodyGetRotation(Stuff_BodyID[0]) );
//    stuff1_Model->draw( dBodyGetPosition(Stuff_BodyID[2]), dBodyGetRotation(Stuff_BodyID[2]) );
//    stuff2_Model->draw( dBodyGetPosition(Stuff_BodyID[1]), dBodyGetRotation(Stuff_BodyID[1]) );
//    stuff2_Model->draw( dBodyGetPosition(Stuff_BodyID[3]), dBodyGetRotation(Stuff_BodyID[3]) );

  // transparent geoms //

//    dsSetColorAlpha (1,0,0,0.5);
//    dReal sides1[3] = {0.4,0.3,0.2};
//    dsDrawBox ( dBodyGetPosition(Stuff_BodyID[0]), dBodyGetRotation(Stuff_BodyID[0]), sides1);
//    dsDrawBox ( dBodyGetPosition(Stuff_BodyID[1]), dBodyGetRotation(Stuff_BodyID[1]), sides1);
//    dsDrawBox ( dBodyGetPosition(Stuff_BodyID[2]), dBodyGetRotation(Stuff_BodyID[2]), sides1);
//    dsDrawBox ( dBodyGetPosition(Stuff_BodyID[3]), dBodyGetRotation(Stuff_BodyID[3]), sides1);

//    dsSetColorAlpha (0,0,0,0.4);
//    dReal sides2[3] = {LENGTH,WIDTH,HEIGHT};
//    dsDrawBox ( dBodyGetPosition(Chassis_BodyID), dBodyGetRotation(Chassis_BodyID), sides2);

    dsSetColorAlpha (0,1,0,0.5);
    dReal sides3[3] = {6,6,1};
    dsDrawBox( dGeomGetPosition(platform),dGeomGetRotation(platform),sides3);

//    dReal sides_camera_box[3] = {0.4,0.4,0.4};
//    dsDrawBox( dGeomGetPosition(camera_box),dGeomGetRotation(camera_box),sides_camera_box);
//    dsDrawBox( dGeomGetPosition(camera_view_box),dGeomGetRotation(camera_view_box),sides_camera_box);
}

/// //////////////////////////////////////////////////////////////////////// ///
/// CAR SIMULATION

void Car::SpeedIncrease(int sign)
{
  if (sign > 0)
  {
    if (speed < MAX_SPEED)
      speed += SPEED_ACCEL_SENSIBILITY;
  }
  else
  if (sign < 0)
  {
    if (speed > -MIN_SPEED)
      speed -= SPEED_ACCEL_SENSIBILITY;
  }
}

void Car::SpeedDecrease()
{
    if (speed < 0)
      speed += SPEED_BREAK_SENSIBILITY;
    if (speed > 0)
      speed -= SPEED_BREAK_SENSIBILITY;
}

void Car::SteerIncrease(int sign)
{
  if (sign > 0)
  {
    if (steer > -MAX_STEER)
      steer -= STEER_SENSIBILITY;  }
  else
  if (sign < 0)
  {
    if (steer < MAX_STEER)
      steer += STEER_SENSIBILITY;
  }
}

void Car::SteerDecrease()
{
   if (steer < 0)
        steer += STEER_SENSIBILITY;
   if (steer > 0)
        steer -= STEER_SENSIBILITY;
}

void Car::DoAction(int action)
{
  switch (action)
  {
     case 1:
       dBodySetForce(Chassis_BodyID,0.0,0,10);
       break;
     case 2:
       // Tecla "R" = reiniciar partida
       speed = 0;           // Paramos el coche
       steer = 0;           // Ponemos las ruedas rectas
       setPosCar(1,1,0,90); // Nos movemos al principio
       setJugando(0);       // Volvemos a jugar
       setCronometro(0);    // Pararemos el cronometro al final
       resetTempo();        // Reseteamos la cuenta atras
       resetRecord();       // Reseteamos el record
       break;
     case 3:
       speed = 0;
       steer = 0;
       break;
    }
}

void Car::Sim()
{
    // ruedas delanteras [0,1]
    for (int i=0; i<2; i++)
    {
        // motor
        dJointSetHinge2Param (Wheel_JointID[i], dParamVel2, -speed);
        dJointSetHinge2Param (Wheel_JointID[i], dParamFMax2, 0.1);

        // steering
        dReal v = steer - dJointGetHinge2Angle1 (Wheel_JointID[i]);

        if (v > 0.1) v = 0.1;
        if (v < -0.1) v = -0.1;
        v *= 10.0;

        dJointSetHinge2Param (Wheel_JointID[i], dParamVel, v);
        dJointSetHinge2Param (Wheel_JointID[i], dParamFMax, 0.2);
        dJointSetHinge2Param (Wheel_JointID[i], dParamLoStop, -0.75);
        dJointSetHinge2Param (Wheel_JointID[i], dParamHiStop, 0.75);
        dJointSetHinge2Param (Wheel_JointID[i], dParamFudgeFactor, 0.1);
    }
}
