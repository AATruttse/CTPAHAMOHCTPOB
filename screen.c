#include <stdlib.h>

#include "include/curses.h"

#include "screen.h"

short curs_color(int fg)
{
    switch (7 & fg) {           /* RGB */
    case 0:                     /* 000 */
        return COLOR_BLACK;
    case 1:                     /* 001 */
        return COLOR_BLUE;
    case 2:                     /* 010 */
        return COLOR_GREEN;
    case 3:                     /* 011 */
        return COLOR_CYAN;
    case 4:                     /* 100 */
        return COLOR_RED;
    case 5:                     /* 101 */
        return COLOR_MAGENTA;
    case 6:                     /* 110 */
        return COLOR_YELLOW;
    case 7:                     /* 111 */
        return COLOR_WHITE;
    }

    return COLOR_BLACK;
}

int colornum(int fg, int bg)
{
    int B, bbb, ffff;

    B = 1 << 7;
    bbb = (7 & bg) << 4;
    ffff = 7 & fg;

    return (B | bbb | ffff);
}

void init_colorpairs(void)
{
    int colorpair;
    for (int bg = 0; bg <= 7; bg++) {
        for (int fg = 0; fg <= 7; fg++) {
            colorpair = colornum(fg, bg);
            init_pair(colorpair, curs_color(fg), curs_color(bg));
        }
    }
}

int is_bold(int fg)
{
    /* return the intensity bit */
    int i;

    i = 1 << 3;
    return (i & fg);
}

void setcolor(int fg, int bg)
{
    /* set the color pair (colornum) and bold/bright (A_BOLD) */

    attron(COLOR_PAIR(colornum(fg, bg)));
    if (is_bold(fg)) {
        attron(A_BOLD);
    }
}

void unsetcolor(int fg, int bg)
{
    /* unset the color pair (colornum) and bold/bright (A_BOLD) */

    attroff(COLOR_PAIR(colornum(fg, bg)));
    if (is_bold(fg)) {
        attroff(A_BOLD);
    }
}

void sc_flushBuf() {

    clear();

    for (int i = 0; i < SC_HEIGHT; i++) {
        for (int j = 0; j < SC_WIDTH; j++) {
            setcolor(g_scrBuf[i][j].ch_color, g_scrBuf[i][j].bg_color);
            mvaddch(i , j, g_scrBuf[i][j].ch);
            unsetcolor(g_scrBuf[i][j].ch_color, g_scrBuf[i][j].bg_color);
        }
    }

	refresh();
}

void sc_init() {
    if (has_colors() == FALSE) {
        endwin();
        puts("Your terminal does not support color");
        exit(1);
    }

    start_color();

    init_colorpairs();

    for (int i = 0; i < SC_HEIGHT; i++) {
        for (int j = 0; j < SC_WIDTH; j++) {
            g_scrBuf[i][j].ch = ' ';
            g_scrBuf[i][j].bg_color = COLOR_BLACK;
            g_scrBuf[i][j].ch_color = COLOR_WHITE;
        }
    }
}

void sc_uninit() {

}

