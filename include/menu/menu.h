#ifndef __MENU_H__
#define __MENU_H__

#include <cstdlib>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_framerate.h>
//#include <SDL_config.h>
#include <drawstuff/drawstuff.h>

#include <button.h>
#include <point3d.h>
#include <defines.h>
#include <list>
#include <map>

#define methods
#define variables

//Global variable of SDL_Surface* collecion
#include <img_collection.h>
extern IMG_Collection image_collection;


class Menu
{
    public:
        Menu(char* some_name = "none", SDL_Surface* some_screen = NULL);
        Menu(const Menu&);
        virtual Menu& operator=(const Menu&);
        virtual ~Menu();
        virtual int Run();
        virtual void Add_Button(Button & button, SDLKey key);
        virtual void Add_Button(Button & button);
        inline virtual void Set_Screen(SDL_Surface * surface) {screen = surface;}
        inline virtual void Set_Background(SDL_Surface * surface) {background = surface;}
        inline virtual void Set_Background(char* surface) {background = image_collection(surface);}
        inline virtual SDL_Surface * Get_Background() {return background;}
        inline bool* Get_Loop_Boolean() {return &loop_done;}

        virtual void Init_Menu();
        virtual void Set_Video_Mode_CFG(int x, int y, int bpp, Uint32 flags);
        virtual void Set_Video_Mode();
        virtual void Quit_Menu();

    protected methods:
        virtual bool Manage_Events(SDL_Event & event);
        virtual void Draw();

    protected :
        string name;
        SDL_Surface * background;
        SDL_Surface * screen;

        SDL_Surface * cursor;
        Rect2D cursor_rect;

        bool update_required;
        bool loop_done;
        list< Button* > button_list;
        Button * actual_button;
        map< pair<int,int> , Button* > buttons;
        map< int , Button* > keys;

        Point3D< Uint16 > video_mode;
        Uint32 video_mode_flags;
        bool video_mode_ready;
};

#endif // __MENU_H__
