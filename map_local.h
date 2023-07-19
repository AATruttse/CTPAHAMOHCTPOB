#ifndef MAP_LOCAL_H_INCLUDED
#define MAP_LOCAL_H_INCLUDED

#include "common.h"
#include "map.h"

#define MAP_LOCAL_WIDTH   MAP_WIDTH
#define MAP_LOCAL_HEIGHT  MAP_HEIGHT

struct MapLocalCell {
    enum E_LocalCellType    type;

    unsigned char       flags;
};

struct MapLocal {
    bool                is_generated;

    size_t              x;
    size_t              y;

    struct MapLocalCell cells[MAP_LOCAL_HEIGHT][MAP_LOCAL_WIDTH];
};

struct MapLocal g_LocalMaps[MAP_HEIGHT][MAP_WIDTH];

void maps_local_init_all();
void map_local_init(struct MapCell *_pMapCell, struct MapLocal *_pLocalMap);
void map_local_draw(struct MapLocal *_pLocalMap);
bool maps_local_save_all(FILE *fptr);
bool maps_local_load_all(FILE *fptr);

#endif // MAP_LOCAL_H_INCLUDED
