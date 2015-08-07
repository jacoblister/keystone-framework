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
#include "../memfile.c"
#include "gcanvas.c"
#include "ganimation.c"
#include "gxulelement.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

#define SVG_LOAD_ANIM_MAP TRUE
#define LAYOUT_TOOLTIP TRUE

class CGLayout;

class CGSelection : CGObject {
 private:
   /* State Machine*/
   CFsm fsm;
   STATE state_choose(CEvent *event);
   STATE state_choose_key(CEvent *event);
   CObjServer *server;

 private:
   TRect area;
   bool bold_highlight;

   void pointer_event(CEventPointer *pointer, CObject *parent);
   void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);

   void new(void);
   void delete(void);
 public:
   CSelection selection;
   CSelection sibling_selection;

   void CGSelection(CGLayout *layout, CObjServer *server);
   static inline bool inside_area(TPoint *point, int extend);
   void select_clear(void);
   bool select_area(CObject *parent);
   bool select_input(void);
   bool select_sibling_next(void);
   bool select_sibling_previous(void);
   bool select_sibling_first(void);
   bool select_sibling_last(void);
   void clear(void);
   void delete_selected(void);
   void selection_update(void);
};

static inline bool CGSelection::inside_area(TPoint *point, int extend) {
   /*>>>coordinate space transform*/
   if (point->x >= (this->area.point[0].x - extend) && point->x <= (this->area.point[1].x + extend) &&
       point->y >= (this->area.point[0].y - extend) && point->y <= (this->area.point[1].y + extend))
      return TRUE;

   return FALSE;
}/*CSelection::inside_area*/

class CObjClientGLayout : CObjClient {
 private:
   void new(void);
   void delete(void);

   void iterate(void);
   void resolve_animation(void *data);

   CGLayout *glayout;
   virtual void notify_select(void);
   virtual void notify_all_update(void);
   virtual void notify_object_child_add(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_child_remove(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing);

   /*>>>consider moving pending update queue to CObjClient */
   ARRAY<CObjPersistent *> pending_object; /* unhandled changed objects on data server */
 public:
   void CObjClientGLayout(CObjServer *obj_server, CObject *host, CObjPersistent *object_root, CGLayout *glayout);
};

/*>>>inherit from CObjPersistent*/
class CGDefs : CGObject {
 private:
   void new(void);
   void delete(void);
   virtual void show(bool show);
 public:
   ALIAS<"svg:defs">;
   ELEMENT:OBJECT<CGPalette palette>;

   void CGDefs(void);
};

ENUM:typedef<EGLayoutRender> {
   {normal}, {buffered}, {none}
};

class CGLayout : CGCanvas {
 private:
   /* State Machine*/
   CFsm fsm;
   STATE state_freeze(CEvent *event);
   STATE state_animate(CEvent *event);

 private:
   bool disable_child_alloc;
   CMutex update_mutex;
#if LAYOUT_TOOLTIP
   CGObject *tooltip_gobject;
   CGObject *tooltip_layout;
   CGObject *tooltip_window;
#endif
   EFileLockMode load_locked;
   TPoint hover_position;
   int hover_timeout;
   double relative_time;
   CTimer iterate_timer;
   ARRAY<CObjPersistent *> time_animation; /* objects in document with time based animations */
   ARRAY<CObjPersistent *> data_animation; /* objects in document with data driven animations */
   void iterate(void);
   int frame_length;
   CObjPersistent *selected_held;

   CGSelection hover_gselection;
   CGSelection key_gselection;

   EGLayoutRender render;
   CGCanvasBitmap canvas_bitmap;

   CObjClientGLayout client;
   void new(void);
   void delete(void);   
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_file_load(const char *filename);
//>>>   virtual void notify_selection_update(void);

   void show_children(bool show);
   CGLayout *find_visible_child(void);
   void key_gselection_set(CGObject *object);
   void key_gselection_refresh(void);

   void animation_resolve_inhibit(CObjPersistent *object);
   void animation_resolve(CObjPersistent *object);
   void animation_reset(CObjPersistent *object, bool animated);
   void animation_buildmap(CObjPersistent *object);
   void animation_add(CObjPersistent *object);
   void animation_remove(CObjPersistent *object);

   static inline void selected_held_set(CObjPersistent *object);

   /*>>>not the right place for these!*/
   CObject *exports_child_tree_first(CObject *parent);
   CObject *exports_child_tree_next(CObject *parent, CObject *child);
 public:
   ALIAS<"svg:svg">;
   ELEMENT:OBJECT<CGDefs defs>; /*>>>should be 'svg:defs' !*/
   ATTRIBUTE<CString xmlns> {
      if (data) {
         CString(&this->xmlns).set(CString(data).string());
      }
      else {
         CString(&this->xmlns).set("http:\057\057www.w3.org""\057""2000\057svg");
      }
   };
   ATTRIBUTE<CString xmlns_xlink, "xmlns:xlink"> {
      if (data) {
         CString(&this->xmlns).set(CString(data).string());
      }
      else {
         CString(&this->xmlns_xlink).set("http:\057\057www.w3.org""\057""1999\057xlink");
      }
   };
   ELEMENT<CString title> {
      if (data) {
         CString(&this->title).set_string(data);
      }
      else {
         CString(&this->title).clear();
      }
   };
   CString filename;


   void CGLayout(int width, int height, CObjServer *obj_server, CObjPersistent *data_source);

   static inline void render_set(EGLayoutRender render);

   CObjPersistent *child_find_by_id(char *id);

   virtual void clear_all(void);
   bool load_svg_file(const char *filename, CString *error_result);

   /*>>>perhaps*/
   bool inplace_edit(CGObject *object);

   virtual void show(bool show);
   virtual void redraw(TRect *extent, EGObjectDrawMode mode);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void event(CEvent *event);

   void animate(void);
};

static inline void CGLayout::render_set(EGLayoutRender render) {
   this->render = render;
}/*CGLayout::render_set*/

static inline void CGLayout::selected_held_set(CObjPersistent *object) {
   this->selected_held = object;
}/*CGLayout::selected_held_set*/

class CGLayoutTab : CGLayout {
 private:
   void new(void);
   void delete(void);
 public:
   ALIAS<"xul:tabpanel">;
   ATTRIBUTE<CString label>;

