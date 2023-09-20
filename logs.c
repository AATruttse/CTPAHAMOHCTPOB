#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "debug.h"
#include "logs.h"

#ifdef LOG_MESSAGES
FILE* fMessagePtr;
#endif // LOG_MESSAGES
#ifdef LOG_WARNINGS
FILE* fWarningPtr;
#endif // LOG_WARNINGS
FILE* fErrorPtr;
FILE* fCriticalPtr;

bool log_to_file_ptr(FILE* _fptr, const char *_fmt_msg, ...) {
    fprintf(_fptr, _fmt_msg);
    if (ferror (_fptr)) {
        return false;
    } // if (ferror (fptr))

    return true;
} // log_to_file(FILE* _fptr, const char *_fmt_msg, ...)

FILE* open_log_file(const char *_fname) {
    FILE* fptr = fopen(_fname, "a");
    if(fptr == NULL) {
        eprintf("Can't open file %s for writing!\n", _fname);
#ifdef EXIT_ON_FILE_IO_ERROR
        exit(1);
#endif // EXIT_ON_FILE_IO_ERROR
    } // if(fptr == NULL)

    return fptr;
} // open_log_file(const char *_fname)

bool log_to_file(const char *_fname, const char *_fmt_msg, ...) {
    FILE* fptr = open_log_file(_fname);
    if (!fptr) { return false; }

    if (!log_to_file_ptr(fptr, _fmt_msg)) {
        eprintf("Can't write to file %s!\n", _fname);
        fclose(fptr);
#ifdef EXIT_ON_FILE_IO_ERROR
        exit(1);
#endif // EXIT_ON_FILE_IO_ERROR
        return false;
    } // if (!log_to_file(_fname, _fmt_msg))

    fclose(fptr);
    return true;
} // bool log_to_file(const char *_fname, const char *_fmt_msg, ...)

void make_log_name(char* _buf, const char *_fname) {
     time_t t = time(NULL);
     struct tm tm = *localtime(&t);
     sprintf(_buf, "%s%d%02d%02d_%s", LOG_PATH, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, _fname);
} // make_log_name(char* _buf, const char *_fname)

bool logs_init() {
#ifdef LOG_MESSAGES
    char messageFileName[256];
    make_log_name(messageFileName, MESSAGE_FNAME);
    fMessagePtr = open_log_file(messageFileName);
#endif // LOG_MESSAGES
#ifdef LOG_WARNINGS
    char warnFileName[256];
    make_log_name(warnFileName, WARNING_FNAME);
    #ifdef LOG_MESSAGES
    fWarningPtr = strcmp(MESSAGE_FNAME, WARNING_FNAME) ? open_log_file(warnFileName) : fMessagePtr;
    #else
    fWarningPtr = open_log_file(warnFileName);
    #endif // LOG_MESSAGES
#endif // LOG_WARNINGS
    char errorFileName[256];
    make_log_name(errorFileName, ERROR_FNAME);
    fErrorPtr =
#ifdef LOG_MESSAGES
        strcmp(MESSAGE_FNAME, ERROR_FNAME) ?
#endif // LOG_MESSAGES
#ifdef LOG_WARNINGS
            strcmp(WARNING_FNAME, ERROR_FNAME) ?
#endif // LOG_WARNINGS
                open_log_file(errorFileName)
#ifdef LOG_WARNINGS
                : fWarningPtr
#endif // LOG_WARNINGS
#ifdef LOG_MESSAGES
            : fMessagePtr
#endif // LOG_MESSAGES
    ;
    char critFileName[256];
    make_log_name(critFileName, CRITICAL_FNAME);
    fCriticalPtr = strcmp(CRITICAL_FNAME, ERROR_FNAME) ?
#ifdef LOG_MESSAGES
                strcmp(CRITICAL_FNAME, MESSAGE_FNAME) ?
#endif // LOG_MESSAGES
#ifdef LOG_WARNINGS
                    strcmp(CRITICAL_FNAME, WARNING_FNAME) ?
#endif // LOG_WARNINGS
                        open_log_file(errorFileName)
#ifdef LOG_WARNINGS
                        : fWarningPtr
#endif // LOG_WARNINGS
#ifdef LOG_MESSAGES
                : fMessagePtr
#endif // LOG_MESSAGES
        : fErrorPtr;

#ifdef LOG_MESSAGES
    if (!fMessagePtr) { return false; }
#endif // LOG_MESSAGES
#ifdef LOG_WARNINGS
    if (!fWarningPtr) { return false; }
#endif // LOG_WARNINGS

    return fErrorPtr && fCriticalPtr;
} // logs_init()

