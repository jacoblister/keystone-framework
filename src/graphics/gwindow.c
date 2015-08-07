/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/
/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "../framework_base.c"
#include "../cstring.c"
#include "../file.c"
#include "gobject.c"
#include "glayout.c"
#include "gxulelement.c"
#include "gprimitive.c"
#include "gprimcontainer.c"
#include "gpalette.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGCanvas;

class CGMenu : CGObject {
 private:
//#if OS_Linux
   CGMenuItem *selected;
//#endif
   void notify_all_update(bool changing);

   void NATIVE_allocate(void);
   void NATIVE_release(void);
   void NATIVE_item_allocate(CGMenuItem *parent);
   void NATIVE_item_update(CGMenuItem *item);

   CGMenuItem *child_find(CObject *menu, const char *label);
   void menu_item_select(CGMenuItem *item);
 public:
   void CGMenu(void);

   void menu_item_add(CObjPersistent *object, const TAttribute *attribute);
   CGMenuItem *menu_item_find(const char *path);
};

class CGMenuPopup : CGMenu {
//#if OS_Linux
 private:
   bool selection_done;
//#endif
 public:
   void CGMenuPopup(void);
   CGMenuItem *execute(CGCanvas *canvas);
};

/*>>>consider attributes for modal/topmost/disable close etc*/
class CGWindow : CGContainer {
 private:
   CGWindow *parent;
   CGObject *input_focus;
   bool nodecoration;
   bool disable_close;
   bool topmost;
   void *native_data[2];

   void new(void);
   void delete(void);
   void NATIVE_new(void);
   void NATIVE_delete(void);
   void NATIVE_show(bool show);
   void NATIVE_title_set(const char *title);
   void NATIVE_extent_set(TRect *extent);
   void NATIVE_maximize(bool maximize);
   void NATIVE_fullscreen(bool fullscreen);
   void NATIVE_opacity(double opacity);
   void NATIVE_show_help(CString *help_topic);
   void NATIVE_monitor_area(TRect *result);

   bool modal;
//#if OS_Linux
   void *vbox;
//#endif
   bool event(CEvent *event);
 public:
   ATTRIBUTE<CString title> {
      CString(&this->title).set(CString(data).string());
      CGWindow(this).NATIVE_title_set(CString(&this->title).string());
   };
   ELEMENT:OBJECT<CGMenu menu>;
   ATTRIBUTE<TRect placement>;
   ATTRIBUTE<bool maximized> {
      this->maximized = *data;
      CGWindow(this).NATIVE_maximize(this->maximized);
   };
   ATTRIBUTE<bool fullscreen> {
      this->fullscreen = *data;
      CGWindow(this).NATIVE_fullscreen(this->fullscreen);
   };
   void CGWindow(const char *title, CGCanvas *canvas, CGWindow *parent);

   void topmost(bool topmost, bool stick);
   void disable_close(bool disable);
   void disable_maximize(bool disable);
   void disable_minimize(bool disable);
   void modal(bool modal);

   virtual bool load(CGLayout *layout, const char *file_name, bool warn_dialog);

   void size_set(int width, int height);
   void position_set(int x, int y);
   void position_size_set(int x, int y, int width, int height);

   void object_menu_add(CObjPersistent *object, bool update);
   void object_menu_remove(CObjPersistent *object);

   virtual bool notify_request_close(void);
   virtual void show(bool show);
};

class CGLayoutDialog : CGLayout {
 private:
   /*STATE state_animate()*/
      STATE state_animate_dialog(CEvent *event);
 public:
   ALIAS<"svg:svg">;
   void CGLayoutDialog(void);
};

class CGWindowDialog : CGWindow {
 private:
   CGLayoutDialog layout;
   CGLayout layout_buttons;
   CGVBox vbox;
   CGVBox hbox;
   bool wait;

   void new(void);
   void delete(void);
   CGButton button[5];

   virtual bool notify_request_close(void);
 public:
   void CGWindowDialog(const char *title, CGCanvas *canvas, CGWindow *parent);
   void button_add(const char *label, int id);
   int execute(void);
   int execute_wait(void);
   int wait(bool wait);
   int close(void);

   ATTRIBUTE<int button_press> {
      this->button_press = *data;
      if (this->wait) {
         CFramework(&framework).NATIVE_modal_exit(CObject(this));
      }
   };
};

class CGDialogMessage : CGWindowDialog {
 private:
   CGLayout layout;
   CGTextLabel text;
   void delete(void);
 public:
   void CGDialogMessage(const char *title, const char *message, CGWindow *parent);
};

/* Common Dialogs */

class CGChooseColour : CObjPersistent {
 private:
   bool in_update;
   TObjectPtr object;

   void new(void);
   void delete(void);

   static inline void colour_change(bool is_default);
   static inline void colour_set(TGColour colour);
 public:
   ATTRIBUTE<ARRAY<TGColour> palette>;
   ATTRIBUTE<ARRAY<TGColour> paletteB>;
   ATTRIBUTE<TGColour colour> {
      if (data) {
         this->colour = *data;
         CGChooseColour(this).colour_change(FALSE);
      }
      else {
         CGChooseColour(this).colour_change(TRUE);
      }
   };
   ATTRIBUTE<EGColourNamed colour_name> {
      if (data) {
         this->colour_name = *data;
      }

      CGChooseColour(this).colour_set(GCOL_NAMED_VALUE(this->colour_name));
   };
   ATTRIBUTE<bool colour_default> {
      CGChooseColour(this).colour_change(TRUE);
   };

