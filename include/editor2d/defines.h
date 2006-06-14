#ifndef __DEFINES_H__
#define __DEFINES_H__

//#include<SDL/SDL.h>

//Diretories
#define DIR_modbase "baseJRV"
#define DIR_Textures "textures"
#define DIR_Tracks "tracks"
#define DIR_players "players"

//file names
#define FILE_track_model "model.3ds"
#define FILE_track_model_config "model.cfg"
#define FILE_Skin_ext "skin"
//#define DIR_ ""


//Video Modes
#if 0
    #define VIDEO_width     1024
    #define VIDEO_height    768
#endif
#if 1
    #define VIDEO_width     800
    #define VIDEO_height    600
#endif
#if 0
    #define VIDEO_width     640
    #define VIDEO_height    480
#endif

#define VIDEO_depth     32

//#if SDL_BYTEORDER == SDL_BIG_ENDIAN
//    Uint32 Rmask = 0xFF000000;
//    Uint32 Gmask = 0x00FF0000;
//    Uint32 Bmask = 0x0000FF00;
//    Uint32 Amask = 0x000000FF;
//#else
//    Uint32 Rmask = 0x000000FF;
//    Uint32 Gmask = 0x0000FF00;
//    Uint32 Bmask = 0x00FF0000;
//    Uint32 Amask = 0xFF000000;
//#endif


#endif // __DEFINES_H__
