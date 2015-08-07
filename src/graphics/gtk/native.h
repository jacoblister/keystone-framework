typedef struct {
   GdkWindow *wnd;
   GdkGC *gc;
   GdkFont *font_default;
   GdkFont *font_current;
   bool button_state;
   GtkWidget *fixed;
   GtkWidget *table;
   GtkWidget *hscroll, *vscroll;
   GtkAdjustment *hadjust, *vadjust;
#if KEYSTONE_GTK_CAIRO    
   cairo_t *cairo;    
#endif
} TGCanvasNative;

//#define CGCanvas_NATIVE_DATA(canvas) ((TGCanvasNative *)(&CGCanvas(canvas)->native_data))
#define CGCanvas_NATIVE_DATA(canvas) ((TGCanvasNative *)(&((CGCanvas *)canvas)->native_data))
