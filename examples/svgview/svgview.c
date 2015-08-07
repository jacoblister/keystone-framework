/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGLayoutSVGView : CGLayout {
 private:
   CObjServer obj_server;

   void new(void);
   void delete(void);
 public:
   ALIAS<"svg:svg">;

   void CGLayoutSVGView(void);
};

class CGWindowSVGView : CGWindow {
 private:
   CGLayout layout_contain;
   CGSplitter splitter;
   CGLayoutSVGView layout;
   CGLayout tree_layout;
   CGTree tree;

   void new(void);
   void delete(void);
 
   void print(void);
   void export(void);
   void fullscreen(bool fullscreen);

   bool notify_request_close(void);
 public:
   void CGWindowSVGView(const char *title);

   bool load(CGLayout *layout, const char *file_name, bool warn_dialog);
   void save(CGLayout *layout, const char *file_name);

   MENU<bool file, "/File">;
   MENU<bool file_open, "/File/Open..."> {
      bool result;
      CString file_name;
      CGDialogFileSelect file_sel;

      new(&file_name).CString(NULL);

      new(&file_sel).CGDialogFileSelect("Open...", EDialogFileSelectMode.open, CGWindow(this));
      result = CGDialogFileSelect(&file_sel).execute(&file_name);
      delete(&file_sel);

      CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_freeze);       
       
      if (result) {
         CGWindowSVGView(this).load(CGLayout(&this->layout), CString(&file_name).string(), TRUE);
      }

      CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_freeze);
      CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_animate);

	  delete(&file_name);
   };
   MENU<bool file_save, "/File/Save As..."> {
      bool result;
      CString file_name;
      CGDialogFileSelect file_sel;

      new(&file_name).CString(NULL);

      new(&file_sel).CGDialogFileSelect("Save...", EDialogFileSelectMode.open, NULL);
      result = CGDialogFileSelect(&file_sel).execute(&file_name);
      delete(&file_sel);

      if (result) {
         CGWindowSVGView(this).save(CGLayout(&this->layout), CString(&file_name).string());
      }

	  delete(&file_name);
   };
   MENU<bool file_print, "/File/Print"> {
      CGWindowSVGView(this).print();
   };
   MENU<bool file_export, "/File/Clipboard Export"> {
      CGWindowSVGView(this).export();
   };
   MENU<bool view_fullscreen, "/View/Fullscreen"> {
      CGWindowSVGView(this).fullscreen(!CGWindow(this)->fullscreen);
   };
   MENU<bool anim_reset, "/Animation/Reset"> {
      CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_freeze);
      CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_animate);
   };
};

class CSVGViewApp : CApplication {
 private:
   CGWindowSVGView window;

   void quit(void);
 public:
   MENU<bool file_quit, "/File/Quit"> {
	  CSVGViewApp(this).quit();
   };
   MENU<bool help_about, "/Help/About"> {
      CGLayout layout_about;
      CGWindowDialog dialog;
	  CGLicenseFill license_fill;

      new(&layout_about).CGLayout(0, 0, NULL, NULL);

	  new(&license_fill).CGLicenseFill();
	  CGLicenseFill(&license_fill).fill_layout(&layout_about);
	  delete(&license_fill);
	  CGCanvas(&layout_about).colour_background_set(GCOL_DIALOG);

      new(&dialog).CGWindowDialog("About SVG Image Viewer", CGCanvas(&layout_about), NULL);
      CGWindowDialog(&dialog).button_add("OK", 0);
      CGWindowDialog(&dialog).execute();
      delete(&dialog);
   };

