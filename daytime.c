#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "daytime.h"
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

char* moon_phases_names[MOON_PHASES_NUM] = {
    "new moon",
    "waxing crescent",
    "first quarter",
    "waxing gibbous",
    "full moon",
    "waning gibbous",
    "last quarter",
    "waxing crescent"
};

char* moon_names[MOON_NUM] = { "Heitmena, the White Moon", "Rauthmena, the Blood Moon", "Swartmena, the Black Moon" };
double moon_periods[MOON_NUM] = { 27.9, 44.1, 77.7 };
double moon_phases[MOON_PHASES_NUM] = { 0.05, 0.2, 0.3, 0.45, 0.55, 0.7, 0.8, 0.95 };
size_t moon_phases_limits[MOON_NUM][MOON_PHASES_NUM];

char* weekdays_names[DAYS_IN_WEEK] = { "Auzawandag", "Andanahdag", "Stairnodag", "Swartmendag", "Rauthmendag", "Heitmendag", "Sunnodag" };

char time_buf[TIME_HEIGHT][TIME_WIDTH];

void calc_moon_days() {
    float moonSecondsInPeriod;
    for (size_t i = 0; i < MOON_NUM; i++) {
        moonSecondsInPeriod = g_MoonSeconds[i] % (size_t)((double)SEC_IN_DAY * moon_periods[i]);
        g_MoonDay[i] = 1 + ( moonSecondsInPeriod / SEC_IN_DAY );

        g_MoonPhases[i] = EMP_NEWMOON;
        for (int j = 0; j < MOON_PHASES_NUM; j++) {
            if (moonSecondsInPeriod < moon_phases_limits[i][j]) {
                g_MoonPhases[i] = j;
                break;
            }
        }
    }
}

void time_init(){
    g_Day = 1;
    g_DayTime = rand() % HOURS_IN_DAY;
    g_WeekDay = rand() % DAYS_IN_WEEK;

    // calc initial position of the moons
    for (size_t i = 0; i < MOON_NUM; i++) {
        g_MoonSeconds[i] = SEC_IN_DAY * (rand() % (int)floor(moon_periods[i]));
    }

    calc_moon_days();

    //calc moons' phases limits
    for (size_t i = 0; i < MOON_NUM; i++) {
        for (size_t j = 0; j < MOON_PHASES_NUM; j++) {
            moon_phases_limits[i][j] = (size_t)((double)SEC_IN_DAY * moon_periods[i] * moon_phases[j]);
        }
    }
}

void time_draw() {
   for (size_t i = 0; i < TIME_HEIGHT; i++) {
        for (size_t j = 0; j < TIME_WIDTH; j++) {
            time_buf[i][j] = ' ';
        }
    }
    sprintf(time_buf[0], "      ASTROLOGY:");
    sprintf(time_buf[1], " Day %d, %s    Time: %02d:00", (int)g_Day, weekdays_names[g_WeekDay], (int)g_DayTime);
    for (size_t i = 0; i < MOON_NUM; i++) {
        sprintf(time_buf[2 + i*2], " %s:", moon_names[i]);
        sprintf(time_buf[3 + i*2], "    %s, %d day", moon_phases_names[g_MoonPhases[i]], (int)g_MoonDay[i]);
    }

    for (size_t i = 0; i < TIME_HEIGHT; i++) {
        for (size_t j = 0; j < TIME_WIDTH; j++) {
            if (time_buf[i][j]) {
                g_scrBuf[DAYTIME_Y0 + i][DAYTIME_X0 + j].ch = time_buf[i][j];
            }
        }
    }
}

void time_advance(size_t _hours) {
    g_DayTime += _hours;
    if ( g_DayTime >= HOURS_IN_DAY ) {
        g_DayTime %= HOURS_IN_DAY;
        g_Day++;
        g_WeekDay = ((int)g_WeekDay + 1) % DAYS_IN_WEEK;
    }

    for (size_t i = 0; i < MOON_NUM; i++) {
        g_MoonSeconds[i] += SECS_IN_HOUR * _hours;
    }

    calc_moon_days();
}

bool time_save(FILE *fptr) {
    fprintf(fptr, "%lu %lu %lu\n", (unsigned long)g_Day, (unsigned long)g_DayTime, (unsigned long)g_WeekDay);
    if (ferror (fptr)) {
        return false;
    }
    for (size_t i = 0; i < MOON_NUM; i++) {
        fprintf(fptr, "%lu %lu %lu\n", (unsigned long)g_MoonDay[i], (unsigned long)g_MoonPhases[i], (unsigned long)g_MoonSeconds[i]);
        if (ferror (fptr)) {
            return false;
        }
    }

    return true;
}

bool time_load(FILE *fptr) {
    unsigned long temp_day, temp_daytime, temp_weekday, temp_moonday, temp_moonphase, temp_moonseconds;
    fscanf(fptr, "%lu %lu %lu", &temp_day, &temp_daytime,&temp_weekday);
    if (ferror (fptr)) {
        return false;
    }
    g_Day = temp_day;
    g_DayTime = temp_daytime;
    g_WeekDay = temp_weekday;

    for (size_t i = 0; i < MOON_NUM; i++) {
        fscanf(fptr, "%lu %lu %lu", &temp_moonday, &temp_moonphase, &temp_moonseconds);
        if (ferror (fptr)) {
            return false;
        }

        g_MoonDay[i] = temp_moonday;
        g_MoonPhases[i] = temp_moonphase;
        g_MoonSeconds[i] = temp_moonseconds;
    }

    return true;

}
