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

void init_all() {
    logs_init();            // init logs
    logMessage("Starting init! #init");

    setlocale(LC_ALL, "en_GB");

    srand(time(0));         // init seed for rand

#ifdef DEBUG
    g_Debug = true;
#endif // DEBUG

    initscr();              // curses init
    sc_init();              // init screen
    keypad(stdscr, TRUE);   // enable KEY_UP/KEY_DOWN/KEY_RIGHT/KEY_LEFT
    noecho();               // turn off the key echo

#ifndef NOLOAD
    if (!load_all()) {
#endif // NOLOAD
        g_Mode = EGM_MAP_GLOBAL;

        map_global_init();      // init map
        maps_local_init_all();  // init local maps
        hero_init();            // init hero
        time_init();            // init time
        init_gen_map_local();   // init local map generator
#ifndef NOLOAD
    } // if (!load_all())
    logMessage("Init ok! #init");
#endif // NOLOAD
} // void init_all()

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


