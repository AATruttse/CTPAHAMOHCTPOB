#include <stdlib.h>

#include "include/curses.h"

#include "common.h"
#include "debug.h"
#include "map_global.h"
#include "map_local.h"
#include "screen.h"

#define NUM_TRIES_TO_PLACE_SPOT 1000

char get_local_cell_char(struct MapLocal *_pLocalMap, size_t _x, size_t _y) {
    /* non-explored map cells shown blank*/
    /* non-explored map cells shown blank*/
    #ifdef DEBUG
        if (!(_pLocalMap->cells[_y][_x].flags & EXPLORED_FLAG) && g_Debug) {
            return ' ';
        }
    #else
        if (!_pLocalMap->cells[_y][_x].flags & EXPLORED_FLAG) {
            return ' ';
        }
    #endif // DEBUG

    switch (_pLocalMap->cells[_y][_x].type) {
        case ELCT_NOGRASS:
            return '.';
        case ELCT_GRASS:
            return ',';
        case ELCT_L_UNDERGROWTH:
            return '"';
        case ELCT_H_UNDERGROWTH:
            return '*';
        case ELCT_TREE:
            return 'f';
        case ELCT_B_TREE:
            return 'F';
        case ELCT_TREE_FALLEN:
            return 'x';
        case ELCT_B_TREE_FALLEN:
            return 'X';
        case ELCT_SWAMP:
            return '_';
        case ELCT_SWAMP_DEEP:
            return '_';
        case ELCT_ROCK:
            return '#';
        case ELCT_RUBBLE:
            return ':';
        case ELCT_WATER:
            return '~';
        case ELCT_WATER_DEEP:
            return '~';
    }

    return ' ';
}

short get_local_cell_color(struct MapLocal *_pLocalMap, size_t _x, size_t _y) {
    switch (g_Map[_pLocalMap->y][_pLocalMap->x].hum) {
    case ECH_NORMAL:
        return _pLocalMap->cells[_y][_x].flags & VISIBLE_FLAG ? COLOR_BRIGHTGREEN : COLOR_GREEN;
    case ECH_DRY:
        return _pLocalMap->cells[_y][_x].flags & VISIBLE_FLAG ? COLOR_BRIGHTYELLOW : COLOR_YELLOW;
    case ECH_SWAMP:
        return _pLocalMap->cells[_y][_x].flags & VISIBLE_FLAG ? COLOR_BRIGHTCYAN : COLOR_CYAN;
    case ECH_WATER:
        return _pLocalMap->cells[_y][_x].flags & VISIBLE_FLAG ? COLOR_BRIGHTBLUE : COLOR_BLUE;
    case ECH_SNOW:
        return _pLocalMap->cells[_y][_x].flags & VISIBLE_FLAG ? COLOR_BRIGHTWHITE : COLOR_WHITE;
    case ECH_BURNED:
        return COLOR_WHITE;
    case ECH_FIRE:
        return _pLocalMap->cells[_y][_x].flags & VISIBLE_FLAG ? COLOR_BRIGHTRED : COLOR_RED;
    }

    return COLOR_BLACK;
}

enum E_LocalCellType get_local_default_cell (struct MapLocal *_pLocalMap) {
    switch (g_Map[_pLocalMap->y][_pLocalMap->x].type) {
        case ECT_PLAIN:
        case ECT_LOWLAND:
            switch (g_Map[_pLocalMap->y][_pLocalMap->x].hum) {
                case ECH_NORMAL:
                    return ELCT_GRASS;
                case ECH_DRY:
                case ECH_SNOW:
                case ECH_BURNED:
                case ECH_FIRE:
                    return ELCT_NOGRASS;
                case ECH_SWAMP:
                    return ELCT_SWAMP;
                case ECH_WATER:
                    return ELCT_WATER;
            }
            break;
        case ECT_FOREST:
        case ECT_DEADFALL:
            switch (g_Map[_pLocalMap->y][_pLocalMap->x].hum) {
                case ECH_NORMAL:
                case ECH_DRY:
                case ECH_SNOW:
                    return ELCT_L_UNDERGROWTH;
                case ECH_SWAMP:
                    return ELCT_SWAMP;
                case ECH_WATER:
                    return ELCT_WATER;
                case ECH_BURNED:
                case ECH_FIRE:
                    return ELCT_NOGRASS;
            }
            break;
        case ECT_HILL:
            switch (g_Map[_pLocalMap->y][_pLocalMap->x].hum) {
                case ECH_NORMAL:
                    return ELCT_GRASS;
                case ECH_DRY:
                case ECH_SNOW:
                case ECH_BURNED:
                case ECH_FIRE:
                    return ELCT_NOGRASS;
                case ECH_SWAMP:
                    return ELCT_SWAMP;
                case ECH_WATER:
                    return ELCT_WATER;
            }
            break;
        case ECT_MOUNTAIN:
            switch (g_Map[_pLocalMap->y][_pLocalMap->x].hum) {
                case ECH_NORMAL:
                case ECH_DRY:
                case ECH_SNOW:
                case ECH_BURNED:
                case ECH_FIRE:
                    return ELCT_RUBBLE;
                case ECH_SWAMP:
                    return ELCT_SWAMP;
                case ECH_WATER:
                    return ELCT_WATER;
            }
            break;
        case ECT_SHOAL:
            return ELCT_WATER;
            break;
        case ECT_LAKE:
        case ECT_RIVER:
            return ELCT_WATER_DEEP;
            break;
    }

