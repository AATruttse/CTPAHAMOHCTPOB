#include <stdlib.h>

#include "include/curses.h"

#include "common.h"
#include "hero.h"
#include "map.h"
#include "screen.h"

#define HERO_START_AREA 10
#define HERO_VISIBILITY_RADIUS 3

void hero_init() {
    /* river shores x coordinate */
    int shore_x1 = (MAP_WIDTH / 2) - 1;
    int shore_x2 = shore_x1 + 3;

    int hero_x0 = (rand() % 2) ? shore_x1 : shore_x2;   /* left or right shore? */
    int hero_y0 = 1 + (rand() % HERO_START_AREA);       /* place on shore */

    g_Hero.map_x = hero_x0;
    g_Hero.map_y = hero_y0;

    hero_check_visibility();
}

void hero_draw() {
    g_scrBuf[g_Hero.map_y + MAP_Y0][g_Hero.map_x + MAP_X0].ch = '@';
    g_scrBuf[g_Hero.map_y + MAP_Y0][g_Hero.map_x + MAP_X0].ch_color = COLOR_BRIGHTWHITE;
}

void hero_step(enum E_DIR _dir) {
    int target_x = g_Hero.map_x;
    int target_y = g_Hero.map_y;

    switch (_dir) {
    case E_DIR_N:
        target_y--;
        break;
    case E_DIR_NE:
        target_x++;
        target_y--;
        break;
    case E_DIR_E:
        target_x++;
        break;
    case E_DIR_SE:
        target_x++;
        target_y++;
        break;
    case E_DIR_S:
        target_y++;
        break;
    case E_DIR_SW:
        target_x--;
        target_y++;
        break;
    case E_DIR_W:
        target_x--;
        break;
    case E_DIR_NW:
        target_x--;
        target_y--;
        break;
    default:
        break;
    }

    if (g_Map[target_y][target_x].type == ECT_MOUNTAIN) {
        return;
    }

    g_Hero.map_x = target_x;
    g_Hero.map_y = target_y;
}

void hero_check_visibility() {
    int distance_sqr;
    int radius_sqr = (HERO_VISIBILITY_RADIUS + 1) * (HERO_VISIBILITY_RADIUS + 1);

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            distance_sqr = (g_Hero.map_y - i) * (g_Hero.map_y - i) + (g_Hero.map_x - j) * (g_Hero.map_x - j);
            if (distance_sqr < radius_sqr) {
                    g_Map[i][j].is_explored = true;
                    g_Map[i][j].is_visible = true;
            }
            else {
                    g_Map[i][j].is_visible = false;
            }
        }
    }
}

