#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#define eprintf(args...) fprintf(stderr, ##args)

#ifdef DEBUG
#include <stdbool.h>
#include "common.h"

#define dprintf(args...) printf(##args)

#define DEBUG_STR_BUF_SIZE 1024

bool g_Debug;

void debug_init();
void debug_uninit();
void debug_draw();
void debug_log(char *_debug_string);

#define logprintf(args...) {              \
    char* __debug_str_buf__ = (char*)malloc(DEBUG_STR_BUF_SIZE * sizeof(char));\
    sprintf(__debug_str_buf__, ##args); \
    debug_log(__debug_str_buf__);       \
    free(__debug_str_buf__);            \
    }

#endif // DEBUG

#endif // DEBUG_H_INCLUDED
