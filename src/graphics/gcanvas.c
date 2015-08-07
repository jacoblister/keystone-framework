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
#include "gobject.c"
#include "gxulelement.c"
#include "gcoord.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGCanvas;

class CGBitmap : CObjPersistent {
 private:
   bool image;
   int width, height;

   void *native_object;
   void *native_data;
   
   void delete(void);   
   void NATIVE_allocate(void);
   void NATIVE_release(void);

   void datalines_write(int line_start, int lines, void *data);
   void datalines_read(int line_start, int lines, void *data);
 public:
   void CGBitmap(void);
   void load_file(CGCanvas *canvas, const char *filename);
};

class CImageLink : CBinData {
 public:
   void CImageLink(void);
};

ATTRIBUTE:typedef<CImageLink>;

class CGImage : CGObject {
 private:
   bool loaded;
   CGBitmap bitmap;

   void new(void);
   void delete(void);

   void load_file(CGCanvas *canvas);
   void embed_image(void);
 public:
   ALIAS<"svg:image">;

   ATTRIBUTE<TCoord x>;
   ATTRIBUTE<TCoord y>;
   ATTRIBUTE<TCoord width> {
       if (data)
           this->width = *data;
       else
          this->width = this->bitmap.width - 1;
   };
   ATTRIBUTE<TCoord height> {
       if (data)
           this->height = *data;
       else
          this->height = this->bitmap.height - 1;
   };
   ATTRIBUTE<CImageLink imagelink, "xlink:href">;
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void extent_set(CGCanvas *canvas);
   virtual bool transform_assimilate(TGTransform *transform);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);

   void CGImage(void);
};

class CGGradientStop : CObjPersistent {
 public:
   ALIAS<"svg:stop">;
   ATTRIBUTE<double offset>;
   ATTRIBUTE<TGColour stop_colour, "stop-color">;

   void CGGradientStop(void);
};

ENUM:typedef<EGSpreadMethod> {
   {pad}, {reflect}, {repeat}
};

class CGLinearGradient : CObjPersistent {
 public:
   ALIAS<"svg:linearGradient">;
   ATTRIBUTE<TCoord x1>;
   ATTRIBUTE<TCoord y1>;
   ATTRIBUTE<TCoord x2>;
   ATTRIBUTE<TCoord y2>;
   ATTRIBUTE<EGSpreadMethod spread_method, "spreadMethod">;

   void CGLinearGradient(void);
};

ENUM:typedef<EGStrokeLineCap> {
   {butt}, {round}, {square}, {none}
};

ENUM:typedef<EGStrokeLineJoin> {
   {miter}, {round}, {bevel}, {none}
};

ENUM:typedef<EGPathInstruction> {
   {MoveToAbs,      "M"},
   {MoveToRel,      "m"},
   {ClosePath,      "Z"},
   {ClosePathAlias, "z"},
   {LineToAbs,      "L"},
   {LineToRel,      "l"},
   {HLineToAbs,     "H"},
   {HLineToRel,     "h"},
   {VLineToAbs,     "V"},
   {VLineToRel,     "v"},

   {CurveToAbs,     "C"},
   {CurveToRel,     "c"},
   {CurveToShortAbs,"S"},
   {CurveToShortRel,"s"},
   {QuadToAbs,      "Q"},
   {QuadToRel,      "q"},
   {QuadToShortAbs, "T"},
   {QuadToShortRel, "t"},
   {ArcToAbs,       "A"},
   {ArcToRel,       "a"},

   {Repeat,         " "},
   {NoOp,           "!"}
};

typedef struct {
   EGPathInstruction instruction;
   TPoint point[3];
} TGPathInstruction;

ARRAY:typedef<TGPathInstruction>;
ATTRIBUTE:typedef<TGPathInstruction>;

ENUM:typedef<EGPreserveAspectRatio> {
   {none},
   {xMinYMin},
   {xMidYMin},
   {xMaxYMin},
   {xMinYMid},
   {xMidYMid},
   {xMaxYMid},
   {xMinYMax},
   {xMidYMax},
   {xMaxYMax},
   {oneToOne},
};

ENUM:typedef<EGMeetOrSlice> {
   {meet},
   {slice}
};

typedef struct {
   EGPreserveAspectRatio aspect;
   EGMeetOrSlice meet_or_slice;
} TGPreserveAspectRatio;

ATTRIBUTE:typedef<TGPreserveAspectRatio>;

ENUM:typedef<EGPointerCursor> {
   {auto},
   {crosshair},
   {default},
   {pointer},
   {move},
   {resize_e,  "e-resize"},
   {resize_ne, "ne-resize"},
   {resize_nw, "nw-resize"},
   {resize_n,  "n-resize"},
   {resize_se, "se-resize"},
   {resize_sw, "sw-resize"},
   {resize_s,  "s-resize"},
   {resize_w,  "w-resize"},
   {text},
   {wait},
   {help},
};

class CGWindow;

ENUM:typedef<EGCanvasRender> {
   {basic}, {full}
};

ENUM:typedef<EGCanvasScroll> {
   {normal}, {none}, {follow}
};

#define NSIZE (sizeof(void*)*16)

class CGCanvas : CGXULElement {
 private:
   CGCoordSpace coord_space;
   CGPalette *palette;
   CGPalette *paletteB;
   int enter_count;
   bool xor_write;
   bool in_draw;
   bool translate_palette;
   bool queue_draw_disable;
   bool native_redraw_immediate;
   bool native_redraw_show;
   bool component_reposition;
   EGCanvasScroll scrollMode;
   bool oem_font;
   TGColour native_erase_background_colour;
   TPoint pointer_position;
   EGPointerCursor pointer_cursor;
   byte native_data[NSIZE];
   EGCanvasRender render_mode_canvas;
   EGCanvasRender render_mode;

   double opacity;

   double stroke_width;
   ARRAY<int> stroke_dasharray;
   int stroke_dashoffset;
   int stroke_miterlimit;
   EGStrokeLineCap stroke_linecap;
   EGStrokeLineJoin stroke_linejoin;

   EGTextAnchor text_anchor;
   EGTextBaseline text_baseline;
   CGFontFamily font_family;
   EGTextDecoration text_decoration;

