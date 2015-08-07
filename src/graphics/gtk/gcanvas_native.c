/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "../../framework_base.c"
#include "../../object.c"
#include "../gobject.c"
#include "../gcanvas.c"
#include "../gwindow.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#define CANVAS_REDRAW TRUE
#define CANVAS_IMMEDIATE_REPAINT FALSE

#include <gtk/gtk.h>
#if KEYSTONE_GTK_CAIRO
#include <cairo.h>
#endif

#include "native.h"

void CGBitmap::NATIVE_allocate(void) {
//   int x, y;
   GdkGC *gc;

   this->native_object =
      gdk_pixmap_new(NULL, this->width, this->height, 24/*gdk_visual_get_best_depth()*/);
//   this->native_object =
//      gdk_pixmap_new(NULL, this->width, this->height, gdk_visual_get_best_depth());

   gc = gdk_gc_new(this->native_object);

#if 0
   for (x = 0; x < this->width; x++) {
      for (y = 0; y < this->height; y++) {
         gdk_rgb_gc_set_foreground(gc, GCOL_RGB((x * 255) / this->width, 0, 0));
         gdk_draw_point(this->native_object, gc, x, y);
      }
   }
#endif

   gdk_gc_destroy(gc);
}/*CGBitmap::NATIVE_allocate*/

void CGBitmap::NATIVE_release(void) {
#if KEYSTONE_GTK_CAIRO
   if (this->image) {
       cairo_surface_destroy((cairo_surface_t *)this->native_object);
       return;
   }
#endif

   gdk_pixmap_unref(this->native_object);
}/*CGBitmap::NATIVE_release*/

void CGBitmap::load_file(CGCanvas *canvas, const char *filename) {
#if KEYSTONE_GTK_CAIRO
   this->image = TRUE;

   this->native_object = (cairo_surface_t *)cairo_image_surface_create_from_png(filename);

   this->width = cairo_image_surface_get_width((cairo_surface_t *)this->native_object);
   this->height = cairo_image_surface_get_height((cairo_surface_t *)this->native_object);

   return;
#endif
   ASSERT("CGBitmap::load_file - not implemented");
#if 0
   int i;
   word data[1024];
   int j, val;
   HBITMAP hBitmap;
   HDC hdc, hdcB;

   CGBitmap(this).NATIVE_allocate();

   for (i = 0; i < this->height; i++) {
      for (j = 0; j < this->width; j++) {
         val = (i / 10) / 2 + 16;
         data[j] = ((val & 31) << 11) | ((val & 31) << 6) | ((val & 31) << 0);
      }
      CGBitmap(this).datalines_write(i, 1, data);
   }
#endif
}/*CGBitmap::load_file*/

typedef struct {
   CString name;
   GdkFont *font;
} TGCanvasXFont;

ARRAY:typedef<TGCanvasXFont>;

class CXFontCache : CObject {
 public:
   GdkFont *load_font(const char *xfont_desc);
 private:
   void new(void);
   void delete(void);
   ARRAY<TGCanvasXFont> fcache;
};

OBJECT<CXFontCache, font_cache>;

void CXFontCache::new(void) {
   ARRAY(&this->fcache).new();
}/*CXFontCache::new*/

void CXFontCache::delete(void) {
   ARRAY(&this->fcache).delete();
}/*CXFontCache::delete*/

GdkFont*CXFontCache::load_font(const char *xfont_desc) {
   int i;
   TGCanvasXFont *cachefont;

   /* look for font in cache first */
   for (i = 0; i < ARRAY(&this->fcache).count(); i++) {
      cachefont = &ARRAY(&this->fcache).data()[i];
      if (CString(&cachefont->name).strcmp(xfont_desc) == 0) {
         return cachefont->font;
      }
   }

   /* else allocate new cache record */
   ARRAY(&this->fcache).item_add_empty();
   cachefont = ARRAY(&this->fcache).item_last();
   new(&cachefont->name).CString(xfont_desc);
   cachefont->font = gdk_font_load(xfont_desc);

   return cachefont->font;
}/*CXFontCache::load_font*/

#if CANVAS_REDRAW
/*>>>kludge for thread safe redraw operation*/
bool canvas_redraw_inited = FALSE;
GtkWidget *canvas_redraw_widget = NULL;

gint canvas_redraw(gpointer data) {
   if (canvas_redraw_widget) {
      gtk_widget_queue_draw(canvas_redraw_widget);
      canvas_redraw_widget = NULL;
   }

   return TRUE;
}/*canvas_redraw*/
#endif

Local gint CGCanvas__draw(GtkWidget *widget, GdkRectangle *draw,
                          gpointer data) {
   TRect ud_rect;
   CGCanvas *this = CGCanvas(data);

   if (!widget->window)
      return FALSE;

   CGCanvas(this).NATIVE_enter(TRUE);

   gdk_gc_set_clip_rectangle(CGCanvas_NATIVE_DATA(this)->gc, draw);

   ud_rect.point[0].x = draw->x;
   ud_rect.point[0].y = draw->y;
   ud_rect.point[1].x = draw->x + draw->width  - 1;
   ud_rect.point[1].y = draw->y + draw->height - 1;

//   gdk_window_clear_area(widget->window,
//                         draw->x, draw->y,
//                         draw->width, draw->height);

   this->native_erase_background_colour = this->colour_background;
   if (this->native_erase_background_colour == GCOL_DIALOG) {
       this->native_erase_background_colour = GCOL_NONE;
   }
   CGCanvas(this).redraw(&ud_rect, EGObjectDrawMode.Draw);

   CGCanvas(this).NATIVE_enter(FALSE);

   return FALSE;
}/*CGCanvas__draw*/

/* Repaint a proportion of the screen */
Local gint CGCanvas__expose_event(GtkWidget *widget, GdkEventExpose *event,
                                  gpointer data) {
   CGCanvas__draw(widget, &event->area, data);
   return FALSE;
}/*CGCanvas__expose_event*/

gint CGCanvas__motion_notify_event(GtkWidget *widget,
                                   GdkEventMotion *event_motion,
                                   gpointer data) {
   CEventPointer event;
   GdkModifierType state;
   gint x, y;
   CGCanvas *this = CGCanvas(data);

   if (event_motion->is_hint) {
      gdk_window_get_pointer(event_motion->window, &x, &y, &state);
   }
   else {
      x = (gint)event_motion->x;
      y = (gint)event_motion->y;
      state = event_motion->state;
   }

   x += (gint)this->viewBox.point[0].x;
   y += (gint)this->viewBox.point[0].y;

   new(&event).CEventPointer(EEventPointer.Move, x, y, 0);
   CGObject(this).event(CEvent(&event));
   delete(&event);

   return FALSE;
}/*CGCanvas__motion_notify_event*/

gint CGCanvas__leave_notify_event(GtkWidget *widget,
                                  GdkEventMotion *event_motion,
                                  gpointer data) {
   CEventPointer event;
   CGCanvas *this = CGCanvas(data);

   new(&event).CEventPointer(EEventPointer.Leave, 0, 0, 0);
   CGObject(this).event(CEvent(&event));
   delete(&event);

   return FALSE;
}/*CGCanvas__leave_notify_event*/

