#include <game/Ground.h>

/// //////////////////////////////////////////////////////////////////////// ///
/// STATICS MEMBERS DEFINITION

Ground_Cell Ground::Cell_Matrix[X][Y];

Model *Ground::ground_Model;
Model *Ground::ModelosMilky[K_MODEL];
dTriMeshDataID Ground::MeshesData[K_MODEL];

/// //////////////////////////////////////////////////////////////////////// ///
/// CONSTRUCTOR

Ground::Ground(dWorldID world, dSpaceID space)
{
    int i,j,rotation,model;
    string s_model;

    // Cargamos todos los modelos de las pistas
    ground_Model = new MilkshapeModel(); // solo para dibujado del suelo
    ground_Model->loadModelData("data/sector.ms3d");

    ModelosMilky[BANK_A] = new MilkshapeModel();
    ModelosMilky[BANK_B] = new MilkshapeModel();
    ModelosMilky[BANKED_STANDARD_CURVE] = new MilkshapeModel();
    ModelosMilky[BANKED_STRAIGHT] = new MilkshapeModel();
    ModelosMilky[BUILDING1] = new MilkshapeModel();
    ModelosMilky[STRAIGHT] = new MilkshapeModel();
    ModelosMilky[INNER_CURVE] = new MilkshapeModel();
    ModelosMilky[STANDARD_CURVE] = new MilkshapeModel();
    ModelosMilky[CROSS] = new MilkshapeModel();
    ModelosMilky[RAMP_A] = new MilkshapeModel();
    ModelosMilky[RAMP_B] = new MilkshapeModel();
    ModelosMilky[LOOP] = new MilkshapeModel();
    ModelosMilky[TREE] = new MilkshapeModel();


    ModelosMilky[BANK_A]->loadModelData("tracks/bank_a/model.ms3d");
    ModelosMilky[BANK_B]->loadModelData("tracks/bank_b/model.ms3d");
    ModelosMilky[BANKED_STANDARD_CURVE]->loadModelData("tracks/banked_standard_curve/model.ms3d");
    ModelosMilky[BANKED_STRAIGHT]->loadModelData("tracks/banked_straight/model.ms3d");
    ModelosMilky[BUILDING1]->loadModelData("tracks/building1/model.ms3d");
    ModelosMilky[STRAIGHT]->loadModelData("tracks/straight/model.ms3d");
    ModelosMilky[INNER_CURVE]->loadModelData("tracks/inner_curve/model.ms3d");
    ModelosMilky[STANDARD_CURVE]->loadModelData("tracks/standard_curve/model.ms3d");
    ModelosMilky[CROSS]->loadModelData("tracks/cross/model.ms3d");
    ModelosMilky[RAMP_A]->loadModelData("tracks/ramp_a/model.ms3d");
    ModelosMilky[RAMP_B]->loadModelData("tracks/ramp_b/model.ms3d");
    ModelosMilky[LOOP]->loadModelData("tracks/loop/model.ms3d");
    ModelosMilky[TREE]->loadModelData("tracks/tree/model.ms3d");

    // Creamos ahora la información de las mesh de estos modelos de pista
    for(int p=0; p<K_MODEL ; p++)
    {
        MeshesData[p] = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSimple(MeshesData[p],(dReal*)ModelosMilky[p]->getVertices(),
                                                   ModelosMilky[p]->getVertexCount(),
                                                   ModelosMilky[p]->getIndices(),
                                                   ModelosMilky[p]->getIndexCount());
    }

    for(i=0; i<X ; i++)
        for(j=0; j<Y; j++){
            Cell_Matrix[i][j].arbol = 0;
            Cell_Matrix[i][j].Checkpoint = 0;
            Cell_Matrix[i][j].num_modelo = 0;
            Cell_Matrix[i][j].modelo = NULL;
            Cell_Matrix[i][j].geomID = dCreateBox(0,7,7,0.1);
            dGeomSetPosition(Cell_Matrix[i][j].geomID,i*7,j*7,0);
            dSpaceAdd(space,Cell_Matrix[i][j].geomID);
        }

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
        map< string,int > ids;
        ids["BANK_A"] = BANK_A;
        ids["BANK_B"] = BANK_B;
        ids["BANKED_STANDARD_CURVE"] = BANKED_STANDARD_CURVE;
        ids["BANKED_STRAIGHT"] = BANKED_STRAIGHT;
        ids["BUILDING1"] = BUILDING1;
        ids["STRAIGHT"] = STRAIGHT;
        ids["INNER_CURVE"] = INNER_CURVE;
        ids["STANDARD_CURVE"] = STANDARD_CURVE;
        ids["CROSS"] = CROSS;
        ids["RAMP_A"] = RAMP_A;
        ids["RAMP_B"] = RAMP_B;
        ids["LOOP"] = LOOP;
        ids["TREE"] = TREE;

        // Tenemos un pequeño problema con las X pero bueno
        // esta mas o menos arreglado
        dMatrix3 R;
        i = CFG_ReadInt("x",0);
        j = CFG_ReadInt("y",0);
        rotation = CFG_ReadInt("rotation",0);
        s_model = CFG_ReadText("model","RECTA");
        model = ids[s_model];
        Cell_Matrix[X-1-i][j].Checkpoint = 1;
        Cell_Matrix[X-1-i][j].num_modelo = model;
        Cell_Matrix[X-1-i][j].modelo = ModelosMilky[model];
        Cell_Matrix[X-1-i][j].geomPista = dCreateTriMesh(space,MeshesData[model],0,0,0);

        if (rotation==180) dRFromAxisAndAngle(R,0,0,1,0);
        if (rotation==270)  dRFromAxisAndAngle(R,0,0,1,+M_PI/2);
        if (rotation==0)  dRFromAxisAndAngle(R,0,0,1,+M_PI);
        if (rotation==90)  dRFromAxisAndAngle(R,0,0,1,+(3*M_PI)/2);
        dGeomSetRotation(Cell_Matrix[X-1-i][j].geomPista,R);
        dGeomSetPosition (Cell_Matrix[X-1-i][j].geomPista,(X-i-1)*7,j*7,0.1);
    }
}

void Ground::LoadTextures()
{
    ground_Model->reloadTextures();
    for(int p=0; p<K_MODEL; p++)
        ModelosMilky[p]->reloadTextures();
}

void Ground::Draw(int cell_x, int cell_y)
{
  int R = 12; //pinta 2R+1*2R+1 casillas
  int min_x = cell_x <  R  ? 0 : cell_x-R;
  int max_x = cell_x >= X-R ? X-1 : cell_x+R;
  int min_y = cell_y <  R  ? 0 : cell_y-R;
  int max_y = cell_y >= Y-R ? Y-1 : cell_y+R;

  for(int i=min_x; i<=max_x; i++)
    for(int j=min_y; j<=max_y; j++)
    {
      ground_Model->draw(3,dGeomGetPosition(Cell_Matrix[i][j].geomID),
                           dGeomGetRotation(Cell_Matrix[i][j].geomID));
      if (Cell_Matrix[i][j].modelo)
      {
          Cell_Matrix[i][j].modelo->draw(Cell_Matrix[i][j].num_modelo+10,
                                         dGeomGetPosition(Cell_Matrix[i][j].geomPista),
                                         dGeomGetRotation(Cell_Matrix[i][j].geomPista));
          dGeomEnable(Cell_Matrix[i][j].geomPista);
      }
      dGeomEnable(Cell_Matrix[i][j].geomID);
    }
}