   void CGChooseColour(TObjectPtr *object);
};

static inline void CGChooseColour::colour_change(bool is_default) {
   if (this->in_update) {
      return;
   }
   this->in_update = TRUE;

   if (is_default) {
      CObjPersistent(this->object.object).attribute_default_set(this->object.attr.attribute, TRUE);
   }
   else {
      CObjPersistent(this->object.object).attribute_default_set(this->object.attr.attribute, FALSE);
      CObjPersistent(this->object.object).attribute_set(this->object.attr.attribute,
                                                        this->object.attr.element,
                                                        this->object.attr.index,
                                                        &ATTRIBUTE:type<TGColour>, (void *)&this->colour);
   }
   /*>>>cheating, out of order */
   CObjPersistent(this->object.object).attribute_update(this->object.attr.attribute);
   CObjPersistent(this->object.object).attribute_update_end();

   /* Read back the new colour */
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGChooseColour,colour>);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGChooseColour,colour>, is_default);
   CObjPersistent(this->object.object).attribute_get(this->object.attr.attribute,
                                                      this->object.attr.element,
                                                      this->object.attr.index,
                                                      &ATTRIBUTE:type<TGColour>, (void *)&this->colour);
   CObjPersistent(this).attribute_update_end();
   this->in_update = FALSE;
};

static inline void CGChooseColour::colour_set(TGColour colour) {
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGChooseColour,colour>, FALSE);
   CObjPersistent(this).attribute_set(ATTRIBUTE<CGChooseColour,colour>, -1, ATTR_INDEX_VALUE,
                                      &ATTRIBUTE:type<TGColour>, &colour);
//   /*>>>cheating, out of order */
//   CObjPersistent(this).attribute_update(ATTRIBUTE<CGChooseColour,colour>);
//   CObjPersistent(this).attribute_update_end();
};

class CGLayoutChooseColour : CGLayoutTab {
 private:
   CObjServer obj_server;
   CGChooseColour colour;

   void delete(void);
 public:
   void CGLayoutChooseColour(const char *label, TObjectPtr *object, CGPalette *palette, CGPalette *paletteB);
   static inline TGColour colour_get(void);
};

static inline TGColour CGLayoutChooseColour::colour_get(void) {
   return this->colour.colour;
}/*CGLayoutChooseColour::colour_get*/

class CGDialogChooseColour : CGWindowDialog {
 private:
   bool native_dialog;
   CGLayoutChooseColour layout;
   TObjectPtr *object;
   CGPalette *palette;
   CGPalette *paletteB;

   int execute(void);
 public:
   void CGDialogChooseColour(const char *title, TObjectPtr *object, CGPalette *palette, CGPalette *paletteB, CGWindow *parent,
                             bool native_dialog);
   static inline TGColour colour_get(void);
};

static inline TGColour CGDialogChooseColour::colour_get(void) {
   return CGLayoutChooseColour(&this->layout).colour_get();
}/*CGDialogChooseColour::colour_get*/

class CGDialogChooseDate : CGWindowDialog {
 private:
   void delete(void);
   bool immediate;
   
   CObjServer obj_server; 
   TObjectPtr object;
   CGLayout layout;

   int execute(void);
   static inline void set_date(TUNIXTime *date);
   static inline void set_midnight();
 public:
   void CGDialogChooseDate(const char *title, bool immedate, TObjectPtr *object, CGWindow *parent);
   
   ATTRIBUTE<bool set_midnight> {
      CGDialogChooseDate(this).set_midnight();
   };
   ATTRIBUTE<TUNIXTime date> {
      if (data) {
         this->date = *data;
      }
      CGDialogChooseDate(this).set_date(&this->date);
   };
};

static inline void CGDialogChooseDate::set_date(TUNIXTime *date) {
   if (this->immediate) {
      CObjPersistent(this->object.object).attribute_set(this->object.attr.attribute,
                                                        this->object.attr.element,
                                                        this->object.attr.index,
                                                        &ATTRIBUTE:type<TUNIXTime>, (void *)date);
      CObjPersistent(this->object.object).attribute_update(this->object.attr.attribute);
      CObjPersistent(this->object.object).attribute_update_end();
   }
}/*CGDialogChooseDate::set_date*/

static inline void CGDialogChooseDate::set_midnight(void) {
   CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGDialogChooseDate,date>, -1, 3, 0);
   CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGDialogChooseDate,date>, -1, 4, 0);   
   CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGDialogChooseDate,date>, -1, 5, 0);   
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGDialogChooseDate,date>);
   CObjPersistent(this).attribute_update_end();
}/*CGDialogChooseDate::set_midnight*/

ENUM:typedef<EDialogFileSelectMode> {
   {open}, {save}
};

class CGDialogFileSelect : CGWindow {
 private:
   EDialogFileSelectMode mode;
   const char *title;
   CGWindow *parent;
   int exec_result;

