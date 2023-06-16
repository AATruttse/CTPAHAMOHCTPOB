#include "common.h"
#ifdef DEBUG
#include <stdio.h>

#include "debug.h"
#include "hero.h"
#include "map.h"
#include "screen.h"

#define DEBUG_X0 (MAP_WIDTH + 1)
#define DEBUG_Y0 0

#define DEBUG_HEIGHT 20
#define DEBUG_WIDTH 35

void debug_draw() {
    char debug_buf[DEBUG_HEIGHT][DEBUG_WIDTH];

    for (int i = 0; i < DEBUG_HEIGHT; i++) {
        for (int j = 0; j < DEBUG_WIDTH; j++) {
            debug_buf[i][j] = ' ';
        }
    }

    if (!g_Debug) {
        sprintf(debug_buf[1], "    Hero x = %d", g_Hero.map_x);
        sprintf(debug_buf[2], "    Hero y = %d", g_Hero.map_y);
    }

    for (int i = 0; i < DEBUG_HEIGHT; i++) {
        for (int j = 0; j < DEBUG_WIDTH; j++) {
            if (debug_buf[i][j]) {
                g_scrBuf[i + DEBUG_Y0][j + DEBUG_X0].ch = debug_buf[i][j];
            }
        }
    }
}

#endif // DEBUG
