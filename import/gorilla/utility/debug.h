#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <syslog.h>
#ifdef __cplusplus
}
#endif

#include "../log/logger.h"
#include "../log/logger_config.h"


using namespace gorilla::log;

#if defined(DEBUG)

#define ANSI_CLEAR      "\x1B[0;00m"
#define ANSI_RED        "\x1B[1;31m"
#define ANSI_GREEN      "\x1B[1;32m"
#define ANSI_YELLOW     "\x1B[1;33m"
#define ANSI_BLUE       "\x1B[1;34m"
#define ANSI_MAGENTA    "\x1B[1;35m"
#define ANSI_CYAN       "\x1B[1;36m"

void LoggerWrapper(gorilla::log::SL level, const char* color,
    const char *fmt, ...);

#define DBG_TRACE(str, ...) LoggerWrapper(gorilla::log::trace, ANSI_BLUE, \
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DBG_DEBUG(str, ...) LoggerWrapper(gorilla::log::debug, ANSI_GREEN, \
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DBG_INFO(str, ...) LoggerWrapper(gorilla::log::info, ANSI_CYAN, \
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DBG_WARN(str, ...) LoggerWrapper(gorilla::log::warn, ANSI_YELLOW,\
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DBG_ERR(str, ...) LoggerWrapper(gorilla::log::error, ANSI_RED, \
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#else // NON-DEBUG

void LoggerWrapper(gorilla::log::SL level, const char *fmt, ...);

#define DBG_TRACE(str, ...) LoggerWrapper(gorilla::log::trace, \
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DBG_DEBUG(str, ...) LoggerWrapper(gorilla::log::debug, \
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DBG_INFO(str, ...) LoggerWrapper(gorilla::log::info, \
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DBG_WARN(str, ...) LoggerWrapper(gorilla::log::warn, \
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define DBG_ERR(str, ...) LoggerWrapper(gorilla::log::error, \
    "%s:%d:%s(): " str, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#endif // DEBUG

#define DBG_SYS(str, ...) syslog(LOG_DEBUG, \
    "%s:%d:%s(): " str "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

