#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "include/curses.h"

#include "common.h"
#include "menu.h"
#include "savegame.h"
#include "screen.h"

#define MENU_TITLE "  CTPAHA MOHCTPOB  "
#define NAME_MAX_LEN 32

static void clear_scr_buf(void) {
    for (size_t i = 0; i < SC_HEIGHT; i++) {
        for (size_t j = 0; j < SC_WIDTH; j++) {
            g_scrBuf[i][j].ch = ' ';
            g_scrBuf[i][j].bg_color = COLOR_BLACK;
            g_scrBuf[i][j].ch_color = COLOR_WHITE;
        }
    }
}

static void draw_text_centered(int row, const char *text, short color) {
    int len = (int)strlen(text);
    int col = (SC_WIDTH - len) / 2;
    if (col < 0) col = 0;
    for (int i = 0; text[i] && (col + i) < SC_WIDTH; i++) {
        g_scrBuf[row][col + i].ch = (char)text[i];
        g_scrBuf[row][col + i].ch_color = color;
        g_scrBuf[row][col + i].bg_color = COLOR_BLACK;
    }
}

enum MenuAction menu_run(char *name_buf, size_t name_size,
                         char *selected_path, size_t path_size) {
    int state = 0;  /* 0 = main menu, 1 = enter name, 2 = load list */
    char name[NAME_MAX_LEN];
    int name_len = 0;
    char save_names[SAVE_LIST_MAX][SAVE_NAME_MAX];
    int save_count = 0;
    int load_sel = 0;

    name_buf[0] = '\0';
    if (selected_path && path_size > 0)
        selected_path[0] = '\0';

    for (;;) {
        clear_scr_buf();

        if (state == 0) {
            draw_text_centered(5, MENU_TITLE, COLOR_BRIGHTWHITE);
            draw_text_centered(8, "1. New game (create character)", COLOR_GREEN);
            draw_text_centered(10, "2. Load game", COLOR_GREEN);
            draw_text_centered(12, "3. Quit", COLOR_GREEN);
            draw_text_centered(SC_HEIGHT - 2, "Press 1, 2 or 3", COLOR_YELLOW);
        } else if (state == 1) {
            draw_text_centered(8, "Enter character name:", COLOR_BRIGHTWHITE);
            name[name_len] = '\0';
            char line[NAME_MAX_LEN + 32];
            snprintf(line, sizeof(line), "> %s_", name);
            draw_text_centered(10, line, COLOR_GREEN);
            draw_text_centered(12, "Press Enter when done, Backspace to delete", COLOR_YELLOW);
        } else if (state == 2) {
            draw_text_centered(5, "Select save to load:", COLOR_BRIGHTWHITE);
            if (save_count == 0) {
                draw_text_centered(8, "No saved games found.", COLOR_YELLOW);
                draw_text_centered(10, "Press any key to return", COLOR_YELLOW);
            } else {
                for (int i = 0; i < save_count && i < 15; i++) {
                    char line[80];
                    snprintf(line, sizeof(line), "%d. %s", i + 1, save_names[i]);
                    draw_text_centered(8 + i, line, (i == load_sel) ? COLOR_BRIGHTGREEN : COLOR_GREEN);
                }
                draw_text_centered(SC_HEIGHT - 2, "1-9 to select, Enter to load, Esc to back", COLOR_YELLOW);
            }
        }

        sc_flushBuf();
        int ch = getch();

        if (state == 0) {
            if (ch == '1' || ch == 'n' || ch == 'N') {
                state = 1;
                name_len = 0;
                name[0] = '\0';
            } else if (ch == '2' || ch == 'l' || ch == 'L') {
                state = 2;
                save_count = list_save_dirs(save_names, SAVE_LIST_MAX);
                load_sel = 0;
            } else if (ch == '3' || ch == 'q' || ch == 'Q') {
                return MENU_ACTION_QUIT;
            }
        } else if (state == 1) {
            if (ch == '\r' || ch == '\n' || ch == KEY_ENTER || ch == PADENTER) {
                name[name_len] = '\0';
                if (name_len > 0) {
                    strncpy(name_buf, name, name_size - 1);
                    name_buf[name_size - 1] = '\0';
                    return MENU_ACTION_NEW;
                }
            } else if ((ch == KEY_BACKSPACE || ch == 127) && name_len > 0) {
                name_len--;
            } else if (ch == 27) {
                state = 0;
            } else if (name_len < NAME_MAX_LEN - 1 && isprint((unsigned char)ch)) {
                name[name_len++] = (char)ch;
            }
        } else if (state == 2) {
            if (save_count == 0) {
                if (ch != -1)
                    state = 0;
            } else {
                if (ch == 27) {
                    state = 0;
                } else if (ch == '\r' || ch == '\n' || ch == KEY_ENTER || ch == PADENTER) {
                    if (load_sel >= 0 && load_sel < save_count && selected_path && path_size > 0) {
                        snprintf(selected_path, path_size, "%s%s/", SAVE_BASE, save_names[load_sel]);
                        return MENU_ACTION_LOAD;
                    }
                } else if (ch >= '1' && ch <= '9') {
                    int idx = (ch - '1');
                    if (idx < save_count)
                        load_sel = idx;
                } else if (ch == KEY_UP || ch == 'w' || ch == 'W') {
                    load_sel--;
                    if (load_sel < 0) load_sel = save_count - 1;
                } else if (ch == KEY_DOWN || ch == 's' || ch == 'S') {
                    load_sel++;
                    if (load_sel >= save_count) load_sel = 0;
                }
            }
        }
    }
}
