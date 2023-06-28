#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#define SC_WIDTH   120
#define SC_HEIGHT  30

#define COLOR_GRAY          8
#define COLOR_BRIGHTBLUE    9
#define COLOR_BRIGHTGREEN   10
#define COLOR_BRIGHTCYAN    11
#define COLOR_BRIGHTRED     12
#define COLOR_BRIGHTMAGENTA 13
#define COLOR_BRIGHTYELLOW  14
#define COLOR_BRIGHTWHITE   15

struct ScChr {
    short   bg_color;
    short   ch_color;
    char    ch;
};

struct ScChr g_scrBuf[SC_HEIGHT][SC_WIDTH];

void sc_flushBuf();
void sc_init();
void sc_uninit();
#endif // SCREEN_H_INCLUDED