   void CGLayoutTab(const char *name, int width, int height, CObjServer *obj_server, CObjPersistent *data_source);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include "gprimcontainer.c"
#if LAYOUT_TOOLTIP
#include "gwindow.c"
#endif
#include "ganimationinput.c"

/*>>>not the best place for this */
bool CGObject::keyboard_input(void) {
   CObject *child;

   if (CGObject(this)->visibility != EGVisibility.visible) {
      return FALSE;
   }

   child = CObject(this).child_first();
   while (child) {
      if (CObjClass_is_derived(&class(CGAnimateInput), CObject(child).obj_class())) {
         return TRUE;
      }
      child = CObject(this).child_next(CObject(child));
   }

   return FALSE;
}/*CGObject::keyboard_input*/

void CGObject::select_area_get(CGCanvas *canvas, ARRAY<TPoint> *point) {
   TPoint *pt;
   TCoord extend = 20, length; /*>>>calculate based on device coordinate space */
   TRect extent;

   if (CGCoordSpace(&canvas->coord_space).extent_empty(&this->extent)) {
      ARRAY(point).used_set(0);
   }
   else {
      extent = this->extent;
      length = extent.point[1].x - extent.point[0].x;
      if (length < extend) {
         extent.point[0].x -= (extend - length) / 2;
         extent.point[1].x += (extend - length) / 2;
      }
      length = extent.point[1].y - extent.point[0].y;
      if (length < extend) {
         extent.point[0].y -= (extend - length) / 2;
         extent.point[1].y += (extend - length) / 2;
      }

      ARRAY(point).used_set(4);
      pt = &ARRAY(point).data()[0];
      pt->x = extent.point[0].x;
      pt->y = extent.point[0].y;
      pt = &ARRAY(point).data()[1];
      pt->x = extent.point[1].x;
      pt->y = extent.point[0].y;
      pt = &ARRAY(point).data()[2];
      pt->x = extent.point[1].x;
      pt->y = extent.point[1].y;
      pt = &ARRAY(point).data()[3];
      pt->x = extent.point[0].x;
      pt->y = extent.point[1].y;
   }
}/*CGObject::select_area_get*/

void CGAnimation::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));

   switch (linkage) {
   case EObjectLinkage.ParentRelease:
      if (layout) {
         CGLayout(layout).animation_remove(CObjPersistent(this));
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
   class:base.notify_object_linkage(linkage, object);
}/*CGAnimation::notify_object_linkage*/

bool CGXULElement::keyboard_input(void) {
   if (CObjClass_is_derived(&class(CGBox), CObject(this).obj_class())) {
       return FALSE;
   }
   if (CObjClass_is_derived(&class(CGTextLabel), CObject(this).obj_class())) {
       return FALSE;
   }
   if (CObjClass_is_derived(&class(CGScrollBar), CObject(this).obj_class())) {
       return FALSE;
   }
   if (CObjClass_is_derived(&class(CGScrolledArea), CObject(this).obj_class())) {
       return FALSE;
   }
   if (CObjClass_is_derived(&class(CGSplitter), CObject(this).obj_class())) {
       return FALSE;
   }
   if (this->disabled) {
       return FALSE;
   }
   if (CGObject(this)->visibility == EGVisibility.hidden) {
       return FALSE;
   }

   return TRUE;
}/*CGXULElement::keyboard_input*/

TGColour CGCanvas::colour_background_default(void) {
    if (CObject(this).obj_class() == &class(CGLayoutTab)) {
       return GCOL_NONE;//GCOL_DIALOG;
    }
    return GCOL_WHITE;
}/*CGCanvas::colour_background_default*/

void CGSelection::new(void) {
   new(&this->fsm).CFsm(CObject(this), (STATE)&CGSelection::state_choose);

   new(&this->selection).CSelection(TRUE);
   new(&this->sibling_selection).CSelection(TRUE);
}/*CGSelection::new*/

void CGSelection::CGSelection(CGLayout *layout, CObjServer *server) {
   CObject(this).parent_set(CObject(layout));

   this->server = server;
}/*CGSelection::CGSelection*/

void CGSelection::delete(void) {
   delete(&this->sibling_selection);
   delete(&this->selection);
}/*CGSelection::delete*/

void CGSelection::pointer_event(CEventPointer *event, CObject *parent) {
   ARRAY<TObjectPtr> *selection;
   CGObject *gobject;
   CObject *child, *next_child;
   CGLayout *layout = CGLayout(CObject(this).parent());
   bool handled, key_clear;
   int i;

#if LAYOUT_TOOLTIP
   CGRect *border_rect;
   CGText *text;
   CGWindow *parent_window = CGWindow(CObject(layout).parent_find(&class(CGWindow)));
   TPoint position;
   int width, height;
   bool selected_items;
   const TAttribute *tooltip_attr;
   CString tooltip_text;

   selected_items = CGSelection(this).select_area(parent);

   selection = CSelection(&this->sibling_selection).selection();
   gobject = ARRAY(selection).count() ? CGObject(ARRAY(selection).data()[0].object) : NULL;

   if (event->type == EEventPointer.Leave && layout->tooltip_window) {
      delete(layout->tooltip_window);
      layout->tooltip_window = NULL;
      layout->tooltip_gobject = NULL;
   }

   if (event->type != EEventPointer.Hover) {
//       if (layout->tooltip_window && gobject != layout->tooltip_gobject) {
      if (layout->tooltip_window && (!gobject ||
          !(CObject(gobject).is_child(CObject(layout->tooltip_gobject)) || CObject(layout->tooltip_gobject).is_child(CObject(gobject))))) {
         delete(layout->tooltip_window);
         layout->tooltip_window = NULL;
         layout->tooltip_gobject = NULL;
      }
   }

   new(&tooltip_text).CString(NULL);
   tooltip_attr = NULL;
   if (gobject) {
//      printf("class hit %s\n", CObjClass_name(CObject(gobject).obj_class()));
      /*>>>not ideal, too much overhead performing search*/
      tooltip_attr = CObjPersistent(gobject).attribute_find("tooltiptext");
      if (tooltip_attr) {
         CObjPersistent(gobject).attribute_get_string(tooltip_attr, &tooltip_text);
//         printf("class=%s tooltip %s\n", CObjClass_name(CObject(gobject).obj_class()), CString(&tooltip_text).string());
         if (!CString(&tooltip_text).length()) {
             tooltip_attr = NULL;
         }
      }
   }

   if (parent_window && event->type == EEventPointer.Hover && CString(&tooltip_text).length()) {
      layout->tooltip_gobject = gobject;
      position = event->position;
      CGCanvas(layout).point_array_utov(1, &position);
      CGCanvas(layout).point_ctos(&position);
      if (layout->tooltip_window) {
         delete(layout->tooltip_window);
         layout->tooltip_window = NULL;
      }

      layout->tooltip_layout = (CGObject *) new.CGLayout(0, 0, NULL, NULL);
      CObjPersistent(layout->tooltip_layout).attribute_default_set(ATTRIBUTE<CGCanvas,colour_background>, FALSE);
      CGCanvas(layout->tooltip_layout)->colour_background = (GCOL_TYPE_NAMED | EGColourNamed.lightgoldenrodyellow);

      layout->tooltip_window = (CGObject *) new.CGWindow(NULL, CGCanvas(layout->tooltip_layout), parent_window);

      text = new.CGText(GCOL_BLACK, 4, 12, CString(&tooltip_text).string());
//      CObjPersistent(text).attribute_set_int(ATTRIBUTE<CGText,font_family>, EGFontFamily.Arial);
      CObjPersistent(text).attribute_default_set(ATTRIBUTE<CGText,font_family>, TRUE);
      CObjPersistent(text).attribute_set_int(ATTRIBUTE<CGText,font_size>, 12);
      CObject(layout->tooltip_layout).child_add(CObject(text));
      CGObject(text).extent_set(CGCanvas(layout->tooltip_layout));
      width = (int)(CGObject(text)->extent.point[1].x - CGObject(text)->extent.point[0].x + 6);
      height = (int)(CGObject(text)->extent.point[1].y - CGObject(text)->extent.point[0].y + 2);
      border_rect = new.CGRect(GCOL_BLACK, GCOL_NONE, 0, 0, width - 1, height - 1);
      CObject(layout->tooltip_layout).child_add(CObject(border_rect));

      CGWindow(layout->tooltip_window).position_size_set((int)position.x, (int)position.y + 16, width, height);
      CGWindow(layout->tooltip_window).show(TRUE);
   }
   delete(&tooltip_text);

//   if (event->type == EEventPointer.Hover) {
//       return;
//   }
#endif

   if (layout->selected_held) {
      if (CObjClass_is_derived(&class(CGAnimation), CObject(layout->selected_held).obj_class())) {
         CGAnimation(layout->selected_held).event(CEvent(event));
      }
      return;
   }

   CGSelection(this)->area.point[0].x = event->position.x;
   CGSelection(this)->area.point[0].y = event->position.y;
   CGSelection(this)->area.point[1].x = event->position.x;
   CGSelection(this)->area.point[1].y = event->position.y;

   key_clear = FALSE;
   if (selected_items) {
      selection = CSelection(&this->sibling_selection).selection();

      gobject = NULL;
      for (i = ARRAY(selection).count() - 1; i >= 0; i--) {
         gobject = CGObject(ARRAY(selection).data()[i].object);
//       printf("mouse event %s\n", CObjClass_name(CObject(gobject).obj_class()));

         /*>>>not the best method of identifing if event caused a new selection */
         if (CGObject(gobject).keyboard_input() && event->type == EEventPointer.LeftDown) {
            CGLayout(layout).key_gselection_set(CGObject(gobject));
         }

         /* offer event to objects animations (also links) */
         handled = FALSE;
         child = CObject(gobject).child_first();
         while (child && !handled) {
         next_child = CObject(gobject).child_next(child);
            if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class())) {
               handled = CGAnimation(child).event(CEvent(event));
            }
            else if (CObjClass_is_derived(&class(CGLink), CObject(child).obj_class())) {
               handled = CGObject(child).event(CEvent(event));
            }
            child = next_child;
         }

         /* offer event to object. */
         if (!handled) {
            handled = CGObject(gobject).event(CEvent(event));
         }

         /*>>>not the best method of identifing if event caused a new selection */
         if (event->type == EEventPointer.LeftUp && CGObject(gobject).keyboard_input()) {
            key_clear = TRUE;
         }

         if (handled) {
            return;
         }
      }
//     if (key_clear) {
//         CGLayout(layout).key_gselection_set(NULL);
//     }

      if (gobject && (!CObjClass_is_derived(&class(CGXULElement), CObject(gobject).obj_class()) && key_clear)) {
         CGLayout(layout).key_gselection_set(NULL);
      }
      
      if (gobject && (!CObjClass_is_derived(&class(CGXULElement), CObject(gobject).obj_class()) || 
          CObjClass_is_derived(&class(CGContainer), CObject(gobject).obj_class()))) {

         /* If not processed, offer to child object */
         if (!tooltip_attr) {
           /*>>>kludge, don't pass through if object has tooltip*/
           CGSelection(this).pointer_event(event, CObject(gobject));
         }
      }
   }
   else {
      /* Have not hit an object, return cursor to default */
      /* Not handled, return cursor to default */
   CGCanvas(layout).pointer_cursor_set(EGPointerCursor.default);
   }
}/*CGSelection::pointer_event*/