   int resize_width, resize_height;
   int content_width, content_height;           /* Canvas content size, local coordinates */
   int allocated_width, allocated_height;       /* Allocated canvas size, pixels */
   int scroll_width, scroll_height;             /* Visible scrollable area, pixels */
   TPoint view_origin;

   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);

   void draw_gobject_transformed(CGCanvas *canvas, CGObject *gobject, TRect *extent, EGObjectDrawMode mode);
   void point_array_utov(int count, TPoint *point);
   void point_array_vtou(int count, TPoint *point);
   void point_ctos(TPoint *point);
   void point_stoc(TPoint *point);
   void view_update(bool resize);
   void transform_viewbox_calculate(TRect *viewbox_src, TRect *viewbox_dest, TGPreserveAspectRatio preserve,
                                    TGTransform *result_scale, TGTransform *result_translate);
   void transform_viewbox(void);
   void transform_set_gobject(CGObject *gobject, bool viewbox);
   void transform_prepend_gobject(CGObject *gobject);
   virtual void redraw(TRect *extent, EGObjectDrawMode mode);
   void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   void resize_request(int x, int y, int width, int height);

   void component_reposition(CObjPersistent *object);

   void NATIVE_enter(bool enter);
   void NATIVE_allocate(CGLayout *layout);
   void NATIVE_release(CGLayout *layout);
   void NATIVE_show(bool show, bool end);
   void NATIVE_size_allocate(void);

   /* Native drawing methods*/
   /*>>>Private, internal methods, actually static inline */
   void NATIVE_line_set(TGColour colour, bool fill);
   void NATIVE_fill_set(TGColour colour, bool fill);

   void NATIVE_stroke_style_set(double stroke_width, ARRAY<int> *stroke_dasharray, int stroke_dashoffset,
                                int stroke_miterlimit,
                                EGStrokeLineCap stroke_linecap, EGStrokeLineJoin stroke_linejoin);
   void NATIVE_queue_draw(const TRect *extent);
   void NATIVE_clip_set_rect(TRect *rect);
   void NATIVE_clip_reset(void);
   void NATIVE_draw_point(TGColour colour, TCoord x, TCoord y);
   void NATIVE_draw_line(TGColour colour, TCoord x, TCoord y, TCoord x2, TCoord y2);
   void NATIVE_draw_rectangle(TGColour colour, bool fill, TCoord x, TCoord y, TCoord x2, TCoord y2);
   void NATIVE_draw_ellipse(TGColour colour, bool fill, TCoord x, TCoord y, TCoord rx, TCoord ry);
   void NATIVE_draw_polygon(TGColour colour, bool fill,
                            int point_count, TPoint *point_data, bool closed);
   void NATIVE_draw_path(TGColour colour, bool fill,
                         int instruction_count, TGPathInstruction *instruction);
   void NATIVE_font_set(CGFontFamily *font_family, int font_size,
                        EGFontStyle font_style, EGFontWeight font_weight,
                        EGTextDecoration text_decoration);
   void NATIVE_text_align_set(EGTextAnchor anchor, EGTextBaseline baseline);
   void NATIVE_text_extent_get(TCoord x, TCoord y, const char *text, TRect *result);
   void NATIVE_text_extent_get_string(TCoord x, TCoord y, CString *text, TRect *result);
   void NATIVE_draw_text(TGColour colour, TCoord x, TCoord y, const char *text);
   void NATIVE_draw_text_string(TGColour colour, TCoord x, TCoord y, CString *text);
   void NATIVE_draw_bitmap(CGBitmap *bitmap, TCoord x, TCoord y);
   void NATIVE_draw_bitmap_scaled(CGBitmap *bitmap, TCoord x, TCoord y, TCoord width, TCoord height);
//   void NATIVE_draw_bitmap_section(CGBitmap *bitmap, int x, int y,
//                                   int src_x, int src_y, int width, int height);
 public:
   void new(void);
   void CGCanvas(int width, int height);
   void delete(void);

   static inline void render_set(EGCanvasRender render);
   static inline void render_restore(void);

   ALIAS<"svg:canvas">;

   ATTRIBUTE<TGColour colour_background, "bgcolor"> {
   if (data)
      this->colour_background = *data;
   else
      this->colour_background = CGCanvas(this).colour_background_default();
   };
   ATTRIBUTE<int zoom> {
      /*>>>move to window and pass value down through method in canvas */
      if (data)
         this->zoom = *data;
      else
         this->zoom = 100;

      CGCanvas(this).NATIVE_size_allocate();
//      this->view_origin.x = 0;
//      this->view_origin.y = 0;
   };
   ATTRIBUTE<double yAspect> {
      /*>>>move to window and pass value down through method in canvas */
      if (data)
         this->yAspect = *data;
//      else
//         this->yAspect = 1;
      CGCanvas(this).NATIVE_size_allocate();
   };
   ATTRIBUTE<TRect viewBox>;
   ATTRIBUTE<TGPreserveAspectRatio preserveAspectRatio> {
      if (data) {
         this->preserveAspectRatio = *data;
      }
      else {
         this->preserveAspectRatio.aspect = EGPreserveAspectRatio.xMidYMid;
         this->preserveAspectRatio.meet_or_slice = EGMeetOrSlice.meet;
      }
   };

//   virtual void show(bool show);

   static inline void palette_set(CGPalette *palette);
   static inline void palette_B_set(CGPalette *palette);

   static inline void queue_draw_disable(bool disable);
   static inline void queue_draw(const TRect *extent);
   void xor_write_set(bool xor_write);

   void colour_background_set(TGColour colour);
   TGColour colour_background_default(void);

   void pointer_position_get(TPoint *position);
   void pointer_position_set(const TPoint *position);
   void pointer_cursor_set(EGPointerCursor pointer_cursor);
   void pointer_capture(bool capture);

//   void CTM_object_set(CGObject *object);
   static inline void draw_point(TGColour colour, TCoord x, TCoord y);
   static inline void draw_line(TGColour colour, TCoord x, TCoord y, TCoord x2, TCoord y2);
   static inline void draw_rectangle(TGColour colour, bool fill, TCoord x, TCoord y, TCoord x2, TCoord y2);
   static inline void draw_ellipse(TGColour colour, bool fill, TCoord x, TCoord y, TCoord rx, TCoord ry);
   static inline void draw_text(TGColour colour, TCoord x, TCoord y, const char *text);
   static inline void draw_text_string(TGColour colour, TCoord x, TCoord y, CString *text);
   static inline void opacity_set(double opacity);
   static inline void stroke_style_set(TCoord stroke_width, ARRAY<int> *stroke_dasharray, int stroke_dashoffset,
                                         int stroke_miterlimit,
                                         EGStrokeLineCap stroke_linecap, EGStrokeLineJoin stroke_linejoin);
   static inline void font_set(CGFontFamily *font_family, int font_size,
                               EGFontStyle font_style, EGFontWeight font_weight,
                               EGTextDecoration text_decoration);

   void draw_polygon(TGColour colour, bool fill,
                     int point_count, TPoint *point_data, bool closed);
   EXCEPTION<BadObject>;
};

static inline void CGCanvas::render_set(EGCanvasRender render) {
    if (!this->enter_count) {
       this->render_mode_canvas = render;
    }
    this->render_mode = render;
}

static inline void CGCanvas::render_restore(void) {
   this->render_mode = this->render_mode_canvas;
}

