#include "appsrv_thread.h"

#if defined(WIN32)

/* !checksrc! disable SPACEBEFOREPAREN 1 */
appsrv_thread_t appsrv_thread_create(
    unsigned int(APPSRV_STDCALL *func)(void *),
    void *arg)
{
    appsrv_thread_t t;
#ifdef _WIN32_WCE
    t = CreateThread(NULL, 0, func, arg, 0, NULL);
#else
    t = (appsrv_thread_t)_beginthreadex(NULL, 0, func, arg, 0, NULL);
#endif
    if ((t == 0) || (t == LongToHandle(-1L)))
    {
#ifdef _WIN32_WCE
        DWORD gle = GetLastError();
        errno = ((gle == ERROR_ACCESS_DENIED ||
                  gle == ERROR_NOT_ENOUGH_MEMORY)
                     ? EACCES
                     : EINVAL);
#endif
        return appsrv_thread_t_null;
    }
    return t;
}

void appsrv_thread_destroy(appsrv_thread_t hnd)
{
    CloseHandle(hnd);
}

int appsrv_thread_join(appsrv_thread_t *hnd)
{
#if !defined(_WIN32_WINNT) || !defined(_WIN32_WINNT_VISTA) || \
    (_WIN32_WINNT < _WIN32_WINNT_VISTA)
    int ret = (WaitForSingleObject(*hnd, INFINITE) == WAIT_OBJECT_0);
#else
    int ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);
#endif

    appsrv_thread_destroy(*hnd);

    *hnd = appsrv_thread_t_null;

    return ret;
}

#else

struct appsrv_actual_call
{
    unsigned int (*func)(void *);
    void *arg;
};

static void *appsrv_thread_create_thunk(void *arg)
{
    struct appsrv_actual_call *ac = arg;
    unsigned int (*func)(void *) = ac->func;
    void *real_arg = ac->arg;

    free(ac);

    (*func)(real_arg);

    return 0;
}

appsrv_thread_t appsrv_thread_create(unsigned int (*func)(void *), void *arg)
{
    appsrv_thread_t t = malloc(sizeof(pthread_t));
    struct appsrv_actual_call *ac = malloc(sizeof(struct appsrv_actual_call));
    if (!(ac && t))
        goto err;

    ac->func = func;
    ac->arg = arg;

    if (pthread_create(t, NULL, appsrv_thread_create_thunk, ac) != 0)
        goto err;

    return t;

err:
    free(t);
    free(ac);
    return appsrv_thread_t_null;
}

void appsrv_thread_destroy(appsrv_thread_t hnd)
{
    if (hnd != appsrv_thread_t_null)
    {
        pthread_detach(*hnd);
        free(hnd);
    }
}

int appsrv_thread_join(appsrv_thread_t *hnd)
{
    int ret = (pthread_join(**hnd, NULL) == 0);

    free(*hnd);
    *hnd = appsrv_thread_t_null;

    return ret;
}

#endif /* USE_THREADS_* */
