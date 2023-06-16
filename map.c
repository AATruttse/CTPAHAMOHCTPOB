#include "include/curses.h"

#include "common.h"
#include "map.h"
#include "screen.h"

char get_cell_char(struct MapCell _cell) {
    /* non-explored map cells shown blank*/
    if (!_cell.is_explored) {
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

void map_init() {
    int max_height = MAP_HEIGHT - 1;
    int max_width = MAP_WIDTH - 1;

    int river_x1 = MAP_WIDTH / 2;
    int river_x2 = river_x1 + 1;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
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
}

void map_draw() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch = get_cell_char(g_Map[i][j]);
            g_scrBuf[i + MAP_Y0][j + MAP_X0].ch_color = get_cell_color(g_Map[i][j]);
        }
    }
}