static inline void CGCanvas::palette_set(CGPalette *palette) {
   this->palette = palette;
}

static inline void CGCanvas::palette_B_set(CGPalette *palette) {
   this->paletteB = palette;
}

/* Drawing */
static inline void CGCanvas::queue_draw_disable(bool disable) {
   this->queue_draw_disable = disable;
}

static inline void CGCanvas::queue_draw(const TRect *extent) {
   if (!this->in_draw && !this->queue_draw_disable) {
      CGCanvas(this).NATIVE_queue_draw(extent);
   }
}

static inline void CGCanvas::draw_point(TGColour colour,
                                          TCoord x, TCoord y) {
   TPoint point;
   point.x = x;
   point.y = y;

   CGCoordSpace(&this->coord_space).point_array_utov(1, &point);
   CGCanvas(this).NATIVE_draw_point(colour, point.x, point.y);
}

static inline void CGCanvas::draw_line(TGColour colour,
                                         TCoord x, TCoord y, TCoord x2, TCoord y2) {
   TPoint point[2];
   point[0].x = x;
   point[0].y = y;
   point[1].x = x2;
   point[1].y = y2;

   CGCoordSpace(&this->coord_space).point_array_utov(2, point);
   CGCanvas(this).NATIVE_draw_line(colour,
                                    point[0].x, point[0].y,
                                    point[1].x, point[1].y);
}

static inline void CGCanvas::draw_rectangle(TGColour colour, bool fill,
                                               TCoord x, TCoord y, TCoord x2, TCoord y2) {
   TPoint point[4];

   if (colour == GCOL_NONE)
      return;

   point[0].x = x;
   point[0].y = y;
   point[1].x = x2;
   point[1].y = y;
   point[2].x = x2;
   point[2].y = y2;
   point[3].x = x;
   point[3].y = y2;

   CGCoordSpace(&this->coord_space).point_array_utov(4, point);

   if (point[0].y == point[1].y &&
       point[1].x == point[2].x &&
       point[2].y == point[3].y &&
       point[3].x == point[0].x) {
          CGCanvas(this).NATIVE_draw_rectangle(colour, fill,
                                                point[0].x, point[0].y,
                                                point[2].x, point[2].y);
   }
   else {
      CGCanvas(this).NATIVE_draw_polygon(colour, fill,
                                          4, point, TRUE);
   }
}

static inline void CGCanvas::draw_ellipse(TGColour colour, bool fill,
                                          TCoord x, TCoord y, TCoord rx, TCoord ry) {
   TPoint point[2];

   if (colour == GCOL_NONE)
      return;
   point[0].x = x;
   point[0].y = y;
   point[1].x = x + rx;
   point[1].y = y + ry;

   CGCoordSpace(&this->coord_space).point_array_utov(2, point);
   point[1].x -= point[0].x;
   point[1].y -= point[0].y;
   CGCanvas(this).NATIVE_draw_ellipse(colour, fill,
                                      point[0].x, point[0].y,
                                      point[1].x, point[1].y);
}

static inline void CGCanvas::draw_text(TGColour colour, TCoord x, TCoord y, const char *text) {
   TPoint point;

   if (colour == GCOL_NONE)
      return;

   point.x = x;
   point.y = y;

   CGCoordSpace(&this->coord_space).point_utov(&point);
   CGCanvas(this).NATIVE_draw_text(colour, point.x, point.y, text);
}

static inline void CGCanvas::draw_text_string(TGColour colour, TCoord x, TCoord y, CString *text) {
   TPoint point;

   if (colour == GCOL_NONE)
      return;

   point.x = x;
   point.y = y;

   CGCoordSpace(&this->coord_space).point_utov(&point);
   CGCanvas(this).NATIVE_draw_text_string(colour, point.x, point.y, text);
}

static inline void CGCanvas::opacity_set(double opacity) {
    this->opacity = opacity;
}/*CGCanvas::opacity_set*/

static inline void CGCanvas::stroke_style_set(TCoord stroke_width, ARRAY<int> *stroke_dasharray, int stroke_dashoffset,
                                              int stroke_miterlimit,
                                              EGStrokeLineCap stroke_linecap, EGStrokeLineJoin stroke_linejoin) {
   double scale_stroke_width;

   scale_stroke_width = /*(int)*/(stroke_width * CGCoordSpace(&this->coord_space).scaling() + 0.5);
   /*>>>scale dash array too*/
   CGCanvas(this).NATIVE_stroke_style_set(scale_stroke_width, stroke_dasharray, stroke_dashoffset,
                                          stroke_miterlimit, stroke_linecap, stroke_linejoin);
}/*CGCanvas::stroke_style_set*/

static inline void CGCanvas::font_set(CGFontFamily *font_family, int font_size,
                                      EGFontStyle font_style, EGFontWeight font_weight,
                                      EGTextDecoration text_decoration) {
   int scale_font_size;

   CGCoordSpace(&this->coord_space).scaling_recalc();
   scale_font_size = (int)((double)font_size * CGCoordSpace(&this->coord_space).scaling());

   CGCanvas(this).NATIVE_font_set(font_family, scale_font_size, font_style, font_weight, text_decoration);
}/*CGCanvas::font_set*/

class CGCanvasBitmap : CGCanvas {
 private:
   CGBitmap bitmap;
   void delete(void);
 public:
   void CGCanvasBitmap(int width, int height);
   void resize(int width, int height);

   void NATIVE_allocate(CGLayout *layout);
   void NATIVE_release(CGLayout *layout);
};

ENUM:typedef<EGCanvasPrintOutput> {
   {printer}, {clipboard}, {file}
};

ENUM:typedef<EGCanvasPrintOrientation> {
   {none}, {portrait}, {landscape}
};

class CGCanvasPrint : CGCanvas {
 private:
   void new(void);
   void delete(void);
   CGCanvas *src_canvas;
   CGWindow *parent_window;
   EGCanvasPrintOutput output_mode;
 public:
   ATTRIBUTE<CString printer>;
   ATTRIBUTE<CString outfile>;
   ATTRIBUTE<EGCanvasPrintOrientation orientation>;
   void CGCanvasPrint(CGCanvas *src, bool copy_child, CGWindow *parent_window);
   bool NATIVE_output_file(const char *filename);
   bool NATIVE_output_clipboard(void);
   bool NATIVE_output_choose_printer(void);
   void print_document(void);

