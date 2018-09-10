#ifndef APPSRV_SERVICE_EXT_H_
#define APPSRV_SERVICE_EXT_H_
#include "duktape.h"
#include "appsrv.h"

typedef struct
{
    typedef void *(*create)(const char *name);
    typedef void (*attch)(duk_context *duk, void *obj);
    typedef void (*stop)(void *obj);
} appsrv_service_cls;

int appsrv_service_reg(appsrv_handle *srv, const char *cls_name, appsrv_service_cls *cls);
int appsrv_service_set_stoped(appsrv_handle *srv, const char *name);

#endif