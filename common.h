#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define DEBUG

enum E_DIR {
    E_DIR_N     = 0,
    E_DIR_NE    = 1,
    E_DIR_E     = 2,
    E_DIR_SE    = 3,
    E_DIR_S     = 4,
    E_DIR_SW    = 5,
    E_DIR_W     = 6,
    E_DIR_NW    = 7
};

enum E_CellType {
    ECT_PLAIN       = 0,
    ECT_LOWLAND     = 10,
    ECT_FOREST      = 20,
    ECT_DEADFALL    = 30,
    ECT_HILL        = 40,
    ECT_MOUNTAIN    = 50,
    ECT_SHOAL       = 60,
    ECT_LAKE        = 70,
    ECT_RIVER       = 80
};

enum E_CellHumidity {
    ECH_NORMAL  = 0,
    ECH_DRY     = 1,
    ECH_SWAMP   = 2,
    ECH_WATER   = 3,
    ECH_SNOW    = 4
};



#endif // COMMON_H_INCLUDED
