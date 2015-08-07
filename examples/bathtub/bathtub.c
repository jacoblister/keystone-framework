/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CBathTap : CObjPersistent {
 public:
   ALIAS<"tap">;

   ATTRIBUTE<int temperature>;
   ATTRIBUTE<bool on> {
      this->on = *data;
   };

   void CBathTap(int temperature);
};

class CBathPlug : CObjPersistent {
 public:
   ALIAS<"plug">;

   ATTRIBUTE<bool in> {
      this->in = *data;
//      throw(CObject(this), NULL, "control locked");
   };

   void CBathPlug(void);
};

class CBathControl : CObjPersistent {
 private:
   void new(void);
   void ~CBathControl(void);
 public:
   ALIAS<"bath-control">;

   ATTRIBUTE<bool active> {
      this->active = *data;
   };
   ATTRIBUTE<float temperature> {
      this->temperature = data ? *data : 25;
   };
   ATTRIBUTE<float depth> {
      this->depth = data ? *data : 80;
   };

   void CBathControl(void);
};

class CBathTub : CObjPersistent {
 private:
   void new(void);
   void ~CBathTub(void);

   CObjServer obj_server;
   CBathControl bath_control;
 public:
   ALIAS<"bathtub">;

   ATTRIBUTE<bool controlSetup> {
      CGLayout layout;
      CGWindowDialog dialog;

      new(&layout).CGLayout(0, 0, &this->obj_server, CObjPersistent(this));

      CGLayout(&layout).load_svg_file("memfile:control.svg", NULL);
      CGCanvas(&layout).colour_background_set(GCOL_DIALOG);
      new(&dialog).CGWindowDialog("Bathtub Control", CGCanvas(&layout), NULL);
      CGWindowDialog(&dialog).button_add("OK", 0);
      CGWindowDialog(&dialog).execute();
      delete(&dialog);
   };

   ATTRIBUTE<float water_depth> {
      this->water_depth = *data;

      if (this->water_depth > 100) {
         this->water_depth = 100;
      }
      if (this->water_depth < 0) {
         this->water_depth = 0;
      }
   };
   ATTRIBUTE<bool water_overflow>;
   ATTRIBUTE<float water_temperature>;
   ATTRIBUTE<float air_temperature>;
   ELEMENT:OBJECT<CBathTap hot_tap>;
   ELEMENT:OBJECT<CBathTap cold_tap>;
   ELEMENT:OBJECT<CBathPlug plug>;

   void CBathTub(void);
};

class CBathSimulator : CApplication {
 private:
   CTimer iterate_timer;

   CGWindow window;
   CGLayout layout;
 public:
   CBathTub bathtub;

   void iterate(void);

   void main(ARRAY<CString> *args);

   MENU<bool file_save, "/File/Save Screen"> {
   };
   MENU<bool file_new, "/File/Quit"> {
      CFramework(&framework).kill();       
   };
   MENU<bool help_about, "/Help/About"> {
   };
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

OBJECT<CBathSimulator, bathtub_demo>;

void CBathTap::CBathTap(int temperature) {
   this->temperature = temperature;
}/*CBathTap::CBathTap*/

void CBathPlug::CBathPlug(void) {
   this->in = TRUE;
}/*CBathPlug::CBathPlug*/

void CBathTub::new(void) {
   new(&this->hot_tap).CBathTap(40);
   new(&this->cold_tap).CBathTap(10);
   new(&this->plug).CBathPlug();

   this->water_depth = 70;
   this->air_temperature = this->water_temperature = 14;

   CObject(&this->hot_tap).parent_set(CObject(this));
   CObject(&this->cold_tap).parent_set(CObject(this));
   CObject(&this->plug).parent_set(CObject(this));

   new(&this->bath_control).CBathControl();
   CObject(this).child_add(CObject(&this->bath_control));

   new(&this->obj_server).CObjServer(CObjPersistent(this));
}/*CBathTub::new*/

void CBathTub::CBathTub(void) {
}/*CBathTub::CBathTub*/

void CBathTub::~CBathTub(void) {
   delete(&this->plug);
   delete(&this->cold_tap);
   delete(&this->hot_tap);
}/*CBathTub::~CBathTub*/

void CBathControl::new(void) {
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CBathControl,temperature>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CBathControl,depth>, TRUE);
}/*CBathControl::new*/

