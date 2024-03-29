#include <track.h>

Track::Track(
            char* some_name, int DimX, int DimY, int DimZ,
            char* some_description)
{
    name = some_name;
    dimx = DimX;
    dimy = DimY;
    dimz = DimZ;
    description = some_description;

    Set_Icon3D(TRACK_icon3d_void);
    Set_Icon(TRACK_icon_void);
    active_trans = 0;

    Cell3D aux(this);
    vector< Cell3D > vz(dimz,aux);
    vector< vector< Cell3D > > vy(dimy,vz);
    cells.resize(dimx,vy);

    window = icon; //sets w and h
    rotation = 0;
    num_connectors = 0;
    start = 0;
    i_checkpoint = 0;
    speedfactor = 1;
}

Track::Track(
            char* some_name,
            SDL_Surface* some_icon, SDL_Surface* some_icon3d,
            int DimX, int DimY, int DimZ, char* some_description)
{
    name = some_name;
    dimx = DimX;
    dimy = DimY;
    dimz = DimZ;
    description = some_description;

    Set_Icon3D(some_icon3d);
    Set_Icon(some_icon);
    active_trans = 0;

    Cell3D aux(this);
    vector< Cell3D > vz(dimz,aux);
    vector< vector< Cell3D > > vy(dimy,vz);
    cells.resize(dimx,vy);

    window = icon; //sets w and h
    rotation = 0;
    num_connectors = 0;
    start = 0;
    i_checkpoint = 0;
    speedfactor = 1;
}

Track::Track(
            char* some_name,
            char* some_icon, char* some_icon3d,
            int DimX, int DimY, int DimZ, char* some_description)
{
    name = some_name;
    dimx = DimX;
    dimy = DimY;
    dimz = DimZ;
    description = some_description;

    Set_Icon3D(some_icon3d);
    Set_Icon(some_icon);
    active_trans = 0;

    Cell3D aux(this);
    vector< Cell3D > vz(dimz,aux);
    vector< vector< Cell3D > > vy(dimy,vz);
    cells.resize(dimx,vy);

    window = icon; //sets w and h
    rotation = 0;
    num_connectors = 0;
    start = 0;
    i_checkpoint = 0;
    speedfactor = 1;
}

Track::Track(string & file)
{
    CFG_File config;

    int result = CFG_OpenFile(file.c_str(), &config );
    if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
    {
        fprintf(stderr,"Unable to load file: %s\n", SDL_GetError());
        exit(1);
    }

    if( CFG_OK == CFG_SelectGroup("Info", 0) ||
        CFG_OK == CFG_SelectGroup("info", 0) )
    {
//        name = CFG_ReadText("name", "unspecified");
        description = CFG_ReadText("description", "");
        dimx = CFG_ReadInt("size_x", 1);
        dimy = CFG_ReadInt("size_y", 1);
        dimz = CFG_ReadInt("size_z", 1);
        num_connectors = CFG_ReadInt("connectors", 2);
        start = CFG_ReadBool("start", false);
        i_checkpoint = CFG_ReadBool("i_checkpoint", false);
        speedfactor  = CFG_ReadInt("speedfactor", 1);

        char fcadena[300];
        string ruta = getcwd(fcadena,300);
        string ruta_icon = ruta + "\\icon.png";
        string ruta_icon3d = ruta + "\\icon3d.png";
        Set_Icon((char*)ruta_icon.c_str());
        Set_Icon3D((char*)ruta_icon3d.c_str());

    }
    else
    {
//        name = "none";
        description = "";
        dimx=1;
        dimy=1;
        dimz=1;
        num_connectors = 0;
        Set_Icon3D(TRACK_icon3d_void);  //puede petardear si no esta
        Set_Icon(TRACK_icon_void);      //en el directorio del ejecutable!!
        start = 0;
        i_checkpoint = 0;
        speedfactor = 1;
    }

    Cell3D aux(this);
    vector< Cell3D > vz(dimz,aux);
    vector< vector< Cell3D > > vy(dimy,vz);
    cells.resize(dimx,vy);

    active_trans = 0;
    window = icon; //sets w and h
    rotation = 0;

    fprintf(stderr,"description: %s\n", (char*)description.c_str());
    fprintf(stderr,"tamanyo: <%d,%d,%d>\n", dimx,dimy,dimz);
    fprintf(stderr,"num_conn: %d\n", num_connectors);
    if(start) fprintf(stderr,"is start\n");
    if(i_checkpoint) fprintf(stderr,"i_checkpoint\n");
    if(num_connectors) for ( CFG_StartGroupIteration(); !CFG_IsLastGroup(); CFG_SelectNextGroup() )
    {
        if(!strcmp(CFG_GetSelectedGroupName(),"Info") || !strcmp(CFG_GetSelectedGroupName(),"info")) continue;
        int x = CFG_ReadInt("x",0);
        int y = CFG_ReadInt("y",0);
        int z = CFG_ReadInt("y",0);
        x = 0<=x && x<dimx ? x : 0;
        y = 0<=y && y<dimy ? y : 0;
        z = 0<=z && z<dimz ? z : 0;

        cells[x][y][z].Set_North(CFG_ReadText("north","NONE"));
        cells[x][y][z].Set_West(CFG_ReadText("west","NONE"));
        cells[x][y][z].Set_South(CFG_ReadText("south","NONE"));
        cells[x][y][z].Set_East(CFG_ReadText("east","NONE"));
        cells[x][y][z].Set_Track(this);

//        fprintf(stderr,"  celda[%d][%d][%d]\n",x,y,z);
//        if(cells[x][y][z].Get_North())
//            fprintf(stderr,"  north = %d\n",cells[x][y][z].Get_North());
//        if(cells[x][y][z].Get_West())
//            fprintf(stderr,"  west = %d\n",cells[x][y][z].Get_West());
//        if(cells[x][y][z].Get_South())
//            fprintf(stderr,"  south = %d\n",cells[x][y][z].Get_South());
//        if(cells[x][y][z].Get_East())
//            fprintf(stderr,"  east = %d\n",cells[x][y][z].Get_East());
    }
    CFG_CloseFile(0);
}