   void new(void);
   void delete(void);
 public:
   void CGDialogFileSelect(const char *title, EDialogFileSelectMode mode, CGWindow *parent);
   bool execute(CString *filename);

   ELEMENT:OBJECT<CGListMenuPopup filter>;
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

/*>>>not the best place for these*/
void CGLayout::key_gselection_set(CGObject *object) {
   CGWindow *window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
   CObjPersistent *cur_object = NULL;
   TRect extent;

   if (!window) {
      return;
   }

   if (CSelection(&this->key_gselection.selection).count()) {
      cur_object = ARRAY(CSelection(&this->key_gselection.selection).selection()).data()[0].object;
   }

   if (window->input_focus) {
      CSelection(&CGLayout(window->input_focus)->key_gselection.selection).clear();
   }

   CSelection(&this->key_gselection.selection).clear();

   /*>>>redrawing is redundant, have keyboard selection redraw go through here as well */
   if (cur_object && !CObjClass_is_derived(&class(CGXULElement), CObject(cur_object).obj_class())) {
      extent = CGObject(cur_object)->extent;
      CGCoordSpace(&CGCanvas(this)->coord_space).reset();
      CGCanvas(this).transform_viewbox();
      CGCanvas(this).point_array_utov(2, extent.point);
      CGCoordSpace(&CGCanvas(this)->coord_space).extent_extend(&extent, 3);
      CGCanvas(this).queue_draw(&extent);
   }

   window->input_focus = NULL;
   if (object) {
      window->input_focus = CGObject(this);

      CSelection(&this->key_gselection.selection).add(CObjPersistent(object), NULL);

      if (!CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
         extent = CGObject(object)->extent;
         CGCoordSpace(&CGCanvas(this)->coord_space).reset();
         CGCanvas(this).transform_viewbox();
         CGCanvas(this).point_array_utov(2, extent.point);
         CGCoordSpace(&CGCanvas(this)->coord_space).extent_extend(&extent, 3);
         CGCanvas(this).queue_draw(&extent);
      }
   }
}/*CGLayout::key_gselection_set*/

void CGLayout::key_gselection_refresh(void) {
   TObjectPtr *object_selection;

   if (CSelection(&this->key_gselection.selection).count() != 1) {
      return;
   }
   object_selection = &ARRAY(CSelection(&this->key_gselection.selection).selection()).data()[0];

   if (CObjClass_is_derived(&class(CGXULElement), CObject(object_selection->object).obj_class())) {
      CGXULElement(object_selection->object).NATIVE_focus_in();
   }
}/*CGLayout::key_gselection_refresh*/

void CXLink::link_choose(void) {
   CString filename;
   CGDialogFileSelect file_sel;
   bool result;

   new(&filename).CString(NULL);
   CXLink(this).link_get_file(&filename);

   new(&file_sel).CGDialogFileSelect("Choose File...", EDialogFileSelectMode.open, NULL);
   result = CGDialogFileSelect(&file_sel).execute(&filename);
   delete(&file_sel);
   if (result) {
      CXLink(this).link_set(CString(&filename).string());
   }

   delete(&filename);
}/*CXLink::link_choose*/

void CGXULElement::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   /* clear layout selection when object is unlinked */
   /*>>>should just remove this object from selection, not clear whole selection, and
        should do this for CGPrimative objects too */
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));

   switch (linkage) {
   case EObjectLinkage.ParentRelease:
      if (layout) {
         CGLayout(layout).animation_remove(CObjPersistent(this));
         CGLayout(layout).key_gselection_set(NULL);
      }
      break;
   case EObjectLinkage.ParentSet:
      if (layout) {
         CGLayout(layout).animation_add(CObjPersistent(this));
      }
      break;
   default:
      break;
   }
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);
}/*CGXULElement::notify_object_linkage*/

void CGXULElement::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGColour stroke, fill;
   CString classname;

   class:base.draw(canvas, extent, mode);

   CGXULElement(this).NATIVE_draw(canvas, extent, mode);

   if (CGObject(this)->native_object)
      return;

   switch (mode) {
   case EGObjectDrawMode.Draw:
      fill   = GCOL_BLACK;
      stroke = (GCOL_TYPE_NAMED | EGColourNamed.palevioletred);
      CGCanvas(canvas).stroke_style_set(0, NULL, 0, 0, 0, 0);
      break;
   case EGObjectDrawMode.Select:
      fill   = GCOL_NONE;
      stroke = GCOL_WHITE;
      break;
   default:
      return;
   }
   CGCanvas(canvas).draw_rectangle(stroke, FALSE, this->x, this->y,
                                   this->x + this->width - 1, this->y + this->height - 1);
   CGCanvas(canvas).draw_line(stroke, this->x, this->y,
                              this->x + this->width - 1, this->y + this->height - 1);
   CGCanvas(canvas).draw_line(stroke, this->x, this->y + this->height - 1,
                              this->x + this->width - 1, this->y);

   new(&classname).CString(NULL);
   CString(&classname).printf("Arial");
   CGCanvas(canvas).font_set((CGFontFamily *)&classname, 14,
                             EGFontStyle.normal, EGFontWeight.bold, EGTextDecoration.none);
   CString(&classname).printf("<%s/>", CObjClass_alias_short(CObject(this).obj_class()));
   CGCanvas(canvas).NATIVE_text_align_set(EGTextAnchor.middle, EGTextBaseline.middle);
   CGCanvas(canvas).draw_text(GCOL_RGB(255,255,255), this->x + (this->width / 2) + 1, this->y + (this->height / 2) + 1,
                              CString(&classname).string());
   CGCanvas(canvas).draw_text(fill, this->x + (this->width / 2), this->y + (this->height / 2),
                              CString(&classname).string());
   delete(&classname);
}/*CGXULElement::draw*/

