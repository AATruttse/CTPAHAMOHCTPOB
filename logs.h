#ifndef LOGS_H_INCLUDED
#define LOGS_H_INCLUDED

#include "common.h"

#define EXIT_ON_FILE_IO_ERROR

#define LOG_WARNINGS
#define LOG_MESSAGES

#define MESSAGE_FNAME   "log.log"
#define WARNING_FNAME   "log.log"
#define ERROR_FNAME     "error.log"
#define CRITICAL_FNAME  "error.log"

#define CRITICAL_STRING "!!! CRITICAL !!!"
#define ERROR_STRING    "! ERROR !"
#define WARNING_STRING  "WARNING"

bool log_to_file(const char *_fname, const char *_fmt_msg, ...);

bool logs_init();
void logs_uninit();

bool logCritical(const char *_fmt_msg, ...);
bool logError(const char *_fmt_msg, ...);
bool logWarning(const char *_fmt_msg, ...);
bool logMessage(const char *_fmt_msg, ...);
#endif // LOGS_H_INCLUDED
