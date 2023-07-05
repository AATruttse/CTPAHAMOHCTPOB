#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "daytime.h"
#include "debug.h"
#include "hero.h"
#include "map.h"
#include "savegame.h"

bool check_dir() {
    DIR* dir = opendir(SAVE_PATH);
    if (dir) {
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        if (mkdir(SAVE_PATH)) {
            eprintf("Unable to create directory %s\n", SAVE_PATH);
            return false;
        }
    } else {
        eprintf("Problem with directory %s\n", SAVE_PATH);
        return false;
    }

    return true;
}

bool save_all() {
    if (!check_dir()) {
        return false;
    }

    char path[255];
    strcpy(path, SAVE_PATH);
    strcat(path, SAVE_FILE);

    FILE *file;
    if ((file = fopen(path, "w")))
    {
        hero_save(file);
        time_save(file);
        map_save(file);
        fclose(file);
        return true;
    }

    eprintf("Can't save game to %s\n", SAVE_PATH);
    return false;
}

bool load_all() {
    char path[255];
    strcpy(path, SAVE_PATH);
    strcat(path, SAVE_FILE);

    FILE *file;
    if ((file = fopen(path, "r")))
    {
        hero_load(file);
        time_load(file);
        map_load(file);
        fclose(file);
        return true;
    }

    return false;
}