void CGListItem::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);

   switch (linkage) {
   case EObjectLinkage.ParentSet:
      CObjPersistent(this).attribute_refresh(FALSE);
      break;
   default:
      break;
   }
}/*CGListItem::notify_object_linkage*/

void CGListItem::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGMenu *menu;

   /*>>>quick hack*/
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);

   if (!changing) {
      menu = CGMenu(CObject(this).parent_find(&class(CGMenu)));
      if (menu) {
         CGMenu(menu).NATIVE_item_update(CGMenuItem(this));
      }
   }
}/*CGListItem::menu_update*/

bool CGLink::event(CEvent *event) {
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   CString file_name;
//   CGDialogMessage message;

   CGLayout *link_layout;
   CGWindow *link_window;

   if (CObject(event).obj_class() == &class(CEventPointer)) {
      switch (CEventPointer(event)->type) {
      case EEventPointer.Move:
         CGCanvas(layout).pointer_cursor_set(EGPointerCursor.pointer);
         break;
      case EEventPointer.LeftDown:
         new(&file_name).CString(NULL);
         CXLink(&CGUse(this)->link).link_get(&file_name);

         if (this->show == EXLinkShow.new) {
            link_layout = new.CGLayout(0, 0, CObjClient(&layout->client)->server,
            ARRAY(&CObjClient(&layout->client)->object_root).data()[0]);

            CGLayout(link_layout).render_set(EGLayoutRender.buffered);

            CFsm(&CGLayout(link_layout)->fsm).transition((STATE)&CGLayout::state_freeze);
            //CGLayout(link_layout).load_svg_file(CString(&file_name).string(), NULL);
            link_window = new.CGWindow("Link Window", CGCanvas(link_layout), NULL);
            CGWindow(link_window).load(link_layout, CString(&file_name).string(), TRUE);

            CGObject(link_window).show(TRUE);
            CGObject(link_layout).show(TRUE);
            CFsm(&CGLayout(link_layout)->fsm).transition((STATE)&CGLayout::state_animate);
         }
         else {
            link_window = CGWindow(CObject(layout).parent_find(&class(CGWindow)));
            CGWindow(link_window).load(layout, CString(&file_name).string(), TRUE);
         }
         delete(&file_name);

         break;
      default:
         break;
      }
      return TRUE;
   }

   return FALSE;
}/*CGLink::event*/

void CGMenu::CGMenu(void) {
}/*CGMenu::CGMenu*/

void CGMenu::notify_all_update(bool changing) {
   if (!changing) {
      /* recreate the menu */
      CGMenu(this).NATIVE_item_allocate(NULL);
   }
}/*CGMenu::notify_all_update*/

CGMenuItem *CGMenu::child_find(CObject *menu, const char *label) {
   CGMenuItem *child;

   child = CGMenuItem(CObject(menu).child_first());
   while (child) {
      if (CString(&CGListItem(child)->label).strcmp(label) == 0) {
         return child;
      }
      child = CGMenuItem(CObject(menu).child_next(CObject(child)));
   }
   /*>>>search for child*/
   return NULL;
}/*CGMenu::child_find*/

void CGMenu::menu_item_select(CGMenuItem *item) {
   if (item->menu_object && item->menu_attribute && item->menu_attribute->method) {
      (*item->menu_attribute->method)(item->menu_object, FALSE, -1);
   }
}/*CGMenu::menu_item_select*/

CGMenuItem *CGMenu::menu_item_find(const char *path) {
   CString temp, section;
   CObject *menu_current = CObject(this);
   CGMenuItem *item_current = NULL;

   new(&temp).CString(path);
   new(&section).CString(NULL);

   CString(&temp).tokenise(&section, "/", TRUE);
   while (CString(&temp).tokenise(&section, "/", TRUE)) {
      item_current = CGMenu(this).child_find(menu_current, CString(&section).string());
      menu_current = CObject(item_current);
   }

   delete(&section);
   delete(&temp);

   return item_current;
}/*CGMenu::menu_item_find*/