   void NATIVE_allocate(CGLayout *layout);
   void NATIVE_release(CGLayout *layout);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

bool GCOL_PALETTE_FIND(CObjPersistent *object, CGPalette **palette, CGPalette **paletteB, bool *translate) {
   CGCanvas *canvas = CGCanvas(CObject(object).parent_find(&class(CGCanvas)));

   if (canvas) {
      *palette = canvas->palette;
      *paletteB = canvas->paletteB;
      *translate = canvas->translate_palette;
      return TRUE;
   }
   return FALSE;
}/*GCOL_PALETTE_FIND*/

#include "../file.c"

void CImageLink::CImageLink(void) {
   /*>>>kludge, no_expand should be defined along with attribute declaration */
   (&ATTRIBUTE:type<CImageLink>)->no_expand = TRUE;

   CBinData(this).CBinData();
};

bool ATTRIBUTE:convert<CImageLink>(CObjPersistent *object,
                                    const TAttributeType *dest_type, const TAttributeType *src_type,
                                    int dest_index, int src_index,
                                    void *dest, const void *src) {
   int value;

   if (dest_type == &ATTRIBUTE:type<CImageLink> && src_type == NULL) {
      return FALSE;
   }
   if (dest_type == &ATTRIBUTE:type<CImageLink> && src_type == &ATTRIBUTE:type<CImageLink>) {
      return FALSE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CImageLink>) {
      return FALSE;
   }
   /*>>>probably call the base class */

   if (dest_type == &ATTRIBUTE:type<CImageLink>) {
      dest_type = &ATTRIBUTE:type<CBinData>;
      if (dest_index != -1) {
         value = 0;
         if (src_type == &ATTRIBUTE:type<int>) {
             value = (*(int *)src);
         }
         else if (src_type == &ATTRIBUTE:type<CString>) {
             value = CString((void *)src).string()[0] == 'T';
         }
         if (value != 0) {
            CGImage(object).embed_image();
         }
         return TRUE;
      }
   }
   if (src_type == &ATTRIBUTE:type<CImageLink>) {
      src_type = &ATTRIBUTE:type<CBinData>;
      if (src_index != -1 && dest_type == &ATTRIBUTE:type<int>) {
          (*(int *)dest) = ARRAY(&CBinData((void *)src)->data).count();
          return TRUE;
      }
   }

   return ATTRIBUTE:convert<CBinData>(object, dest_type, src_type, dest_index, src_index, dest, src);
}

void CGFontFamily::CGFontFamily(void) {
    CString(this).CString(NULL);
}/*CGFontFamily::CGFontFamily*/

bool ATTRIBUTE:convert<CGFontFamily>(CObjPersistent *object,
                                      const TAttributeType *dest_type, const TAttributeType *src_type,
                                      int dest_index, int src_index,
                                      void *dest, const void *src) {
   const char *cp, *cpe, c = '\'';

   if (dest_type == &ATTRIBUTE:type<CGFontFamily> && src_type == NULL) {
      memset(dest, 0, sizeof(CGFontFamily));
      new(dest).CGFontFamily();
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CGFontFamily>) {
      delete(src);
      return TRUE;
   }

   if (src_type == &ATTRIBUTE:type<CGFontFamily>) {
      src_type = &ATTRIBUTE:type<CString>;
   }
   if (dest_type == &ATTRIBUTE:type<CGFontFamily>) {
      if (src_type == &ATTRIBUTE:type<CString>) {
         /* Convert multi part string */
         cp = CString((void *)src).strchr(c);
         if (cp) {
            cpe = strchr(cp + 1, c);
            if (cp && cpe) {
               CString((void *)src).extract(dest, cp + 1, cpe - 1);
               return TRUE;
            }
         }
      }
      dest_type = &ATTRIBUTE:type<CString>;
   }

   return ATTRIBUTE:convert<CString>(object, dest_type, src_type, dest_index, src_index, dest, src);
}

void CGXULElement::extent_set(CGCanvas *canvas) {
   TPoint point[4];
   point[0].x = this->x;
   point[0].y = this->y;
   point[1].x = this->x + this->width;
   point[1].y = this->y;
   point[2].x = this->x + this->width;
   point[2].y = this->y + this->height;
   point[3].x = this->x;
   point[3].y = this->y + this->height;

   CGCoordSpace(&canvas->coord_space).point_array_utov(4, point);
   CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);
   CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 4, point);
}/*CGXULElement::extent_set*/

bool ATTRIBUTE:convert<TGPreserveAspectRatio>(CObjPersistent *object,
                                              const TAttributeType *dest_type, const TAttributeType *src_type,
                                              int dest_index, int src_index,
                                              void *dest, const void *src) {
   TGPreserveAspectRatio *value;
   CString *string;
   CString element;
   bool result;

   if (dest_type == &ATTRIBUTE:type<TGPreserveAspectRatio> && src_type == NULL) {
      memset(dest, 0, sizeof(TGPreserveAspectRatio));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TGPreserveAspectRatio> && src_type == &ATTRIBUTE:type<TGPreserveAspectRatio>) {
      *((TGPreserveAspectRatio *)dest) = *((TGPreserveAspectRatio *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TGPreserveAspectRatio>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TGPreserveAspectRatio> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (TGPreserveAspectRatio *)dest;
      string = (CString *)src;

      result = TRUE;
      if (dest_index == -1) {
         new(&element).CString(NULL);
         CString(string).tokenise(&element, " ", TRUE);
         value->aspect = ENUM:decode(EGPreserveAspectRatio, CString(&element).string());
         if (value->aspect == -1) {
            result = FALSE;
         }
         if (CString(string).tokenise(&element, " ", TRUE)) {
            value->meet_or_slice = ENUM:decode(EGMeetOrSlice, CString(&element).string());
            if (value->meet_or_slice == -1) {
               result = FALSE;
            }
         }
         else {
            value->meet_or_slice = EGMeetOrSlice.meet;
         }
         delete(&element);
      }
      else {
         switch(dest_index) {
         case 0:
            value->aspect = ENUM:decode(EGPreserveAspectRatio, CString(string).string());
            if (value->aspect == -1) {
               result = FALSE;
            }
            break;
         case 1:
            value->meet_or_slice = ENUM:decode(EGMeetOrSlice, CString(string).string());
            if (value->meet_or_slice == -1) {
               result = FALSE;
            }
            break;
         default:
            result = FALSE;
         }
      }

      return result;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TGPreserveAspectRatio>) {
      value  = (TGPreserveAspectRatio *)src;
      string = (CString *)dest;

      if (src_index == -1) {
         CString(string).printf("%s %s",
                                EGPreserveAspectRatio(value->aspect).name(),
                                EGMeetOrSlice(value->meet_or_slice).name());
      }
      else {
         switch(src_index) {
         case 0:
            CString(string).set(EGPreserveAspectRatio(value->aspect).name());
            break;
         case 1:
            CString(string).set(EGMeetOrSlice(value->meet_or_slice).name());
            break;
         default:
            return FALSE;
         }
      }
      return TRUE;
   }
   return FALSE;
}

void CGBitmap::CGBitmap(void) {
}/*CGBitmap::CGBitmap*/

void CGBitmap::delete(void) {
   CGBitmap(this).NATIVE_release(void);
}/*CGBitmap::delete*/

void CGImage::new(void) {
   class:base.new();

   new(&this->imagelink).CImageLink();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGImage,width>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGImage,height>, TRUE);
}/*CGImage::new*/

void CGImage::CGImage(void) {
}/*CGImage::CGImage*/

void CGImage::delete(void) {
   delete(&this->imagelink);

   if (this->loaded) {
      delete(&this->bitmap);
   }

   class:base.delete();
}/*CGImage::delete*/

void CGImage::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TPoint point[2];

   point[0].x = this->x;
   point[0].y = this->y;
   point[1].x = this->x + this->width;
   point[1].y = this->y + this->height;

   CGCoordSpace(&canvas->coord_space).point_array_utov(2, point);

   switch (mode) {
   case EGObjectDrawMode.Draw:
      CGCanvas(canvas).NATIVE_draw_bitmap_scaled(&this->bitmap, (int)point[0].x, (int)point[0].y,
                                                 (int)point[1].x - (int)point[0].x, (int)point[1].y - (int)point[0].y);
      break;
   default:
      break;
   }
}/*CGImage::draw*/

