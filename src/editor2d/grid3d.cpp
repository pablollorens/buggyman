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

    Set_BackGround(GRID_background);
    offset = 0;
    minimap = NULL;
    minimap_view = NULL;
    circuit_not_valid = NULL;
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

    Set_BackGround(GRID_background);
    offset=0;
    minimap = NULL;
    minimap_view = NULL;
    circuit_not_valid = NULL;
    Set_Window(some_window);
}

Grid3D::Grid3D(Grid3D & some)
{
    window_background = NULL;
    window_surface = NULL;
    minimap = NULL;
    minimap_view = NULL;
    circuit_not_valid = NULL;
    (*this)=some;
}

Grid3D::~Grid3D()
{
    dim = 0;
    if(window_surface) SDL_FreeSurface(window_surface);
    if(minimap) SDL_FreeSurface(minimap);
    window_surface = NULL;
    window_background = NULL;
    minimap = NULL;
    minimap_view = NULL;
    circuit_not_valid = NULL;
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
    window_background = some.window_background;
    minimap = zoomSurface(some.minimap,1,1,0);
    minimap_rect = some.minimap_rect;
    minimap_view = zoomSurface(some.minimap_view,1,1,0);
    minimap_view_rect = some.minimap_view_rect;
    circuit_not_valid = zoomSurface(some.circuit_not_valid,1,1,0);
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
    if(dim<1) return;

//    if(window_changed)
    {
        SDL_SetClipRect(screen, &window);
        Rect2D rect(window.x - offset.x,window.y - offset.y,0,0);
        SDL_BlitSurface(window_surface, 0, screen, &rect);

//        rect.Set_values(window.x - offset.x,window.y - offset.y,0,0);
//        SDL_BlitSurface(window_surface, 0, screen, &rect);

        window_changed = 0;
        SDL_SetClipRect(screen, NULL);

        SDL_SetClipRect(screen, &minimap_rect);
        SDL_BlitSurface(minimap, 0, screen, &minimap_rect);
        SDL_BlitSurface(minimap_view, 0, screen, &minimap_view_rect);
        SDL_SetClipRect(screen, NULL);
    }
    return;
}

void
Grid3D::Draw(Point3D<int> & p, Track & track, bool draw_track)
{
    Rect2D rect(p.x * CELL_X, p.y * CELL_Y,
                track.Get_DimX()*CELL_X, track.Get_DimY()*CELL_Y);

    SDL_SetClipRect(window_surface, &rect);
    SDL_BlitSurface(window_background, 0, window_surface, 0);
    SDL_SetClipRect(window_surface, NULL);

    if(draw_track) track.Draw(window_surface,rect);
    //Update_Minimap();
    window_changed = true;
}

void
Grid3D::Set_BackGround(SDL_Surface* image)
{
    window_background = zoomSurface(
        image,(double)dim.x*CELL_X / image->w,(double)dim.y*CELL_Y / image->h,1);
    window_surface = zoomSurface(window_background,1,1,0);
}

void
Grid3D::Set_BackGround(char* image)
{
    Set_BackGround(image_collection(image));
}

void
Grid3D::Set_Minimap_Window(SDL_Rect& some_window)
{
    Set_Minimap_Window(some_window.x, some_window.y, some_window.w, some_window.h);
}

void
Grid3D::Set_Minimap_Window(int x, int y, Uint16 w, Uint16 h)
{
    minimap_rect.Set_values(x,y,w,h);
    minimap_view = image_collection(GRID_minimap_view);

    minimap_view = zoomSurface(minimap_view,
        (double)(window.w * minimap_rect.w / (dim.x * CELL_X)) / minimap_view->w,
        (double)(window.h * minimap_rect.h / (dim.y * CELL_Y)) / minimap_view->h,
        1);
    minimap_view_rect.Set_values(minimap_rect.x, minimap_rect.y, minimap_view->w, minimap_view->h);
    Update_Minimap();
}

void
Grid3D::Update_Minimap()
{
    if(minimap) SDL_FreeSurface(minimap);
    minimap = zoomSurface(window_surface,
    (double)minimap_rect.w/ window_surface->w,
    (double)minimap_rect.h/ window_surface->h,1);
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

    for(int i=0; i<nx; i++)
        for(int j=0; j<ny; j++)
            for(int k=0; k<nz; k++)
            {
                Track* aux = grid[p.x+i][p.y+j][p.z+k];
                if((*aux).Get_Name() != "none")
                {
                    //Clear_Cell_Sisters(p.x+i,p.y+j,p.z+k,aux);
                    //delete aux;
                    Delete_Track_without_Update(p.x+i,p.y+j,p.z+k);
                }
                grid[p.x+i][p.y+j][p.z+k] = newtrack;
            }

    Draw(p,(*newtrack), true);
    Update_Minimap();
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
    bool inserted = false;

    for (int y_grid = 0; y_grid <= Get_DimY() && inserted == false; y_grid++)
    {
        for (int x_grid = 0; x_grid <= Get_DimX() && inserted == false; x_grid++)
        {
            if ((*Get_Track(x_grid,y_grid,0)).Get_Name() == "none")
            {
                if ((x_grid + (*some_track).Get_DimX() <= Get_DimX()) && (y_grid + (*some_track).Get_DimY() <= Get_DimY()))
                {
                    Set_Track(x_grid, y_grid, 0, (*some_track));
                    inserted = true;
                }
            }
        }
    }
    return true;
}

