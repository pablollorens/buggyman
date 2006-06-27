#include <cell3d.h>

Cell3D::Cell3D()
{
    connectors.resize(4,CONN_NONE);
    track = NULL;
}

Cell3D::Cell3D(Track* some_track, int north, int west, int south, int east)
{
    connectors.resize(4,CONN_NONE);
    track = some_track;
    if(track)
    {
        connectors[CELL3D_NORTH] = north;
        connectors[CELL3D_WEST] = west;
        connectors[CELL3D_SOUTH] = south;
        connectors[CELL3D_EAST] = east;
    }
}

Cell3D::Cell3D(const Cell3D & some)
{
    (*this)=some;
}

Cell3D::~Cell3D()
{
    track = NULL;
}

Cell3D &
Cell3D::operator=(const Cell3D & some)
{
	if(this != &some)
	{
        connectors = some.connectors;
        track = some.track;
	}
    return(*this);
}

bool
Cell3D::operator==(const Cell3D & some)
{
    bool equal = 1;
    equal &= connectors == some.connectors;
    equal &= track == some.track;
    return equal;
}

bool
Cell3D::Is_totally_none()
{
    bool value = 1;
    for(int i=0; i<4; i++)
        value += connectors[i];
    return (!value);
}

int
Cell3D::Validate_Connector(int some_conn)
{
    some_conn = CONN_MIN_CONNECTOR <= some_conn && some_conn <= CONN_MAX_CONNECTOR ?
                some_conn :
                CONN_NONE;
    return some_conn;
}

void
Cell3D::Connectors_Rotation()
{
    int aux = connectors[3];
    connectors[3] = connectors[2];
    connectors[2] = connectors[1];
    connectors[1] = connectors[0];
    connectors[0] = aux;
}
