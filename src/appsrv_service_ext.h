#ifndef APPSRV_SERVICE_EXT_H_
#define APPSRV_SERVICE_EXT_H_
#include "duktape.h"
#include "appsrv.h"

typedef void *(*appsrv_service_create_routine)(const char *name);

typedef struct
{
    void *(*create)(const char *name);
    void (*attch)(duk_context *duk, void *obj);
    void (*stop)(void *obj);
} appsrv_service_cls;

int appsrv_service_reg(appsrv_handle *srv, const char *cls_name, appsrv_service_cls *cls);
int appsrv_service_set_stoped(appsrv_handle *srv, const char *name);

#endif