void logs_uninit() {
    fclose(fCriticalPtr);

    if (!strcmp(CRITICAL_FNAME, ERROR_FNAME)) {
        fclose(fErrorPtr);
    } // if (!strcmp(critFileName, errorFileName))

#ifdef LOG_WARNINGS
    if (!strcmp(CRITICAL_FNAME, WARNING_FNAME) && !strcmp(ERROR_FNAME, WARNING_FNAME)) {
        fclose(fWarningPtr);
    } // if (!strcmp(CRITICAL_FNAME, WARNING_FNAME) && !strcmp(ERROR_FNAME, WARNING_FNAME))
#endif // LOG_WARNINGS
#ifdef LOG_MESSAGES
    if (!strcmp(CRITICAL_FNAME, MESSAGE_FNAME) && !strcmp(ERROR_FNAME, MESSAGE_FNAME)
    #ifdef LOG_WARNINGS
        && !strcmp(WARNING_FNAME, MESSAGE_FNAME)
    #endif // LOG_WARNINGS
        ) {
        fclose(fMessagePtr);
    } // (!strcmp(CRITICAL_FNAME, MESSAGE_FNAME) && !strcmp(ERROR_FNAME, MESSAGE_FNAME) && !strcmp(WARNING_FNAME, MESSAGE_FNAME))
#endif // LOG_MESSAGES
} // logs_uninit()

bool logCritical(const char *_fmt_msg, ...) {
    eprintf(_fmt_msg);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return log_to_file_ptr(fCriticalPtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, CRITICAL_STRING) && log_to_file_ptr(fCriticalPtr, _fmt_msg) && log_to_file_ptr(fCriticalPtr, "\n")
#ifdef LOG_MESSAGES
        && (strcmp(CRITICAL_FNAME, MESSAGE_FNAME) ?
             log_to_file_ptr(fMessagePtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, CRITICAL_STRING) && log_to_file_ptr(fMessagePtr, _fmt_msg) && log_to_file_ptr(fMessagePtr, "\n") :
             true)
#endif // LOG_MESSAGES
    ;
} // logCritical(const char *_fmt_msg, ...)

bool logError(const char *_fmt_msg, ...) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return log_to_file_ptr(fErrorPtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, ERROR_STRING) && log_to_file_ptr(fErrorPtr, _fmt_msg) && log_to_file_ptr(fErrorPtr, "\n")
#ifdef LOG_MESSAGES
        && (strcmp(CRITICAL_FNAME, MESSAGE_FNAME) ?
             log_to_file_ptr(fMessagePtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, ERROR_STRING) && log_to_file_ptr(fMessagePtr, _fmt_msg) && log_to_file_ptr(fMessagePtr, "\n") :
             true)
#endif // LOG_MESSAGES
    ;
} // logError(const char *_fmt_msg, ...)

bool logWarning(const char *_fmt_msg, ...) {
#ifdef LOG_WARNINGS
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return log_to_file_ptr(fWarningPtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, WARNING_STRING) && log_to_file_ptr(fWarningPtr, _fmt_msg) && log_to_file_ptr(fWarningPtr, "\n");
#endif // LOG_WARNINGS
} // logWarning(const char *_fmt_msg, ...)

bool logMessage(const char *_fmt_msg, ...) {
#ifdef LOG_MESSAGES
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    return log_to_file_ptr(fMessagePtr, "%02d:%02d:%02d - ", tm.tm_hour, tm.tm_min, tm.tm_sec) && log_to_file_ptr(fMessagePtr, _fmt_msg) && log_to_file_ptr(fMessagePtr, "\n");
#endif // LOG_MESSAGES
} // logMessage(const char *_fmt_msg, ...)



