#include <cstdio>
#include <cstdarg>
#include <vector>

#include "ini_file.h"
#include "string_format.h"
#include "debug.h"

#if defined(DEBUG)
void LoggerWrapper(gorilla::log::SL level, const char* color,
    const char *fmt, ...)
#else // NON-DEBUG
void LoggerWrapper(gorilla::log::SL level, const char *fmt, ...)
#endif // DEBUG
{
    std::vector<char> str(200);
    va_list ap;
    while(1) {
        va_start(ap, fmt);
        int n = vsnprintf(str.data(), str.size(), fmt, ap);
        va_end(ap);
        if (n > -1 && n < str.size()) {
            str.resize(n);
#if defined(DEBUG)
            LOGGER_S(level) << color << str.data() << ANSI_CLEAR;
#else // NON-DEBUG
            LOGGER_S(level) << str.data();
#endif // DEBUG
            return;
        }
        str.resize(str.size() * 2);
    }
}

