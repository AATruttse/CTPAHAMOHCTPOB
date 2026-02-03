#ifndef MAP_GEN_LOCAL_H_INCLUDED
#define MAP_GEN_LOCAL_H_INCLUDED

#include "rxi_map/src/map.h"

#include "common.h"
#include "map_local.h"

#define LOCAL_MAP_CONFIG_FILE "localmaps.cfg"

typedef void (*placeLocalFunction_t)(struct MapLocalCell*);

struct MapLocalSpotParams {
    size_t numSpots;
    placeLocalFunction_t placeFunction;
    unsigned int chance;
    size_t sizeMin;
    size_t sizeMax;
    size_t typesRemoved_size;
    enum E_LocalCellType *typesRemoved;
};

struct E_LocalCellTypeArray {
    enum E_LocalCellType* cellTypesArray;
    size_t                arraySize;
};

typedef map_t(struct E_LocalCellTypeArray)  map_ELCT_t;
typedef map_t(placeLocalFunction_t)         map_pPlaceLocalFunction_t;
typedef map_t(struct MapLocalSpotParams)    map_LocalSpotParams_t;
typedef map_t(map_LocalSpotParams_t)        map_map_LocalSpotParams_t;
typedef map_t(map_map_LocalSpotParams_t)    map_map_map_LocalSpotParams_t;

map_ELCT_t                      g_ELCTs;
map_pPlaceLocalFunction_t       g_PlaceLocalFunctions;
map_map_map_LocalSpotParams_t   g_MapLocalSpotParams;

void init_gen_map_local();
void apply_local_map_gen(struct MapCell *_pMapCell, struct MapLocal *_pLocalMap);
#endif // MAP_GEN_LOCAL_H_INCLUDED
