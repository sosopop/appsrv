#include "appsrv.h"
#include "appsrv_internal.h"
#include "appsrv_utils.h"

appsrv_handle appsrv_create()
{
    appsrv_t *appsrv = 0;
    appsrv = (appsrv_t *)malloc(sizeof(appsrv_t));
    memset(appsrv, 0, sizeof(appsrv_t));
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv created");
    return appsrv;
}

int appsrv_set_option(
    appsrv_handle appsrv,
    appsrv_option opt,
    ...)
{
}

int appsrv_get_info(
    appsrv_handle appsrv,
    appsrv_info info,
    ...)
{
}

int appsrv_start(
    appsrv_handle appsrv)
{
}

int appsrv_stop(
    appsrv_handle appsrv)
{
}

void appsrv_wait_and_close(
    appsrv_handle appsrv)
{
}

void appsrv_free(
    void *data)
{
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