/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "base/environ.h"
#include "base/contain.h"
#include "base/exception.h"
#include "base/parameter.h"
#include "base/text.h"

/* define some common data container types */
ARRAY:typedef<char>;
ARRAY:typedef<char *>;
ARRAY:typedef<byte>;
ARRAY:typedef<int>;

#include "object.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*>>>must remove time.h references in favour to TUNIXTime type!*/
//#include "../datetime.c"
#include "time.h"
typedef unsigned long TUNIXTime;

#define KEYSTONE_LICENSE_LENGTH 200

/* Thread Local Storage Indexes */
ENUM:typedef<ETLSIndex> {
   {This}
};

class CApplication;
class CThread;
class CMemFileDirectory;
class CGWindow;

ARRAY:typedef<CObjClass *>;
ARRAY:typedef<TObjectDef *>;
ARRAY:typedef<CGWindow *>;

#define KEYSTONE_SCRATCH_BUFFERS 16

class CFramework : CObject {
 private:
   CThread *thread_main;
   void *last_object;
   bool executing;
   CObject *modal_object;
   int modal_count;
//   TObjClassDefBegin *cdef_begin;
   CObjClass         *obj_class_current;
//   CObjClass         *obj_persistent_class;
//   TPClassList pclass_list;
   ARRAY<CObjClass *> obj_class;
   ARRAY<TObjectDef *> auto_object;
   ARRAY<CGWindow *> window;
   CGWindow *active_window;           
   CGWindow *modal_window;
   bool full_graphics;
   CApplication *application;

   void *scratch_buffer[KEYSTONE_SCRATCH_BUFFERS];
#if OS_Win32
   ARRAY<int> tls_index;
#endif
   void new(void);
   int execute(int argc, char *argv[]);
   void delete(void);
   void initdata_search(void);
   CApplication *construct_auto_object(void);
   void NATIVE_new(void);
   void NATIVE_delete(void);
   void NATIVE_main_iteration(void);
   void NATIVE_main(void);
   void NATIVE_kill(void);
   bool display_device_resolution(int *res_x, int *res_y);
 public:
   CMemFileDirectory *memfile_directory;
   
   /*>>>not the ideal place for this */
   char xul_font_family[256];
   int xul_font_size;
   int xul_font_weight;
   int xul_font_style;

   void main(void);
   void kill(void);
   void NATIVE_modal_wait(CObject *object);
   void NATIVE_modal_exit(CObject *object);
   void beep(void);
//   void *static_buffer(void);
   void *scratch_buffer_allocate(void);
   void scratch_buffer_release(void *buffer);
   
   void debug_message(char *format, ...);

   CObjClass *obj_class_find(const char *class_name);
   void obj_class_list_all(void);
   static inline CThread *thread_main(void);
   CThread *thread_this(void);
       
   static inline CGWindow *window_active(void);
   static inline void window_active_set(CGWindow *window);
   
   void time_time(TUNIXTime *result);
   void time_time_local(TUNIXTime *result);   
   struct tm* time_localtime(TUNIXTime *time);

   void license_info(char *result);
   void license_components(char *result);
};

extern CFramework framework;

static inline CThread *CFramework::thread_main(void) {
   return this->thread_main;
}

static inline CGWindow *CFramework::window_active(void) {
    return this->active_window;
}

static inline void CFramework::window_active_set(CGWindow *window) {
    this->active_window = window;
}

void FRAMEWORK_sync_init(void);
void FRAMEWORK_sync_deinit(void);
//void *FRAMEWORK_last_object_set(void *object);
//void *FRAMEWORK_last_object();

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <time.h>

void CFramework::time_time(TUNIXTime *result) {
   time_t time_current;
   time(&time_current);
   *result = (TUNIXTime)time_current;
}

time_t mktime_gmt(struct tm *ptm) {
#if OS_Win32	
   return _mkgmtime(ptm);
#else
   return 0;
#endif	
}

void CFramework::time_time_local(TUNIXTime *result) {
   struct tm local;
   time_t time_current;
   
   time(&time_current);
   local = *(localtime(&time_current));
   *result = (TUNIXTime)mktime_gmt(&local);
//   *result = mktime(local);
}

struct tm* CFramework::time_localtime(TUNIXTime *time) {
   time_t time_current = *time;
   return localtime(&time_current);
}

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
