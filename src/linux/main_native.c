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

#if !FRAMEWORK_NOMAIN
int main(int argc, char *argv[]) {
   FRAMEWORK_sync_init();

   ((CObject *)&framework)->obj_sid = SID_OBJ;
   ((CObject *)&framework)->obj_class = &class(CFramework);

   CFramework(&framework).new();
//   gtk_timeout_add(20, CFramework__LINUX_timeout, NULL);   
   CFramework(&framework).execute(argc, argv);
   CFramework(&framework).delete();
   
   FRAMEWORK_sync_deinit();

   return 0;
}/*main*/
#endif

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
