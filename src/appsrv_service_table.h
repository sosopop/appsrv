#ifndef APPSRV_SERVICE_TABLE_H_
#define APPSRV_SERVICE_TABLE_H_
#include "tree.h"
/*
let websrv = APPSRV.startService("main_server", "websrv");
websrv = APPSRV.getService("main_server");
APPSRV.stopService("main_server");
*/

typedef enum
{
    APPSRV_SERVICE_STATUS_RUNNING,
    APPSRV_SERVICE_STATUS_STOPING,
    APPSRV_SERVICE_STATUS_STOPED
} appsrv_service_status;

struct appsrv_service_s
{
    char *name;
    char *cls;
    struct appsrv_service_cls_s *fun;
    void *obj;
    char status : 7;
    RB_ENTRY(appsrv_service_s)
    tree_entry;
};

RB_HEAD(appsrv_service_table, appsrv_service_s);

struct appsrv_s;

int appsrv_service_create(struct appsrv_s *appsrv, const char *name, const char *cls, struct appsrv_service_cls_s *obj, struct appsrv_service_s **service);
int appsrv_service_get(struct appsrv_s *appsrv, const char *name, struct appsrv_service_s **service);
int appsrv_service_query_status(struct appsrv_s *appsrv, const char *name, appsrv_service_status *status);
int appsrv_service_stop_all(struct appsrv_s *appsrv);
int appsrv_service_set_stoped(appsrv_handle *srv, const char *name);

int appsrv_service_test(struct appsrv_s *appsrv);
#endif