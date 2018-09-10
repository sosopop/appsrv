#ifndef APPSRV_SERVICE_TABLE_H_
#define APPSRV_SERVICE_TABLE_H_

/*
let websrv = APPSRV.startService("main_server", "websrv");
websrv = APPSRV.getService("main_server");
APPSRV.stopService("main_server");
*/

typedef struct
{
    char *name;
    char *cls;
    void *obj;
} appsrv_service;

int appsrv_service_start(const char *name, const char *cls, appsrv_service *service);
int appsrv_service_get(const char *name, appsrv_service *service);
int appsrv_service_stop(const char *name);

#endif