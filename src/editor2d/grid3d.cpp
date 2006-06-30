#include "grid3d.h"
using namespace std;

Grid3D::Grid3D( char* some_name,
                Uint16 dx, Uint16 dy, Uint16 dz,
                int wx, int wy, Uint16 ww, Uint16 wh)
{
    name = some_name;

    dim.Set_Comparation_Mode(COMPARATION_OR);
    dim.Set_Values(dx,dy,dz);
    dim = dim.Get_Lower_Limited_to(1);

    Track* aux = NULL;
    vector< Track* > vz(dim.z,aux);
    vector< vector< Track* > > vy(dim.y,vz);
    grid.resize(dim.x,vy);
    for(int i = 0, n = dim.x; i<n; i++)
        for(int j = 0, m = dim.y; j<m; j++)
            for(int k = 0, l = dim.z; k<l; k++)
                grid[i][j][k] = new Track;

    window_surface = NULL;
    window_background = NULL;
    offset = 0;
    Set_Window(wx, wy, ww, wh);

}

Grid3D::Grid3D( char* some_name, Point3D<int> & some_dim, Rect2D & some_window)
{
    name = some_name;

    dim = some_dim.Get_Lower_Limited_to(1);

    Track* aux = NULL;
    vector< Track* > vz(dim.z,aux);
    vector< vector< Track* > > vy(dim.y,vz);
    grid.resize(dim.x,vy);
    for(int i = 0, n = dim.x; i<n; i++)
        for(int j = 0, m = dim.y; j<m; j++)
            for(int k = 0, l = dim.z; k<l; k++)
                grid[i][j][k] = new Track;

    window_surface = NULL;
    window_background = NULL;
    offset=0;
    Set_Window(some_window);
}

Grid3D::Grid3D(Grid3D & some)
{
    window_background = NULL;
    window_surface = NULL;
    (*this)=some;
}

Grid3D::~Grid3D()
{
    dim = 0;
    if(window_surface)
    {
        SDL_FreeSurface(window_surface);
        window_surface = NULL;
        window_background = NULL;
    }
}

Grid3D &
Grid3D::operator=(Grid3D & some)
{
    if(this == &some) return(*this);

    (*this).~Grid3D();
    name = some.name;

    dim = some.dim;
    grid = some.grid;
    //tracks = some.tracks;

    window = some.window;
    window_surface = zoomSurface(some.window_surface,1,1,0);
    window_background = zoomSurface(some.window_background,1,1,0);
    offset = some.offset;
    window_changed = true;

    return(*this);
}

bool
Grid3D::operator==(const Grid3D & some)
{
    return(name == some.name && dim == some.dim);
}

bool
Grid3D::operator!=(const Grid3D & some)
{
    return(name != some.name || dim != some.dim);
}











// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
//                                                                          //
//                                  DRAW                                    //
//                                                                          //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
void
Grid3D::Draw(SDL_Surface* screen)
{
    if(screen == NULL) return;
    //if(window_surface == NULL) return;
    if(dim<1) return;

    //cout<< *this<<endl;
//    if(!window_changed)
//    {
//        SDL_BlitSurface(window_surface, 0, screen, &window);
//        window_changed = 0;
//        return;
//    }
//    SDL_BlitSurface(window_background, 0, screen, &window);
    SDL_SetClipRect(screen, &window);

    //limit and offset in cells
    Point3D<int> limit( window.w / CELL_X +1, window.h / CELL_Y +1,1);
    Point3D<int> cell_offset( offset.x / CELL_X, offset.y / CELL_Y,offset.z);

    map< Track* , Track* > painted;
    Track *auxt=NULL;
    Point3D<int> top_left;

    for(int i=0; i<=limit.x && i+cell_offset.x<dim.x; i++)
        for(int j=0; j<=limit.y && j+cell_offset.y<dim.y; j++)
        {
            auxt = grid[i+cell_offset.x][j+cell_offset.y][cell_offset.z];
            if(!painted[auxt])
            {
                top_left = *Get_Top_Left_Syster(i+cell_offset.x, j+cell_offset.y);
                (*auxt).Set_Window(
                                top_left.x * CELL_X + window.x - offset.x,
                                top_left.y * CELL_Y + window.y - offset.y);
                (*auxt).Draw(screen);//window_surface);
                painted[auxt]=auxt;
            }
        }
    //SDL_BlitSurface(window_surface, 0, screen, &window);
    SDL_SetClipRect(screen, NULL);
    window_changed = 0;
}












// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
//                                                                          //
//                     Set/Get, Delete & Activate TRACK                     //
//                                                                          //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
Track*
Grid3D::Get_Track(Uint16 x, Uint16 y, Uint16 z)
{
    if(dim<1) return NULL;

    Point3D<int> p(x,y,z);
    p = p.Get_Limited_to(0,0,0,dim.x-1,dim.y-1,dim.z-1);

    Track* track= grid[p.x][p.y][p.z];
    track = new Track(*track);
    return track;
}

Track*
Grid3D::Get_Track(Uint16 x, Uint16 y)
{
    if(dim<1) return NULL;
    if(x < window.x || y < window.y) return NULL;
    if(x >= window.x + window.w || y >= window.y + window.h) return NULL;

    int i = (int) (x-window.x + offset.x)/CELL_X;
    int j = (int) (y-window.y + offset.y)/CELL_Y;

    return (Get_Track(i,j,offset.z));
}

bool
Grid3D::Set_Track(Uint16 x, Uint16 y, Uint16 z, Track& some_track)
{
    if(dim<1) return false;
    if(!(&some_track)) return false;

    Point3D<int> p(x,y,z);
    p = p.Get_Limited_to(0,0,0,dim.x-1,dim.y-1,dim.z-1);

    int nx=some_track.Get_DimX();
    int ny=some_track.Get_DimY();
    int nz=some_track.Get_DimZ();
    if(p.x + nx -1 >= dim.x) return false;
    if(p.y + ny -1 >= dim.y) return false;
    if(p.z + nz -1 >= dim.z) return false;

    //new copy of received track
    Track* newtrack = new Track(some_track);
//    Track* voidtrack = new Track();
    //tracks[newtrack]=newtrack;


    for(int i=0; i<nx; i++)
        for(int j=0; j<ny; j++)
            for(int k=0; k<nz; k++)
            {
                Track* aux = grid[p.x+i][p.y+j][p.z+k];
                if((*aux).Get_Name() != "none")
                {
                    Clear_Cell_Sisters(p.x+i,p.y+j,p.z+k,aux);
                    delete aux;
                }
                grid[p.x+i][p.y+j][p.z+k] = newtrack;
            }

    //esto hay que cambiarlo:
    //para que solo actualize la zona afectada y no toda la surface
    window_changed = true;
    return true;
}

bool
Grid3D::Set_Track(Uint16 x, Uint16 y, Track& some_track)
{
    if(dim<1) return false;
    if(x < window.x || y < window.y) return false;
    if(x >= window.x + window.w || y >= window.y + window.h) return false;

    int i = (int) (x-window.x + offset.x)/CELL_X;
    int j = (int) (y-window.y + offset.y)/CELL_Y;

    return(Set_Track(i,j,offset.z,some_track));
}

bool
Grid3D::Add_Track(Track * some_track)
{
    return 0;
    //lo comentado aki es copia de Set_Track
    //devuelve cierto si se ha insertado la track

//    if(dim<1) return false;
//    if(!some_track) return false;
//
//    Point3D<int> p(x,y,z);
//    p = p.Get_Limited_to(0,0,0,dim.x-1,dim.y-1,dim.z-1);
//
//    int nx=some_track.Get_DimX();
//    int ny=some_track.Get_DimY();
//    int nz=some_track.Get_DimZ();
//    if(p.x + nx -1 >= dim.x) return false;
//    if(p.y + ny -1 >= dim.y) return false;
//    if(p.z + nz -1 >= dim.z) return false;
//
//    //new copy of received track
//    Track* newtrack = new Track(some_track);
////    Track* voidtrack = new Track();
//    //tracks[newtrack]=newtrack;
//
//
//    for(int i=0; i<nx; i++)
//        for(int j=0; j<ny; j++)
//            for(int k=0; k<nz; k++)
//            {
//                Track* aux = grid[p.x+i][p.y+j][p.z+k];
//                if((*aux).Get_Name() != "none")
//                {
//                    Clear_Cell_Sisters(p.x+i,p.y+j,p.z+k,aux);
//                    delete aux;
//                }
//                grid[p.x+i][p.y+j][p.z+k] = newtrack;
//            }
//
//    //esto hay que cambiarlo:
//    //para que solo actualize la zona afectada y no toda la surface
//    window_changed = true;
//    return true;
}

