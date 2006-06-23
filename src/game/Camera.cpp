
#include <game/Camera.h>

/// //////////////////////////////////////////////////////////////////////// ///
/// STATICS MEMBERS DEFINITION

dReal Camera::seguir[3];
bool Camera::move = 0;
bool Camera::interior = 0;

float Camera::distance_actual = 0;
float Camera::distance_view = 5.0;
float Camera::camera_height = 3.5;
float Camera::cam_speed = 0.1;

/// //////////////////////////////////////////////////////////////////////// ///
/// CAMERA FUNCTIONS

void Camera::Update()
{


    /// PARA LAS CAMARAS DE EXTERIOR
    if (!interior)
    {
        //Camera Stuff
        const dReal * pos = dBodyGetPosition(Car::Chassis_BodyID);
        dVector3 pos_Camara;
        dVector3 dir_Camara;
        dsGetViewpoint(pos_Camara,dir_Camara);
        dVector3 vector;
        for(int i=0; i < 3 ; i++)
            vector[i] = pos[i]-pos_Camara[i];

        distance_actual = sqrt(vector[0]*vector[0]+vector[1]*vector[1]+vector[2]*vector[2]);
        if (distance_actual >= distance_view){
             move=1;
             for(int j=0;j<3;j++) seguir[j]=pos[j];
        }

        if (move){
            for(int p = 0 ; p < 2; p++)
            {
                if (seguir[p] < 0)
                    if (pos_Camara[p] < 0)
                        (pos_Camara[p]<seguir[p])?pos_Camara[p]+=cam_speed:pos_Camara[p]-=cam_speed; //C -4 //S -1
                    else
                        (pos_Camara[p]>seguir[p])?pos_Camara[p]-=cam_speed:pos_Camara[p]+=cam_speed; //C 1 //S -4
                else
                    if (pos_Camara[p] < 0)
                        (pos_Camara[p]<seguir[p])?pos_Camara[p]+=cam_speed:pos_Camara[p]-=cam_speed; //C -4 //S 1
                    else
                        (pos_Camara[p]>seguir[p])?pos_Camara[p]-=cam_speed:pos_Camara[p]+=cam_speed; //C 4 //S 1
            }
            if ((pos[0]-pos_Camara[0]) < cam_speed || (pos[0]-pos_Camara[0]) > cam_speed)
                if ((pos[1]-pos_Camara[1]) < cam_speed || (pos[1]-pos_Camara[1]) > cam_speed)
                    move=0;

            pos_Camara[2] = camera_height;

            dsSetViewpoint(pos_Camara,NULL);
        }
        dsSetCameraLookAt(pos[1], pos[2],pos[0], pos_Camara[0],pos_Camara[1],pos_Camara[2]);
    }
    /// PARA LA CAMARA SUBJETIVA
    else
    {
        const dReal * pos_box = dGeomGetPosition(Car::camera_box);
        dVector3 pos_interior = {pos_box[0],pos_box[1],pos_box[2]+0.3};
        dsSetViewpoint(pos_interior,NULL);
    }
}

/// //////////////////////////////////////////////////////////////////////// ///
/// CHANGE THE CAMERA

void Camera::SetView(int view)
{
    switch (view)
    {
        case 0:
          distance_view = 5.0;
          camera_height = 3.5;
          cam_speed = 0.1;
          interior = 0;
          break;
        case 1:
          distance_view = 10.0;
          camera_height = 7.0;
          cam_speed = 0.1;
          interior = 0;
          break;
        case 2:
          distance_view = 2.0;
          camera_height = 1.0;
          cam_speed = 0.1;
          interior = 0;
          break;
        case 3:
          distance_view = 16.0;
          camera_height = 15.0;
          cam_speed = 0.12;
          interior = 0;
          break;
        case 4:
          interior = 1;
          break;
    }

    move = true;
}
