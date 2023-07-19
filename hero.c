#include <stdlib.h>

#include "include/curses.h"

#include "common.h"
#include "daytime.h"
#include "hero.h"
#include "map.h"
#include "map_local.h"
#include "screen.h"

#define HERO_START_AREA 10
#define HERO_VISIBILITY_RADIUS 3
#define HERO_LOCAL_VISIBILITY_RADIUS 10

#define HERO_CHARACTER '@'

/////////////////////////////////////////////////
/// Inits all variables for hero
/////////////////////////////////////////////////
void hero_init() {
    unsigned int shore_x1 = (MAP_WIDTH / 2) - 1;                 // left river's shore x coordinate
    unsigned int shore_x2 = shore_x1 + 3;                        // right river's shore x coordinate

    unsigned int hero_x0 = (rand() % 2) ? shore_x1 : shore_x2;   // left or right shore?
    unsigned int hero_y0 = 1 + (rand() % HERO_START_AREA);       // place hero on shore

    g_Hero.map_x = hero_x0;
    g_Hero.map_y = hero_y0;

    hero_check_visibility();
}

void hero_draw() {
    if (g_Mode == EGM_MAP_GLOBAL) {
        g_scrBuf[g_Hero.map_y + MAP_Y0][g_Hero.map_x + MAP_X0].ch = HERO_CHARACTER;
        g_scrBuf[g_Hero.map_y + MAP_Y0][g_Hero.map_x + MAP_X0].ch_color = COLOR_BRIGHTRED;
    } else if (g_Mode == EGM_MAP_LOCAL){
        g_scrBuf[g_Hero.local_map_y + MAP_Y0][g_Hero.local_map_x + MAP_X0].ch = HERO_CHARACTER;
        g_scrBuf[g_Hero.local_map_y + MAP_Y0][g_Hero.local_map_x + MAP_X0].ch_color = COLOR_BRIGHTRED;
    }
}

void hero_global_step(enum E_DIR _dir) {
    unsigned int target_x = g_Hero.map_x;
    unsigned int target_y = g_Hero.map_y;

    switch (_dir) {
    case EDIR_N:
        target_y--;
        break;
    case EDIR_NE:
        target_x++;
        target_y--;
        break;
    case EDIR_E:
        target_x++;
        break;
    case EDIR_SE:
        target_x++;
        target_y++;
        break;
    case EDIR_S:
        target_y++;
        break;
    case EDIR_SW:
        target_x--;
        target_y++;
        break;
    case EDIR_W:
        target_x--;
        break;
    case EDIR_NW:
        target_x--;
        target_y--;
        break;
    default:
        break;
    }

    if (g_Map[target_y][target_x].type == ECT_MOUNTAIN) {
        return;
    }
    if (g_Map[target_y][target_x].type == ECT_LAKE || g_Map[target_y][target_x].type == ECT_RIVER) {
        return;
    }
    g_Hero.map_x = target_x;
    g_Hero.map_y = target_y;

    g_Hero.map_move_dir = _dir;

    time_advance(1);
}

void hero_local_step(enum E_DIR _dir) {
    unsigned int target_x = g_Hero.local_map_x;
    unsigned int target_y = g_Hero.local_map_y;

    switch (_dir) {
    case EDIR_N:
        target_y--;
        break;
    case EDIR_NE:
        target_x++;
        target_y--;
        break;
    case EDIR_E:
        target_x++;
        break;
    case EDIR_SE:
        target_x++;
        target_y++;
        break;
    case EDIR_S:
        target_y++;
        break;
    case EDIR_SW:
        target_x--;
        target_y++;
        break;
    case EDIR_W:
        target_x--;
        break;
    case EDIR_NW:
        target_x--;
        target_y--;
        break;
    default:
        break;
    }

    g_Hero.local_map_x = target_x;
    g_Hero.local_map_y = target_y;
}

void hero_step(enum E_DIR _dir) {
    if (g_Mode == EGM_MAP_GLOBAL) {
        hero_global_step(_dir);
    } else if (g_Mode == EGM_MAP_LOCAL){
        hero_local_step(_dir);
    }
}

