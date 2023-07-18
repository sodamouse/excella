#pragma once

namespace Miriam {

/*
   Example usage:
   log_info("Player velocity: x: %d, y: %d\n", 21.0, 32.0);
   Fatal log level will exit(1)
*/

enum LogLevel {
    INFO,
    WARN,
    ERROR,
    FATAL
};

void flog_message(LogLevel level, const char* fmt, ...);
#define log_info(fmt, ...) flog_message(Miriam::INFO, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) flog_message(Miriam::WARN, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) flog_message(Miriam::ERROR, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) flog_message(Miriam::FATAL, fmt, ##__VA_ARGS__)

} // namespace Miriam

#ifdef MIRIAM_IMPL

#include <iostream>
#include <cstdarg>

namespace Miriam {

const char* levelStrings[] = {
    "[INFO] ",
    "[WARN] ",
    "[ERROR] ",
    "[FATAL] ",
};

void flog_message(LogLevel level, const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);

    std::cout << levelStrings[level];

    for (auto* p = fmt; *p != '\0'; ++p)
    {
        switch (*p)
        {
            case '%':
                switch (*++p) {
                    case 'i': std::cout << va_arg(args, int); continue;
                    case 'f': 
                    case 'd': std::cout << va_arg(args, double); continue;
                    case 's': std::cout << va_arg(args, const char*); continue;
                    case '%': std::cout << '%'; continue;
                    default: continue;
                }
            case '\n':
                std::cout << '\n'; continue;
            case '\t':
                std::cout << '\t'; continue;
        }
        std::cout << *p;
    }

    std::cout << '\n';

    va_end(args);

    if (level == FATAL)
        exit(1);
}

} // namespace Miriam
#endif
