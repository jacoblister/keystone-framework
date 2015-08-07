#ifndef I_THREADNATIVE
#define I_THREADNATIVE
#include "environ.h"

#include <sched.h>
#include <pthread.h>

typedef struct {
   pthread_t thread_id;
   pthread_cond_t cond;
} TThreadNative;

typedef struct {
   pthread_mutex_t mutex;
} TMutexNative;

#define CMutex_NATIVE_DATA(mutex) ((TMutexNative *)(&((CMutex *)mutex)->native_data))
#endif/*THREADNATIVE*/