Local gint CGCanvas__button_event(GtkWidget *widget,
                                  GdkEventButton *event_button,
                                  gpointer data) {
   CEventPointer event;
   EEventPointer type;
   CGCanvas *this = CGCanvas(data);
   bool state = event_button->state;

#if OS_Win32
   state = !state;
#endif

   switch (event_button->button) {
   case 1:
      if (state) {
         type = EEventPointer.LeftUp;
         CGCanvas_NATIVE_DATA(this)->button_state = FALSE;
      }
      else {
         if (CGCanvas_NATIVE_DATA(this)->button_state)  {
            type = EEventPointer.LeftDClick;
         }
         else {
            type = EEventPointer.LeftDown;
         }
         CGCanvas_NATIVE_DATA(this)->button_state = TRUE;
      }
      break;
   case 3:
      type = state ?
         EEventPointer.RightUp : EEventPointer.RightDown;
      break;
   default:
      return FALSE;
   }
   new(&event).CEventPointer(type,
                              (int)event_button->x + (int)this->viewBox.point[0].x,
                              (int)event_button->y + (int)this->viewBox.point[0].y,
                              event_button->state & 0xFF);
   CGObject(this).event(CEvent(&event));
   delete(&event);

   return FALSE;
}/*CGCanvas__button_event*/

Local gint CGCanvas__size_request(GtkWidget *widget,
                                  GtkRequisition *requisition,
                                  gpointer data) {
   widget->requisition.width  = 0;
   widget->requisition.height = 0;
   return FALSE;
};

Local gint CGCanvas__size_allocate(GtkWidget *widget,
                                   GtkAllocation *allocation,
                                   gpointer data) {
   CGCanvas *this = CGCanvas(data);
   gint width, height;

   width  = allocation->width;
   height = allocation->height;

   /*>>>hack, ignore transient size sets */
   if (width == 1 && height == 1)
      return FALSE;

   if (width == this->resize_width &&
       height == this->resize_height)
      return FALSE;

   this->resize_width  = width;
   this->resize_height = height;

   this->allocated_width  = width;
   this->allocated_height = height;

   CGCanvas(this).NATIVE_size_allocate();

   return FALSE;
}/*CGCanvas__size_allocate*/

Local gint CGCanvas__hscroll_value_changed(GtkAdjustment *allocation,
                                           gpointer data) {
   CGCanvas *this = CGCanvas(data);

   this->view_origin.x = (int) allocation->value;
   CGCanvas(this).view_update(FALSE);
   gtk_widget_queue_draw(GTK_WIDGET(CGObject(this)->native_object));

   return FALSE;
}/*CGCanvas__hscroll_value_changed*/

Local gint CGCanvas__vscroll_value_changed(GtkAdjustment *allocation,
                                           gpointer data) {
   CGCanvas *this = CGCanvas(data);

   this->view_origin.y = (int) allocation->value;
   CGCanvas(this).view_update(FALSE);
   gtk_widget_queue_draw(GTK_WIDGET(CGObject(this)->native_object));

   return FALSE;
}/*CGCanvas__vscroll_value_changed*/

void CGCanvas::NATIVE_enter(bool enter) {
   if (enter) {
      if (!this->enter_count) {
         if (CObject(this).obj_class() == &class(CGCanvasBitmap)) {
            CGCanvas_NATIVE_DATA(this)->font_default =
               gdk_font_load("-*-helvetica-bold-r-normal--12-120-*-*-*-*-iso8859-1");

            CGCanvas_NATIVE_DATA(this)->wnd = (GdkWindow *)CGCanvasBitmap(this)->bitmap.native_object;
            CGCanvas_NATIVE_DATA(this)->gc  = gdk_gc_new((GdkWindow *)CGCanvasBitmap(this)->bitmap.native_object);
         }
         else {
            CGCanvas_NATIVE_DATA(this)->wnd = CGCanvas_NATIVE_DATA(this)->fixed ? CGCanvas_NATIVE_DATA(this)->fixed->window : NULL;
            if (CGCanvas_NATIVE_DATA(this)->wnd) {
               CGCanvas_NATIVE_DATA(this)->gc  = gdk_gc_new(CGCanvas_NATIVE_DATA(this)->fixed->window);
            }
         }
#if KEYSTONE_GTK_CAIRO
         if (CGCanvas_NATIVE_DATA(this)->wnd && this->render_mode_canvas == EGCanvasRender.full) {
            CGCanvas_NATIVE_DATA(this)->cairo = gdk_cairo_create(CGCanvas_NATIVE_DATA(this)->wnd);
         }
#endif
      }
      this->enter_count++;
   }
   else {
      this->enter_count--;
      if (!this->enter_count) {
#if KEYSTONE_GTK_CAIRO
         if (CGCanvas_NATIVE_DATA(this)->cairo) {
            cairo_destroy(CGCanvas_NATIVE_DATA(this)->cairo);
         }
         CGCanvas_NATIVE_DATA(this)->cairo = NULL;
#endif
         if (CGCanvas_NATIVE_DATA(this)->wnd) {
            gdk_gc_destroy(CGCanvas_NATIVE_DATA(this)->gc);
         }
         CGCanvas_NATIVE_DATA(this)->wnd = NULL;
         CGCanvas_NATIVE_DATA(this)->gc  = NULL;
      }
   }
}/*CGCanvas::NATIVE_enter*/

void CGCanvas::NATIVE_allocate(CGLayout *layout) {
   GtkWidget **widget;
   CGObject *parent;
   TPoint position[2];

   widget = (GtkWidget **)&(CGObject(this)->native_object);

   CGCanvas_NATIVE_DATA(this)->font_default =
      gdk_font_load("-*-helvetica-bold-r-normal--12-120-*-*-*-*-iso8859-1");

#if 0
   CGCanvas_NATIVE_DATA(this)->fixed = gtk_fixed_new();
   *widget = CGCanvas_NATIVE_DATA(this)->fixed;
   gtk_fixed_set_has_window(GTK_FIXED(*widget), TRUE);
#else
   CGCanvas_NATIVE_DATA(this)->table = gtk_table_new(2, 2, FALSE);
   CGCanvas_NATIVE_DATA(this)->fixed = gtk_fixed_new();
   gtk_fixed_set_has_window(GTK_FIXED(CGCanvas_NATIVE_DATA(this)->fixed), TRUE);

   *widget = CGCanvas_NATIVE_DATA(this)->table;
   gtk_table_attach(GTK_TABLE(CGCanvas_NATIVE_DATA(this)->table),
                    CGCanvas_NATIVE_DATA(this)->fixed,
                    0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
                    0, 0);

   CGCanvas_NATIVE_DATA(this)->hadjust =
      GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 0, 1, 1, 0));
   CGCanvas_NATIVE_DATA(this)->hscroll =
      gtk_hscrollbar_new(CGCanvas_NATIVE_DATA(this)->hadjust);
   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->hadjust),
                      "value_changed",
                      (GtkSignalFunc) CGCanvas__hscroll_value_changed, this);
   gtk_widget_show(CGCanvas_NATIVE_DATA(this)->hscroll);
   gtk_table_attach(GTK_TABLE(CGCanvas_NATIVE_DATA(this)->table),
                    CGCanvas_NATIVE_DATA(this)->hscroll,
                    0, 1, 1, 2, GTK_FILL, 0, 0, 0);

   CGCanvas_NATIVE_DATA(this)->vadjust =
      GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 0, 1, 1, 0));
   CGCanvas_NATIVE_DATA(this)->vscroll =
      gtk_vscrollbar_new(CGCanvas_NATIVE_DATA(this)->vadjust);
   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->vadjust),
                      "value_changed",
                      (GtkSignalFunc) CGCanvas__vscroll_value_changed, this);
   gtk_widget_show(CGCanvas_NATIVE_DATA(this)->vscroll);
   gtk_table_attach(GTK_TABLE(CGCanvas_NATIVE_DATA(this)->table),
                    CGCanvas_NATIVE_DATA(this)->vscroll,
                    1, 2, 0, 1, 0, GTK_FILL, 0, 0);

#endif
   /* Signals used to handle repaint */
   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
                      "expose_event",
                      (GtkSignalFunc) CGCanvas__expose_event, this);
   /*>>>need this?*/
