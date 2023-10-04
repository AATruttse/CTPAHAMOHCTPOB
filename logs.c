#include <stdarg.h>
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

bool log_to_file_ptr_va(FILE* _fptr, const char *_fmt_msg, va_list args) {
    int len = vsnprintf(NULL, 0, _fmt_msg, args);

    if (len < 0) {
        return false;
    } // if (len < 0)

    char fmt_msg[len + 1];
    vsnprintf(fmt_msg, len + 1, _fmt_msg, args);

    fprintf(_fptr, "%s", fmt_msg);

    if (ferror (_fptr)) {
        return false;
    } // if (ferror (fptr))

    return true;
} // log_to_file_ptr_va(FILE* _fptr, const char *_fmt_msg, va_list args)

bool log_to_file_ptr(FILE* _fptr, const char *_fmt_msg, ...) {
    va_list args;
    va_start(args, _fmt_msg);
    bool is_ok = log_to_file_ptr_va(_fptr, _fmt_msg, args);
    va_end(args);
    return is_ok;
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

    va_list args;
    va_start(args, _fmt_msg);
    bool is_ok = log_to_file_ptr_va(fptr, _fmt_msg, args);
    va_end(args);
    fclose(fptr);

    if (!is_ok) {
        eprintf("Can't write to file %s!\n", _fname);
#ifdef EXIT_ON_FILE_IO_ERROR
        exit(1);
#endif // EXIT_ON_FILE_IO_ERROR
    } // if (!is_ok)

    return is_ok;
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
    bool is_ok = true;
    eprintf(_fmt_msg);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    va_list args;
    va_start(args, _fmt_msg);
    is_ok &= log_to_file_ptr(fCriticalPtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, CRITICAL_STRING);
    is_ok &= log_to_file_ptr_va(fCriticalPtr, _fmt_msg, args);
    is_ok &= log_to_file_ptr(fCriticalPtr, "\n");
#ifdef LOG_MESSAGES
    if (strcmp(CRITICAL_FNAME, MESSAGE_FNAME)) {
        is_ok &= log_to_file_ptr(fMessagePtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, CRITICAL_STRING);
        is_ok &= log_to_file_ptr_va(fMessagePtr, _fmt_msg, args);
        is_ok &= log_to_file_ptr(fMessagePtr, "\n");
    }
#endif // #ifdef LOG_MESSAGES
    va_end(args);

    return is_ok;
} // logCritical(const char *_fmt_msg, ...)

bool logError(const char *_fmt_msg, ...) {
    bool is_ok = true;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    va_list args;
    va_start(args, _fmt_msg);
    is_ok &= log_to_file_ptr(fErrorPtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, ERROR_STRING);
    is_ok &= log_to_file_ptr_va(fErrorPtr, _fmt_msg, args);
    is_ok &= log_to_file_ptr(fErrorPtr, "\n");
#ifdef LOG_MESSAGES
    if (strcmp(ERROR_FNAME, MESSAGE_FNAME)) {
        is_ok &= log_to_file_ptr(fMessagePtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, ERROR_STRING);
        is_ok &= log_to_file_ptr_va(fMessagePtr, _fmt_msg, args);
        is_ok &= log_to_file_ptr(fMessagePtr, "\n");
    }
#endif // #ifdef LOG_MESSAGES
    va_end(args);

    return is_ok;
} // logError(const char *_fmt_msg, ...)

bool logWarning(const char *_fmt_msg, ...) {
    bool is_ok = true;
#ifdef LOG_WARNINGS
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    va_list args;
    va_start(args, _fmt_msg);
    is_ok &= log_to_file_ptr(fWarningPtr, "%02d:%02d:%02d %s - ", tm.tm_hour, tm.tm_min, tm.tm_sec, WARNING_STRING);
    is_ok &= log_to_file_ptr_va(fWarningPtr, _fmt_msg, args);
    is_ok &= log_to_file_ptr(fWarningPtr, "\n");
    va_end(args);
#endif // #ifdef LOG_WARNINGS
    return is_ok;
} // logWarning(const char *_fmt_msg, ...)

bool logMessage(const char *_fmt_msg, ...) {
    bool is_ok = true;
#ifdef LOG_MESSAGES
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    va_list args;
    va_start(args, _fmt_msg);
    is_ok &= log_to_file_ptr(fMessagePtr, "%02d:%02d:%02d - ", tm.tm_hour, tm.tm_min, tm.tm_sec);
    is_ok &= log_to_file_ptr_va(fMessagePtr, _fmt_msg, args);
    is_ok &= log_to_file_ptr(fMessagePtr, "\n");
    va_end(args);
#endif // #ifdef LOG_MESSAGES
    return is_ok;
} // logMessage(const char *_fmt_msg, ...)



