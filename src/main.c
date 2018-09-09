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

    ret = appsrv_set_option(app, APPSRV_OPT_BIND_HTTP_ADDR, "0.0.0.0:0");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_set_option(app, APPSRV_OPT_BIND_HTTP_FILE_ADDR, "0.0.0.0:0");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_set_option(app, APPSRV_OPT_HTTP_FILE_PATH, "./");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_set_option(app, APPSRV_OPT_BIND_MQTT_ADDR, "0.0.0.0:0");
    if (APPSRV_E_OK != ret)
        goto cleanup;

    ret = appsrv_start(app);
    if (APPSRV_E_OK != ret)
        goto cleanup;

    char *addr = 0;

    ret = appsrv_get_info(app, APPSRV_INFO_BIND_HTTP_ADDR, &addr);
    if (APPSRV_E_OK != ret)
        goto cleanup;
    printf("http addr bind: %s\n", addr);
    appsrv_free(addr);

    ret = appsrv_get_info(app, APPSRV_INFO_BIND_HTTP_FILE_ADDR, &addr);
    if (APPSRV_E_OK != ret)
        goto cleanup;
    printf("http file addr bind: %s\n", addr);
    appsrv_free(addr);

    ret = appsrv_get_info(app, APPSRV_INFO_BIND_MQTT_ADDR, &addr);
    if (APPSRV_E_OK != ret)
        goto cleanup;
    printf("mqtt addr bind: %s\n", addr);
    appsrv_free(addr);

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
