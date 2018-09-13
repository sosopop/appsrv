#include "../appsrv.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int test()
{
    int ret = APPSRV_E_OK;
    appsrv_handle app = 0;
    ret = appsrv_create(".\\script", ".\\data", &app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

    struct appsrv_data input_data = {"test", 4};
    ret = appsrv_data_set(app, "a", &input_data);
    if (APPSRV_E_OK != ret)
        goto cleanup;

    struct appsrv_data output_data = {0};
    ret = appsrv_data_get(app, "a", &output_data);
    if (APPSRV_E_OK != ret)
        goto cleanup;
    assert(strncmp("test", output_data.data, output_data.size) == 0);

    char *script_out = 0;
    ret = appsrv_post_task(app, "", 0, "function test(){console.log('hello appsrv');} test(); console.log(require('test'));");
    ret = appsrv_post_task(app, "", 0, "JSON.stringify({a:'test',b:'hello'})");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_wait(app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_stop(app);
    if (APPSRV_E_OK != ret)
        goto cleanup;
cleanup:
    if (output_data.data)
        appsrv_free(output_data.data);

    if (app)
        appsrv_destroy(app);
    return ret;
}