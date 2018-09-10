#include "../appsrv.h"
#include <stdio.h>

int test()
{
    int ret = APPSRV_E_OK;
    appsrv_handle app = 0;
    ret = appsrv_create(".\\script", ".\\data", &app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

    char *script_out = 0;
    ret = appsrv_post_task(app, "function test(){console.log('hello appsrv');} test(); console.log(require('test'));");
    ret = appsrv_post_task(app, "JSON.stringify({a:'test',b:'hello'})");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_wait(app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_stop(app);
    if (APPSRV_E_OK != ret)
        goto cleanup;
cleanup:
    if (app)
        appsrv_destroy(app);
    return ret;
}