void CGSelection::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   bool in_key = CFsm(&this->fsm).in_state((STATE)&CGSelection::state_choose_key);
   ARRAY<int> dash_array;
   TObjectPtr *object_selection;
   CGObject *gobject;

    if (CSelection(&this->selection).count() != 1)
      return;

   object_selection = &ARRAY(CSelection(&this->selection).selection()).data()[0];

   if (in_key && CObjClass_is_derived(&class(CGObject), CObject(object_selection->object).obj_class()) &&
       !CObjClass_is_derived(&class(CGXULElement), CObject(object_selection->object).obj_class())) {
      gobject = CGObject(object_selection->object);
      if (this->bold_highlight) {
         CGCanvas(canvas).NATIVE_stroke_style_set(5, NULL, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
         CGCanvas(canvas).xor_write_set(TRUE);
         CGCanvas(canvas).draw_rectangle(GCOL_WHITE, FALSE,
                                         gobject->extent.point[0].x, gobject->extent.point[0].y,
                                         gobject->extent.point[1].x, gobject->extent.point[1].y);
         CGCanvas(canvas).xor_write_set(FALSE);
      }
      else {
         ARRAY(&dash_array).new();
         ARRAY(&dash_array).used_set(2);
         ARRAY(&dash_array).data()[0] = 1;
         ARRAY(&dash_array).data()[1] = 1;
         CGCanvas(canvas).NATIVE_stroke_style_set(1, &dash_array, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
          CGCanvas(canvas).xor_write_set(TRUE);
         CGCanvas(canvas).draw_rectangle(GCOL_WHITE, FALSE,
                                         gobject->extent.point[0].x, gobject->extent.point[0].y,
                                         gobject->extent.point[1].x, gobject->extent.point[1].y);
         /*>>>kludge for now*/
         CGCanvas(canvas).xor_write_set(FALSE);
         ARRAY(&dash_array).delete();
      }
   }
}/*CGSelection::draw*/

STATE CGSelection::state_choose(CEvent *event) {
   CEventPointer *pointer;
   CGLayout *layout = CGLayout(CObject(this).parent());

   if (CObject(event).obj_class() == &class(CEventPointer)) {
      pointer = CEventPointer(event);

      /* convert selection to user space coordinates */
      CGCoordSpace(&CGCanvas(layout)->coord_space).reset();
      CGCanvas(layout).transform_viewbox();
      CGCanvas(layout).point_array_vtou(1, (TPoint *)&pointer->position);

      CGSelection(this).pointer_event(CEventPointer(event), CObject(layout));
   }

   return (STATE)&CFsm:top;
}/*CGSelection::state_choose*/

STATE CGSelection::state_choose_key(CEvent *event) {
   CGLayout *layout = CGLayout(CObject(this).parent()), *parent_layout;
   CObjPersistent *child = NULL, *next, *test;
   int i = 0, j, count;
   int diff_a = 0, diff_b = 0, diff_test_a = 0, diff_test_b = 0;
   bool handled;
   static TRect extent;

   if (CObject(event).obj_class() == &class(CEventKey)) {
      if (CEventKey(event)->type != EEventKeyType.down ||
         CEventKey(event)->modifier & (1 << EEventModifier.ctrlKey) ||
         CEventKey(event)->modifier & (1 << EEventModifier.altKey)) {
         return (STATE)&CFsm:top;
      }

      /* build selectable input index on each keypress for now */
      CGSelection(this).select_input();
      count = ARRAY(CSelection(&this->sibling_selection).selection()).count();

      if (!count) {
         parent_layout = CGLayout(CObject(layout).parent_find(&class(CGLayout)));
         if (parent_layout && event->source != CObject(parent_layout)) {
            /* Enter parent layout & repeat event */
            ARRAY(CSelection(&this->selection).selection()).used_set(0);
            CGLayout(parent_layout).key_gselection_set(CGObject(layout));
            /* Translate 'up/down' keypress */
            if (CEventKey(event)->key == EEventKey.Up) {
                CEventKey(event)->key = EEventKey.Tab;
                CEventKey(event)->modifier = (1 << EEventModifier.shiftKey);
            }
            /* Translate 'up/down' keypress */
            if (CEventKey(event)->key == EEventKey.Down) {
               CEventKey(event)->key = EEventKey.Tab;
               CEventKey(event)->modifier = 0;
            }
            CGLayout(parent_layout).event(event);
         }
         return (STATE)&CFsm:top;
      }
      else {
         if (CSelection(&this->selection).count() != 1) {
            child = NULL;
         }
         else {
            for (i = 0; i < count; i++) {
               child = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i].object;
               if (child == ARRAY(CSelection(&this->selection).selection()).data()[0].object) {
                  break;
               }
            }
            if (i == count) {
               child = NULL;
            }
         }
      }

      switch (CEventKey(event)->key) {
      case EEventKey.Up:
      case EEventKey.Down:
      case EEventKey.Tab:
      case EEventKey.Home:
      case EEventKey.End:
         if (child) {
            if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class())) {
               CGXULElement(child).NATIVE_focus_out();
               CGSelection(this).select_input();
            }
         }
         break;
      default:
         break;
      }

      next = child;
      switch (CEventKey(event)->key) {
      case EEventKey.Left:
         if (child &&
             (CObjClass_is_derived(&class(CGTextBox), CObject(child).obj_class()) ||
              CObjClass_is_derived(&class(CGTabBox), CObject(child).obj_class()))) {
             break;
         }
      case EEventKey.Up:
         if (!child) {
            next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[(count - 1)].object;
            goto update;
         }
         goto move;
      case EEventKey.Right:
         if (child &&
             (CObjClass_is_derived(&class(CGTextBox), CObject(child).obj_class()) ||
              CObjClass_is_derived(&class(CGTabBox), CObject(child).obj_class()))) {
             break;
         }
      case EEventKey.Down:
         if (!child) {
            next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object;
            goto update;
         }
         goto move;
      move:
         next = NULL;

         /* Special cases for tabs */
         if (CObjClass_is_derived(&class(CGTabBox), CObject(child).obj_class()) &&
             CEventKey(event)->key == EEventKey.Down) {
            next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i + 1].object;
            goto update;
         }
         if (CObjClass_is_derived(&class(CGLayoutTab), CObject(child).obj_class())) {
            child = CObjPersistent(CObject(child).parent());
         }

         for (j = 0; j < count; j++) {
            test = ARRAY(CSelection(&this->sibling_selection).selection()).data()[j].object;
            if (CObjClass_is_derived(&class(CGLayoutTab), CObject(test).obj_class())) {
               continue;
            }
            switch (CEventKey(event)->key) {
            case EEventKey.Up:
               diff_test_a = (int)CGObject(child)->extent.point[0].y - (int)CGObject(test)->extent.point[0].y;
               diff_test_b = abs((int)CGObject(test)->extent.point[0].x - (int)CGObject(child)->extent.point[0].x);
               break;
            case EEventKey.Down:
               diff_test_a = (int)CGObject(test)->extent.point[0].y - (int)CGObject(child)->extent.point[0].y;
               diff_test_b = abs((int)CGObject(test)->extent.point[0].x - (int)CGObject(child)->extent.point[0].x);
               break;
            case EEventKey.Left:
               diff_test_a = (int)CGObject(child)->extent.point[0].x - (int)CGObject(test)->extent.point[0].x;
               diff_test_b = abs((int)CGObject(test)->extent.point[0].y - (int)CGObject(child)->extent.point[0].y);
               break;
            case EEventKey.Right:
               diff_test_a = (int)CGObject(test)->extent.point[0].x - (int)CGObject(child)->extent.point[0].x;
               diff_test_b = abs((int)CGObject(test)->extent.point[0].y - (int)CGObject(child)->extent.point[0].y);
               break;
            default:
               break;
            }

            if (diff_test_a > 0) {
               /* A candidate */
               if (!next || diff_test_b < diff_b ||
                   ((diff_test_b == diff_b) && (diff_test_a < diff_a))) {
                  /* A better candidate than current */
                  next = test;
                  diff_a = diff_test_a;
                  diff_b = diff_test_b;
               }
            }
         }

         if (next && CObjClass_is_derived(&class(CGTabBox), CObject(next).obj_class()) &&
            CEventKey(event)->key == EEventKey.Up) {
            next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i - 1].object;
         }
         goto update;
      case EEventKey.Tab:
         next = NULL;
         if ((CEventKey(event)->modifier)) {
            if (!child) {
               next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[(count - 1)].object;
            }
            else {
               next = i > 0 ? ARRAY(CSelection(&this->sibling_selection).selection()).data()[i - 1].object : NULL;
            }
         }
         else {
            if (!child) {
               next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object;
            }
            else {
               next = i < (count - 1) ? ARRAY(CSelection(&this->sibling_selection).selection()).data()[i + 1].object : NULL;
            }
         }
         goto update;
      case EEventKey.Home:
         next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object;
         goto update;
      case EEventKey.End:
         next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[(count - 1)].object;
         goto update;
      update:
         /* Focus out of current selection */
         if (child) {
            if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class())) {
            }
            else {
               parent_layout = CGLayout(CObject(child).parent_find(&class(CGLayout)));
               extent = CGObject(child)->extent;
               CGCoordSpace(&CGCanvas(parent_layout)->coord_space).reset();
               CGCanvas(parent_layout).transform_viewbox();
               CGCanvas(parent_layout).point_array_utov(2, extent.point);
               CGCanvas(parent_layout).queue_draw(&extent);
            }
         }

         if (!next) {
            parent_layout = CGLayout(CObject(layout).parent_find(&class(CGLayout)));
            if (parent_layout) {
               /* Enter parent layout & repeat event */
               ARRAY(CSelection(&this->selection).selection()).used_set(0);
               CGLayout(parent_layout).key_gselection_set(CGObject(layout));

               /* Translate 'up/down' keypress */
               if (CEventKey(event)->key == EEventKey.Up) {
                   CEventKey(event)->key = EEventKey.Tab;
                   CEventKey(event)->modifier = (1 << EEventModifier.shiftKey);
               }
               /* Translate 'up/down' keypress */
               if (CEventKey(event)->key == EEventKey.Down) {
                   CEventKey(event)->key = EEventKey.Tab;
                   CEventKey(event)->modifier = 0;
               }
               CGLayout(parent_layout).event(event);
            }
            else {
               /* Clear selection , repeat event in current layout */
               ARRAY(CSelection(&this->selection).selection()).used_set(1);
               ARRAY(CSelection(&this->selection).selection()).data()[0].object = next;
               CGLayout(layout).event(event);
            }
            return (STATE)&CFsm:top;
         }

         if (CObjClass_is_derived(&class(CGCanvas), CObject(next).obj_class())) {
            /* Enter child layout & repeat event */
            ARRAY(CSelection(&this->selection).selection()).used_set(0);
            CGLayout(next).key_gselection_set(NULL);
            CGLayout(next).event(event);
            return (STATE)&CFsm:top;
         }

         /* Select next sibling */
         if (!CObjClass_is_derived(&class(CGXULElement), CObject(next).obj_class())) {
             parent_layout = CGLayout(CObject(next).parent_find(&class(CGLayout)));
             /*enter parent_layout canvas as for XUL Elements when focus is selected */
             CGLayout(parent_layout).key_gselection_set(CGObject(next));
             extent = CGObject(next)->extent;
             CGCoordSpace(&CGCanvas(parent_layout)->coord_space).reset();
             CGCanvas(parent_layout).transform_viewbox();
             CGCanvas(parent_layout).point_array_utov(2, extent.point);
             CGCanvas(parent_layout).queue_draw(&extent);
         }
         if (CObjClass_is_derived(&class(CGXULElement), CObject(next).obj_class())) {
            CGXULElement(next).NATIVE_focus_in();
         }
         ARRAY(CSelection(&this->selection).selection()).used_set(1);
         ARRAY(CSelection(&this->selection).selection()).data()[0].object = next;
         break;
      default:
         /* offer event to objects animations */
         if (child) {
            handled = FALSE;
            next = CObjPersistent(CObject(child).child_first());
            while (next && !handled) {
               if (CObjClass_is_derived(&class(CGAnimation), CObject(next).obj_class())) {
                  handled = CGAnimation(next).event(CEvent(event));
               }
               next = CObjPersistent(CObject(child).child_next(CObject(next)));
            }
         }
         break;
      }
   }
   return (STATE)&CFsm:top;
}/*CGSelection::state_choose_key*/

