#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <cstdlib>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_framerate.h>
#include <drawstuff/drawstuff.h>
#include "game_config.h"

#include <button.h>
#include <point3d.h>
#include <menu.h>
#include <list>
#include <vector>
#include <map>

#define methods
#define variables

//Global variable of SDL_Surface* collecion
#include <img_collection.h>
extern IMG_Collection image_collection;

typedef struct sub_Config
{
    SDL_Surface* def;
    SDL_Surface* press;
    int x;
    int y;

    bool fullscreen;
    bool sky;
    int scenery;
    int draw_ratio;
};


class Configuration : public Menu
{
    public:
        Configuration(char* some_name = "none", SDL_Surface* some_screen = NULL)
            : Menu(some_name,some_screen) {};
        Configuration(const Configuration& some) : Menu(some) {};
        ~Configuration();
        int Run();

    private methods:
        virtual bool Manage_Events(SDL_Event & event);
        virtual void Draw();
        static int Quit(void* data);

    private:
    vector< sub_Config* > resolutions;
    int resolutions_index;
    Rect2D resolutions_rect;

    vector< sub_Config* > fullscreen;
    int fullscreen_index;
    Rect2D fullscreen_rect;

    vector< sub_Config* > sky;
    int sky_index;
    Rect2D sky_rect;

    vector< sub_Config* > scenery;
    int scenery_index;
    Rect2D scenery_rect;

    vector< sub_Config* > draw_ratios;
    int draw_ratios_index;
    Rect2D draw_ratios_rect;
};

#endif // __CONFIGURATION_H__
