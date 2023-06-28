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
                    size_t _rightTypes_size,
                    enum E_CellType *_rightTypes) {
    if (_rightTypes_size < 1 || _rightTypes == NULL) {
            return false;
    }

    for (size_t i = 0; i < _rightTypes_size; i++) {
        if (_type == _rightTypes[i]) {
            return true;
        }
    }

    return false;
};

size_t map_change_cell_type_recursive(
        enum E_CellType _type,
        unsigned int _chance,
        size_t* p_curSize,
        size_t _maxSize,
        unsigned int _x,
        unsigned int _y,
        size_t _typesRemoved_size,
        enum E_CellType *_typesRemoved,
        bool _is_first) {

    dprintf("map_change_cell_type_recursive, type: %d, chance: %d, current size: %d, max size: %d, x: %d, y: %d, is_first: %d", _type, _chance, (int)(*p_curSize), (int)_maxSize, _x, _y, _is_first);
    if (_typesRemoved_size < 1 || _typesRemoved == NULL || _chance < 1) {
        return 0;
    }

    if (_x < 0 || _x >= MAP_WIDTH || _y < 0 || _y >= MAP_HEIGHT) {
        return 0;
    }

    if (*p_curSize >= _maxSize) {
        return 0;
    }

    if (!is_right_type(g_Map[_y][_x].type, _typesRemoved_size, _typesRemoved)) {
        return 0;
    }

    if (_is_first || (rand() % 100) < _chance) {
        g_Map[_y][_x].type = _type;
        *p_curSize += 1;

        map_change_cell_type_recursive(_type, _chance, p_curSize, _maxSize,
            _x, _y-1, _typesRemoved_size, _typesRemoved, false);
        map_change_cell_type_recursive(_type, _chance, p_curSize, _maxSize,
            _x, _y+1, _typesRemoved_size, _typesRemoved, false);
        map_change_cell_type_recursive(_type, _chance, p_curSize, _maxSize,
            _x-1, _y, _typesRemoved_size, _typesRemoved, false);
        map_change_cell_type_recursive(_type, _chance, p_curSize, _maxSize,
            _x+1, _y, _typesRemoved_size, _typesRemoved, false);
    }

    return *p_curSize;
}

size_t map_place_spot(
        enum E_CellType _type,
        unsigned int _chance,
        size_t _sizeMin,
        size_t _sizeMax,
        size_t _typesRemoved_size,
        enum E_CellType *_typesRemoved) {
    dprintf("map_place_spot started, type: %d, chance: %d, sizeMin: %d, sizeMax: %d", _type, _chance, (int)_sizeMin, (int)_sizeMax);

    /* we need to know which cells have to be removed*/
    if (_typesRemoved_size < 1 || _typesRemoved == NULL || _chance < 1) {
            return 0;
    }

    /* calculate spot's size */
    size_t size = _sizeMin + ( rand() % (_sizeMax + 1 - _sizeMin));

    unsigned int start_x, start_y;
    size_t num_tries = 0;

    /* find cell with needed type*/
    do {
        /* if we try to place spot and can't find right cell*/
        if (num_tries > NUM_TRIES_TO_PLACE_SPOT) {
            return 0;
        }

        start_x = rand() % MAP_WIDTH;
        start_y = rand() % MAP_HEIGHT;

        num_tries++;
    } while (!is_right_type(g_Map[start_y][start_x].type, _typesRemoved_size, _typesRemoved));

    size_t num_cells = 0;

    map_change_cell_type_recursive(_type, _chance, &num_cells, size,
                                   start_x, start_y, _typesRemoved_size, _typesRemoved, true);

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

    for (size_t i = 0; i < FOREST_SPOTS_NUM; i++) {
        map_place_spot(ECT_FOREST,
                    FOREST_SPOTS_CHANCE,
                    FOREST_SPOTS_SIZE_MIN,
                    FOREST_SPOTS_SIZE_MAX,
                    sizeof(plains) / sizeof(enum E_CellType),
                    plains);
    }
    for (size_t i = 0; i < HILL_SPOTS_NUM; i++) {
        map_place_spot(ECT_HILL,
                    HILL_SPOTS_CHANCE,
                    HILL_SPOTS_SIZE_MIN,
                    HILL_SPOTS_SIZE_MAX,
                    sizeof(plains) / sizeof(enum E_CellType),
                    plains);
    }
    for (size_t i = 0; i < LOWLAND_SPOTS_NUM; i++) {
        map_place_spot(ECT_LOWLAND,
                    LOWLAND_SPOTS_CHANCE,
                    LOWLAND_SPOTS_SIZE_MIN,
                    LOWLAND_SPOTS_SIZE_MAX,
                    sizeof(plains) / sizeof(enum E_CellType),
                    plains);
    }
    for (size_t i = 0; i < MOUNTAIN_SPOTS_NUM; i++) {
        map_place_spot(ECT_MOUNTAIN,
                    MOUNTAIN_SPOTS_CHANCE,
                    MOUNTAIN_SPOTS_SIZE_MIN,
                    MOUNTAIN_SPOTS_SIZE_MAX,
                    sizeof(hills) / sizeof(enum E_CellType),
                    hills);
    }
    for (size_t i = 0; i < DEADFALL_SPOTS_NUM; i++) {
        map_place_spot(ECT_DEADFALL,
                    DEADFALL_SPOTS_CHANCE,
                    DEADFALL_SPOTS_SIZE_MIN,
                    DEADFALL_SPOTS_SIZE_MAX,
                    sizeof(forests) / sizeof(enum E_CellType),
                    forests);
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