void CGSelection::select_clear(void) {
   CObjServer(this->server).root_selection_update();
   CSelection(&this->selection).clear();
   CObjServer(this->server).root_selection_update_end();
}/*CGSelection::select_clear*/

bool CGSelection::select_area(CObject *parent) {
   CObject *object;
   CObjPersistent *embedded;
   CGLayout *layout = CGLayout(CObject(this).parent());
   bool server_update = FALSE;
   ARRAY<TPoint> point;

   /* Only notify server of selection update if layout is a child of the server */
   if (this->server && CObject(this->server->server_root).is_child(parent)) {
      server_update = TRUE;
   }

   CSelection(&this->sibling_selection).clear();

   /* Clear selection if not empty */
   if (CSelection(&this->selection).count()) {
      if (server_update) {
         CObjServer(this->server).root_selection_update();
      }
      CSelection(&this->selection).clear();
      if (server_update) {
         CObjServer(this->server).root_selection_update_end();
      }
   }

   ARRAY(&point).new();

   embedded = CObjPersistent(parent).child_element_class_find(&class(CGPrimContainer), NULL);

   /* Attempt to select the all objects enclosed by selection */
   if (server_update) {
      CObjServer(this->server).root_selection_update();
   }
   object = CObject(parent).child_last();
   while (object) {
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {

         /* Reset selected object extent if not in animate state >>>hack, shouldn't need at all*/
         if (!CFsm(&layout->fsm).in_state((STATE)&CGLayout::state_animate) ||
             (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class()))) {
            CGCanvas(layout).transform_set_gobject(CGObject(object), FALSE);
            CGObject(object).extent_set(CGCanvas(layout));
         }
         if (!CGCoordSpace(&CGCanvas(layout)->coord_space).extent_empty(&CGObject(object)->extent) &&
             this->area.point[0].x <= CGObject(object)->extent.point[0].x &&
             this->area.point[1].x >= CGObject(object)->extent.point[1].x &&
             this->area.point[0].y <= CGObject(object)->extent.point[0].y &&
             this->area.point[1].y >= CGObject(object)->extent.point[1].y) {
             CSelection(&this->selection).add(CObjPersistent(object), NULL);
         }
      }
      if (object == CObject(embedded)) {
         object = NULL;
      }
      else {
         object = CObject(parent).child_previous(object);
         if (!object) {
            object = CObject(embedded);
         }
      }
   }
   if (server_update)
      CObjServer(this->server).root_selection_update_end();

   if (CSelection(&this->selection).count() == 0) {
      /* Attempt to select the first object in layout which current selection area is inside */
      object = CObject(parent).child_last();
      while (object) {
         if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class()) &&
             CGObject(object)->visibility != EGVisibility.hidden) {
//            /* Reset selected object extent if not in animate state >>>hack, shouldn't need at all*/
//            if (!CFsm(&layout->fsm).in_state((STATE)&CGLayout::state_animate)) {
//               CGCanvas(layout).transform_set_gobject(CGObject(object), FALSE);
//               CGObject(object).extent_set(CGCanvas(layout));
//            }
            CGObject(object).select_area_get(CGCanvas(layout), &point);
            if (CGCoordSpace(&CGCanvas(layout)->coord_space).rect_inside_area(&this->area, &point)) {
                CSelection(&this->sibling_selection).add(CObjPersistent(object), NULL);
            }
         }

         if (object == CObject(embedded)) {
            object = NULL;
         }
         else {
            object = CObject(parent).child_previous(object);
            if (!object) {
               object = CObject(embedded);
            }
         }
      }
   }
   ARRAY(&point).delete();

   if (CSelection(&this->sibling_selection).count()) {
      if (server_update) {
         CObjServer(this->server).root_selection_update();
      }
      CSelection(&this->selection).add(ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object, NULL);
      if (server_update) {
         CObjServer(this->server).root_selection_update_end();
      }
      return TRUE;
   }
   return CSelection(&this->selection).count() != 0;
}/*CGSelection::select_area*/

bool CGSelection::select_input(void) {
   /* Select all objects in layout which can take input */
   CObjPersistent *child;
   CObject *parent;
   CGLayout *layout = CGLayout(CObject(this).parent());

   CSelection(&this->sibling_selection).clear();

   child = CObjPersistent(CObject(layout).child_tree_first());
   while (child) {
      if (CObjClass_is_derived(&class(CGObject), CObject(child).obj_class()) &&
         (CGObject(child).keyboard_input() /* || CObjClass_is_derived(&class(CGCanvas), CObject(child).obj_class()) */ ) ) {
         CSelection(&this->sibling_selection).add(CObjPersistent(child), NULL);
      }
      if (CObjClass_is_derived(&class(CGCanvas), CObject(child).obj_class())) {
         /* Don't descent into layouts */
         parent = CObject(child).parent();
         if (CObjClass_is_derived(&class(CGTabBox), CObject(parent).obj_class())) {
            child = CObjPersistent(CObject(CObject(parent).parent()).child_next(CObject(parent)));
         }
         else {
            child = CObjPersistent(CObject(parent).child_next(CObject(child)));
         }
      }
      else {
         if (CObjClass_is_derived(&class(CGTabBox), CObject(child).obj_class())) {
            child = CObjPersistent(CObject(child).child_n(CGTabBox(child)->selectedIndex));
         }
         else {
            child = CObjPersistent(CObject(layout).child_tree_next(CObject(child)));
         }
      }
//      if (CObjClass_is_derived(&class(CGCanvas), CObject(child).obj_class())) {
//         /* Don't descent into layouts */
//         child = CObjPersistent(CObject(CObject(child).parent()).child_next(CObject(child)));
//     }
//     else {
//         child = CObjPersistent(CObject(layout).child_tree_next(CObject(child)));
//      }
   }

   return TRUE;
}/*CGSelection::select_input*/

bool CGSelection::select_sibling_next(void) {
   int i, count;
   CObjPersistent *object;

   count = ARRAY(CSelection(&this->sibling_selection).selection()).count();

   for (i = 0; i < count - 1; i++) {
      object = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i].object;
      if (object == ARRAY(CSelection(&this->selection).selection()).data()[0].object) {
         object = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i + 1].object;
         CObjServer(this->server).root_selection_update();
         CSelection(&this->selection).clear();
         CSelection(&this->selection).add(object, NULL);
         CObjServer(this->server).root_selection_update_end();
         return TRUE;
      }
   }
   return FALSE;
}/*CGSelection::select_sibling_next*/

bool CGSelection::select_sibling_previous(void) {
   int i, count;
   CObjPersistent *object;

   count = ARRAY(CSelection(&this->sibling_selection).selection()).count();

   for (i = count - 1; i > 0; i--) {
      object = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i].object;
      if (object == ARRAY(CSelection(&this->selection).selection()).data()[0].object) {
         object = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i - 1].object;
         CObjServer(this->server).root_selection_update();
         CSelection(&this->selection).clear();
         CSelection(&this->selection).add(object, NULL);
         CObjServer(this->server).root_selection_update_end();
         return TRUE;
      }
   }
   return FALSE;
}/*CGSelection::select_sibling_previous*/

bool CGSelection::select_sibling_first(void) {
   int count;

   count = ARRAY(CSelection(&this->sibling_selection).selection()).count();
   if (count) {
      CObjServer(this->server).root_selection_update();
      CSelection(&this->selection).clear();
      CSelection(&this->selection).add(ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object, NULL);
      CObjServer(this->server).root_selection_update_end();
      return TRUE;
   }
   return FALSE;
}/*CGSelection::select_sibling_first*/

bool CGSelection::select_sibling_last(void) {
   int count;

   count = ARRAY(CSelection(&this->sibling_selection).selection()).count();
   if (count) {
      CObjServer(this->server).root_selection_update();
      CSelection(&this->selection).clear();
      CSelection(&this->selection).add(ARRAY(CSelection(&this->sibling_selection).selection()).data()[count - 1].object, NULL);
      CObjServer(this->server).root_selection_update_end();
      return TRUE;
   }
   return FALSE;
}/*CGSelection::select_sibling_last*/

