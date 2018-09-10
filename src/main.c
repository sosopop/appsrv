#include <stdio.h>
#include <assert.h>
#include "appsrv.h"
#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <windows.h>
#include <stdlib.h>
#include <crtdbg.h>
#endif

//target_link_libraries(uv advapi32 iphlpapi psapi userenv shell32 ws2_32)
int main(int argc, char const *argv[])
{
    int ret = APPSRV_E_OK;

#ifdef WIN32
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
    appsrv_handle app = 0;
    ret = appsrv_create(".\\script", ".\\data", &app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

    char *script_out = 0;
    ret = appsrv_exec_script(app, "function test(){console.log('hello appsrv');} test(); console.log(require('test'));", &script_out);
    if (script_out)
        printf("script exec result: %s\n", script_out);

    ret = appsrv_exec_script(app, "JSON.stringify({a:'test',b:'hello'})", &script_out);
    if (script_out)
        printf("script exec result: %s\n", script_out);

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

#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    assert(_CrtDumpMemoryLeaks() == 0);
#endif
    return 0;
}
