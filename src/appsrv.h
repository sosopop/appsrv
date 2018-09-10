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
        APPSRV_OPT_DATA_PATH = 0,
        APPSRV_OPT_SCRIPT_PATH
    } appsrv_option;

    typedef void *appsrv_handle;

#define APPSRV_ERROR_MAP(XX)                         \
    XX(OK, "success")                                \
    XX(UNKNOWN, "unknown error happened")            \
    XX(INVALID_ARGS, "invalid arguments")            \
    XX(INVALID_OPTIONS, "invalid options")           \
    XX(ADDR_BIND, "address bind failed")             \
    XX(OBJECT_NOT_EXIST, "object not exist")         \
    XX(SCRIPT_CREATE_FAILED, "script create failed") \
    XX(SCRIPT_RUN_FAILED, "script run failed")
#define APPSRV_ERROR_GEN(n, s) APPSRV_E_##n,
    typedef enum
    {
        APPSRV_ERROR_MAP(APPSRV_ERROR_GEN)
    } appsrv_errno;
#undef APPSRV_ERROR_GEN

    int appsrv_create(
        const char *script_path,
        const char *data_path,
        appsrv_handle *appsrv);

    int appsrv_exec_script(
        appsrv_handle appsrv, const char *script, char **out);

    int appsrv_wait(
        appsrv_handle appsrv);

    int appsrv_stop(
        appsrv_handle appsrv);

    void appsrv_destroy(
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