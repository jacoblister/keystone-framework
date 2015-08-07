/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework_base.c"
#include "../syncobject.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "native.h"

//#define CThread_NATIVE_DATA(thread) ((TThreadNative *)(&CThread(thread)->native_data))
#define CThread_NATIVE_DATA(thread) ((TThreadNative *)(&((CThread *)thread)->native_data))

pthread_key_t thread_key;
pthread_key_t object_key;

Local void *CThread_LINUX_entry(void *param) {
   CThread *this = CThread(param);

   pthread_setspecific(thread_key, (void *)this);
   pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

   CThread(this).entry();

   return NULL;
}/*CThread_LINUX_entry*/

void CThread::new(void) {
   /*>>>associate thread object pointer with thread local storage */
}/*CThread::new(void)*/

void CThread::CThread(void) {
   CThread_NATIVE_DATA(this)->thread_id =
      pthread_create(&CThread_NATIVE_DATA(this)->thread_id, NULL,
                     CThread_LINUX_entry, (void *)this);
}/*CThread::CThread*/

void CThread::sleep(int time_ms) {
   if (this->method) {
      (*this->method)(this->method_object, this->method_data);
      this->method = NULL;
//      pthread_cond_signal(&CThread_NATIVE_DATA(this)->cond);
   }

   sleep(time_ms / 1000);
}/*CThread::sleep*/

CSyncObject* CThread::select_object(ARRAY<CSyncObject *> *object, int timeout_ms) {
   /*>>>works for sockets only */
   int i, n = 0;
   CSyncObject *sync_object = NULL;
   fd_set readfds;
   struct timeval timeout;

   FD_ZERO(&readfds);
   for (i = 0; i < ARRAY(object).count(); i++) {
      sync_object = ARRAY(object).data()[i];
      if (CIOObject(sync_object)->handle + 1 > n) {
         n = CIOObject(sync_object)->handle + 1;
      }
      FD_SET(CIOObject(sync_object)->handle, &readfds);
   }

   CLEAR(&timeout);
   if (timeout_ms == -1) {
      select(n, &readfds, NULL, NULL, NULL);
   }
   else {
      timeout.tv_usec = timeout_ms * 1000;
      select(n, &readfds, NULL, NULL, &timeout);
   }

   /* identify which object is ready */
   CLEAR(&timeout);
   for (i = 0; i < ARRAY(object).count(); i++) {
      sync_object = ARRAY(object).data()[i];
      FD_ZERO(&readfds);
      FD_SET(CIOObject(sync_object)->handle, &readfds);

      if (select(CIOObject(sync_object)->handle + 1, &readfds, NULL, NULL, &timeout)) {
         return sync_object;
      }
   }

   return NULL;
}/*CThread::select_object*/

#if 0
CSyncObject* CThread::select_object(ARRAY<CSyncObject *> *object) {
   int i;
   CSyncObject *sync_object;
   fd_set readfds;
   struct timeval timeout;

   FD_ZERO(&readfds);
   for (i = 0; i < ARRAY(object).count(); i++) {
      sync_object = ARRAY(object).data()[i];
      FD_SET(CIOObject(sync_object)->handle, &readfds);
   }

   select(CIOObject(sync_object)->handle + 1, &readfds, NULL, NULL, NULL);

   /* identify which object is ready */
   CLEAR(&timeout);
   for (i = 0; i < ARRAY(object).count(); i++) {
      sync_object = ARRAY(object).data()[i];
      FD_ZERO(&readfds);
      FD_SET(CIOObject(sync_object)->handle, &readfds);

      if (select(CIOObject(sync_object)->handle + 1, &readfds, NULL, NULL, &timeout)) {
         return sync_object;
      }
   }
   ASSERT("select_object()::error");

   return NULL;
}/*CThread::select_object*/
#endif

int CThread::wait(void) {
   return 0;
}/*CThread::wait*/

void CThread::realtime_set(bool realtime) {
   /*>>>exceptions*/
   struct sched_param sparam;

   if (realtime) {
      CLEAR(&sparam);
      sparam.sched_priority = 1; /*real time process*/
      if (sched_setscheduler(0, SCHED_FIFO, &sparam)) {
      }
   }
   else {
      /*>>>implement*/
   }
}/*CThread::realtime*/

void CThreadMain::new(void) {
   pthread_key_create(&thread_key, NULL);
   pthread_setspecific(thread_key, (void *)this);

   class:base.new();
   framework.thread_main = CThread(this);
   CThread_NATIVE_DATA(this)->thread_id = pthread_self();
}/*CThreadMain::new*/

void CMutex::new(void) {
}/*CMutex::new*/

void CMutex::CMutex(void) {
   pthread_mutex_init(&CMutex_NATIVE_DATA(this)->mutex, NULL);
}/*CMutex::CMutex*/

void CMutex::delete(void) {
   pthread_mutex_destroy(&CMutex_NATIVE_DATA(this)->mutex);
}/*CMutex::delete*/

int CMutex::lock(void) {
   pthread_mutex_lock(&CMutex_NATIVE_DATA(this)->mutex);
   return TRUE;
}/*CMutex::lock*/

int CMutex::trylock(void) {
   return pthread_mutex_trylock(&CMutex_NATIVE_DATA(this)->mutex);
}/*CMutex::trylock*/

int CMutex::unlock(void) {
   pthread_mutex_unlock(&CMutex_NATIVE_DATA(this)->mutex);
   return TRUE;
}/*CMutex::unlock*/

CThread *CFramework::thread_this(void) {
   return (CThread *)pthread_getspecific(thread_key);
}/*CFramework::thread_this*/

void FRAMEWORK_sync_init(void) {
//   tls_index = TlsAlloc();        
//   tls_lastobject = TlsAlloc();            
   pthread_key_create(&object_key, NULL);
};
void FRAMEWORK_sync_deinit(void) {
//   TlsFree(tls_lastobject);           
//   TlsFree(tls_index);           
};

void *FRAMEWORK_last_object_set(void *object) {
//   TlsSetValue(tls_lastobject, object);    
   pthread_setspecific(object_key, (void *)object);
   return object;
}
void *FRAMEWORK_last_object() {
//    return TlsGetValue(tls_lastobject);
   return pthread_getspecific(object_key);
}

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

