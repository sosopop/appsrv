#include "../appsrv.h"
#include <stdio.h>
#include "../appsrv_service_table.h"

int test1()
{
    int ret = APPSRV_E_OK;
    appsrv_handle app = 0;
    ret = appsrv_create(".\\script", ".\\data", &app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

	ret = appsrv_service_test(app);
cleanup:
    if (app)
        appsrv_destroy(app);
    return ret;
}