#include "appsrv_service_table.h"
#include "appsrv.h"
#include "appsrv_internal.h"
#include <string.h>
#include <stdlib.h>

static int service_name_compare(appsrv_service_t *l, appsrv_service_t *r)
{
    return strcmp(l->name, r->name);
}

RB_GENERATE_STATIC(appsrv_service_table, appsrv_service_s, tree_entry, service_name_compare);

int appsrv_service_start(struct appsrv_s *appsrv, const char *name, const char *cls, appsrv_service_t **service)
{
	//¼ÓËø
    int ret = APPSRV_E_OK;
    appsrv_service_t *obj = malloc(sizeof(appsrv_service_t));
	memset(obj, 0, sizeof(appsrv_service_t));
	obj->name = strdup(name);
	obj->cls = strdup(cls);
    RB_INSERT(appsrv_service_table, &appsrv->table, obj);

    *service = obj;

    return ret;
}

int appsrv_service_get(struct appsrv_s *appsrv, const char *name, appsrv_service_t *service)
{
    int ret = APPSRV_E_OK;
    return ret;
}

int appsrv_service_stop(struct appsrv_s *appsrv, const char *name)
{
    int ret = APPSRV_E_OK;
    return ret;
}

int appsrv_service_test(struct appsrv_s *appsrv)
{
	int ret = APPSRV_E_OK;

	appsrv_service_t *serivce = 0;
	ret = appsrv_service_start(appsrv, "7", "websrv", &serivce);
	ret = appsrv_service_start(appsrv, "6", "websrv", &serivce);
	ret = appsrv_service_start(appsrv, "2", "websrv", &serivce);
	ret = appsrv_service_start(appsrv, "5", "websrv", &serivce);
	ret = appsrv_service_start(appsrv, "1", "websrv", &serivce);
	ret = appsrv_service_start(appsrv, "3", "websrv", &serivce);
	ret = appsrv_service_start(appsrv, "4", "websrv", &serivce);

	appsrv_service_t *obj = 0;
	int i = 1;
	RB_FOREACH(obj, appsrv_service_table, &appsrv->table)
	{
		assert(atoi(obj->name) == i);
		printf("service: %s\n", obj->name);
		i++;
	}

	return 0;
}
