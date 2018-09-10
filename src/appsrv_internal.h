#ifndef APPSRV_INTERNAL_H_
#define APPSRV_INTERNAL_H_
#include "appsrv.h"
#include <mongoose.h>

typedef struct
{
    char *data_path;
    char *script_path;
    char stop_sign : 1;
} appsrv_t;

typedef struct
{
    unsigned long long id;
    int type;
    struct mg_connection *nc;
} appsrv_conn_t;

#endif