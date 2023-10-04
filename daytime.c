#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "daytime.h"
#include "logs.h"
#include "screen.h"

#define DAYTIME_X0  80
#define DAYTIME_Y0  0

#define TIME_HEIGHT 10
#define TIME_WIDTH  40

#define DAYS_IN_WEEK    7
#define SECS_IN_HOUR    3600
#define HOURS_IN_DAY    24
#define SEC_IN_DAY      (SECS_IN_HOUR * HOURS_IN_DAY)

#define MOON_PHASES_NUM 8

size_t g_MoonSeconds[MOON_NUM];

const char* MOON_PHASES_NAMES[MOON_PHASES_NUM] = {
    "new moon",
    "waxing crescent",
    "first quarter",
    "waxing gibbous",
    "full moon",
    "waning gibbous",
    "last quarter",
    "waxing crescent"
}; // char* moon_phases_names[MOON_PHASES_NUM]

const char* MOON_NAMES[MOON_NUM] = { "Heitmena, the White Moon", "Rauthmena, the Blood Moon", "Swartmena, the Black Moon" };
double MOON_PERIODS[MOON_NUM] = { 27.9, 44.1, 77.7 };
double MOON_PHASES[MOON_PHASES_NUM] = { 0.05, 0.2, 0.3, 0.45, 0.55, 0.7, 0.8, 0.95 };
size_t moon_phases_limits[MOON_NUM][MOON_PHASES_NUM];

const char* WEEKDAYS_NAMES[DAYS_IN_WEEK] = { "Auzawandag", "Andanahdag", "Stairnodag", "Swartmendag", "Rauthmendag", "Heitmendag", "Sunnodag" };

char time_buf[TIME_HEIGHT][TIME_WIDTH];

void calc_moon_days() {
    float moonSecondsInPeriod;
    for (size_t i = 0; i < MOON_NUM; i++) {
        moonSecondsInPeriod = g_MoonSeconds[i] % (size_t)((double)SEC_IN_DAY * MOON_PERIODS[i]);
        g_MoonDay[i] = 1 + ( moonSecondsInPeriod / SEC_IN_DAY );

        g_MoonPhases[i] = EMP_NEWMOON;
        for (int j = 0; j < MOON_PHASES_NUM; j++) {
            if (moonSecondsInPeriod < moon_phases_limits[i][j]) {
                g_MoonPhases[i] = j;
                break;
            } // if (moonSecondsInPeriod < moon_phases_limits[i][j])
        } // for (int j = 0; j < MOON_PHASES_NUM; j++)
    } // for (size_t i = 0; i < MOON_NUM; i++)
} // void calc_moon_days()

void time_init() {
    logMessage("Time init started! #init #daytime");

    g_Day = 1;
    g_DayTime = rand() % HOURS_IN_DAY;
    g_WeekDay = rand() % DAYS_IN_WEEK;

    // calc initial position of the moons
    for (size_t i = 0; i < MOON_NUM; i++) {
        g_MoonSeconds[i] = SEC_IN_DAY * (rand() % (int)floor(MOON_PERIODS[i]));
    } // for (size_t i = 0; i < MOON_NUM; i++)

    calc_moon_days();

    //calc moons' phases limits
    for (size_t i = 0; i < MOON_NUM; i++) {
        for (size_t j = 0; j < MOON_PHASES_NUM; j++) {
            moon_phases_limits[i][j] = (size_t)((double)SEC_IN_DAY * MOON_PERIODS[i] * MOON_PHASES[j]);
        } // for (size_t j = 0; j < MOON_PHASES_NUM; j++)
    } // for (size_t i = 0; i < MOON_NUM; i++)
    logMessage("Time init ok! #init #daytime");
} // void time_init()

