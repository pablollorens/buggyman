
# include <game/Ground_Cell.h>

Ground_Cell::Ground_Cell()
{
    i_checkpoint = 0;
    id = 0;
    start = 0;
    road_model = NULL;
    road_geom = NULL;
    geomID = dCreateBox(0,CELL_TAM,CELL_TAM,0.1);
}
