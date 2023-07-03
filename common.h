#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define DEBUG

enum E_DIR {
    EDIR_N     = 0,
    EDIR_NE    = 1,
    EDIR_E     = 2,
    EDIR_SE    = 3,
    EDIR_S     = 4,
    EDIR_SW    = 5,
    EDIR_W     = 6,
    EDIR_NW    = 7
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

enum E_MapLayers {
    EML_Mundane = 0,
    EML_Astral = 1,
    EML_Ethereal = 2,
    EML_Shadow = 3
};

enum E_Auras {
    EA_Mundane = 0,
    EA_Arcane = 1,
    EA_Divine = 2,
    EA_Nature = 3,
    EA_Occult = 4
};



#endif // COMMON_H_INCLUDED