void hero_enter() {
    if (g_Mode == EGM_MAP_GLOBAL) {
        if (!g_LocalMaps[g_Hero.map_y][g_Hero.map_x].is_generated) {
            map_local_init(&g_Map[g_Hero.map_y][g_Hero.map_x], &g_LocalMaps[g_Hero.map_y][g_Hero.map_x]);
        }

        g_Mode = EGM_MAP_LOCAL;

        size_t border_cell_sector_wide = (MAP_LOCAL_WIDTH + MAP_LOCAL_HEIGHT - 1) / 4;
        size_t border_cell = rand() % border_cell_sector_wide;
        border_cell += (unsigned int)g_Hero.map_move_dir * border_cell_sector_wide + MAP_LOCAL_WIDTH / 3;
        if (border_cell < MAP_LOCAL_WIDTH) {
                g_Hero.local_map_y = 0;
                g_Hero.local_map_x = border_cell;
        } else if (border_cell < MAP_LOCAL_HEIGHT + MAP_LOCAL_WIDTH) {
                g_Hero.local_map_y = border_cell - MAP_LOCAL_WIDTH;
                g_Hero.local_map_x = MAP_LOCAL_WIDTH - 1;
        } else if (border_cell < MAP_LOCAL_HEIGHT + MAP_LOCAL_WIDTH * 2) {
                g_Hero.local_map_y = MAP_LOCAL_HEIGHT - 1;
                g_Hero.local_map_x = MAP_LOCAL_WIDTH - border_cell + MAP_LOCAL_WIDTH + MAP_LOCAL_HEIGHT;
        } else {
                g_Hero.local_map_y = border_cell - MAP_LOCAL_HEIGHT - MAP_LOCAL_WIDTH - MAP_LOCAL_WIDTH;
                g_Hero.local_map_x = 0;
        }
    } else if (g_Mode == EGM_MAP_LOCAL) {
        if (g_Hero.local_map_y == 0 ||
            g_Hero.local_map_y == MAP_LOCAL_HEIGHT - 1 ||
            g_Hero.local_map_x == 0 ||
            g_Hero.local_map_x == MAP_LOCAL_WIDTH - 1) {
                g_Mode = EGM_MAP_GLOBAL;
            }
    }
}

void hero_check_visibility() {
    unsigned int distance_sqr, radius_sqr;

    if (g_Mode == EGM_MAP_GLOBAL) {
        radius_sqr = (HERO_VISIBILITY_RADIUS + 1) * (HERO_VISIBILITY_RADIUS + 1);
        for (size_t i = 0; i < MAP_HEIGHT; i++) {
            for (size_t j = 0; j < MAP_WIDTH; j++) {
                distance_sqr = (g_Hero.map_y - i) * (g_Hero.map_y - i) + (g_Hero.map_x - j) * (g_Hero.map_x - j);
                if (distance_sqr < radius_sqr) {
                        g_Map[i][j].flags |= EXPLORED_FLAG;
                        g_Map[i][j].flags |= VISIBLE_FLAG;
                }
                else {
                        g_Map[i][j].flags &= ~VISIBLE_FLAG;
                }
            }
        }
    } else if (g_Mode == EGM_MAP_LOCAL) {
        radius_sqr = (HERO_LOCAL_VISIBILITY_RADIUS + 1) * (HERO_LOCAL_VISIBILITY_RADIUS + 1);
        for (size_t i = 0; i < MAP_LOCAL_HEIGHT; i++) {
            for (size_t j = 0; j < MAP_LOCAL_WIDTH; j++) {
                distance_sqr = (g_Hero.local_map_y - i) * (g_Hero.local_map_y - i) + (g_Hero.local_map_x - j) * (g_Hero.local_map_x - j);
                if (distance_sqr < radius_sqr) {
                        g_LocalMaps[g_Hero.map_y][g_Hero.map_x].cells[i][j].flags |= EXPLORED_FLAG;
                        g_LocalMaps[g_Hero.map_y][g_Hero.map_x].cells[i][j].flags |= VISIBLE_FLAG;
                }
                else {
                        g_LocalMaps[g_Hero.map_y][g_Hero.map_x].cells[i][j].flags &= ~VISIBLE_FLAG;
                }
            }
        }

    }
}

bool hero_save(FILE *fptr) {
    fprintf(fptr, "%lu %lu %lu %lu %lu\n",
            (unsigned long)g_Hero.map_x,
            (unsigned long)g_Hero.map_y,
            (unsigned long)g_Hero.local_map_x,
            (unsigned long)g_Hero.local_map_y,
            (unsigned long)g_Hero.map_move_dir
            );
    if (ferror (fptr)) {
        return false;
    }

    return true;
}

bool hero_load(FILE *fptr) {
    unsigned long temp_x, temp_y, temp_local_x, temp_local_y, temp_move_dir;
    fscanf(fptr, "%lu %lu %lu %lu %lu\n", &temp_x, &temp_y, &temp_local_x, &temp_local_y, &temp_move_dir);
    if (ferror (fptr)) {
        return false;
    }

    g_Hero.map_x = temp_x;
    g_Hero.map_y = temp_y;
    g_Hero.local_map_x = temp_local_x;
    g_Hero.local_map_y = temp_local_y;
    g_Hero.map_move_dir = temp_move_dir;

    return true;
}

