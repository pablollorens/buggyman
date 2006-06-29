#include <game/Ground.h>

/// //////////////////////////////////////////////////////////////////////// ///
/// STATICS MEMBERS DEFINITION

Ground_Cell Ground::Cell_Matrix[X][Y];

Model *Ground::ground_Model;
Model *Ground::ModelosMilky[K_MODEL];
dTriMeshDataID Ground::MeshesData[K_MODEL];
map< string, TrackInfo* > Ground::track_map;
dGeomID Ground::walls[4];

/// //////////////////////////////////////////////////////////////////////// ///
/// CONSTRUCTOR

Ground::Ground(dWorldID world, dSpaceID space)
{
    int id = 1000;
    char fcadena[300];
    string ruta = getcwd(fcadena,300);

    dsPrint("\tCarga de Modelos\n");

    list< string > directorios = Get_Directories(DIR_TRACKS);
    for (list< string >::iterator itr = directorios.begin(), end = directorios.end();
         itr != end ; ++itr, id++)
    {
        string ruta_nueva = ruta + "\\";
               ruta_nueva += DIR_TRACKS;
               ruta_nueva += "\\";
               ruta_nueva += *itr;
        chdir(ruta_nueva.c_str());
        TrackInfo* info = new TrackInfo;

        info->id = id;
        info->name = *itr;
        info->roadmodel = new MilkshapeModel();
        info->roadmodel->loadModelData("model.ms3d");
        info->roadmesh = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSimple(info->roadmesh,(dReal*)info->roadmodel->getVertices(),
                                                    info->roadmodel->getVertexCount(),
                                                    info->roadmodel->getIndices(),
                                                    info->roadmodel->getIndexCount());
        track_map[*itr] = info;
        chdir(ruta.c_str());
    }

    dsPrint("\tInicialización de Casillas\n");
    // Inicializamos todas las casillas
    for(int i=0; i<X ; i++)
        for(int j=0; j<Y; j++){
            dGeomSetPosition(Cell_Matrix[i][j].geomID,i*CELL_TAM,j*CELL_TAM,0);
            dSpaceAdd(space,Cell_Matrix[i][j].geomID);
        }

    // Creamos muros invisibles
    walls[0] = dCreateBox(0,X*CELL_TAM+CELL_TAM,WALL_TAM,WALL_HEIGHT);
    walls[1] = dCreateBox(0,WALL_TAM,X*CELL_TAM+CELL_TAM,WALL_HEIGHT);
    walls[2] = dCreateBox(0,WALL_TAM,X*CELL_TAM+CELL_TAM,WALL_HEIGHT);
    walls[3] = dCreateBox(0,X*CELL_TAM+CELL_TAM,WALL_TAM,WALL_HEIGHT);
    dGeomSetPosition(walls[0],(float)((X*CELL_TAM/2)-(CELL_TAM/2)),-(CELL_TAM/2)-WALL_TAM,WALL_HEIGHT/2);
    dGeomSetPosition(walls[1],(float)((X*CELL_TAM)-(CELL_TAM/2)),(float)((X*CELL_TAM/2)-(CELL_TAM/2)),WALL_HEIGHT/2);
    dGeomSetPosition(walls[2],(float)(-(CELL_TAM/2)-WALL_TAM),(float)((X*CELL_TAM/2)-(CELL_TAM/2)),WALL_HEIGHT/2);
    dGeomSetPosition(walls[3],(float)((X*CELL_TAM/2)-(CELL_TAM/2)),(X*CELL_TAM)-(CELL_TAM/2),WALL_HEIGHT/2);
    dSpaceAdd(space,walls[0]);
    dSpaceAdd(space,walls[1]);
    dSpaceAdd(space,walls[2]);
    dSpaceAdd(space,walls[3]);

    /// TRACK READING
    CFG_File config;

    int result = CFG_OpenFile("terreno.cfg", &config );

    if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
    {
        dsPrint("Unable to load file: %s\n", SDL_GetError());
        exit(1);
    }
    for ( CFG_StartGroupIteration(); !CFG_IsLastGroup(); CFG_SelectNextGroup() )
    {
        // Tenemos un pequeño problema con las X pero bueno
        // esta mas o menos arreglado
        dMatrix3 R;
        int i = CFG_ReadInt("x",0);
        int j = CFG_ReadInt("y",0);
        //int k = CFG_ReadInt("z",0);
        int rotation = CFG_ReadInt("rotation",0);
        string s_model = CFG_ReadText("model",(*track_map.begin()).first.c_str());
        Cell_Matrix[X-1-i][j].i_checkpoint = CFG_ReadBool("i_checkpoint",0);
        Cell_Matrix[X-1-i][j].start = CFG_ReadBool("start",0);

        Cell_Matrix[X-1-i][j].id = track_map[s_model]->id;
        Cell_Matrix[X-1-i][j].road_model = track_map[s_model]->roadmodel;
        Cell_Matrix[X-1-i][j].road_geom = dCreateTriMesh(space,track_map[s_model]->roadmesh,0,0,0);
        Cell_Matrix[X-1-i][j].name = s_model;

             if (rotation==180) dRFromAxisAndAngle(R,0,0,1,0);
        else if (rotation==270) dRFromAxisAndAngle(R,0,0,1,+M_PI/2);
        else if (rotation==0)   dRFromAxisAndAngle(R,0,0,1,+M_PI);
        else if (rotation==90)  dRFromAxisAndAngle(R,0,0,1,+(3*M_PI)/2);
        dGeomSetRotation(Cell_Matrix[X-1-i][j].road_geom,R);
        dGeomSetPosition (Cell_Matrix[X-1-i][j].road_geom,(X-i-1)*CELL_TAM,j*CELL_TAM,0.1);
    }
}

