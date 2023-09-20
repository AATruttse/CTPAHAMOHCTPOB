#include <stdlib.h>

#include "map_gen_local.h"

enum E_LocalCellType no_grass[1] = {ELCT_NOGRASS};
enum E_LocalCellType grass[1] = {ELCT_GRASS};
enum E_LocalCellType lug[1] = {ELCT_L_UNDERGROWTH};

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

bool config_map_local() {
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
            printf("%s %s %s %d %d %d %d %s\n",
                   type, humidity, local_type, numSpots, chance, sizeMin, sizeMax, types_removed);

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
                    pNewTypeMap = map_get(&g_MapLocalSpotParams, type);
            }

            map_LocalSpotParams_t* pNewHumMap = map_get(pNewTypeMap, humidity);
            if (pNewHumMap == NULL) {
                    map_LocalSpotParams_t newHumMap;
                    map_init(&newHumMap);
                    map_set(pNewTypeMap, humidity, newHumMap);
                    pNewHumMap = map_get(pNewTypeMap, humidity);
            }

            map_set(pNewHumMap, local_type, spot_param);
        }
/*
        const char* key;
        const char* key1;
        const char* key2;

        map_iter_t iter = map_iter(&g_MapLocalSpotParams);
        while ((key = map_next(&g_MapLocalSpotParams, &iter))) {
            printf("%s\n", key);
            map_iter_t iter1 = map_iter(map_get(&g_MapLocalSpotParams, key));
            while ((key1 = map_next(map_get(&g_MapLocalSpotParams, key), &iter1))) {
                printf("    %s\n", key1);
                map_iter_t iter2 = map_iter(map_get(map_get(&g_MapLocalSpotParams, key), key1));
                while ((key2 = map_next(map_get(map_get(&g_MapLocalSpotParams, key), key1), &iter2))) {
                    printf("        %s -> ", key2);
                    struct MapLocalSpotParams* params = map_get(map_get(map_get(&g_MapLocalSpotParams, key), key1), key2);
                    printf("        %d %d %d %d %d\n", params->chance, params->numSpots, params->sizeMin, params->sizeMax, params->typesRemoved_size);
                }
            }
        }
*/

        fclose(file);
        exit(1);
        return true;
    }

    return false;
}

void init_gen_map_local() {
    map_init(&g_ELCTs);

    struct E_LocalCellTypeArray grass_array = {cellTypesArray: grass, arraySize: sizeof(grass) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray nograss_array = {cellTypesArray: no_grass, arraySize: sizeof(no_grass) / sizeof(enum E_LocalCellType)};
    struct E_LocalCellTypeArray lug_array = {cellTypesArray: lug, arraySize: sizeof(lug) / sizeof(enum E_LocalCellType)};

    map_set(&g_ELCTs, "grass", grass_array);
    map_set(&g_ELCTs, "nograss", nograss_array);
    map_set(&g_ELCTs, "lug", lug_array);

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
}
