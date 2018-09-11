#include "appsrv.h"
#include "appsrv_internal.h"
#include "appsrv_utils.h"
#include "appsrv_thread.h"
#include "appsrv_log.h"
#include "appsrv_script.h"

int appsrv_create(
    const char *script_path,
    const char *data_path,
    appsrv_handle *appsrv)
{
    int ret = APPSRV_E_OK;
    struct appsrv_s *handle = 0;
    handle = (struct appsrv_s *)malloc(sizeof(struct appsrv_s));
    memset(handle, 0, sizeof(struct appsrv_s));
    handle->data_path = strdup(data_path);
    handle->script_path = strdup(script_path);
    appsrv_mutex_init(&handle->service_table_mutex);
    appsrv_mutex_init(&handle->service_cls_mutex);
    *appsrv = handle;
    return ret;
}

int appsrv_post_task(
    appsrv_handle appsrv,
    const char *script)
{
    int ret = APPSRV_E_OK;
    duk_context *script_ctx = appsrv_script_create();
    if (!script_ctx)
    {
        ret = APPSRV_E_SCRIPT_CREATE_FAILED;
        goto cleanup;
    }
    if (appsrv_script_run(script_ctx, script) != 0)
    {
        ret = APPSRV_E_SCRIPT_RUN_FAILED;
        goto cleanup;
    }
cleanup:
    if (script_ctx)
    {
        appsrv_script_destroy(script_ctx);
    }
    return ret;
}

int appsrv_wait(
    appsrv_handle appsrv)
{
    struct appsrv_s *app = (struct appsrv_s *)appsrv;
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_wait");
    return APPSRV_E_OK;
}

int appsrv_stop(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_stop");
    struct appsrv_s *app = (struct appsrv_s *)appsrv;
    app->stop_signal = 1;
    return APPSRV_E_OK;
}

void appsrv_destroy(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_destroy");
    struct appsrv_s *app = (struct appsrv_s *)appsrv;

    appsrv_mutex_destroy(&app->service_table_mutex);
    appsrv_mutex_destroy(&app->service_cls_mutex);
    free(app->data_path);
    free(app->script_path);
    free(app);
}

void appsrv_free(
    void *data)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_free");
    free(data);
}

#define APPSRV_ERROR_GEN(n, s) {"APPSRV_E_" #n, s},
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