void CGSelection::clear(void) {
   CSelection(&this->selection).clear();
}/*CGSelection::clear*/

void CGSelection::delete_selected(void) {
//   CGLayout *layout = CGLayout(CObject(this).parent());

   if (this->server) {
      CObjServer(this->server).root_selection_update();
   }
   CSelection(&this->selection).delete_selected();
   if (this->server) {
      CObjServer(this->server).root_selection_update_end();
   }
}/*CGSelection::delete_selected*/

void CGSelection::selection_update(void) {
   CObjPersistent *object;
   int count, i;
   CGLayout *layout = CGLayout(CObject(this).parent());

   count = ARRAY(CSelection(&this->selection).selection()).count();
   for (i = 0; i < count; i++) {
      object = ARRAY(CSelection(&this->selection).selection()).data()[i].object;
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {

      /* Reset selected object extent if not in animate state >>>hack, shouldn't need at all*/
         if (!CFsm(&layout->fsm).in_state((STATE)&CGLayout::state_animate)) {
            CGCanvas(layout).transform_set_gobject(CGObject(object), FALSE);
            CGObject(object).extent_set(CGCanvas(layout));
         }

         if (i == 0) {
            this->area = CGObject(object)->extent;
         }
         else {
            if (CGObject(object)->extent.point[0].x < this->area.point[0].x)
               this->area.point[0].x = CGObject(object)->extent.point[0].x;
            if (CGObject(object)->extent.point[0].y < this->area.point[0].y)
               this->area.point[0].y = CGObject(object)->extent.point[0].y;
            if (CGObject(object)->extent.point[1].x > this->area.point[1].x)
               this->area.point[1].x = CGObject(object)->extent.point[1].x;
            if (CGObject(object)->extent.point[1].y > this->area.point[1].y)
               this->area.point[1].y = CGObject(object)->extent.point[1].y;
         }
      }
   }

   CGCanvas(layout).queue_draw(NULL);
}/*CGSelection::selection_update*/

void CObjClientGLayout::new(void) {
   ARRAY(&this->pending_object).new();
   ARRAY(&this->pending_object).used_set(1000);
   ARRAY(&this->pending_object).used_set(0);
   class:base.new();
}/*CObjClientGLayout::new*/

void CObjClientGLayout::CObjClientGLayout(CObjServer *obj_server, CObject *host,
                                          CObjPersistent *object_root, CGLayout *glayout) {
   this->glayout = glayout;
   CObjClient(this).CObjClient(obj_server, host);
   CObjClient(this).object_root_add(object_root);
}/*CObjClientGLayout::CObjClientGLayout*/

void CObjClientGLayout::delete(void) {
   class:base.delete();
   ARRAY(&this->pending_object).delete();
}/*CObjClientGLayout::delete*/

void CObjClientGLayout::notify_select(void) {
}/*CObjClientGLayout::notify_select*/

void CObjClientGLayout::notify_all_update(void) {
}/*CObjClientGLayout::notify_all_update*/

void CObjClientGLayout::iterate(void) {
   int i;
   CObjPersistent *object;

   CMutex(&this->glayout->update_mutex).lock();
#if 0
   for (i = 0; i < ARRAY(&this->pending_object).count(); i++) {
      CObjClientGLayout(this).resolve_animation(ARRAY(&this->pending_object).data()[i]);
   }
#else
   if (ARRAY(&this->pending_object).count()) {
//      /*>>>not efficient, should just scan data list */
//      CGLayout(this->glayout).animate();
      for (i = 0; i < ARRAY(&this->glayout->data_animation).count(); i++) {
         object = ARRAY(&this->glayout->data_animation).data()[i];
         CGLayout(this->glayout).animation_resolve(object);
      }
   }
#endif
   ARRAY(&this->pending_object).used_set(0);
   CMutex(&this->glayout->update_mutex).unlock();
}/*CObjClientGLayout::iterate*/

void CObjClientGLayout::resolve_animation(void *data) {
   CObjPersistent *object = CObjPersistent(data);
   CObjPersistent *data_object;
   int i;

   if (CFsm(&CGLayout(this->glayout)->fsm).in_state((STATE)&CGLayout::state_animate)) {
      if (object) {
         for (i = 0; i < ARRAY(&this->glayout->data_animation).count(); i++) {
            data_object = ARRAY(&this->glayout->data_animation).data()[i];
            if (CObjClass_is_derived(&class(CGXULElement), CObject(data_object).obj_class())) {
               if (CGXULElement(data_object)->binding.object.object == object) {
                  CGLayout(this->glayout).animation_resolve(data_object);
               }
            }
            if (CObjClass_is_derived(&class(CGAnimation), CObject(data_object).obj_class())) {
               if (CGAnimation(data_object)->binding.object.object == object) {
                  CGLayout(this->glayout).animation_resolve(data_object);
               }
            }
         }
      }
      else {
         CGLayout(this->glayout).animate();
      }
   }
}/*CObjClientGLayout::resolve_animation*/

void CObjClientGLayout::notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing) {
   if (CFsm(&this->glayout->fsm).in_state((STATE)&CGLayout::state_animate)) {
      if (!CMutex(&this->glayout->update_mutex).trylock()) {
         ARRAY(&this->pending_object).item_add(object);
//          WARN("layout client update ok");
         CMutex(&this->glayout->update_mutex).unlock();
      }
      else {
//          WARN("layout client update missed");
      }
   }
}/*CObjClientGLayout::notify_object_attribute_update*/

void CObjClientGLayout::notify_object_child_add(CObjPersistent *object, CObjPersistent *child) {
   if (CFsm(&this->glayout->fsm).in_state((STATE)&CGLayout::state_animate)) {
      ARRAY(&this->pending_object).item_add(NULL);
   }
}/*CObjClientGLayout::notify_object_child_add*/

void CObjClientGLayout::notify_object_child_remove(CObjPersistent *object, CObjPersistent *child) {
//>>>getting lots of these notifications, tooltips?*/
   /*>>>uuber hack, monitor deletion of animations and refect in layout */
   if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class()) ||
       CObjClass_is_derived(&class(CGPrimitive), CObject(child).obj_class())) {
      if (object == ARRAY(&CObjClient(this)->object_root).data()[0]) {
         /*ignore parent notify*/
         return;
      }
      /*>>>hack! if removed child is this layout's server root, then delete this layout */
      if (child == ARRAY(&CObjClient(this)->object_root).data()[0]) {
         CGLayout(this->glayout).show(FALSE);
         delete(this->glayout);
         return;
      }
   }

   /*>>>remove animation from data/time animation list */
   if (CFsm(&this->glayout->fsm).in_state((STATE)&CGLayout::state_animate)) {
      ARRAY(&this->pending_object).item_add(NULL);
   }
}/*CObjClientGLayout::notify_object_child_remove*/

void CGDefs::new(void) {
   class:base.new();
   new(&this->palette).CGPalette(NULL);
   CObjPersistent(&this->palette).attribute_default_set(ATTRIBUTE<CGPalette,colour>, FALSE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGDefs,palette>, TRUE);
   CObject(&this->palette).parent_set(CObject(this));
}/*CGDefs::new*/

void CGDefs::CGDefs(void) {
}/*CGDefs::CGDefs*/

void CGDefs::delete(void) {
   delete(&this->palette);
   class:base.delete();
}/*CGDefs::delete*/

void CGDefs::show(bool show) {
}/*CGDefs::show*/

void CGLayout::new(void) {
   class:base.new();

   new(&this->update_mutex).CMutex();

   CGCanvas(this).CGCanvas(0, 0);

   ARRAY(&this->time_animation).new();
   ARRAY(&this->data_animation).new();

   new(&this->defs).CGDefs();
   CObject(&this->defs).parent_set(CObject(this));
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLayout,defs>, TRUE);

   CGCanvas(this).palette_set(&this->defs.palette);

   new(&this->hover_gselection).CGSelection(this, NULL);
   new(&this->key_gselection).CGSelection(this, NULL);
   CFsm(&this->key_gselection.fsm).transition((STATE)&CGSelection::state_choose_key);

   if (this->frame_length != -1) {
      this->frame_length = 25;
   }
   new(&this->fsm).CFsm(CObject(this), this->frame_length == -1 ? (STATE)&CGLayout::state_freeze : (STATE)&CGLayout::state_animate);
   CFsm(&this->fsm).init();

   new(&this->filename).CString(NULL);
   new(&this->title).CString(NULL);
   new(&this->xmlns).CString(NULL);
   new(&this->xmlns_xlink).CString(NULL);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLayout,xmlns>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLayout,xmlns_xlink>, TRUE);

   new(&this->canvas_bitmap).CGCanvasBitmap(1, 1);
   /* buffered output rendering by default */
//   this->render = EGLayoutRender.buffered;
}/*CGLayout::new*/

void CGLayout::CGLayout(int width, int height, CObjServer *obj_server, CObjPersistent *data_source) {
   if (width != 0 && height != 0) {
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>,  FALSE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGXULElement,width>, width);
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGXULElement,height>, height);
   }
   if (obj_server) {
//      CObjServer(obj_server).selection_set(&CGSelection(&this->hover_gselection)->selection);
      new(&this->client).CObjClientGLayout(obj_server, CObject(this), data_source, this);
   }

   this->hover_gselection.server = obj_server;
}/*CGLayout::CGLayout*/

