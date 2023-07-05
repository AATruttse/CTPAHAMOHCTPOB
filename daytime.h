#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include <stdbool.h>

#define MOON_NUM 3

enum E_MOONS { EMOON_WHITE = 0, EMOON_RED = 1, EMOON_BLACK = 2 };
enum E_MOONPHASES {
    EMP_NEWMOON         = 0,
    EMP_WAXCRESCENT     = 1,
    EMP_FIRSTQUARTER    = 2,
    EMP_WAXGIBBOUS      = 3,
    EMP_FULLMOON        = 4,
    EMP_WANGIBBOUS      = 5,
    EMP_LASTQUARTER     = 6,
    EMP_WANCRESCENT     = 7
};

enum E_WEEKDAYS {
    EMP_AUZAWANDAG      = 0,
    EMP_ANDANAHDAG      = 1,
    EMP_STAIRNODAG      = 2,
    EMP_SWARTMENDAG     = 3,
    EMP_RAUTHMENDAG     = 4,
    EMP_HEITMENDAG      = 5,
    EWD_SUNNODAG        = 6,
};

enum E_Signs {
    ES_Lover            = 0,
    ES_Twins            = 1,
    ES_Banner           = 2,
    ES_Bow              = 3,
    ES_Chariot          = 4,
    ES_Horse            = 5,
    ES_Raven            = 6,
    ES_Wolf             = 7,
    ES_Wyrm             = 8,
    ES_Barrow           = 9,
    ES_Hermit           = 10,
    ES_Tower            = 11,
    ES_Guest            = 12
};

size_t g_Day;
enum E_WEEKDAYS g_WeekDay;
size_t g_DayTime;
size_t g_MoonDay[MOON_NUM];
enum E_MOONPHASES g_MoonPhases[MOON_NUM];

void time_init();
void time_draw();
void time_advance(size_t _hours);

bool time_save(FILE *fptr);
bool time_load(FILE *fptr);
#endif // TIME_H_INCLUDED
