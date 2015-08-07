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

#include <windows.h>

#if !FRAMEWORK_NOMAIN
/* Note, two versions of main.  Once which is used depends on /SUBSYSTEM link option */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
#if 1
   FRAMEWORK_sync_init();
                       
   ((CObject *)&framework)->obj_sid = SID_OBJ;
   ((CObject *)&framework)->obj_class = &class(CFramework);

   CFramework(&framework).new();
   CFramework(&framework).execute(__argc, __argv);
   CFramework(&framework).delete();
   
   FRAMEWORK_sync_deinit();      
#else
   char *argv[2];

   ((CObject *)&framework)->obj_sid = SID_OBJ;
   ((CObject *)&framework)->obj_class = &class(CFramework);

   printf("cmdline:%d,%s\n", strlen(lpCmdLine), lpCmdLine);

   CFramework(&framework).new();
   argv[0] = "application.exe";
   argv[1] = lpCmdLine;
   CFramework(&framework).execute(strlen(lpCmdLine) ? 2 : 1, argv);
   CFramework(&framework).delete();

   return 0;
#endif
   }/*WinMain*/

int main(int argc, char *argv[]) {
   FRAMEWORK_sync_init();
    
   ((CObject *)&framework)->obj_sid = SID_OBJ;
   ((CObject *)&framework)->obj_class = &class(CFramework);

   CFramework(&framework).new();
   CFramework(&framework).execute(argc, argv);
   CFramework(&framework).delete();
   
   FRAMEWORK_sync_deinit();   

   return 0;
}/*main*/
#endif

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
