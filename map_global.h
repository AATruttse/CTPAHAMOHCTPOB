#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

#include "common.h"

#define MAP_WIDTH   80
#define MAP_HEIGHT  25

#define MAP_X0  0
#define MAP_Y0  0

#define EXPLORED_FLAG   1
#define VISIBLE_FLAG    2

struct MapCell {
    enum E_CellType     type;
    enum E_CellHumidity hum;

    unsigned char       flags;
};

struct MapCell g_Map[MAP_HEIGHT][MAP_WIDTH];

void map_global_init();
void map_global_draw();
bool map_global_save(FILE *fptr);
bool map_global_load(FILE *fptr);
#endif // MAP_H_INCLUDED
