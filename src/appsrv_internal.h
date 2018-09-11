#ifndef APPSRV_INTERNAL_H_
#define APPSRV_INTERNAL_H_
#include "appsrv.h"
#include <mongoose.h>
#include "appsrv_service_table.h"
#include "appsrv_service_ext.h"
#include "appsrv_thread.h"

struct appsrv_s
{
    char *data_path;
    char *script_path;
    appsrv_mutex_t service_table_mutex;
    struct appsrv_service_table service_table;
    appsrv_mutex_t service_cls_mutex;
    struct appsrv_service_cls_table service_cls_table;
    char stop_signal : 1;
};

struct appsrv_conn_s
{
    unsigned long long id;
    int type;
    struct mg_connection *nc;
};

#endif