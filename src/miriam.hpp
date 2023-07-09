#pragma once

/* Copyright (c) 2023 sodamouse. See LICENSE.md */

#include <stdio.h>

/* Definitions ---------------------------------------------------------------*/
typedef enum {
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogType;

/* Interface -----------------------------------------------------------------*/
void set_log_level(LogType logLevel);
void set_format(const char* format);
void flog_message(FILE* stream, LogType type, const char* message);
void log_info(const char* message);
void log_warning(const char* message);
void log_error(const char* message);
void log_fatal(const char* message);

/* Implementation ------------------------------------------------------------*/
#ifdef MIRIAM_IMPL

#include <assert.h>
#include <stdlib.h>

#define MAX_LEVELS 4
const char* LOG_LEVELS[MAX_LEVELS] = {
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};

LogType LOG_LEVEL = LOG_LEVEL_INFO;

void set_log_level(LogType logLevel)
{
    LOG_LEVEL = logLevel;
}

const char* FORMAT = "[%s] %s\n";
size_t stringSpecifierCount = 2;

void set_format(const char* format)
{
    stringSpecifierCount = 0;
    FORMAT = format;

    while ((*format) != '\0') {
        if (*format == '%' && *(++format) == 's')
            ++stringSpecifierCount;

        ++format;
    }

    assert(stringSpecifierCount > 0 && 
        "Invalid format string. At least one string format specifier must be provided!");
}

void flog_message(FILE* stream, LogType type, const char* message)
{
    // assert(type < MAX_LEVELS && type >= 0 && "Invalid log level");
    if (type < LOG_LEVEL) return;

    if (stringSpecifierCount < 2) {
        fprintf(stream, FORMAT, message);
        return;
    }

    fprintf(stream, FORMAT, LOG_LEVELS[type], message);
}

void log_info(const char* message)
{
    flog_message(stdout, LOG_LEVEL_INFO, message);
}

void log_warning(const char* message)
{
    flog_message(stdout, LOG_LEVEL_WARN, message);
}

void log_error(const char* message)
{
    flog_message(stderr, LOG_LEVEL_ERROR, message);
}

void log_fatal(const char* message)
{
    flog_message(stderr, LOG_LEVEL_FATAL, message);
    exit(1);
}

#endif
