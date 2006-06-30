#ifndef CLASS_GAME
#define CLASS_GAME

#include <SDL/SDL.h>
#include <ode/ode.h>
#include <drawstuff/drawstuff.h>

#include <game/Engine.h>
#include <game/Crono.h>

#define GRAVITY -2.5

/// //////////////////////////////////////////////////////////////////////// ///

class Game {

  public:

    static void Run();

    static void FunctionsInit();
    static void InterfacesInit();

    static void WorldInit();
    static void WorldConfig();
    static void WorldDestroy();
    static void GroundInit();
    static void GroundDestroy();
    static void CarInit();
    static void CarDestroy();

    static Engine engine;
    static Crono crono;

    /// CONFIGURATION

    inline static void SetWidth(int w) { width = w; }
    inline static void SetHeight(int h) { height = h; }
    inline static void SetResolution(int w, int h) { width = w; height = h; }
    inline static void SetFullScreen(bool fs) { fullScreen = fs; }

    inline static void SetKeyForward (SDLKey k) { Key_Forward = k; }
    inline static void SetKeyBackward (SDLKey k) { Key_Backward = k; }
    inline static void SetKeyLeft (SDLKey k) { Key_Left = k; }
    inline static void SetKeyRight (SDLKey k) { Key_Right = k; }
    inline static void SetKeyBreak (SDLKey k) { Key_Break = k; }
    inline static void SetKeyAction (SDLKey k) { Key_Action = k; }

/// //////////////////////////////////////////////////////////////////////// ///

  private:

    static dsFunctions functions;
    static dsInterfaces interfaces;
    static CarInfo car_info;

    static int width;
    static int height;
    static bool fullScreen;

    static SDLKey Key_Forward;
    static SDLKey Key_Backward;
    static SDLKey Key_Left;
    static SDLKey Key_Right;
    static SDLKey Key_Break;
    static SDLKey Key_Action;
};

#endif
