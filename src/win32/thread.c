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

//#include <io.h>
//#include <fcntl.h>
//#include <winsock2.h>
#include "native.h"

class CWin32MessageWindow : CObjPersistent {
 private:
   ARRAY<int> active_timer;
   HWND hwnd;
   void new(void);
   void delete(void);

   static inline void timer_add(int timer_id);
   static inline void timer_remove(int timer_id);
   static inline bool timer_exists(int timer_id);
};

OBJECT<CWin32MessageWindow, message_window>;

/*>>>for now, should store in main framework instance */
static int tls_index;
static int tls_lastobject;

Local LRESULT CALLBACK CWin32MessageWindow_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
   CTimer *timer;

   switch (message) {
   case WM_TIMER:
      /*>>>hack! should arange things so WM_TIMER message is never
      received when the timer object has been deleted */
      if (CWin32MessageWindow(&message_window).timer_exists(wParam)) {
         timer = CTimer((void *)wParam);
         (*timer->method)(timer->method_object, timer->method_data);
      }
      break;
   }

   return DefWindowProc(hwnd, message, wParam, lParam);
};

void CWin32MessageWindow::new(void) {
   static TCHAR szClassName[] = TEXT("MsgWindow");
   WNDCLASS cls;

   cls.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
   cls.lpfnWndProc   = CWin32MessageWindow_wnd_proc;
   cls.cbClsExtra    = 0;
   cls.cbWndExtra    = sizeof(long);
   cls.hInstance     = GetModuleHandle(NULL);
   cls.hIcon         = NULL;
   cls.hbrBackground = NULL;
   cls.lpszMenuName  = NULL;
   cls.lpszClassName = szClassName;
   cls.hCursor       = LoadCursor(NULL, IDC_ARROW);

   RegisterClass(&cls);

   this->hwnd =
      CreateWindowEx(0, szClassName, "Message Window", 0,
                     CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
                     0, 0, GetModuleHandle(NULL), (LPVOID)this);

   ARRAY(&this->active_timer).new();
};

void CWin32MessageWindow::delete(void) {
   ARRAY(&this->active_timer).delete();
};

static inline void CWin32MessageWindow::timer_add(int timer_id) {
   ARRAY(&this->active_timer).item_add(timer_id);
};

static inline void CWin32MessageWindow::timer_remove(int timer_id) {
   ARRAY(&this->active_timer).item_remove(timer_id);
};

static inline bool CWin32MessageWindow::timer_exists(int timer_id) {
   int *timer_id_p, i;

   for (i = 0; i < ARRAY(&this->active_timer).count(); i++) {
      timer_id_p = &ARRAY(&this->active_timer).data()[i];
      if (*timer_id_p == timer_id) {
         return TRUE;
      }
   }

   return FALSE;
};

Local DWORD WINAPI CThread_WIN32_entry(void *param) {
   CThread *this = CThread(param);
   int exit_code = 0;   /*>>>exit code from OBJ_VT not working yet*/

   TlsSetValue(tls_index, this);

   CThread(this).entry();
//>>>   OBJ_delete(C_OBJ(this));

   return exit_code;
}/*CThread_WIN32_entry*/

void CThread::new(void) {
}/*CThread::new*/

void CThread::CThread(void) {
   DWORD threadId;

   CThread_NATIVE_DATA(this)->hThread =
      CreateThread(NULL, 0, CThread_WIN32_entry,
                   (LPVOID)this, 0, &threadId);
}/*CThread::CThread*/