bool
Grid3D::Delete_Track(Uint16 x, Uint16 y, Uint16 z)
{
    if(dim < 1) return false;

    Point3D<int> p(x,y,z);
    p = p.Get_Limited_to(0,0,0,dim.x-1,dim.y-1,dim.z-1);

    Track* aux = grid[p.x][p.y][p.z];
    //Track *voidtrack = new Track;
    if((*aux).Get_Name() == "none") return false;
    Point3D<int> p0 = *Get_Top_Left_Syster(p.x,p.y);
    Clear_Cell_Sisters(p.x,p.y,p.z,aux);

    Draw(p0,(*aux), false);
    Update_Minimap();
    delete aux;
    return true;
}


bool
Grid3D::Delete_Track_without_Update(Uint16 x, Uint16 y, Uint16 z)
{
    if(dim < 1) return false;

    Point3D<int> p(x,y,z);
    p = p.Get_Limited_to(0,0,0,dim.x-1,dim.y-1,dim.z-1);

    Track* aux = grid[p.x][p.y][p.z];
    //Track *voidtrack = new Track;
    if((*aux).Get_Name() == "none") return false;
    Point3D<int> p0 = *Get_Top_Left_Syster(p.x,p.y);
    Clear_Cell_Sisters(p.x,p.y,p.z,aux);

    Draw(p0,(*aux), false);
    delete aux;
    return true;
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

    Deactivate_All_Tracks();

    Track* aux = NULL;
    for(int i=0; i<nx; i++)
        for(int j=0; j<ny; j++)
            for(int k=0; k<nz; k++)
            {
                aux = grid[p.x+i][p.y+j][p.z+k];
                if((*aux).Get_Selected()) continue;
                (*aux).Select();

                Point3D<int> *p0 = Get_Top_Left_Syster(p.x+i,p.y+j);
                Draw(*p0,(*aux), true);
                //pair<Track* , Point3D<int> > par(aux,*p0);
                //activated_tracks.push_back(par);
            }

    return window_changed;
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
//    fprintf(stderr,"Pistas activadas = %d\n",activated_tracks.size());
//    for(list<pair<Track*, Point3D<int> > >::iterator itr = activated_tracks.begin(),
//    end = activated_tracks.begin();
//    itr != end; ++itr)
//    {
//        Track* track = (*itr).first;
//        (*track).UnSelect();
//        //Point3D<int> p = (*itr).second;
//        Draw((*itr).second,(*track), true);
//    }
//    activated_tracks.clear();

    for(int i=0; i<dim.x; i++)
        for(int j=0; j<dim.y; j++)
            for(int k=0; k<dim.z; k++)
            {
                if(!(*grid[i][j][k]).Get_Selected()) continue;
                (*grid[i][j][k]).UnSelect();
                Point3D<int> *p = Get_Top_Left_Syster(i,j);
                Draw(*p,*grid[i][j][k], true);
            }
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

    window_changed = 1;
}

