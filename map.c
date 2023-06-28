#include <stdlib.h>

#include "include/curses.h"

#include "common.h"
#include "debug.h"
#include "map.h"
#include "screen.h"

#define NUM_TRIES_TO_PLACE_SPOT 1000
#define LOWLAND_SPOTS_NUM          4
#define LOWLAND_SPOTS_SIZE_MIN     10
#define LOWLAND_SPOTS_SIZE_MAX     20
#define LOWLAND_SPOTS_CHANCE       75

#define FOREST_SPOTS_NUM        16
#define FOREST_SPOTS_SIZE_MIN   10
#define FOREST_SPOTS_SIZE_MAX   100
#define FOREST_SPOTS_CHANCE     67

#define HILL_SPOTS_NUM          8
#define HILL_SPOTS_SIZE_MIN     10
#define HILL_SPOTS_SIZE_MAX     50
#define HILL_SPOTS_CHANCE       75

#define MOUNTAIN_SPOTS_NUM          5
#define MOUNTAIN_SPOTS_SIZE_MIN     2
#define MOUNTAIN_SPOTS_SIZE_MAX     10
#define MOUNTAIN_SPOTS_CHANCE       75

#define DEADFALL_SPOTS_NUM          10
#define DEADFALL_SPOTS_SIZE_MIN     2
#define DEADFALL_SPOTS_SIZE_MAX     10
#define DEADFALL_SPOTS_CHANCE       75

#define LAKE_SPOTS_NUM          6
#define LAKE_SPOTS_SIZE_MIN     1
#define LAKE_SPOTS_SIZE_MAX     21
#define LAKE_SPOTS_CHANCE       67

#define SHOAL_SPOTS_NUM          6
#define SHOAL_SPOTS_SIZE_MIN     1
#define SHOAL_SPOTS_SIZE_MAX     2
#define SHOAL_SPOTS_CHANCE       100

#define DRY_SPOTS_NUM          8
#define DRY_SPOTS_SIZE_MIN     10
#define DRY_SPOTS_SIZE_MAX     50
#define DRY_SPOTS_CHANCE       75

#define SWAMP_SPOTS_NUM          8
#define SWAMP_SPOTS_SIZE_MIN     10
#define SWAMP_SPOTS_SIZE_MAX     50
#define SWAMP_SPOTS_CHANCE       75

char get_cell_char(struct MapCell _cell) {
    /* non-explored map cells shown blank*/
    if (!_cell.is_explored && g_Debug) {
        return ' ';
    }

    switch (_cell.type) {
    case ECT_PLAIN:
        return '"';
    case ECT_LOWLAND:
        return '.';
    case ECT_FOREST:
        return 'F';
    case ECT_DEADFALL:
        return 'X';
    case ECT_HILL:
        return '^';
    case ECT_MOUNTAIN:
        return 'A';
    case ECT_SHOAL:
        return '-';
    case ECT_LAKE:
        return '~';
    case ECT_RIVER:
        return '~';
    }

    return ' ';
}

short get_cell_color(struct MapCell _cell) {
    switch (_cell.hum) {
    case ECH_NORMAL:
        return _cell.is_visible ? COLOR_BRIGHTGREEN : COLOR_GREEN;
    case ECH_DRY:
        return _cell.is_visible ? COLOR_BRIGHTYELLOW : COLOR_YELLOW;
    case ECH_SWAMP:
        return _cell.is_visible ? COLOR_BRIGHTCYAN : COLOR_CYAN;
    case ECH_WATER:
        return _cell.is_visible ? COLOR_BRIGHTBLUE : COLOR_BLUE;
    case ECH_SNOW:
        return _cell.is_visible ? COLOR_BRIGHTWHITE : COLOR_WHITE;
    }

    return COLOR_BLACK;
}

