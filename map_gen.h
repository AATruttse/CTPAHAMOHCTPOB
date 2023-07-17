#ifndef MAP_GEN_H_INCLUDED
#define MAP_GEN_H_INCLUDED

#include "common.h"
#include "map.h"

enum E_CellType plains[1] = {ECT_PLAIN};
enum E_CellType forests[1] = {ECT_FOREST};
enum E_CellType hills[1] = {ECT_HILL};
enum E_CellType lakes_rivers[2] = {ECT_LAKE, ECT_RIVER};
enum E_CellType not_mountains[4] = {ECT_PLAIN, ECT_HILL, ECT_FOREST, ECT_LOWLAND};
enum E_CellType dry_types[3] = {ECT_PLAIN, ECT_HILL, ECT_LOWLAND};
enum E_CellType swamp_types[3] = {ECT_PLAIN, ECT_FOREST, ECT_LOWLAND};

enum E_CellHumidity normal[1] = {ECH_NORMAL};
enum E_CellHumidity all_humidities[5] = {ECH_NORMAL, ECH_DRY, ECH_SWAMP, ECH_WATER, ECH_SNOW};
enum E_CellHumidity all_waters[1] = {ECH_WATER};

struct MapSpotParams {
    size_t numSpots;
    void (*placeFunction)(struct MapCell*);
    unsigned int chance;
    size_t sizeMin;
    size_t sizeMax;
    size_t typesRemoved_size;
    enum E_CellType *typesRemoved;
    size_t humRemoved_size;
    enum E_CellHumidity *humRemoved;
};

void place_type(struct MapCell* _pCell, enum E_CellType _type) { (*_pCell).type = _type; }
void place_humidity(struct MapCell* _pCell, enum E_CellHumidity _hum) { (*_pCell).hum = _hum; }

void place_forest(struct MapCell* _pCell)   { place_type(_pCell, ECT_FOREST); }
void place_hill(struct MapCell* _pCell)     { place_type(_pCell, ECT_HILL); }
void place_lowland(struct MapCell* _pCell)  { place_type(_pCell, ECT_LOWLAND); }
void place_deadfall(struct MapCell* _pCell) { place_type(_pCell, ECT_DEADFALL); }
void place_dry(struct MapCell* _pCell)      { place_humidity(_pCell, ECH_DRY); }
void place_swamp(struct MapCell* _pCell)    { place_humidity(_pCell, ECH_SWAMP); }
void place_mountain(struct MapCell* _pCell) { place_type(_pCell, ECT_MOUNTAIN); place_humidity(_pCell, ECH_SNOW); }
void place_lake(struct MapCell* _pCell)     { place_type(_pCell, ECT_LAKE); place_humidity(_pCell, ECH_WATER); }
void place_shoal(struct MapCell* _pCell)    { place_type(_pCell, ECT_SHOAL); place_humidity(_pCell, ECH_WATER); }

struct MapSpotParams mapSpots[9] = {
    // forests
    { numSpots: 16, placeFunction: place_forest, chance: 67, sizeMin: 10, sizeMax: 100,
        typesRemoved_size:  sizeof(plains) / sizeof(enum E_CellType), typesRemoved: plains,
        humRemoved_size: sizeof(all_humidities) / sizeof(enum E_CellHumidity), humRemoved: all_humidities },
    // hills
    { numSpots: 8, placeFunction: place_hill, chance: 75, sizeMin: 10, sizeMax: 50,
        typesRemoved_size:  sizeof(plains) / sizeof(enum E_CellType), typesRemoved: plains,
        humRemoved_size: sizeof(all_humidities) / sizeof(enum E_CellHumidity), humRemoved: all_humidities },
    // lowlands
    {numSpots: 4, placeFunction: place_lowland, chance: 75, sizeMin: 10, sizeMax: 20,
        typesRemoved_size:  sizeof(plains) / sizeof(enum E_CellType), typesRemoved: plains,
        humRemoved_size: sizeof(all_humidities) / sizeof(enum E_CellHumidity), humRemoved: all_humidities },
    // mountains
    {numSpots: 5, placeFunction: place_mountain, chance: 75, sizeMin: 2, sizeMax: 10,
        typesRemoved_size:  sizeof(hills) / sizeof(enum E_CellType), typesRemoved: hills,
        humRemoved_size: sizeof(all_humidities) / sizeof(enum E_CellHumidity), humRemoved: all_humidities },
    // deadfalls
    {numSpots: 5, placeFunction: place_deadfall, chance: 75, sizeMin: 2, sizeMax: 10,
        typesRemoved_size:  sizeof(forests) / sizeof(enum E_CellType), typesRemoved: forests,
        humRemoved_size: sizeof(all_humidities) / sizeof(enum E_CellHumidity), humRemoved: all_humidities },
    // lakes
    {numSpots: 6, placeFunction: place_lake, chance: 67, sizeMin: 1, sizeMax: 20,
        typesRemoved_size:  sizeof(not_mountains) / sizeof(enum E_CellType), typesRemoved: not_mountains,
        humRemoved_size: sizeof(all_humidities) / sizeof(enum E_CellHumidity),humRemoved:  all_humidities },
    // shoals
    {numSpots: 6, placeFunction: place_shoal, chance: 100, sizeMin: 1, sizeMax: 2,
        typesRemoved_size:  sizeof(lakes_rivers) / sizeof(enum E_CellType), typesRemoved: lakes_rivers,
        humRemoved_size: sizeof(all_waters) / sizeof(enum E_CellHumidity), humRemoved: all_waters},
    // swamps
    {numSpots: 8, placeFunction: place_swamp, chance: 75, sizeMin: 10, sizeMax: 40,
        typesRemoved_size:  sizeof(swamp_types) / sizeof(enum E_CellType), typesRemoved: swamp_types,
        humRemoved_size: sizeof(normal) / sizeof(enum E_CellHumidity), humRemoved: normal },
    // drylands
    {numSpots: 8, placeFunction: place_dry, chance: 75, sizeMin: 10, sizeMax: 40,
        typesRemoved_size:  sizeof(dry_types) / sizeof(enum E_CellType), typesRemoved: dry_types,
        humRemoved_size: sizeof(normal) / sizeof(enum E_CellHumidity), humRemoved: normal },
};

#endif // MAP_GEN_H_INCLUDED