//   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
//                      "draw",
//                      (GtkSignalFunc) CGCanvas__draw, this);
   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
                      "motion_notify_event",
                      (GtkSignalFunc) CGCanvas__motion_notify_event, this);
   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
                      "leave_notify_event",
                      (GtkSignalFunc) CGCanvas__leave_notify_event, this);
   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
                      "button_press_event",
                      (GtkSignalFunc) CGCanvas__button_event, this);
   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
                      "button_release_event",
                      (GtkSignalFunc) CGCanvas__button_event, this);
   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
                      "size_request",
                      (GtkSignalFunc) CGCanvas__size_request, this);
   gtk_signal_connect(GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
                      "size_allocate",
                      (GtkSignalFunc) CGCanvas__size_allocate, this);
   gtk_widget_set_events(CGCanvas_NATIVE_DATA(this)->fixed, GDK_EXPOSURE_MASK
                           | GDK_LEAVE_NOTIFY_MASK
                           | GDK_BUTTON_PRESS_MASK
                           | GDK_BUTTON_RELEASE_MASK
                           | GDK_POINTER_MOTION_MASK
//                         | GDK_POINTER_MOTION_HINT_MASK
                         );
   //gtk_widget_show(d_area);
   gtk_widget_show(CGCanvas_NATIVE_DATA(this)->fixed);

   /*>>>shouldn't need this, canvas object isn't getting the SHOW message
    * at the GObject level for some reason */
   gtk_widget_show(*widget);

#if CANVAS_REDRAW
   if (!canvas_redraw_inited) {
      canvas_redraw_inited = TRUE;
      gtk_timeout_add(1000, canvas_redraw, NULL);
   }
#endif

   parent = CGObject(CObject(this).parent());
   while (parent) {
      if (CObjClass_is_derived(&class(CGCanvas), CObject(parent).obj_class())) {
         break;
      }
      if (CObjClass_is_derived(&class(CGTabBox), CObject(parent).obj_class())   ||
          CObjClass_is_derived(&class(CGSplitter), CObject(parent).obj_class())
         ) {
         parent = NULL;
         break;
      }
      parent = CGObject(CObject(parent).parent());
   }

   if (parent) {
      CGXULElement(this).utov(CGCanvas(parent), position);

      gtk_widget_set_usize(*widget,
                           (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
      gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(parent)->fixed),
                    *widget, (int)position[0].x, (int)position[0].y);
   }
   class:base.NATIVE_allocate(layout);
}/*CGCanvas::NATIVE_allocate*/

void CGCanvas::NATIVE_release(CGLayout *layout) {
   if (CObject(this).obj_class() != &class(CGCanvasBitmap)) {
      class:base.NATIVE_release(layout);
   }
}/*CGCanvas::NATIVE_release*/

void CGCanvas::NATIVE_show(bool show, bool end) {
    if (show && end) {
        /*>>>do nothing for now, should hide/show main canvas*/
    }
}/*CGCanvas::NATIVE_show*/

void CGCanvas::NATIVE_size_allocate(void) {
   GtkAdjustment *adjust;

   if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,width>) &&
       CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,height>)) {
      CGXULElement(this)->width  = this->allocated_width;
      CGXULElement(this)->height = this->allocated_height;
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_update_end();
   }

   CGCanvas(this).view_update(TRUE);

   if (CGCanvas_NATIVE_DATA(this)->hscroll &&
       CGCanvas_NATIVE_DATA(this)->vscroll) {
      adjust = CGCanvas_NATIVE_DATA(this)->hadjust;
      adjust->lower     = 0;
      adjust->upper     = this->scroll_width - 1;
      adjust->page_size = this->allocated_width;

      adjust = CGCanvas_NATIVE_DATA(this)->vadjust;
      adjust->lower     = 0;
      adjust->upper     = this->scroll_height - 1;
      adjust->page_size = this->allocated_height;

//      if (this->allocated_width >= (this->scroll_width - 100)) {
      if (this->allocated_width < (this->scroll_width - 20) && this->scrollMode != EGCanvasScroll.none) {
         gtk_widget_show(CGCanvas_NATIVE_DATA(this)->hscroll);
      }
      else {
         gtk_widget_hide(CGCanvas_NATIVE_DATA(this)->hscroll);
      }

//      if (this->allocated_height >= (this->scroll_height - 100)) {
      if (this->allocated_height < (this->scroll_height - 20) && this->scrollMode != EGCanvasScroll.none) {
         gtk_widget_show(CGCanvas_NATIVE_DATA(this)->vscroll);
      }
      else {
         gtk_widget_hide(CGCanvas_NATIVE_DATA(this)->vscroll);
      }
   }
}/*CGCanvas::NATIVE_size_allocate*/

void CGCanvas::point_ctos(TPoint *point) {
    gint x, y;
    /*>>>hack, just get current pointer position */
    gdk_display_get_pointer(gdk_display_get_default(), NULL, &x, &y, NULL);
    point->x = x;
    point->y = y;
}/*CGCanvas::point_ctos*/

void CGCanvas::point_stoc(TPoint *point) {
}/*CGCanvas::point_stoc*/

void CGCanvas::NATIVE_queue_draw(const TRect *extent) {
//   GdkEventExpose event;

   if (!CGObject(this)->native_object) {
      return;
   }

#if CANVAS_REDRAW
   if (CFramework(&framework).thread_this() ==
       CFramework(&framework).thread_main()) {
      if (extent) {
#if CANVAS_IMMEDIATE_REPAINT
         CLEAR(&event);
         event.area.x = extent->point[0].x - 5;
         event.area.y = extent->point[0].y - 5;
         event.area.width  = extent->point[1].x - extent->point[0].x + 1 + 5;
         event.area.height = extent->point[1].y - extent->point[0].y + 1 + 5;
         CGCanvas__expose_event((GtkWidget *)GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
                                &event, (gpointer) this);
#else
         gtk_widget_queue_draw_area(CGCanvas_NATIVE_DATA(this)->fixed,
                                    (int)extent->point[0].x - 5,
                                    (int)extent->point[0].y - 5,
                                    (int)extent->point[1].x - (int)extent->point[0].x + 1 + 5,
                                    (int)extent->point[1].y - (int)extent->point[0].y + 1 + 5);
#endif
      }
      else {
#if CANVAS_IMMEDIATE_REPAINT
         CLEAR(&event);
         event.area.x = 0;
         event.area.y = 0;
         event.area.width  = 10000;
         event.area.height = 10000;
         CGCanvas__expose_event((GtkWidget *)GTK_OBJECT(CGCanvas_NATIVE_DATA(this)->fixed),
                             &event, (gpointer) this);
#else
         gtk_widget_queue_draw(CGCanvas_NATIVE_DATA(this)->fixed);
#endif
      }
   }
   else {
      canvas_redraw_widget = CGCanvas_NATIVE_DATA(this)->fixed;
   }
#endif
}/*CGCanvas::NATIVE_queue_draw*/

/*>>>move static cursors to elsewhere*/
struct {
   GdkCursor *cursor_hand;
   GdkCursor *cursor_crosshair;
   GdkCursor *cursor_top_left_corner;
   GdkCursor *cursor_top_side;
   GdkCursor *cursor_top_right_corner;
   GdkCursor *cursor_right_side;
   GdkCursor *cursor_bottom_right_corner;
   GdkCursor *cursor_bottom_side;
   GdkCursor *cursor_bottom_left_corner;
   GdkCursor *cursor_left_side;
} gdk_static;

void CGCanvas::pointer_position_get(TPoint *position) {
    /*>>>implement*/
}/*CGCanvas::pointer_position_get*/

void CGCanvas::pointer_position_set(const TPoint *position) {
    /*>>>implement*/
}/*CGCanvas::pointer_position_set*/

