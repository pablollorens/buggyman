
#include <game/Game.h>

/// //////////////////////////////////////////////////////////////////////// ///
/// STATICS MEMBERS DEFINITION

dsFunctions Game::functions;
dsInterfaces Game::interfaces;
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

void Game::Run()
{
  /// Init ///
  dsPrint("Initializing Game...\n");

  FunctionsInit();
  InterfacesInit();

  WorldInit();
  WorldConfig();
  GroundInit();
  CarInit();

  /// Simulacion ///
  dsPrint("Simulating...\n");

  dsSimulationLoop (0,0,width,height,fullScreen,&functions,&interfaces);

  // printf debug info
  printf("\tSim: %2.3f %c\n",100*(float)Engine::ticks_sim/Engine::ticks_total,'%');
  printf("\tDraw: %2.3f %c\n",100*(float)Engine::ticks_draw/Engine::ticks_total,'%');

  /// End ///

  CarDestroy();
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
  functions.path_to_textures = "./textures";
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

void Game::GroundInit()
{
  // Generación del Suelo
  engine.ground = new Ground(engine.World, engine.Space);
}


/// //////////////////////////////////////////////////////////////////////// ///
/// CAR

void Game::CarInit()
{
  engine.car = new Car(engine.World, engine.Space,1,1,0,90);
}

void Game::CarDestroy()
{
  dGeomDestroy (Car::Chassis_GeomID);
  for(int i=0; i<WHEELS; i++)
    dGeomDestroy (Car::Wheel_GeomID[i]);
}
