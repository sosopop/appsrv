#include "appsrv_service_ext.h"

static int service_cls_name_compare(struct appsrv_service_cls_s *l, struct appsrv_service_cls_s *r)
{
    return strcmp(l->name, r->name);
}

RB_GENERATE_STATIC(appsrv_service_cls_table, appsrv_service_cls_s, tree_entry, service_cls_name_compare);

int appsrv_service_reg(
    appsrv_handle *srv,
    const char *cls_name,
    appsrv_service_routine_create create_routine,
    appsrv_service_routine_attch attach_routine,
    appsrv_service_routine_stop stop_routine)
{
    int ret = APPSRV_E_OK;
    appsrv_mutex_acquire(&app->service_cls_mutex);
    int ret = APPSRV_E_OK;
    struct appsrv_service_cls_s service_cls_name;
    service_cls_name.name = name;
    struct appsrv_service_cls_s *srv_cls = RB_FIND(appsrv_service_cls_table, &appsrv->service_cls_table, &service_cls_name);
    if (srv_cls)
    {
        ret = APPSRV_E_OBJECT_ALREADY_EXIST;
        goto cleanup;
    }
    srv = malloc(sizeof(struct appsrv_service_cls_s));
    memset(srv, 0, sizeof(struct appsrv_service_cls_s));
    srv->name = strdup(name);
    srv->create_routine = create_routine;
    srv->attach_routine = attach_routine;
    srv->stop_routine = stop_routine;
    RB_INSERT(appsrv_service_cls_table, &appsrv->service_cls_table, srv);
cleanup:
    appsrv_mutex_release(&app->service_cls_mutex);
    return ret;
}