void CGCanvas::pointer_cursor_set(EGPointerCursor pointer_cursor) {
//   static GdkCursor *gdk_cursor_hand;
   GdkCursor *gdk_cursor;

   if (!gdk_static.cursor_hand) {
      gdk_static.cursor_hand = gdk_cursor_new(GDK_HAND2);
      gdk_static.cursor_crosshair = gdk_cursor_new(GDK_CROSSHAIR);
      gdk_static.cursor_top_left_corner = gdk_cursor_new(GDK_TOP_LEFT_CORNER);
      gdk_static.cursor_top_side = gdk_cursor_new(GDK_TOP_SIDE);
      gdk_static.cursor_top_right_corner = gdk_cursor_new(GDK_TOP_RIGHT_CORNER);
      gdk_static.cursor_right_side = gdk_cursor_new(GDK_RIGHT_SIDE);
      gdk_static.cursor_bottom_right_corner = gdk_cursor_new(GDK_BOTTOM_RIGHT_CORNER);
      gdk_static.cursor_bottom_side = gdk_cursor_new(GDK_BOTTOM_SIDE);
      gdk_static.cursor_bottom_left_corner = gdk_cursor_new(GDK_BOTTOM_LEFT_CORNER);
      gdk_static.cursor_left_side = gdk_cursor_new(GDK_LEFT_SIDE);
   }

   if (this->pointer_cursor != pointer_cursor) {
      gdk_cursor = NULL;
      switch (pointer_cursor) {
      case EGPointerCursor.default:
         break;
      case EGPointerCursor.pointer:
         gdk_cursor = gdk_static.cursor_hand;
         break;
      case EGPointerCursor.crosshair:
         gdk_cursor = gdk_static.cursor_crosshair;
         break;
      case EGPointerCursor.move:
         break;
      case EGPointerCursor.resize_n:
         gdk_cursor = gdk_static.cursor_top_side;
         break;
      case EGPointerCursor.resize_s:
         gdk_cursor = gdk_static.cursor_bottom_side;
         break;
      case EGPointerCursor.resize_e:
         gdk_cursor = gdk_static.cursor_right_side;
         break;
      case EGPointerCursor.resize_w:
         gdk_cursor = gdk_static.cursor_left_side;
         break;
      case EGPointerCursor.resize_ne:
         gdk_cursor = gdk_static.cursor_top_right_corner;
         break;
      case EGPointerCursor.resize_sw:
         gdk_cursor = gdk_static.cursor_bottom_left_corner;
         break;
      case EGPointerCursor.resize_nw:
         gdk_cursor = gdk_static.cursor_top_left_corner;
         break;
      case EGPointerCursor.resize_se:
         gdk_cursor = gdk_static.cursor_bottom_right_corner;
         break;
      default:
         break;
      }

      if (CGCanvas_NATIVE_DATA(this)->wnd) {
         gdk_window_set_cursor(CGCanvas_NATIVE_DATA(this)->wnd, gdk_cursor);
      }

      this->pointer_cursor = pointer_cursor;
   }
}/*CGCanvas::pointer_cursor_set*/

void CGCanvas::pointer_capture(bool capture) {
   /* no need to do anything special, GDK grabs pointer for window
    * while button down */
}/*CGCanvas::pointer_capture*/

void CGCanvas::xor_write_set(bool xor_write) {
   gdk_gc_set_function(CGCanvas_NATIVE_DATA(this)->gc,
                       xor_write ? GDK_XOR : GDK_COPY);
   this->xor_write = xor_write;
}/*CGCanvas::xor_write_set*/

void CGCanvas::NATIVE_line_set(TGColour colour, bool fill) {
   GdkLineStyle line_style =
      ARRAY(&this->stroke_dasharray).count() ?
      GDK_LINE_ON_OFF_DASH : GDK_LINE_SOLID;
   GdkCapStyle cap_style;
   GdkJoinStyle join_style;
#if KEYSTONE_GTK_CAIRO
   /*>>>kludge, this should be held in canvas (this->storke_dasharray)*/
   int i;
   static double dasharray[100];
   cairo_line_cap_t linecap;
   cairo_line_join_t linejoin;
#endif

   colour = GCOL_CONVERT_RGB(this->palette, this->paletteB, colour);
#if KEYSTONE_GTK_CAIRO
   if (this->render_mode == EGCanvasRender.full) {
      cairo_set_source_rgba(CGCanvas_NATIVE_DATA(this)->cairo,
                           (double)GCOL_RGB_RED(colour) / 256,
                           (double)GCOL_RGB_GREEN(colour) / 256,
                           (double)GCOL_RGB_BLUE(colour) / 256, this->opacity);
      cairo_set_line_width(CGCanvas_NATIVE_DATA(this)->cairo, fill ? 0 : this->stroke_width);

      switch (this->stroke_linecap) {
      case EGStrokeLineCap.none:      
      case EGStrokeLineCap.butt:
         linecap = CAIRO_LINE_CAP_BUTT;
         break;
      case EGStrokeLineCap.round:
         linecap = CAIRO_LINE_CAP_ROUND;
         break;
      case EGStrokeLineCap.square:
         linecap = CAIRO_LINE_CAP_SQUARE;
         break;
      default:
         ASSERT("CGCanvas::NATIVE_line_set - bad cap style");
      }

      switch (this->stroke_linejoin) {
      case EGStrokeLineJoin.none:            
      case EGStrokeLineJoin.bevel:
         linejoin = CAIRO_LINE_JOIN_BEVEL;
         break;
      case EGStrokeLineJoin.miter:
         linejoin = CAIRO_LINE_JOIN_MITER;
         cairo_set_miter_limit(CGCanvas_NATIVE_DATA(this)->cairo, this->stroke_miterlimit);
         break;
      case EGStrokeLineJoin.round:
         linejoin = CAIRO_LINE_JOIN_ROUND;
         break;
      default:
         ASSERT("CGCanvas::NATIVE_line_set - bad cap style");
      }
      cairo_set_line_cap(CGCanvas_NATIVE_DATA(this)->cairo, linecap);
      cairo_set_line_join(CGCanvas_NATIVE_DATA(this)->cairo, linejoin);

      if (ARRAY(&this->stroke_dasharray).count()) {
         for (i = 0; i < ARRAY(&this->stroke_dasharray).count() && i < 100; i++) {
            dasharray[i] = ARRAY(&this->stroke_dasharray).data()[i];
         }
         cairo_set_dash(CGCanvas_NATIVE_DATA(this)->cairo, dasharray, ARRAY(&this->stroke_dasharray).count(), this->stroke_dashoffset);
      }
      else {
         cairo_set_dash(CGCanvas_NATIVE_DATA(this)->cairo, NULL, 0, 0);
      }
   }
#endif

   gdk_rgb_gc_set_foreground(CGCanvas_NATIVE_DATA(this)->gc, colour);

   switch (this->stroke_linecap) {
   default:   
   case EGStrokeLineCap.none:         
   case EGStrokeLineCap.butt:
      cap_style = GDK_CAP_BUTT;
      break;
   case EGStrokeLineCap.round:
      cap_style = GDK_CAP_ROUND;
      break;
   case EGStrokeLineCap.square:
      cap_style = GDK_CAP_PROJECTING;
      break;
   }

   switch (this->stroke_linejoin) {
   default:   
   case EGStrokeLineJoin.none:         
   case EGStrokeLineJoin.bevel:
      join_style = GDK_JOIN_BEVEL;
      break;
   case EGStrokeLineJoin.miter:
      join_style = GDK_JOIN_MITER;
      break;
   case EGStrokeLineJoin.round:
      join_style = GDK_JOIN_ROUND;
      break;
   }

   gdk_gc_set_line_attributes(CGCanvas_NATIVE_DATA(this)->gc,
                              fill ? 0 : (gint)this->stroke_width,
                              line_style, cap_style, join_style);
}/*CGCanvas::NATIVE_line_set*/

