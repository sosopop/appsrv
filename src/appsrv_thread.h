#ifndef APPSRV_THREAD_H_
#define APPSRV_THREAD_H_

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

#if defined(WIN32)
#define APPSRV_STDCALL __stdcall
#define appsrv_mutex_t CRITICAL_SECTION
#define appsrv_thread_t HANDLE
#define appsrv_thread_t_null (HANDLE)0
#if !defined(_WIN32_WINNT) || !defined(_WIN32_WINNT_VISTA) || \
    (_WIN32_WINNT < _WIN32_WINNT_VISTA)
#define appsrv_mutex_init(m) InitializeCriticalSection(m)
#else
#define appsrv_mutex_init(m) InitializeCriticalSectionEx(m, 0, 1)
#endif
#define appsrv_mutex_acquire(m) EnterCriticalSection(m)
#define appsrv_mutex_release(m) LeaveCriticalSection(m)
#define appsrv_mutex_destroy(m) DeleteCriticalSection(m)
#else
#define APPSRV_STDCALL
#define appsrv_mutex_t pthread_mutex_t
#define appsrv_thread_t pthread_t *
#define appsrv_thread_t_null (pthread_t *)0
#define appsrv_mutex_init(m) pthread_mutex_init(m, NULL)
#define appsrv_mutex_acquire(m) pthread_mutex_lock(m)
#define appsrv_mutex_release(m) pthread_mutex_unlock(m)
#define appsrv_mutex_destroy(m) pthread_mutex_destroy(m)
#endif

appsrv_thread_t appsrv_thread_create(
    unsigned int(APPSRV_STDCALL *func)(void *),
    void *arg);

void appsrv_thread_destroy(appsrv_thread_t hnd);

int appsrv_thread_join(appsrv_thread_t *hnd);

#endif