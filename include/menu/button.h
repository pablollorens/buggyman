#ifndef BUTTON_H
#define BUTTON_H


#define NONE_BUTTON_POSITION 0
#define LEFT_BUTTON_POSITION 1
#define CENTER_BUTTON_POSITION 2
#define RIGHT_BUTTON_POSITION 3
#define TOP_BUTTON_POSITION 4
#define BOTTOM_BUTTON_POSITION 5

using namespace std;

#include <string>
#include <vector>
#include "rect2d.h"
#include "defines.h"

class Button
{
    public:
        Button();
        Button(string some_name, int some_x, int some_y, int some_w, int some_h, string some_image_normal, string some_image_pressed, string imageButton, int imageButton_position, string textButton, int textButton_position);
        Button(Button & some);
        ~Button();

        inline void Set_Name(string some_name) { name = some_name;}
        inline string Get_Name() {return name;}

        inline void Set_X(int some_x) { Calculate_Attributes(some_x, y, w, h);}
        inline int Get_X() {return x;}
        inline void Set_Y(int some_y) { Calculate_Attributes(x, some_y, w, h);}
        inline int Get_Y() {return y;}
        inline void Set_Width(int some_w) { Calculate_Attributes(x, y, some_w, h);}
        inline int Get_Width() {return w;}
        inline void Set_Height(int some_h) { Calculate_Attributes(x, y, w, some_h);}
        inline int Get_Height() {return h;}

        inline void Set_ImageButton_Position(int some_position) { imageButton_position = some_position;}
        inline void Set_TextButton_Position(int some_position) { textButton_position = some_position;}

        inline void Set_ImageNormal (string some_image) { image_normal = some_image;}
        inline void Set_ImagePressed (string some_image) { image_pressed = some_image;}
        inline void Set_ImageButton (string some_image) { imageButton = some_image;}
        inline void Set_TextButton (string some_text) {textButton = some_text;}

        bool Is_Over (int coord_x, int coord_y);
        void Load (SDL_Surface* screen);

    protected:
    private:
        string name;
        int x;
        int y;
        int w;
        int h;
        int posx_ini;
        int posy_ini;
        int posx_fin;
        int posy_fin;
        string image_normal;
        string image_pressed;
        string imageButton;
        int imageButton_position;
        string textButton;
        int textButton_position;

        //private methods
        void Calculate_Attributes(int some_x, int some_y, int some_w, int some_h);

};

#endif // BUTTON_H