   void main(ARRAY<CString> *args);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

OBJECT<CSVGViewApp, svgviewapp>;

void CGLayoutSVGView::new(void) {
   class:base.new();

   CGLayout(this)->load_locked = EFileLockMode.none;
   
   new(&this->obj_server).CObjServer(CObjPersistent(this));
}/*CGLayoutSVGView::new*/

void CGLayoutSVGView::CGLayoutSVGView(void) {
   CGLayout(this).CGLayout(0, 0, NULL, NULL);
}/*CGLayoutSVGView::CGLayoutSVGView*/

void CGLayoutSVGView::delete(void) {
   delete(&this->obj_server);
}/*CGLayoutSVGView::delete*/

void CGWindowSVGView::new(void) {
   class:base.new();
}/*CGWindowSVGView::new*/

void CGWindowSVGView::CGWindowSVGView(const char *title) {
   new(&this->layout).CGLayoutSVGView();
   CGLayout(&this->layout).render_set(EGLayoutRender.buffered);
   CGCanvas(&this->layout).render_set(EGCanvasRender.full);

   new(&this->tree_layout).CGLayout(0, 0, NULL, NULL);
   new(&this->tree).CGTree(&this->layout.obj_server, CObjPersistent(&this->layout), 0, 0, 0, 0);
   CObject(&this->tree_layout).child_add(CObject(&this->tree));
   CGObject(&this->tree_layout).show(TRUE);

   new(&this->splitter).CGSplitter(0, 0, 0, 0);
   CObject(&this->splitter).child_add(CObject(&this->tree_layout));
   CObject(&this->splitter).child_add(CObject(&this->layout));

   new(&this->layout_contain).CGLayout(0, 0, NULL, NULL);
   CObject(&this->layout_contain).child_add(CObject(&this->splitter));
   CGObject(&this->splitter).show(TRUE); /*>>>shouldn't need this*/

   CGWindow(this).CGWindow(title, CGCanvas(&this->layout_contain), NULL);
   CGWindow(this).object_menu_add(CObjPersistent(this), FALSE);
}/*CGWindowSVGView::CGWindowSVGView*/

void CGWindowSVGView::delete(void) {
   class:base.delete();
}/*CGWindowSVGView::delete*/

void CGWindowSVGView::fullscreen(bool fullscreen) {
   CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGWindow,fullscreen>, fullscreen);       
}/*CGWindowSVGView::fullscreen*/

void CGWindowSVGView::print(void) {
   CGCanvasPrint *canvas = new.CGCanvasPrint(CGCanvas(&this->layout), TRUE, CGWindow(this));
   
CObjPersistent(canvas).attribute_set_int(ATTRIBUTE<CGCanvasPrint,orientation>, EGCanvasPrintOrientation.landscape);
   
   if (CGCanvasPrint(canvas).NATIVE_output_choose_printer()) {
      CGCanvasPrint(canvas).print_document();
   }
   
   delete(canvas);
}/*CGWindowSVGView::print*/

void CGWindowSVGView::export(void) {
   CGCanvasPrint *canvas = new.CGCanvasPrint(CGCanvas(&this->layout), TRUE, CGWindow(this));
   
   if (CGCanvasPrint(canvas).NATIVE_output_clipboard()) {
      CGCanvasPrint(canvas).print_document();
   }
   
   delete(canvas);
}/*CGWindowSVGView::export*/

bool CGWindowSVGView::load(CGLayout *layout, const char *file_name, bool warn_dialog) {
   CString error_result;
   CGDialogMessage message;
   bool result = TRUE;

   new(&error_result).CString(NULL);

   CObjServer(&CGLayoutSVGView(&this->layout)->obj_server).disable(TRUE);

   if (!CGLayout(&this->layout).load_svg_file(file_name, &error_result)) {
      new(&message).CGDialogMessage("Load Error", CString(&error_result).string(), NULL);
      CGWindowDialog(&message).execute();
      delete(&message);
	  result = FALSE;
   }
   CGObject(&this->layout).show(TRUE);   

   CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGWindow,title>, file_name);
   CObjServer(&CGLayoutSVGView(&this->layout)->obj_server).disable(FALSE);
   CObjPersistent(&this->layout).notify_all_update(FALSE);

   delete(&error_result);
   
   return result;
}/*CGWindowSVGView::load*/

void CGWindowSVGView::save(CGLayout *layout, const char *file_name) {
   CFile file;

   new(&file).CFile();
   CIOObject(&file).open(file_name, O_WRONLY | O_CREAT | O_TRUNC);
   CObjPersistent(layout).state_xml_store(CIOObject(&file), "svg");
   CIOObject(&file).close();
   delete(&file);
}/*CGWindowSVGView::load*/

bool CGWindowSVGView::notify_request_close(void) {
   CSVGViewApp(&svgviewapp).quit();
   return FALSE;
};

int CSVGViewApp::main(ARRAY<CString> *args) {
   new(&this->window).CGWindowSVGView("SVG Image Viewer");

   if (ARRAY(args).count() >= 2) {
      CGLayout(&this->window.layout).load_svg_file(CString(&ARRAY(args).data()[1]).string(), NULL);
	  CGObject(&this->window.layout).show(TRUE);   
   }
   CGWindow(&this->window).object_menu_add(CObjPersistent(this), TRUE);

   CGObject(&this->window).show(TRUE);
//   CGWindow(&this->window).maximize(TRUE);
   CObjPersistent(&this->window).attribute_set_int(ATTRIBUTE<CGWindow,maximized>, TRUE);   

   CFramework(&framework).main();

   return 0;
}/*CSVGViewApp::main*/

void CSVGViewApp::quit(void) {
   CFramework(&framework).kill();
}/*CSVGViewApp::quit*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
