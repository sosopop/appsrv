#ifndef APPSRV_INTERNAL_H_
#define APPSRV_INTERNAL_H_
#include "appsrv.h"
#include <mongoose.h>
#include "appsrv_thread.h"
#include <sqlite3.h>

#define APPSRV_VERSION "0.0.0.1"
#define SQLITE_TIMEOUT 10 * 1000

struct appsrv_s
{
    char *data_path;
    char *script_path;
    char *data_db_name;
    sqlite3 *data_db;
    char stop_signal : 1;
};

#endif