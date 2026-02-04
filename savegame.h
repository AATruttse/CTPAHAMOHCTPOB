#ifndef SAVEGAME_H_INCLUDED
#define SAVEGAME_H_INCLUDED

#define SAVE_BASE     "./save/"
#define SAVE_FILE     "cmnctpb.svg"
#define SAVE_PATH_MAX 256
#define SAVE_NAME_MAX 64
#define SAVE_LIST_MAX 32

/* Current character's save directory (e.g. "./save/hero/") - always ends with / */
extern char g_SavePath[SAVE_PATH_MAX];

void set_save_path(const char *path);
/* Create save folder for new character: tries name, name1, name2...; sets g_SavePath. Returns true on success. */
bool create_save_dir_for_character(const char *name);
/* List save folder names (subdirs of SAVE_BASE that contain SAVE_FILE). Returns count, fills names[]. */
int list_save_dirs(char names[][SAVE_NAME_MAX], int max_count);

bool save_all();
bool load_all();

#endif // SAVEGAME_H_INCLUDED
