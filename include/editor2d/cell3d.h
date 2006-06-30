#ifndef CELL3D_H
#define CELL3D_H

//Cell size in pixels
#define CELL_X 48
#define CELL_Y 48
#define CELL_Z 48

#define CELL3D_NORTH    0
#define CELL3D_WEST     1
#define CELL3D_SOUTH    2
#define CELL3D_EAST     3

//Connector types for to join tracks.
//The undeclared edges will be considered like NONE connectors.
#define CONN_NONE     0       //null connector
#define CONN_PLANE    1       //the standart connector
#define CONN_FREEWAY  2       //wide road
#define CONN_BANKED_A 3       //for inclined roads, left side
#define CONN_BANKED_B 4       //for inclined roads, right side
#define CONN_TUBE     5       //for tubural roads
#define CONN_BRIDGE   6       //elevated roads

#define CONN_MIN_CONNECTOR 0
#define CONN_MAX_CONNECTOR 6
using namespace std;

class Grid3D;
class Track;
#include <string>
#include <vector>
#include "point3d.h"
#include "utils.h"
class Cell3D
{
    public:
        Cell3D();
        Cell3D(Track* some_track, int north = CONN_NONE, int east = CONN_NONE, int south = CONN_NONE, int west = CONN_NONE);
        Cell3D(const Cell3D & some);
        ~Cell3D();
        Cell3D & operator=(const Cell3D & some);
        bool operator==(const Cell3D & some);
        inline int Get_North() {return connectors[CELL3D_NORTH];}
        inline int Get_East() {return connectors[CELL3D_EAST];}
        inline int Get_South() {return connectors[CELL3D_SOUTH];}
        inline int Get_West() {return connectors[CELL3D_WEST];}
        inline void Set_North(int value) {connectors[CELL3D_NORTH] = Validate_Connector(value);}
        inline void Set_East(int value) {connectors[CELL3D_EAST] = Validate_Connector(value);}
        inline void Set_South(int value) {connectors[CELL3D_SOUTH] = Validate_Connector(value);}
        inline void Set_West(int value) {connectors[CELL3D_WEST] = Validate_Connector(value);}
        inline void Set_North(const char* value) {connectors[CELL3D_NORTH] = Validate_Connector(value);}
        inline void Set_East(const char* value) {connectors[CELL3D_EAST] = Validate_Connector(value);}
        inline void Set_South(const char* value) {connectors[CELL3D_SOUTH] = Validate_Connector(value);}
        inline void Set_West(const char* value) {connectors[CELL3D_WEST] = Validate_Connector(value);}
        inline Track* Get_Track() {return track;}
        inline void Set_Track(Track* some_track) {track = some_track;}
        bool Is_totally_none();
        int Validate_Connector(int some_conn);
        int Validate_Connector(const char* some_conn);
        void Connectors_Rotation();

    private:
        vector<int> connectors;
        Track* track;
};

#endif // CELL3D_H