void CGCanvas::NATIVE_stroke_style_set(double stroke_width, ARRAY<int> *stroke_dasharray, int stroke_dashoffset,
                                       int stroke_miterlimit,
                                       EGStrokeLineCap stroke_linecap, EGStrokeLineJoin stroke_linejoin) {
   gint8 dash_list[20];
   int i;

   this->stroke_width     = stroke_width;
   this->stroke_dashoffset = stroke_dashoffset;
   this->stroke_miterlimit = stroke_miterlimit;
   this->stroke_linecap   = stroke_linecap;
   this->stroke_linejoin  = stroke_linejoin;

   if (stroke_dasharray) {
      ARRAY(&this->stroke_dasharray).copy(stroke_dasharray);
   }
   else {
      ARRAY(&this->stroke_dasharray).used_set(0);
   }
   if (stroke_dasharray && ARRAY(stroke_dasharray).count()) {
      for (i = 0; i < ARRAY(stroke_dasharray).count(); i++) {
         dash_list[i] = (gint8)ARRAY(stroke_dasharray).data()[i];
      }
      gdk_gc_set_dashes(CGCanvas_NATIVE_DATA(this)->gc,
                        stroke_dashoffset, &dash_list[0], ARRAY(stroke_dasharray).count());
   }
}/*CGCanvas::NATIVE_stroke_style_set*/

void CGCanvas::NATIVE_draw_point(TGColour colour, TCoord x, TCoord y) {
   CGCanvas(this).NATIVE_line_set(colour, FALSE);
   gdk_draw_point(CGCanvas_NATIVE_DATA(this)->wnd,
                  CGCanvas_NATIVE_DATA(this)->gc, (int)x, (int)y);
}/*CGCanvas::NATIVE_draw_point*/

void CGCanvas::NATIVE_draw_line(TGColour colour,
                                 TCoord x, TCoord y, TCoord x2, TCoord y2) {
   if (colour == GCOL_NONE)
     return;

   CGCanvas(this).NATIVE_line_set(colour, FALSE);
#if KEYSTONE_GTK_CAIRO
   if (this->render_mode == EGCanvasRender.full) {
      cairo_new_path(CGCanvas_NATIVE_DATA(this)->cairo);
      cairo_move_to(CGCanvas_NATIVE_DATA(this)->cairo, x, y);
      cairo_line_to(CGCanvas_NATIVE_DATA(this)->cairo, x2, y2);
      cairo_stroke(CGCanvas_NATIVE_DATA(this)->cairo);
      return;
   }
#endif
   gdk_draw_line(CGCanvas_NATIVE_DATA(this)->wnd,
                 CGCanvas_NATIVE_DATA(this)->gc, (int)x, (int)y, (int)x2, (int)y2);
}/*CGCanvas::NATIVE_draw_line*/

void CGCanvas::NATIVE_draw_rectangle(TGColour colour,
                                     bool fill, TCoord x, TCoord y, TCoord x2, TCoord y2) {
   TCoord width, height, i;

   if (x > x2) {
      i  = x;
      x  = x2;
      x2 = i;
   }
   if (y > y2) {
      i  = y;
      y  = y2;
      y2 = i;
   }

   width = x2 - x;
   height = y2 - y;

   if (fill) {
      width++;
      height++;
   }

   CGCanvas(this).NATIVE_line_set(colour, fill);
#if KEYSTONE_GTK_CAIRO
   if (this->render_mode == EGCanvasRender.full) {
      cairo_new_path(CGCanvas_NATIVE_DATA(this)->cairo);
      cairo_rectangle(CGCanvas_NATIVE_DATA(this)->cairo, x, y, x2 - x, y2 - y);

      if (fill) {
         cairo_fill(CGCanvas_NATIVE_DATA(this)->cairo);
      }
      else {
         cairo_stroke(CGCanvas_NATIVE_DATA(this)->cairo);
      }
      return;
  }
#endif
   gdk_draw_rectangle(CGCanvas_NATIVE_DATA(this)->wnd,
                      CGCanvas_NATIVE_DATA(this)->gc,
                      fill, (gint)x, (gint)y, (gint)width, (gint)height);
}/*CGCanvas::NATIVE_draw_rectangle*/

void CGCanvas::NATIVE_draw_ellipse(TGColour colour,
                                   bool fill, TCoord x, TCoord y, TCoord rx, TCoord ry) {
   CGCanvas(this).NATIVE_line_set(colour, fill);
#if KEYSTONE_GTK_CAIRO
   if (this->render_mode == EGCanvasRender.full) {
      cairo_new_path(CGCanvas_NATIVE_DATA(this)->cairo);
      cairo_save(CGCanvas_NATIVE_DATA(this)->cairo);
      cairo_translate(CGCanvas_NATIVE_DATA(this)->cairo, x, y);
      cairo_scale(CGCanvas_NATIVE_DATA(this)->cairo, rx, ry);
      cairo_arc(CGCanvas_NATIVE_DATA(this)->cairo, 0., 0., 1., 0, 360);
      cairo_restore(CGCanvas_NATIVE_DATA(this)->cairo);

      if (fill) {
         cairo_fill(CGCanvas_NATIVE_DATA(this)->cairo);
      }
      else {
         cairo_stroke(CGCanvas_NATIVE_DATA(this)->cairo);
      }
      return;
   }
#endif
   gdk_draw_arc(CGCanvas_NATIVE_DATA(this)->wnd,
                CGCanvas_NATIVE_DATA(this)->gc,
                fill, (int)(x - rx), (int)(y - ry), (int)(rx * 2), (int)(ry * 2), 0, 36000);
}/*CGCanvas::NATIVE_draw_ellipse*/

void CGCanvas::NATIVE_draw_polygon(TGColour colour,
                                   bool fill,
                                   int point_count, TPoint *point_data,
                                   bool final_connect) {
   GdkPoint *gdk_point;
   int i;

   if (point_count == 0)
      return;

   if (point_count > 1000)
     ASSERT("GOCANVAS_NATIVE_draw_polygon:point count too large");

   CGCanvas(this).NATIVE_line_set(colour, fill);

#if KEYSTONE_GTK_CAIRO
   if (this->render_mode == EGCanvasRender.full) {
      cairo_new_path(CGCanvas_NATIVE_DATA(this)->cairo);
      cairo_move_to(CGCanvas_NATIVE_DATA(this)->cairo, point_data[0].x, point_data[0].y);
      for (i = 0; i < point_count; i++) {
         cairo_line_to(CGCanvas_NATIVE_DATA(this)->cairo, point_data[i].x, point_data[i].y);
      }

      if (final_connect) {
         cairo_close_path(CGCanvas_NATIVE_DATA(this)->cairo);
      }

      if (fill) {
         cairo_fill(CGCanvas_NATIVE_DATA(this)->cairo);
      }
      else {
         cairo_stroke(CGCanvas_NATIVE_DATA(this)->cairo);
      }
      return;
   }
#endif
   gdk_point = CFramework(&framework).scratch_buffer_allocate();
   for (i = 0; i < point_count; i++) {
      gdk_point[i].x = (gint16) point_data[i].x;
      gdk_point[i].y = (gint16) point_data[i].y;
   }

   if (!fill && !final_connect) {
      gdk_draw_lines(CGCanvas_NATIVE_DATA(this)->wnd,
                     CGCanvas_NATIVE_DATA(this)->gc,
                     gdk_point, point_count);
   }
   else {
      gdk_draw_polygon(CGCanvas_NATIVE_DATA(this)->wnd,
                       CGCanvas_NATIVE_DATA(this)->gc,
                       fill, gdk_point, point_count);
   }
   CFramework(&framework).scratch_buffer_release(gdk_point);
}/*CGCanvas::NATIVE_draw_polygon*/

