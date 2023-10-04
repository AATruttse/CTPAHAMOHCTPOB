#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "daytime.h"
#include "debug.h"
#include "hero.h"
#include "logs.h"
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
            logCritical("Unable to create directory %s! #save #crit", SAVE_PATH);
            return false;
        }
    } else {
        logCritical("Problem with directory %s! #save #crit", SAVE_PATH);
        return false;
    }

    return true;
}

bool save_mode(FILE *fptr) {
    fprintf(fptr, "%lu\n", (unsigned long)g_Mode);
    if (ferror (fptr)) {
        logCritical("Can't save mode! #save #crit");
        return false;
    }

    return true;
}

bool load_mode(FILE *fptr) {
    unsigned long temp_mode;
    fscanf(fptr, "%lu\n", &temp_mode);
    if (ferror (fptr)) {
        logCritical("Can't load mode! #load #crit");
        return false;
    }

    g_Mode = temp_mode;

    return true;

}

bool save_all() {
    logMessage("Save started! #save");

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
        logMessage("Save game to file %s ok! #load", path);
        return true;
    }

    logCritical("Can't save game to file %s! #save #crit", path);
    return false;
}

bool load_all() {
    logMessage("Load started! #load");

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
        logMessage("Load game from file %s ok! #load", path);
        return true;
    }

    logWarning("Can't load game from file %s! #load", path);

    return false;
}
