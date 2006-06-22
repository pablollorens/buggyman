
#include <cstdlib>
#include <windows.h>
#include <editor.h>
#include <vector>
#include <SDL/SDL.h>
#include <game/Game.h>

using namespace std;
IMG_Collection image_collection;

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

int main ( int argc, char** argv )
{
    bool update_required = 1;

    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }
    //atexit(SDL_Quit);

    // create a new window
    SDL_Surface* screen = SDL_SetVideoMode(
        800, 600, 32,
        SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen ) { printf("Unable to set video: %s\n", SDL_GetError()); return 1; }
    SDL_ShowCursor(SDL_DISABLE);

    // load an image
    SDL_Surface* background = image_collection("menu/menu_background.jpg");
    SDL_Surface* cursor_default = image_collection(CURSOR_DEFAULT);
    SDL_Surface* cursor = cursor_default;
    Rect2D cursor_rect(0,0,48,48);
    Rect2D position;

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
                if( event.button.x >= MENU_NEW_GAME_X &&
                    event.button.x < MENU_NEW_GAME_X + MENU_NEW_GAME_W &&
                    event.button.y >= MENU_NEW_GAME_Y &&
                    event.button.y < MENU_NEW_GAME_Y + MENU_NEW_GAME_H)
                {
                    SDL_Quit();
                    Game::SetResolution(640,480);
                    Game::SetFullScreen(false);
                    Game::Run();

                    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
                    {
                        printf( "Unable to init SDL: %s\n", SDL_GetError() );
                        return 1;
                    }
                    // recreate a new window
                    screen = SDL_SetVideoMode(
                        800, 600, 32,
                        SDL_HWSURFACE|SDL_DOUBLEBUF);
                    if ( !screen ) { printf("Unable to set video: %s\n", SDL_GetError()); return 1; }
                    update_required = 1;
                }
                else
                if( event.button.x >= MENU_TRACK_EDITOR_X &&
                    event.button.x < MENU_TRACK_EDITOR_X + MENU_TRACK_EDITOR_W &&
                    event.button.y >= MENU_TRACK_EDITOR_Y &&
                    event.button.y < MENU_TRACK_EDITOR_Y + MENU_TRACK_EDITOR_H)
                {
//                    SDL_WM_IconifyWindow();
//                    system("BuggyTracks.exe");

                    SDL_Quit();
                    Editor ed;
                    ed.Run("terreno.cfg");

                    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
                    {
                        printf( "Unable to init SDL: %s\n", SDL_GetError() );
                        return 1;
                    }
                    // create a new window
                    SDL_Surface* screen = SDL_SetVideoMode(
                        800, 600, 32,
                        SDL_HWSURFACE|SDL_DOUBLEBUF);
                    if ( !screen ) { printf("Unable to set video: %s\n", SDL_GetError()); return 1; }
                    update_required = 1;
                }
                else
                if( event.button.x >= MENU_EXIT_X &&
                    event.button.x < MENU_EXIT_X + MENU_EXIT_W &&
                    event.button.y >= MENU_EXIT_Y &&
                    event.button.y < MENU_EXIT_Y + MENU_EXIT_H)
                {
                    done = 1;
                    update_required = 0;
                }
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
