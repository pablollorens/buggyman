#ifndef _GAME_MENUS_H_
#define _GAME_MENUS_H_

#include <cstdlib>

#include <windows.h>
#include <editor.h>
#include <button.h>
#include "game_config.h"

#include <menu.h>
#include <functions.h>
#include "grid3d.h"
#include <configuration.h>
#include <vector>
#include <map>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <game/Game.h>
#include <drawstuff/drawstuff.h>

#include <img_collection.h>
extern IMG_Collection image_collection;

extern SDL_Surface *screen;
extern Mix_Music *musica_juego;
extern Mix_Music *musica_menu;
extern Mix_Music *musica_editor;

int Run_Parametrized_Game(int, char**);
int Quit_Game(void* data);
int Run_Game(void* data);
int Select_Car(void* data);
int Run_Configuration(void* data);
int Run_Editor(void* data);
int Run_Main_Menu(void* data);



#endif // _GAME_MENUS_H_
