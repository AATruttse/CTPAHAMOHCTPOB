#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "daytime.h"
#include "debug.h"
#include "hero.h"
#include "map_global.h"
#include "map_local.h"
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

bool save_mode(FILE *fptr) {
    fprintf(fptr, "%lu\n", (unsigned long)g_Mode);
    if (ferror (fptr)) {
        return false;
    }

    return true;
}

bool load_mode(FILE *fptr) {
    unsigned long temp_mode;
    fscanf(fptr, "%lu\n", &temp_mode);
    if (ferror (fptr)) {
        return false;
    }

    g_Mode = temp_mode;

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
        map_global_save(file);
        maps_local_save_all(file);
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
        map_global_load(file);
        maps_local_load_all(file);
        fclose(file);
        return true;
    }

    return false;
}
