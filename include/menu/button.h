#ifndef BUTTON_H
#define BUTTON_H

#define BUTTON_STATUS_DEFAULT   0
#define BUTTON_STATUS_OVER      1
#define BUTTON_STATUS_PRESSED   2
#define BUTTON_STATUS_DISABLED  3

#include <string>
//#include <vector>
#include "rect2d.h"
#include "defines.h"

#include <img_collection.h>
extern IMG_Collection image_collection;

using namespace std;

class Button
{
    public:
        Button();
        Button( char* some_name, int x, int y,
                char* image_default, char* image_pressed,
                char* image_over, char* image_disabled,
                int (*func)(void* data), void* some_data,
                int some_status = 0);
        Button( char* some_name, SDL_Rect & some_position,
                char* image_default, char* image_pressed,
                char* image_over, char* image_disabled,
                int (*func)(void* data), void* some_data,
                int some_status = 0);
        Button(Button & some);
        Button & operator=(Button & some);
        ~Button();

        inline void Set_Name(char* some_name) { name = some_name;}
        inline char* Get_Name() {return (char*)name.c_str();}
        inline void Set_Position(SDL_Rect & some_pos) { position = some_pos; position.w = i_default->w; position.h = i_default->h;}
        inline void Set_Position(int x, int y) { position.x = x; position.y = y; position.w = i_default->w; position.h = i_default->h;}
        inline SDL_Rect & Get_Position() {return position;}

        inline void Set_Image_Default  (char* image)        { i_default  = image_collection(image);}
        inline void Set_Image_Default  (SDL_Surface* image) { i_default  = image;}
        inline void Set_Image_Pressed  (char* image)        { i_pressed = image_collection(image);}
        inline void Set_Image_Pressed  (SDL_Surface* image) { i_pressed = image;}
        inline void Set_Image_Over     (char* image)        { i_over    = image_collection(image);}
        inline void Set_Image_Over     (SDL_Surface* image) { i_over    = image;}
        inline void Set_Image_Disabled (char* image)        { i_disabled  = image_collection(image);}
        inline void Set_Image_Disabled (SDL_Surface* image) { i_disabled = image;}
        inline SDL_Surface* Get_Image_Default()  { return i_default;}
        inline SDL_Surface* Get_Image_Pressed()  { return i_pressed;}
        inline SDL_Surface* Get_Image_Over()     { return i_over;}
        inline SDL_Surface* Get_Image_Disabled() { return i_disabled;}
        inline SDL_Surface* Get_Image_Actual()   { return i_actual;}
        inline void Set_Default(int value)  { i_actual = i_default; }
        inline void Set_Pressed(int value)  { i_actual = i_pressed; }
        inline void Set_Over(int value)     { i_actual = i_over; }
        inline void Set_Disabled(int value) { i_actual = i_disabled; }
        void Set_Status(int some_status);
        inline int Get_Status()  { return status;}
        inline void Enable() { status = 0; Set_Status(BUTTON_STATUS_DEFAULT); }
        inline void Disable() { Set_Status(BUTTON_STATUS_DISABLED); }

        inline void  Set_Click_Function(int (*func)(void* data)) {click = func;}
        inline void  Set_Click_Data(void* some_data) {click_data = some_data;}
        inline void* Get_Click_Data() { return click_data;}
        inline void  Click() {if(click) click(click_data);}

        void Draw(SDL_Surface* screen);

    private:
        string name;
        Rect2D position;
        SDL_Surface* i_default;
        SDL_Surface* i_pressed;
        SDL_Surface* i_over;
        SDL_Surface* i_disabled;
        SDL_Surface* i_actual;
        int (*click)(void* data);
        void* click_data;
        int status;
};

#endif // BUTTON_H
