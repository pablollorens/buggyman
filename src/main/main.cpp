
#include <cstdlib>
#include <windows.h>
#include <editor.h>
#include <vector>
#include <map>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <game/Game.h>

using namespace std;
IMG_Collection image_collection;

SDL_Surface *screen;

#define MENU_NEW_GAME_X 271
#define MENU_NEW_GAME_Y 255
#define MENU_NEW_GAME_W 257
#define MENU_NEW_GAME_H 40

#define MENU_TRACK_EDITOR_X 216
#define MENU_TRACK_EDITOR_Y 314
#define MENU_TRACK_EDITOR_W 368
#define MENU_TRACK_EDITOR_H 38

#define MENU_EXIT_X 343
#define MENU_EXIT_Y 431
#define MENU_EXIT_W 115
#define MENU_EXIT_H 37

int Run_Game_LowRes(void* data)
{
    //SDL_Quit();
    Game::SetResolution(320,240);
    Game::SetFullScreen(false);
    Game::Run();
    return 0;
}

int Run_Game_HighRes(void* data)
{
    //SDL_Quit();
    Game::SetResolution(640,480);
    Game::SetFullScreen(false);
    Game::Run();
    return 0;
}

int Run_Editor(void* data)
{
    Editor ed;
    ed.Run(screen, "terreno.cfg");
    return 0;
}

int Quit_Game(void* data)
{
    data = (bool*)true;
    return 0;
}

struct func_p
{
    Rect2D pos;
    void* data;
    int (*func)(void* data);
};

int main ( int argc, char** argv )
{
    bool update_required = 1;

    // //////////////////////////////////////// //
    // Posible codigo para botones y su gestion
    // //////////////////////////////////////// //
    //tabla hash de punteros a funcion indexado por point3D's
    map< pair<int,int> , func_p* > actions;

    //usamos los struct como si fueran la clase boton
    //las creamos y configuramos
    func_p* playlow = new func_p;
    playlow->func = Run_Game_LowRes;
    playlow->data = NULL;
    playlow->pos.Set_values(242,208,317,47);

    func_p* playhigh = new func_p;
    playhigh->func = Run_Game_HighRes;
    playhigh->data = NULL;
    playhigh->pos.Set_values(241,268,319,47);

    func_p* runeditor = new func_p;
    runeditor->func = Run_Editor;
    runeditor->data = NULL;
    runeditor->pos.Set_values(277,324,247,39);

    func_p* quit_game = new func_p;
    quit_game->func = Quit_Game;
    quit_game->data = (bool*)true;
    quit_game->pos.Set_values(362,437,77,39);

    //insercion en la tabla hash
    for(int i = playlow->pos.x, n = playlow->pos.x + playlow->pos.w; i<n; i++)
        for(int j = playlow->pos.y, m = playlow->pos.y + playlow->pos.h; j<m; j++)
        {
            pair<int,int> aux(i,j);
            actions[aux] = playlow;
        }
    for(int i = playhigh->pos.x, n = playhigh->pos.x + playhigh->pos.w; i<n; i++)
        for(int j = playhigh->pos.y, m = playhigh->pos.y + playhigh->pos.h; j<m; j++)
        {
            pair<int,int> aux(i,j);
            actions[aux] = playhigh;
        }
    for(int i = runeditor->pos.x, n = runeditor->pos.x + runeditor->pos.w; i<n; i++)
        for(int j = runeditor->pos.y, m = runeditor->pos.y + runeditor->pos.h; j<m; j++)
        {
            pair<int,int> aux(i,j);
            actions[aux] = runeditor;
        }
    for(int i = quit_game->pos.x, n = quit_game->pos.x + quit_game->pos.w; i<n; i++)
        for(int j = quit_game->pos.y, m = quit_game->pos.y + quit_game->pos.h; j<m; j++)
        {
            pair<int,int> aux(i,j);
            actions[aux] = quit_game;
        }


    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }
    //atexit(SDL_Quit);

    // create a new window
    /*SDL_Surface* */screen = SDL_SetVideoMode(
        800, 600, 32,
        SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen ) { printf("Unable to set video: %s\n", SDL_GetError()); return 1; }
    SDL_ShowCursor(SDL_DISABLE);

    // load an image
    Rect2D position;
    SDL_Surface* background = image_collection("menu/backg_main.png");
    SDL_Surface* button_play320 = image_collection("menu/button_play320_def.png");
    position.Set_values(242,208,317,47);
    SDL_BlitSurface(button_play320, 0, background, &position);
    SDL_Surface* button_play640 = image_collection("menu/button_play640_def.png");
    position.Set_values(241,268,319,47);
    SDL_BlitSurface(button_play640, 0, background, &position);
    SDL_Surface* button_editor = image_collection("menu/button_editor_def.png");
    position.Set_values(277,324,247,39);
    SDL_BlitSurface(button_editor, 0, background, &position);
    SDL_Surface* button_configuration = image_collection("menu/button_configuration_dis.png");
    position.Set_values(270,383,260,41);
    SDL_BlitSurface(button_configuration, 0, background, &position);
    SDL_Surface* button_exit = image_collection("menu/button_exit_def.png");
    position.Set_values(362,437,77,39);
    SDL_BlitSurface(button_exit, 0, background, &position);

    SDL_Surface* cursor_default = image_collection(CURSOR_DEFAULT);
    SDL_Surface* cursor = cursor_default;
    Rect2D cursor_rect(0,0,48,48);
    position.Set_values(0,0,0,0);

    // program main loop
    bool done = false;
    while (!done)
    {
        // message processing loop
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                done = true;
                break;
            case SDL_MOUSEMOTION:
                update_required = 1;
                cursor_rect.x = event.motion.x;
                cursor_rect.y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                pair<int,int> coord(event.button.x,event.button.y);
                func_p* boton = actions[coord];
                if(boton)
                {
                    boton->func(boton->data);
                    screen = SDL_SetVideoMode(
                        800, 600, 32,
                        SDL_HWSURFACE|SDL_DOUBLEBUF);
                    if ( !screen ) { printf("Unable to set video: %s\n", SDL_GetError()); return 1; }
                }
                //update_required = 1;
        } // end switch

        if(update_required)
        {
            // DRAWING STARTS HERE

            SDL_BlitSurface(background, 0, screen, &position);
            SDL_BlitSurface(cursor, 0, screen, &cursor_rect);

            // DRAWING ENDS HERE

            // finally, update the screen :)
            SDL_Flip(screen);
            update_required = 0;
        }
    } // end main loop

    SDL_Quit();
    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}