Track::Track(const Track & some)
{
    (*this)=some;
}

Track::~Track()
{
    icon = NULL;
    icon3D = NULL;
    //SDL_FreeSurface(selected);
    selected = NULL;
    dimx = dimy = dimz = 0;
    rotation = 0;
    start = 0;
    i_checkpoint = 0;
    speedfactor = 1;
}

Track &
Track::operator=(const Track & some)
{
    if(this == &some) return (*this);

    name = some.name;
    icon = some.icon;
    selected = some.selected;
    active_trans = 0;//some.active_trans;
    icon3D = some.icon3D;
    description = some.description;

    dimx = some.dimx;
    dimy = some.dimy;
    dimz = some.dimz;

    cells = some.cells;
    for(int i = 0; i<dimx; i++)
        for(int j = 0; j<dimy; j++)
            for(int k = 0; k<dimz; k++)
                cells[i][j][k].Set_Track(this);

    window = some.window;
    rotation = some.rotation;
    start = some.start;
    i_checkpoint = some.i_checkpoint;
    speedfactor = some.speedfactor;

    return(*this);
}

bool
Track::operator==(const Track & some)
{
    if(!(&some)) return false;
    bool ok;
    ok = (
        name == some.name &&
        dimx == some.dimx &&
        dimy == some.dimy &&
        dimz == some.dimz &&
        icon == some.icon &&
        icon3D == some.icon3D &&
        start == some.start &&
        i_checkpoint == some.i_checkpoint &&
        speedfactor == some.speedfactor);
    return ok;
}

bool
Track::operator!=(const Track & some)
{
    return(!(*this == some));
}

Cell3D*
Track::Get_Cell(unsigned int x, unsigned int y, unsigned int z)
{
    if(!dimx || !dimy || !dimz) return NULL;

    x=(x>=dimx?dimx-1:x);
    y=(y>=dimy?dimy-1:y);
    z=(z>=dimz?dimz-1:z);

    return &cells[x][y][z];
}

void
Track::Set_Cell(unsigned int x, unsigned int y, unsigned int z, Cell3D* one_cell)
{
    if(!dimx || !dimy || !dimz) return;

    x=(x>=dimx?dimx-1:x);
    y=(y>=dimy?dimy-1:y);
    z=(z>=dimz?dimz-1:z);

    cells[x][y][z] = *one_cell;
    cells[x][y][z].Set_Track(this);
}

int
Track::Normalize_Angle(unsigned int angle)
{
    while(angle >= 360) angle -= 360;
    angle =   0 <= angle && angle <  90 ?   0 : angle ;
    angle =  90 <= angle && angle < 180 ?  90 : angle ;
    angle = 180 <= angle && angle < 270 ? 180 : angle ;
    angle = 270 <= angle && angle < 360 ? 270 : angle ;
    return angle;
}

