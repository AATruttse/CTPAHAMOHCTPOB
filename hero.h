#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED

#include <stdbool.h>

#include "common.h"

struct Hero {
    size_t      map_x;
    size_t      map_y;
    size_t      local_map_x;
    size_t      local_map_y;

    enum E_DIR  map_move_dir;
};

struct Hero g_Hero;

void hero_check_visibility();
void hero_init();
void hero_enter();
void hero_draw();
void hero_step(enum E_DIR _dir);
bool hero_save(FILE *fptr);
bool hero_load(FILE *fptr);
#endif // HERO_H_INCLUDED
