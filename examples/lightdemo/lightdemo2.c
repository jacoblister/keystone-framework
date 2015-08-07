/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

#include "framework.c"

class CLights : CObjPersistent {
 public:
   ALIAS<"lights">;

   void CLights(void);
};

class CLight : CObjPersistent {
 public:
   ALIAS<"light">;
   ATTRIBUTE<bool state>;

   void CLight(void);
};

class CLightDemo : CApplication {
 private:
   void new(void);
   void delete(void);
 public:
   ALIAS<"lightdemo">;
 
   ELEMENT:OBJECT<CLights lights>;
   ATTRIBUTE<ARRAY<bool> switchState>;
 
   void main(ARRAY<CString> *args);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

OBJECT<CLightDemo, lightdemo>;

void CLightDemo::new(void) {
   ARRAY(&this->switchState).new();
   new(&this->lights).CLights();    
   CObject(&this->lights).parent_set(CObject(this));    
   
   class:base.new();
}/*CLightDemo::new*/

void CLightDemo::delete(void) {
   class:base.delete();

   delete(&this->lights);
   ARRAY(&this->switchState).delete();
}/*CLightDemo::delete*/

void CLights::CLights(void) {
}/*CLights::CLights*/

int CLightDemo::main(ARRAY<CString> *args) {
   CFile file;    
   CString filename, message;
   CGDialogMessage dialog;

   new(&message).CString(NULL);
   new(&filename).CString(NULL);    
    
   /* read input file chosen on command line and write it back to standard output */
   if (ARRAY(args).count() < 2) {
      CString(&filename).set("lightdemo.xml");
   }
   else {
      CString(&filename).set(CString(&ARRAY(args).data()[1]).string());
   }
   if (CObjPersistent(this).state_xml_load_file(CString(&filename).string(), NULL, TRUE, &message)) {
      new(&file).CFile();
      CIOObject(&file).open_handle(STDOUT);
      CObjPersistent(this).state_xml_store(CIOObject(&file), NULL);
      delete(&file);
   }

   if (CString(&message).length()) {
      new(&dialog).CGDialogMessage("Light Demo Error", CString(&message).string(), NULL);
      CGWindowDialog(&dialog).execute();
      delete(&dialog);
   }
   
   delete(&filename);
   delete(&message);

   return 0;
}/*CLightDemo::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
