#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED

#include "common.h"

struct Hero {
    int map_x;
    int map_y;
};

struct Hero g_Hero;

void hero_check_visibility();
void hero_init();
void hero_draw();
void hero_step(enum E_DIR _dir);
#endif // HERO_H_INCLUDED