void CGImage::extent_set(CGCanvas *canvas) {
   TPoint point[4];
   point[0].x = this->x;
   point[0].y = this->y;
   point[1].x = this->x + this->width;
   point[1].y = this->y;
   point[2].x = this->x + this->width;
   point[2].y = this->y + this->height;
   point[3].x = this->x;
   point[3].y = this->y + this->height;

   CGCoordSpace(&canvas->coord_space).point_array_utov(4, point);
   CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);
   CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 4, point);
}/*CGImage::extent_set*/

bool CGImage::transform_assimilate(TGTransform *transform) {
   switch (transform->type) {
   case EGTransform.scale:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGImage,x>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGImage,y>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGImage,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGImage,height>);
      this->x = this->x * transform->t.scale.sx;
      this->y = this->y * transform->t.scale.sy;
      this->width = this->width * transform->t.scale.sx;
      this->height = this->height * transform->t.scale.sy;
      CObjPersistent(this).attribute_update_end();
      return TRUE;
   case EGTransform.translate:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGImage,x>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGImage,y>);
      this->x += transform->t.translate.tx;
      this->y += transform->t.translate.ty;
      CObjPersistent(this).attribute_update_end();
      return TRUE;
   default:
      break;
   }
   return FALSE;
}/*CGImage::transform_assimilate*/

void CGImage::load_file(CGCanvas *canvas) {
   CFile file;
   int count;

   if (!CString(&CBinData(&this->imagelink)->header).length())
      return;

   if (this->loaded) {
      delete(&this->bitmap);
   }

   new(&this->bitmap).CGBitmap();
   this->bitmap.width = (int)this->width;
   this->bitmap.height = (int)this->height;

   count = ARRAY(&CBinData(&this->imagelink)->data).count();
   if (count) {
      /* embedded image, write out to file and read back in
         >>>use native stream interface instead */
      new(&file).CFile();
      CIOObject(&file).open("$image$.$$$", O_CREAT | O_TRUNC | O_RDWR | O_BINARY);
      CIOObject(&file).write(ARRAY(&CBinData(&this->imagelink)->data).data(), ARRAY(&CBinData(&this->imagelink)->data).count());
      CIOObject(&file).close();
      delete(&file);

      CGBitmap(&this->bitmap).load_file(canvas, "$image$.$$$");

      remove("$image$.$$$");
   }
   else {
      CGBitmap(&this->bitmap).load_file(canvas, CString(&CBinData(&this->imagelink)->header).string());
   }

   if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGImage,width>)) {
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGImage,width>, TRUE);
   };
   if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGImage,height>)) {
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGImage,height>, TRUE);
   };
   this->loaded = TRUE;
}/*CGImage::load_file*/

void CGImage::embed_image(void) {
   CFile file;
   int length;
   char *cp;

   new(&file).CFile();
   CIOObject(&file).open(CString(&CBinData(&this->imagelink)->header).string(), O_RDONLY | O_BINARY);
   cp = CString(&CBinData(&this->imagelink)->header).strchr('.');
   if (cp) {
      cp++;
   }
   else {
      cp = "???";
   }
   CString(&CBinData(&this->imagelink)->header).printf("data:image/%s;base64", cp);

   length = CIOObject(&file).length();
   ARRAY(&CBinData(&this->imagelink)->data).used_set(length);
   CIOObject(&file).read_data(&CBinData(&this->imagelink)->data, length);

   CIOObject(&file).close();
}/*CGImage::embed_image*/

void CGImage::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGCanvas *canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
   static TRect extent; /*>>>hack, get last position from object server*/

   if (!changing) {
      if (canvas) {
         CGImage(this).load_file(canvas);
      }
   }

   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);

   if (canvas) {
      if (changing) {
         CGCanvas(canvas).transform_set_gobject(CGObject(this), FALSE);
         CGObject(this).extent_set(CGCanvas(canvas));
         memcpy(&extent, &CGObject(this)->extent, sizeof(TRect));
      }
      else {
//>>>?         CGCanvas(layout).queue_draw(&extent);
         CGCanvas(canvas).transform_set_gobject(CGObject(this), FALSE);
         CGObject(this).extent_set(CGCanvas(canvas));

         CGCoordSpace(&CGCanvas(canvas)->coord_space).extent_add(&extent, 1, &CGObject(this)->extent.point[0]);
         CGCoordSpace(&CGCanvas(canvas)->coord_space).extent_add(&extent, 1, &CGObject(this)->extent.point[1]);

         CGCoordSpace(&CGCanvas(canvas)->coord_space).reset();
         CGCanvas(canvas).transform_viewbox();
         CGCanvas(canvas).point_array_utov(2, extent.point);
         CGCanvas(canvas).queue_draw(&extent);
      }
   }
}/*CGImage::notify_attribute_update*/

void CGImage::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGCanvas *canvas;

   if (linkage == EObjectLinkage.ParentSet) {
      canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
      if (canvas) {
         CGImage(this).load_file(canvas);
      }
   }

   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);
}/*CGImage::notify_object_linkage*/

void CGGradientStop::CGGradientStop(void) {
}/*CGGradientStop::CGGradientStop*/

void CGLinearGradient::CGLinearGradient(void) {
}/*CGLinearGradient::CGLinearGradient*/

void CGCanvas::new(void) {
   new(&this->coord_space).CGCoordSpace();
   ARRAY(&this->stroke_dasharray).new();
   new(&this->font_family).CGFontFamily();
   this->opacity = 1.0;

   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,colour_background>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,preserveAspectRatio>, TRUE);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,viewBox>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,zoom>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,yAspect>, TRUE);
   this->yAspect = 1;
}/*CGCanvas::new*/

