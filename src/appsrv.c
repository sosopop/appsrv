#include "appsrv.h"
#include "appsrv_internal.h"
#include "appsrv_utils.h"
#include "appsrv_thread.h"
#include "appsrv_log.h"

//日志互斥变量
extern appsrv_mutex_t log_mutex;
//是否已经进行过初始化
volatile int inited = 0;

void appsrv_global_init()
{
    if (inited)
        return;
    inited = 1;
    appsrv_mutex_init(&log_mutex);
    appsrv_set_log_level(APPSRV_LOG_DEBUG);
}

void appsrv_global_uninit()
{
    if (!inited)
        return;
    appsrv_mutex_destroy(&log_mutex);
    inited = 0;
}

appsrv_handle appsrv_create()
{
    appsrv_global_init();

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
    int ret = APPSRV_E_OK;

    appsrv_t *app = (appsrv_t *)appsrv;
    va_list ap;
    va_start(ap, opt);

    switch (opt)
    {
    case APPSRV_OPT_DATA_PATH:
        if (app->data_path)
            free(app->data_path);
        app->data_path = strdup(va_arg(ap, const char *));
        break;
    case APPSRV_OPT_MAIN_SCRIPT_PATH:
        if (app->main_script_path)
            free(app->main_script_path);
        app->main_script_path = strdup(va_arg(ap, const char *));
        break;
    case APPSRV_OPT_BIND_HTTP_ADDR:
        if (app->http_bind_addr)
            free(app->http_bind_addr);
        app->http_bind_addr = strdup(va_arg(ap, const char *));
        break;
    case APPSRV_OPT_BIND_HTTP_FILE_ADDR:
        if (app->http_file_bind_addr)
            free(app->http_file_bind_addr);
        app->http_file_bind_addr = strdup(va_arg(ap, const char *));
        break;
    case APPSRV_OPT_BIND_MQTT_ADDR:
        if (app->mqtt_bind_addr)
            free(app->mqtt_bind_addr);
        app->mqtt_bind_addr = strdup(va_arg(ap, const char *));
        break;
    case APPSRV_OPT_HTTP_FILE_PATH:
        if (app->http_file_path)
            free(app->http_file_path);
        app->http_file_path = strdup(va_arg(ap, const char *));
        break;
    default:
        ret = APPSRV_E_INVALID_ARGS;
        break;
    }
    va_end(ap);
    return ret;
}

static char *appsrv_get_addr_info(struct mg_connection *nc)
{
    char addr[30] = {0};
    if (nc && mg_conn_addr_to_str(nc, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT))
    {
        return strdup(addr);
    }
    return 0;
}

int appsrv_get_info(
    appsrv_handle appsrv,
    appsrv_info info,
    ...)
{
    int ret = APPSRV_E_OK;

    appsrv_t *app = (appsrv_t *)appsrv;
    va_list ap;
    va_start(ap, info);

    switch (info)
    {
    case APPSRV_INFO_BIND_HTTP_ADDR:
    {
        char *addr = appsrv_get_addr_info(app->http_nc);
        if (addr)
        {
            *va_arg(ap, const char **) = addr;
        }
        else
        {
            ret = APPSRV_E_OBJECT_NOT_EXIST;
            goto cleanup;
        }
    }
    break;
    case APPSRV_INFO_BIND_HTTP_FILE_ADDR:
    {
        char *addr = appsrv_get_addr_info(app->http_file_nc);
        if (addr)
        {
            *va_arg(ap, const char **) = addr;
        }
        else
        {
            ret = APPSRV_E_OBJECT_NOT_EXIST;
            goto cleanup;
        }
    }
    break;
    case APPSRV_INFO_BIND_MQTT_ADDR:
    {
        char *addr = appsrv_get_addr_info(app->mqtt_nc);
        if (addr)
        {
            *va_arg(ap, const char **) = addr;
        }
        else
        {
            ret = APPSRV_E_OBJECT_NOT_EXIST;
            goto cleanup;
        }
    }
    break;
    default:
        ret = APPSRV_E_INVALID_ARGS;
        break;
    }
cleanup:
    va_end(ap);
    return ret;
}

