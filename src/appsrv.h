#ifndef APPSRV_H_
#define APPSRV_H_

#if defined(WIN32)
#if defined(APPSRV_EXPORTS)
#ifdef __cplusplus
#define APPSRV_EXPORT extern "C" __declspec(dllexport)
#else
#define APPSRV_EXPORT __declspec(dllexport)
#endif
#else /* defined (APPSRV_EXPORTS) */
#ifdef __cplusplus
#define APPSRV_EXPORT extern "C"
#else
#define APPSRV_EXPORT
#endif
#endif
#else /* defined (_WIN32) */
#if defined(APPSRV_EXPORTS)
#ifdef __cplusplus
#define APPSRV_EXPORT extern "C" __attribute__((visibility("default")))
#else
#define APPSRV_EXPORT __attribute__((visibility("default")))
#endif
#else /* defined (APPSRV_EXPORTS) */
#ifdef __cplusplus
#define APPSRV_EXPORT extern "C"
#else
#define APPSRV_EXPORT
#endif
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        APPSRV_OPT_DATA_PATH = 1,
        APPSRV_OPT_MAIN_SCRIPT_PATH,
        APPSRV_OPT_BIND_HTTP_ADDR,
        APPSRV_OPT_BIND_MQTT_ADDR,
        APPSRV_OPT_SERVICE
    } appsrv_option;

    typedef enum
    {
        APPSRV_SERVICE_HTTP = 1,
        APPSRV_SERVICE_MQTT_OVER_WEBSOCKET
    } appsrv_service_type;

    typedef enum
    {
        APPSRV_INFO_BIND_HTTP_ADDR = 1,
        APPSRV_INFO_BIND_MQTT_ADDR
    } appsrv_info;

    typedef void *appsrv_handle;

#define APPSRV_ERROR_MAP(XX) \
    XX(OK, "success")        \
    XX(UNKNOWN, "unknown error happened")
#define APPSRV_ERROR_GEN(n, s) SP_E_##n,
    typedef enum
    {
        APPSRV_ERROR_MAP(APPSRV_ERROR_GEN)
    } appsrv_errno;
#undef APPSRV_ERROR_GEN

    appsrv_handle appsrv_create();

    int appsrv_set_option(
        appsrv_handle appsrv,
        appsrv_option opt,
        ...);

    int appsrv_get_info(
        appsrv_handle appsrv,
        appsrv_info info,
        ...);

    int appsrv_start(
        appsrv_handle appsrv);

    int appsrv_stop(
        appsrv_handle appsrv);

    void appsrv_wait_and_close(
        appsrv_handle appsrv);

    void appsrv_free(
        void *data);

    const char *appsrv_errno_description(
        appsrv_errno error);

    const char *appsrv_version();

#ifdef __cplusplus
}
#endif
#endif