void CGCanvas::CGCanvas(int width, int height) {
   CGXULElement(this).CGXULElement(0, 0, width, height);
   this->colour_background = GCOL_WHITE;
//>>>   CGCanvas(this).NATIVE_new();
}/*CGCanvas::CGCanvas*/

void CGCanvas::delete(void) {
//>>>   CGCanvas(this).NATIVE_delete();

   class:base.delete();

   delete(&this->font_family);
   ARRAY(&this->stroke_dasharray).delete();
   delete(&this->coord_space);
}/*CGCanvas::delete*/

void CGCanvas::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);

   if (!changing) {
      CGCanvas(this).view_update(FALSE);
   }
   CGCanvas(this).queue_draw(NULL);
}/*CGCanvas::notify_attribute_update*/

void CGCanvas::colour_background_set(TGColour colour) {
   this->colour_background = colour;
   /*>>>repaint?*/
}/*CGCanvas::colour_background_set*/

/*>>>use thead buffer, move back to static inline*/
void CGCanvas::draw_polygon(TGColour colour,
                            bool fill, int point_count, TPoint *point_data,
                            bool closed) {
   TPoint *point;

   if (colour == GCOL_NONE)
      return;

   point = CFramework(&framework).scratch_buffer_allocate();

   memcpy(point, point_data, point_count * sizeof(TPoint));
   CGCoordSpace(&this->coord_space).point_array_utov(point_count, point);
   CGCanvas(this).NATIVE_draw_polygon(colour, fill,
                                      point_count, point, closed);

   CFramework(&framework).scratch_buffer_release(point);
}

void CGCanvas::transform_viewbox_calculate(TRect *viewbox_src, TRect *viewbox_dest, TGPreserveAspectRatio preserve,
                                           TGTransform *result_scale, TGTransform *result_translate) {
   int space;

   CLEAR(result_scale);
   result_scale->type = EGTransform.scale;
   result_scale->t.scale.sx = viewbox_dest->point[1].x / viewbox_src->point[1].x;
   result_scale->t.scale.sy = viewbox_dest->point[1].y / viewbox_src->point[1].y;
   if (preserve.aspect != EGPreserveAspectRatio.none) {
      switch (this->preserveAspectRatio.meet_or_slice) {
      case EGMeetOrSlice.meet:
         if (result_scale->t.scale.sx > (result_scale->t.scale.sy)) {
            result_scale->t.scale.sx = (result_scale->t.scale.sy);
         }
         if ((result_scale->t.scale.sy) > result_scale->t.scale.sx) {
            result_scale->t.scale.sy = (result_scale->t.scale.sx);
         }
         break;
      case EGMeetOrSlice.slice:
         if (result_scale->t.scale.sx < (result_scale->t.scale.sy)) {
            result_scale->t.scale.sx = (result_scale->t.scale.sy);
         }
         if ((result_scale->t.scale.sy) < result_scale->t.scale.sx) {
            result_scale->t.scale.sy = (result_scale->t.scale.sx);
         }
         break;
      }
   }

   /*transform for min/mid/max*/
   result_translate->type = EGTransform.translate;

   space = (int)((float)viewbox_dest->point[1].x - (float)viewbox_src->point[1].x * result_scale->t.scale.sx);
   switch (preserve.aspect) {
   case EGPreserveAspectRatio.none:
   case EGPreserveAspectRatio.xMinYMin:
   case EGPreserveAspectRatio.xMinYMid:
   case EGPreserveAspectRatio.xMinYMax:
   default:
      result_translate->t.translate.tx = 0;
      break;
   case EGPreserveAspectRatio.xMidYMin:
   case EGPreserveAspectRatio.xMidYMid:
   case EGPreserveAspectRatio.xMidYMax:
      result_translate->t.translate.tx = (float)(space) / 2;
      break;
   case EGPreserveAspectRatio.xMaxYMin:
   case EGPreserveAspectRatio.xMaxYMid:
   case EGPreserveAspectRatio.xMaxYMax:
      result_translate->t.translate.tx = (float)(space);
      break;
   }

   space = (int)((float)viewbox_dest->point[1].y - (float)viewbox_src->point[1].y * result_scale->t.scale.sy);
   switch (preserve.aspect) {
   case EGPreserveAspectRatio.none:
   case EGPreserveAspectRatio.xMinYMin:
   case EGPreserveAspectRatio.xMidYMin:
   case EGPreserveAspectRatio.xMaxYMin:
   default:
      result_translate->t.translate.ty = 0;
      break;
   case EGPreserveAspectRatio.xMinYMid:
   case EGPreserveAspectRatio.xMidYMid:
   case EGPreserveAspectRatio.xMaxYMid:
      result_translate->t.translate.ty = (float)(space) / 2;
      break;
   case EGPreserveAspectRatio.xMinYMax:
   case EGPreserveAspectRatio.xMidYMax:
   case EGPreserveAspectRatio.xMaxYMax:
      result_translate->t.translate.ty = (float)(space);
      break;
   }
}/*CGCanvas::transform_viewbox_calculate*/

