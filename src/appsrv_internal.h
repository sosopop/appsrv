#ifndef APPSRV_INTERNAL_H_
#define APPSRV_INTERNAL_H_
#include "appsrv.h"

typedef struct
{
    const char *data_path;
    const char *main_script_path;
    const char *http_bind_addr;
    const char *mqtt_bind_addr;
    unsigned int service_type;
} appsrv_t;

#endif