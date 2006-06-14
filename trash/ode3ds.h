#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include <string>

// includes correspondientes a Lib3DS
#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/mesh.h>
#include <lib3ds/matrix.h>
#include <lib3ds/material.h>
#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef _LIBRARY_JRV_
#define _LIBRARY_JRV_

#define MAX 20000
#define DEFNODE "__ALL__"
#define DEFFILE "terreno.3ds"
#define DEFNAME "terreno"

class Ode3DS
{
        public:
            Ode3DS(const char * filename=DEFFILE,char* somename=DEFNAME);
            ~Ode3DS();
            void Get3DS(const char * nodo=DEFNODE);
            void Draw3DS(const char * nodo=DEFNODE);
            inline int getVertexCount(){ return VertexCount; };
            inline int getIndexCount(){ return IndexCount; };
            inline dVector3 * getVertices(){ return Vertices; };
            inline float * getBounding(){ return bounding_values; };
            inline int * getIndices(){ return Indices; };
            bool operator==(Ode3DS& some);
            bool operator==(const char* some);
            bool operator<(Ode3DS& some);

        private:
            float bounding_values[6];
            int VertexCount;
            int IndexCount;
            int cont_vert; // para llevar la cuenta de los vertices
            int cont_indi; // para llevar la cuenta de los indices
            char* nombre;

            dVector3 Vertices[MAX];
            int Indices[MAX];
            Lib3dsFile *file;
            void info3ds(Lib3dsNode *father,const char * nodo=DEFNODE);
            void render_node(Lib3dsNode *node,const char * nodo=DEFNODE);
};

#endif
