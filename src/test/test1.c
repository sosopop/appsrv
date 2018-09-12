#include "../appsrv.h"
#include <stdio.h>

int test1()
{
    int ret = APPSRV_E_OK;
    appsrv_handle app = 0;
    ret = appsrv_create(".\\script", ".\\data", &app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

cleanup:
    if (app)
        appsrv_destroy(app);
    return ret;
}