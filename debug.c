#include "common.h"
#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "hero.h"
#include "map_global.h"
#include "screen.h"

#define LOG_FILE "ctpaha_monctrob.log"

#define DEBUG_X0 (MAP_WIDTH + 1)
#define DEBUG_Y0 20

#define DEBUG_HEIGHT 10
#define DEBUG_WIDTH 35

char debug_buf[DEBUG_HEIGHT][DEBUG_WIDTH];

void debug_draw() {
    for (size_t i = 0; i < DEBUG_HEIGHT; i++) {
        for (size_t j = 0; j < DEBUG_WIDTH; j++) {
            debug_buf[i][j] = ' ';
        }
    }

    if (!g_Debug) {
        sprintf(debug_buf[1], "    Hero x = %lu", (unsigned long)g_Hero.map_x);
        sprintf(debug_buf[2], "    Hero y = %lu", (unsigned long)g_Hero.map_y);
    }

    for (size_t i = 0; i < DEBUG_HEIGHT; i++) {
        for (size_t j = 0; j < DEBUG_WIDTH; j++) {
            if (debug_buf[i][j]) {
                g_scrBuf[i + DEBUG_Y0][j + DEBUG_X0].ch = debug_buf[i][j];
            }
        }
    }
}
void debug_init() {
}

void debug_uninit() {
}

void debug_log(char *_debug_string) {
    FILE *fptr;
    fptr = fopen(LOG_FILE, "a");

    if(fptr == NULL)
    {
        eprintf("Error! Can't open file %s\n", LOG_FILE);
        exit(1);
    }

    fprintf(fptr,"%s\n",_debug_string);
    fclose(fptr);
}

#endif // DEBUG
