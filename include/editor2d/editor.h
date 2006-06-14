#ifndef __EDITOR_H__
#define __EDITOR_H__

#if 0
    #define FULLSCREEN_FLAG SDL_FULLSCREEN
#else
    #define FULLSCREEN_FLAG 0
#endif

#define FPSrate 25

#define CURSOR_DEFAULT "cursor/mouse1.gif"
#define CURSOR_HAND_OPEN "cursor/hand_open.png"
#define CURSOR_HAND_CLOSE "cursor/hand_close.png"

#define EDITOR_background "editor/background.jpg"
#define EDITOR_VOID_ICON "textures/track_void_icon.gif"
#define EDITOR_VOID_ICON3D "textures/track_void_icon3d.gif"

#define EDITOR_CIRCUIT_WINDOW_X 288
#define EDITOR_CIRCUIT_WINDOW_Y 64
#define EDITOR_CIRCUIT_WINDOW_W 448     //7 tracks
#define EDITOR_CIRCUIT_WINDOW_H 384    //6 tracks

#define EDITOR_TRACKS_WINDOW_X 32
#define EDITOR_TRACKS_WINDOW_Y 96
#define EDITOR_TRACKS_WINDOW_W 192  //3 tracks
#define EDITOR_TRACKS_WINDOW_H 320 //5 tracks

#define EDITOR_ICON3D_X 32
#define EDITOR_ICON3D_Y 460
#define EDITOR_ICON3D_W 96  //3 tracks
#define EDITOR_ICON3D_H 96 //5 tracks

//Editor status: Adding track to circuit, moving track from circuit, etc
#define EDITOR_STATUS_NONE  0
#define EDITOR_STATUS_ADD   1
#define EDITOR_STATUS_MOVE  2

#include <cstdlib>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_framerate.h>
#include <SDL_config.h>

#include <windows.h>
#include <gl/gl.h>

#include <grid3d.h>
#include <defines.h>
#include <functions.h>
#include <list>

#define methods

//Global variable of SDL_Surface* collecion
#include <img_collection.h>
extern IMG_Collection image_collection;


class Editor
{
    public:
        Editor();
        Editor(const Editor&);
        Editor& operator=(const Editor&);
        ~Editor();
        int Run(char* path_to_save_circuit=NULL);
    private methods:
        void Add_Tracks(Grid3D & grid);
    private:
};

#endif // __EDITOR_H__
