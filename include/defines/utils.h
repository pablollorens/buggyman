
#ifndef _DEFINES_UTILS_H_
#define _DEFINES_UTILS_H_



///Defines de Ground

//World size in cells
#define GRID_X 30
#define GRID_Y 30
#define GRID_Z 1

//#define X 30 // Tamaño X del terreno
//#define Y 30 // Tamaño Y del terreno
//#define Z 30

#define CELL_TAM 7
#define WALL_TAM 1
#define WALL_HEIGHT 2

#define WAIT                    0
#define RUN                     1
#define STOP                    2

#define MAX_SPEED 16
#define ODE_STEP_SENSIVILITY (float)1/20


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




#endif //_DEFINES_UTILS_H_
