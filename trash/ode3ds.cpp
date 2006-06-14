/* Libreria Ode3DS por Pablo Llorens Lledo */
/* *************************************** */
/*
    La funcion get3DS modifica lo siguiente:
    -----------------------------------------------------------------
    - bounding_values con la info del 3DS.
    - IndexCount y VertexCount con el numero de vertices del objeto.

    La funcion info3DS modifica lo siguiente:
    -----------------------------------------------------------------
    - Indices con los indices de las caras (cada 3 indices se dibuja una cara).
    - Vertices con todos los valores de los vertices almacenados en una matriz.
*/
#include <game/ode3ds.h>

Ode3DS::Ode3DS(const char * filename,char* somename){
        file=lib3ds_file_load(filename);
        Get3DS(); // cargamos todos los vertices y los indices del objeto
        VertexCount=0;
        IndexCount=0;
        cont_indi=0;
        cont_vert=0;
        nombre=somename;
}

Ode3DS::~Ode3DS(){}

bool
Ode3DS::operator==(Ode3DS& some)
{
        return(!strcmp(nombre,some.nombre));
}

bool
Ode3DS::operator==(const char* some)
{
        return(!strcmp(nombre,some));
}

bool
Ode3DS::operator<(Ode3DS& some)
{
        return(strcmp(nombre,some.nombre)<0);
}

void
Ode3DS::info3ds(Lib3dsNode *father,const char * nodo)
{
    ASSERT(file){
        Lib3dsNode *child;
        for (child=father->childs; child!=0; child=child->next) {
            info3ds(child);
        }
        // codigo para sacar x,y,z maximas y minimas
        Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file, father->name);
        unsigned p;
        for (p=0; p<mesh->faces; ++p) {
          Lib3dsFace *f=&mesh->faceL[p];
          //Conseguimos los valores minimos y maximos para cada comp.
          // aabb -> maximos y minimos de cada comp. de la face tratada
          float aabb[6];
          for(int i;i<6;i++) aabb[i]=0; //Inicializamos aabb
          //Establecemos los valores minimos para aabb (los del primer vertice)
          for(int j=0; j < 3 ;j++){
              aabb[j*2] = mesh->pointL[f->points[0]].pos[j];
              aabb[j*2+1] = mesh->pointL[f->points[0]].pos[j];
          }
          // Miramos los dos vertices restantes
          for(int vert=1; vert<3; ++vert){
            for(int j=0; j<3; ++j){
                // comparamos si es mayor o menor y sustituimos si es necesario
                if (mesh->pointL[f->points[vert]].pos[j] < aabb[j*2])
                    aabb[j*2] = mesh->pointL[f->points[vert]].pos[j];
                if (mesh->pointL[f->points[vert]].pos[j] > aabb[j*2+1])
                    aabb[j*2+1] = mesh->pointL[f->points[vert]].pos[j];
            }
          }
          // recorremos todos los vertices, guardandolos en "Vertices"
          // y añadiendo los indices en "Indices"
          float x,y,z = 0;
          for(int vert=0; vert<3; ++vert){
              x = mesh->pointL[f->points[vert]].pos[0];
              y = mesh->pointL[f->points[vert]].pos[1];
              z = mesh->pointL[f->points[vert]].pos[2];
              Vertices[cont_vert][0] = x;
              Vertices[cont_vert][1] = y;
              Vertices[cont_vert][2] = z;
              Indices[cont_indi] = cont_vert;
              cont_indi++;
              cont_vert++;
          }
          // Hasta aqui ya tenemos nuestros 3 vertices en Vertices y los Indices de la face
          // Comparamos con las bounding values globales
          for (int i=0; i<3; i++){
              if (aabb[i*2+1] > bounding_values[i*2+1])
                bounding_values[i*2+1] = aabb[i*2+1];
              if (aabb[i*2] < bounding_values[i*2])
                bounding_values[i*2] = aabb[i*2];
          }
        } // fin para mesh
    } // fin de nodo father
}

void
Ode3DS::Get3DS(const char * nodo)
{
    VertexCount = 0;
    Lib3dsNode *p;
    cont_vert=cont_indi=0; // Esto lo hacemos para el buen funcionamiento de info3DS
    for(int i=0;i<6;i++) bounding_values[i]=0;  // Limpiamos las bounding_values
    ASSERT(file);{for (p=file->nodes; p!=0; p=p->next) info3ds(p,nodo);}
    IndexCount = VertexCount = cont_vert;
}

void
Ode3DS::render_node(Lib3dsNode *node,const char* nodo)
{
  ASSERT(file);

  {
    Lib3dsNode *p;
    for (p=node->childs; p!=0; p=p->next) {
      render_node(p);
    }
  }
  if (node->type==LIB3DS_OBJECT_NODE) {
    if (strcmp(node->name,"$$$DUMMY")==0) {
      return;
    }

    if (!node->user.d) {
      Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file, node->name);
      ASSERT(mesh);
      if (!mesh) {
        return;
      }

      node->user.d=glGenLists(1);
      glNewList(node->user.d, GL_COMPILE);

      {
        unsigned p;
        Lib3dsVector *normalL=(Lib3dsVector*)malloc(3*sizeof(Lib3dsVector)*mesh->faces);

        {
          Lib3dsMatrix M;
          lib3ds_matrix_copy(M, mesh->matrix);
          lib3ds_matrix_inv(M);
          glMultMatrixf(&M[0][0]);
        }
        lib3ds_mesh_calculate_normals(mesh, normalL);

        for (p=0; p<mesh->faces; ++p) {
          Lib3dsFace *f=&mesh->faceL[p];
          Lib3dsMaterial *mat=0;
          if (f->material[0]) {
            mat=lib3ds_file_material_by_name(file, f->material);
          }

          if (mat) {
            static GLfloat a[4]={0,0,0,1};
            float s;
            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
            s = pow(2, 10.0*mat->shininess);
            if (s>128.0) {
              s=128.0;
            }
            glMaterialf(GL_FRONT, GL_SHININESS, s);
          }
          else {
            Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
            Lib3dsRgba d={89, 152, 0.8, 1.0};
            Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
            glMaterialfv(GL_FRONT, GL_SPECULAR, s);
          }
          {
            glBegin(GL_TRIANGLES);
            for(int i = 0;i < 3;i++)
            {
                glNormal3fv(normalL[3*p*i]); //set normal vector of that point
                glVertex3fv(mesh->pointL[f->points[i]].pos); //Draw point of the mesh
            }
            glEnd();
          }
        }
        free(normalL);
      }
      glEndList();
    }
    if (node->user.d) {
      Lib3dsObjectData *d;

      glPushMatrix();
      d=&node->data.object;
      glMultMatrixf(&node->matrix[0][0]);
      glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
      glCallList(node->user.d);
      /*glutSolidSphere(50.0, 20,20);*/
      glPopMatrix();
    }
  }
}

//Función que cargara un archivo 3ds definido en filename y lo mostrara con OGL
void
Ode3DS::Draw3DS(const char* nodo)
{
//    dMatrix3 R;
//    const float pos[3] = {0,0,0};
//    dVector3 bbsides;
//    for (int h=0; h<3; h++) bbsides[h] = bounding_values[h*2+1] - bounding_values[h*2];
//    dRFromAxisAndAngle (R,0,0,1,0);
//    dsDraw3DS (pos,R,bbsides,"terreno.3ds");

    if (file){
        Lib3dsNode *p;
        for (p=file->nodes; p!=0; p=p->next) {
          render_node(p,nodo);
        }
    }
}
