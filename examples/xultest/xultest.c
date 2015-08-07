/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

ENUM:typedef<ETimedEventSource> {
   {master}, {remote}
};

class CTimedEvent : CObjPersistent {
 private:
   void new(void);
   void delete(void);
 public:
   void CTimedEvent(int time, char *point, ETimedEventSource source, int value);
   ALIAS<"xul:event">;
   
   ATTRIBUTE<TUNIXTime time>;
   ATTRIBUTE<CString point>;
   ATTRIBUTE<ETimedEventSource source>;
   ATTRIBUTE<int value>;
};

class CTimedEvents : CObjPersistent {
 public:
   void CTimedEvents(void);
   ALIAS<"xul:events">;
};

ENUM:typedef<EXULTestList> {
   {ItemA},
   {ItemB},
   {ItemC},
};

class CGWindowXULTest : CGWindow {
 private:
   bool notify_request_close(void);
 public:
   void CGWindowXULTest(CGCanvas *layout);
};

class CXULTest : CApplication {
 private:
   CGWindowXULTest window;
   CGLayout layout;
   CObjServer obj_server; 
 
   CGMenuItem menuheader;
   CGMenuItem menuitem[4];
   MENU<bool menu_child, "/_Menu/Child"> {
      CXULTest(this).disable_menu();
   };
   MENU<bool menu_sep, "/_Menu/-">;
   MENU<bool menu_expand, "/_Menu/Expand">;
   MENU<bool menu_expand_submenu, "/_Menu/Expand/Submenu">;   
   MENU<bool menu_sep_2, "/_Menu/-">;   
   MENU<bool menu_reload, "/_Menu/Reload"> {
       CGLayout(&this->layout).load_svg_file("xultest.svg", NULL);       
   };
   MENU<bool menu_dump_layout, "/_Menu/Dump Layout"> {
      CFile file;
      new(&file).CFile();
      CIOObject(&file).open_handle(STDOUT);
      CObjPersistent(&this->layout).state_xml_store(CIOObject(&file), NULL);
      delete(&file);   
   };
   MENU<bool menu_dump_events, "/_Menu/Dump Events"> {
      CFile file;
      new(&file).CFile();
      CIOObject(&file).open_handle(STDOUT);
      CObjPersistent(&this->events).state_xml_store(CIOObject(&file), NULL);
      delete(&file);   
   };
   MENU<bool layout_freeze, "/_Layout/Free_ze", "Ctrl+Z"> {
      CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_freeze);    
   };
   MENU<bool layout_animate, "/_Layout/_Animate", "Ctrl+A"> {
      CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_animate);    
   };
   
   void disable_menu(void);
 public:
   void main(ARRAY<CString> *args);
 
   ELEMENT:OBJECT<CTimedEvents events, "xul:events">;
   ATTRIBUTE<bool toggle>;
   ATTRIBUTE<EXULTestList list> {
       if (data) {       
           this->list = *data;
       }
       else {
           this->list = EXULTestList.ItemA;
       }
       printf("list set %d\n", this->list);
   };
   ATTRIBUTE<int x_position> {
       this->x_position = *data;
   };
   ATTRIBUTE<int y_position> {
       this->y_position = *data;
   };
   ATTRIBUTE<int progress> {
      this->progress = data ? *data : 0;
      printf("progress = %d\n", this->progress);
   };
   ATTRIBUTE<TGColour colour>;
   ATTRIBUTE<TUNIXTime time>;
   ATTRIBUTE<CString textvalue>;   
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

CObjClass *gridedit = &class(CGGridEdit);

void CTimedEvent::new(void) {
   new(&this->point).CString(NULL);
}/*CTimedEvent::new*/

void CTimedEvent::CTimedEvent(int time, char *point, ETimedEventSource source, int value) {
   this->time = time;
   CString(&this->point).set(point);
   this->source = source;
   this->value = value;   
}/*CTimedEvent::CTimedEvent*/

void CTimedEvent::delete(void) {
   delete(&this->point);
}/*CTimedEvent::delete*/

void CTimedEvents::CTimedEvents(void) {
   int i;
   CTimedEvent *event;
   TUNIXTime time;
   
   CFramework(&framework).time_time_local(&time);
   
   for (i = 0; i < 100; i++) {
      event = new.CTimedEvent(time, "S001000", ETimedEventSource.master, i);
      CObject(this).child_add(CObject(event));
      time += 60 * 60 * 24;
   }
}/*CTimedEvents::CTimedEvents*/

void CGWindowXULTest::CGWindowXULTest(CGCanvas *layout) {
   CGWindow(this).CGWindow("XUL Test", layout, NULL);
}/*CGWindowXULTest::CGWindowXULTest*/

