#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <stdbool.h>

#include "common.h"

#ifdef DEBUG

bool g_Debug;

void debug_draw();

#endif // DEBUG

#endif // DEBUG_H_INCLUDED