void CGLayout::delete(void) {
   CEventPointer event;

   /*>>>allocate draw only 'extent' for bitmap, not entire canvas*/
   delete(&this->canvas_bitmap);

   if (CGObject(this)->native_object) {
      /*>>>perhaps generate this on destroy of the native canvas */
      if (CGXULElement(this)->width != 0 && CGXULElement(this)->height != 0) {
         new(&event).CEventPointer(EEventPointer.Leave, 0, 0, 0);
         CGObject(this).event(CEvent(&event));
         delete(&event);
      }
   }

   CFsm(&this->fsm).transition((STATE)&CGLayout::state_freeze);
   CGLayout(this).key_gselection_set(NULL);

   class:base.delete();

   if (CObject_exists((CObject *)&this->client)) {
      delete(&this->client);
   }
   
   delete(&this->defs);

   delete(&this->xmlns_xlink);
   delete(&this->xmlns);
   delete(&this->title);
   delete(&this->filename);

//   delete(&this->fsm);
   delete(&this->hover_gselection);
   delete(&this->key_gselection);
   delete(&this->fsm);
   ARRAY(&this->data_animation).delete();
   ARRAY(&this->time_animation).delete();

   delete(&this->update_mutex);
}/*CGLayout::delete*/

void CGLayout::notify_file_load(const char *filename) {}

void CGLayout::iterate(void) {
   CObjPersistent *object;
   int i;
   CEventPointer event;

   if (this->hover_timeout > 0) {
       this->hover_timeout -= this->frame_length;
       if (this->hover_timeout <= 0) {
          /*>>>include last position and modifier*/
          new(&event).CEventPointer(EEventPointer.Hover, (int)this->hover_position.x, (int)this->hover_position.y, 0);
          CGObject(this).event(CEvent(&event));
          delete(&event);
       }
   }

   this->relative_time += ((double)this->frame_length) / 1000;

   if (CObject_exists((CObject *)&this->client)) {
       CObjClientGLayout(&this->client).iterate();
   }
   for (i = 0; i < ARRAY(&this->time_animation).count(); i++) {
      object = ARRAY(&this->time_animation).data()[i];
      CGLayout(this).animation_resolve(object);
   }
}/*CGLayout::iterate*/

void CGLayout::clear_all(void) {
   CObject *child, *next;
   
   CGLayout(this).show_children(FALSE);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,x>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,y>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,colour_background>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,zoom>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,viewBox>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,preserveAspectRatio>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLayout,title>, TRUE);

   CSelection(&this->key_gselection.selection).clear();
   CSelection(&this->hover_gselection.selection).clear();
   
   child = CObject(this).child_first();
   while (child) {
      next = CObject(this).child_next(child);
      delete(child);
      child = next;
   }

   child = CObject(&this->defs).child_first();
   while (child) {
      next = CObject(&this->defs).child_next(child);
      delete(child);
      child = next;
   }
}/*CGLayout::clear_all*/

#define MEMFILE_PREFIX "memfile:"

bool CGLayout::load_svg_file(const char *filename, CString *error_result) {
   CFileGZ file;
   TException *exception;
   bool result = TRUE, close_result = 1, usefile = FALSE;
   CIOObject *io_object;
   CIOMemory io_memory;
   CMemFile *memfile = NULL;
   bool in_animate = CFsm(&CGLayout(this)->fsm).in_state((STATE)&CGLayout::state_animate);
   
   CGCanvas(this).pointer_cursor_set(EGPointerCursor.wait);

#if SVG_LOAD_ANIM_MAP
   /* re-enter animation state, which will show children and reset animation map */
   if (in_animate) {
      CFsm(&CGLayout(this)->fsm).transition((STATE)&CGLayout::state_freeze);
   }
#endif

   CGCanvas(this).queue_draw_disable(TRUE);

   try {
      if (strncmp(filename, MEMFILE_PREFIX, strlen(MEMFILE_PREFIX)) == 0) {
         memfile = CMemFileDirectory(framework.memfile_directory).memfile_find(filename + strlen(MEMFILE_PREFIX));
         if (!memfile) {
            throw(CObject(framework.memfile_directory), EXCEPTION<>, "File Not Found");
         }

         new(&io_memory).CIOMemory(memfile->memfile_def->data, memfile->memfile_def->size);
         io_object = CIOObject(&io_memory);
      }
      else {
         usefile = TRUE;
#if 1
         new(&file).CFileGZ(this->load_locked);
         if (CFileGZ(&file).open(filename, "rb") == -1) {
             throw(CObject(&file), EXCEPTION<>, "File Not Found");
         }
#else
         new(&file).CFile();
         if (CIOObject(&file).open(filename, O_RDONLY) == -1) {
             throw(CObject(&file), EXCEPTION<>, "File Not Found");
         }
#endif
         io_object = CIOObject(&file);
      }

      ARRAY(&this->time_animation).used_set(0);
      ARRAY(&this->data_animation).used_set(0);

      CGLayout(this).clear_all();
      CObjPersistent(this).state_xml_load(CIOObject(io_object), "svg", FALSE);
   }
   exception = catch(NULL, EXCEPTION<>) {
      result = FALSE;
      if (error_result) {
         CString(error_result).set(exception->message);
      }
   }
   finally {
      /*>>>blocks not working in finally part of exeception syntax*/
      if (memfile)
        delete(&io_memory);
#if 1
      if (usefile)
        close_result = CFileGZ(&file).close() == 0;
#else
      if (usefile)
        close_result = CIOObject(&file).close() == 0;
#endif
      if (result && !close_result && error_result)
         CString(error_result).set("invalid file");
      if (result && !close_result)
         result = close_result;
      if (usefile)
        delete(&file);
   }

   if (!result) {
      CGLayout(this).clear_all();
   }

#if 1
   /*>>>this was used to load the SVG test suite, need to find another method */
   if (CGXULElement(this)->width == 100 && CGXULElement(this)->height == 100) {
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>, TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, TRUE);
      CObjPersistent(this).attribute_update_end();
      CGCanvas(this).NATIVE_size_allocate();
      WARN("CGLayout::load_svg_file(): unsupported width/height/viewBox set, alignment changed");
   }
#else
   /*>>>this was used to load the SVG test suite, need to find another method */
   if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,width>) &&
       !CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,height>) &&
       !CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGCanvas,viewBox>)) {

      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>, TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, TRUE);
      CObjPersistent(this).attribute_update_end();
      CGCanvas(this).NATIVE_size_allocate();
      WARN("CGLayout::load_svg_file(): unsupported width/height/viewBox set, alignment changed");
   }
#endif

   CGCanvas(this).pointer_cursor_set(EGPointerCursor.default);

#if SVG_LOAD_ANIM_MAP
   /* re-enter animation state, which will show children and reset animation map */
   if (in_animate) {
      CFsm(&CGLayout(this)->fsm).transition((STATE)&CGLayout::state_animate);
   }

//   CGLayout(this).animation_reset(CObjPersistent(this), TRUE);
//
//   if (CFsm(&CGLayout(this)->fsm).in_state((STATE)&CGLayout::state_animate)) {
//      CGLayout(this).animation_resolve(CObjPersistent(this));
//   }
#endif
   if (result) {
      CGLayout(this).notify_file_load(filename);
      CString(&this->filename).set(filename);
   }

   CGCanvas(this).queue_draw_disable(FALSE);
   CGCanvas(this).queue_draw(NULL);

   return result;
}/*CGLayout::load_svg_file*/

STATE CGLayout::state_freeze(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         /*>>>not the best place for this*/
         CSelection(&this->key_gselection.selection).clear();
         break;
      case EEventState.leave:
         break;
      default:
         break;
      }
   }
   return (STATE)&CFsm:top;
}/*CGLayout::state_freeze*/

STATE CGLayout::state_animate(CEvent *event) {
   CGLayout *parent_layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));

   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         this->relative_time = 0;

         CGLayout(this).animation_reset(CObjPersistent(this), TRUE);
         CGLayout(this).animation_resolve(CObjPersistent(this));
         CGLayout(this).animation_buildmap(CObjPersistent(this));
         CGSelection(&this->key_gselection).select_input();
         new(&this->iterate_timer).CTimer(this->frame_length, CObject(this), (THREAD_METHOD)&CGLayout::iterate, NULL);
         CGCanvas(this).queue_draw(NULL);
         return NULL;
      case EEventState.leave:
         delete(&this->iterate_timer);

         CGLayout(this).animation_reset(CObjPersistent(this), FALSE);
         return NULL;
      default:
         break;
      }
   }

   if (CObject(event).obj_class() == &class(CEventKey)) {
      if (parent_layout &&
           (CEventKey(event)->key == EEventKey.Enter ||
            CEventKey(event)->key == EEventKey.Escape ||
         CEventKey(event)->key == EEventKey.Function)) {
         CGObject(parent_layout).event(CEvent(event));
      }
   }

   if (CObject(event).obj_class() == &class(CEventPointer) &&
      CEventPointer(event)->type != EEventPointer.Hover &&
      !(CEventPointer(event)->position.x == this->hover_position.x && CEventPointer(event)->position.y == this->hover_position.y)) {
      this->hover_position.x = CEventPointer(event)->position.x;
      this->hover_position.y = CEventPointer(event)->position.y;
      this->hover_timeout = 400;
   }
   if (CObject(event).obj_class() == &class(CEventPointer) && CEventPointer(event)->type == EEventPointer.Leave) {
      this->hover_timeout = 0;
   }

   /* pass event to selection */
   CFsm(&CGSelection(&this->key_gselection)->fsm).event(event);
   CFsm(&CGSelection(&this->hover_gselection)->fsm).event(event);

   return (STATE)&CFsm:top;
}/*CGLayout::state_animate*/

