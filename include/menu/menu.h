#ifndef __MENU_H__
#define __MENU_H__

#include <cstdlib>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_framerate.h>
//#include <SDL_config.h>
#include <drawstuff/drawstuff.h>

#include <button.h>
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
        inline virtual void Set_Screen(SDL_Surface * surface) {screen = surface;}
        inline virtual void Set_Background(SDL_Surface * surface) {background = surface;}
        inline virtual void Set_Background(char* surface) {background = image_collection(surface);}
        inline virtual SDL_Surface * Get_Background() {return background;}
        inline bool* Get_Loop_Boolean() {return &loop_done;}
        virtual void Init_Menu(int x, int y, int bpp, Uint32 flags);
        virtual void Quit_Menu();

    private methods:
        virtual bool Manage_Events(SDL_Event & event, Rect2D & cursor_rect, Button* actual_button);
        virtual void Draw();

    private:
        string name;
        SDL_Surface * background;
        SDL_Surface * screen;
        bool update_required;
        bool loop_done;
        list< Button* > button_list;
        map< pair<int,int> , Button* > buttons;
        map< int , Button* > keys;
};

#endif // __MENU_H__