void
Track::Draw(SDL_Surface* screen, SDL_Rect & some_window, int angle, bool set_window, bool set_rotation)
{
    if(screen == NULL) return;

    angle = Normalize_Angle(angle);
    Rect2D rect = some_window;

    SDL_Surface *bmp=NULL, *sel=NULL;
    switch(angle)
    {
        case 0:
            SDL_BlitSurface(icon, 0, screen, &some_window);
            if(active_trans)
            {
                sel = zoomSurface(selected,
                        (double)icon->w/selected->w,
                        (double)icon->h/selected->h, 0);
                SDL_BlitSurface(sel, 0, screen, &rect);
                SDL_FreeSurface(sel);
            }
            break;
        case 90:
            bmp = rotozoomSurface(icon, -270, 1, 0);
            some_window.x--;
            some_window.y-=2;
            SDL_BlitSurface(bmp, 0, screen, &some_window);
            if(active_trans)
            {
                rect.x+1;
                rect.y+2;
                sel = zoomSurface(selected,
                        (double)((bmp->w-1)/selected->w),
                        (double)((bmp->h-2)/selected->h), 0);
                SDL_BlitSurface(sel, 0, screen, &rect);
                SDL_FreeSurface(sel);
            }
            SDL_FreeSurface(bmp);
            break;
        case 180:
            bmp = rotozoomSurface(icon, 180, 1, 0);
            some_window.x-=2;
            some_window.y-=2;
            SDL_BlitSurface(bmp, 0, screen, &some_window);
            if(active_trans)
            {
                rect.x+2;
                rect.y+2;
                sel = zoomSurface(selected,
                        (double)((bmp->w-2)/selected->w),
                        (double)((bmp->h-2)/selected->h), 0);
                SDL_BlitSurface(sel, 0, screen, &rect);
                SDL_FreeSurface(sel);
            }
            SDL_FreeSurface(bmp);
            break;
        case 270:
            bmp = rotozoomSurface(icon, 270, 1, 0);
            some_window.x-=2;
            some_window.y--;
            SDL_BlitSurface(bmp, 0, screen, &some_window);
            if(active_trans)
            {
                rect.x+2;
                rect.y+1;
                sel = zoomSurface(selected,
                        (double)((bmp->w-2)/selected->w),
                        (double)((bmp->h-1)/selected->h), 0);
                SDL_BlitSurface(sel, 0, screen, &rect);
                SDL_FreeSurface(sel);
            }
            SDL_FreeSurface(bmp);
            break;
    }
    if(set_rotation) rotation = angle;
    if(set_window) window=some_window;
}

void
Track::Draw(SDL_Surface* screen, SDL_Rect & some_window)
{
    Draw(screen, some_window, rotation);
}

void
Track::Draw(SDL_Surface* screen)
{
    Draw(screen, window, rotation);
}

void
Track::Draw_Icon3D(SDL_Surface* screen, int x, int y)
{
    if(screen == NULL) return;
    Rect2D win(x,y);
    SDL_BlitSurface(icon3D, 0, screen, &win);
}

void
Track::Draw_Icon3D(SDL_Surface* screen, SDL_Rect some_window)
{
    if(screen == NULL) return;
    SDL_BlitSurface(icon3D, 0, screen, &some_window);
}

void
Track::Clear()
{
    Set_Icon3D(TRACK_icon3d_void);
    Set_Icon(TRACK_icon_void);
}

void
Track::Rotate_Counter_Clockwise()
{
    rotation = Normalize_Angle(rotation + 90);

    // create an auxiliar triple-vector
    Cell3D aux_C;
    vector< Cell3D > vz(dimz,aux_C);
    vector< vector< Cell3D > > vy(dimx,vz);
    vector< vector< vector< Cell3D > > > aux_cells(dimy,vy);

    // Rotate of cells (+90�)
    for (int i=0, aux_y = dimx-1; i<dimx; i++, aux_y--)
        for (int j=0, aux_x=0; j<dimy; j++, aux_x++)
            for (int k=0; k<dimz; k++)
            {
                aux_cells[aux_x][aux_y][k] = cells[i][j][k];
                aux_cells[aux_x][aux_y][k].Connectors_Rotation();
            }

    cells = aux_cells;
    //swap dimensions with three xor
    dimx ^= dimy;
    dimy ^= dimx;
    dimx ^= dimy;
}

void
Track::Set_Icon(SDL_Surface* some_icon)
{
    icon = some_icon!=NULL ? some_icon : image_collection(TRACK_icon_void);
    //if(selected) SDL_FreeSurface(selected);
    SDL_Surface* alpha = image_collection(TRACK_icon_selected);
    selected = zoomSurface(alpha,icon->w/alpha->w,icon->h/alpha->h,0);
}

void
Track::Set_Icon3D(SDL_Surface* some_icon3d)
{
    icon3D = some_icon3d!=NULL ? some_icon3d : image_collection(TRACK_icon3d_void);
}

Point3D<int>*
Track::Get_First_Cell()
{
    Point3D<int>* point = new Point3D<int>;

    switch(rotation)
    {
        case 0:
            (*point).Set_Values(0,0,0);
            break;
        case 90:
            (*point).Set_Values(0,dimy-1,0);
            break;
        case 180:
            (*point).Set_Values(dimx-1,dimy-1,0);
            break;
        case 270:
            (*point).Set_Values(dimx-1,0,0);
            break;
    };
    return (point);
}
