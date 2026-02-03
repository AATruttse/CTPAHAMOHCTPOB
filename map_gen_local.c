#include <stdlib.h>
#include <string.h>

#include "logs.h"
#include "map_gen_local.h"
#include "map_global.h"
#include "map_local.h"

enum E_LocalCellType no_grass[1] = {ELCT_NOGRASS};
enum E_LocalCellType grass[1] = {ELCT_GRASS};
enum E_LocalCellType lug[1] = {ELCT_L_UNDERGROWTH};
enum E_LocalCellType hug[1] = {ELCT_H_UNDERGROWTH};
enum E_LocalCellType tree[1] = {ELCT_TREE};
enum E_LocalCellType btree[1] = {ELCT_B_TREE};
enum E_LocalCellType ftree[1] = {ELCT_TREE_FALLEN};
enum E_LocalCellType bftree[1] = {ELCT_B_TREE_FALLEN};
enum E_LocalCellType swamp_arr[1] = {ELCT_SWAMP};
enum E_LocalCellType dswamp_arr[1] = {ELCT_SWAMP_DEEP};
enum E_LocalCellType rock[1] = {ELCT_ROCK};
enum E_LocalCellType rubble[1] = {ELCT_RUBBLE};
enum E_LocalCellType water_arr[1] = {ELCT_WATER};
enum E_LocalCellType dwater_arr[1] = {ELCT_WATER_DEEP};

void place_local_type(struct MapLocalCell* _pCell, enum E_LocalCellType _type) { (*_pCell).type = _type; }

void place_local_nograss(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_NOGRASS); }
void place_local_grass(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_GRASS); }
void place_local_lug(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_L_UNDERGROWTH); }
void place_local_hug(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_H_UNDERGROWTH); }
void place_local_tree(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_TREE); }
void place_local_btree(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_B_TREE); }
void place_local_ftree(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_TREE_FALLEN); }
void place_local_bftree(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_B_TREE_FALLEN); }
void place_local_swamp(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_SWAMP); }
void place_local_dswamp(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_SWAMP_DEEP); }
void place_local_rock(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_ROCK); }
void place_local_rubble(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_RUBBLE); }
void place_local_water(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_WATER); }
void place_local_dwater(struct MapLocalCell* _pCell) { place_local_type(_pCell, ELCT_WATER_DEEP); }

static const char* cell_type_to_str(enum E_CellType t) {
    switch (t) {
        case ECT_PLAIN:    return "plain";
        case ECT_LOWLAND:  return "lowland";
        case ECT_FOREST:   return "forest";
        case ECT_DEADFALL: return "deadfall";
        case ECT_HILL:     return "hill";
        case ECT_MOUNTAIN: return "mountain";
        case ECT_SHOAL:    return "shoal";
        case ECT_LAKE:     return "lake";
        case ECT_RIVER:    return "river";
        default:           return "plain";
    }
}

static const char* cell_hum_to_str(enum E_CellHumidity h) {
    switch (h) {
        case ECH_NORMAL: return "normal";
        case ECH_DRY:    return "dry";
        case ECH_SWAMP:  return "swamp";
        case ECH_WATER:  return "water";
        case ECH_SNOW:   return "snow";
        case ECH_BURNED: return "burned";
        case ECH_FIRE:   return "fire";
        default:         return "normal";
    }
}

/* Fixed order so layers apply correctly: lug on grass, hug on lug, tree on hug, etc. */
static const char* const SPOT_ORDER[] = {
    "lug", "hug", "tree", "btree", "ftree", "bftree",
    "nograss", "grass", "swamp", "dswamp", "rock", "rubble", "water", "dwater"
};
static const size_t SPOT_ORDER_LEN = sizeof(SPOT_ORDER) / sizeof(SPOT_ORDER[0]);