void CGCanvas::transform_viewbox(void) {
   TGTransform transform;
   int space;
   TCoord scale_x, scale_y;

   /*>>>use transform_viewbox_calculate()! */

   if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGCanvas,viewBox>) &&
        this->preserveAspectRatio.aspect != EGPreserveAspectRatio.oneToOne) {
      CLEAR(&transform);
      transform.type = EGTransform.scale;
      transform.t.scale.sx = CGXULElement(this)->width / this->content_width;
      transform.t.scale.sy = CGXULElement(this)->height / this->content_height;
      if (this->preserveAspectRatio.aspect != EGPreserveAspectRatio.none) {
         switch (this->preserveAspectRatio.meet_or_slice) {
         case EGMeetOrSlice.meet:
            if (transform.t.scale.sx > (transform.t.scale.sy * this->yAspect)) {
               transform.t.scale.sx = (transform.t.scale.sy * this->yAspect);
            }
            if ((transform.t.scale.sy * this->yAspect) > transform.t.scale.sx) {
               transform.t.scale.sy = (transform.t.scale.sx / this->yAspect);
            }
            break;
         case EGMeetOrSlice.slice:
            if (transform.t.scale.sx < (transform.t.scale.sy * this->yAspect)) {
               transform.t.scale.sx = (transform.t.scale.sy * this->yAspect);
            }
            if ((transform.t.scale.sy * this->yAspect) < transform.t.scale.sx) {
               transform.t.scale.sy = (transform.t.scale.sx / this->yAspect);
            }
            break;
         }
      }
      scale_x = transform.t.scale.sx;
      scale_y = transform.t.scale.sy;

      CGCoordSpace(&CGCanvas(this)->coord_space).transform_apply(&transform, FALSE);

      /*extra transform for min/mid/max*/
      CLEAR(&transform);
      transform.type = EGTransform.translate;

      space = (int)(CGXULElement(this)->width - this->content_width * scale_x);
      switch (this->preserveAspectRatio.aspect) {
      case EGPreserveAspectRatio.none:
      case EGPreserveAspectRatio.xMinYMin:
      case EGPreserveAspectRatio.xMinYMid:
      case EGPreserveAspectRatio.xMinYMax:
      default:
         transform.t.translate.tx = 0;
         break;
      case EGPreserveAspectRatio.xMidYMin:
      case EGPreserveAspectRatio.xMidYMid:
      case EGPreserveAspectRatio.xMidYMax:
         transform.t.translate.tx = (float)(space) / 2;
         break;
      case EGPreserveAspectRatio.xMaxYMin:
      case EGPreserveAspectRatio.xMaxYMid:
      case EGPreserveAspectRatio.xMaxYMax:
         transform.t.translate.tx = (float)(space);
         break;
      }

      space = (int)((float)CGXULElement(this)->height - (float)this->content_height * scale_y);
      switch (this->preserveAspectRatio.aspect) {
      case EGPreserveAspectRatio.none:
      case EGPreserveAspectRatio.xMinYMin:
      case EGPreserveAspectRatio.xMidYMin:
      case EGPreserveAspectRatio.xMaxYMin:
      default:
         transform.t.translate.ty = 0;
         break;
      case EGPreserveAspectRatio.xMinYMid:
      case EGPreserveAspectRatio.xMidYMid:
      case EGPreserveAspectRatio.xMaxYMid:
         transform.t.translate.ty = (float)(space) / 2;
         break;
      case EGPreserveAspectRatio.xMinYMax:
      case EGPreserveAspectRatio.xMidYMax:
      case EGPreserveAspectRatio.xMaxYMax:
         transform.t.translate.ty = (float)(space);
         break;
      }
      CGCoordSpace(&CGCanvas(this)->coord_space).transform_apply(&transform, FALSE);
   }

   CLEAR(&transform);
   transform.type = EGTransform.translate;
   transform.t.translate.tx = (float) -this->view_origin.x;
   transform.t.translate.ty = (float) -this->view_origin.y;
   CGCoordSpace(&CGCanvas(this)->coord_space).transform_apply(&transform, FALSE);
   CGCoordSpace(&CGCanvas(this)->coord_space).scaling_recalc();
}/*CGCanvas::transform_viewbox*/

void CGCanvas::point_array_utov(int count, TPoint *point) {
   CGCoordSpace(&this->coord_space).point_array_utov(count, point);
}/*CGCanvas::point_array_utov*/

void CGCanvas::point_array_vtou(int count, TPoint *point) {
   CGCoordSpace(&this->coord_space).point_array_vtou(count, point);
}/*CGCanvas::point_array_vtou*/

void CGCanvas::view_update(bool resize) {
   TPoint point[2];
   int scroll_height_last = this->scroll_height;

   if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGCanvas,viewBox>)) {
      this->content_width  = (int)this->viewBox.point[1].x;
      this->content_height = (int)this->viewBox.point[1].y;
   }
   else {
      this->content_width  = (int)CGXULElement(this)->width;
      this->content_height = (int)CGXULElement(this)->height;
   }

   this->scroll_width  = this->content_width;
   this->scroll_height = this->content_height;

   this->content_width  = this->content_width  * 100 / this->zoom;
   this->content_height = this->content_height * 100 / this->zoom;

   CGCoordSpace(&this->coord_space).reset();
   CGCanvas(this).transform_viewbox();
   point[0].x = 0;
   point[0].y = 0;
   point[1].x = this->scroll_width;
   point[1].y = this->scroll_height;
   CGCanvas(this).point_array_utov(2, point);
   this->scroll_width  = (int)CGXULElement(this)->width;
   this->scroll_height = (int)CGXULElement(this)->height;

   if (this->allocated_width >= this->scroll_width) {
      this->view_origin.x = 0;
   }
   else {
   }
   if (this->allocated_height >= this->scroll_height) {
      this->view_origin.y = 0;
   }
   else if (this->scrollMode == EGCanvasScroll.follow) {
      if (scroll_height_last - this->view_origin.y <= this->allocated_height) {
         this->view_origin.y = this->scroll_height - this->allocated_height;
      }
   }

   /*>>>should be in CGLayout, and interfers with box repositioning in any case*/
   /*>>>causes crash in operation with canvas attribute update*/

   if (this->component_reposition == 1) {
      this->component_reposition = 2;
      CGCanvas(this).component_reposition(CObjPersistent(this));
      this->component_reposition = 1;
   }
}/*CGCanvas::view_update*/

void CGCanvas::transform_set_gobject(CGObject *gobject, bool viewbox) {
   /* set coordinate space for specifed object's transforms */
   TGTransform *transform;
   CObject *transform_obj;
   int i;

   transform_obj = CObject(gobject);
   CGCoordSpace(&this->coord_space).reset();
   while (transform_obj && transform_obj != CObject(this)) {
      if (CObjClass_is_derived(&class(CGObject), CObject(transform_obj).obj_class())) {
         for (i = ARRAY(&CGObject(transform_obj)->transform).count(); i > 0; i--) {
            transform = &ARRAY(&CGObject(transform_obj)->transform).data()[i - 1];
            CGCoordSpace(&CGCanvas(this)->coord_space).transform_apply(transform, FALSE);
         }
      }
      transform_obj = CObject(transform_obj).parent();
   }

   if (viewbox) {
      CGCanvas(this).transform_viewbox();
   }
}/*CGCanvas::transform_set_gobject*/

void CGCanvas::transform_prepend_gobject(CGObject *gobject) {
   /* set coordinate space for specifed object's transforms */
   TGTransform *transform;
   int i;

   for (i = 0; i < ARRAY(&CGObject(gobject)->transform).count(); i++) {
      transform = &ARRAY(&CGObject(gobject)->transform).data()[i];
      CGCoordSpace(&CGCanvas(this)->coord_space).transform_apply(transform, TRUE);
   }
}/*CGCanvas::transform_set_gobject*/

