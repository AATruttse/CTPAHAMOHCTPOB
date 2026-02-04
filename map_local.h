#ifndef MAP_LOCAL_H_INCLUDED
#define MAP_LOCAL_H_INCLUDED

#include "common.h"
#include "map_global.h"

#define MAP_LOCAL_WIDTH   100
#define MAP_LOCAL_HEIGHT  100

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

/* Viewport origin when drawing local map (hero-centered until near border) */
extern int g_LocalViewX0;
extern int g_LocalViewY0;

void maps_local_init_all();
void map_local_init(struct MapCell *_pMapCell, struct MapLocal *_pLocalMap);
void map_local_draw(struct MapLocal *_pLocalMap);

size_t local_map_place_spot(struct MapLocal *_pLocalMap,
    void (*_placeFunction)(struct MapLocalCell*),
    unsigned int _chance, size_t _sizeMin, size_t _sizeMax,
    size_t _typesRemoved_size, enum E_LocalCellType *_typesRemoved);
bool maps_local_save_all(FILE *fptr);
bool maps_local_load_all(FILE *fptr);

/* Save/load a single local map to/from the save directory (e.g. local_0_0.dat). */
bool map_local_save_to_file(size_t map_y, size_t map_x);
bool map_local_load_from_file(size_t map_y, size_t map_x);

#endif // MAP_LOCAL_H_INCLUDED