void apply_local_map_gen(struct MapCell *_pMapCell, struct MapLocal *_pLocalMap) {
    const char *type_str = cell_type_to_str(_pMapCell->type);
    const char *hum_str = cell_hum_to_str(_pMapCell->hum);

    map_map_LocalSpotParams_t *pTypeMap = map_get(&g_MapLocalSpotParams, type_str);
    if (!pTypeMap) return;

    map_LocalSpotParams_t *pSpotMap = map_get(pTypeMap, hum_str);
    if (!pSpotMap) return;

    for (size_t o = 0; o < SPOT_ORDER_LEN; o++) {
        struct MapLocalSpotParams *params = map_get(pSpotMap, SPOT_ORDER[o]);
        if (!params || params->typesRemoved_size < 1 || !params->typesRemoved || params->chance < 1)
            continue;
        for (size_t n = 0; n < params->numSpots; n++) {
            local_map_place_spot(_pLocalMap, params->placeFunction, params->chance,
                params->sizeMin, params->sizeMax, params->typesRemoved_size, params->typesRemoved);
        }
    }
}

bool config_map_local() {
    logMessage("Start config map local! #maplocal #init");

    char path[DEFAULT_LINE_LENGTH];
    strcpy(path, CONFIG_PATH);
    strcat(path, LOCAL_MAP_CONFIG_FILE);

    FILE *file;
    if ((file = fopen(path, "r")))
    {
        char line[DEFAULT_LINE_LENGTH];
        char type[DEFAULT_LINE_LENGTH];
        char humidity[DEFAULT_LINE_LENGTH];
        char local_type[DEFAULT_LINE_LENGTH];
        char types_removed[DEFAULT_LINE_LENGTH];
        int numSpots, chance, sizeMin, sizeMax;

        while(fgets(line, DEFAULT_LINE_LENGTH, file)) {
            sscanf( line, "%s %s %s  numSpots: %d chance: %d sizeMin: %d sizeMax: %d typesRemoved: %s",
                    type, humidity, local_type, &numSpots, &chance, &sizeMin, &sizeMax, types_removed);
            //printf("%s %s %s %d %d %d %d %s\n",
            //      type, humidity, local_type, numSpots, chance, sizeMin, sizeMax, types_removed);

            placeLocalFunction_t pPlaceFunction = *map_get(&g_PlaceLocalFunctions, local_type);
            struct E_LocalCellTypeArray* typesRemovedArray = map_get(&g_ELCTs, types_removed);

            struct MapLocalSpotParams spot_param = {
                numSpots :          numSpots,
                placeFunction :     pPlaceFunction,
                chance :            chance,
                sizeMin :           sizeMin,
                sizeMax :           sizeMax,
                typesRemoved_size : typesRemovedArray->arraySize,
                typesRemoved :      typesRemovedArray->cellTypesArray
            };

            map_map_LocalSpotParams_t* pNewTypeMap = map_get(&g_MapLocalSpotParams, type);
            if (pNewTypeMap == NULL) {
                    map_map_LocalSpotParams_t newTypeMap;
                    map_init(&newTypeMap);
                    map_set(&g_MapLocalSpotParams, type, newTypeMap);
                    logMessage("Create new local map parameters' hashmap for global map's type %s. #maplocal #init", type);

                    pNewTypeMap = map_get(&g_MapLocalSpotParams, type);
            }

            map_LocalSpotParams_t* pNewHumMap = map_get(pNewTypeMap, humidity);
            if (pNewHumMap == NULL) {
                    map_LocalSpotParams_t newHumMap;
                    map_init(&newHumMap);
                    map_set(pNewTypeMap, humidity, newHumMap);
                    logMessage("Create new local map parameters' hashmap for global map's humidity %s for type %s. #maplocal #init", humidity, type);
                    pNewHumMap = map_get(pNewTypeMap, humidity);
            }

            logMessage("Create new local map parameters' for local map's type %s for global map's humidity for type %s. #maplocal #init", local_type, humidity, type);
            logMessage("Parameters: numSpots=%d, chance=%d, sizeMin=%d, sizeMax=%d, types_removed=%s. #maplocal #init", numSpots, chance, sizeMin, sizeMax, types_removed);

            map_set(pNewHumMap, local_type, spot_param);
        }

        fclose(file);
        logMessage("Config map local ok! #maplocal #init");

        return true;
    }

    logError("Config map local error! Can't open file %s! #maplocal #init #error", path);

    return false;
}