void Ground::LoadTextures()
{
    char fcadena[300];
    string ruta = getcwd(fcadena,300);

    for (map< string, TrackInfo* >::iterator itr = track_map.begin(), end = track_map.end();
         itr != end ; ++itr )
    {
        string ruta_nueva = ruta + "\\";
        ruta_nueva += DIR_TRACKS;
        ruta_nueva += "\\";
        ruta_nueva += (*itr).first;
        chdir(ruta_nueva.c_str());
        (*itr).second->roadmodel->reloadTextures();
        chdir(ruta.c_str());
    }
}

void Ground::Draw(int cell_x, int cell_y)
{
    int R = 5; //pinta 2R+1*2R+1 casillas
    int min_x = cell_x <  R  ? 0 : cell_x-R;
    int max_x = cell_x >= X-R ? X-1 : cell_x+R;
    int min_y = cell_y <  R  ? 0 : cell_y-R;
    int max_y = cell_y >= Y-R ? Y-1 : cell_y+R;

    for(int i = min_x; i <= max_x; i++)
    for(int j = min_y; j <= max_y; j++)
    {
      /// No es necesario dibujar cada sector, ground cubre esta necesidad
      //ground_Model->draw(3,dGeomGetPosition(Cell_Matrix[i][j].geomID),
      //                     dGeomGetRotation(Cell_Matrix[i][j].geomID));
      if (Cell_Matrix[i][j].road_model)
      {
          Cell_Matrix[i][j].road_model->draw(Cell_Matrix[i][j].id,
                                            dGeomGetPosition(Cell_Matrix[i][j].road_geom),
                                            dGeomGetRotation(Cell_Matrix[i][j].road_geom));
          dGeomEnable(Cell_Matrix[i][j].road_geom);
      }
      dGeomEnable(Cell_Matrix[i][j].geomID);
    }
    /// Draw the wall!

    dsSetColorAlpha (0,1,0,0.5);
    dReal sides0[3] = {X*CELL_TAM+CELL_TAM,WALL_TAM,WALL_HEIGHT};
    dReal sides1[3] = {WALL_TAM,X*CELL_TAM+CELL_TAM,WALL_HEIGHT};
    dsDrawBox(dGeomGetPosition(walls[0]),dGeomGetRotation(walls[0]),sides0);
    dsDrawBox(dGeomGetPosition(walls[1]),dGeomGetRotation(walls[1]),sides1);
    dsDrawBox(dGeomGetPosition(walls[2]),dGeomGetRotation(walls[2]),sides1);
    dsDrawBox(dGeomGetPosition(walls[3]),dGeomGetRotation(walls[3]),sides0);

    /// BackGround
    dsDrawSkyDome(dGeomGetPosition(Cell_Matrix[0][0].geomID), dGeomGetRotation(Cell_Matrix[0][0].geomID),0,1000);
    dsDrawFakeGround();
}
