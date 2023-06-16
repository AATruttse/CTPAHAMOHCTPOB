#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/curses.h"

#include "common.h"
#include "debug.h"
#include "hero.h"
#include "mainloop.h"
#include "map.h"
#include "screen.h"

void init_all() {
    srand(time(0));

#ifdef DEBUG
    g_Debug = true;
#endif // DEBUG

    initscr();
    sc_init();
    keypad(stdscr, TRUE);   /* enable KEY_UP/KEY_DOWN/KEY_RIGHT/KEY_LEFT */
    noecho();

    map_init();
    hero_init();
}

void uninit_all() {
    sc_uninit();
    endwin();
}

int main(int argc, char **argv)
{
    init_all();

    main_loop();

    uninit_all();

    return 0;
}