void CGMenu::menu_item_add(CObjPersistent *object, const TAttribute *attribute) {
   CString temp, section;
   CObject *menu_current = CObject(this);
   CGMenuItem *item_current = NULL;

   new(&temp).CString(attribute->path);
   new(&section).CString(NULL);

   CString(&temp).tokenise(&section, "/", TRUE);
   while (CString(&temp).tokenise(&section, "/", TRUE)) {
      item_current = CGMenu(this).child_find(menu_current, CString(&section).string());
      if (!item_current) {
         if (CString(&section).string()[0] == '-') {
            item_current = (CGMenuItem *)new.CGMenuSeparator();
         }
         else {
            item_current = new.CGMenuItem(CString(&section).string(), NULL, 0, FALSE, FALSE);
         }
         CGMenuItem(item_current).attribute_set(object, attribute);
         CObject(menu_current).child_add(CObject(item_current));
      }
      menu_current = CObject(item_current);
   }
   if (item_current && attribute->delim) {
      CObjPersistent(item_current).attribute_default_set(ATTRIBUTE<CGMenuItem,accelerator>, FALSE);
      CObjPersistent(item_current).attribute_set_text(ATTRIBUTE<CGMenuItem,accelerator>, attribute->delim);

      /*>>>default acceltext should do this automatically*/
      CObjPersistent(item_current).attribute_default_set(ATTRIBUTE<CGMenuItem,acceltext>, FALSE);
      CObjPersistent(item_current).attribute_set_text(ATTRIBUTE<CGMenuItem,acceltext>, attribute->delim);
   }

   delete(&section);
   delete(&temp);
}/*CGMenu::menu_item_add*/

void CGMenuPopup::CGMenuPopup(void) {
   CGMenu(this).CGMenu();
}/*CGMenuPopup::CGMenuPopup*/

void CGWindow::new(void) {
   /*>>>quick hack*/
   _virtual_CGXULElement_new(CGXULElement(this));

   new(&this->title).CString(NULL);
   new(&this->menu).CGMenu();
   CGObject(&this->menu)->visibility = EGVisibility.visible;
   CObject(&this->menu).parent_set(CObject(this));

   CGObject(this)->visibility = EGVisibility.visible;

   ARRAY(&framework.window).item_add(this);
}/*CGWindow::new*/

void CGWindow::CGWindow(const char *title, CGCanvas *canvas, CGWindow *parent) {
   CGXULElement(this).CGXULElement(0, 0, 0, 0);
   CString(&this->title).set(title);
//   this->canvas = canvas;
   this->input_focus = CGObject(canvas);
   this->parent = parent;

   /*>>>hack, no decoration if no title */
   if (!title) {
      this->nodecoration = TRUE;
   }

   CObject(this).child_add(CObject(canvas));
   CGWindow(this).NATIVE_new();
   CGWindow(this).NATIVE_title_set(title);
   if (parent) {
      /* For now, treat any windows with parent as dialogs */
      CGWindow(this).disable_maximize(TRUE);
      CGWindow(this).disable_minimize(TRUE);      
   }

   /*>>>kludge*/
   CObject(this).child_remove(CObject(canvas));
   CObject(this).child_add(CObject(canvas));
}/*CGWindow::CGWindow*/

void CGWindow::delete(void) {
//   this->canvas = NULL;
   delete(&this->menu);
   delete(&this->title);
//   class:base.delete();

   CGWindow(this).NATIVE_delete();

   ARRAY(&framework.window).item_remove(this);
}/*CGWindow::delete*/

bool CGWindow::notify_request_close(void) {
   return TRUE;
}/*CGWindow::notify_request_close*/

bool CGWindow::load(CGLayout *layout, const char *file_name, bool warn_dialog) {
   /*>>>provide default content load*/
   return TRUE;
}/*CGWindow::load*/

void CGWindow::show(bool show) {
   CGWindow(this).NATIVE_show(show);
}/*CGWindow::show*/

bool CGWindow::event(CEvent *event) {
   CObject *child;
   const TAttribute *attribute;
   CString helpFile;
   CGLayout *layout;

   if (CObject(event).obj_class() == &class(CEventKey)) {
      if (CEventKey(event)->type == EEventKeyType.down) {
         child = CObject(&this->menu).child_tree_first();
         while (child) {
            if (CObject(child).obj_class() == &class(CGMenuItem))  {
                if ((CGMenuItem(child)->accelerator.key != EEventKey.None) && !CGListItem(child)->disabled) {
                   if (CEventKey(event)->key == CGMenuItem(child)->accelerator.key &&
                       toupper(CEventKey(event)->value) == toupper(CGMenuItem(child)->accelerator.value) &&
                       CEventKey(event)->modifier == CGMenuItem(child)->accelerator.modifier) {
                       CGMenu(&this->menu).menu_item_select(CGMenuItem(child));
                       return FALSE;//TRUE;
                   }
                }
            }
            child = CObject(&this->menu).child_tree_next(child);
         }
#if 1
         /* Special handling for F1 */
         if (CEventKey(event)->key == EEventKey.Function && CEventKey(event)->value == 1) {
            layout = CGLayout(CObject(this).child_first());
            layout = CGLayout(layout).find_visible_child();

            if (CString(&CGObject(layout)->id).length()) {
//             CGWindow(this).NATIVE_show_help(&CGWindow(this)->input_focus->id);
               CGWindow(this).NATIVE_show_help(&CGObject(layout)->id);
            }
            else {
               attribute = CObjPersistent(framework.application).attribute_find("defaultHelp");
               new(&helpFile).CString(NULL);
               CObjPersistent(framework.application).attribute_get_string(attribute, &helpFile);
               CGWindow(this).NATIVE_show_help(&helpFile);
               delete(&helpFile);
            }
            return FALSE;
         }
#endif

      }

      if (!this->input_focus) {
          WARN("CGWindow::event - no focus layout");
      }
      else {
//>>>         return CGObject(this->input_focus).event(CEvent(event));
         CGObject(this->input_focus).event(CEvent(event));
      }
   }
   return FALSE;
}/*CGWindow::event*/