bool is_right_type(enum E_CellType _type,
                   enum E_CellHumidity _hum,
                   size_t _rightTypes_size,
                   enum E_CellType *_rightTypes,
                   size_t _rightHum_size,
                   enum E_CellHumidity *_rightHums
                    ) {
    if (_rightTypes_size < 1 || _rightTypes == NULL || _rightHum_size < 1 || _rightHums == NULL) {
            return false;
    }

    for (size_t i = 0; i < _rightTypes_size; i++) {
        if (_type == _rightTypes[i]) {
            for (size_t j = 0; j < _rightHum_size; j++) {
                if (_hum == _rightHums[j]) {
                    return true;
                }
            }
        }
    }

    return false;
};

void place_type(struct MapCell* _pCell, enum E_CellType _type) { (*_pCell).type = _type; }
void place_humidity(struct MapCell* _pCell, enum E_CellHumidity _hum) { (*_pCell).hum = _hum; }

void place_forest(struct MapCell* _pCell) { place_type(_pCell, ECT_FOREST); }
void place_hill(struct MapCell* _pCell) { place_type(_pCell, ECT_HILL); }
void place_lowland(struct MapCell* _pCell) { place_type(_pCell, ECT_LOWLAND); }
void place_deadfall(struct MapCell* _pCell) { place_type(_pCell, ECT_DEADFALL); }
void place_dry(struct MapCell* _pCell) { place_humidity(_pCell, ECH_DRY); }
void place_swamp(struct MapCell* _pCell) { place_humidity(_pCell, ECH_SWAMP); }
void place_mountain(struct MapCell* _pCell) {
    place_type(_pCell, ECT_MOUNTAIN);
    place_humidity(_pCell, ECH_SNOW);
}
void place_lake(struct MapCell* _pCell) {
    place_type(_pCell, ECT_LAKE);
    place_humidity(_pCell, ECH_WATER);
}
void place_shoal(struct MapCell* _pCell) {
    place_type(_pCell, ECT_SHOAL);
    place_humidity(_pCell, ECH_WATER);
}

size_t map_change_cell_type_recursive(
        void (*_placeFunction)(struct MapCell* _pCell),
        unsigned int _chance,
        size_t* p_curSize,
        size_t _maxSize,
        unsigned int _x,
        unsigned int _y,
        size_t _typesRemoved_size,
        enum E_CellType *_typesRemoved,
        size_t _humRemoved_size,
        enum E_CellHumidity *_humRemoved,
        bool _is_first) {
//    dprintf("map_change_cell_type_recursive, type: %d, chance: %d, current size: %d, max size: %d, x: %d, y: %d, is_first: %d", _type, _chance, (int)(*p_curSize), (int)_maxSize, _x, _y, _is_first);
    if (_typesRemoved_size < 1 || _typesRemoved == NULL || _chance < 1) {
        return 0;
    }

    if (_x < 0 || _x >= MAP_WIDTH || _y < 0 || _y >= MAP_HEIGHT) {
        return 0;
    }

    if (*p_curSize >= _maxSize) {
        return 0;
    }

    if (!is_right_type(g_Map[_y][_x].type, g_Map[_y][_x].hum, _typesRemoved_size, _typesRemoved, _humRemoved_size, _humRemoved)) {
        return 0;
    }

    if (_is_first || (rand() % 100) < _chance) {
        _placeFunction(&(g_Map[_y][_x]));
        *p_curSize += 1;

        map_change_cell_type_recursive(_placeFunction, _chance, p_curSize, _maxSize,
            _x, _y-1, _typesRemoved_size, _typesRemoved,
            _humRemoved_size, _humRemoved, false);
        map_change_cell_type_recursive(_placeFunction, _chance, p_curSize, _maxSize,
            _x, _y+1, _typesRemoved_size, _typesRemoved,
            _humRemoved_size, _humRemoved, false);
        map_change_cell_type_recursive(_placeFunction, _chance, p_curSize, _maxSize,
            _x-1, _y, _typesRemoved_size, _typesRemoved,
            _humRemoved_size, _humRemoved, false);
        map_change_cell_type_recursive(_placeFunction, _chance, p_curSize, _maxSize,
            _x+1, _y, _typesRemoved_size, _typesRemoved,
            _humRemoved_size, _humRemoved, false);
    }

    return *p_curSize;
}