void CBathControl::CBathControl(void) {
}/*CBathControl::CBathControl*/

void CBathControl::~CBathControl(void) {
}/*CBathControl::~CBathControl*/

void CBathSimulator::iterate(void) {
   if (this->bathtub.hot_tap.on) {
      this->bathtub.water_temperature =
         ((this->bathtub.water_temperature * this->bathtub.water_depth) + this->bathtub.hot_tap.temperature)
         / (this->bathtub.water_depth + 1);
      this->bathtub.water_depth += 1;
   }
   if (this->bathtub.cold_tap.on) {
      this->bathtub.water_temperature =
         ((this->bathtub.water_temperature * this->bathtub.water_depth) + this->bathtub.cold_tap.temperature)
         / (this->bathtub.water_depth + 1);
      this->bathtub.water_depth += 1;
   }
   if (!this->bathtub.plug.in) {
      this->bathtub.water_depth -= 2;
   }

   if (this->bathtub.water_depth >= 100 && !this->bathtub.water_overflow) {
      CObjPersistent(&this->bathtub).attribute_update(ATTRIBUTE<CBathTub,water_overflow>);
      CObjPersistent(&this->bathtub).attribute_set_int(ATTRIBUTE<CBathTub,water_overflow>, 1);
   }
   if (this->bathtub.water_depth < 100 && this->bathtub.water_overflow) {
      CObjPersistent(&this->bathtub).attribute_update(ATTRIBUTE<CBathTub,water_overflow>);
      CObjPersistent(&this->bathtub).attribute_set_int(ATTRIBUTE<CBathTub,water_overflow>, 0);
   }

   CObjPersistent(&this->bathtub).attribute_update(ATTRIBUTE<CBathTub,water_depth>);
   CObjPersistent(&this->bathtub).attribute_set_float(ATTRIBUTE<CBathTub,water_depth>,
                                                      this->bathtub.water_depth);

   CObjPersistent(&this->bathtub).attribute_update(ATTRIBUTE<CBathTub,water_temperature>);
   CObjPersistent(&this->bathtub).attribute_set_float(ATTRIBUTE<CBathTub,water_temperature>,
                                                      this->bathtub.water_temperature);

   CObjPersistent(&this->bathtub).attribute_update_end();
}/*CBathSimulator::iterate*/

int CBathSimulator::main(ARRAY<CString> *args) {
   CFile file;

   new(&this->bathtub).CBathTub();

   new(&this->iterate_timer).CTimer(500, CObject(this), (THREAD_METHOD)&CBathSimulator::iterate, NULL);

   new(&this->layout).CGLayout(0, 0, &this->bathtub.obj_server, CObjPersistent(&this->bathtub));
   CGLayout(&this->layout).render_set(EGLayoutRender.buffered);
   CGCanvas(&this->layout).render_set(EGCanvasRender.full);

   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_freeze);
   CGLayout(&this->layout).load_svg_file("memfile:bathtub.svg", NULL);
   new(&this->window).CGWindow("Bath Tub", CGCanvas(&this->layout), NULL);
   CGWindow(&this->window).disable_close(TRUE);
   CGWindow(&this->window).object_menu_add(CObjPersistent(this), TRUE);

   CGObject(&this->window).show(TRUE);
   CGObject(&this->layout).show(TRUE);
   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_animate);
   CGLayout(&this->layout).animate();    

   new(&file).CFile();
   CIOObject(&file).open(NULL, O_WRONLY | O_CREAT);
   CObjPersistent(&this->bathtub).state_xml_store(CIOObject(&file), NULL);
   delete(&file);

   CFramework(&framework).main();

   return 0;
}/*CBathSimulator::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
