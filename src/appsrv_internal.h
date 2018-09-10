#ifndef APPSRV_INTERNAL_H_
#define APPSRV_INTERNAL_H_
#include "appsrv.h"
#include <mongoose.h>
#include "appsrv_service_table.h"

typedef struct appsrv_s
{
    char *data_path;
    char *script_path;
    struct appsrv_service_table table;
    char stop_sign : 1;
} appsrv_t;

typedef struct appsrv_conn_s
{
    unsigned long long id;
    int type;
    struct mg_connection *nc;
} appsrv_conn_t;

#endif