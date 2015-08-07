/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/
/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "../main.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#define NO_GTK FALSE

#include "../../syncobject.c"
#include <gtk/gtk.h>

#if !FRAMEWORK_NOMAIN && OS_Linux
Local gint CFramework__LINUX_timeout(gpointer data) {
   CThread(CFramework(&framework).thread_this()).sleep(0);

   return TRUE;
}/*CFramework__LINUX_timeout*/
#endif

void CFramework::NATIVE_new(void) {
#if !NO_GTK
   gtk_init(0, NULL);
   gdk_rgb_init();
#if OS_Linux
   gtk_timeout_add(20, CFramework__LINUX_timeout, NULL);
#endif
#endif
}/*CFramework::NATIVE_new*/

void CFramework::NATIVE_delete(void) {
}/*CFramework::NATIVE_delete*/

void CFramework::NATIVE_main_iteration(void) {
   gtk_main_iteration();
}/*CFramework::NATIVE_main_iteration*/

void CFramework::NATIVE_main(void) {
#if !NO_GTK
   gtk_main();
#endif
}/*CFramework::main*/

void CFramework::NATIVE_modal_wait(CObject *object) {
#if 1
   int modal_count_entry = this->modal_count + 1;

   this->modal_count = modal_count_entry;

   while (this->modal_count >= modal_count_entry) {
      gtk_main_iteration();
   }
#else
   if (this->modal_object) {
      ASSERT("CFramework::NATIVE_modal_wait: already waiting");
   }
   this->modal_object = object;

   while (this->modal_object) {
      gtk_main_iteration();
   }
#endif
}/*CFramework::NATIVE_modal_wait*/

void CFramework::NATIVE_modal_exit(CObject *object) {
#if 1
   this->modal_count--;
#else
   this->modal_object = NULL;
#endif
}/*CFramework::NATIVE_modal_exit*/

void CFramework::NATIVE_kill(void) {
#if !NO_GTK
   gtk_main_quit();
#endif
}/*CFramework::NATIVE_kill*/

void CFramework::beep(void) {
}/*CFramework::beep*/

bool CFramework::display_device_resolution(int *res_x, int *res_y) {
   /*>>> fake it */
   *res_x = 1024;
   *res_y = 768;

   return TRUE;
}/*CFramework::display_device_resolution*/

void CFramework::debug_message(char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   va_start(args, format);

   vsprintf(output, format, args);
   printf("%s", output);

   CFramework(&framework).scratch_buffer_release(output);
}/*CFramework::debug_message*/

void CApplication::NATIVE_set_home_path(char *argv0) {
   /*>>>get path of executable */
}/*CApplication::NATIVE_set_home_path*/

#if OS_Linux
Local gint CTimer__LINUX_interval(gpointer data) {
   CTimer *this = CTimer(data);

   (*this->method)(this->method_object, this->method_data);

   return TRUE;
}/*CTimer__WIN32_interval*/

void CTimer::CTimer(int interval_ms, CObject *object, THREAD_METHOD method, void *data) {
   this->method = method;
   this->method_object = object;
   this->method_data = data;

   this->timer_id = gtk_timeout_add(interval_ms, CTimer__LINUX_interval, this);
}/*CTimer::CTimer*/

void CTimer::delete(void) {
   gtk_timeout_remove(this->timer_id);
}/*CTimer::delete*/
#endif

void *Win32_ATL_wndproc;
void *Win32_ATL_wndproc_subclassed;

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