bool
Grid3D::Delete_Track(Uint16 x, Uint16 y, Uint16 z)
{
    if(dim < 1) return false;

    Point3D<int> p(x,y,z);
    p = p.Get_Limited_to(0,0,0,dim.x-1,dim.y-1,dim.z-1);

    Track* aux = grid[p.x][p.y][p.z];
    //Track *voidtrack = new Track;
    if((*aux).Get_Name() != "none")
    {
        Clear_Cell_Sisters(p.x,p.y,p.z,aux);
        delete aux;
        window_changed = true;
        return true;
    }

    return false;
}

bool
Grid3D::Delete_Track(Uint16 x, Uint16 y)
{
    if(dim < 1) return false;
    if(x < window.x || y < window.y) return false;
    if(x >= window.x + window.w || y >= window.y + window.h) return false;

    int i = (int) (x-window.x + offset.x)/CELL_X;
    int j = (int) (y-window.y + offset.y)/CELL_Y;

    return(Delete_Track(i,j,offset.z));
}

int
Grid3D::Activate_Tracks_Under(Uint16 x,Uint16 y,Uint16 z, Track* some_track)
{
    if(dim < 1) return false;
    if(!(&some_track)) return false;

    Point3D<int> p(x,y,z);
    p = p.Get_Limited_to(0,0,0,dim.x-1,dim.y-1,dim.z-1);

    int nx=(*some_track).Get_DimX();
    int ny=(*some_track).Get_DimY();
    int nz=(*some_track).Get_DimZ();
    if(p.x + nx -1 >= dim.x) return false;
    if(p.y + ny -1 >= dim.y) return false;
    if(p.z + nz -1 >= dim.z) return false;

    for(int i=0; i<dim.x; i++)
        for(int j=0; j<dim.y; j++)
            for(int k=0; k<dim.z; k++)
                (*grid[i][j][k]).UnSelect();

    Track* aux = NULL;
    for(int i=0; i<nx; i++)
        for(int j=0; j<ny; j++)
            for(int k=0; k<nz; k++)
            {
                aux = grid[p.x+i][p.y+j][p.z+k];
                (*aux).Select();
            }

    // /////////////////////////////
    // Codigo para actualisar las surface
    // /////////////////////////////
    window_changed = true;
    return true;
}

int
Grid3D::Activate_Tracks_Under(Uint16 x,Uint16 y, Track* some_track)
{
    if(dim < 1) return false;
    if(x < window.x || y < window.y) return false;
    if(x >= window.x + window.w || y >= window.y + window.h) return false;

    int i = (int) (x-window.x + offset.x)/CELL_X;
    int j = (int) (y-window.y + offset.y)/CELL_Y;

    return(Activate_Tracks_Under(i,j,offset.z,some_track));
}

void
Grid3D::Deactivate_All_Tracks()
{
    for(int i=0; i<dim.x; i++)
        for(int j=0; j<dim.y; j++)
            for(int k=0; k<dim.z; k++)
                (*grid[i][j][k]).UnSelect();

    // /////////////////////////////
    // Codigo para actualisar las surface
    // /////////////////////////////
    window_changed = true;
}

