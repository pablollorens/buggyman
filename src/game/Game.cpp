
#include <game/Game.h>

/// //////////////////////////////////////////////////////////////////////// ///
/// STATICS MEMBERS DEFINITION

dsFunctions Game::functions;
dsInterfaces Game::interfaces;
CarInfo Game::car_info;
Crono Game::crono;

Engine Game::engine;

// Default Values

int Game::width = 640;
int Game::height = 480;
bool Game::fullScreen = false;

SDLKey Game::Key_Forward = SDLK_UP;
SDLKey Game::Key_Backward = SDLK_DOWN;
SDLKey Game::Key_Left = SDLK_LEFT;
SDLKey Game::Key_Right = SDLK_RIGHT;
SDLKey Game::Key_Break = SDLK_SPACE;
SDLKey Game::Key_Action = SDLK_z;

/// //////////////////////////////////////////////////////////////////////// ///
/// RUN

void Game::Run(string & circuit, string & car)
{
  /// Init ///
  dsPrint("\tInitializing Functions & Interfaces...\n");
  FunctionsInit();
  InterfacesInit();

  dsPrint("\tCreating the World, Ground & Car...\n");
  WorldInit();
  dsPrint("\tWorld OK\n");
  WorldConfig();
  dsPrint("\tWorld Config OK\n");
  GroundInit(circuit);
  dsPrint("\tGround OK\n");
  CarInit(circuit,car);
  dsPrint("\tCar OK\n");

  /// Simulacion ///

  dsPrint("\tStart Simulation...\n");
  dsSimulationLoop (0,0,width,height,fullScreen,&functions,&interfaces);

  // printf debug info
  printf("\t# Sim: %2.3f %c\n",100*(float)Engine::ticks_sim/Engine::ticks_total,'%');
  printf("\t# Draw: %2.3f %c\n",100*(float)Engine::ticks_draw/Engine::ticks_total,'%');

  /// End ///

  dsPrint("\tDestroying the Car, Ground & World...\n");
  CarDestroy();
  GroundDestroy();
  WorldDestroy();

  dCloseODE();
}

/// //////////////////////////////////////////////////////////////////////// ///
/// FUNCTIONS

void Game::FunctionsInit()
{
  functions.version = DS_VERSION;
  functions.start = &Engine::Start;
  functions.step = &Engine::SimLoop;
  functions.command = 0;
  functions.stop = 0;
}

/// //////////////////////////////////////////////////////////////////////// ///
/// INTERFACES

void Game::InterfacesInit()
{
  // car functions
  interfaces.SpeedIncrease = &Car::SpeedIncrease;
  interfaces.SpeedDecrease = &Car::SpeedDecrease;
  interfaces.SteerIncrease = &Car::SteerIncrease;
  interfaces.SteerDecrease = &Car::SteerDecrease;
  interfaces.DoAction = &Car::DoAction;
  // camera functions
  interfaces.SetView = & Camera::SetView;
  // keys
  interfaces.Forward = Key_Forward;
  interfaces.Backward = Key_Backward;
  interfaces.Left = Key_Left;
  interfaces.Right = Key_Right;
  interfaces.Break = Key_Break;
  interfaces.Action = Key_Action;
}

/// //////////////////////////////////////////////////////////////////////// ///
/// WORLD

void Game::WorldInit()
{
  // Creacion del Mundo //
  engine.World = dWorldCreate();                        // id world
  engine.Space = dHashSpaceCreate (0);                  // id espacio de colisiones ????
  engine.ContactGroup = dJointGroupCreate (0);          //
}

void Game::WorldConfig()
{
  // Configuracion
  dWorldSetGravity (engine.World,0,0,GRAVITY);               // ajuste de gravedad
  dWorldSetCFM (engine.World,10e-5);
//  dWorldSetAutoDisableFlag (engine.world,1);               // objetos en autodisable
  dWorldSetContactMaxCorrectingVel (engine.World,dInfinity);
  dWorldSetContactSurfaceLayer (engine.World,0.001);         // precision de contacto entre objetos
}

void Game::WorldDestroy()
{
  dJointGroupDestroy (Engine::ContactGroup);
  dSpaceDestroy (Engine::Space);
  dWorldDestroy (Engine::World);
}

void Game::GroundInit(string & circuit)
{
  // Generación del Suelo
  engine.ground = new Ground(engine.World, engine.Space, circuit);
}

void Game::GroundDestroy()
{
//    free( engine.ground );
}


/// //////////////////////////////////////////////////////////////////////// ///
/// CAR

void Game::CarInit(string circuit, string car)
{
  char fcadena[300];
  CFG_File config;
  bool start = 0;
  int i=0;
  int j=0;
  int k=0;
  int rotation=0;

  /// Characteristics of Circuit
  int result = CFG_OpenFile( circuit.c_str() , &config );
  if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
  {
    dsPrint("Unable to load file: %s\n", SDL_GetError());
    exit(1);
  }
  for ( CFG_StartGroupIteration(); !CFG_IsLastGroup(); CFG_SelectNextGroup() )
  {
      start = CFG_ReadBool("start",0);
      if (start)
      {
          i = CFG_ReadInt("x",0);
          j = CFG_ReadInt("y",0);
          k = CFG_ReadInt("z",0);
          rotation = CFG_ReadInt("rotation",0);
          break;
      }
  }

  // entramos dentro del directorio de los coches
  string ruta = getcwd(fcadena,300);
  string ruta_nueva = ruta + "\\";
  ruta_nueva += DIR_VEHICLES;
  chdir(ruta_nueva.c_str());

  /// We can use "golgotha.car" too
  result = CFG_OpenFile( car.c_str() , &config );

  if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
  {
    dsPrint("Unable to load file: %s\n", SDL_GetError());
    exit(1);
  }

  car_info.name = CFG_ReadText("NAME","DESERT AP");
  car_info.carfile = CFG_ReadText("CARFILE","car.ms3d");
  car_info.wheelfile = CFG_ReadText("WHEELFILE","wheel.ms3d");
  car_info.length = CFG_ReadFloat("LENGTH",1.3);
  car_info.width = CFG_ReadFloat("WIDTH",0.5);
  car_info.height = CFG_ReadFloat("HEIGHT",0.3);
  car_info.radius = CFG_ReadFloat("RADIUS",0.2);
  car_info.posX_front = CFG_ReadFloat("POSX_FRONT",0.5);
  car_info.posX_back = CFG_ReadFloat("POSX_BACK",0.425);
  car_info.posY_front = CFG_ReadFloat("POSY_FRONT",0.35);
  car_info.posY_back = CFG_ReadFloat("POSY_BACK",0.35);
  car_info.distance = CFG_ReadFloat("DIST",0.2);

  chdir(ruta.c_str());

  engine.car = new Car(engine.World, engine.Space,car_info,i,j,k,rotation);
}

void Game::CarDestroy()
{
  dGeomDestroy (Car::Chassis_GeomID);
  for(int i=0; i<WHEELS; i++)
    dGeomDestroy (Car::Wheel_GeomID[i]);
//free( engine.car );
}
