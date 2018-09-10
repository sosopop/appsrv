#include "appsrv_log.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
#include "appsrv_thread.h"

static appsrv_log_level __appsrv_log_level = APPSRV_LOG_INFO;

void appsrv_set_log_level(
    appsrv_log_level level)
{
    __appsrv_log_level = level;
}

void _appsrv_log(
    appsrv_log_level level,
    const char *fun,
    const char *fmt,
    ...)
{
    if (level < __appsrv_log_level)
    {
        return;
    }

    va_list ap;
    va_start(ap, fmt);

    struct tm *time_info;
    char time_str[30];
    struct timeb tmb;
    ftime(&tmb);
    time_info = localtime(&tmb.time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %T", time_info);

    char stack_buf[128] = {0};
    char *temp_buf = stack_buf;
    const char *match = NULL;
    const char *fun_name = fun;
    for (match = fun; *match != 0; match += 1)
    {
        if ((*match == '\\') || (*match == '/'))
            fun_name = match + 1;
    }

    int len = appsrv_vsnprintf(&temp_buf, sizeof(stack_buf), fmt, ap) + 1;

    const char *debug_level = "DEBUG";
    switch (level)
    {
    case APPSRV_LOG_DEBUG:
        debug_level = "DEBUG";
        break;
    case APPSRV_LOG_INFO:
        debug_level = "INFO";
        break;
    case APPSRV_LOG_WARN:
        debug_level = "WARN";
        break;
    case APPSRV_LOG_ERROR:
        debug_level = "ERROR";
        break;
    }
    printf("%s.%03d %s [%s] %s\n", time_str, tmb.millitm, debug_level, fun_name, temp_buf);

    if (temp_buf != stack_buf)
        free(temp_buf);

    va_end(ap);
}
