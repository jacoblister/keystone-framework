#ifndef I_THREADNATIVE
#define I_THREADNATIVE
#include "environ.h"

#include <windows.h>

typedef struct {
   HANDLE hThread;
} TThreadNative;

#define CThread_NATIVE_DATA(thread) ((TThreadNative *)(&((CThread *)thread)->native_data))

typedef struct {
   HANDLE hMutex;
} TMutexNative;

#define CMutex_NATIVE_DATA(mutex) ((TMutexNative *)(&((CMutex *)mutex)->native_data))

typedef struct {
   UINT message;
   WPARAM wParam;
   LPARAM lParam;
} TEventKeyNative;

#define CEventKey_NATIVE_DATA(event) ((TEventKeyNative *)(&((CEventKey *)event)->native_data))

#endif/*THREADNATIVE*/