void CGWindow::object_menu_add(CObjPersistent *object, bool update) {
   const TAttribute *attribute;
   ARRAY<const TAttribute *> attr_list;
   int i;

   ARRAY(&attr_list).new();
   CObjPersistent(object).attribute_list(&attr_list, FALSE, FALSE, TRUE);

   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      attribute = ARRAY(&attr_list).data()[i];
      CGMenu(&this->menu).menu_item_add(object, attribute);
   }

   ARRAY(&attr_list).delete();

   /*>>>kludge*/
   if (update) {
      CObjPersistent(&this->menu).notify_all_update(FALSE);
   }
}/*CGWindow::object_menu_add*/

void CGLayoutDialog::CGLayoutDialog(void) {
    CGLayout(this).CGLayout(0, 0, NULL, NULL);
}/*CGLayoutDialog::CGLayoutDialog*/

STATE CGLayoutDialog::state_animate_dialog(CEvent *event) {
    CGWindowDialog *window = CGWindowDialog(CObject(this).parent());
// const TAttribute *attribute;
// CString helpFile;
//   if (CObject(event).obj_class() == &class(CEventState)) {
//       /* passthrough state change events >>>hack */
//       CGLayout(this).state_animate(event);
//   }
   if (CObject(event).obj_class() == &class(CEventKey) && CEventKey(event)->type == EEventKeyType.down) {
      switch (CEventKey(event)->key) {
      case EEventKey.Enter:
         if (!CGWindow(window)->disable_close) {
            CObjPersistent(window).attribute_set_int(ATTRIBUTE<CGWindowDialog,button_press>, 1);
         }
         break;
       case EEventKey.Escape:
         if (!CGWindow(window)->disable_close) {
            CObjPersistent(window).attribute_set_int(ATTRIBUTE<CGWindowDialog,button_press>, 0);
         }
         break;
       default:
         break;
      }
   }
   return (STATE)&CGLayout::state_animate;
}/*CGLayoutDialog::state_animate_dialog*/

void CGWindowDialog::new(void) {
   class:base.new();
}/*CGWindowDialog::new*/

void CGWindowDialog::delete(void) {
   class:base.delete();
}/*CGWindowDialog::delete*/

void CGWindowDialog::CGWindowDialog(const char *title, CGCanvas *canvas, CGWindow *parent) {
   int width, height;

//   this->canvas = canvas;

   new(&this->layout).CGLayoutDialog();
   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayoutDialog::state_animate_dialog);

   width  = CObjPersistent(canvas).attribute_get_int(ATTRIBUTE<CGXULElement,min_width>);
   height = CObjPersistent(canvas).attribute_get_int(ATTRIBUTE<CGXULElement,min_height>) + 22;
   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_width>, width);
//   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,width>, FALSE);
//   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,width>, width);
   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_height>, height);
//   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);
//   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,height>, height);

   CGLayout(&this->layout).render_set(EGLayoutRender.none);
   CGCanvas(&this->layout).colour_background_set(GCOL_DIALOG);
   new(&this->vbox).CGVBox();
   new(&this->layout_buttons).CGLayout(0, 30, NULL, NULL);
   CGCanvas(&this->layout_buttons).colour_background_set(GCOL_DIALOG);
   CGLayout(&this->layout_buttons).render_set(EGLayoutRender.none);
   CObjPersistent(&this->layout_buttons).attribute_set_int(ATTRIBUTE<CGXULElement,height>, 22);
   CObjPersistent(&this->layout_buttons).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);

   new(&this->hbox).CGHBox();
   CObject(&this->layout_buttons).child_add(CObject(&this->hbox));

   CObject(&this->vbox).child_add(CObject(canvas));
   CObject(&this->vbox).child_add(CObject(&this->layout_buttons));
   CObject(&this->layout).child_add(CObject(&this->vbox));

   CGWindow(this).CGWindow(title, CGCanvas(&this->layout), parent);
}/*CGWindowDialog::CGWindowDialog*/

bool CGWindowDialog::notify_request_close(void) {
   /*same as pressing 'cancel' */
   CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGWindowDialog,button_press>, 0);
   return FALSE;
}/*CGWindowDialog::notify_request_close*/

void CGWindowDialog::button_add(const char *label, int id) {
   int index = CObject(&this->hbox).child_count();
   CString temp;

   new(&temp).CString(NULL);
   CString(&temp).printf("%d", id);

   new(&this->button[index]).CGButton(0, 0, 0, 0, label);
   CObject(&this->hbox).child_add(CObject(&this->button[index]));
   CXPath(&CGXULElement(&this->button[index])->binding).set(CObjPersistent(this), ATTRIBUTE<CGWindowDialog,button_press>, -1);

/*>>>yuck!!! get rid of this gobject id*/
   CObjPersistent(CGXULElement(&this->button[index])).attribute_set_string(ATTRIBUTE<CGObject,id>, &temp);

   CXPath(&CGXULElement(&this->button[index])->binding)->assignment =
   CString(&CGObject(&this->button[index])->id).string();

   delete(&temp);
}/*CGWindowDialog::button_add*/

