#ifndef APPSRV_INTERNAL_H_
#define APPSRV_INTERNAL_H_
#include "appsrv.h"
#include <mongoose.h>
#include "appsrv_thread.h"

struct appsrv_s
{
    char *data_path;
    char *script_path;
    char stop_signal : 1;
};

#endif