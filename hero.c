#include <stdlib.h>

#include "include/curses.h"

#include "common.h"
#include "daytime.h"
#include "hero.h"
#include "logs.h"
#include "map_global.h"
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
    logMessage("Hero init started! #init #hero");

    unsigned int shore_x1 = (MAP_WIDTH / 2) - 1;                 // left river's shore x coordinate
    unsigned int shore_x2 = shore_x1 + 3;                        // right river's shore x coordinate

    unsigned int hero_x0 = (rand() % 2) ? shore_x1 : shore_x2;   // left or right shore?
    unsigned int hero_y0 = 1 + (rand() % HERO_START_AREA);       // place hero on shore

    g_Hero.map_x = hero_x0;
    g_Hero.map_y = hero_y0;

    logMessage("Hero starting place: (%d,%d)! #init #hero", g_Hero.map_x, g_Hero.map_y);

    hero_check_visibility();
    logMessage("Hero init ok! #init #hero");
}

void hero_draw() {
    if (g_Mode == EGM_MAP_GLOBAL) {
        g_scrBuf[g_Hero.map_y + MAP_Y0][g_Hero.map_x + MAP_X0].ch = HERO_CHARACTER;
        g_scrBuf[g_Hero.map_y + MAP_Y0][g_Hero.map_x + MAP_X0].ch_color = COLOR_BRIGHTRED;
    } else if (g_Mode == EGM_MAP_LOCAL) {
        int sy = (int)g_Hero.local_map_y - g_LocalViewY0;
        int sx = (int)g_Hero.local_map_x - g_LocalViewX0;
        if (sy >= 0 && sy < MAP_HEIGHT && sx >= 0 && sx < MAP_WIDTH) {
            g_scrBuf[sy + MAP_Y0][sx + MAP_X0].ch = HERO_CHARACTER;
            g_scrBuf[sy + MAP_Y0][sx + MAP_X0].ch_color = COLOR_BRIGHTRED;
        }
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
        logMessage("Hero tried to step on global map from (%d,%d) to (%d,%d) but (%d,%d) is mountain! #hero", g_Hero.map_x, g_Hero.map_y, target_x, target_y, target_x, target_y );
        return;
    }
    if (g_Map[target_y][target_x].type == ECT_LAKE || g_Map[target_y][target_x].type == ECT_RIVER) {
        logMessage("Hero tried to step on global map from (%d,%d) to (%d,%d) but (%d,%d) is water! #hero", g_Hero.map_x, g_Hero.map_y, target_x, target_y, target_x, target_y );
        return;
    }

    g_Hero.map_move_dir = (4 + (unsigned int)_dir) % 8;

    logMessage("Hero stepped on global map from (%d,%d) to (%d,%d). move_dir is %s.#hero", g_Hero.map_x, g_Hero.map_y, target_x, target_y, STR_DIR[g_Hero.map_move_dir] );

    g_Hero.map_x = target_x;
    g_Hero.map_y = target_y;


    time_advance(1);
}

void hero_local_step(enum E_DIR _dir) {
    int target_x = (int)g_Hero.local_map_x;
    int target_y = (int)g_Hero.local_map_y;

    switch (_dir) {
    case EDIR_N:  target_y--; break;
    case EDIR_NE: target_x++; target_y--; break;
    case EDIR_E:  target_x++; break;
    case EDIR_SE: target_x++; target_y++; break;
    case EDIR_S:  target_y++; break;
    case EDIR_SW: target_x--; target_y++; break;
    case EDIR_W:  target_x--; break;
    case EDIR_NW: target_x--; target_y--; break;
    default: break;
    }

    /* Stepping off the map returns to global map */
    if (target_x < 0 || target_x >= (int)MAP_LOCAL_WIDTH ||
        target_y < 0 || target_y >= (int)MAP_LOCAL_HEIGHT) {
        logMessage("Hero left local map to global. #hero");
        g_Mode = EGM_MAP_GLOBAL;
        return;
    }

    logMessage("Hero stepped on local map from (%lu,%lu) to (%d,%d). #hero",
        (unsigned long)g_Hero.local_map_x, (unsigned long)g_Hero.local_map_y, target_x, target_y);

    g_Hero.local_map_x = (size_t)target_x;
    g_Hero.local_map_y = (size_t)target_y;
    time_advance(1);
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

        /* Entry position on local map border: center of the side we came from, or corner for diagonals */
        switch ((unsigned int)g_Hero.map_move_dir) {
        case 0:  /* came from N */ g_Hero.local_map_x = MAP_LOCAL_WIDTH / 2;  g_Hero.local_map_y = 0; break;
        case 1:  /* came from NE */ g_Hero.local_map_x = 0;                   g_Hero.local_map_y = MAP_LOCAL_HEIGHT - 1; break; /* SW corner */
        case 2:  /* came from E */ g_Hero.local_map_x = 0;                   g_Hero.local_map_y = MAP_LOCAL_HEIGHT / 2; break;
        case 3:  /* came from SE */ g_Hero.local_map_x = 0;                   g_Hero.local_map_y = 0; break; /* NW corner */
        case 4:  /* came from S */ g_Hero.local_map_x = MAP_LOCAL_WIDTH / 2;  g_Hero.local_map_y = MAP_LOCAL_HEIGHT - 1; break;
        case 5:  /* came from SW */ g_Hero.local_map_x = MAP_LOCAL_WIDTH - 1;  g_Hero.local_map_y = 0; break; /* NE corner */
        case 6:  /* came from W */ g_Hero.local_map_x = MAP_LOCAL_WIDTH - 1;   g_Hero.local_map_y = MAP_LOCAL_HEIGHT / 2; break;
        case 7:  /* came from NW */ g_Hero.local_map_x = MAP_LOCAL_WIDTH - 1; g_Hero.local_map_y = MAP_LOCAL_HEIGHT - 1; break; /* SE corner */
        default: g_Hero.local_map_x = MAP_LOCAL_WIDTH / 2;  g_Hero.local_map_y = MAP_LOCAL_HEIGHT - 1; break;
        }
    }
    /* In local mode, exit only by stepping off the border (handled in hero_local_step) */
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
    logMessage("Hero save started! #save #hero");

    fprintf(fptr, "%lu %lu %lu %lu %lu\n",
            (unsigned long)g_Hero.map_x,
            (unsigned long)g_Hero.map_y,
            (unsigned long)g_Hero.local_map_x,
            (unsigned long)g_Hero.local_map_y,
            (unsigned long)g_Hero.map_move_dir
            );
    if (ferror (fptr)) {
        logError("Can't save hero! #save #hero #error");
        return false;
    }

    logMessage("Hero save ok! #save #hero");
    return true;
}

bool hero_load(FILE *fptr) {
    logMessage("Hero load started! #load #hero");

    unsigned long temp_x, temp_y, temp_local_x, temp_local_y, temp_move_dir;
    fscanf(fptr, "%lu %lu %lu %lu %lu\n", &temp_x, &temp_y, &temp_local_x, &temp_local_y, &temp_move_dir);
    if (ferror (fptr)) {
        logError("Can't load hero! #load #hero #error");
        return false;
    }

    g_Hero.map_x = temp_x;
    g_Hero.map_y = temp_y;
    g_Hero.local_map_x = temp_local_x;
    g_Hero.local_map_y = temp_local_y;
    g_Hero.map_move_dir = temp_move_dir;

    logMessage("Hero load ok! #load #hero");
    return true;
}

