#ifndef APPSRV_LOG_H_
#define APPSRV_LOG_H_

#include <mongoose.h>

#define appsrv_vsnprintf mg_avprintf

typedef enum
{
    APPSRV_LOG_DEBUG,
    APPSRV_LOG_INFO,
    APPSRV_LOG_WARN,
    APPSRV_LOG_ERROR
} appsrv_log_level;

void appsrv_log_init();

void appsrv_set_log_level(
    appsrv_log_level level);

void _appsrv_log(
    appsrv_log_level level,
    const char *fun,
    const char *fmt,
    ...);

#define __symx(l) #l
#define __sym(l) __symx(l)
#define __sym__ __FILE__ "(" __sym(__LINE__) ")"

#define appsrv_log(level, fmt, ...) \
    _appsrv_log(level, __sym__, fmt, ##__VA_ARGS__);

#endif
