#ifndef __RECT2D_H__
#define __RECT2D_H__

#include <SDL/SDL.h>
#include <iostream>
using namespace std;

class Rect2D : public SDL_Rect
{
    friend ostream & operator<<(ostream &, Rect2D &);
    public:
        Rect2D();
        Rect2D(const Rect2D& some_rect);
        Rect2D(const SDL_Rect& some_rect);
        Rect2D(int x, int y, unsigned int w=0, unsigned int h=0);
        Rect2D& operator=(const SDL_Rect& some_rect);
        Rect2D& operator=(SDL_Surface* some_surface);
        bool operator==(const SDL_Rect& rect);
        bool operator<(const SDL_Rect& rect);
        void Set_XY(int nx, int ny);
        void Set_values(int nx, int ny, Uint16 nw, Uint16 nh);
};

#endif // __RECT2D_H__
