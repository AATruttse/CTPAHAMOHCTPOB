#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/curses.h"

#include "common.h"
#include "debug.h"
#include "hero.h"
#include "logs.h"
#include "map_gen_local.h"
#include "map_global.h"
#include "map_local.h"
#include "savegame.h"
#include "screen.h"

#define NUM_TRIES_TO_PLACE_SPOT 1000
#define LOCAL_MAP_FILE_MAGIC "LM01"

int g_LocalViewX0 = 0;
int g_LocalViewY0 = 0;

char get_local_cell_char(struct MapLocal *_pLocalMap, size_t _x, size_t _y) {
    /* non-explored map cells shown blank*/
    /* non-explored map cells shown blank*/
    #ifdef DEBUG
        if (!(_pLocalMap->cells[_y][_x].flags & EXPLORED_FLAG) && g_Debug) {
            return ' ';
        }
    #else
        if (!(_pLocalMap->cells[_y][_x].flags & EXPLORED_FLAG)) {
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
    enum E_LocalCellType lct = _pLocalMap->cells[_y][_x].type;
    enum E_CellHumidity hum = g_Map[_pLocalMap->y][_pLocalMap->x].hum;
    int visible = (_pLocalMap->cells[_y][_x].flags & VISIBLE_FLAG) ? 1 : 0;

    switch (lct) {
    case ELCT_NOGRASS:
        switch (hum) {
        case ECH_SWAMP:  return visible ? COLOR_BRIGHTCYAN : COLOR_CYAN;
        case ECH_SNOW:   return visible ? COLOR_BRIGHTWHITE : COLOR_WHITE;
        case ECH_BURNED:
        case ECH_FIRE:   return COLOR_GRAY;
        default:         return visible ? COLOR_BRIGHTYELLOW : COLOR_YELLOW;
        }
    case ELCT_GRASS:
        switch (hum) {
        case ECH_NORMAL:
        case ECH_WATER:  return visible ? COLOR_BRIGHTGREEN : COLOR_GREEN;
        case ECH_DRY:    return visible ? COLOR_BRIGHTYELLOW : COLOR_YELLOW;
        case ECH_SWAMP:  return visible ? COLOR_BRIGHTCYAN : COLOR_CYAN;
        case ECH_SNOW:   return visible ? COLOR_BRIGHTWHITE : COLOR_WHITE;
        case ECH_BURNED:
        case ECH_FIRE:   return COLOR_GRAY;
        default:         return visible ? COLOR_BRIGHTGREEN : COLOR_GREEN;
        }
    case ELCT_L_UNDERGROWTH:
    case ELCT_H_UNDERGROWTH:
    case ELCT_TREE:
    case ELCT_B_TREE:
        switch (hum) {
        case ECH_NORMAL:
        case ECH_WATER:
        case ECH_SWAMP:  return visible ? COLOR_BRIGHTGREEN : COLOR_GREEN;
        case ECH_DRY:    return visible ? COLOR_BRIGHTYELLOW : COLOR_YELLOW;
        case ECH_SNOW:   return visible ? COLOR_BRIGHTWHITE : COLOR_WHITE;
        case ECH_BURNED:
        case ECH_FIRE:   return COLOR_GRAY;
        default:         return visible ? COLOR_BRIGHTGREEN : COLOR_GREEN;
        }
    case ELCT_TREE_FALLEN:
    case ELCT_B_TREE_FALLEN:
        switch (hum) {
        case ECH_DRY:    return visible ? COLOR_BRIGHTYELLOW : COLOR_YELLOW;
        case ECH_SNOW:   return visible ? COLOR_BRIGHTWHITE : COLOR_WHITE;
        default:         return COLOR_GRAY;
        }
    case ELCT_SWAMP:
    case ELCT_SWAMP_DEEP:
        switch (hum) {
        case ECH_DRY:    return visible ? COLOR_BRIGHTYELLOW : COLOR_YELLOW;
        case ECH_SNOW:   return visible ? COLOR_BRIGHTWHITE : COLOR_WHITE;
        default:         return visible ? COLOR_BRIGHTCYAN : COLOR_CYAN;
        }
    case ELCT_ROCK:
        return (hum == ECH_DRY) ? (visible ? COLOR_BRIGHTYELLOW : COLOR_YELLOW) : COLOR_GRAY;
    case ELCT_RUBBLE:
        switch (hum) {
        case ECH_DRY:    return visible ? COLOR_BRIGHTYELLOW : COLOR_YELLOW;
        case ECH_SNOW:   return visible ? COLOR_BRIGHTWHITE : COLOR_WHITE;
        default:         return COLOR_GRAY;
        }
    case ELCT_WATER:
    case ELCT_WATER_DEEP:
        return visible ? COLOR_BRIGHTBLUE : COLOR_BLUE;
    default:
        return COLOR_BLACK;
    }
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

    // calculate spot's size (guard against sizeMax < sizeMin to avoid underflow)
    size_t size_range = (_sizeMax >= _sizeMin) ? (_sizeMax - _sizeMin + 1) : 1;
    size_t size = _sizeMin + (rand() % size_range);
    if (size < 1) size = 1;

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

    } while (!is_right_local_type(_pLocalMap->cells[cur_y][cur_x].type, _typesRemoved_size, _typesRemoved));

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
    size_t my = _pLocalMap->y;
    size_t mx = _pLocalMap->x;

    if (map_local_load_from_file(my, mx))
        return;

    _pLocalMap->is_generated = true;
    enum E_LocalCellType default_type = get_local_default_cell(_pLocalMap);
    for (size_t i = 0; i < MAP_LOCAL_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_LOCAL_WIDTH; j++) {
            _pLocalMap->cells[i][j].type = default_type;
            _pLocalMap->cells[i][j].flags = 0;
        }
    }
    apply_local_map_gen(_pMapCell, _pLocalMap);
    map_local_save_to_file(my, mx);
}

