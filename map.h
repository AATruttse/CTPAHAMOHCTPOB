#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <stdbool.h>

#include "common.h"

#define MAP_WIDTH   80
#define MAP_HEIGHT  25

#define MAP_X0  0
#define MAP_Y0  0

struct MapCell {
    enum E_CellType     type;
    enum E_CellHumidity hum;

    bool                is_visible;
    bool                is_explored;
};

struct MapCell g_Map[MAP_HEIGHT][MAP_WIDTH];

void map_init();
void map_draw();
#endif // MAP_H_INCLUDED
