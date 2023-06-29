#include <stdbool.h>

#include "include/curses.h"

#include "common.h"
#include "daytime.h"
#include "debug.h"
#include "hero.h"
#include "mainloop.h"
#include "map.h"
#include "screen.h"

void main_draw() {
    map_draw();
    hero_draw();

    time_draw();

#ifdef DEBUG
    debug_draw();
#endif // DEBUG

    sc_flushBuf();
}

bool loop_command() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
            hero_step(EDIR_N);
            break;
        case KEY_DOWN:
            hero_step(EDIR_S);
            break;
        case KEY_RIGHT:
            hero_step(EDIR_E);
            break;
        case KEY_LEFT:
            hero_step(EDIR_W);
            break;
#ifdef DEBUG
        case 'd':
        case 'D':
            g_Debug = !g_Debug;
            break;
#endif // DEBUG
        case 'q':
        case 'Q':
            return false;
            break;
    }

    return true;
}

void main_checks() {
    hero_check_visibility();
}

void main_loop() {
    do {
        main_checks();
        main_draw();
    } while (loop_command());
}
