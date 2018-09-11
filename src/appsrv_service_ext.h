#ifndef APPSRV_SERVICE_EXT_H_
#define APPSRV_SERVICE_EXT_H_
#include "duktape.h"
#include "appsrv.h"

typedef void *(*appsrv_service_routine_create)(const char *name);
typedef void (*appsrv_service_routine_attch)(duk_context *duk, void *obj);
typedef void (*appsrv_service_routine_stop)(void *obj);

struct appsrv_service_cls_s
{
    char *cls_name;
    appsrv_service_routine_create create_routine;
    appsrv_service_routine_attch attach_routine;
    appsrv_service_routine_stop stop_routine;
    RB_ENTRY(appsrv_service_cls_s)
};

RB_HEAD(appsrv_service_cls_table, appsrv_service_cls_s);

struct appsrv_s;

int appsrv_service_reg(
    appsrv_handle *srv,
    const char *cls_name,
    appsrv_service_routine_create create_routine,
    appsrv_service_routine_attch attach_routine,
    appsrv_service_routine_stop stop_routine);

#endif