/*
	Model.cpp

		Abstract base class for a model. The specific extended class will render the given model.

	Author:	Brett Porter
	Email: brettporter@yahoo.com
	Website: http://www.geocities.com/brettporter/
	Copyright (C)2000, Brett Porter. All Rights Reserved.

	This file may be used only as long as this copyright notice remains intact.
*/

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>													// Header File For The GLu32 Library
#include <GL/glaux.h>												    // Our Local Copy Of The Header File
#include <stdio.h>													// Header File For Standard Input/Output

#include <game/Model.h>
#include <game/MilkshapeModel.h>											// Header File For Milkshape File

/// //////// //////// //////// //////// //////// //////// //////// /////
/// //////// //////// //////// //////// //////// //////// //////// /////

AUX_RGBImageRec *LoadBMP(const char *Filename)						// Loads A Bitmap Image
{
	FILE *File=NULL;												// File Handle

	if (!Filename)													// Make Sure A Filename Was Given
	{
		return NULL;												// If Not Return NULL
	}

	File=fopen(Filename,"r");										// Check To See If The File Exists

	if (File)														// Does The File Exist?
	{
		fclose(File);												// Close The Handle
		return auxDIBImageLoad(Filename);							// Load The Bitmap And Return A Pointer
	}

	return NULL;													// If Load Failed Return NULL
}

GLuint LoadGLTexture( const char *filename )						// Load Bitmaps And Convert To Textures
{
	AUX_RGBImageRec *pImage;										// Create Storage Space For The Texture
	GLuint texture = 0;												// Texture ID

	pImage = LoadBMP( filename );									// Loads The Bitmap Specified By filename

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if ( pImage != NULL && pImage->data != NULL )					// If Texture Image Exists
	{
		glGenTextures(1, &texture);									// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, pImage->sizeX, pImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		free(pImage->data);											// Free The Texture Image Memory
		free(pImage);												// Free The Image Structure
	}

	return texture;													// Return The Status
}

static void setTransform (const float pos[3], const float R[12])
{
  GLfloat matrix[16];
  matrix[0]=R[0];
  matrix[1]=R[4];
  matrix[2]=R[8];
  matrix[3]=0;
  matrix[4]=R[1];
  matrix[5]=R[5];
  matrix[6]=R[9];
  matrix[7]=0;
  matrix[8]=R[2];
  matrix[9]=R[6];
  matrix[10]=R[10];
  matrix[11]=0;
  matrix[12]=pos[0];
  matrix[13]=pos[1];
  matrix[14]=pos[2];
  matrix[15]=1;
  glPushMatrix();
  glMultMatrixf (matrix);
}

/// //////// //////// //////// //////// //////// //////// //////// /////
/// //////// //////// //////// //////// //////// //////// //////// /////

Model::Model()
{
	m_numMeshes = 0;
	m_pMeshes = NULL;
	m_numMaterials = 0;
	m_pMaterials = NULL;
	m_numTriangles = 0;
	m_pTriangles = NULL;
	m_numVertices = 0;
	m_pVertices = NULL;
}

Model::~Model()
{
	int i;
	for ( i = 0; i < m_numMeshes; i++ )
		delete[] m_pMeshes[i].m_pTriangleIndices;
	for ( i = 0; i < m_numMaterials; i++ )
		delete[] m_pMaterials[i].m_pTextureFilename;

	m_numMeshes = 0;
	if ( m_pMeshes != NULL )
	{
		delete[] m_pMeshes;
		m_pMeshes = NULL;
	}

	m_numMaterials = 0;
	if ( m_pMaterials != NULL )
	{
		delete[] m_pMaterials;
		m_pMaterials = NULL;
	}

	m_numTriangles = 0;
	if ( m_pTriangles != NULL )
	{
		delete[] m_pTriangles;
		m_pTriangles = NULL;
	}

	m_numVertices = 0;
	if ( m_pVertices != NULL )
	{
		delete[] m_pVertices;
		m_pVertices = NULL;
	}
}

void Model::draw(int id,const dVector3 posicion, const float R[12])
{
    if (glIsList(id)){
        setTransform(posicion,R);
        glCallList(id);
    }
    else
        ModelDrawList(id);
}