void time_draw() {
   for (size_t i = 0; i < TIME_HEIGHT; i++) {
        for (size_t j = 0; j < TIME_WIDTH; j++) {
            time_buf[i][j] = ' ';
        } // for (size_t j = 0; j < TIME_WIDTH; j++)
    } // for (size_t i = 0; i < TIME_HEIGHT; i++)

    sprintf(time_buf[0], "      ASTROLOGY:");
    sprintf(time_buf[1], " Day %d, %s    Time: %02d:00", (int)g_Day, WEEKDAYS_NAMES[g_WeekDay], (int)g_DayTime);
    for (size_t i = 0; i < MOON_NUM; i++) {
        sprintf(time_buf[2 + i*2], " %s:", MOON_NAMES[i]);
        sprintf(time_buf[3 + i*2], "    %s, %d day", MOON_PHASES_NAMES[g_MoonPhases[i]], (int)g_MoonDay[i]);
    } // for (size_t i = 0; i < MOON_NUM; i++)

    for (size_t i = 0; i < TIME_HEIGHT; i++) {
        for (size_t j = 0; j < TIME_WIDTH; j++) {
            if (time_buf[i][j]) {
                g_scrBuf[DAYTIME_Y0 + i][DAYTIME_X0 + j].ch = time_buf[i][j];
            } // if (time_buf[i][j])
        } // for (size_t j = 0; j < TIME_WIDTH; j++)
    } // for (size_t i = 0; i < TIME_HEIGHT; i++)
} // void time_draw()

void time_advance(size_t _hours) {
    logMessage("Time advance %d hours. #daytime", _hours);

    g_DayTime += _hours;

    if ( g_DayTime >= HOURS_IN_DAY ) {
        g_DayTime %= HOURS_IN_DAY;
        g_Day++;
        g_WeekDay = ((int)g_WeekDay + 1) % DAYS_IN_WEEK;
        logMessage("New day %d - %s. #daytime", g_Day, WEEKDAYS_NAMES[g_WeekDay]);
    } // if ( g_DayTime >= HOURS_IN_DAY )
    logMessage("New time %d. #daytime", g_DayTime);

    for (size_t i = 0; i < MOON_NUM; i++) {
        g_MoonSeconds[i] += SECS_IN_HOUR * _hours;
    } // for (size_t i = 0; i < MOON_NUM; i++)

    calc_moon_days();
} // void time_advance(size_t _hours)

bool time_save(FILE *fptr) {
    logMessage("Time save started! #save #daytime");
    fprintf(fptr, "%lu %lu %lu\n", (unsigned long)g_Day, (unsigned long)g_DayTime, (unsigned long)g_WeekDay);
    if (ferror (fptr)) {
        logError("Can't save time! #save #daytime #error");
        return false;
    } // if (ferror (fptr))
    for (size_t i = 0; i < MOON_NUM; i++) {
        fprintf(fptr, "%lu %lu %lu\n", (unsigned long)g_MoonDay[i], (unsigned long)g_MoonPhases[i], (unsigned long)g_MoonSeconds[i]);
        if (ferror (fptr)) {
            logError("Can't save moons' phases! #save #daytime #error");
            return false;
        } // if (ferror (fptr))
    } // for (size_t i = 0; i < MOON_NUM; i++)

    logMessage("Time save ok! #save #daytime");
    return true;
} // bool time_save(FILE *fptr)

bool time_load(FILE *fptr) {
    logMessage("Time load started! #load #daytime");
    unsigned long temp_day, temp_daytime, temp_weekday, temp_moonday, temp_moonphase, temp_moonseconds;
    fscanf(fptr, "%lu %lu %lu", &temp_day, &temp_daytime,&temp_weekday);
    if (ferror (fptr)) {
        logError("Can't load daytime! #load #daytime #error");
        return false;
    } // if (ferror (fptr))
    g_Day = temp_day;
    g_DayTime = temp_daytime;
    g_WeekDay = temp_weekday;

    for (size_t i = 0; i < MOON_NUM; i++) {
        fscanf(fptr, "%lu %lu %lu", &temp_moonday, &temp_moonphase, &temp_moonseconds);
        if (ferror (fptr)) {
            logError("Can't load moons' phases! #load #daytime #error");
            return false;
        }

        g_MoonDay[i] = temp_moonday;
        g_MoonPhases[i] = temp_moonphase;
        g_MoonSeconds[i] = temp_moonseconds;
    } // for (size_t i = 0; i < MOON_NUM; i++)

    logMessage("Time load ok! #load #daytime");
    return true;
} // bool time_load(FILE *fptr)