void CGCanvas::draw_gobject_transformed(CGCanvas *canvas, CGObject *gobject, TRect *extent, EGObjectDrawMode mode) {
   TRect gobj_extent, canvas_extent;
   int i;
   TGTransform *transform;

   if (gobject->visibility == EGVisibility.visible) {
      CGCoordSpace(&this->coord_space).push();

      /*only calculate coordinate space if object has a new transform from parent*/
      if (ARRAY(&gobject->transform).count()) {
         for (i = 0; i < ARRAY(&CGObject(gobject)->transform).count(); i++) {
            transform = &ARRAY(&CGObject(gobject)->transform).data()[i];
            CGCoordSpace(&CGCanvas(this)->coord_space).transform_apply(transform, TRUE);
         }
         CGCoordSpace(&CGCanvas(this)->coord_space).scaling_recalc();
      }
//      /*>>>have this push/pop with the current coordinate space*/
//      CGCoordSpace(&CGCanvas(this)->coord_space).scaling_recalc();

      /* copy transform matrix and scaling to destination canvas >>>hacky*/
      canvas->coord_space.matrix    = this->coord_space.matrix;
      canvas->coord_space.scaling.x = this->coord_space.scaling.x;
      canvas->coord_space.scaling.y = this->coord_space.scaling.y;
      canvas->coord_space.rotation  = this->coord_space.rotation;

      /* Draw only objects within redraw extent */
      if (extent) {
         gobj_extent = gobject->extent;
         CGCoordSpace(&this->coord_space).push();
         CGCoordSpace(&this->coord_space).reset();
         CGCanvas(this).transform_viewbox();
         canvas_extent = *extent;
         CGCanvas(this).point_array_utov(2, gobj_extent.point);
         CGCoordSpace(&this->coord_space).pop();
         if (TRect_overlap(&canvas_extent, &gobj_extent)) {
            CGObject(gobject).draw(canvas, extent, mode);
         }
      }
      else {
         CGObject(gobject).draw(canvas, extent, mode);
      }
      CGCoordSpace(&this->coord_space).pop();
   }
}/*CGCanvas::draw_gobject_transformed*/

void CGCanvas::redraw(TRect *extent, EGObjectDrawMode mode) {
   CGCoordSpace(&this->coord_space).reset();
   CGCanvas(this).draw(this, extent, mode);

   CGCanvas(this).transform_viewbox();
}/*CGCanvas::redraw*/

void CGCanvas::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
//   CGCoordSpace(&this->coord_space).reset();
   CGCanvas(this).opacity_set(1.0);
  CGCanvas(this).draw_rectangle(CGCanvas(this)->colour_background,
                                TRUE, extent->point[0].x, extent->point[0].y, extent->point[1].x, extent->point[1].y);
//
//   CGCanvas(this).transform_viewbox();
}/*CGCanvas::draw*/

void CGCanvas::component_reposition(CObjPersistent *object) {
   CObjPersistent *child = CObjPersistent(CObject(object).child_first());

   while (child) {
      if (!CObjClass_is_derived(&class(CGCanvas), CObject(child).obj_class())) {
         CGCanvas(this).component_reposition(child);
      }

      if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class()) &&
          CGObject(child)->native_object) {
//         CObjPersistent(child).attribute_update(ATTRIBUTE<CGXULElement,x>);
//         CObjPersistent(child).attribute_update(ATTRIBUTE<CGXULElement,y>);
//         CObjPersistent(child).attribute_update(ATTRIBUTE<CGXULElement,width>);
//         CObjPersistent(child).attribute_update(ATTRIBUTE<CGXULElement,height>);
         CObjPersistent(child).attribute_update_end();
      }
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }
}/*CGCanvas::component_reposition*/

//void CGCanvas::show(bool show) {
//   CGCanvas(this).NATIVE_show(show);
//}/*CGCanvas::show*/

void CGCanvasBitmap::CGCanvasBitmap(int width, int height) {
   new(&this->bitmap).CGBitmap();

   this->bitmap.width  = width;
   this->bitmap.height = height;

   CGBitmap(&this->bitmap).NATIVE_allocate();
}/*CGCanvasBitmap::CGCanvasBitmap*/

void CGCanvasBitmap::delete(void) {
   CGBitmap(&this->bitmap).NATIVE_release();

   delete(&this->bitmap);

   class:base.delete();
}/*CGCanvasBitmap::delete*/

void CGCanvasBitmap::resize(int width, int height) {
   if (width  != this->bitmap.width ||
       height != this->bitmap.height) {
      CGBitmap(&this->bitmap).NATIVE_release();
      this->bitmap.width  = width;
      this->bitmap.height = height;
      CGBitmap(&this->bitmap).NATIVE_allocate();
   }
}/*CGCanvasBitmap::CGCanvasBitmap*/

void CGCanvasPrint::new(void) {
   class:base.new();

   new(&this->printer).CString(NULL);
   new(&this->outfile).CString(NULL);
}/*CGCanvasPrint::new*/

void CGCanvasPrint::CGCanvasPrint(CGCanvas *src, bool copy_child, CGWindow *parent_window) {
   CObjPersistent *child, *child_copy;

   this->src_canvas = src;
   this->parent_window = parent_window;

   if (src) {
      CGCanvas(this).render_set(src->render_mode);
      CGCanvas(this)->colour_background = src->colour_background;
      CGCanvas(this)->viewBox = src->viewBox;
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,viewBox>, CObjPersistent(src).attribute_default_get(ATTRIBUTE<CGCanvas,viewBox>));
      CGCanvas(this)->preserveAspectRatio = src->preserveAspectRatio;
      if (copy_child) {
         child = CObjPersistent(CObject(src).child_first());
         while (child) {
            child_copy = CObjPersistent(child).copy(TRUE);
            CObject(this).child_add(CObject(child_copy));
            child = CObjPersistent(CObject(src).child_next(CObject(child)));
         }
      }
   }
}/*CGCanvasPrint::CGCanvasPrint*/

void CGCanvasPrint::delete(void) {
   delete(&this->outfile);
   delete(&this->printer);

   class:base.delete();
}/*CGCanvasPrint::delete*/

void CGCanvasPrint::print_document(void) {
   CObject *object = CObject(this).child_first();

   CGCanvasPrint(this).NATIVE_allocate(NULL);

//   CGCoordSpace(&CGCanvas(this)->coord_space).reset();
//   CGCanvas(this).transform_viewbox();
   CGCanvas(this).view_update(FALSE);
//   CGCanvas(this).draw(CGCanvas(this), NULL, EGObjectDrawMode.Draw);
   while (object) {
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class()) &&
          !CObjClass_is_derived(&class(CGCanvas), CObject(object).obj_class())) {
         CGCanvas(this).draw_gobject_transformed(CGCanvas(this), CGObject(object), NULL, EGObjectDrawMode.Draw);
      }
      if (CObjClass_is_derived(&class(CGCanvas), CObject(object).obj_class()) &&
          !CGObject(object)->native_object) {
         CGCanvas(this).draw_gobject_transformed(CGCanvas(this), CGObject(object), NULL, EGObjectDrawMode.Draw);
      }
      object = CObject(this).child_next(object);
   }

   CGCanvasPrint(this).NATIVE_release(NULL);
}/*CGCanvasPrint::print_document*/

void CGPalette::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGCanvas *canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));

   if (canvas && !changing) {
      CGCanvas(canvas).queue_draw(NULL);
   }

   class:base.notify_attribute_update(attribute, changing);
}/*CGPalette::notify_attribute_update*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/