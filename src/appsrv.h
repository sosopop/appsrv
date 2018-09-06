#ifndef APPSRV_H_
#define APPSRV_H_
#include "appsrv_common.h"
#include "appsrv_task.h"

typedef enum
{
    APPSRV_OPT_DATA_PATH = 1,
    APPSRV_OPT_MAIN_SCRIPT_PATH,
    APPSRV_OPT_BIND_ADDR
} appsrv_option;

typedef enum
{
    APPSRV_INFO_BIND_ADDR = 1
} appsrv_info;

typedef void *appsrv_handle;

appsrv_handle appsrv_create();

int appsrv_set_option(
    appsrv_handle appsrv,
    appsrv_option opt,
    ...);

int appsrv_get_info(
    appsrv_handle appsrv,
    appsrv_info info,
    ...);

int appsrv_start(
    appsrv_handle appsrv);

int appsrv_stop(
    appsrv_handle appsrv);

void appsrv_wait_and_close(
    appsrv_handle appsrv);

void appsrv_free(
    void *data);

#endif