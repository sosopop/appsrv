#ifndef APPSRV_THREAD_H_
#define APPSRV_THREAD_H_
#include "curl_threads.h"

#define APPSRV_STDCALL CURL_STDCALL
#define appsrv_mutex_t curl_mutex_t
#define appsrv_thread_t curl_thread_t
#define appsrv_thread_t_null curl_thread_t_null
#define appsrv_mutex_init(m) Curl_mutex_init(m)
#define appsrv_mutex_acquire(m) Curl_mutex_acquire(m)
#define appsrv_mutex_release(m) Curl_mutex_release(m)
#define appsrv_mutex_destroy(m) Curl_mutex_destroy(m)

#define appsrv_thread_create Curl_thread_create
#define appsrv_thread_destroy Curl_thread_destroy
#define appsrv_thread_join Curl_thread_join

#endif