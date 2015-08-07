/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGWindowCounter : CGWindow {
 private:
   bool notify_request_close(void);
 public:
   void CGWindowCounter(CGCanvas *layout);
};

ENUM:typedef<ECountCommand> {
   {Inc},
   {Dec},
   {Clear},
};

class CCounter : CApplication {
 private:
   CGWindowCounter window;
   CGLayout layout;
 
   static inline void exec_command(ECountCommand command);
 public:
   ALIAS<"counter">;
   CObjServer obj_server;

   ATTRIBUTE<ECountCommand command> {
      CCounter(this).exec_command(*data);
   };
   ATTRIBUTE<int value>;

   void main(ARRAY<CString> *args);
};

static inline void CCounter::exec_command(ECountCommand command) {
   CObjPersistent(this).attribute_update(ATTRIBUTE<CCounter,value>);
   switch (command) {
   case ECountCommand.Inc:
      this->value++;
      break;
   case ECountCommand.Dec:
      this->value--;
      break;
   case ECountCommand.Clear:
      this->value = 0;
      break;
   }
   CObjPersistent(this).attribute_update_end();  
}

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

OBJECT<CCounter, counter>;

void CGWindowCounter::CGWindowCounter(CGCanvas *layout) {
   CGWindow(this).CGWindow("Keystone Counter", layout, NULL);
}/*CGWindowCounter::CGWindowCounter*/

bool CGWindowCounter::notify_request_close(void) {
   CFramework(&framework).kill();
   return FALSE;
}/*CGWindowCounter::notify_close*/

int CCounter:main(ARRAY<CString> *args) {
   new(&this->obj_server).CObjServer(CObjPersistent(this));

   new(&this->layout).CGLayout(0, 0, &this->obj_server, CObjPersistent(this));
   CGLayout(&this->layout).load_svg_file("memfile:counter.svg", NULL);
   CGCanvas(&this->layout).colour_background_set(GCOL_DIALOG);

   new(&this->window).CGWindowCounter(CGCanvas(&this->layout));
   CGWindow(&this->window).disable_maximize(TRUE);
   CGObject(&this->window).show(TRUE);    

   CFramework(&framework).main();

   return 0;
}/*CCounter::main*/

