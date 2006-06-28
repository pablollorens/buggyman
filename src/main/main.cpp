
#include <cstdlib>
#include <windows.h>
#include <editor.h>
#include <button.h>
#include <menu.h>
#include <vector>
#include <map>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <game/Game.h>
#include <drawstuff/drawstuff.h>

#include <img_collection.h>

IMG_Collection image_collection;

using namespace std;

SDL_Surface *screen;

void Insert_into_map(map< pair<int,int> , Button* > & buttons, Button & button)
{
    for(int i = button.Get_Position().x, n = button.Get_Position().x + button.Get_Position().w; i<n; i++)
        for(int j = button.Get_Position().y, m = button.Get_Position().y + button.Get_Position().h; j<m; j++)
        {
            pair<int,int> aux(i,j);
            buttons[aux] = &button;
        }
}

void Init_Menu()
{
    if(SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        dsError( "Unable to init SDL: %s\n", SDL_GetError() );
        exit( 100 );
    }

    screen = SDL_SetVideoMode( 800, 600, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen )
    {
        dsError("Unable to set video: %s\n", SDL_GetError());
        exit( 101 );
    }
    SDL_ShowCursor(SDL_DISABLE);
}

void Quit_Menu()
{
    SDL_Quit();
}

int Run_Game(void* data)
{
    dsPrint("\t### RUN GAME ###\n");

    Game::SetResolution(((int*)data)[0],((int*)data)[1]);
    Game::SetFullScreen(false);
    Game::Run();

    dsPrint("\t### END GAME ###\n");

    return 0;
}

int Run_Editor(void* data)
{
    dsPrint("\t### RUN EDITOR ###\n");

    Editor ed;
    ed.Run(screen, "terreno.cfg");

    dsPrint("\t### END EDITOR ###\n");

    return 0;
}

int Quit_Game(void* data)
{
    *((bool*)data) = true;
    return 0;
}








int Prueba_Menu(void* data)
{
    Menu un_menu("pepe",screen);
    un_menu.Set_Background("menu/prueba.jpg");

    Button quit_game("Quit Game",362,437,
                    "menu/button_exit_def.png","menu/button_exit_press.png",
                    "menu/button_exit_over.png","menu/button_exit_dis.png",
                    Quit_Game,(bool*)un_menu.Get_Loop_Boolean());
    un_menu.Add_Button(quit_game, SDLK_p);
    return un_menu.Run();
}









int main ( int argc, char** argv )
{
    dsPrint("### MAIN ###\n");

    bool update_required = 1;
    bool done = false;

    Init_Menu();

    Rect2D position;
    map< pair<int,int> , Button* > buttons;

    Button playlow( "Play LowRes",242,208,
                    "menu/button_play320_def.png","menu/button_play320_press.png",
                    "menu/button_play320_over.png","menu/button_play320_dis.png",
                    Run_Game,(int*)new int(2));
    ((int*)playlow.Get_Click_Data())[0]=320;
    ((int*)playlow.Get_Click_Data())[1]=240;
    Insert_into_map(buttons,playlow);

    Button playhigh("Play HighRes",241,268,
                    "menu/button_play640_def.png","menu/button_play640_press.png",
                    "menu/button_play640_over.png","menu/button_play640_dis.png",
                    Run_Game,(int*)new int(2));
    ((int*)playhigh.Get_Click_Data())[0]=640;
    ((int*)playhigh.Get_Click_Data())[1]=480;
    Insert_into_map(buttons,playhigh);

    Button runeditor("Run Editor",277,324,
                    "menu/button_editor_def.png","menu/button_editor_press.png",
                    "menu/button_editor_over.png","menu/button_editor_dis.png",
                    Run_Editor,NULL);
    Insert_into_map(buttons,runeditor);

    Button configuration("Configuration",270,383,
                    "menu/button_configuration_def.png","menu/button_configuration_press.png",
                    "menu/button_configuration_over.png","menu/button_configuration_dis.png",
                    NULL,NULL,BUTTON_STATUS_DISABLED);
    Insert_into_map(buttons,configuration);

    Button quit_game("Quit Game",362,437,
                    "menu/button_exit_def.png","menu/button_exit_press.png",
                    "menu/button_exit_over.png","menu/button_exit_dis.png",
                    Quit_Game,(bool*)&done);
    Insert_into_map(buttons,quit_game);

    Button prueba("PRUEBA",0,0,
                    "menu/button_exit_def.png","menu/button_exit_press.png",
                    "menu/button_exit_over.png","menu/button_exit_dis.png",
                    Prueba_Menu,NULL);
    Insert_into_map(buttons,prueba);

    SDL_Surface* background = image_collection("menu/backg_main.png");

    SDL_Surface* cursor_default = image_collection(CURSOR_DEFAULT);
    SDL_Surface* cursor = cursor_default;
    Rect2D cursor_rect(0,0,48,48);
    position = 0;
    Button* actual_button =NULL;

    /// program main loop

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
            {
                update_required = 1;
                cursor_rect.x = event.motion.x;
                cursor_rect.y = event.motion.y;
                pair<int,int> coord(event.motion.x,event.motion.y);
                Button* boton = buttons[coord];
                if(boton)
                {
                    (*boton).Set_Status(BUTTON_STATUS_OVER);
                    if(actual_button)
                    {
                        if(actual_button != boton)
                        {
                            (*actual_button).Set_Status(BUTTON_STATUS_DEFAULT);
                            actual_button = boton;
                        }
                    }
                    else actual_button = boton;
                }
                else if(actual_button)
                {
                    (*actual_button).Set_Status(BUTTON_STATUS_DEFAULT);
                    actual_button = NULL;
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                pair<int,int> coord(event.button.x,event.button.y);
                Button* boton = buttons[coord];
                if(boton)
                {
                    (*boton).Set_Status(BUTTON_STATUS_PRESSED);
                }
                update_required = 1;
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                pair<int,int> coord(event.button.x,event.button.y);
                Button* boton = buttons[coord];
                if(boton)
                {
                    //Quit_Menu();
                    (*boton).Set_Status(BUTTON_STATUS_OVER);
                    (*boton).Click();
                    Init_Menu();
                }
                update_required = 1;
                break;
            }
        } // end switch

        if(update_required)
        {
            // DRAWING STARTS HERE

            SDL_BlitSurface(background, 0, screen, &position);
            playlow.Draw(screen);
            playhigh.Draw(screen);
            runeditor.Draw(screen);
            configuration.Draw(screen);
            quit_game.Draw(screen);
            prueba.Draw(screen);
            SDL_BlitSurface(cursor, 0, screen, &cursor_rect);

            // DRAWING ENDS HERE

            // finally, update the screen :)
            SDL_Flip(screen);
            update_required = 0;
        }
    } // end main loop

    SDL_Quit();
    // all is well ;)
    dsPrint("### THE END ###\n");
    return 0;
}
