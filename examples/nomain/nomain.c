/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CHelloWorldApp : CApplication {
 private:
   CGDialogMessage message;
 public:
   void main(ARRAY<CString> *args);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

OBJECT<CHelloWorldApp, helloworldapp>;

int CHelloWorldApp::main(ARRAY<CString> *args) {
   new(&this->message).CGDialogMessage("Hello Keystone!", "Hello World!", NULL);
   CGWindowDialog(&this->message).execute_wait();
//   CGWindowDialog(&this->message).wait(TRUE);
   while (1) {
      CFramework(&framework).NATIVE_main_iteration();   
   }
   CGWindowDialog(&this->message).close();
   delete(&this->message);

   return 0;
}/*CHelloWorldApp::main*/

int main(int argc, char *argv[]) {
printf("mainline\n");    
   FRAMEWORK_sync_init();        
   ((CObject *)&framework)->obj_sid = SID_OBJ;
   ((CObject *)&framework)->obj_class = &class(CFramework);

   CFramework(&framework).new();
   CFramework(&framework).execute(argc, argv);
   CFramework(&framework).delete();
   FRAMEWORK_sync_deinit();       

   return 0;
}/*main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
