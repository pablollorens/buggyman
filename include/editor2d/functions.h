#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <SDL.h>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <list>

using namespace std;

typedef struct
{
    SDL_Surface * src;
    SDL_Rect window;
    SDL_Surface * dst;
} Blit_Task;

list<string> Get_Directories(char*);

//Blit_Task* New_Blit_Task(SDL_Surface * some_src, SDL_Rect &some_window, SDL_Surface * some_dst);

#endif //__FUNCTIONS_H__

