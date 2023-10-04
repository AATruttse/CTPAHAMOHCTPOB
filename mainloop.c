#include <stdbool.h>

#include "include/curses.h"

#include "common.h"
#include "daytime.h"
#include "debug.h"
#include "hero.h"
#include "logs.h"
#include "mainloop.h"
#include "map_global.h"
#include "map_local.h"
#include "savegame.h"
#include "screen.h"

void main_draw() {
    logMessage("Start drawing! #draw");

    if (g_Mode == EGM_MAP_GLOBAL) {
        map_global_draw();
    } // if (g_Mode == EGM_MAP_GLOBAL)
    else if (g_Mode == EGM_MAP_LOCAL) {
        map_local_draw(&g_LocalMaps[g_Hero.map_y][g_Hero.map_x]);
    } // else if (g_Mode == EGM_MAP_LOCAL)

    hero_draw();

    time_draw();

#ifdef DEBUG
    debug_draw();
#endif // DEBUG

    sc_flushBuf();
    logMessage("Drawing ok! #draw");
} // void main_draw()

bool loop_command() {
    int ch = getch();
    logMessage("Get command %d('%c')! #command", ch, ch);
    switch (ch) {
        case KEY_UP:
        case KEY_A2:
            hero_step(EDIR_N);
            break;
        case KEY_DOWN:
        case KEY_C2:
            hero_step(EDIR_S);
            break;
        case KEY_RIGHT:
        case KEY_B3:
            hero_step(EDIR_E);
            break;
        case KEY_LEFT:
        case KEY_B1:
            hero_step(EDIR_W);
            break;
        case KEY_A1:
            hero_step(EDIR_NW);
            break;
        case KEY_A3:
            hero_step(EDIR_NE);
            break;
        case KEY_C1:
            hero_step(EDIR_SW);
            break;
        case KEY_C3:
            hero_step(EDIR_SE);
            break;
        case KEY_ENTER:
        case KEY_B2:
        case PADENTER:
            hero_enter();
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
    } // switch (ch)

    return true;
} // bool loop_command()

void main_checks() {
    logMessage("Start checks! #checks");
    hero_check_visibility();
    logMessage("Checks ok! #checks");
} // void main_checks()

void main_loop() {
    do { // while (loop_command())
        logMessage("Start loop! #loop");

        main_checks();
        main_draw();

        save_all();
        logMessage("Loop ok! #loop");
    } while (loop_command());
} // void main_loop()