int CGWindowDialog::execute(void) {
   int result;

   CGWindowDialog(this).execute_wait();
   result = CGWindowDialog(this).wait(TRUE);
   CGWindowDialog(this).close();

   return result;
}/*CGWindowDialog::execute*/

int CGWindowDialog::execute_wait(void) {
   CObject *focus_element;
   CGLayout *layout = CGLayout(&this->layout).find_visible_child();

   focus_element = CObject(layout).child_tree_first();
   while (focus_element) {
      if (!CObjClass_is_derived(&class(CGLayout), CObject(focus_element).obj_class()) &&
         (CObjClass_is_derived(&class(CGObject), CObject(focus_element).obj_class()) && CGObject(focus_element).keyboard_input())) {
         break;
      }
      focus_element = CObject(layout).child_tree_next(focus_element);
   }

   this->button_press = -1;
   CGObject(&this->layout).show(TRUE);
   if (focus_element) {
      CGXULElement(focus_element).NATIVE_focus_in();
   }
//   CGXULElement(&this->button[0]).NATIVE_focus_in();

//   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayoutDialog::state_animate_dialog);
   CGLayout(&this->layout).animate();
   if (CGObject(this)->visibility) {
      CGObject(this).show(TRUE);
   }

   CGWindow(this).modal(TRUE);
//   CGWindow(this).topmost(TRUE);
   return 0;
}/*CGWindowDialog::execute_wait*/

int CGWindowDialog::wait(bool wait) {
   this->wait = wait;
   if (wait) {
      CGLayout(&this->layout).animate();
      CFramework(&framework).NATIVE_modal_wait(CObject(this));
   }
   else {
      CFramework(&framework).NATIVE_modal_exit(CObject(this));
   }

   return this->button_press;
}/*CGWindowDialog::wait*/

int CGWindowDialog::close(void) {
   CGWindow(this).modal(FALSE);
   CGWindow(this).show(FALSE);
   CGLayout(&this->layout).show_children(FALSE);
   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_freeze);

   return 0;
}/*CGWindowDialog::close*/

void CGDialogMessage::delete(void) {
   class:base.delete();
}/*CGDialogMessage::delete*/

void CGDialogMessage::CGDialogMessage(const char *title, const char *message, CGWindow *parent) {
   new(&this->layout).CGLayout(0, 0, NULL, NULL);
   CGCanvas(&this->layout).colour_background_set(GCOL_DIALOG);

   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>,  FALSE);
   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_width>,  320);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_height>, 100);
   new(&this->text).CGTextLabel(10, 40, 300, 22, message);
   CObject(&this->layout).child_add(CObject(&this->text));

   CGWindowDialog(this).CGWindowDialog(title, CGCanvas(&this->layout), parent);
   CGWindowDialog(this).button_add("OK", 1);
}/*CGDialogMessage::CGDialogMessage*/

void CGChooseColour::new(void) {
   ARRAY(&this->palette).new();
   ARRAY(&this->paletteB).new();
}/*CGChooseColour::new*/

void CGChooseColour::CGChooseColour(TObjectPtr *object) {
   bool is_default;
   this->object = *object;

   is_default = CObjPersistent(this->object.object).attribute_default_get(this->object.attr.attribute);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGChooseColour,colour>, is_default);
   CObjPersistent(this->object.object).attribute_get(this->object.attr.attribute,
                                                      this->object.attr.element,
                                                      this->object.attr.index,
                                                      &ATTRIBUTE:type<TGColour>, (void *)&this->colour);
}/*CGChooseColour::CGChooseColour*/

void CGChooseColour::delete(void) {
   ARRAY(&this->paletteB).delete();
   ARRAY(&this->palette).delete();
}/*CGChooseColour::delete*/

void CGLayoutChooseColour::CGLayoutChooseColour(const char *label, TObjectPtr *object, CGPalette *palette, CGPalette *paletteB) {
   new(&this->colour).CGChooseColour(object);
   new(&this->obj_server).CObjServer(CObjPersistent(&this->colour));

   CGLayoutTab(this).CGLayoutTab(label, 0, 0, CObjServer(&this->obj_server), CObjPersistent(&this->colour));

   if (palette) {
      CGCanvas(this).palette_set(palette);
      ARRAY(&this->colour.palette).copy(&palette->colour);
   }
   if (paletteB) {
      CGCanvas(this).palette_B_set(paletteB);
      ARRAY(&this->colour.paletteB).copy(&paletteB->colour);
   }

   CGLayout(this).load_svg_file("memfile:colour.svg", NULL);

//   CGLayout(this).render_set(EGLayoutRender.normal);
   CGCanvas(this).colour_background_set(/*GCOL_DIALOG*/GCOL_NONE);
}/*CGLayoutChooseColour::CGLayoutChooseColour*/

