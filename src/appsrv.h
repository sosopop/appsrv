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
    typedef void *appsrv_task_container_handle;
    struct appsrv_data
    {
        char *data;
        unsigned int size;
    };

    typedef enum
    {
        APPSRV_TASK_STATUS_RUNNING,
        APPSRV_TASK_STATUS_PENDING,
        APPSRV_TASK_STATUS_DELAY
    } appsrv_task_status;

    struct appsrv_task_info
    {
        const char *task_name;
        appsrv_task_status status;
        unsigned int delay;
        const char *script;
    };

#define APPSRV_ERROR_MAP(XX)                         \
    XX(OK, "success")                                \
    XX(UNKNOWN, "unknown error happened")            \
    XX(INVALID_ARGS, "invalid arguments")            \
    XX(OBJECT_ALREADY_EXIST, "object already exist") \
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

    int appsrv_wait(
        appsrv_handle appsrv);

    int appsrv_stop(
        appsrv_handle appsrv);

    void appsrv_destroy(
        appsrv_handle appsrv);

    void appsrv_free(
        void *data);

    void appsrv_data_free(
        struct appsrv_data *data);

    int appsrv_create_task_container(
        appsrv_handle appsrv,
        const char *name,
        unsigned int concurrency,
        unsigned int persistence);

    int appsrv_empty_task_container(
        const char *name);

    int appsrv_count_task_container(
        const char *name,
        unsigned int *count);

    int appsrv_query_task_container(
        const char *name,
        unsigned int start,
        unsigned int in_count,
        struct appsrv_task_info *task_info,
        unsigned int *out_count);

    int appsrv_free_task_info(
        struct appsrv_task_info *task_info);

    int appsrv_destroy_task_container(
        const char *name);

    int appsrv_post_task(
        appsrv_handle appsrv,
        const char *task_container_name,
        unsigned int delay,
        const char *script);

    int appsrv_data_set(
        appsrv_handle appsrv,
        const char *key,
        unsigned int expire,
        struct appsrv_data *data);

    int appsrv_data_get(
        appsrv_handle appsrv,
        const char *key,
        struct appsrv_data *data);

    int appsrv_data_delete(
        appsrv_handle appsrv,
        const char *key);

    const char *appsrv_errno_description(
        appsrv_errno error);

    const char *appsrv_version();

#ifdef __cplusplus
}
#endif
#endif