void Model::reloadTextures()
{
	for ( int i = 0; i < m_numMaterials; i++ )
		if ( strlen( m_pMaterials[i].m_pTextureFilename ) > 0 )
			m_pMaterials[i].m_texture = LoadGLTexture( m_pMaterials[i].m_pTextureFilename );
		else
			m_pMaterials[i].m_texture = 0;
}

void Model::ModelDrawList(int id)
{
    glNewList(id,GL_COMPILE_AND_EXECUTE);
        GLboolean texEnabled = glIsEnabled( GL_TEXTURE_2D );
        // Draw by group
        for ( int i = 0; i < m_numMeshes; i++ )
        {
            int materialIndex = m_pMeshes[i].m_materialIndex;
            if ( materialIndex >= 0 )
            {
                glMaterialfv( GL_FRONT, GL_AMBIENT, m_pMaterials[materialIndex].m_ambient );
                glMaterialfv( GL_FRONT, GL_DIFFUSE, m_pMaterials[materialIndex].m_diffuse );
                glMaterialfv( GL_FRONT, GL_SPECULAR, m_pMaterials[materialIndex].m_specular );
                glMaterialfv( GL_FRONT, GL_EMISSION, m_pMaterials[materialIndex].m_emissive );
                glMaterialf( GL_FRONT, GL_SHININESS, m_pMaterials[materialIndex].m_shininess );

                if ( m_pMaterials[materialIndex].m_texture > 0 )
                {
                    glBindTexture( GL_TEXTURE_2D, m_pMaterials[materialIndex].m_texture );
                    glEnable( GL_TEXTURE_2D );
                }
                else
                    glDisable( GL_TEXTURE_2D );
            }
            else
            {
                // Material properties?
                glDisable( GL_TEXTURE_2D );
            }

            glPushMatrix();
            glBegin( GL_TRIANGLES );
            { //la malla puede tener muchos triangulos
                for ( int j = 0; j < m_pMeshes[i].m_numTriangles; j++ )
                {// para cada triangulo vemos su indice
                    int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
                    const Triangle* pTri = &m_pTriangles[triangleIndex];

                    for ( int k = 0; k < 3; k++ )
                    { //dibujamos la figura por triangulos
                        int index = pTri->m_vertexIndices[k];

                        glNormal3fv( pTri->m_vertexNormals[k] );
                        glTexCoord2f( pTri->m_s[k], pTri->m_t[k] );
                        glVertex3fv( m_pVertices[index].m_location );
                    }
                }
            }
            glEnd();
        }
        glPopMatrix();

        if ( texEnabled )
            glEnable( GL_TEXTURE_2D );
        else
            glDisable( GL_TEXTURE_2D );
    glEndList();
}

/* Las siguientes funciones solo se ejecutaran, en principio, en los contructores
   de las formas fisicas en el juego (trimesh de ODE) con lo que las calculare
   suponiendo un unico nivel de malla y nos las guardare en ninguna variable */

int
Model::getIndexCount()
{
    return(m_numMeshes*m_numTriangles*3);
}

int*
Model::getIndices()
{
    int * Indices;
    int numIndices = getIndexCount();
    Indices = new int[numIndices];
    int contador=0;

    for ( int i = 0; i < m_numMeshes; i++ )
        for ( int j = 0; j < m_pMeshes[i].m_numTriangles; j++ )
        {
                int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
				const Triangle* pTri = &m_pTriangles[triangleIndex];
				for ( int k = 0; k < 3; k++ )
				{
                    Indices[contador] = pTri->m_vertexIndices[k];
                    contador++;
				}
        }
    return( Indices );
}

int
Model::getVertexCount()
{
    return( m_numVertices );
}

dVector3*
Model::getVertices()
{
    dVector3 * Vertices;
    Vertices = new dVector3[m_numVertices];

    for(int i=0; i < m_numVertices; i++)
        for(int k=0; k<3;k++)
            Vertices[i][k]=m_pVertices[i].m_location[k];
    return( Vertices );
}
