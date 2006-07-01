#ifndef __EDITOR_H__
#define __EDITOR_H__

#if 0
    #define FULLSCREEN_FLAG SDL_FULLSCREEN
#else
    #define FULLSCREEN_FLAG 0
#endif

#define FPSrate 25


//Editor status: Adding track to circuit, moving track from circuit, etc
#define EDITOR_STATUS_NONE  0
#define EDITOR_STATUS_ADD   1
#define EDITOR_STATUS_MOVE  2

#include <cstdlib>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_framerate.h>
#include <SDL_config.h>

#include <windows.h>
#include <gl/gl.h>

#include <menu.h>
#include <grid3d.h>
#include <functions.h>
#include <paths.h>
#include <utils.h>
#include <list>

#define methods

//Global variable of SDL_Surface* collecion
#include <img_collection.h>
extern IMG_Collection image_collection;

class Editor : public Menu
{
    public:
        Editor(char* some_name = "none", SDL_Surface* some_screen = NULL):
            Menu(some_name,some_screen) {};
        Editor(const Editor& some) : Menu(some) {};
        ~Editor();
        int Run();

    private methods:
        //Manage_Events(SDL_Event & event);
        //void Draw();
        void Add_Tracks(Grid3D & grid);
        void Load_Tracks(Grid3D & grid);
    private:
        map<string, Track* > tracks_map;
};

#endif // __EDITOR_H__
