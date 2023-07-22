#include "include/curses.h"

#include "common.h"
#include "debug.h"
#include "map.h"
#include "map_local.h"
#include "screen.h"

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
    }

    return COLOR_BLACK;
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


