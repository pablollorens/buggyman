#include "../include/button.h"

Button::Button()
{
    name = "";
    x = 0;
    y = 0;
    w = 0;
    h = 0;
    Calculate_Attributes(x, y, w, h);
    image_normal = "";
    image_pressed = "";
    imageButton = "";
    imageButton_position = NONE_BUTTON_POSITION;
    textButton = "";
    textButton_position = NONE_BUTTON_POSITION;
}

Button::Button(string some_name, int some_x, int some_y, int some_w, int some_h, string some_image_normal, string some_image_pressed, string some_imageButton, int some_imageButton_position, string some_textButton, int some_textButton_position)
{
    name = some_name;
    x = some_x;
    y = some_y;
    w = some_w;
    h = some_h;
    image_normal = some_image_normal;
    image_pressed = some_image_pressed;
    Calculate_Attributes(some_x, some_y, some_w, some_y);
    imageButton = some_imageButton;
    imageButton_position = some_imageButton_position;
    textButton = some_textButton;
    textButton_position = some_textButton_position;
}

Button::~Button()
{
    name = "";
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    Calculate_Attributes(x, y, w, h);
    image_normal = "";
    image_pressed = "";
    string imageButton = "";
    int imageButton_position = NONE_BUTTON_POSITION;
    string textButton = "";
    int textButton_position = NONE_BUTTON_POSITION;
}

void
Button::Calculate_Attributes (int some_x, int some_y, int some_w, int some_h)
{
    x = some_x;
    y = some_y;
    w = some_w;
    h = some_h;
    posx_ini = x;
    posy_ini = y;
    posx_fin = x + w;
    posy_fin = y + h;
}

bool
Button::Is_Over (int coord_x, int coord_y)
{
    if (coord_x >= posx_ini && coord_x <= posx_fin)
        if (coord_y >= posy_ini && coord_y <= posy_fin)
            return true;
    return false;
}

void
Button::Load(SDL_Surface* screen)
{
    SDL_Surface* button = SDL_LoadBMP("editor/boton_normal.bmp");
    SDL_Rect dstrect_button;
    dstrect_button.x = x;
    dstrect_button.y = y;
    SDL_BlitSurface(button, 0, screen, &dstrect_button);

}
