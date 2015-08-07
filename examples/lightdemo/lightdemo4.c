/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

#include "framework.c"

class CGWindowLightDemo : CGWindow {
 private:
   CGLayout layout;
   CGSplitter splitter;
   CGLayout layout_main;
   CGLayout tree_layout;
   CGTree tree;
 
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
   static inline void switch_set(int index, bool state);
 public:
   ALIAS<"lightdemo">;
 
   ELEMENT:OBJECT<CLights lights>;
   ATTRIBUTE<ARRAY<bool> switchState> {
       ARRAY(&this->switchState).data()[attribute_element] = *data;
       CLightDemo(this).switch_set(attribute_element, *data);
   };
 
   void main(ARRAY<CString> *args);
};

static inline void CLightDemo::switch_set(int index, bool state) {
    CLight *light = CLight(CObject(&this->lights).child_n(index));
    
    if (light) {
        CObjPersistent(light).attribute_update(ATTRIBUTE<CLight,state>);
        CObjPersistent(light).attribute_set_int(ATTRIBUTE<CLight,state>, state);
        CObjPersistent(light).attribute_update_end();
    }
}/*CLightDemo::switch_set*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

OBJECT<CLightDemo, lightdemo>;

void CGWindowLightDemo::CGWindowLightDemo(CObjServer *obj_server) {
   new(&this->layout_main).CGLayout(0, 0, obj_server, obj_server->server_root);
   CGLayout(&this->layout_main).load_svg_file("lightdemo2.svg", NULL);

   new(&this->tree_layout).CGLayout(0, 0, NULL, NULL);
   new(&this->tree).CGTree(obj_server, obj_server->server_root, 0, 0, 0, 0);
   CObject(&this->tree_layout).child_add(CObject(&this->tree));

   new(&this->splitter).CGSplitter(0, 0, 0, 0);
   CObject(&this->splitter).child_add(CObject(&this->tree_layout));
   CObject(&this->splitter).child_add(CObject(&this->layout_main));

   new(&this->layout).CGLayout(0, 0, NULL, NULL);
   CObject(&this->layout).child_add(CObject(&this->splitter));
    
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
      CObjPersistent(&this->window).attribute_set_int(ATTRIBUTE<CGWindow,maximized>, TRUE);
//      CGWindow(&this->window).maximize(TRUE);           
   
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
