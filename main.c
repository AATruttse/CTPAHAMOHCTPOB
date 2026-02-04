#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/curses.h"

#include "common.h"
#include "daytime.h"
#include "debug.h"
#include "hero.h"
#include "logs.h"
#include "mainloop.h"
#include "map_gen_local.h"
#include "map_global.h"
#include "map_local.h"
#include "savegame.h"
#include "screen.h"

void init_game_fresh(void) {
    g_Mode = EGM_MAP_GLOBAL;
    map_global_init();
    maps_local_init_all();
    hero_init();
    time_init();
    init_gen_map_local();
    logMessage("New game init ok! #init");
}

void init_all() {
    logs_init();
    logMessage("Starting init! #init");

    setlocale(LC_ALL, "en_GB");
    srand((unsigned)time(0));

#ifdef DEBUG
    g_Debug = true;
#endif

    initscr();
    sc_init();
    keypad(stdscr, TRUE);
    noecho();

    g_Mode = EGM_START_MENU;
    init_gen_map_local();
    logMessage("Init ok! #init");
}

void uninit_all() {
    logMessage("Starting uninit! #uninit");

    sc_uninit();            // uninit screen
    endwin();               // uninit curses
    logMessage("Uninit ok! #uninit");

    logs_uninit();          // uninit logs
} // void uninit_all()

int main(int argc, char **argv)
{
    init_all();

    main_loop();

    uninit_all();

    return 0;
} // int main(int argc, char **argv)


