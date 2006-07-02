
#include <cstdlib>

#include <windows.h>
#include <editor.h>
#include <button.h>
#include"game_config.h"

#include <menu.h>
#include <functions.h>
#include <game_menus.h>
#include "grid3d.h"
#include <configuration.h>
#include <vector>
#include <map>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <game/Game.h>
#include <drawstuff/drawstuff.h>

#include <img_collection.h>
IMG_Collection image_collection;

SDL_Surface *screen;
Mix_Music *musica_juego;
Mix_Music *musica_menu;
Mix_Music *musica_editor;
using namespace std;

int main ( int argc, char** argv )
{
    dsPrint("### MAIN ###\n");

    Run_Parametrized_Game(argc, argv);
    Run_Main_Menu(NULL);

    dsPrint("### THE END ###\n");
    return 0;
}
