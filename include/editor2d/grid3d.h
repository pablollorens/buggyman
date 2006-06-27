#ifndef __GRID3D_H__
#define __GRID3D_H__

extern int world_frame;

//World size in cells
#define GRID_X 30
#define GRID_Y 30
#define GRID_Z 5

#include<iostream>
#include<fstream>
#include<SDL/SDL.h>
#include<string>
#include<vector>
#include <map>
#include"defines.h"
#include"track.h"
#include"grid3d.h"
#include"point3d.h"

#define GRID_Background "textures/land.jpg"
//Global variable of SDL_Surface* collecion
#include <img_collection.h>
extern IMG_Collection image_collection;
using namespace std;

class Grid3D
{
    friend ostream & operator<<(ostream &, Grid3D &);
    public:
        //Basic methods
        Grid3D();
        Grid3D( char* some_name, Uint16 dx, Uint16 dy, Uint16 dz, int wx, int wy, Uint16 ww, Uint16 wh);
        Grid3D( char* some_name, Point3D<int> & some_dim, Rect2D & some_window);
        Grid3D(Grid3D & some);
        ~Grid3D();
        Grid3D & operator=(Grid3D & some);

        //Comparation methods
        bool operator==(const Grid3D & some);
        bool operator!=(const Grid3D & some);

        //Name manipulation methods
        inline string Get_Name() {return name;}
        inline void Set_Name(string some_name) { name = some_name;}

        //Track Management methods
        bool   Set_Track(Uint16 x, Uint16 y, Uint16 z, Track& some_track);
        bool   Set_Track(Uint16 x, Uint16 y, Track& some_track);
        Track* Get_Track(Uint16 x, Uint16 y, Uint16 z);
        Track* Get_Track(Uint16 x, Uint16 y);
        bool Delete_Track(Uint16 x, Uint16 y, Uint16 z);
        bool Delete_Track(Uint16 x, Uint16 y);
        int Activate_Tracks_Under(Uint16 x,Uint16 y,Uint16 z, Track* some_track);
        int Activate_Tracks_Under(Uint16 x,Uint16 y, Track* some_track);
        void Deactivate_All_Tracks();
        void Clear_Cell_Sisters(Uint16 x,Uint16 y,Uint16 z, Track* track);

        //Window management
        inline SDL_Rect Get_Window() { return window; }
        void Set_Window(SDL_Rect& some_window);
        inline void Set_Window(int x, int y) {window.x = x; window.y = y;};
        void Set_Window(int x, int y, Uint16 w, Uint16 h);

        //Offset point management
        void Set_Offset(Point3D<int>& some_offset);
        inline void Set_Offset(int x, int y, int z) { Point3D<int> aux(x,y,z); Set_Offset(aux); }
        inline void Set_Offset_X(int x) { Point3D<int> aux(x,offset.y,offset.z); Set_Offset(aux); }
        inline void Set_Offset_Y(int y) { Point3D<int> aux(offset.x,y,offset.z); Set_Offset(aux); }
        inline void Set_Offset_Z(int z) { Point3D<int> aux(offset.x,offset.y,z); Set_Offset(aux); }
        inline void Incr_Offset_X() { Point3D<int> aux(1,0,0); Set_Offset(offset + aux);}
        inline void Incr_Offset_Y() { Point3D<int> aux(0,1,0); Set_Offset(offset + aux);}
        inline void Incr_Offset_Z() { Point3D<int> aux(0,0,1); Set_Offset(offset + aux);}
        inline void Incr_Offset(int x, int y, int z) { Point3D<int> aux(x,y,z); Set_Offset(offset + aux);}
        inline void Decr_Offset_X() { Point3D<int> aux(1,0,0); Set_Offset(offset - aux);}
        inline void Decr_Offset_Y() { Point3D<int> aux(0,1,0); Set_Offset(offset - aux);}
        inline void Decr_Offset_Z() { Point3D<int> aux(0,0,1); Set_Offset(offset - aux);}
        inline void Decr_Offset(int x, int y, int z) { Point3D<int> aux(x,y,z); Set_Offset(offset - aux);}
        inline Point3D<int> & Get_Offset() { return offset; }

        //Render 2D methods
        void Draw(SDL_Surface* screen);

        //Dimension access methods
        inline Point3D<int> & Get_Dims() { return dim; }
        inline int Get_DimX() { return (int)dim.x; }
        inline int Get_DimY() { return (int)dim.y; }
        inline int Get_DimZ() { return (int)dim.z; }

        //Circuit Load & Save methods
        int Save(char* path);
        int Load(char* path);
        void Debug_Print_Grid(char* fich, int sufix, char* ext, char* remmark);

    private:
        string name;

        Rect2D window;
        SDL_Surface* window_surface;
        SDL_Surface* window_background;
        bool window_changed;

        Point3D<int> dim;
        Point3D<int> offset;
        vector< vector< vector< Track* > > > grid;
        //map< Track* , Track* > tracks;
        //Track null_track;

        //Private methods
        Point3D<int>& Normalize_Offset(Point3D<int>& some_offset);
        Point3D<int>* Get_Top_Left_Syster(int x, int y);
        //void Clean_Sisters_and_yourself(vector< vector< vector< Cell3D > > >& grid,unsigned int x,unsigned int y,unsigned int z, Track* old_track, Track* new_track);
};

#endif // __GRID3D_H__