void CGCanvas::NATIVE_draw_path(TGColour colour, bool fill,
                                 int instruction_count, TGPathInstruction *instruction) {
   GdkPoint *p_point = NULL;
   EGPathInstruction ins_op, ins_last_op = EGPathInstruction.NoOp;
   TGPathInstruction *ip;
   int i, j;
   TPoint point, point_first = {0,0}, point_last, point_control_last = {0,0}, point_t[4];
   bool closepath = FALSE;

   if (colour == GCOL_NONE)
     return;

   CGCanvas(this).NATIVE_line_set(colour, fill);

   j = 0;

   if (this->render_mode == EGCanvasRender.full) {
#if KEYSTONE_GTK_CAIRO
      cairo_new_path(CGCanvas_NATIVE_DATA(this)->cairo);
#endif
   }
   else {
       p_point = CFramework(&framework).scratch_buffer_allocate();
   }
   point.x = 0;
   point.y = 0;
   point_last.x = 0;
   point_last.y = 0;
   for (i = 0; i < instruction_count; i++) {
      ip = &instruction[i];
      ins_op = instruction[i].instruction == EGPathInstruction.Repeat ? ins_last_op : instruction[i].instruction;
      ins_last_op = ins_op;
      switch (ins_op) {
      case EGPathInstruction.NoOp:
         break;
      case EGPathInstruction.MoveToAbs:
         point.x = ip->point[0].x;
         point.y = ip->point[0].y;
         goto path;
      case EGPathInstruction.MoveToRel:
         point.x += ip->point[0].x;
         point.y += ip->point[0].y;
         goto path;
      case EGPathInstruction.ClosePath:
      case EGPathInstruction.ClosePathAlias:
         closepath = TRUE;
         point = point_first;
#if KEYSTONE_GTK_CAIRO
         if (this->render_mode == EGCanvasRender.full) {
            cairo_close_path(CGCanvas_NATIVE_DATA(this)->cairo);
         }
#endif
         break;
      case EGPathInstruction.LineToAbs:
         point.x = ip->point[0].x;
         point.y = ip->point[0].y;
         goto point;
      case EGPathInstruction.LineToRel:
         point.x += ip->point[0].x;
         point.y += ip->point[0].y;
         goto point;
      case EGPathInstruction.HLineToAbs:
         point.x = ip->point[0].x;
         goto point;
      case EGPathInstruction.HLineToRel:
         point.x += ip->point[0].x;
         goto point;
      case EGPathInstruction.VLineToAbs:
         point.y = ip->point[0].y;
         goto point;
      case EGPathInstruction.VLineToRel:
         point.y += ip->point[0].y;
         goto point;
      path:
         point_first = point;
         closepath = FALSE;
         j = 0;
#if KEYSTONE_GTK_CAIRO
         if (this->render_mode == EGCanvasRender.full) {
            point_t[0] = point;
            CGCoordSpace(&this->coord_space).point_utov(&point_t[0]);
            cairo_move_to(CGCanvas_NATIVE_DATA(this)->cairo, point_t[0].x, point_t[0].y);
            break;
         }
#endif
      point:
         point_t[0] = point;
         CGCoordSpace(&this->coord_space).point_utov(&point_t[0]);
         if (this->render_mode == EGCanvasRender.full) {
#if KEYSTONE_GTK_CAIRO
            cairo_line_to(CGCanvas_NATIVE_DATA(this)->cairo, point_t[0].x, point_t[0].y);
#endif
         }
         else {
            p_point[j].x = (gint)point_t[0].x;
            p_point[j].y = (gint)point_t[0].y;
         }
         j++;
         break;
      case EGPathInstruction.CurveToAbs:
      case EGPathInstruction.QuadToAbs:
          point_t[0] = point_last;
          point_t[1] = ip->point[0];
          point_t[2] = ip->point[ins_op == EGPathInstruction.QuadToAbs ? 0 : 1];
          point_t[3] = ip->point[ins_op == EGPathInstruction.QuadToAbs ? 1 : 2];
          goto curve;
      case EGPathInstruction.CurveToRel:
      case EGPathInstruction.QuadToRel:
          point_t[0] = point_last;
          point_t[1].x = point_last.x + ip->point[0].x;
          point_t[1].y = point_last.y + ip->point[0].y;
          point_t[2].x = point_last.x + ip->point[ins_op == EGPathInstruction.QuadToRel ? 0 : 1].x;
          point_t[2].y = point_last.y + ip->point[ins_op == EGPathInstruction.QuadToRel ? 0 : 1].y;
          point_t[3].x = point_last.x + ip->point[ins_op == EGPathInstruction.QuadToRel ? 1 : 2].x;
          point_t[3].y = point_last.y + ip->point[ins_op == EGPathInstruction.QuadToRel ? 1 : 2].y;
          goto curve;
      case EGPathInstruction.CurveToShortAbs:
      case EGPathInstruction.QuadToShortAbs:
          point_t[0] = point_last;
          point_t[1].x = point_last.x + (point_last.x - point_control_last.x);
          point_t[1].y = point_last.y + (point_last.y - point_control_last.y);
          point_t[2] = ip->point[0];
          point_t[3] = ip->point[ins_op == EGPathInstruction.QuadToShortAbs ? 0 : 1];
          goto curve;
      case EGPathInstruction.CurveToShortRel:
      case EGPathInstruction.QuadToShortRel:
          point_t[0] = point_last;
          point_t[1].x = point_last.x + (point_last.x - point_control_last.x);
          point_t[1].y = point_last.y + (point_last.y - point_control_last.y);
          point_t[2].x = point_last.x + ip->point[0].x;
          point_t[2].y = point_last.y + ip->point[0].y;
          point_t[3].x = point_last.x + ip->point[ins_op == EGPathInstruction.QuadToShortRel ? 0 : 1].x;
          point_t[3].y = point_last.y + ip->point[ins_op == EGPathInstruction.QuadToShortRel ? 0 : 1].y;
          goto curve;
      curve:
          switch (ins_op) {
          case EGPathInstruction.QuadToAbs:
          case EGPathInstruction.QuadToRel:
          case EGPathInstruction.QuadToShortAbs:
          case EGPathInstruction.QuadToShortRel:
             point_control_last = point_t[1];
             /* Convert quadratic to cubic */
             point_t[2] = point_t[1];
             point_t[1].x = (point_t[0].x + point_t[1].x * 2) / 3;
             point_t[1].y = (point_t[0].y + point_t[1].y * 2) / 3;
             point_t[2].x = (point_t[3].x + point_t[2].x * 2) / 3;
             point_t[2].y = (point_t[3].y + point_t[2].y * 2) / 3;
             break;
          default:
             point_control_last = point_t[2];
             break;
          }
          point = point_t[3];
          CGCoordSpace(&this->coord_space).point_array_utov(4, point_t);
          if (this->render_mode == EGCanvasRender.full) {
#if KEYSTONE_GTK_CAIRO
             cairo_curve_to(CGCanvas_NATIVE_DATA(this)->cairo, point_t[1].x, point_t[1].y,
                                     point_t[2].x, point_t[2].y, point_t[3].x, point_t[3].y);
#endif
          }
          break;
      case EGPathInstruction.ArcToAbs:
      case EGPathInstruction.ArcToRel:
#if KEYSTONE_GTK_CAIRO
         if (this->render_mode == EGCanvasRender.full) {
            point_t[0] = point_last;
            if (ins_op == EGPathInstruction.ArcToRel) {
               point.x = point_last.x + ip->point[2].x;
                point.y = point_last.y + ip->point[2].y;
             }
             else {
                point = ip->point[2];
             }
             point_t[2] = point;

             CGCoordSpace(&this->coord_space).point_array_utov(3, point_t);
             point_t[1].x = ip->point[0].x * CGCoordSpace(&this->coord_space).scaling_x();
             point_t[1].y = ip->point[0].y * CGCoordSpace(&this->coord_space).scaling_y();
             rotation = ip->point[1].x + CGCoordSpace(&this->coord_space).rotation();

             CGCoordSpace(&this->coord_space).arc_position(point_t[0].x, point_t[0].y,
                                                           point_t[1].x, point_t[1].y,
                                                           rotation,
                                                           (((int)ip->point[1].y) & 1) ? 1 : 0,
                                                           (((int)ip->point[1].y) & 2) ? 1 : 0,
                                                           point_t[2].x, point_t[2].y,
                                                           &cx, &cy, &rx, &ry, &th0, &thd);
             /*>>>need to convert th0, thd to values in scaled coordinate system */
//printf("arc c=%g,%g r=%g,%g t=%g,%g rt=%g\n", cx, cy, rx, ry, th0, thd, ip->point[1].x);
             cairo_save(CGCanvas_NATIVE_DATA(this)->cairo);
             cairo_translate(CGCanvas_NATIVE_DATA(this)->cairo, cx, cy);
             cairo_rotate(CGCanvas_NATIVE_DATA(this)->cairo, /*ip->point[1].x*/ rotation * (PI / 180.0));
             cairo_scale(CGCanvas_NATIVE_DATA(this)->cairo, rx, ry);

             th0 = th0 * (PI / 180.0);
             thd = thd * (PI / 180.0);
             if (thd < 0) {
                cairo_arc_negative(CGCanvas_NATIVE_DATA(this)->cairo, 0, 0, 1, th0, th0 + thd);
             }
             else {
                cairo_arc(CGCanvas_NATIVE_DATA(this)->cairo, 0, 0, 1, th0, th0 + thd);
             }
             cairo_restore(CGCanvas_NATIVE_DATA(this)->cairo);
         }
#endif
         break;
      default:
         break;
      }
      point_last = point;
   }
#if KEYSTONE_GTK_CAIRO
   if (this->render_mode == EGCanvasRender.full) {
      if (fill) {
         cairo_set_fill_rule(CGCanvas_NATIVE_DATA(this)->cairo, CAIRO_FILL_RULE_EVEN_ODD);
         cairo_fill(CGCanvas_NATIVE_DATA(this)->cairo);
      }
      else {
         cairo_stroke(CGCanvas_NATIVE_DATA(this)->cairo);
      }
      return;
   }
#endif
   if (closepath || fill) {
      gdk_draw_polygon(CGCanvas_NATIVE_DATA(this)->wnd,
                       CGCanvas_NATIVE_DATA(this)->gc,
                       fill, p_point, j);
   }
   else {
      gdk_draw_lines(CGCanvas_NATIVE_DATA(this)->wnd,
                     CGCanvas_NATIVE_DATA(this)->gc,
                     p_point, j);
   }

   CFramework(&framework).scratch_buffer_release(p_point);
}/*CGCanvas::NATIVE_draw_path*/

