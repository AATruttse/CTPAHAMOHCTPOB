#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

/* Menu action when user made a choice */
enum MenuAction {
    MENU_ACTION_NONE = 0,
    MENU_ACTION_NEW,   /* start new game - name_buf filled */
    MENU_ACTION_LOAD,  /* load game - selected_path filled */
    MENU_ACTION_QUIT
};

/* Run start menu (draw + handle input). Blocks until user picks an action.
 * name_buf/name_size: for MENU_ACTION_NEW, character name is written here.
 * selected_path/path_size: for MENU_ACTION_LOAD, path like "./save/name/" is written here.
 * Returns the chosen action. */
enum MenuAction menu_run(char *name_buf, size_t name_size,
                         char *selected_path, size_t path_size);

#endif // MENU_H_INCLUDED