void CGLayout::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CObject *parent;

   switch (linkage) {
   case EObjectLinkage.ChildAdd:
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
         CGCanvas(this).transform_set_gobject(CGObject(object), TRUE);
         CGObject(object).extent_set(CGCanvas(this));
         CGCanvas(this).queue_draw(&CGObject(object)->extent);
      }
      break;
   case EObjectLinkage.ParentSet:
      CGObject(this).show(TRUE);
      
      /* Create a new datasource client based on parent layout */
      if (!CObject_exists((CObject *)&this->client)) {
         parent = object;
         while (parent) {
            if (CObjClass_is_derived(&class(CGLayout), CObject(parent).obj_class())) {
               if (CObject_exists((CObject *)&CGLayout(parent)->client)) {
                  new(&this->client).CObjClientGLayout(CObjClient(&CGLayout(parent)->client)->server,
                                                       CObject(this),
                                                       ARRAY(&CObjClient(&CGLayout(parent)->client)->object_root).data()[0],
                                                       this);
                  CGCanvas(this)->component_reposition = CGCanvas(parent)->component_reposition;
                  if (this->render != EGLayoutRender.buffered) {
                     this->render = CGLayout(parent)->render;
                  }
                  break;
               }
            }
            parent = CObject(parent).parent();
         }
      }
      break;
   case EObjectLinkage.ParentRelease:
      CGObject(this).show(FALSE);      
      break;
   default:
      CGCanvas(this).queue_draw(NULL);
      break;
   }

   /*>>>quick hack*/
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);
}/*CGLayout::notify_object_linkage*/

void CGLayout::show_children(bool show) {
   CObjPersistent *object;

   if (!CObject_exists((CObject *)&this->defs) || !CGObject(this)->native_object) {
      return;
   }   
   
   if (this->disable_child_alloc) {
      show = FALSE;
   }
   
   object = CObjPersistent(this).child_first();

   while (object) {
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class()) &&
          !CObjClass_is_derived(&class(CGDefs), CObject(object).obj_class())) {
         CGObject(object).show(show);
         CGCanvas(this).transform_set_gobject(CGObject(object), FALSE);
         CGObject(object).extent_set(CGCanvas(this));
      }
      object = CObjPersistent(this).child_next(object);
   }
}/*CGLayout::show_children*/

CGLayout *CGLayout::find_visible_child(void) {
   CObject *object = CObject(this).child_tree_first();
   CGLayout *result = this;

   while (object) {
      if (CObjClass_is_derived(&class(CGTabBox), CObject(object).obj_class())) {
         result = CGLayout(CObject(object).child_n(CGTabBox(object)->selectedIndex));
         if (result) {
            result = CGLayout(result).find_visible_child();
            return result;
         }
      }
      object = CObject(this).child_tree_next(object);
   }

   return result;
}/*CGLayout::find_visible_child*/

void CGLayout::show(bool show) {
   CObject *object = CObject(this).child_first();
   CEventPointer event;

   CGCanvas(this).NATIVE_show(show, FALSE);

   if (show) {
      _virtual_CGXULElement_show(CGXULElement(this), show);
   }

   /* show all children of container object */
   CGLayout(this).show_children(show);

   if (!show) {
      if (CGObject(this)->native_object && CGXULElement(this)->width && CGXULElement(this)->height) {
         new(&event).CEventPointer(EEventPointer.Leave, 0, 0, 0);
         CGObject(this).event(CEvent(&event));
         delete(&event);
      }
      _virtual_CGXULElement_show(CGXULElement(this), show);
   }

   /*>>>hack!!!*/
   object = CObject(this).parent();
   if (object && CObject(object).obj_class() == &class(CGVBox)) {
      _virtual_CGXULElement_show(CGXULElement(this), show);
   }

   CGCanvas(this).NATIVE_show(show, TRUE);
}/*CGLayout::show*/

void CGLayout::redraw(TRect *extent, EGObjectDrawMode mode) {
   CGCanvas *dest_canvas;
   bool viewbox_default;

   _virtual_CGXULElement_draw((CGXULElement *)this, CGCanvas(this), extent, mode);

   /*>>>hacky*/
//   if (CGCanvas(this)->colour_background == GCOL_NONE) {
//      return;
//   }

   switch (this->render) {
   case EGLayoutRender.normal:
      dest_canvas = CGCanvas(this);
      break;
   case EGLayoutRender.buffered:
      CGCanvasBitmap(&this->canvas_bitmap).resize(CGCanvas(this)->allocated_width, CGCanvas(this)->allocated_height);

      /* copy palette & other state for source canvas */
      viewbox_default = CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGCanvas,viewBox>);
      CObjPersistent(&this->canvas_bitmap).attribute_default_set(ATTRIBUTE<CGCanvas,viewBox>, viewbox_default);
      CGXULElement(&this->canvas_bitmap)->width = CGXULElement(this)->width;
      CGXULElement(&this->canvas_bitmap)->height = CGXULElement(this)->height;
      CGCanvas(&this->canvas_bitmap)->content_width = CGCanvas(this)->content_width;
      CGCanvas(&this->canvas_bitmap)->content_height = CGCanvas(this)->content_height;
      CGCanvas(&this->canvas_bitmap)->allocated_width = CGCanvas(this)->allocated_width;
      CGCanvas(&this->canvas_bitmap)->allocated_height = CGCanvas(this)->allocated_height;
      CGCanvas(&this->canvas_bitmap)->viewBox = CGCanvas(this)->viewBox;
      CGCanvas(&this->canvas_bitmap)->zoom = CGCanvas(this)->zoom;
      CGCanvas(&this->canvas_bitmap)->yAspect = CGCanvas(this)->yAspect;
      CGCanvas(&this->canvas_bitmap)->preserveAspectRatio = CGCanvas(this)->preserveAspectRatio;
      CGCanvas(&this->canvas_bitmap)->view_origin = CGCanvas(this)->view_origin;
      CGCanvas(&this->canvas_bitmap)->palette = CGCanvas(this)->palette;
      CGCanvas(&this->canvas_bitmap)->paletteB = CGCanvas(this)->paletteB;
      CGCanvas(&this->canvas_bitmap)->render_mode_canvas = CGCanvas(this)->render_mode_canvas;
      CGCanvas(&this->canvas_bitmap)->render_mode = CGCanvas(this)->render_mode;
      CGCanvas(&this->canvas_bitmap)->oem_font = CGCanvas(this)->oem_font;

      CGCanvasBitmap(&this->canvas_bitmap).NATIVE_allocate(NULL);
      CGCanvas(&this->canvas_bitmap).NATIVE_enter(TRUE);

      dest_canvas = CGCanvas(&this->canvas_bitmap);
      break;
   default:
      return;
   }

   CGCanvas(dest_canvas).opacity_set(1.0);
   CGCoordSpace(&CGCanvas(this)->coord_space).reset();
   CGCoordSpace(&CGCanvas(dest_canvas)->coord_space).reset();
   if (extent) {
      CGCanvas(dest_canvas).draw_rectangle(CGCanvas(this)->native_erase_background_colour,
                                           TRUE, extent->point[0].x, extent->point[0].y, extent->point[1].x, extent->point[1].y);
   }
   else {
      CGCanvas(dest_canvas).draw_rectangle(CGCanvas(this)->native_erase_background_colour,
                                           TRUE, 0, 0, CGCanvas(this)->allocated_width, CGCanvas(this)->allocated_height);
   }
   CGCoordSpace(&CGCanvas(this)->coord_space).reset();
   CGCoordSpace(&CGCanvas(dest_canvas)->coord_space).reset();
   CGCanvas(this).transform_viewbox();
   CGCanvas(dest_canvas).transform_viewbox();
   CGCanvas(this).draw(CGCanvas(dest_canvas), extent, mode);

//   CGSelection(&this->hover_gselection).draw(dest_canvas, extent, mode);
//   CGSelection(&this->key_gselection).draw(dest_canvas, extent, mode);

   CGCoordSpace(&CGCanvas(dest_canvas)->coord_space).reset();

   /* fill unused canvas space */
   if (CGCanvas(this)->preserveAspectRatio.aspect != EGPreserveAspectRatio.oneToOne) {
      if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,width>)) {
         CGCanvas(dest_canvas).draw_rectangle(GCOL_RGB(245,245,245), TRUE,
                                              CGXULElement(this)->width, 0, 2000, 2000);
      }
      if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,height>)) {
         CGCanvas(dest_canvas).draw_rectangle(GCOL_RGB(245,245,245), TRUE,
                                              0, CGXULElement(this)->height, 2000, 2000);
      }
   }

   switch (this->render) {
   case EGLayoutRender.buffered:
      CGCanvas(&this->canvas_bitmap).NATIVE_enter(FALSE);
      CGCanvasBitmap(&this->canvas_bitmap).NATIVE_release(NULL);
      CGCoordSpace(&CGCanvas(this)->coord_space).reset();
      CGCanvas(this).NATIVE_draw_bitmap(&this->canvas_bitmap.bitmap, 0, 0);
   default:
      break;
   }
}/*CGLayout::redraw*/

