#include <stdlib.h>

#include "include/curses.h"

#include "common.h"
#include "debug.h"
#include "map.h"
#include "map_gen.h"
#include "screen.h"

#define NUM_TRIES_TO_PLACE_SPOT 1000

char get_cell_char(struct MapCell* _pCell) {
    /* non-explored map cells shown blank*/
    if (!(_pCell->flags & EXPLORED_FLAG) && g_Debug) {
        return ' ';
    }

    switch (_pCell->type) {
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

short get_cell_color(struct MapCell* _pCell) {
    switch (_pCell->hum) {
    case ECH_NORMAL:
        return _pCell->flags & VISIBLE_FLAG ? COLOR_BRIGHTGREEN : COLOR_GREEN;
    case ECH_DRY:
        return _pCell->flags & VISIBLE_FLAG ? COLOR_BRIGHTYELLOW : COLOR_YELLOW;
    case ECH_SWAMP:
        return _pCell->flags & VISIBLE_FLAG ? COLOR_BRIGHTCYAN : COLOR_CYAN;
    case ECH_WATER:
        return _pCell->flags & VISIBLE_FLAG ? COLOR_BRIGHTBLUE : COLOR_BLUE;
    case ECH_SNOW:
        return _pCell->flags & VISIBLE_FLAG ? COLOR_BRIGHTWHITE : COLOR_WHITE;
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
        void (*_placeFunction)(struct MapCell*),
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

void map_place_n_spots(
        size_t _numSpots,
        void (*_placeFunction)(struct MapCell*),
        unsigned int _chance,
        size_t _sizeMin,
        size_t _sizeMax,
        size_t _typesRemoved_size,
        enum E_CellType *_typesRemoved,
        size_t _humRemoved_size,
        enum E_CellHumidity *_humRemoved
        ) {
    for (size_t i = 0; i < _numSpots; i++) {
        map_place_spot(_placeFunction,
                    _chance,
                    _sizeMin,
                    _sizeMax,
                    _typesRemoved_size,
                    _typesRemoved,
                    _humRemoved_size,
                    _humRemoved);
    }
}

void map_place_spots() {
    for (size_t i = 0; i < sizeof(mapSpots) / sizeof(struct MapSpotParams); i++) {
        map_place_n_spots(mapSpots[i].numSpots, mapSpots[i].placeFunction,
                    mapSpots[i].chance, mapSpots[i].sizeMin, mapSpots[i].sizeMax,
                    mapSpots[i].typesRemoved_size, mapSpots[i].typesRemoved,
                    mapSpots[i].humRemoved_size, mapSpots[i].humRemoved);
    }
}

void map_init() {
    unsigned int max_height = MAP_HEIGHT - 1;
    unsigned int max_width = MAP_WIDTH - 1;

    unsigned int river_x1 = MAP_WIDTH / 2;
    unsigned int river_x2 = river_x1 + 1;

    for (size_t i = 0; i < MAP_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_WIDTH; j++) {
            g_Map[i][j].flags = 0;

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

    map_place_spots();
}

void map_draw() {
    for (size_t i = 0; i < MAP_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_WIDTH; j++) {
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch = get_cell_char(&(g_Map[i][j]));
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch_color = get_cell_color(&(g_Map[i][j]));
        }
    }
}
