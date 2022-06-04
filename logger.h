#ifndef H_LOGGER
#define H_LOGGER

#include <stdio.h>
#include <time.h>

#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_TRACE 5

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#define __LOG(format, level, ...)                                        \
    {                                                                    \
        time_t curtime = time(NULL);                                     \
        struct tm* ltm = localtime(&curtime);                            \
        printf("%d-%02d-%02d %02d:%02d:%02d - " level " - " format "\n", \
            ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday,          \
            ltm->tm_hour, ltm->tm_min, ltm->tm_sec, ##__VA_ARGS__);      \
    }

#if LOG_LEVEL_FATAL <= LOG_LEVEL
#define LOG_FATAL(format, ...) __LOG(format, "FATAL", ##__VA_ARGS__)
#else
#define LOG_FATAL(format, ...) /* SKIP */
#endif

#if LOG_LEVEL_ERROR <= LOG_LEVEL
#define LOG_ERROR(format, ...) __LOG(format, "ERROR", ##__VA_ARGS__)
#else
#define LOG_ERROR(format, ...) /* SKIP */
#endif

#if LOG_LEVEL_WARN <= LOG_LEVEL
#define LOG_WARN(format, ...) __LOG(format, "WARN", ##__VA_ARGS__)
#else
#define LOG_WARN(format, ...) /* SKIP */
#endif

#if LOG_LEVEL_INFO <= LOG_LEVEL
#define LOG_INFO(format, ...) __LOG(format, "INFO", ##__VA_ARGS__)
#else
#define LOG_INFO(format, ...) /* SKIP */
#endif

#if LOG_LEVEL_DEBUG <= LOG_LEVEL
#define LOG_DEBUG(format, ...) __LOG(format, "DEBUG", ##__VA_ARGS__)
#else
#define LOG_DEBUG(format, ...) /* SKIP */
#endif

#if LOG_LEVEL_TRACE <= LOG_LEVEL
#define LOG_TRACE(format, ...) __LOG(format, "TRACE", ##__VA_ARGS__)
#else
#define LOG_TRACE(format, ...) /* SKIP */
#endif

#endif