void
Grid3D::Clear_Cell_Sisters(Uint16 x,Uint16 y,Uint16 z, Track* track)
{
    if(grid[x][y][z] != track) return;
    if((*grid[x][y][z]).Get_Name() == "none") return;

    grid[x][y][z] = new Track();
    for(        int i=(x>0? x-1 : x), nx =(x<dim.x -1? x+1 : x); i<=nx; i++)
        for(    int j=(y>0? y-1 : y), ny =(y<dim.y -1? y+1 : y); j<=ny; j++)
            for(int k=(z>0? z-1 : z), nz =(z<dim.z -1? z+1 : z); k<=nz; k++)
                Clear_Cell_Sisters(i,j,k,track);
}















// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
//                                                                          //
//                         Set_Window & Set_Offset                          //
//                                                                          //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //

void
Grid3D::Set_Window(SDL_Rect& some_window)
{
    Set_Window(some_window.x, some_window.y, some_window.w, some_window.h);
}

void
Grid3D::Set_Window(int x, int y, Uint16 w, Uint16 h)
{
    if(w > dim.x * CELL_X) w = dim.x * CELL_X;
    if(h > dim.y * CELL_Y) h = dim.y * CELL_Y;
    window.Set_values(x,y,w,h);

    //window_background = image_collection(GRID_Background);
    //window_background = zoomSurface(window_background,(double)w/window_background->w,(double)h/window_background->h,1);

//    SDL_BlitSurface(window_background, 0, window_surface, &window);
//    if(window_surface) SDL_FreeSurface(window_surface);
//    Set_Offset(offset);
    window_changed = 1;
}

void
Grid3D::Set_Offset(Point3D<int>& some_offset)
{
    window_changed = 1;
    offset = Normalize_Offset(some_offset);
}

bool
Grid3D::Mouse_Over(Uint16 x, Uint16 y)
{
    if(x < window.x || y < window.y) return 0;
    if(x >= window.x + window.w || y >= window.y + window.h) return 0;
    return 1;
}













// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
//                                                                          //
//                             Private Methods                              //
//                                                                          //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
Point3D<int> *
Grid3D::Get_Top_Left_Syster(int x, int y)
{
    if(x<0 || y<0) return NULL;
    if(x>=dim.x || y>=dim.y) return NULL;

    Track *aux = grid[x][y][offset.z];

    int i=x,j=y;
    for(;x>=0; x--, i--) if(grid[x][y][offset.z] != aux) break;
    x++;
    for(;y>=0; y--, j--) if(grid[x][y][offset.z] != aux) break;

    Point3D<int> *point = new Point3D<int>(i+1,j+1,0);

    return(point);
}

Point3D<int>&
Grid3D::Normalize_Offset(Point3D<int>& some_offset)
{
    Point3D<int> *value = new Point3D<int>, aux = some_offset;

    if(aux.x <0) aux.x = 0;
    if(aux.y <0) aux.y = 0;
    if(aux.z <0) aux.z = 0;

    if(aux.x + window.w -1 >= dim.x * CELL_X) aux.x = dim.x * CELL_X - window.w;
    if(aux.y + window.h -1 >= dim.y * CELL_Y) aux.y = dim.y * CELL_Y - window.h;
    if(aux.z >= dim.z) aux.z = dim.z-1;

    (*value) = aux;
    return (*value);
}

ostream &
operator<<(ostream & o, Grid3D & some)
{
    o<<"Grid3D       name: "<<some.name<<endl;
    o<<"       dimensions: "<<some.dim.Print()<<endl;
    o<<"           offset: "<<some.offset.Print()<<endl;
    o<<"           window: "<<some.window<<endl;
    if(some.window_surface)
        o<<"   window_surface: "<<"<"<<(*some.window_surface).w<<","<<(*some.window_surface).h<<">"<<endl;
    else
        o<<"   window_surface: "<<"<NULL,NULL>\n";
    if(some.window_background)
        o<<"window_background: "<<"<"<<(*some.window_background).w<<","<<(*some.window_background).h<<">"<<endl;
    else
        o<<"window_background: "<<"<NULL,NULL>\n";
    o<<"   window_changed: "<<some.window_changed<<endl;
    return o;
}














// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
//                                                                          //
//                           Load & save circuits                           //
//                                                                          //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
int
Grid3D::Save(char* path)
{
    if(!path) return 0;
    if(dim<1) return 0;
    fstream o(path,ios::out);
    if(!o) return 0;

    map< Track* , Track* > painted;
    Track* aux=NULL;
    Point3D<int> point;
    for(int i=0; i<dim.x; i++)
        for(int j=0; j<dim.y; j++)
            for(int k=0; k<1/*dim.z*/; k++)
            {
                aux = grid[i][j][k];
                if((*aux).Get_Name() != "none" && !painted[aux])
                {
                    point = *((*aux).Get_First_Cell());
                    o<<"[Cell_"<<i<<j<<k<<"]\n";
                    o<<"x = "<<point.x + i<<"\n";
                    o<<"y = "<<point.y + j<<"\n";
                    o<<"z = "<<point.z + k<<"\n";
                    o<<"x0 = "<<i<<"\n";
                    o<<"y0 = "<<j<<"\n";
                    o<<"z0 = "<<k<<"\n";
                    o<<"model = "<<(*aux).Get_Name().c_str()<<"\n";
                    o<<"rotation = "<<(*aux).Get_Rotation() %360<<"\n";
                    if((*aux).Get_Start()) o<<"start = true\n";
                    if((*aux).Get_I_CheckPoint()) o<<"i_checkpoint = true\n";
                    o<<endl;
                    painted[aux]=aux;
                }
            }
    o.close();
    return 1;
}

int
Grid3D::Load(char* path, map<string, Track* > &tracks_map)
{
    if(!path) return 0;
    if(dim<1) return 0;

    fstream i(path,ios::in);
    if(!i) return 0;

    CFG_File config;

    int result = CFG_OpenFile(path, &config);
    if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
    {
        fprintf(stderr,"Unable to load file: %s\n", SDL_GetError());
        exit(1);
    }

//    if( CFG_OK == CFG_SelectGroup("Info", 0) ||
//        CFG_OK == CFG_SelectGroup("info", 0) )
//    {
//    }

    for ( CFG_StartGroupIteration(); !CFG_IsLastGroup(); CFG_SelectNextGroup() )
    {
//        if( !strcmp(CFG_GetSelectedGroupName(),"Info") ||
//            !strcmp(CFG_GetSelectedGroupName(),"info")) continue;
        int x = CFG_ReadInt("x0",0);
        int y = CFG_ReadInt("y0",0);
        int z = CFG_ReadInt("y0",0);
        x = 0<x && x<dim.x ? x : 0;
        y = 0<y && y<dim.y ? y : 0;
        z = 0<z && z<dim.z ? z : 0;

        string some_name = CFG_ReadText("model","");
        int rotation  = CFG_ReadInt("rotation",0);

        Track* track = tracks_map[some_name];
        if(!track) continue;
        track = new Track(*track);

        switch(rotation)
        {
            case 90:
                (*track).Rotate_Counter_Clockwise();
                break;
            case 180:
                (*track).Rotate_Counter_Clockwise();
                (*track).Rotate_Counter_Clockwise();
                break;
            case 270:
                (*track).Rotate_Counter_Clockwise();
                (*track).Rotate_Counter_Clockwise();
                (*track).Rotate_Counter_Clockwise();
                break;
        };
        Set_Track(x,y,z,*track);
        //delete track;
    }
    CFG_CloseFile(0);
    i.close();
    return 1;
}

void
Grid3D::Debug_Print_Grid(char* fich, int sufix, char* ext, char* remmark)
{
    char cad[100];
    sprintf(cad,"%s.%d.%s",(fich?fich:strdup("")),sufix,(ext?ext:strdup("")));
    ofstream o;
    o.open(cad);

    o<<remmark<<endl;
    for(int k = 0, nz = dim.z; k<nz; k++)
        for(int j = 0, ny = dim.y; j<ny; j++)
            for(int i = 0, nx = dim.x; i<nx; )
            {
                if((*grid[i][j][k]).Get_Name() == "none") o<<"[    ]";
                else    o<<"["<< (*grid[i][j][k]).Get_Name()<<"]";
                if(++i >= nx) o<<endl;
            }
    o<<endl;
    o.close();
}

