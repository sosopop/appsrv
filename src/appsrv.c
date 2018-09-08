#include "appsrv.h"
#include "appsrv_internal.h"
#include "appsrv_utils.h"
#include "appsrv_thread.h"
#include "appsrv_log.h"

//日志互斥变量
extern appsrv_mutex_t log_mutex;
//是否已经进行过初始化
int inited = 0;

void appsrv_global_init()
{
    if (inited)
        return;
    appsrv_mutex_init(&log_mutex);
}

void appsrv_global_uninit()
{
    if (!inited)
        return;
    appsrv_mutex_destroy(&log_mutex);
}

appsrv_handle appsrv_create()
{
    appsrv_global_init();

    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_create");
    appsrv_t *appsrv = 0;
    appsrv = (appsrv_t *)malloc(sizeof(appsrv_t));
    memset(appsrv, 0, sizeof(appsrv_t));
    return appsrv;
}

int appsrv_set_option(
    appsrv_handle appsrv,
    appsrv_option opt,
    ...)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_set_option");
    return APPSRV_E_OK;
}

int appsrv_get_info(
    appsrv_handle appsrv,
    appsrv_info info,
    ...)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_get_info");
    return APPSRV_E_OK;
}

int appsrv_start(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_start");
    return APPSRV_E_OK;
}

int appsrv_stop(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_stop");
    return APPSRV_E_OK;
}

void appsrv_wait_and_close(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_wait_and_close");
}

void appsrv_free(
    void *data)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_free");
}

#define APPSRV_ERROR_GEN(n, s) {"SP_E_" #n, s},
static struct
{
    const char *name;
    const char *description;
} appsrv_strerror_tab[] = {APPSRV_ERROR_MAP(APPSRV_ERROR_GEN)};
#undef APPSRV_ERROR_GEN

const char *appsrv_errno_description(
    appsrv_errno error)
{
    return appsrv_strerror_tab[error].description;
}

#define APPSRV_VERSION "0.0.0.1"

const char *appsrv_version()
{
    return APPSRV_VERSION;
}