size_t map_place_spot(
        void (*_placeFunction)(struct MapCell* _pCell),
        unsigned int _chance,
        size_t _sizeMin,
        size_t _sizeMax,
        size_t _typesRemoved_size,
        enum E_CellType *_typesRemoved,
        size_t _humRemoved_size,
        enum E_CellHumidity *_humRemoved
        ) {
    //dprintf("map_place_spot started, type: %d, chance: %d, sizeMin: %d, sizeMax: %d", _type, _chance, (int)_sizeMin, (int)_sizeMax);

    // we need to know which cells' types and humidities have to be removed
    if (_typesRemoved_size < 1 || _typesRemoved == NULL ||
        _humRemoved_size < 1 || _humRemoved == NULL || _chance < 1) {
            return 0;
    }

    // calculate spot's size
    size_t size = _sizeMin + ( rand() % (_sizeMax + 1 - _sizeMin));

    unsigned int start_x, start_y;
    size_t num_tries = 0;

    // find cell with needed type
    do {
        // if we try to place spot and can't find right cell
        if (num_tries > NUM_TRIES_TO_PLACE_SPOT) {
            return 0;
        }

        start_x = rand() % MAP_WIDTH;
        start_y = rand() % MAP_HEIGHT;

        num_tries++;
    } while (!is_right_type(g_Map[start_y][start_x].type, g_Map[start_y][start_x].hum, _typesRemoved_size, _typesRemoved, _humRemoved_size, _humRemoved));

    size_t num_cells = 0;

    map_change_cell_type_recursive(_placeFunction, _chance, &num_cells, size,
                                   start_x, start_y,
                                   _typesRemoved_size, _typesRemoved,
                                   _humRemoved_size, _humRemoved, true);

    return num_cells;
}