void init_gen_map_local() {
    logMessage("Start local maps' generator init! #maplocal #init");

    map_init(&g_ELCTs);

    struct E_LocalCellTypeArray grass_array = {cellTypesArray: grass, arraySize: sizeof(grass) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray nograss_array = {cellTypesArray: no_grass, arraySize: sizeof(no_grass) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray lug_array = {cellTypesArray: lug, arraySize: sizeof(lug) / sizeof(enum E_LocalCellType)};

    map_set(&g_ELCTs, "grass", grass_array);
    map_set(&g_ELCTs, "nograss", nograss_array);
    map_set(&g_ELCTs, "lug", lug_array);
    struct E_LocalCellTypeArray hug_array = {cellTypesArray: hug, arraySize: sizeof(hug) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray tree_array = {cellTypesArray: tree, arraySize: sizeof(tree) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray btree_array = {cellTypesArray: btree, arraySize: sizeof(btree) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray ftree_array = {cellTypesArray: ftree, arraySize: sizeof(ftree) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray bftree_array = {cellTypesArray: bftree, arraySize: sizeof(bftree) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray swamp_array = {cellTypesArray: swamp_arr, arraySize: sizeof(swamp_arr) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray dswamp_array = {cellTypesArray: dswamp_arr, arraySize: sizeof(dswamp_arr) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray rock_array = {cellTypesArray: rock, arraySize: sizeof(rock) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray rubble_array = {cellTypesArray: rubble, arraySize: sizeof(rubble) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray water_array = {cellTypesArray: water_arr, arraySize: sizeof(water_arr) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray dwater_array = {cellTypesArray: dwater_arr, arraySize: sizeof(dwater_arr) / sizeof(enum E_LocalCellType)};
    map_set(&g_ELCTs, "hug", hug_array);
    map_set(&g_ELCTs, "tree", tree_array);
    map_set(&g_ELCTs, "btree", btree_array);
    map_set(&g_ELCTs, "ftree", ftree_array);
    map_set(&g_ELCTs, "bftree", bftree_array);
    map_set(&g_ELCTs, "swamp", swamp_array);
    map_set(&g_ELCTs, "dswamp", dswamp_array);
    map_set(&g_ELCTs, "rock", rock_array);
    map_set(&g_ELCTs, "rubble", rubble_array);
    map_set(&g_ELCTs, "water", water_array);
    map_set(&g_ELCTs, "dwater", dwater_array);

    map_init(&g_PlaceLocalFunctions);
    map_set(&g_PlaceLocalFunctions, "nograss", place_local_nograss);
    map_set(&g_PlaceLocalFunctions, "grass", place_local_grass);
    map_set(&g_PlaceLocalFunctions, "lug", place_local_lug);
    map_set(&g_PlaceLocalFunctions, "hug", place_local_hug);
    map_set(&g_PlaceLocalFunctions, "tree", place_local_tree);
    map_set(&g_PlaceLocalFunctions, "btree", place_local_btree);
    map_set(&g_PlaceLocalFunctions, "ftree", place_local_ftree);
    map_set(&g_PlaceLocalFunctions, "bftree", place_local_bftree);
    map_set(&g_PlaceLocalFunctions, "swamp", place_local_swamp);
    map_set(&g_PlaceLocalFunctions, "dswamp", place_local_dswamp);
    map_set(&g_PlaceLocalFunctions, "rock", place_local_rock);
    map_set(&g_PlaceLocalFunctions, "rubble", place_local_rubble);
    map_set(&g_PlaceLocalFunctions, "water", place_local_water);
    map_set(&g_PlaceLocalFunctions, "dwater", place_local_dwater);

    map_init(&g_MapLocalSpotParams);

    config_map_local();
    logMessage("Local maps' generator init ok! #maplocal #init");
}