void CGLayoutChooseColour::delete(void) {
   class:base.delete();

   delete(&this->obj_server);
   delete(&this->colour);
}/*CGLayoutChooseColour::delete*/

void CGDialogChooseColour::CGDialogChooseColour(const char *title, TObjectPtr *object,
                                                CGPalette *palette, CGPalette *paletteB,
                                                CGWindow *parent, bool native_dialog) {
   this->native_dialog = native_dialog;
   if (this->native_dialog) {
      CGWindow(this)->parent = parent;
      this->object = object;
      this->palette = palette;
      this->paletteB = paletteB;
   }
   else {
      new(&this->layout).CGLayoutChooseColour(NULL, object, palette, paletteB);
      CGCanvas(&this->layout).colour_background_set(GCOL_DIALOG);
      CGWindowDialog(this).CGWindowDialog(title, CGCanvas(&this->layout), parent);
   }
}/*CGDialogChooseColour::CGDialogChooseColour*/

void CGDialogChooseDate::CGDialogChooseDate(const char *title, bool immediate, TObjectPtr *object, CGWindow *parent) {
   CGVBox *vbox;
   CGDatePicker *date_picker;
   CGHBox *hbox;
   CGTextBox *textbox;
   CGButton *button;
   
   this->immediate = immediate;
   this->object = *object;
   CObjPersistent(this->object.object).attribute_get(this->object.attr.attribute,
                                                     this->object.attr.element,
                                                     this->object.attr.index,
                                                     &ATTRIBUTE:type<TUNIXTime>, (void *)&this->date);
  
   new(&this->obj_server).CObjServer(CObjPersistent(this));
   new(&this->layout).CGLayout(0, 0, &this->obj_server, CObjPersistent(this));
   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_width>, 190);   
   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_height>, 176);   
   CGCanvas(&this->layout).colour_background_set(GCOL_DIALOG);
   
   vbox = new.CGVBox();
   CObject(&this->layout).child_add(CObject(vbox));
   date_picker = new.CGDatePicker(0, 0, 190, 154, 0);
//   CXPath(&CGXULElement(date_picker)->binding).set(object->object, object->attr.attribute, object->attr.index);
   CXPath(&CGXULElement(date_picker)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,date>, -1);
   CObject(vbox).child_add(CObject(date_picker));
   hbox = new.CGHBox();
   CObjPersistent(hbox).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);
   CObjPersistent(hbox).attribute_set_int(ATTRIBUTE<CGXULElement,height>, 22);   
   CObject(vbox).child_add(CObject(hbox));
   button = new.CGButton(0, 0, 0, 0, "midnight");
   CObject(hbox).child_add(CObject(button));
   CXPath(&CGXULElement(button)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,set_midnight>, -1);   

   textbox = new.CGTextBox(0, 0, 0, 0, "0");
   textbox->maxlength = 2;
   textbox->type = EGTextBoxType.numeric;
   CObject(hbox).child_add(CObject(textbox));
//   CXPath(&CGXULElement(textbox)->binding).set(object->object, object->attr.attribute, 3);   
   CXPath(&CGXULElement(textbox)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,date>, 3);   
   textbox = new.CGTextBox(0, 0, 0, 0, "0");
   textbox->maxlength = 2;
   textbox->type = EGTextBoxType.numeric;
   CObject(hbox).child_add(CObject(textbox));
//   CXPath(&CGXULElement(textbox)->binding).set(object->object, object->attr.attribute, 4);   
   CXPath(&CGXULElement(textbox)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,date>, 4);   
   textbox = new.CGTextBox(0, 0, 0, 0, "0");
   textbox->maxlength = 2;
   textbox->type = EGTextBoxType.numeric;
   CObject(hbox).child_add(CObject(textbox));
//   CXPath(&CGXULElement(textbox)->binding).set(object->object, object->attr.attribute, 5);
   CXPath(&CGXULElement(textbox)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,date>, 5);   
     
   CGWindowDialog(this).CGWindowDialog(title, CGCanvas(&this->layout), parent);
}/*CGDialogChooseDate::CGDialogChooseDate*/

void CGDialogChooseDate::delete(void) {
   if (!this->immediate && CGWindowDialog(this)->button_press == 1) {
      CObjPersistent(this->object.object).attribute_set(this->object.attr.attribute,
                                                        this->object.attr.element,
                                                        this->object.attr.index,
                                                        &ATTRIBUTE:type<TUNIXTime>, (void *)&this->date);
      CObjPersistent(this->object.object).attribute_update(this->object.attr.attribute);
      CObjPersistent(this->object.object).attribute_update_end();
   }

   delete(&this->obj_server);
   class:base.delete();
}/*CGDialogChooseDate::delete*/

void CGDialogFileSelect::new(void) {
   new(&this->filter).CGListMenuPopup();
}/*CGDialogFileSelect::new*/

void CGDialogFileSelect::CGDialogFileSelect(const char *title, EDialogFileSelectMode mode, CGWindow *parent) {
   this->parent = parent;
   this->title = title;
   this->mode = mode;
}/*CGDialogFileSelect::CGDialogFileSelect*/

void CGDialogFileSelect::delete(void) {
   delete(&this->filter);
}/*CGDialogFileSelect::delete*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/