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
    int ret = 0;

#ifdef WIN32
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
    appsrv_global_init();

    appsrv_handle app = appsrv_create();
    if (!app)
        goto cleanup;

    ret = appsrv_set_option(app, APPSRV_OPT_DATA_PATH, ".\\test\\");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_set_option(app, APPSRV_OPT_MAIN_SCRIPT_PATH, "main.js");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_set_option(app, APPSRV_OPT_BIND_HTTP_ADDR, "0.0.0.0:7011");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_set_option(app, APPSRV_OPT_BIND_MQTT_ADDR, "0.0.0.0:7012");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_start(app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_poll(app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_stop(app);
    if (APPSRV_E_OK != ret)
        goto cleanup;
cleanup:
    if (app)
        appsrv_close(app);

    appsrv_global_uninit();
#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    assert(_CrtDumpMemoryLeaks() == 0);
#endif
    return 0;
}
