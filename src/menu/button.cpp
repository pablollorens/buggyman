#include "button.h"

Button::Button()
{
    name = "";
    position=0;
    i_default  = NULL;
    i_pressed  = NULL;
    i_over     = NULL;
    i_disabled = NULL;
    i_actual   = NULL;
    status = BUTTON_STATUS_DEFAULT;
    click = NULL;
    click_data = NULL;
}

Button::Button( char* some_name, SDL_Rect & some_position,
                char* image_default, char* image_pressed,
                char* image_over, char* image_disabled,
                int (*func)(void* data), void* some_data,
                int some_status)
{
    name = some_name;
    i_default  = image_collection(image_default);
    i_pressed  = image_collection(image_pressed);
    i_over     = image_collection(image_over);
    i_disabled = image_collection(image_disabled);
    i_actual = i_default;
    position.Set_values(some_position.x, some_position.y, i_default->w, i_default->h);
    Set_Status(some_status);
    click = func;
    click_data = some_data;
}

Button::Button( char* some_name, int x, int y,
                char* image_default, char* image_pressed,
                char* image_over, char* image_disabled,
                int (*func)(void* data), void* some_data,
                int some_status)
{
    name = some_name;
    i_default  = image_collection(image_default);
    i_pressed  = image_collection(image_pressed);
    i_over     = image_collection(image_over);
    i_disabled = image_collection(image_disabled);
    i_actual = i_default;
    position.Set_values(x, y, i_default->w, i_default->h);
    Set_Status(some_status);
    click = func;
    click_data = some_data;
}

Button::Button(Button & some)
{
    (*this)=some;
}

Button &
Button::operator=(Button & some)
{
	if(this == &some) return(*this);

    name = some.name;
    position = some.position;
    i_default  = some.i_default;
    i_pressed  = some.i_pressed;
    i_over     = some.i_over;
    i_disabled = some.i_disabled;
    Set_Status(some.status);
    click = some.click;
    click_data = some.click_data;
    return(*this);
}

Button::~Button()
{
    name = "";
    position = 0;
    i_default  = NULL;
    i_pressed  = NULL;
    i_over     = NULL;
    i_disabled = NULL;
    i_actual   = NULL;
    status = BUTTON_STATUS_DEFAULT;
    click = NULL;
    click_data = NULL;
}

void
Button::Set_Status(int some_status)
{
    if(status == BUTTON_STATUS_DISABLED) return;
    switch(some_status)
    {
        case BUTTON_STATUS_DEFAULT:
            i_actual =i_default;
            break;
        case BUTTON_STATUS_PRESSED:
            i_actual =i_pressed;
            break;
        case BUTTON_STATUS_OVER:
            i_actual =i_over;
            break;
        case BUTTON_STATUS_DISABLED:
            i_actual =i_disabled;
            break;
        default:
            return;
    };
    status = some_status;
}

void
Button::Draw(SDL_Surface* screen)
{
    SDL_BlitSurface(i_actual, 0, screen, &position);
}