bool CGWindowXULTest::notify_request_close(void) {
   CFramework(&framework).kill();
   return FALSE;
}/*CGWindowXULTest::notify_request_close*/

OBJECT<CXULTest, xul_test_app>;

void CXULTest::disable_menu(void) {
   CGButton *button = CGButton(CObject(&this->layout).child_first());
   CObjPersistent(button).attribute_update(ATTRIBUTE<CGButton,label>);
   CObjPersistent(button).attribute_set_text(ATTRIBUTE<CGButton,label>, CGListItem(&this->menuitem[1])->disabled ? "Enabled" : "Disabled");
   CObjPersistent(button).attribute_update_end();   
    
   CObjPersistent(&this->menuitem[1]).attribute_update(ATTRIBUTE<CGListItem,disabled>);
   CObjPersistent(&this->menuitem[1]).attribute_set_int(ATTRIBUTE<CGListItem,disabled>, !CGListItem(&this->menuitem[1])->disabled);
   CObjPersistent(&this->menuitem[1]).attribute_update_end();   

   CObjPersistent(&this->menuitem[2]).attribute_update(ATTRIBUTE<CGListItem,checked>);
   CObjPersistent(&this->menuitem[2]).attribute_set_int(ATTRIBUTE<CGListItem,checked>, !CGListItem(&this->menuitem[2])->checked);
   CObjPersistent(&this->menuitem[2]).attribute_update_end();   
}/*CXULTest::disable_menu*/

int CXULTest:main(ARRAY<CString> *args) {
   /*>>>quick test, set date to tomrrow */
   CFramework(&framework).time_time_local(&this->time);
   this->time += 60 * 60 * 24;

   new(&this->textvalue).CString("textvalue");
   
   new(&this->events).CTimedEvents();
   CObject(&this->events).parent_set(CObject(this));
   
   new(&this->obj_server).CObjServer(CObjPersistent(this));    
   new(&this->layout).CGLayout(0, 0, &this->obj_server, CObjPersistent(this));
   CGLayout(&this->layout).render_set(EGLayoutRender.buffered);
//   CGCanvas(&this->layout).render_set(EGCanvasRender.full);
   
   if (ARRAY(args).count() == 2) {
       CGLayout(&this->layout).load_svg_file(CString(&ARRAY(args).data()[1]).string(), NULL);       
   }
   else {
       CGLayout(&this->layout).load_svg_file("xultest.svg", NULL);
   }
   
//   CGCanvas(&this->layout).colour_background_set(GCOL_NONE);
   CGCanvas(&this->layout)->component_reposition = TRUE;    

   new(&this->window).CGWindowXULTest(CGCanvas(&this->layout));
   CGWindow(&this->window).object_menu_add(CObjPersistent(this), FALSE);

   new(&this->menuheader).CGMenuItem("_Dynamic", NULL, 0, FALSE, FALSE);
//   CObjPersistent(&this->menuheader).attribute_set_int(ATTRIBUTE<CGListItem,disabled>, TRUE);
   CObject(&CGWindow(&this->window)->menu).child_add(CObject(&this->menuheader));
   new(&this->menuitem[0]).CGMenuItem("Item _A", "Alt+A", 0, FALSE, TRUE);
   CObjPersistent(&this->menuitem[0]).attribute_set_int(ATTRIBUTE<CGListItem,type>, EGListItemType.checkbox);
   CObject(&this->menuheader).child_add(CObject(&this->menuitem[0]));

   new(&this->menuitem[1]).CGMenuItem("Item _B", NULL, 0, FALSE, FALSE);
   this->menuitem[1].accelerator.key = EEventKey.Ascii;
   this->menuitem[1].accelerator.value = 'B';
   this->menuitem[1].accelerator.modifier = (1 << EEventModifier.altKey);
   CObject(&this->menuheader).child_add(CObject(&this->menuitem[1]));
   
   new(&this->menuitem[2]).CGMenuItem("Item _C", "Alt+C", 0, FALSE, FALSE);
   CObjPersistent(&this->menuitem[2]).attribute_set_int(ATTRIBUTE<CGListItem,type>, EGListItemType.checkbox);   
   CObject(&this->menuheader).child_add(CObject(&this->menuitem[2]));

   /*>>>kludge*/
   CObjPersistent(&CGWindow(&this->window)->menu).notify_all_update(FALSE);
   CGObject(&this->window).show(TRUE);
   CObjPersistent(&this->window).attribute_set_int(ATTRIBUTE<CGWindow,maximized>, TRUE);   
//   CGXULElement(&this->layout).NATIVE_focus_in();   

   CFramework(&framework).main();
   
   delete(&this->textvalue);

   return 0;
}/*CXULTest::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
