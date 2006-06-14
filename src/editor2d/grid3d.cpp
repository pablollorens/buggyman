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

    Cell3D aux(&null_track);
    vector< Cell3D > vz(dim.z,aux);
    vector< vector< Cell3D > > vy(dim.y,vz);
    grid.resize(dim.x,vy);
    tracks[&null_track]= &null_track;

    window_surface = NULL;
    window_background = NULL;
    offset = 0;
    Set_Window(wx, wy, ww, wh);
}

Grid3D::Grid3D( char* some_name, Point3D<int> & some_dim, Rect2D & some_window)
{
    name = some_name;

    dim = some_dim.Get_Lower_Limited_to(1);

    Cell3D aux(&null_track);
    vector< Cell3D > vz(dim.z,aux);
    vector< vector< Cell3D > > vy(dim.y,vz);
    grid.resize(dim.x,vy);
    tracks[&null_track]= &null_track;

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
    tracks = some.tracks;

    window = some.window;
    window_surface = zoomSurface(some.window_surface,1,1,0);
    window_background = zoomSurface(some.window_background,1,1,0);
    offset = some.offset;
    window_changed = true;

    return(*this);
}

bool
Grid3D::operator==(Grid3D & some)
{
    return(name == some.name && dim == some.dim);
}

bool
Grid3D::operator!=(Grid3D & some)
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

    map< Track* , Track* > drown;
    drown[&null_track]=&null_track;
    Track *auxt=NULL;
    Point3D<int> *top_left=NULL;

    for(int i=0; i<=limit.x && i+cell_offset.x<dim.x; i++)
        for(int j=0; j<=limit.y && j+cell_offset.y<dim.y; j++)
        {
            auxt = grid[i+cell_offset.x][j+cell_offset.y][cell_offset.z].Get_Track();
            if(!drown[auxt] && tracks[auxt] && auxt!= &null_track)
            {
                top_left = Get_Top_Left_Syster(i+cell_offset.x, j+cell_offset.y);
                (*auxt).Set_Window(
                                (*top_left).x * CELL_X + window.x - offset.x,
                                (*top_left).y * CELL_Y + window.y - offset.y);
                (*auxt).Draw(screen);//window_surface);
                drown[auxt]=auxt;
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

    Track* track=grid[p.x][p.y][p.z].Get_Track();
    if(track!= &null_track && tracks[track]) track = new Track(*track);
    else track=NULL;
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
    tracks[newtrack]=newtrack;

    for(int i=0; i<nx; i++)
        for(int j=0; j<ny; j++)
            for(int k=0; k<nz; k++)
            {
                Track* aux = grid[p.x+i][p.y+j][p.z+k].Get_Track();
                if(aux != &null_track && tracks[aux])
                {
                    delete aux;
                    grid[p.x+i][p.y+j][p.z+k].Clean_Sisters_and_yourself(grid,p.x+i,p.y+j,p.z+k,aux,&null_track);
                    tracks[aux]=NULL;
                }
                grid[p.x+i][p.y+j][p.z+k] = *((*newtrack).Get_Cell(i,j,k));
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
Grid3D::Delete_Track(Uint16 x, Uint16 y, Uint16 z)
{
    if(dim < 1) return false;

    Point3D<int> p(x,y,z);
    p = p.Get_Limited_to(0,0,0,dim.x-1,dim.y-1,dim.z-1);

    Track* aux = grid[p.x][p.y][p.z].Get_Track();
    if(aux != &null_track && tracks[aux])
    {
        delete aux;
        grid[p.x][p.y][p.z].Clean_Sisters_and_yourself(grid,p.x,p.y,p.z,aux,&null_track);
        tracks[aux]=NULL;
        window_changed = true;
        return true;
    }

    // ////////////////////////////////// //
    // Codigo para actualisar las surface //
    // ////////////////////////////////// //
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
                if(tracks[grid[i][j][k].Get_Track()])
                    (*grid[i][j][k].Get_Track()).UnSelect();

    Track* aux = NULL;
    for(int i=0; i<nx; i++)
        for(int j=0; j<ny; j++)
            for(int k=0; k<nz; k++)
            {
                aux = grid[p.x+i][p.y+j][p.z+k].Get_Track();
                if(tracks[aux])
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
                if(tracks[grid[i][j][k].Get_Track()])
                    (*grid[i][j][k].Get_Track()).UnSelect();

    // /////////////////////////////
    // Codigo para actualisar las surface
    // /////////////////////////////
    window_changed = true;
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

    Track *aux = grid[x][y][offset.z].Get_Track();
    if(aux == &null_track || !tracks[aux]) return NULL;

    int i=x,j=y;
    for(;x>=0; x--, i--) if(grid[x][y][offset.z].Get_Track() != aux) break;
    x++;
    for(;y>=0; y--, j--) if(grid[x][y][offset.z].Get_Track() != aux) break;

    Point3D<int> *point = new Point3D<int>(i+1,j+1,0);

    return(point);
}

Point3D<int>&
Grid3D::Normalize_Offset(Point3D<int>& some_offset)
{
    Point3D<int>* value = new Point3D<int>, aux;
    aux = some_offset;

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

    map< Track* , Track* > drown;
    drown[&null_track]=&null_track;
    Track* aux=NULL;
    Point3D<int>* point=NULL;
    for(int i=0; i<dim.x; i++)
        for(int j=0; j<dim.y; j++)
            for(int k=0; k<1/*dim.z*/; k++)
            {
                aux = grid[i][j][k].Get_Track();
                if(!drown[aux] && tracks[aux] && aux!= &null_track)
                {
                    point = (*aux).Get_First_Cell();
                    o<<"[Cell_"<<i<<j<<"]\n";
                    o<<"x = "<<(*point).x + i<<"\n";
                    o<<"y = "<<(*point).y + j<<"\n";
                    o<<"model = "<<(*aux).Get_Name().c_str()<<"\n";
                    o<<"rotation = "<<((*aux).Get_Rotation()+0)%360<<"\n\n";
                    drown[aux]=aux;
                    delete point;
                    point = NULL;
                }
            }
    o.close();
    return 1;
}

int
Grid3D::Load(char* path)
{
    return 0;
//    if(!path) return 0;
//    if(dim<1) return 0;
//
//    CFG_File config;
//
//    int result = CFG_OpenFile(path, &config);
//
//    if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
//    {
//        fprintf(stderr,"Unable to load file: %s\n", SDL_GetError());
//        exit(1);
//    }
//
//    if ( CFG_OK == CFG_SelectGroup("Info", 0) )
//    {
//        name = CFG_ReadText("name", "unspecified");
//        description = CFG_ReadText("description", "");
//        dimx = CFG_ReadInt("size_x", 1);
//        dimy = CFG_ReadInt("size_y", 1);
//        dimz = CFG_ReadInt("size_z", 1);
//        num_connectors = CFG_ReadInt("connectors", 2);
//        Set_Icon3D((char*)CFG_ReadText("icon3d",TRACK_icon3d_void));
//        Set_Icon((char*)CFG_ReadText("icon",TRACK_icon_void));
//    }
//    else
//    {
//        name = "none";
//        description = "none";
//        dimx=1;
//        dimy=1;
//        dimz=1;
//        num_connectors = 0;
//        Set_Icon3D(TRACK_icon3d_void);
//        Set_Icon(TRACK_icon_void);
//    }
//
//    Cell3D aux(this);
//    vector< Cell3D > vz(dimz,aux);
//    vector< vector< Cell3D > > vy(dimy,vz);
//    cells.resize(dimx,vy);
//
//    active_trans = 0;
//    window = icon; //sets w and h
//    rotation = 0;
//
//    for ( CFG_StartGroupIteration(); !CFG_IsLastGroup(); CFG_SelectNextGroup() )
//    {
//        int x = CFG_ReadInt("x",0);
//        int y = CFG_ReadInt("y",0);
//        int z = CFG_ReadInt("y",0);
//        x = 0<=x && x<dimx ? x : 0;
//        y = 0<=y && y<dimy ? y : 0;
//        z = 0<=z && z<dimz ? z : 0;
//
//        cells[x][y][z].Set_North(CFG_ReadInt("north",CONN_NONE));
//        cells[x][y][z].Set_West(CFG_ReadInt("west",CONN_NONE));
//        cells[x][y][z].Set_South(CFG_ReadInt("south",CONN_NONE));
//        cells[x][y][z].Set_East(CFG_ReadInt("east",CONN_NONE));
//        cells[x][y][z].Set_Track(this);
//    }
//    CFG_CloseFile(0);
//
//
//
//
//
//
//    fstream o(path,ios::out);
//    if(!o) return 0;
//
//    map< Track* , Track* > drown;
//    drown[&null_track]=&null_track;
//    Track* aux=NULL;
//    for(int i=0; i<dim.x; i++)
//        for(int j=0; j<dim.y; j++)
//            for(int k=0; k<1/*dim.z*/; k++)
//            {
//                aux = grid[i][j][k].Get_Track();
//                if(!drown[aux] && tracks[aux] && aux!= &null_track)
//                {
//                    o<<"[Cell_"<<i<<j<<"]\n";
//                    o<<"x = "<<i<<"\n";
//                    o<<"y = "<<j<<"\n";
//                    o<<"model = "<<(*aux).Get_Name().c_str()<<"\n";
//                    o<<"rotation = "<<((*aux).Get_Rotation()+0)%360<<"\n\n";
//                    drown[aux]=aux;
//                }
//            }
//    o.close();
}

