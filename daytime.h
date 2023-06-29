#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#define MOON_NUM 3

enum E_MOONS { EMOON_WHITE = 0, EMOON_RED = 1, EMOON_BLACK = 2 };
enum E_MOONPHASES {
    EMP_NEWMOON = 0,
    EMP_WAXCRESCENT = 1,
    EMP_FIRSTQUARTER = 2,
    EMP_WAXGIBBOUS = 3,
    EMP_FULLMOON = 4,
    EMP_WANGIBBOUS = 5,
    EMP_LASTQUARTER = 6,
    EMP_WANCRESCENT = 7
};

size_t g_Day;
size_t g_DayTime;
size_t g_MoonDay[MOON_NUM];
enum E_MOONPHASES g_MoonPhases[MOON_NUM];

void time_init();
void time_draw();
void time_advance(size_t _hours);

#endif // TIME_H_INCLUDED
