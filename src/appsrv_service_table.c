#include "appsrv_service_table.h"
#include "appsrv.h"
#include "appsrv_internal.h"
#include <string.h>
#include <stdlib.h>

static int service_name_compare(struct appsrv_service_s *l, struct appsrv_service_s *r)
{
    return strcmp(l->name, r->name);
}

RB_GENERATE_STATIC(appsrv_service_table, appsrv_service_s, tree_entry, service_name_compare);

int appsrv_service_start(struct appsrv_s *appsrv, const char *name, const char *cls, struct appsrv_service_s **service)
{
    int ret = APPSRV_E_OK;
    appsrv_mutex_acquire(&app->service_table_mutex);
    struct appsrv_service_s service_name;
    service_name.name = name;
    struct appsrv_service_s *srv = RB_FIND(appsrv_service_table, &appsrv->service_table, &service_name);
    if (srv)
    {
        ret = APPSRV_E_OBJECT_ALREADY_EXIST;
        goto cleanup;
    }
    srv = malloc(sizeof(struct appsrv_service_s));
    memset(srv, 0, sizeof(struct appsrv_service_s));
    srv->name = strdup(name);
    srv->cls = strdup(cls);
    srv->obj = obj;
    RB_INSERT(appsrv_service_table, &appsrv->service_table, srv);

    *service = srv;
cleanup:
    appsrv_mutex_release(&app->service_table_mutex);
    return ret;
}

int appsrv_service_get(struct appsrv_s *appsrv, const char *name, struct appsrv_service_s **service)
{
    appsrv_mutex_acquire(&app->service_table_mutex);
    int ret = APPSRV_E_OK;
    struct appsrv_service_s service_name;
    struct appsrv_service_s *srv = RB_FIND(appsrv_service_table, &appsrv->service_table, &service_name);
    if (!srv)
    {
        ret = APPSRV_E_OBJECT_NOT_EXIST;
        goto cleanup;
    }
    *service = srv;
cleanup:
    appsrv_mutex_release(&app->service_table_mutex);
    return ret;
}

int appsrv_service_query_status(struct appsrv_s *appsrv, const char *name, appsrv_service_status *status)
{
    int ret = APPSRV_E_OK;
    return ret;
}

//必须保证不会再有其他线程操作service_table
int appsrv_service_stop_all(struct appsrv_s *appsrv)
{
    int ret = APPSRV_E_OK;
    appsrv_mutex_acquire(&app->service_table_mutex);
    struct appsrv_service_s *srv = 0;
    RB_FOREACH(srv, appsrv_service_table, &appsrv->service_table)
    {
        srv->status = APPSRV_SERVICE_STATUS_STOPING;
        srv->fun->stop(srv->obj);
    }
cleanup:
    appsrv_mutex_release(&app->service_table_mutex);
    return ret;
}

int appsrv_service_set_stoped(appsrv_handle *srv, const char *name)
{
    int ret = APPSRV_E_OK;
    appsrv_mutex_acquire(&app->service_table_mutex);
    struct appsrv_service_s service_name;
    struct appsrv_service_s *srv = RB_FIND(appsrv_service_table, &appsrv->service_table, &service_name);
    if (!srv)
    {
        ret = APPSRV_E_OBJECT_NOT_EXIST;
        goto cleanup;
    }
    *service = srv;
cleanup:
    appsrv_mutex_release(&app->service_table_mutex);
    return ret;
}

int appsrv_service_test(struct appsrv_s *appsrv)
{
    int ret = APPSRV_E_OK;

    struct appsrv_service_s *serivce = 0;
    ret = appsrv_service_start(appsrv, "7", "websrv", &serivce);
    ret = appsrv_service_start(appsrv, "6", "websrv", &serivce);
    ret = appsrv_service_start(appsrv, "2", "websrv", &serivce);
    ret = appsrv_service_start(appsrv, "5", "websrv", &serivce);
    ret = appsrv_service_start(appsrv, "1", "websrv", &serivce);
    ret = appsrv_service_start(appsrv, "3", "websrv", &serivce);
    ret = appsrv_service_start(appsrv, "4", "websrv", &serivce);

    struct appsrv_service_s *obj = 0;
    int i = 1;
    RB_FOREACH(obj, appsrv_service_table, &appsrv->service_table)
    {
        assert(atoi(obj->name) == i);
        printf("service: %s\n", obj->name);
        i++;
    }

    return 0;
}
