#include"functions.h"

Blit_Task* New_Blit_Task(SDL_Surface * some_src, SDL_Rect &some_window, SDL_Surface * some_dst)
{
    Blit_Task* bt = new Blit_Task;
    bt->src = some_src;
    bt->window.x = some_window.x;
    bt->window.y = some_window.y;
    bt->window.w = some_window.w;
    bt->window.h = some_window.h;
    bt->dst = some_dst;
    return bt;
}