void map_init() {
    unsigned int max_height = MAP_HEIGHT - 1;
    unsigned int max_width = MAP_WIDTH - 1;

    unsigned int river_x1 = MAP_WIDTH / 2;
    unsigned int river_x2 = river_x1 + 1;

    for (size_t i = 0; i < MAP_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_WIDTH; j++) {
            g_Map[i][j].is_explored = false;
            g_Map[i][j].is_visible = false;

            /* river from north to south*/
            if (j == river_x1 || j == river_x2) {
                    g_Map[i][j].type = ECT_RIVER;
                    g_Map[i][j].hum = ECH_WATER;
                    continue;
            }

            /* map borders - high mountains */
            if (i == 0 || j == 0 || i == max_height || j == max_width) {
                    g_Map[i][j].type = ECT_MOUNTAIN;
                    g_Map[i][j].hum = ECH_SNOW;
                    continue;
            }

            /* all other - plains*/
            g_Map[i][j].type = ECT_PLAIN;
            g_Map[i][j].hum = ECH_NORMAL;
        }
    }

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

    for (size_t i = 0; i < FOREST_SPOTS_NUM; i++) {
        map_place_spot(place_forest,
                    FOREST_SPOTS_CHANCE,
                    FOREST_SPOTS_SIZE_MIN,
                    FOREST_SPOTS_SIZE_MAX,
                    sizeof(plains) / sizeof(enum E_CellType),
                    plains,
                    sizeof(all_humidities) / sizeof(enum E_CellHumidity),
                    all_humidities);
    }
    for (size_t i = 0; i < HILL_SPOTS_NUM; i++) {
        map_place_spot(place_hill,
                    HILL_SPOTS_CHANCE,
                    HILL_SPOTS_SIZE_MIN,
                    HILL_SPOTS_SIZE_MAX,
                    sizeof(plains) / sizeof(enum E_CellType),
                    plains,
                    sizeof(all_humidities) / sizeof(enum E_CellHumidity),
                    all_humidities);
    }
    for (size_t i = 0; i < LOWLAND_SPOTS_NUM; i++) {
        map_place_spot(place_lowland,
                    LOWLAND_SPOTS_CHANCE,
                    LOWLAND_SPOTS_SIZE_MIN,
                    LOWLAND_SPOTS_SIZE_MAX,
                    sizeof(plains) / sizeof(enum E_CellType),
                    plains,
                    sizeof(all_humidities) / sizeof(enum E_CellHumidity),
                    all_humidities);
    }
    for (size_t i = 0; i < MOUNTAIN_SPOTS_NUM; i++) {
        map_place_spot(place_mountain,
                    MOUNTAIN_SPOTS_CHANCE,
                    MOUNTAIN_SPOTS_SIZE_MIN,
                    MOUNTAIN_SPOTS_SIZE_MAX,
                    sizeof(hills) / sizeof(enum E_CellType),
                    hills,
                    sizeof(all_humidities) / sizeof(enum E_CellHumidity),
                    all_humidities);
    }
    for (size_t i = 0; i < DEADFALL_SPOTS_NUM; i++) {
        map_place_spot(place_deadfall,
                    DEADFALL_SPOTS_CHANCE,
                    DEADFALL_SPOTS_SIZE_MIN,
                    DEADFALL_SPOTS_SIZE_MAX,
                    sizeof(forests) / sizeof(enum E_CellType),
                    forests,
                    sizeof(all_humidities) / sizeof(enum E_CellHumidity),
                    all_humidities);
    }
    for (size_t i = 0; i < LAKE_SPOTS_NUM; i++) {
        map_place_spot(place_lake,
                    LAKE_SPOTS_CHANCE,
                    LAKE_SPOTS_SIZE_MIN,
                    LAKE_SPOTS_SIZE_MAX,
                    sizeof(not_mountains) / sizeof(enum E_CellType),
                    not_mountains,
                    sizeof(all_humidities) / sizeof(enum E_CellHumidity),
                    all_humidities);
    }
    for (size_t i = 0; i < SHOAL_SPOTS_NUM; i++) {
        map_place_spot(place_shoal,
                    SHOAL_SPOTS_CHANCE,
                    SHOAL_SPOTS_SIZE_MIN,
                    SHOAL_SPOTS_SIZE_MAX,
                    sizeof(lakes_rivers) / sizeof(enum E_CellType),
                    lakes_rivers,
                    sizeof(all_waters) / sizeof(enum E_CellHumidity),
                    all_waters);
    }
    for (size_t i = 0; i < SWAMP_SPOTS_NUM; i++) {
        map_place_spot(place_swamp,
                    SWAMP_SPOTS_CHANCE,
                    SWAMP_SPOTS_SIZE_MIN,
                    SWAMP_SPOTS_SIZE_MAX,
                    sizeof(swamp_types) / sizeof(enum E_CellType),
                    swamp_types,
                    sizeof(normal) / sizeof(enum E_CellHumidity),
                    normal);
    }
    for (size_t i = 0; i < DRY_SPOTS_NUM; i++) {
        map_place_spot(place_dry,
                    DRY_SPOTS_CHANCE,
                    DRY_SPOTS_SIZE_MIN,
                    DRY_SPOTS_SIZE_MAX,
                    sizeof(dry_types) / sizeof(enum E_CellType),
                    dry_types,
                    sizeof(normal) / sizeof(enum E_CellHumidity),
                    normal);
    }
}

void map_draw() {
    for (size_t i = 0; i < MAP_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_WIDTH; j++) {
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch = get_cell_char(g_Map[i][j]);
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch_color = get_cell_color(g_Map[i][j]);
        }
    }
}