static void http_file_ev_handler(struct mg_connection *nc, int ev, void *p)
{
    appsrv_t *app = (appsrv_t *)nc->mgr->user_data;
    if (ev == MG_EV_HTTP_REQUEST)
    {
        mg_serve_http(nc, (struct http_message *)p, app->http_server_opts);
    }
}

static void http_dynimac_ev_handler(struct mg_connection *nc, int ev, void *p)
{
    appsrv_t *app = (appsrv_t *)nc->mgr->user_data;
}

static void mqtt_ev_handler(struct mg_connection *nc, int ev, void *p)
{
    appsrv_t *app = (appsrv_t *)nc->mgr->user_data;
}

int appsrv_start(
    appsrv_handle appsrv)
{
    int ret = APPSRV_E_OK;
    appsrv_t *app = (appsrv_t *)appsrv;
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_start");
    appsrv_log(APPSRV_LOG_DEBUG, "opt->data_path: %s", app->data_path);
    appsrv_log(APPSRV_LOG_DEBUG, "opt->main_script_path: %s", app->main_script_path);
    appsrv_log(APPSRV_LOG_DEBUG, "opt->http_bind_addr: %s", app->http_bind_addr);
    appsrv_log(APPSRV_LOG_DEBUG, "opt->mqtt_bind_addr: %s", app->mqtt_bind_addr);
    appsrv_log(APPSRV_LOG_DEBUG, "opt->http_file_bind_addr: %s", app->http_file_bind_addr);
    appsrv_log(APPSRV_LOG_DEBUG, "opt->http_file_path: %s", app->http_file_path);

    mg_mgr_init(&app->mgr, app);

    if (!app->http_bind_addr)
    {
        ret = APPSRV_E_INVALID_OPTIONS;
        goto cleanup;
    }
    app->http_nc = mg_bind(&app->mgr, app->http_bind_addr, http_dynimac_ev_handler);
    if (app->http_nc == 0)
    {
        ret = APPSRV_E_ADDR_BIND;
        goto cleanup;
    }
    mg_set_protocol_http_websocket(app->http_nc);

    if (app->http_file_bind_addr)
    {
        if (!app->http_file_path)
        {
            ret = APPSRV_E_INVALID_OPTIONS;
            goto cleanup;
        }
        app->http_server_opts.document_root = app->http_file_path;
        app->http_file_nc = mg_bind(&app->mgr, app->http_file_bind_addr, http_file_ev_handler);
        if (app->http_file_nc == 0)
        {
            ret = APPSRV_E_ADDR_BIND;
            goto cleanup;
        }
        mg_set_protocol_http_websocket(app->http_file_nc);
    }
    if (app->mqtt_bind_addr)
    {
        app->mqtt_nc = mg_bind(&app->mgr, app->mqtt_bind_addr, mqtt_ev_handler);
        if (app->mqtt_nc == 0)
        {
            ret = APPSRV_E_ADDR_BIND;
            goto cleanup;
        }
        mg_mqtt_broker_init(&app->mqtt_broker, app);
        app->mqtt_nc->priv_2 = &app->mqtt_broker;
        mg_set_protocol_mqtt(app->mqtt_nc);
    }

cleanup:
    return ret;
}

int appsrv_poll(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_poll");
#if WIN32
    Sleep(1000);
#else
    sleep(1);
#endif
    return APPSRV_E_OK;
}

int appsrv_stop(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_stop");
    return APPSRV_E_OK;
}

void appsrv_close(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_close");
    appsrv_t *app = (appsrv_t *)appsrv;

    mg_mgr_free(&app->mgr);

    free(app->data_path);
    free(app->http_bind_addr);
    free(app->http_file_path);
    free(app->mqtt_bind_addr);
    free(app->http_file_bind_addr);
    free(app->main_script_path);
    free(app);
}

void appsrv_free(
    void *data)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_free");
    free(data);
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