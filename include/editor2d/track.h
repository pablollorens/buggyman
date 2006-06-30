#ifndef __TRACK_H__
#define __TRACK_H__

#define TRACK_icon_void "textures/track_void_icon.gif"
#define TRACK_icon3d_void "textures/track_void_icon3d.gif"
#define TRACK_icon3d_background "textures/track_icon3d_background.png"
#define TRACK_icon_selected "textures/cell_yellow.png"


#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_rotozoom.h>
#include <SDL_config.h>

#include <unistd.h>
#include <string>
#include <vector>
#include"cell3d.h"
#include"rect2d.h"
#include"point3d.h"


//Global variable of SDL_Surface* collecion
#include <img_collection.h>
extern IMG_Collection image_collection;


#define methods
using namespace std;

class Track
{
	//friend ostream & operator<<(ostream &, Track &);
	//friend ifstream & operator>>(ifstream &, Track &);

	public:
        Track(
            char* some_name = "none",
            int DimX = 1, int DimY = 1, int DimZ = 1,
            char* some_description = "none");
        Track(
            char* some_name,
            SDL_Surface* some_icon,
            SDL_Surface* some_icon3d,
            int DimX = 1, int DimY = 1, int DimZ = 1,
            char* some_description = "none");
        Track(
            char* some_name,
            char* some_icon,
            char* some_icon3d,
            int DimX = 1, int DimY = 1, int DimZ = 1,
            char* some_description = "none");
        Track(string& file);
        Track(const Track & some);
        ~Track();
        Track & operator=(const Track & some);
        bool operator==(const Track & some);
        bool operator!=(const Track & some);

        inline string Get_Name() {return name;}
        inline void Set_Name(string& some_name) {name = some_name;}
        inline string Get_Description() {return description;}
        inline void Set_Description(string& some_description) {description = some_description;}
        inline int  Get_Rotation() {return rotation;}
        void Rotate_Counter_Clockwise();

        inline SDL_Surface* Get_Icon() {return icon;}
        void Set_Icon(SDL_Surface* some_icon);
        inline void Set_Icon(char* some_icon) { Set_Icon(image_collection(some_icon)); }

        inline SDL_Surface* Get_Icon3D() {return icon3D;}
        void Set_Icon3D(SDL_Surface* some_icon3D);
        inline void Set_Icon3D(char* some_icon3D) { Set_Icon3D(image_collection(some_icon3D));}

        //esto habra q cambiarlo por el operator()
        Cell3D* Get_Cell(unsigned int x, unsigned int y, unsigned int z);
        void Set_Cell(unsigned int x, unsigned int y, unsigned int z, Cell3D* one_cell);
        Point3D<int>* Get_First_Cell();

        void Draw(SDL_Surface* screen, SDL_Rect & some_window, int angle, bool set_window = 0, bool set_rotation = 0);
        void Draw(SDL_Surface* screen, SDL_Rect & some_window);
        void Draw(SDL_Surface* screen);
        void Draw_Icon3D(SDL_Surface* screen, int x, int y);
        void Draw_Icon3D(SDL_Surface* screen, SDL_Rect some_window);

        inline SDL_Rect& Get_Window() { return window; }
        inline void Set_Window(SDL_Rect& some_window) {window = some_window;}
        inline void Set_Window(int x, int y, int w, int h) {Rect2D win(x,y,w,h); window=win;}
        inline void Set_Window(int x, int y) {window.x=x; window.y=y;}

        inline int Get_DimX() { return dimx; }
        inline int Get_DimY() { return dimy; }
        inline int Get_DimZ() { return dimz; }

        inline bool Get_Selected() { return active_trans;}
        inline void Select() { active_trans = true;}
        inline void UnSelect() { active_trans = false;}
        inline void ToggleSelect() { active_trans ^= 1;}

        inline void Set_Start(bool value) {start = value;}
        inline bool Get_Start() {return start;}
        inline void Set_I_CheckPoint(bool value) {i_checkpoint = value;}
        inline bool Get_I_CheckPoint() {return i_checkpoint;}

        void Clear();
        bool Is_Clear();

	private methods:
        int Normalize_Angle(unsigned int angle);

	private:
        string name;
        unsigned int dimx;
        unsigned int dimz;
        unsigned int dimy;
        vector< vector< vector< Cell3D > > > cells;
        unsigned int num_connectors;
        SDL_Surface* icon;
        SDL_Surface* selected;
        bool active_trans;
        SDL_Surface* icon3D;
        int rotation;
        Rect2D window;
        string description;
        bool start;
        bool i_checkpoint;
};

#endif /* __TRACK_H__ */
