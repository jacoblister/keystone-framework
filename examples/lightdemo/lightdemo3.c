/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

#include "framework.c"

class CGWindowLightDemo : CGWindow {
 private:
   CGLayout layout;     
   bool notify_request_close(void);
 public:
   void CGWindowLightDemo(CObjServer *obj_server);
};

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
   CGWindowLightDemo window;
   CObjServer obj_server;
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

void CGWindowLightDemo::CGWindowLightDemo(CObjServer *obj_server) {
   new(&this->layout).CGLayout(0, 0, obj_server, obj_server->server_root);
   CGLayout(&this->layout).load_svg_file("lightdemo1.svg", NULL);
    
   CGWindow(this).CGWindow("Light Demo", CGCanvas(&this->layout), NULL);
}/*CGWindowLightDemo::CGWindowLightDemo*/

bool CGWindowLightDemo::notify_request_close(void) {
   CFramework(&framework).kill();
   return FALSE;
}/*CGWindowLightDemo::notify_close*/

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
   CString filename, message;
   CGDialogMessage dialog;

   new(&message).CString(NULL);
   new(&filename).CString(NULL); 
   
   if (ARRAY(args).count() < 2) {
      CString(&filename).set("lightdemo.xml");
   }
   else {
      CString(&filename).set(CString(&ARRAY(args).data()[1]).string());
   }
   CObjPersistent(this).state_xml_load_file(CString(&filename).string(), NULL, TRUE, &message);   

   if (CString(&message).length()) {
      new(&dialog).CGDialogMessage("Light Demo Error", CString(&message).string(), NULL);
      CGWindowDialog(&dialog).execute();
      delete(&dialog);
   }
   else {   
      new(&this->obj_server).CObjServer(CObjPersistent(this));

      new(&this->window).CGWindowLightDemo(&this->obj_server);
      CGObject(&this->window).show(TRUE);    
   
      CFramework(&framework).main();   

      delete(&this->obj_server);
   }
       
   delete(&filename);
   delete(&message);

   return 0;
}/*CLightDemo::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