void CGCanvas::NATIVE_font_set(CGFontFamily *font_family, int font_size,
                                 EGFontStyle font_style, EGFontWeight font_weight,
                                 EGTextDecoration text_decoration) {
   char xfont_desc[80];
   char *xfont_weight;
   char *xfont_name;
   char xfont_style;

   CString(&this->font_family).set(CString(font_family).string());;
   if (CGCanvas_NATIVE_DATA(this)->font_current) {
      CGCanvas_NATIVE_DATA(this)->font_current = NULL;
   }

   xfont_weight = font_weight == EGFontWeight.bold ? "bold" : "medium";
   xfont_style = font_style == EGFontStyle.italic ? 'i' : 'r';
#if 0
   switch (font_family) {
   case EGFontFamily.Default:
      return;
   case EGFontFamily.Courier:
      xfont_name = "courier";
      break;
   case EGFontFamily.TimesRoman:
      xfont_name = "times";
      break;
   case EGFontFamily.Arial:
      xfont_name = "helvetica";
      break;
   case EGFontFamily.Garamond:
      xfont_name = "utopia";
      break;
   case EGFontFamily.Lucida:
      xfont_name = "lucida";
      break;
   case EGFontFamily.Verdana:
      xfont_name = "new century schoolbook";
      break;
   }
#else
   xfont_name = "helvetica";
   if (!CString(font_family).length())
       return;
   if (CString(font_family).strcmp("Courier") == 0) {
      xfont_name = "courier";
   }
   else if (CString(font_family).strcmp("Times Roman") == 0) {
      xfont_name = "times";
   }
   else if (CString(font_family).strcmp("Arial") == 0) {
      xfont_name = "helvetica";
   }
   else if (CString(font_family).strcmp("Garamond") == 0) {
      xfont_name = "utopia";
   }
   else if (CString(font_family).strcmp("Lucida") == 0) {
      xfont_name = "lucida";
   }
   else if (CString(font_family).strcmp("Verdana") == 0) {
      xfont_name = "new century schoolbook";
   }
#endif

   /*>>>for now, crude conversion of font size from pixels to points */
   sprintf(xfont_desc, "-*-%s-%s-%c-*-*-%d-*-*-*-*-*-*-*",
      xfont_name, xfont_weight,
      xfont_style,
      font_size * 7 / 6);

   CGCanvas_NATIVE_DATA(this)->font_current =
      CXFontCache(&font_cache).load_font(xfont_desc);
}/*CGCanvas::NATIVE_font_set*/

void CGCanvas::NATIVE_text_align_set(EGTextAnchor anchor, EGTextBaseline baseline) {
   this->text_anchor = anchor;
   this->text_baseline = baseline;
}/*CGCanvas::NATIVE_text_align_set*/

void CGCanvas::NATIVE_draw_text(TGColour colour, TCoord x, TCoord y, const char *text) {
   gint x_lbearing, x_rbearing, x_width, x_ascent, x_descent;

   /*>>>hack!*/
   if (!CGCanvas_NATIVE_DATA(this)->font_default) {
      return;
   }

   CGCanvas(this).NATIVE_line_set(colour, FALSE);

   if (!CGCanvas_NATIVE_DATA(this)->font_current) {
      y += gdk_char_height(CGCanvas_NATIVE_DATA(this)->font_default, 'Q');
      gdk_draw_string(CGCanvas_NATIVE_DATA(this)->wnd,
                      CGCanvas_NATIVE_DATA(this)->font_default,
                      CGCanvas_NATIVE_DATA(this)->gc,
                      (int)x, (int)y, text);
      return;
   }

   gdk_string_extents(CGCanvas_NATIVE_DATA(this)->font_current,
                      text, &x_lbearing, &x_rbearing, &x_width,
                      &x_ascent, &x_descent);
   switch (this->text_anchor) {
   case EGTextAnchor.start:
      break;
   case EGTextAnchor.middle:
      x -= x_width >> 1;
      break;
   case EGTextAnchor.end:
      x -= x_width;
      break;
   }

   gdk_string_extents(CGCanvas_NATIVE_DATA(this)->font_current,
                      "Ag", &x_lbearing, &x_rbearing, &x_width,
                      &x_ascent, &x_descent);
   switch (this->text_baseline) {
   case EGTextBaseline.alphabetic:
      break;
   case EGTextBaseline.middle:
   case EGTextBaseline.central:
      y -= x_descent;
      y += (x_ascent + x_descent) >> 1;
      break;
   case EGTextBaseline.text_before_edge:
      y += x_ascent;
      break;
   case EGTextBaseline.text_after_edge:
      y -= x_descent;
      break;
   }

   gdk_draw_string(CGCanvas_NATIVE_DATA(this)->wnd,
                   CGCanvas_NATIVE_DATA(this)->font_current,
                   CGCanvas_NATIVE_DATA(this)->gc,
                   (int)x, (int)y, text);
}/*CGCanvas::NATIVE_draw_text*/