void map_local_draw(struct MapLocal *_pLocalMap){
    int hx = (int)g_Hero.local_map_x;
    int hy = (int)g_Hero.local_map_y;
    /* Viewport: center hero; clamp when near border so we don't show out-of-bounds */
    int max_x0 = (int)MAP_LOCAL_WIDTH - MAP_WIDTH;
    int max_y0 = (int)MAP_LOCAL_HEIGHT - MAP_HEIGHT;
    if (max_x0 < 0) max_x0 = 0;
    if (max_y0 < 0) max_y0 = 0;
    g_LocalViewX0 = hx - MAP_WIDTH / 2;
    g_LocalViewY0 = hy - MAP_HEIGHT / 2;
    if (g_LocalViewX0 < 0) g_LocalViewX0 = 0;
    if (g_LocalViewX0 > max_x0) g_LocalViewX0 = max_x0;
    if (g_LocalViewY0 < 0) g_LocalViewY0 = 0;
    if (g_LocalViewY0 > max_y0) g_LocalViewY0 = max_y0;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            size_t wy = (size_t)(g_LocalViewY0 + i);
            size_t wx = (size_t)(g_LocalViewX0 + j);
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch = get_local_cell_char(_pLocalMap, wx, wy);
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch_color = get_local_cell_color(_pLocalMap, wx, wy);
        }
    }
}

bool map_local_save_to_file(size_t map_y, size_t map_x) {
    struct MapLocal *m = &g_LocalMaps[map_y][map_x];
    if (!m->is_generated)
        return true;

    char path[SAVE_PATH_MAX + 32];
    snprintf(path, sizeof(path), "%slocal_%zu_%zu.dat", g_SavePath, map_y, map_x);
    FILE *f = fopen(path, "wb");
    if (!f) {
        logError("Cannot open local map file for write: %s #save", path);
        return false;
    }

    if (fwrite(LOCAL_MAP_FILE_MAGIC, 1, 4, f) != 4) goto err;
    if (fputc((int)(map_y & 0xff), f) == EOF) goto err;
    if (fputc((int)(map_x & 0xff), f) == EOF) goto err;

    for (size_t i = 0; i < MAP_LOCAL_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_LOCAL_WIDTH; j++) {
            unsigned char t = (unsigned char)m->cells[i][j].type;
            unsigned char fl = m->cells[i][j].flags;
            if (fputc(t, f) == EOF) goto err;
            if (fputc(fl, f) == EOF) goto err;
        }
    }
    fclose(f);
    logMessage("Saved local map %zu,%zu to %s #save", map_y, map_x, path);
    return true;
err:
    fclose(f);
    logError("Failed writing local map %s #save", path);
    return false;
}

bool map_local_load_from_file(size_t map_y, size_t map_x) {
    char path[SAVE_PATH_MAX + 32];
    snprintf(path, sizeof(path), "%slocal_%zu_%zu.dat", g_SavePath, map_y, map_x);
    FILE *f = fopen(path, "rb");
    if (!f)
        return false;

    char magic[4];
    if (fread(magic, 1, 4, f) != 4 || memcmp(magic, LOCAL_MAP_FILE_MAGIC, 4) != 0) {
        fclose(f);
        return false;
    }
    int fy = fgetc(f);
    int fx = fgetc(f);
    if (fy == EOF || fx == EOF || (size_t)fy != map_y || (size_t)fx != map_x) {
        fclose(f);
        return false;
    }

    struct MapLocal *m = &g_LocalMaps[map_y][map_x];
    m->x = map_x;
    m->y = map_y;
    m->is_generated = true;

    for (size_t i = 0; i < MAP_LOCAL_HEIGHT; i++) {
        for (size_t j = 0; j < MAP_LOCAL_WIDTH; j++) {
            int t = fgetc(f);
            int fl = fgetc(f);
            if (t == EOF || fl == EOF) {
                fclose(f);
                logError("Truncated local map file %s #load", path);
                return false;
            }
            m->cells[i][j].type = (enum E_LocalCellType)(unsigned char)t;
            m->cells[i][j].flags = (unsigned char)fl;
        }
    }
    fclose(f);
    logMessage("Loaded local map %zu,%zu from %s #load", map_y, map_x, path);
    return true;
}

bool map_local_save(FILE *_fptr, struct MapLocal *_pLocalMap){
    (void)_fptr;
    (void)_pLocalMap;
    return true;
}

bool map_local_load(FILE *_fptr, struct MapLocal *_pLocalMap){
    (void)_fptr;
    (void)_pLocalMap;
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


