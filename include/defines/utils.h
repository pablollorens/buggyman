
#ifndef _DEFINES_UTILS_H_
#define _DEFINES_UTILS_H_

///Defines de Ground

//World and tools size in cells
#define GRID_X 30
#define GRID_Y 30
#define GRID_Z 2
#define TOOLS_X 4
#define TOOLS_Y 16
#define TOOLS_Z 2

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


// //////////////////////////////////////////////
// Editor Defines
// //////////////////////////////////////////////
#define CURSOR_DEFAULT "cursor/mouse1.gif"
#define CURSOR_HAND_OPEN "cursor/hand_open.png"
#define CURSOR_HAND_CLOSE "cursor/hand_close.png"

#define TRACK_icon_void "textures/track_icon_void.gif"
#define TRACK_icon3d_void "textures/track_icon3d_void.gif"
#define TRACK_icon3d_background "textures/track_icon3d_background.png"
#define TRACK_icon_selected "textures/cell_yellow.png"

#define EDITOR_background "menu/backg_editor.jpg"
#define GRID_world_backg "textures/land1440.jpg"
#define GRID_tools_backg "textures/asphalt768.jpg"
#define GRID_minimap_view "textures/minimap_view.png"


// Editor Appearence
#define EDITOR_CIRCUIT_WINDOW_X 57
#define EDITOR_CIRCUIT_WINDOW_Y 60
#define EDITOR_CIRCUIT_WINDOW_W 528
#define EDITOR_CIRCUIT_WINDOW_H 528

#define EDITOR_CIRCUIT_MINIMAP_WINDOW_X 601
#define EDITOR_CIRCUIT_MINIMAP_WINDOW_Y 408
#define EDITOR_CIRCUIT_MINIMAP_WINDOW_W 180
#define EDITOR_CIRCUIT_MINIMAP_WINDOW_H 180

#define EDITOR_TRACKS_WINDOW_X 595
#define EDITOR_TRACKS_WINDOW_Y 12
#define EDITOR_TRACKS_WINDOW_W 192
#define EDITOR_TRACKS_WINDOW_H 288

#define EDITOR_ICON3D_X 643
#define EDITOR_ICON3D_Y 306
#define EDITOR_ICON3D_W 96
#define EDITOR_ICON3D_H 96


#endif //_DEFINES_UTILS_H_