void CGCanvas::NATIVE_draw_text_string(TGColour colour, TCoord x, TCoord y, CString *text) {
   CGCanvas(this).NATIVE_draw_text(colour, x, y, CString(text).string());
}/*CGCanvas::NATIVE_draw_text_string*/

void CGCanvas::NATIVE_text_extent_get(TCoord x, TCoord y, const char *text, TRect *result) {
   gint x_lbearing, x_rbearing, x_width, x_ascent, x_descent;
   GdkFont *font;

   /*>>>hack!*/
   if (!CGCanvas_NATIVE_DATA(this)->font_default) {
      return;
   }

   if (CGCanvas_NATIVE_DATA(this)->font_current) {
      font = CGCanvas_NATIVE_DATA(this)->font_current;
   }
   else {
      font = CGCanvas_NATIVE_DATA(this)->font_default;
   }

   gdk_string_extents(font,
                      text, &x_lbearing, &x_rbearing, &x_width,
                      &x_ascent, &x_descent);
   x_width = (gint)((double)x_width / CGCoordSpace(&this->coord_space).scaling());
   switch (this->text_anchor) {
   case EGTextAnchor.start:
   default:
      break;
   case EGTextAnchor.middle:
      x -= x_width >> 1;
      break;
   case EGTextAnchor.end:
      x -= x_width;
      break;
   }
   result->point[0].x = x;
   result->point[1].x = x + x_width;

   gdk_string_extents(font,
                      "Ag", &x_lbearing, &x_rbearing, &x_width,
                      &x_ascent, &x_descent);
   x_ascent = (gint)((double)x_ascent / CGCoordSpace(&this->coord_space).scaling());
   x_descent = (gint)((double)x_descent / CGCoordSpace(&this->coord_space).scaling());
   if (CGCanvas_NATIVE_DATA(this)->font_current) {
      switch (this->text_baseline) {
      case EGTextBaseline.alphabetic:
         y -= x_ascent;
         break;
      case EGTextBaseline.middle:
      case EGTextBaseline.central:
         y -= (x_ascent + x_descent) >> 1;
         break;
      case EGTextBaseline.text_before_edge:
         break;
      case EGTextBaseline.text_after_edge:
         y -= x_descent + x_ascent;
         break;
      }
   }

   result->point[0].y = y;
   result->point[1].y = y + (x_ascent + x_descent);
}/*CGCanvas::NATIVE_text_extent_get*/

void CGCanvas::NATIVE_text_extent_get_string(TCoord x, TCoord y, CString *text, TRect *result) {
   CString tempstr;

   new(&tempstr).CString(NULL);
   CString(&tempstr).set_string(text);
   CGCanvas(this).NATIVE_text_extent_get(x, y, CString(&tempstr).string(), result);
   delete(&tempstr);
}/*CGCanvas::NATIVE_text_extent_get_string*/

void CGCanvas::NATIVE_draw_bitmap(CGBitmap *bitmap, TCoord x, TCoord y) {
#if KEYSTONE_GTK_CAIRO
    if (bitmap->image) {
       cairo_save(CGCanvas_NATIVE_DATA(this)->cairo);

       cairo_translate(CGCanvas_NATIVE_DATA(this)->cairo, x, y);
       cairo_set_source_surface(CGCanvas_NATIVE_DATA(this)->cairo, (cairo_surface_t *)bitmap->native_object, 0, 0);
       cairo_paint(CGCanvas_NATIVE_DATA(this)->cairo);

       cairo_restore(CGCanvas_NATIVE_DATA(this)->cairo);
       return;
    }
#endif

   gdk_draw_pixmap(CGCanvas_NATIVE_DATA(this)->wnd,
                   CGCanvas_NATIVE_DATA(this)->gc,
                   bitmap->native_object,
                   0, 0, (int)x, (int)y, bitmap->width, bitmap->height);
}/*CGCanvas::NATIVE_draw_bitmap*/

void CGCanvas::NATIVE_draw_bitmap_scaled(CGBitmap *bitmap, TCoord x, TCoord y, TCoord width, TCoord height) {
#if KEYSTONE_GTK_CAIRO
    if (bitmap->image) {
       if (!bitmap->width && !bitmap->height) {
           return;
       }

       cairo_save(CGCanvas_NATIVE_DATA(this)->cairo);

       cairo_translate(CGCanvas_NATIVE_DATA(this)->cairo, x, y);
       if (bitmap->width && bitmap->height) {
          cairo_scale(CGCanvas_NATIVE_DATA(this)->cairo, (double)width / bitmap->width, (double)height / bitmap->height);
       }
       cairo_set_source_surface(CGCanvas_NATIVE_DATA(this)->cairo, (cairo_surface_t *)bitmap->native_object, 0, 0);
       cairo_paint(CGCanvas_NATIVE_DATA(this)->cairo);

       cairo_restore(CGCanvas_NATIVE_DATA(this)->cairo);
       return;
    }
#endif

   /*>>>not implemented, just do the same as 'draw_bitmap' */
   gdk_draw_pixmap(CGCanvas_NATIVE_DATA(this)->wnd,
                   CGCanvas_NATIVE_DATA(this)->gc,
                   bitmap->native_object,
                   0, 0, (int)x, (int)y, bitmap->width, bitmap->height);
}/*CGCanvas::NATIVE_draw_bitmap_scaled*/

void CGCanvasBitmap::NATIVE_allocate(CGLayout *layout) {
   /*>>>gc allocation/deallocation should be done by CGCanvas::NATIVE_event*/
//   CGCanvas_NATIVE_DATA(this)->font_default =
//      gdk_font_load("-*-helvetica-bold-r-normal--12-120-*-*-*-*-iso8859-1");

//   CGCanvas_NATIVE_DATA(this)->wnd = (GdkWindow *)this->bitmap.native_object;
//   CGCanvas_NATIVE_DATA(this)->gc  =
//      gdk_gc_new((GdkWindow *)this->bitmap.native_object);
}/*CGCanvasBitmap::NATIVE_allocate*/

void CGCanvasBitmap::NATIVE_release(CGLayout *layout) {
   class:base.NATIVE_release(layout);

//   gdk_gc_destroy(CGCanvas_NATIVE_DATA(this)->gc);

//   CGCanvas_NATIVE_DATA(this)->wnd = NULL;
//   CGCanvas_NATIVE_DATA(this)->gc  = NULL;
}/*CGCanvasBitmap::NATIVE_release*/

void CGCanvasPrint::NATIVE_allocate(CGLayout *layout) {
}/*CGCanvasPrint::NATIVE_allocate*/

void CGCanvasPrint::NATIVE_release(CGLayout *layout) {
}/*CGCanvasPrint::NATIVE_release*/

bool CGCanvasPrint::NATIVE_output_clipboard(void) {
   return FALSE;
}/*CGCanvasPrint::NATIVE_output_clipboard*/

bool CGCanvasPrint::NATIVE_output_file(const char *filename) {
   return FALSE;
}/*CGCanvasPrint::NATIVE_output_file*/

bool CGCanvasPrint::NATIVE_output_choose_printer(void) {
   return FALSE;
}/*CGCanvasPrint::NATIVE_output_choose_printer*/

/* Dummy methods */
void CGCanvas::NATIVE_clip_set_rect(TRect *rect) {}
void CGCanvas::NATIVE_clip_reset(void) {}


/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
