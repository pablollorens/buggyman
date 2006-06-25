#include <rect2d.h>

Rect2D::Rect2D()
{
    x = y = w = h = 0;
}

Rect2D::Rect2D(const Rect2D& some_rect)
{
    x = some_rect.x; y = some_rect.y;
    w = some_rect.w; h = some_rect.h;
}

Rect2D::Rect2D(const SDL_Rect& some_rect)
{
    x = some_rect.x; y = some_rect.y;
    w = some_rect.w; h = some_rect.h;
}

Rect2D::Rect2D(int nx, int ny, unsigned int nw, unsigned int nh)
{
    x = nx; y = ny;
    w = nw; h = nh;
}

Rect2D&
Rect2D::operator=(const SDL_Rect& some_rect)
{
    if(this == &some_rect) return *this;
    x = some_rect.x; y = some_rect.y;
    w = some_rect.w; h = some_rect.h;
    return *this;
}

Rect2D&
Rect2D::operator=(SDL_Surface* some_surface)
{
    if(!some_surface) return *this;
    w = some_surface->w; h = some_surface->h;
    return *this;
}

Rect2D&
Rect2D::operator=(int value)
{
    x = y = w = h = 0;
    return *this;
}

Rect2D&
Rect2D::operator+(int value)
{
    Rect2D* aux = new Rect2D(x+value,y+value,w,h);
    return *aux;
}

Rect2D&
Rect2D::operator-(int value)
{
    Rect2D* aux = new Rect2D(x-value,y-value,w,h);
    return *aux;
}

bool
Rect2D::operator==(const SDL_Rect& some_rect)
{
    return ((x == some_rect.x) &&
            (y == some_rect.y) &&
            (w == some_rect.w) &&
            (h == some_rect.h));
}

bool
Rect2D::operator<(const SDL_Rect& some_rect)
{
    return ((x < some_rect.x) && (y < some_rect.y));
}

void
Rect2D::Set_XY(int nx, int ny)
{
    x = nx;
    y = ny;
}

void
Rect2D::Set_values(int nx, int ny, Uint16 nw, Uint16 nh)
{
    x = nx;
    y = ny;
    w = nw;
    h = nh;
}

ostream &
operator<<(ostream & o, Rect2D & some)
{
    o<<"<"<<some.x<<","<<some.y<<">["<<some.w<<","<<some.h<<"]";
    return o;
}

