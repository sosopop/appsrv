#ifndef APPSRV_SERVICE_TABLE_H_
#define APPSRV_SERVICE_TABLE_H_
#include "tree.h"
/*
let websrv = APPSRV.startService("main_server", "websrv");
websrv = APPSRV.getService("main_server");
APPSRV.stopService("main_server");
*/

typedef struct appsrv_service_s
{
    char *name;
    char *cls;
    void *obj;
    RB_ENTRY(appsrv_service_s)
    tree_entry;
} appsrv_service_t;

RB_HEAD(appsrv_service_table, appsrv_service_s);

struct appsrv_s;

int appsrv_service_start(struct appsrv_s *appsrv, const char *name, const char *cls, appsrv_service_t **service);
int appsrv_service_get(struct appsrv_s *appsrv, const char *name, appsrv_service_t *service);
int appsrv_service_stop(struct appsrv_s *appsrv, const char *name);

int appsrv_service_test(struct appsrv_s *appsrv);
#endif