void
Grid3D::Set_Offset(Point3D<int>& some_offset)
{
    window_changed = 1;
    offset = Normalize_Offset(some_offset);

    minimap_view_rect.x = (offset.x * minimap_rect.w / (dim.x * CELL_X)) + minimap_rect.x;
    minimap_view_rect.y = (offset.y * minimap_rect.h / (dim.y * CELL_Y)) + minimap_rect.y;
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
                    o<<"[Cell_"<<i<<"_"<<j<<"_"<<k<<"]\n";
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
                    if((*aux).Get_SpeedFactor() >1) o<<"speedfactor = "<<(*aux).Get_SpeedFactor()<<"\n";
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
        int x = CFG_ReadInt("x0",-1);
        int y = CFG_ReadInt("y0",-1);
        int z = CFG_ReadInt("z0",-1);
        if(!(0<=x && x<dim.x)) continue;
        if(!(0<=y && y<dim.y)) continue;
        if(!(0<=z && z<dim.z)) continue;

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
        delete track;
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



// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
//                                                                          //
//                           Check circuits                                 //
//                                                                          //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////// //

bool
Grid3D::Check_Circuit(Point3D<int> & circuit_error)
{
    bool correct = true;
    int num_starts = 0;
    int x_grid = 0;
    int y_grid = 0;

    for (x_grid = 0; x_grid < Get_DimX() && correct == true; x_grid++)
    //while (x_grid < Get_DimX() && correct == true)
    {
        for (y_grid = 0; y_grid < Get_DimY() && correct == true; y_grid++)
        //while (y_grid < Get_DimY() && correct == true)
        {
            //fprintf(stderr,"<%d,%d> --- (%d,%d,%d,%d)\n", x_grid,y_grid,Get_Connector(x_grid,y_grid,NORTH),Get_Connector(x_grid,y_grid,WEST),Get_Connector(x_grid,y_grid,SOUTH),Get_Connector(x_grid,y_grid,EAST));
            //fprintf(stderr,"<%d,%d> --- (%d,%d,%d,%d)\n", x_grid,y_grid+1,Get_Connector(x_grid,y_grid+1,NORTH),Get_Connector(x_grid,y_grid+1,WEST),Get_Connector(x_grid,y_grid+1,SOUTH),Get_Connector(x_grid,y_grid+1,EAST));

            if (y_grid > 0)
                if(Check_Connector(Get_Connector(x_grid,y_grid,NORTH),Get_Connector(x_grid,y_grid-1,SOUTH)) == false)
                {
                    correct=false;
                    break;
                }

            if (x_grid < Get_DimX()-1)
                if(Check_Connector(Get_Connector(x_grid,y_grid,EAST),Get_Connector(x_grid+1,y_grid,WEST)) == false)
                {
                    correct=false;
                    break;
                }

            if (y_grid < Get_DimY()-1)
                if(Check_Connector(Get_Connector(x_grid,y_grid,SOUTH),Get_Connector(x_grid,y_grid+1,NORTH)) == false)
                {
                    correct=false;
                    break;
                }

            if (x_grid > 0)
                if(Check_Connector(Get_Connector(x_grid,y_grid,WEST),Get_Connector(x_grid-1,y_grid,EAST)) == false)
                {
                    correct=false;
                    break;
                }

            if ((*Get_Track(x_grid,y_grid,0)).Get_Start()) num_starts++;
        }
        if (!correct) break;
    }

    if (!correct)
    {
        circuit_error.x = x_grid;
        circuit_error.y = y_grid;
    }

     if (num_starts != 1) correct = false;

    return correct;
}

int
Grid3D::Get_Connector(int x_grid, int y_grid, int coordenate)
{
    int x_track = 0;
    int y_track = 0;
    Point3D<int> top_left = *Get_Top_Left_Syster(x_grid, y_grid);

    switch (coordenate)
    {
        case 0: return ((*(*Get_Track(x_grid,y_grid,0)).Get_Cell(x_grid-top_left.x,y_grid-top_left.y,0)).Get_North()); break;
        case 1: return ((*(*Get_Track(x_grid,y_grid,0)).Get_Cell(x_grid-top_left.x,y_grid-top_left.y,0)).Get_West()); break;
        case 2: return ((*(*Get_Track(x_grid,y_grid,0)).Get_Cell(x_grid-top_left.x,y_grid-top_left.y,0)).Get_South()); break;
        case 3: return ((*(*Get_Track(x_grid,y_grid,0)).Get_Cell(x_grid-top_left.x,y_grid-top_left.y,0)).Get_East()); break;
        default: return -10; break;
    }
}

bool
Grid3D::Check_Connector(int conn_1, int conn_2)
{
    bool correct = true;
    switch (conn_1)
    {
        case -1: if (conn_2 != -1) correct = false; break;
        case  0: if (conn_2 != 0) correct = false; break;
        case  1: if (conn_2 != 1) correct = false; break;
        case  2: if (conn_2 != 2) correct = false; break;
        case  3: if (conn_2 != 4) correct = false; break;
        case  4: if (conn_2 != 3) correct = false; break;
        case  5: if (conn_2 != 5) correct = false; break;
    }
    return correct;
}

bool
Grid3D::Clear_Circuit()
{
    if(dim<1) return 0;
    for(int i=0; i<dim.x; i++)
        for(int j=0; j<dim.y; j++)
            for(int k=0; k<dim.z; k++)
                Delete_Track(i,j,k);
    return true;
}

int
Grid3D::Activate_Tracks_Error(Uint16 x,Uint16 y,Uint16 z)
{
    if(dim < 1) return false;

    Point3D<int> p(x,y,z);

    Deactivate_All_Tracks();

    Track* aux = NULL;
    aux = grid[p.x][p.y][p.z];
    (*aux).Select();

    Point3D<int> *p0 = Get_Top_Left_Syster(p.x,p.y);
    Draw(*p0,(*aux), true);

    return window_changed;
}

