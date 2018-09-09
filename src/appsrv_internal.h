#ifndef APPSRV_INTERNAL_H_
#define APPSRV_INTERNAL_H_
#include "appsrv.h"
#include <mongoose.h>

typedef struct
{
    char *data_path;
    char *main_script_path;
    char *http_file_bind_addr;
    char *http_bind_addr;
    char *mqtt_bind_addr;
    char *http_file_path;

    struct mg_mgr mgr;
    struct mg_connection *http_nc;
    struct mg_connection *http_file_nc;
    struct mg_connection *mqtt_nc;
    struct mg_serve_http_opts http_server_opts;
    struct mg_mqtt_broker mqtt_broker;

    char stop_sign : 1;
    unsigned long long conn_cur_id;
} appsrv_t;

typedef struct
{
    unsigned long long id;
    int type;
    struct mg_connection *nc;
} appsrv_conn_t;

#endif