void CGLayout::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   CObject *object = CObject(this).child_first();

   while (object) {
      /*>>>check if object is inside redraw extent before drawing*/
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class()) &&
          !CObjClass_is_derived(&class(CGCanvas), CObject(object).obj_class())) {
         CGCanvas(this).draw_gobject_transformed(canvas, CGObject(object), extent, mode);
      }
      if (CObjClass_is_derived(&class(CGCanvas), CObject(object).obj_class()) &&
          !CGObject(object)->native_object) {
         CGCanvas(this).draw_gobject_transformed(canvas, CGObject(object), extent, mode);
      }
      object = CObject(this).child_next(object);
   }

   CGSelection(&this->hover_gselection).draw(canvas, extent, mode);
   CGSelection(&this->key_gselection).draw(canvas, extent, mode);
}/*CGLayout::draw*/

bool CGLayout::event(CEvent *event) {
   /* Pass event to layout state machine */
   CFsm(&CGLayout(this)->fsm).event(event);
   return TRUE;
}/*CGLayout::event*/

void CGLayout::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObject *object;

   class:base.notify_attribute_update(attribute, changing);

   /*>>>hack, should always be on */
   if (CGCanvas(this)->component_reposition == -1 && changing) {
      return;
   }
//   if (changing) {
//      return;
//   }

   object = CObject(this).child_first();
   while (object) {
      if (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class()) &&
         CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,x>)) {
         CGXULElement(object)->x      = 0;
         CGXULElement(object)->y      = 0;
         CGXULElement(object)->width  = CGXULElement(this)->width;
         CGXULElement(object)->height = CGXULElement(this)->height;
         CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,x>);
         CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,y>);
         CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,width>);
         CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,height>);
         CObjPersistent(object).attribute_update_end();
      }
      object = CObject(this).child_next(object);
   }
}/*CGLayout::notify_attribute_update*/

void CGLayout::animation_resolve_inhibit(CObjPersistent *object) {
   CObjPersistent *child, *reset_anim = NULL;
   bool inhibit;

   /* set inhibitation for active low priority animations if needed */
   inhibit = FALSE;
   child = CObjPersistent(CObject(object).child_last());
   while (child) {
      if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class())) {
         if (CGAnimation(child)->state == EGAnimState.Inhibited) {
            CGAnimation(child)->state = EGAnimState.Active;
         }

         if (inhibit && CGAnimation(child)->time_based && CGAnimation(child)->state == EGAnimState.Active) {
            CGAnimation(child)->state = EGAnimState.Inhibited;

            /* reset animation to ensure state change is seen
                  >>>hack */
            if (reset_anim) {
               CGAnimation(reset_anim).reset(TRUE);
               CGAnimation(reset_anim)->state = EGAnimState.Active;
            }
         }

         /* inhibit only on io based animations for now */
         if (!CGAnimation(child)->time_based && CGAnimation(child)->state == EGAnimState.Active) {
            inhibit = TRUE;
            reset_anim = child;
         }
      }
      child = CObjPersistent(CObject(object).child_previous(CObject(child)));
   }
}/*CGLayout::animation_resolve_inhibit*/

void CGLayout::animation_resolve(CObjPersistent *object) {
   CObjPersistent *child;//, *reset_anim;
//   bool inhibit;
//   EGAnimState anim_state;

   if (object == CObjPersistent(this)) {
      child = CObjPersistent(this).child_element_class_find(&class(CGPrimContainer), NULL);
      if (child) {
         CGLayout(this).animation_resolve(child);
      }
   }

   child = CObjPersistent(CObject(object).child_first());
   while (child) {
//      if (!CObjClass_is_derived(&class(CGLayout), CObject(child).obj_class())) {
      if (!CObjClass_is_derived(&class(CGScrolledArea), CObject(child).obj_class())) {
         CGLayout(this).animation_resolve(child);
      }
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }

   if (CObjClass_is_derived(&class(CGAnimation), CObject(object).obj_class())) {
#if 0
      /*>>>don't use inhibit for now*/
      anim_state = CGAnimation(object)->state;
      CGAnimation(object).animation_resolve_value(this->relative_time);
      if (anim_state == EGAnimState.Inhibited) {
         CGAnimation(object)->state = anim_state;
      }
      if (anim_state != CGAnimation(object)->state) {
         CGLayout(this).animation_resolve_inhibit(CObjPersistent(CObject(object).parent()));
      }
      if (CGAnimation(object)->state == EGAnimState.Active ||
         anim_state == EGAnimState.Active) {
         CGAnimation(object).animation_resolve();
      }
#else
      CGAnimation(object).animation_resolve_value(this->relative_time);
      CGAnimation(object).animation_resolve();
#endif
   }
   else if (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
      CGXULElement(object).binding_resolve();
   }
}/*CGLayout::animation_resolve*/

void CGLayout::animation_reset(CObjPersistent *object, bool animated) {
   CObjPersistent *child;
/*   CObjPersistent *use = CObjPersistent(CObject(object).parent_find(&class(CGUse)));*/

   if (object == CObjPersistent(this)) {
      child = CObjPersistent(this).child_element_class_find(&class(CGPrimContainer), NULL);
      if (child) {
         CGLayout(this).animation_reset(child, animated);
      }
   }

   child = CObjPersistent(CObject(object).child_first());

   /*>>>Reset object extent here for now */
#if 0
   if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
      CGCanvas(this).transform_set_gobject(CGObject(object), FALSE);
      CGObject(object).extent_set(CGCanvas(this));
   }
#endif

   while (child) {
      if (!CObjClass_is_derived(&class(CGLayout), CObject(child).obj_class())) {
         CGLayout(this).animation_reset(child, animated);
      }
      if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class()) /*&& !(use && !animated)*/) {
         CGAnimation(child).reset(animated);
      }
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }

}/*CGLayout::animation_reset*/

void CGLayout::animation_buildmap(CObjPersistent *object) {
   CObjPersistent *child;

   if (object == CObjPersistent(this)) {
      ARRAY(&this->time_animation).used_set(0);
      ARRAY(&this->data_animation).used_set(0);

      child = CObjPersistent(this).child_element_class_find(&class(CGPrimContainer), NULL);
      if (child) {
         CGLayout(this).animation_buildmap(child);
      }
   }

   child = CObjPersistent(CObject(object).child_first());
   while (child) {
//      if (!CObjClass_is_derived(&class(CGLayout), CObject(child).obj_class()) ||
//          CObjClass_is_derived(&class(CGLayoutTab), CObject(child).obj_class())) {
      if (!CObjClass_is_derived(&class(CGScrolledArea), CObject(child).obj_class())) {
         CGLayout(this).animation_buildmap(child);
         if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class()) /*&& CGXULElement(child)->binding.object.object*/) {
            ARRAY(&this->data_animation).item_add(child);
         }
      }
      if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class())) {
         CXPath(&CGAnimation(child)->binding).resolve();
//printf("animation buildmap %s\n", CObjClass_name(CObject(child).obj_class()));
         CGAnimation(child).animation_resolve_value(0); /*>>>kludge to set time flag */
         if (CGAnimation(child)->time_based) {
            ARRAY(&this->time_animation).item_add(child);
            /*>>>For now, add time based animations to the data based animation list as well */
            ARRAY(&this->data_animation).item_add(child);
         }
         else {
            ARRAY(&this->data_animation).item_add(child);
         }
      }
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }

}/*CGLayout::animation_buildmap*/

void CGLayout::animation_add(CObjPersistent *object) {
   if (!CFsm(&this->fsm).in_state((STATE)&CGLayout::state_animate)) {
       return;
   }

   if (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
      ARRAY(&this->data_animation).item_add(object);
   }
   if (CObjClass_is_derived(&class(CGAnimation), CObject(object).obj_class())) {
      CGAnimation(object).animation_resolve_value(0); /*>>>kludge to set time flag */
      if (CGAnimation(object)->time_based) {
         ARRAY(&this->time_animation).item_add(object);
      }
      else {
         ARRAY(&this->data_animation).item_add(object);
      }
   }
}/*CGLayout::animation_add*/

void CGLayout::animation_remove(CObjPersistent *object) {
   int i;
   CObjPersistent *anim;

   if (!CFsm(&this->fsm).in_state((STATE)&CGLayout::state_animate)) {
       return;
   }

   for (i = 0; i < ARRAY(&this->data_animation).count(); i++) {
      anim = ARRAY(&this->data_animation).data()[i];
      if (anim == object) {
         ARRAY(&this->data_animation).item_remove(object);
         break;
      }
   }
   for (i = 0; i < ARRAY(&this->time_animation).count(); i++) {
      anim = ARRAY(&this->time_animation).data()[i];
      if (anim == object) {
         ARRAY(&this->time_animation).item_remove(object);
         break;
      }
   }
}/*CGLayout::animation_remove*/

void CGLayout::animate(void) {
   CGLayout(this).animation_resolve(CObjPersistent(this));
}/*CGLayout::animate*/

void CGLayoutTab::new(void) {
   class:base.new();

//   CGLayout(this).render_set(EGLayoutRender.none);

   new(&this->label).CString(NULL);
}/*CGLayoutTab::new*/

void CGLayoutTab::CGLayoutTab(const char *name, int width, int height, CObjServer *obj_server, CObjPersistent *data_source) {
   CGLayout(this).CGLayout(width, height, obj_server, data_source);

   CString(&this->label).set(name);
}/*CGLayoutTab::CGLayoutTab*/

void CGLayoutTab::delete(void) {
   delete(&this->label);

   class:base.delete();
}/*CGLayoutTab::delete*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/