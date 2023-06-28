#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/curses.h"

#include "common.h"
#include "daytime.h"
#include "debug.h"
#include "hero.h"
#include "mainloop.h"
#include "map.h"
#include "screen.h"

void init_all() {
    srand(time(0));         // init seed for rand

#ifdef DEBUG
    g_Debug = true;
#endif // DEBUG

    initscr();              // curses init
    sc_init();              // init screen
    keypad(stdscr, TRUE);   // enable KEY_UP/KEY_DOWN/KEY_RIGHT/KEY_LEFT
    noecho();               // turn off the key echo

    map_init();             // init map
    hero_init();            // init hero
    init_time();
}

void uninit_all() {
    sc_uninit();            // uninit screen
    endwin();               // uninit curses
}

int main(int argc, char **argv)
{
    init_all();

    main_loop();

    uninit_all();

    return 0;
}


