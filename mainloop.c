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
#include "menu.h"
#include "savegame.h"
#include "screen.h"

extern void init_game_fresh(void);

void main_draw() {
    logMessage("Start drawing! #draw");

    if (g_Mode == EGM_START_MENU) {
        /* Menu draws itself in menu_run */
        return;
    }
    if (g_Mode == EGM_MAP_GLOBAL) {
        map_global_draw();
    } else if (g_Mode == EGM_MAP_LOCAL) {
        map_local_draw(&g_LocalMaps[g_Hero.map_y][g_Hero.map_x]);
    }

    hero_draw();
    time_draw();

#ifdef DEBUG
    debug_draw();
#endif

    sc_flushBuf();
    logMessage("Drawing ok! #draw");
}

bool loop_command() {
    if (g_Mode == EGM_START_MENU) {
        char name_buf[SAVE_NAME_MAX];
        char selected_path[SAVE_PATH_MAX];
        enum MenuAction action = menu_run(name_buf, sizeof(name_buf),
                                          selected_path, sizeof(selected_path));
        if (action == MENU_ACTION_QUIT)
            return false;
        if (action == MENU_ACTION_NEW) {
            if (create_save_dir_for_character(name_buf)) {
                init_game_fresh();
            }
            return true;
        }
        if (action == MENU_ACTION_LOAD) {
            set_save_path(selected_path);
            if (load_all()) {
                /* load_all already set g_Mode from file; ensure we're in game */
                if (g_Mode != EGM_MAP_GLOBAL && g_Mode != EGM_MAP_LOCAL)
                    g_Mode = EGM_MAP_GLOBAL;
                /* If we loaded while in local map, init current local map from file or generate */
                if (g_Mode == EGM_MAP_LOCAL)
                    map_local_init(&g_Map[g_Hero.map_y][g_Hero.map_x],
                        &g_LocalMaps[g_Hero.map_y][g_Hero.map_x]);
            }
            return true;
        }
        return true;
    }

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
    if (g_Mode == EGM_START_MENU)
        return;
    logMessage("Start checks! #checks");
    hero_check_visibility();
    logMessage("Checks ok! #checks");
}

void main_loop() {
    do {
        logMessage("Start loop! #loop");
        main_checks();
        main_draw();

        if (g_Mode != EGM_START_MENU)
            save_all();
        logMessage("Loop ok! #loop");
    } while (loop_command());
}
