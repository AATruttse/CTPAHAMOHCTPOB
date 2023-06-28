#include <stdlib.h>
#include <stdio.h>

#include "daytime.h"
#include "screen.h"

#define DAYTIME_X 1
#define DAYTIME_Y 26
#define TIME_WIDTH 20

void init_time(){
    g_DayTime = rand() % 24;
}

void draw_time() {
   char time_buf[TIME_WIDTH];

    for (size_t i = 0; i < TIME_WIDTH; i++) {
        time_buf[i] = ' ';
    }

    sprintf(time_buf, " Time: %02d", g_DayTime);

    for (size_t i = 0; i < TIME_WIDTH; i++) {
        if (time_buf[i]) {
            g_scrBuf[DAYTIME_Y][i + DAYTIME_X].ch = time_buf[i];
        }
    }

}