bool CThread::spawn(const char *process_name) {
   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

   return CreateProcess(NULL, (char *)process_name, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}/*CThread::spawn*/

void CThread::sleep(int time_ms) {
   if (this->method) {
      (*this->method)(this->method_object, this->method_data);
      this->method = NULL;
   }

   Sleep(time_ms);
}/*CThread::sleep*/

CSyncObject* CThread::select_object(ARRAY<CSyncObject *> *object, int timeout_ms) {
   /*>>>works for sockets only */
   /*not usable at present */
#if 0    
   int i;
   CSyncObject *sync_object;
   fd_set readfds;
   struct timeval timeout;

   FD_ZERO(&readfds);
   for (i = 0; i < ARRAY(object).count(); i++) {
      sync_object = ARRAY(object).data()[i];
      FD_SET((SOCKET)CIOObject(sync_object)->handle, &readfds);
   }

   CLEAR(&timeout);
   if (timeout_ms == -1) {
      select(/*CIOObject(sync_object)->handle + 1*/0, &readfds, NULL, NULL, NULL);
   }
   else {
      timeout.tv_usec = timeout_ms * 1000;
      select(/*CIOObject(sync_object)->handle + 1*/0, &readfds, NULL, NULL, &timeout);
   }

   /* identify which object is ready */
   CLEAR(&timeout);
   for (i = 0; i < ARRAY(object).count(); i++) {
      sync_object = ARRAY(object).data()[i];
      FD_ZERO(&readfds);
      FD_SET((SOCKET)CIOObject(sync_object)->handle, &readfds);

      if (select(CIOObject(sync_object)->handle + 1, &readfds, NULL, NULL, &timeout)) {
        return sync_object;
      }
   }
#endif
   return NULL;
}/*CThread::select_object*/

int CThread::wait(void) {
   DWORD result;

   WaitForSingleObject(CThread_NATIVE_DATA(this)->hThread, INFINITE);
   GetExitCodeThread(CThread_NATIVE_DATA(this)->hThread, &result);

   return result;
}/*CThread::wait*/

void CThread::realtime_set(bool realtime) {
   bool pri_class, thread_pri;

   pri_class = SetPriorityClass(CThread_NATIVE_DATA(this)->hThread, REALTIME_PRIORITY_CLASS);
   thread_pri = SetThreadPriority(CThread_NATIVE_DATA(this)->hThread, THREAD_PRIORITY_TIME_CRITICAL);

   printf("realtime thread set %d %d\n", pri_class, thread_pri);
}/*CThread::realtime_set*/

void CThreadMain::new(void) {
   class:base.new();
   framework.thread_main = CThread(this);
   CThread_NATIVE_DATA(this)->hThread = GetCurrentThread();
   TlsSetValue(tls_index, this);
}/*CThreadMain::new*/

void CMutex::new(void) {
}/*CMutex::new*/

void CMutex::CMutex(void) {
   CMutex_NATIVE_DATA(this)->hMutex = CreateMutex(NULL, FALSE, NULL);
}/*CMutex::CMutex*/

void CMutex::delete(void) {
   CloseHandle(CMutex_NATIVE_DATA(this)->hMutex);
}/*CMutex::delete*/

int CMutex::lock(void) {
   WaitForSingleObject(CMutex_NATIVE_DATA(this)->hMutex, INFINITE);
   return TRUE;
}/*CMutex::lock*/

int CMutex::trylock(void) {
   return (WaitForSingleObject(CMutex_NATIVE_DATA(this)->hMutex, 0) != WAIT_OBJECT_0);
}/*CMutex::trylock*/

int CMutex::unlock(void) {
   ReleaseMutex(CMutex_NATIVE_DATA(this)->hMutex);
   return TRUE;
}/*CMutex::unlock*/

void CTimer::CTimer(int interval_ms, CObject *object, THREAD_METHOD method, void *data) {
   this->method = method;
   this->method_object = object;
   this->method_data = data;

   this->timer_id = SetTimer(message_window.hwnd, (UINT_PTR)this, interval_ms, NULL);

   CWin32MessageWindow(&message_window).timer_add(this->timer_id);
}/*CTimer::CTimer*/

void CTimer::delete(void) {
   CWin32MessageWindow(&message_window).timer_remove(this->timer_id);

   KillTimer(NULL, this->timer_id);
}/*CTimer::delete*/

CThread *CFramework::thread_this(void) {
   return (CThread *)TlsGetValue(tls_index);
}/*CFramework::thread_this*/

void FRAMEWORK_sync_init(void) {
   tls_index = TlsAlloc();        
   tls_lastobject = TlsAlloc();            
};
void FRAMEWORK_sync_deinit(void) {
   TlsFree(tls_lastobject);           
   TlsFree(tls_index);           
};

void *FRAMEWORK_last_object_set(void *object) {
   TlsSetValue(tls_lastobject, object);    
   return object;
}
void *FRAMEWORK_last_object() {
    return TlsGetValue(tls_lastobject);
}

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

