#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

#include "common.h"
#include "daytime.h"
#include "debug.h"
#include "hero.h"
#include "logs.h"
#include "map_global.h"
#include "map_local.h"
#include "savegame.h"

char g_SavePath[SAVE_PATH_MAX] = "./save/default/";

void set_save_path(const char *path) {
    strncpy(g_SavePath, path, SAVE_PATH_MAX - 1);
    g_SavePath[SAVE_PATH_MAX - 1] = '\0';
    if (g_SavePath[strlen(g_SavePath) - 1] != '/') {
        size_t len = strlen(g_SavePath);
        if (len < SAVE_PATH_MAX - 1) {
            g_SavePath[len] = '/';
            g_SavePath[len + 1] = '\0';
        }
    }
}

static bool ensure_base_dir(void) {
    DIR *dir = opendir(SAVE_BASE);
    if (dir) {
        closedir(dir);
        return true;
    }
    if (ENOENT == errno) {
        if (MKDIR(SAVE_BASE) != 0 && errno != EEXIST) {
            logCritical("Unable to create directory %s! #save #crit", SAVE_BASE);
            return false;
        }
        return true;
    }
    logCritical("Problem with directory %s! #save #crit", SAVE_BASE);
    return false;
}

bool create_save_dir_for_character(const char *name) {
    if (!name || !name[0] || !ensure_base_dir())
        return false;
    char try_path[SAVE_PATH_MAX];
    char folder[SAVE_NAME_MAX];
    for (int n = 0; n < 1000; n++) {
        if (n == 0)
            snprintf(folder, sizeof(folder), "%s", name);
        else
            snprintf(folder, sizeof(folder), "%s%d", name, n);
        snprintf(try_path, sizeof(try_path), "%s%s/", SAVE_BASE, folder);
        DIR *dir = opendir(try_path);
        if (dir) {
            closedir(dir);
            continue; /* folder exists, try next */
        }
        if (ENOENT == errno) {
            /* mkdir without trailing slash for compatibility */
            try_path[strlen(try_path) - 1] = '\0';
            if (MKDIR(try_path) != 0) {
                try_path[strlen(try_path)] = '/';
                logCritical("Unable to create directory %s! #save #crit", try_path);
                return false;
            }
            try_path[strlen(try_path)] = '/';
            set_save_path(try_path);
            logMessage("Created save folder %s #save", try_path);
            return true;
        }
    }
    return false;
}

int list_save_dirs(char names[][SAVE_NAME_MAX], int max_count) {
    DIR *base = opendir(SAVE_BASE);
    if (!base)
        return 0;
    int count = 0;
    struct dirent *ent;
    while (count < max_count && (ent = readdir(base)) != NULL) {
        if (ent->d_name[0] == '.' && (ent->d_name[1] == '\0' || (ent->d_name[1] == '.' && ent->d_name[2] == '\0')))
            continue;
        char path[SAVE_PATH_MAX];
        snprintf(path, sizeof(path), "%s%s/%s", SAVE_BASE, ent->d_name, SAVE_FILE);
        FILE *f = fopen(path, "r");
        if (f) {
            fclose(f);
            strncpy(names[count], ent->d_name, SAVE_NAME_MAX - 1);
            names[count][SAVE_NAME_MAX - 1] = '\0';
            count++;
        }
    }
    closedir(base);
    return count;
}

bool check_dir() {
    DIR *dir = opendir(g_SavePath);
    if (dir) {
        closedir(dir);
        return true;
    }
    if (ENOENT == errno) {
        size_t len = strlen(g_SavePath);
        if (len > 0 && g_SavePath[len - 1] == '/')
            g_SavePath[len - 1] = '\0';
        if (MKDIR(g_SavePath) != 0) {
            if (len > 0)
                g_SavePath[len - 1] = '/';
            logCritical("Unable to create directory %s! #save #crit", g_SavePath);
            return false;
        }
        if (len > 0)
            g_SavePath[len - 1] = '/';
    } else {
        logCritical("Problem with directory %s! #save #crit", g_SavePath);
        return false;
    }
    return true;
}

bool save_mode(FILE *fptr) {
    fprintf(fptr, "%lu\n", (unsigned long)g_Mode);
    if (ferror(fptr)) {
        logCritical("Can't save mode! #save #crit");
        return false;
    }
    return true;
}

bool load_mode(FILE *fptr) {
    unsigned long temp_mode;
    fscanf(fptr, "%lu\n", &temp_mode);
    if (ferror(fptr)) {
        logCritical("Can't load mode! #load #crit");
        return false;
    }
    g_Mode = temp_mode;
    return true;
}

bool save_all() {
    logMessage("Save started! #save");
    if (!check_dir())
        return false;
    if (g_Mode == EGM_MAP_LOCAL)
        map_local_save_to_file(g_Hero.map_y, g_Hero.map_x);
    char path[SAVE_PATH_MAX + 32];
    snprintf(path, sizeof(path), "%s%s", g_SavePath, SAVE_FILE);
    FILE *file = fopen(path, "w");
    if (file) {
        hero_save(file);
        time_save(file);
        map_global_save(file);
        maps_local_save_all(file);
        fclose(file);
        logMessage("Save game to file %s ok! #save", path);
        return true;
    }
    logCritical("Can't save game to file %s! #save #crit", path);
    return false;
}

bool load_all() {
    logMessage("Load started! #load");
    maps_local_init_all();
    char path[SAVE_PATH_MAX + 32];
    snprintf(path, sizeof(path), "%s%s", g_SavePath, SAVE_FILE);
    FILE *file = fopen(path, "r");
    if (file) {
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