    return COLOR_BLACK;

}

bool is_right_local_type(enum E_LocalCellType _type,
                   size_t _rightTypes_size,
                   enum E_LocalCellType *_rightTypes
                    ) {
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

size_t local_map_place_spot(
        struct MapLocal *_pLocalMap,
        void (*_placeFunction)(struct MapLocalCell*),
        unsigned int _chance,
        size_t _sizeMin,
        size_t _sizeMax,
        size_t _typesRemoved_size,
        enum E_LocalCellType *_typesRemoved
        ) {
    // we need to know which cells' types and humidities have to be removed
    if (_typesRemoved_size < 1 || _typesRemoved == NULL || _chance < 1) {
            return 0;
    }

    // calculate spot's size
    size_t size = _sizeMin + ( rand() % (_sizeMax + 1 - _sizeMin));

    size_t cur_x, cur_y;
    size_t num_tries = 0;

    // find cell with needed type
    do {
        // if we try to place spot and can't find right cell
        if (num_tries > NUM_TRIES_TO_PLACE_SPOT) {
            return 0;
        }

        cur_x = rand() % MAP_LOCAL_WIDTH;
        cur_y = rand() % MAP_LOCAL_HEIGHT;

        num_tries++;

    } while (!is_right_local_type(_pLocalMap->cells[cur_x][cur_y].type, _typesRemoved_size, _typesRemoved));

    size_t num_cells = 0;
    size_t num_fails = 0;
    size_t dir = 0;

    size_t max_x = MAP_LOCAL_WIDTH - 1;
    size_t max_y = MAP_LOCAL_HEIGHT - 1;

    do {
        if (is_right_local_type(_pLocalMap->cells[cur_y][cur_x].type, _typesRemoved_size, _typesRemoved)) {
            if (rand() % 100 < _chance) {
                _placeFunction(&_pLocalMap->cells[cur_y][cur_x]);
                num_cells++;
            }
        }
        else {
            num_fails++;
        }

        dir = rand() % 4;
        switch (dir) {
        case 0:
            cur_y = (cur_y != 0) ? cur_y-1 : cur_y;
            break;
        case 1:
            cur_x = (cur_x != max_x) ? cur_x+1 : cur_x;
            break;
        case 2:
            cur_y = (cur_y != max_y) ? cur_y+1 : cur_y;
            break;
        case 3:
            cur_x = (cur_x != 0) ? cur_x-1 : cur_x;
            break;
        }
    } while (num_cells < size && num_fails < NUM_TRIES_TO_PLACE_SPOT);



    return num_cells;
}

void maps_local_init_all() {
     for (size_t i = 0; i < MAP_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_WIDTH; j++) {
            g_LocalMaps[i][j].x = j;
            g_LocalMaps[i][j].y = i;
            g_LocalMaps[i][j].is_generated = false;
        }
     }
}

void map_local_init(struct MapCell *_pMapCell, struct MapLocal *_pLocalMap){
    _pLocalMap->is_generated = true;
    for (size_t i = 0; i < MAP_LOCAL_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_LOCAL_WIDTH; j++) {
            _pLocalMap->cells[i][j].type = ELCT_GRASS;
        }
    }
}

void map_local_draw(struct MapLocal *_pLocalMap){
    for (size_t i = 0; i < MAP_LOCAL_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_LOCAL_WIDTH; j++) {
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch = get_local_cell_char(_pLocalMap, j, i);
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch_color = get_local_cell_color(_pLocalMap, j, i);
        }
    }
}

bool map_local_save(FILE *_fptr, struct MapLocal *_pLocalMap){

    return true;
}

bool map_local_load(FILE *_fptr, struct MapLocal *_pLocalMap){
    return true;
}

bool maps_local_save_all(FILE *_fptr){
     for (size_t i = 0; i < MAP_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_WIDTH; j++) {
            if (!map_local_save(_fptr, &g_LocalMaps[i][j])) {
                return false;
            }
        }
     }

     return true;
}

bool maps_local_load_all(FILE *_fptr){
    for (size_t i = 0; i < MAP_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_WIDTH; j++) {
            if (!map_local_load(_fptr, &g_LocalMaps[i][j])) {
                return false;
            }
        }
     }

     return true;
}


