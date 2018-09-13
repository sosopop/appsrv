#ifndef APPSRV_INTERNAL_H_
#define APPSRV_INTERNAL_H_
#include "appsrv.h"
#include <mongoose.h>
#include "appsrv_thread.h"
#include <sqlite3.h>

struct appsrv_s
{
    char *data_path;
    char *script_path;
    char *data_db_name;
    sqlite3 *data_db;
    char stop_signal : 1;
};

#endif