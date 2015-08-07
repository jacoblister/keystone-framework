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
#include "../objpersistent.c"
#include "gobject.c"
#include "gpalette.c"
#include "gcanvas.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

typedef int TGDashArray;

ARRAY:typedef<TGDashArray>;
ATTRIBUTE:typedef<TGDashArray>;

ENUM:typedef<EGMarkerStart> {
   {none}, 
   {triangle, "url(#TriangleStart)"},
   {triangle_hollow, "url(#TriangleHollowStart)"},   
   {arrow, "url(#ArrowStart)"},   
   {arrow_large, "url(#ArrowLargeStart)"},      
   {lines, "url(#LinesStart)"},   
   {lines_large, "url(#LinesLargeStart)"},      
};

ENUM:typedef<EGMarkerEnd> {
   {none}, 
   {triangle, "url(#TriangleEnd)"},
   {triangle_hollow, "url(#TriangleHollowEnd)"},      
   {arrow, "url(#ArrowEnd)"},   
   {arrow_large, "url(#ArrowLargeEnd)"},         
   {lines, "url(#LinesStart)"},   
   {lines_large, "url(#LinesLargeStart)"},      
};

class CGMarker;

class CGPrimitive : CGObject {
 private:
   void new(void);
   void delete(void);

   CGMarker *marker_find(int position, int index);
   void marker_transform(CGCanvas *canvas, CGMarker *marker, TPoint *position,  TCoord angle, bool enter);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual bool transform_assimilate(TGTransform *transform);
   virtual bool stroke_linecap_present(void);   
   virtual bool stroke_linejoin_present(void);   
   TRect redraw_extent;                    
 public:
   ATTRIBUTE<TGColour stroke, ,PO_INHERIT> {
      if (data)
         this->stroke = *data;
      else
         this->stroke = GCOL_NONE;
   };
   ATTRIBUTE<TGColour fill, ,PO_INHERIT> {
      if (data)
         this->fill = *data;
      else
         this->fill = GCOL_BLACK;
   };
   ATTRIBUTE<float stroke_opacity, "stroke-opacity" ,PO_INHERIT> {
      if (data)
         this->stroke_opacity = *data;
      else
         this->stroke_opacity = 1;
   };
   ATTRIBUTE<float fill_opacity, "fill-opacity" ,PO_INHERIT> {
      if (data)
         this->fill_opacity = *data;
      else
         this->fill_opacity = 1;
   };
   ATTRIBUTE<float stroke_width, "stroke-width" ,PO_INHERIT> {
      if (data)
         this->stroke_width = *data;
      else
         this->stroke_width = 1;
   };
   ATTRIBUTE<EGStrokeLineCap stroke_linecap, "stroke-linecap" ,PO_INHERIT> {
      if (!CGPrimitive(this).stroke_linecap_present()) 
         this->stroke_linecap = EGStrokeLineCap.none;
      else if (data)
         this->stroke_linecap = *data;
      else
         this->stroke_linecap = EGStrokeLineCap.butt;
   };
   ATTRIBUTE<EGStrokeLineJoin stroke_linejoin, "stroke-linejoin" ,PO_INHERIT> {
      if (!CGPrimitive(this).stroke_linejoin_present()) 
         this->stroke_linejoin = EGStrokeLineJoin.none;
      else if (data)
         this->stroke_linejoin = *data;
      else
         this->stroke_linejoin = EGStrokeLineJoin.miter;
   };
   ATTRIBUTE<int stroke_miterlimit, "stroke-miterlimit"> {
      if (data)
         this->stroke_miterlimit = *data;
      else
         this->stroke_miterlimit = 10;
   };
   ATTRIBUTE<ARRAY<TGDashArray> stroke_dasharray, "stroke-dasharray", PO_INHERIT>;
   ATTRIBUTE<int stroke_dashoffset, "stroke-dashoffset", PO_INHERIT>;

   void CGPrimitive(TGColour stroke, TGColour fill);
};

class CGRect : CGPrimitive {
 private:
   void position_polarize(void);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void extent_set(CGCanvas *canvas);
   virtual void cpoint_get(ARRAY<TPoint> *point);
   virtual void cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new);
   virtual void create_point_set(TPoint *point);
   virtual bool transform_assimilate(TGTransform *transform);

   void new(void);
   void delete(void);
 public:
   ALIAS<"svg:rect", "rectangle">;
   ATTRIBUTE<TCoord x>;
   ATTRIBUTE<TCoord y>;
   ATTRIBUTE<TCoord width>;
   ATTRIBUTE<TCoord height>;
   ATTRIBUTE<TCoord rx>;
   ATTRIBUTE<TCoord ry>;

   void CGRect(TGColour stroke, TGColour fill,
               double x, double y, double width, double height);
};

class CGEllipse : CGPrimitive {
 private:
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void extent_set(CGCanvas *canvas);
   virtual void cpoint_get(ARRAY<TPoint> *point);
   virtual void cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new);
   virtual void create_point_set(TPoint *point);
   virtual bool transform_assimilate(TGTransform *transform);
 public:
   ALIAS<"svg:ellipse">;
   ATTRIBUTE<TCoord cx>;
   ATTRIBUTE<TCoord cy>;
   ATTRIBUTE<TCoord rx>;
   ATTRIBUTE<TCoord ry>;

   void CGEllipse(TGColour stroke, TGColour fill,
                  double cx, double cy, double rx, double ry);
};

class CGCircle : CGEllipse {
 private:
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void cpoint_get(ARRAY<TPoint> *point);
 public:
   ALIAS<"svg:circle">;
   ATTRIBUTE<TCoord r> {
      if (data) {
         this->r = *data;
      }
      else {
         this->r = 0;
      }
      CGEllipse(this)->rx = this->r;
      CGEllipse(this)->ry = this->r;
   };

   void CGCircle(TGColour stroke, TGColour fill,
                 int cx, int cy, int r);
};

class CGLine : CGPrimitive {
 private:
   void new(void);
 public:
   ALIAS<"svg:line">;
   ATTRIBUTE<TCoord x1>;
   ATTRIBUTE<TCoord y1>;
   ATTRIBUTE<TCoord x2>;
   ATTRIBUTE<TCoord y2>;
   ATTRIBUTE<EGMarkerStart marker_start, "marker-start"> {
      if (data) {
         this->marker_start = *data;
      }
      else {
         this->marker_start = EGMarkerStart.none;
      }
   };
   ATTRIBUTE<EGMarkerEnd marker_end, "marker-end"> {
      if (data) {
         this->marker_end = *data;
      }
      else {
         this->marker_end = EGMarkerStart.none;
      }
   };
   
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void extent_set(CGCanvas *canvas);
   virtual void cpoint_get(ARRAY<TPoint> *point);
   virtual void cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new);
   virtual void create_point_set(TPoint *point);
   virtual bool transform_assimilate(TGTransform *transform);

   void CGLine(TGColour stroke,
               double x1, double y1, double x2, double y2);
};

class CGPolyLine : CGPrimitive {
 private:
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void extent_set(CGCanvas *canvas);
   virtual void cpoint_get(ARRAY<TPoint> *point);
   virtual void cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new);
   virtual bool transform_assimilate(TGTransform *transform);

   void new(void);
   void delete(void);   
 public:
   ALIAS<"svg:polyline">;
   ATTRIBUTE<ARRAY<TPoint> point, "points">;
   ATTRIBUTE<EGMarkerStart marker_start, "marker-start"> {
      if (data) {
         this->marker_start = *data;
      }
      else {
         this->marker_start = EGMarkerStart.none;
      }
   };
   ATTRIBUTE<EGMarkerEnd marker_end, "marker-end"> {
      if (data) {
         this->marker_end = *data;
      }
      else {
         this->marker_end = EGMarkerStart.none;
      }
   };
   
   void CGPolyLine(TGColour stroke, TGColour fill,
                   int point_count, TPoint *point);

   void point_add(TPoint *point); /*>>>replace with control point add */
};

class CGPolygon : CGPolyLine {
 private:
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
 public:
   ALIAS<"svg:polygon">;

   void CGPolygon(TGColour stroke, TGColour fill,
                  int point_count, TPoint *point);
};

class CGPath : CGPrimitive {
 private:
   void new(void);
   void delete(void);
 public:
   ALIAS<"svg:path">;
   ATTRIBUTE<ARRAY<TGPathInstruction> instruction, "d">;

   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void extent_set(CGCanvas *canvas);
   virtual bool transform_assimilate(TGTransform *transform);

   void CGPath(TGColour stroke, TGColour fill);
};

class CGText : CGPrimitive {
 private:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void extent_set(CGCanvas *canvas);
   virtual void cpoint_get(ARRAY<TPoint> *point);
   virtual void cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new);
   virtual bool transform_assimilate(TGTransform *transform);
   static inline void y_set_default(void);

   void new(void);
   void delete(void);
 public:
   ALIAS<"svg:text">;
   ATTRIBUTE<TCoord x>;
   ATTRIBUTE<TCoord y> {
      if (data) {
         this->y = *data;
      }
      else {
         CGText(this).y_set_default();
      }
   };
   ATTRIBUTE<CGFontFamily font_family, "font-family", PO_INHERIT> {
      if (data) {
         CString(&this->font_family).set(CString(data).string());
      }
      else {
         CString(&this->font_family).set("Arial");
      }
   };
   ATTRIBUTE<EGFontStyle font_style, "font-style", PO_INHERIT>;
   ATTRIBUTE<EGFontWeight font_weight, "font-weight", PO_INHERIT>;
   ATTRIBUTE<int font_size, "font-size", PO_INHERIT> {
      if (data && *data == 0) {
          return;
      }
      if (data) {
         this->font_size = *data;
      }
      else {
         this->font_size = 12;
      }
   };
   ATTRIBUTE<EGTextAnchor text_anchor, "text-anchor", PO_INHERIT>;
   ATTRIBUTE<EGTextBaseline dominant_baseline, "dominant-baseline", PO_INHERIT>;
   ATTRIBUTE<EGTextDecoration text_decoration, "text-decoration", PO_INHERIT>;
   DATA<CString text>;

   void CGText(TGColour fill, double x, double y, const char *text);
};

ATTRIBUTE:typedef<CGText>;

class CGTextSpan : CGText {
   ALIAS<"svg:tspan">;
   ATTRIBUTE<TCoord dx>;
   ATTRIBUTE<TCoord dy>;

   void CGTextSpan(void);
};

static inline void CGText::y_set_default(void) {
   CObject *reference;
   if (CObject(this).parent() && CObject(CObject(this).parent()).obj_class() == &class(CGText) &&
       CObject(this).obj_class() == &class(CGTextSpan)) {
      reference = CObject(CObject(this).parent()).child_previous(CObject(this));
      while (reference && !CObjClass_is_derived(&class(CGText), CObject(reference).obj_class())) {
         reference = CObject(CObject(this).parent()).child_previous(reference);
      }
      if (!reference) {
         reference = CObject(CObject(this).parent());
      }
      CGText(this)->y = CGText(reference)->y + CGTextSpan(this)->dy;
   }
}/*CGText::y_set_default*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include "gprimcontainer.c"

bool ATTRIBUTE:convert<TGDashArray>(CObjPersistent *object,
                                    const TAttributeType *dest_type, const TAttributeType *src_type,
                                    int dest_index, int src_index,
                                    void *dest, const void *src) {
   CString *string;
   ARRAY<TGDashArray> *array;

   if (dest_type == &ATTRIBUTE:type<TGDashArray> && src_type == NULL) {
      memset(dest, 0, sizeof(TGDashArray));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TGDashArray> && src_type == &ATTRIBUTE:type<TGDashArray>) {
      *((TGDashArray *)dest) = *((TGDashArray *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TGDashArray>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TGDashArray> && src_type == &ATTRIBUTE:type<CString>) {
      ATTRIBUTE:convert<int>(CObjPersistent(object), &ATTRIBUTE:type<int>, src_type, dest_index, src_index, dest, src);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:typearray<TGDashArray> && src_type == &ATTRIBUTE:type<CString>) {
      if (dest_index != ATTR_INDEX_VALUE) {
         string = (void *)src;
         array = dest;
         if (CString(string).strcmp("Solid") == 0) {
            ARRAY(array).used_set(0);
         }
         if (CString(string).strcmp("Dashed") == 0) {
            ARRAY(array).used_set(2);
            ARRAY(array).data()[0] = 10;
            ARRAY(array).data()[1] = 10;
         }
         if (CString(string).strcmp("Dotted") == 0) {
            ARRAY(array).used_set(2);
            ARRAY(array).data()[0] = 2;
            ARRAY(array).data()[1] = 2;
         }
         if (CString(string).strcmp("Custom") == 0) {
            if (ARRAY(array).count() == 0 ||
                ((ARRAY(array).count() == 2 && ARRAY(array).data()[0] == 10 && ARRAY(array).data()[1] == 10) ||
                (ARRAY(array).count() == 2 && ARRAY(array).data()[0] == 2 && ARRAY(array).data()[1] == 2))) {
               ARRAY(array).used_set(4);
               ARRAY(array).data()[0] = 10;
               ARRAY(array).data()[1] = 10;
               ARRAY(array).data()[2] = 2;
               ARRAY(array).data()[3] = 10;
            }
         }
         return TRUE;
     }
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:typearray<TGDashArray>) {
      if (src_index != ATTR_INDEX_VALUE) {
         array = (void *)src;
         if (!ARRAY(array).count() || (ARRAY(array).count() == 1 && ARRAY(array).data()[0] == 0)) {
            CString(dest).set("Solid");
         }
         else if (ARRAY(array).count() == 2 && ARRAY(array).data()[0] == 10 && ARRAY(array).data()[1] == 10) {
            CString(dest).set("Dashed");
         }
         else if (ARRAY(array).count() == 2 && ARRAY(array).data()[0] == 2 && ARRAY(array).data()[1] == 2) {
            CString(dest).set("Dotted");
         }
         else {
            CString(dest).set("Custom");
         }
         return TRUE;
      }
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TGDashArray>) {
      ATTRIBUTE:convert<int>(CObjPersistent(object), dest_type, &ATTRIBUTE:type<int>, dest_index, src_index, dest, src);
      return TRUE;
   }

   return FALSE;
}

void CGPrimitive::new(void) {
   class:base.new();

   ARRAY(&this->stroke_dasharray).new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke_opacity>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,fill_opacity>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke_width>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke_linecap>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke_linejoin>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke_miterlimit>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke_dasharray>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke_dashoffset>, TRUE);
}/*CGPrimitive::new*/

void CGPrimitive::CGPrimitive(TGColour stroke, TGColour fill) {
   this->stroke = stroke;
   this->fill = fill;
}/*CGPrimitive::CGPrimitive*/

void CGPrimitive::delete(void) {
   ARRAY(&this->stroke_dasharray).delete();

   class:base.delete();
}/*CGPrimitive::delete*/

static CGLayout *marker_layout;

CGMarker *CGPrimitive::marker_find(int position, int index) {
   CGMarker *marker;
   
   if (!marker_layout) {
      if (!CMemFileDirectory(framework.memfile_directory).memfile_find("marker.svg")) {
         return NULL;
      }
      marker_layout = new.CGLayout(0, 0, NULL, NULL);
      CGLayout(marker_layout).load_svg_file("memfile:marker.svg", NULL);
   }
   
   if (!marker_layout || !index) {
      return NULL;
   }
   
   marker = CGMarker(CObject(&marker_layout->defs).child_n((index - 1) * 2 + position));
   
   return marker;
}/*CGPrimitive::marker_find*/

void CGPrimitive::marker_transform(CGCanvas *canvas, CGMarker *marker, TPoint *position, TCoord angle, bool enter) {
   TGTransform transform;
   double scale = 1;
   static bool def_stroke = FALSE, def_fill = FALSE; /*>>>kludge!*/
   
   if (enter) {
      if (!CObjPersistent(marker).attribute_default_get(ATTRIBUTE<CGMarker,view_box>)) {
         scale = marker->marker_width / (marker->view_box.point[1].x - marker->view_box.point[0].x);
      }
         
      if (CObjPersistent(marker).attribute_default_get(ATTRIBUTE<CGPrimitive,stroke>)) {
         def_stroke = TRUE;
         CObjPersistent(marker).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke>, FALSE);      
         CObjPersistent(marker).attribute_set(ATTRIBUTE<CGPrimitive,stroke>, -1, -1, &ATTRIBUTE:type<TGColour>, &CGPrimitive(this)->stroke);
      }
      if (CObjPersistent(marker).attribute_default_get(ATTRIBUTE<CGPrimitive,fill>)) {   
         def_fill = TRUE;
         CObjPersistent(marker).attribute_default_set(ATTRIBUTE<CGPrimitive,fill>, FALSE);      
         CObjPersistent(marker).attribute_set(ATTRIBUTE<CGPrimitive,fill>, -1, -1, &ATTRIBUTE:type<TGColour>, &CGPrimitive(this)->stroke);
      }
      
      CGCoordSpace(&canvas->coord_space).push();
      
      CLEAR(&transform);
      transform.type = EGTransform.translate;
      transform.t.translate.tx = position->x;
      transform.t.translate.ty = position->y;
      CGCoordSpace(&canvas->coord_space).transform_apply(&transform, TRUE);
      
      CLEAR(&transform);
      transform.type = EGTransform.rotate;
      transform.t.rotate.rotate_angle = angle;
      CGCoordSpace(&canvas->coord_space).transform_apply(&transform, TRUE);
      
      CLEAR(&transform);
      transform.type = EGTransform.scale;
      transform.t.scale.sx = this->stroke_width ? this->stroke_width : 0.5;
      transform.t.scale.sy = this->stroke_width ? this->stroke_width : 0.5;   
      CGCoordSpace(&canvas->coord_space).transform_apply(&transform, TRUE);
      
      CLEAR(&transform);
      transform.type = EGTransform.translate;
      transform.t.translate.tx = -CGSymbol(marker)->ref_x * scale;
      transform.t.translate.ty = -CGSymbol(marker)->ref_y * scale;
      CGCoordSpace(&canvas->coord_space).transform_apply(&transform, TRUE);
      
      CLEAR(&transform);
      transform.type = EGTransform.scale;
      transform.t.scale.sx = scale;
      transform.t.scale.sy = scale;
      CGCoordSpace(&canvas->coord_space).transform_apply(&transform, TRUE);
      
      CGCoordSpace(&canvas->coord_space).scaling_recalc();   
   }
   else {
      CGCoordSpace(&canvas->coord_space).pop();
   
      if (def_stroke) {
         CObjPersistent(marker).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke>, TRUE);
      }
      if (def_fill) {
         CObjPersistent(marker).attribute_default_set(ATTRIBUTE<CGPrimitive,fill>, TRUE);
      }
   }
}/*CGPrimitive::marker_transform*/

bool CGPrimitive::stroke_linecap_present(void) {
   if (CObjClass_is_derived(&class(CGLine), CObject(this).obj_class()) ||
       CObjClass_is_derived(&class(CGPath), CObject(this).obj_class()) ||
	    CObjClass_is_derived(&class(CGPrimContainer), CObject(this).obj_class()) ||
       (CObjClass_is_derived(&class(CGRect), CObject(this).obj_class()) && CObject(this).obj_class() != &class(CGRect)) ||
       CObject(this).obj_class() == &class(CGPolyLine)) {
       return TRUE;
   }
   return FALSE;
}   
bool CGPrimitive::stroke_linejoin_present(void) {
   if (CObjClass_is_derived(&class(CGLine), CObject(this).obj_class()) ||
       CObjClass_is_derived(&class(CGEllipse), CObject(this).obj_class())) {
       return FALSE;
   }
   return TRUE;
}   

void CGPrimitive::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   CObjPersistent *child;

   switch (mode) {
   case EGObjectDrawMode.Draw:
      child = CObjPersistent(CObject(this).child_first());

      while (child) {
         if (CObjClass_is_derived(&class(CGObject), CObject(child).obj_class())) {
            CGObject(child).draw(canvas, extent, mode);
         }
         child = CObjPersistent(CObject(this).child_next(CObject(child)));
      }
      break;
   default:
      break;
   }
}/*CGPrimitive::draw*/

void CGRect::new(void) {
   class:base.new();
}/*CGRect::new*/

void CGRect::CGRect(TGColour stroke, TGColour fill,
                     double x, double y, double width, double height) {
   CGPrimitive(this).CGPrimitive(stroke, fill);
   this->x      = x;
   this->y      = y;
   this->width  = width;
   this->height = height;
}/*CGRect::CGRect*/

void CGRect::delete(void) {
   class:base.delete();
}/*CGRect::delete*/

void CGRect::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGColour stroke, fill;

   class:base.draw(canvas, extent, mode);

   switch (mode) {
   case EGObjectDrawMode.Draw:
      fill   = CGPrimitive(this)->fill;
      stroke = CGPrimitive(this)->stroke;
      CGCanvas(canvas).stroke_style_set(CGPrimitive(this)->stroke_width,
                                        (ARRAY<int> *)&CGPrimitive(this)->stroke_dasharray,
                                        CGPrimitive(this)->stroke_dashoffset,
                                        CGPrimitive(this)->stroke_miterlimit,
                                        CGPrimitive(this)->stroke_linecap,
                                        CGPrimitive(this)->stroke_linejoin);
      break;
   case EGObjectDrawMode.Select:
      fill   = GCOL_NONE;
      stroke = GCOL_WHITE;
      break;
   default:
      return;
   }
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->fill_opacity);
   CGCanvas(canvas).draw_rectangle(fill, TRUE,
                                   CGRect(this)->x, CGRect(this)->y,
                                   CGRect(this)->x + CGRect(this)->width,
                                   CGRect(this)->y + CGRect(this)->height);
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->stroke_opacity);
   CGCanvas(canvas).draw_rectangle(stroke, FALSE,
                                   CGRect(this)->x, CGRect(this)->y,
                                   CGRect(this)->x + CGRect(this)->width,
                                   CGRect(this)->y + CGRect(this)->height);
}/*CGRect::draw*/

void CGRect::extent_set(CGCanvas *canvas) {
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
   CGCoordSpace(&canvas->coord_space).extent_extend(&CGObject(this)->extent, CGPrimitive(this)->stroke_width / 2);   
}/*CGRect::extent_set*/

void CGRect::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(2);

   ARRAY(point).data()[0].x = this->x;
   ARRAY(point).data()[0].y = this->y;
   ARRAY(point).data()[1].x = this->x + this->width;
   ARRAY(point).data()[1].y = this->y + this->height;
}/*CGRect::cpoint_get*/

void CGRect::cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new) {
   switch (index) {
   case -1:
      this->x = ARRAY(point).data()[0].x;
      this->y = ARRAY(point).data()[0].y;
      this->width  = ARRAY(point).data()[1].x - ARRAY(point).data()[0].x;
      this->height = ARRAY(point).data()[1].y - ARRAY(point).data()[0].y;
      return;
   case 0:
      ARRAY(point).data()[index] = *point_new;
      this->x = ARRAY(point).data()[0].x;
      this->y = ARRAY(point).data()[0].y;
      this->width  = ARRAY(point).data()[1].x - ARRAY(point).data()[0].x;
      this->height = ARRAY(point).data()[1].y - ARRAY(point).data()[0].y;
      break;
   case 1:
      ARRAY(point).data()[index] = *point_new;
      this->width  = ARRAY(point).data()[1].x - ARRAY(point).data()[0].x;
      this->height = ARRAY(point).data()[1].y - ARRAY(point).data()[0].y;
      break;
   }
}/*CGRect::cpoint_set*/

void CGRect::create_point_set(TPoint *point) {
   this->x      = point[0].x;
   this->y      = point[0].y;
   this->width  = point[1].x - point[0].x;
   this->height = point[1].y - point[0].y;
}/*CGRect::create_point_set*/

bool CGRect::transform_assimilate(TGTransform *transform) {
   bool result = FALSE;

   if (ARRAY(&CGObject(this)->transform).count() != 0) {
      result = CGObject(this).transform_list_apply(&CGObject(this)->transform, transform);
   }
   else {
      switch (transform->type) {
      case EGTransform.scale:
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRect,x>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRect,y>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRect,width>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRect,height>);
         this->x = this->x * transform->t.scale.sx;
         this->y = this->y * transform->t.scale.sy;
         this->width = this->width * transform->t.scale.sx;
         this->height = this->height * transform->t.scale.sy;
         CObjPersistent(this).attribute_update_end();
         result = TRUE;
         break;
      case EGTransform.translate:
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRect,x>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRect,y>);
         this->x += transform->t.translate.tx;
         this->y += transform->t.translate.ty;
         CObjPersistent(this).attribute_update_end();
         result = TRUE;
         break;
      default:
         break;
      }
   }

   class:base.transform_assimilate(transform);
   return result;
}/*CGRect::transform_assimilate*/

void CGRect::position_polarize(void) {
   if (this->width < 0) {
      this->x += this->width;
      this->width = -this->width;
   }
   if (this->height < 0) {
      this->y += this->height;
      this->height = -this->height;
   }
}/*CGRect::position_polarize*/

void CGEllipse::CGEllipse(TGColour stroke, TGColour fill,
                           double cx, double cy, double rx, double ry) {
   CGPrimitive(this).CGPrimitive(stroke, fill);
   this->cx = cx;
   this->cy = cy;
   this->rx = rx;
   this->ry = ry;
}/*CGEllipse::CGEllipse*/

void CGEllipse::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGColour stroke, fill;

   class:base.draw(canvas, extent, mode);

   switch (mode) {
   case EGObjectDrawMode.Draw:
      fill   = CGPrimitive(this)->fill;
      stroke = CGPrimitive(this)->stroke;
      CGCanvas(canvas).stroke_style_set(CGPrimitive(this)->stroke_width,
                                        (ARRAY<int> *)&CGPrimitive(this)->stroke_dasharray,
                                        CGPrimitive(this)->stroke_dashoffset,
                                        CGPrimitive(this)->stroke_miterlimit,
                                        CGPrimitive(this)->stroke_linecap,
                                        CGPrimitive(this)->stroke_linejoin);
      break;
   case EGObjectDrawMode.Select:
      fill   = GCOL_NONE;
      stroke = GCOL_WHITE;
      break;
   default:
      return;
   }
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->fill_opacity);
   CGCanvas(canvas).draw_ellipse(fill, TRUE,
                                 CGEllipse(this)->cx, CGEllipse(this)->cy,
                                 CGEllipse(this)->rx, CGEllipse(this)->ry);
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->stroke_opacity);
   CGCanvas(canvas).draw_ellipse(stroke, FALSE,
                                 CGEllipse(this)->cx, CGEllipse(this)->cy, 
                                 CGEllipse(this)->rx, CGEllipse(this)->ry);
}/*CGEllipse::draw*/

void CGEllipse::extent_set(CGCanvas *canvas) {
   /*>>>doesnt handle coordinate space transforms other than translation/scaling well*/
   TPoint point[2];
   point[0].x = this->cx - this->rx;
   point[0].y = this->cy - this->ry;
   point[1].x = this->cx + this->rx;
   point[1].y = this->cy + this->ry;

   CGCoordSpace(&canvas->coord_space).point_array_utov(2, point);
   CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);
   CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 2, point);
   CGCoordSpace(&canvas->coord_space).extent_extend(&CGObject(this)->extent, CGPrimitive(this)->stroke_width / 2);   
}/*CGEllipse::extent_set*/

void CGEllipse::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(3);

   ARRAY(point).data()[0].x = this->cx;
   ARRAY(point).data()[0].y = this->cy;
   ARRAY(point).data()[1].x = this->cx - this->rx;
   ARRAY(point).data()[1].y = this->cy;
   ARRAY(point).data()[2].x = this->cx;
   ARRAY(point).data()[2].y = this->cy - this->ry;
}/*CGEllipse::cpoint_get*/

void CGEllipse::cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new) {
   switch (index) {
   case -1:
      this->cx = ARRAY(point).data()[0].x;
      this->cy = ARRAY(point).data()[0].y;
      this->rx = ARRAY(point).data()[0].x - ARRAY(point).data()[1].x;
      this->ry = ARRAY(point).data()[0].y - ARRAY(point).data()[2].y;
      return;
   case 0:
      ARRAY(point).data()[index] = *point_new;
      this->cx = ARRAY(point).data()[index].x;
      this->cy = ARRAY(point).data()[index].y;
      break;
   case 1:
      ARRAY(point).data()[index] = *point_new;
      this->rx = ARRAY(point).data()[0].x - ARRAY(point).data()[index].x;
      break;
   case 2:
      ARRAY(point).data()[index] = *point_new;
      this->ry = ARRAY(point).data()[0].y - ARRAY(point).data()[index].y;
      break;
   }

   CGEllipse(this).cpoint_get(point);
}/*CGEllipse::cpoint_set*/

void CGEllipse::create_point_set(TPoint *point) {
   this->cx = point[0].x;
   this->cy = point[0].y;
   this->rx = point[1].x - point[0].x;
   this->ry = point[1].y - point[0].y;
}/*CGEllipse::create_point_set*/

bool CGEllipse::transform_assimilate(TGTransform *transform) {
   bool result = FALSE;

   switch (transform->type) {
   case EGTransform.scale:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGEllipse,cx>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGEllipse,cy>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGEllipse,rx>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGEllipse,ry>);
      this->cx = this->cx * transform->t.scale.sx;
      this->cy = this->cy * transform->t.scale.sy;
      this->rx = this->rx * transform->t.scale.sx;
      this->ry = this->ry * transform->t.scale.sy;
      CObjPersistent(this).attribute_update_end();
      result = TRUE;
      break;
   case EGTransform.translate:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGEllipse,cx>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGEllipse,cy>);
      this->cx += transform->t.translate.tx;
      this->cy += transform->t.translate.ty;
      CObjPersistent(this).attribute_update_end();
      result = TRUE;
      break;
   default:
      break;
   }

   class:base.transform_assimilate(transform);

   return result;
}/*CGEllipse::transform_assimilate*/

void CGCircle::CGCircle(TGColour stroke, TGColour fill,
                        int cx, int cy, int r) {
   CGPrimitive(this).CGPrimitive(stroke, fill);
   CGEllipse(this)->cx = cx;
   CGEllipse(this)->cy = cy;
   this->r = r;
}/*CGCircle::CGCircle*/

void CGCircle::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   class:base.draw(canvas, extent, mode);
}/*CGCircle::draw*/

void CGCircle::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(2);

   ARRAY(point).data()[0].x = CGEllipse(this)->cx;
   ARRAY(point).data()[0].y = CGEllipse(this)->cy;
   ARRAY(point).data()[1].x = CGEllipse(this)->cx - this->r;
   ARRAY(point).data()[1].y = CGEllipse(this)->cy;
}/*CGCircle::cpoint_get*/

void CGLine::new(void) {
   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLine,marker_start>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLine,marker_end>, TRUE);      
}/*CGLine::new*/

void CGLine::CGLine(TGColour stroke,
                     double x1, double y1, double x2, double y2) {
   CGPrimitive(this).CGPrimitive(stroke, GCOL_NONE);
   this->x1 = x1;
   this->y1 = y1;
   this->x2 = x2;
   this->y2 = y2;
}/*CGLine::CGLine*/

void CGLine::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGColour stroke;
   TPoint m_point, m_from;
   CGMarker *marker;
   TPoint v1, v2;
   TCoord angle;
   int i;

   class:base.draw(canvas, extent, mode);

   switch (mode) {
   case EGObjectDrawMode.Draw:
      stroke = CGPrimitive(this)->stroke;
      CGCanvas(canvas).stroke_style_set(CGPrimitive(this)->stroke_width,
                                        (ARRAY<int> *)&CGPrimitive(this)->stroke_dasharray,
                                        CGPrimitive(this)->stroke_dashoffset,
                                        CGPrimitive(this)->stroke_miterlimit,
                                        CGPrimitive(this)->stroke_linecap,
                                        CGPrimitive(this)->stroke_linejoin);
      break;
   case EGObjectDrawMode.Select:
      stroke = GCOL_WHITE;
      break;
   default:
      return;
   }
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->stroke_opacity);
   CGCanvas(canvas).draw_line(stroke,
                              CGLine(this)->x1, CGLine(this)->y1,
                              CGLine(this)->x2, CGLine(this)->y2);
                              
   v1.x = 1;
   v1.y = 0;
   for (i = 0; i < 2; i++) {
      marker = (CGMarker *)CGPrimitive(this).marker_find(i, i ? this->marker_end : this->marker_start);
      if (marker) {
         m_point.x = i ? this->x2 : this->x1;
         m_point.y = i ? this->y2 : this->y1;         
         m_from.x = i ? this->x1 : this->x2;
         m_from.y = i ? this->y1 : this->y2;         

         v2.x = i ? (m_point.x - m_from.x) : (m_from.x - m_point.x);
         v2.y = i ? (m_point.y - m_from.y) : (m_from.y - m_point.y);
         angle = CGCoordSpace(&canvas->coord_space).vector_difference_angle(&v1, &v2);
         CGPrimitive(this).marker_transform(canvas, marker, &m_point, angle, TRUE);
         CGObject(marker).draw(canvas, extent, mode);
         CGPrimitive(this).marker_transform(canvas, marker, &m_point, angle, FALSE);      
      }
   }
}/*CGLine::draw*/

void CGLine::extent_set(CGCanvas *canvas) {
   TPoint point[2];
   TPoint m_point, m_from;   
   CGMarker *marker;
   TPoint v1, v2;
   TCoord angle;
   int i;

   point[0].x = this->x1;
   point[0].y = this->y1;
   point[1].x = this->x2;
   point[1].y = this->y2;

   CGCoordSpace(&canvas->coord_space).point_array_utov(2, point);
   CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);
   CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 2, point);
   CGCoordSpace(&canvas->coord_space).extent_extend(&CGObject(this)->extent, CGPrimitive(this)->stroke_width / 2);   
   
   v1.x = 1;
   v1.y = 0;
   for (i = 0; i < 2; i++) {
      marker = (CGMarker *)CGPrimitive(this).marker_find(i, i ? this->marker_end : this->marker_start);
      if (marker) {
         m_point.x = i ? this->x2 : this->x1;
         m_point.y = i ? this->y2 : this->y1;         
         m_from.x = i ? this->x1 : this->x2;
         m_from.y = i ? this->y1 : this->y2;         

         v2.x = i ? (m_point.x - m_from.x) : (m_from.x - m_point.x);
         v2.y = i ? (m_point.y - m_from.y) : (m_from.y - m_point.y);
         angle = CGCoordSpace(&canvas->coord_space).vector_difference_angle(&v1, &v2);
         CGPrimitive(this).marker_transform(canvas, marker, &m_point, angle, TRUE);
         CGObject(marker).extent_set(canvas);
         CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 1, &CGObject(marker)->extent.point[0]);
         CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 1, &CGObject(marker)->extent.point[1]);
         CGPrimitive(this).marker_transform(canvas, marker, &m_point, angle, FALSE);      
      }
   }
}/*CGLine::extent_set*/

void CGLine::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(2);

   ARRAY(point).data()[0].x = this->x1;
   ARRAY(point).data()[0].y = this->y1;
   ARRAY(point).data()[1].x = this->x2;
   ARRAY(point).data()[1].y = this->y2;
}/*CGLine::cpoint_get*/

void CGLine::cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new) {
   switch (index) {
   case -1:
      this->x1 = ARRAY(point).data()[0].x;
      this->y1 = ARRAY(point).data()[0].y;
      this->x2 = ARRAY(point).data()[1].x;
      this->y2 = ARRAY(point).data()[1].y;
      return;
   case 0:
      ARRAY(point).data()[index] = *point_new;
      this->x1 = ARRAY(point).data()[0].x;
      this->y1 = ARRAY(point).data()[0].y;
      break;
   case 1:
      ARRAY(point).data()[index] = *point_new;
      this->x2 = ARRAY(point).data()[1].x;
      this->y2 = ARRAY(point).data()[1].y;
      break;
   }
}/*CGLine::cpoint_set*/

void CGLine::create_point_set(TPoint *point) {
   this->x1 = point[0].x;
   this->y1 = point[0].y;
   this->x2 = point[1].x;
   this->y2 = point[1].y;
}/*CGLine::create_point_set*/

bool CGLine::transform_assimilate(TGTransform *transform) {
   bool result = FALSE;
   TCoord x,y;

   switch (transform->type) {
   case EGTransform.scale:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGLine,x1>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGLine,y1>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGLine,x2>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGLine,y2>);
      this->x1 = this->x1 * transform->t.scale.sx;
      this->y1 = this->y1 * transform->t.scale.sy;
      this->x2 = this->x2 * transform->t.scale.sx;
      this->y2 = this->y2 * transform->t.scale.sy;
      CObjPersistent(this).attribute_update_end();
      result = TRUE;
      break;
   case EGTransform.translate:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGLine,x1>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGLine,y1>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGLine,x2>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGLine,y2>);
      this->x1 += transform->t.translate.tx;
      this->y1 += transform->t.translate.ty;
      this->x2 += transform->t.translate.tx;
      this->y2 += transform->t.translate.ty;
      CObjPersistent(this).attribute_update_end();
      result = TRUE;
      break;
   case EGTransform.rotate:
      this->x1 -= transform->t.rotate.cx;
      this->y1 -= transform->t.rotate.cy;
      x = (this->x1 * cos(transform->t.rotate.rotate_angle * (2*PI) / 360)) +
         (this->y1 * sin(transform->t.rotate.rotate_angle * (2*PI) / 360));
      y = (this->x1 * -sin(transform->t.rotate.rotate_angle * (2*PI) / 360)) +
         (this->y1 * cos(transform->t.rotate.rotate_angle * (2*PI) / 360));
      this->x1 = x;
      this->y1 = y;
      this->x1 += transform->t.rotate.cx;
      this->y1 += transform->t.rotate.cy;

      this->x2 -= transform->t.rotate.cx;
      this->y2 -= transform->t.rotate.cy;
      x = (this->x2 * cos(transform->t.rotate.rotate_angle * (2*PI) / 360)) +
          (this->y2 * sin(transform->t.rotate.rotate_angle * (2*PI) / 360));
      y = (this->x2 * -sin(transform->t.rotate.rotate_angle * (2*PI) / 360)) +
         (this->y2 * cos(transform->t.rotate.rotate_angle * (2*PI) / 360));
      this->x2 = x;
      this->y2 = y;
      this->x2 += transform->t.rotate.cx;
      this->y2 += transform->t.rotate.cy;
      result = TRUE;
      break;
   default:
      break;
   }

   class:base.transform_assimilate(transform);
   return result;
}/*CGLine::transform_assimilate*/

void CGPolyLine::new(void) {
   TAttribute *attr = ATTRIBUTE<CGPolyLine,point>;
   /*>>>quick hack*/
   attr->options |= (PO_ELEMENT_STRIPSPACES | PO_ELEMENT_PAIR);
   attr->delim = " ";

   class:base.new();

   ARRAY(&this->point).new();
   /*>>>hack till defaults are in place */
   CGPrimitive(this)->stroke = GCOL_NONE;
   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPolyLine,marker_start>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPolyLine,marker_end>, TRUE);      
}/*CGPolyLine::new*/

void CGPolyLine::CGPolyLine(TGColour stroke, TGColour fill,
                            int point_count, TPoint *point) {
   CGPrimitive(this).CGPrimitive(stroke, fill);

   ARRAY(&this->point).data_set(point_count, point);
}/*CGPolyLine::CGPolyLine*/

void CGPolyLine::delete(void) {
   class:base.delete();

   ARRAY(&this->point).delete();
}/*CGPolyLine::delete*/

void CGPolyLine::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGColour stroke, fill;
   TPoint *m_point, *m_from;
   CGMarker *marker;
   TPoint v1, v2;
   TCoord angle;
   int i;

   class:base.draw(canvas, extent, mode);

   switch (mode) {
   case EGObjectDrawMode.Draw:
      stroke = CGPrimitive(this)->stroke;
      fill   = CGPrimitive(this)->fill;
      CGCanvas(canvas).stroke_style_set(CGPrimitive(this)->stroke_width,
                                        (ARRAY<int> *)&CGPrimitive(this)->stroke_dasharray,
                                        CGPrimitive(this)->stroke_dashoffset,
                                        CGPrimitive(this)->stroke_miterlimit,
                                        CGPrimitive(this)->stroke_linecap,
                                        CGPrimitive(this)->stroke_linejoin);
      break;
   case EGObjectDrawMode.Select:
      stroke = GCOL_WHITE;
      fill = GCOL_NONE;
      break;
   default:
      return;
   }
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->fill_opacity);
   CGCanvas(canvas).draw_polygon(fill, TRUE,
                                 ARRAY(&CGPolyLine(this)->point).count(),
                                 ARRAY(&CGPolyLine(this)->point).data(),
                                 TRUE);
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->stroke_opacity);
   CGCanvas(canvas).draw_polygon(stroke, FALSE,
                                 ARRAY(&CGPolyLine(this)->point).count(),
                                 ARRAY(&CGPolyLine(this)->point).data(),
                                 FALSE);

   if (ARRAY(&this->point).count() >= 2) {                                 
      v1.x = 1;
      v1.y = 0;
      for (i = 0; i < 2; i++) {
         marker = (CGMarker *)CGPrimitive(this).marker_find(i, i ? this->marker_end : this->marker_start);
         if (marker) {
            m_point = &ARRAY(&CGPolyLine(this)->point).data()[i ? ARRAY(&CGPolyLine(this)->point).count() - 1 : 0];
            m_from = &ARRAY(&CGPolyLine(this)->point).data()[i ? ARRAY(&CGPolyLine(this)->point).count() - 2 : 1]; 

            v2.x = i ? (m_point->x - m_from->x) : (m_from->x - m_point->x);
            v2.y = i ? (m_point->y - m_from->y) : (m_from->y - m_point->y);
            angle = CGCoordSpace(&canvas->coord_space).vector_difference_angle(&v1, &v2);
            CGPrimitive(this).marker_transform(canvas, marker, m_point, angle, TRUE);
            CGObject(marker).draw(canvas, extent, mode);
            CGPrimitive(this).marker_transform(canvas, marker, m_point, angle, FALSE);      
         }
      }
   }
}/*CGPolyLine::draw*/

void CGPolyLine::extent_set(CGCanvas *canvas) {
   TPoint *point = CFramework(&framework).scratch_buffer_allocate();
   TPoint *m_point, *m_from;
   CGMarker *marker;
   TPoint v1, v2;
   TCoord angle;
   int i;
   
   memcpy(point, ARRAY(&this->point).data(), ARRAY(&this->point).count() * sizeof(TPoint));

   CGCoordSpace(&canvas->coord_space).point_array_utov(ARRAY(&this->point).count(), point);
   CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);
   CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, ARRAY(&this->point).count(), point);
   CGCoordSpace(&canvas->coord_space).extent_extend(&CGObject(this)->extent, CGPrimitive(this)->stroke_width / 2);

   if (ARRAY(&this->point).count() >= 2) {
      v1.x = 1;
      v1.y = 0;
      for (i = 0; i < 2; i++) {
         marker = (CGMarker *)CGPrimitive(this).marker_find(i, i ? this->marker_end : this->marker_start);
         if (marker) {
            m_point = &ARRAY(&CGPolyLine(this)->point).data()[i ? ARRAY(&CGPolyLine(this)->point).count() - 1 : 0];
            m_from = &ARRAY(&CGPolyLine(this)->point).data()[i ? ARRAY(&CGPolyLine(this)->point).count() - 2 : 1]; 

            v2.x = i ? (m_point->x - m_from->x) : (m_from->x - m_point->x);
            v2.y = i ? (m_point->y - m_from->y) : (m_from->y - m_point->y);
            angle = CGCoordSpace(&canvas->coord_space).vector_difference_angle(&v1, &v2);
            CGPrimitive(this).marker_transform(canvas, marker, m_point, angle, TRUE);
            CGObject(marker).extent_set(canvas);
            CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 1, &CGObject(marker)->extent.point[0]);
            CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 1, &CGObject(marker)->extent.point[1]);
            CGPrimitive(this).marker_transform(canvas, marker, m_point, angle, FALSE);      
         }
      }
   }

   CFramework(&framework).scratch_buffer_release(point);
}/*CGPolyLine::extent_set*/

void CGPolyLine::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(ARRAY(&this->point).count());

   memcpy(ARRAY(point).data(), ARRAY(&this->point).data(), ARRAY(&this->point).count() * sizeof(TPoint));
}/*CGPolyLine::cpoint_get*/

void CGPolyLine::cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new) {
   TPoint *poly_point;
   int i;

   switch (index) {
   case -1:
      ARRAY(&this->point).used_set(0);
      for (i = 0; i < ARRAY(point).count(); i++) {
         ARRAY(&this->point).item_add(ARRAY(point).data()[i]);
      }
      return;
   default:
      if (ARRAY(point).count() <= index) {
         CGPolyLine(this).point_add(point_new);
         ARRAY(point).item_add(*point_new);
      }
      else {
         poly_point = &ARRAY(&this->point).data()[index];
         *poly_point = *point_new;
         poly_point = &ARRAY(point).data()[index];
         *poly_point = *point_new;
      }
      break;
   }
}/*CGPolyLine::cpoint_set*/

bool CGPolyLine::transform_assimilate(TGTransform *transform) {
   bool result = FALSE;
   TPoint *point;
   int i;
   TCoord x,y;

   switch (transform->type) {
   case EGTransform.scale:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGPolyLine,point>);
      for (i = 0; i < ARRAY(&this->point).count(); i++) {
         point = &ARRAY(&this->point).data()[i];
         point->x = point->x * transform->t.scale.sx;
         point->y = point->y * transform->t.scale.sy;
      }
      CObjPersistent(this).attribute_update_end();
      result = TRUE;
      break;
   case EGTransform.translate:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGPolyLine,point>);
      for (i = 0; i < ARRAY(&this->point).count(); i++) {
         point = &ARRAY(&this->point).data()[i];
         point->x += transform->t.translate.tx;
         point->y += transform->t.translate.ty;
      }
      CObjPersistent(this).attribute_update_end();
      result = TRUE;
      break;
   case EGTransform.rotate:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGPolyLine,point>);
      for (i = 0; i < ARRAY(&this->point).count(); i++) {
         point = &ARRAY(&this->point).data()[i];
         point->x -= transform->t.rotate.cx;
         point->y -= transform->t.rotate.cy;
         x = (point->x * cos(transform->t.rotate.rotate_angle * (2*PI) / 360)) +
            (point->y * sin(transform->t.rotate.rotate_angle * (2*PI) / 360));
         y = (point->x * -sin(transform->t.rotate.rotate_angle * (2*PI) / 360)) +
            (point->y * cos(transform->t.rotate.rotate_angle * (2*PI) / 360));
         point->x = x;
         point->y = y;
         point->x += transform->t.rotate.cx;
         point->y += transform->t.rotate.cy;
      }
      CObjPersistent(this).attribute_update_end();
      result = TRUE;
      break;
   default:
      break;
   }

   class:base.transform_assimilate(transform);
   return result;
}/*CGPolyLine::transform_assimilate*/

void CGPolyLine::point_add(TPoint *point) {
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGPolyLine,point>);
   ARRAY(&this->point).item_add(*point);
   CObjPersistent(this).attribute_update_end();
}/*CGPolyLine::point_add*/

void CGPolygon::CGPolygon(TGColour stroke, TGColour fill,
                          int point_count, TPoint *point) {
   CGPolyLine(this).CGPolyLine(stroke, fill, point_count, point);
//   /*>>>copy array*/
}/*CGPolygon::CGPolygon*/

void CGPolygon::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGColour stroke, fill;

   _virtual_CGPrimitive_draw(CGPrimitive(this), canvas, extent, mode);

   switch (mode) {
   case EGObjectDrawMode.Draw:
      fill   = CGPrimitive(this)->fill;
      stroke = CGPrimitive(this)->stroke;
      CGCanvas(canvas).stroke_style_set(CGPrimitive(this)->stroke_width,
                                        (ARRAY<int> *)&CGPrimitive(this)->stroke_dasharray,
                                        CGPrimitive(this)->stroke_dashoffset,
                                        CGPrimitive(this)->stroke_miterlimit,
                                        CGPrimitive(this)->stroke_linecap,
                                        CGPrimitive(this)->stroke_linejoin);
      break;
   case EGObjectDrawMode.Select:
      fill   = GCOL_NONE;
      stroke = GCOL_WHITE;
      break;
   default:
      return;
   }
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->fill_opacity);
   CGCanvas(canvas).draw_polygon(fill, TRUE,
                                 ARRAY(&CGPolyLine(this)->point).count(),
                                 ARRAY(&CGPolyLine(this)->point).data(),
                                 TRUE);
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->stroke_opacity);
   CGCanvas(canvas).draw_polygon(stroke, FALSE,
                                 ARRAY(&CGPolyLine(this)->point).count(),
                                 ARRAY(&CGPolyLine(this)->point).data(),
                                 TRUE);
}/*CGPolygon::draw*/

static inline double TGPathInstruction_value(char **str) {
   float result;

   while (isspace(**str))
      (*str)++;
   sscanf(*str, "%g", &result);
   if (**str == '-')
      (*str)++;
   while ((**str >= '0' && **str <= '9') || **str =='.')
      (*str)++;
   while (isspace(**str) || **str ==',')
      (*str)++;

   return (double)result;
}

bool ATTRIBUTE:convert<TGPathInstruction>(CObjPersistent *object,
                                          const TAttributeType *dest_type, const TAttributeType *src_type,
                                          int dest_index, int src_index,
                                          void *dest, const void *src) {
   TGPathInstruction *instruction;
   CString *string;
   CString data;
   char op[2], *cp, *cpe, *cps;
   float x = 0, y = 0;
   const char *delim = "MmZzLlHhVvCcSsQqTtAa";
   ARRAY<TGPathInstruction> *array;
   int i;
   int delim_count, expect_count;

   if (dest_type == &ATTRIBUTE:type<TGPathInstruction> && src_type == NULL) {
      memset(dest, 0, sizeof(TGPathInstruction));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TGPathInstruction> && src_type == &ATTRIBUTE:type<TGPathInstruction>) {
      *((TGPathInstruction *)dest) = *((TGPathInstruction *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TGPathInstruction>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:typearray<TGPathInstruction> && src_type == &ATTRIBUTE:type<CString>) {
      array = dest;
      ARRAY(array).used_set(0);

      new(&data).CString(NULL);

      string = (CString *)src;
      cp = CString(string).string();
      /* Strip any leading space */
      while (*cp && isspace(*cp)) {
         cp++;
      }

      i = 0;
      while (*cp) {
         cpe = cp + 1;
         /* Skip spaces following instruction */
         while (*cpe && isspace(*cpe)) {
            cpe++;
         }
         /* Carry on until next instruction or end */
         cps = NULL;
         delim_count = 0;

         /* Get expected operation count */
         if (strchr("Aa", *cp)) {
            expect_count = 7;
         }
         else if (strchr("QqSs", *cp)) {
            expect_count = 6;
         }
         else if (strchr("Cc", *cp)) {
            expect_count = 6;
         }
         else if (strchr("HhVv", *cp)) {
            expect_count = 1;
         }
         else {
            expect_count = 2;
         }

         while (*cpe && strchr(delim, *cpe) == NULL) {
            if (*cpe == ',') {
               delim_count++;
            }
            if (isspace(*cpe)) {
               cps = cpe;
            }
            else if (cps) {
               cps = NULL;
               delim_count++;
               if (delim_count == expect_count) {
                  cpe--;
                  break;
               }
            }
            cpe++;
         }
         /* Process element */
         CString(string).extract(&data, cp, cpe - 1);
         ARRAY(array).used_set(ARRAY(array).count() + 1);
         (*dest_type->convert)(object,
                               &ATTRIBUTE:type<TGPathInstruction>, &ATTRIBUTE:type<CString>,
                               ATTR_INDEX_VALUE, ATTR_INDEX_VALUE,
                               (void *)&ARRAY(array).data()[i],
                               (void *)&data);

         cp = cpe;
         i++;
      }

      delete(&data);

      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TGPathInstruction> && src_type == &ATTRIBUTE:type<CString>) {
      new(&data).CString(NULL);
      instruction = (TGPathInstruction *)dest;
      string      = (CString *)src;
      op[0] = CString(string).string()[0];
      op[1] = '\0';
      instruction->instruction = ENUM:decode(EGPathInstruction, op);
//>>>      CLEAR(instruction->point);
      if (instruction->instruction == EGPathInstruction.Repeat &&
          CString(string).length() == 1) {
          instruction->instruction = EGPathInstruction.NoOp;
      }
      CString(&data).set(CString(string).string() + 1);
      switch (instruction->instruction) {
      case EGPathInstruction.NoOp:
         break;
      case EGPathInstruction.Repeat:
         /*>>>for now, treat repeat as a move/line */
      case EGPathInstruction.MoveToAbs:
      case EGPathInstruction.MoveToRel:
      case EGPathInstruction.LineToAbs:
      case EGPathInstruction.LineToRel:
      case EGPathInstruction.CurveToAbs:
      case EGPathInstruction.CurveToRel:
      case EGPathInstruction.CurveToShortAbs:
      case EGPathInstruction.CurveToShortRel:
      case EGPathInstruction.QuadToAbs:
      case EGPathInstruction.QuadToRel:
      case EGPathInstruction.QuadToShortAbs:
      case EGPathInstruction.QuadToShortRel:
         cp = CString(&data).string();
         i = 0;
         while (*cp && i < 3) {
            instruction->point[i].x = TGPathInstruction_value(&cp);
            instruction->point[i].y = TGPathInstruction_value(&cp);
            i++;
         }
         break;
      case EGPathInstruction.ArcToAbs:
      case EGPathInstruction.ArcToRel:
         cp = CString(&data).string();
         instruction->point[0].x = TGPathInstruction_value(&cp);
         instruction->point[0].y = TGPathInstruction_value(&cp);
         instruction->point[1].x = TGPathInstruction_value(&cp);
         instruction->point[1].y = TGPathInstruction_value(&cp) == 0 ? 0 : 1;
         instruction->point[1].y += TGPathInstruction_value(&cp) == 0 ? 0 : 2;
         instruction->point[2].x = TGPathInstruction_value(&cp);
         instruction->point[2].y = TGPathInstruction_value(&cp);
         break;
      case EGPathInstruction.HLineToAbs:
      case EGPathInstruction.HLineToRel:
         cp = CString(&data).string();
         while (isspace(*cp))
            cp++;
         sscanf(cp, "%g", &x);
         instruction->point[0].x = x;
         break;
      case EGPathInstruction.VLineToAbs:
      case EGPathInstruction.VLineToRel:
         cp = CString(&data).string();
         while (isspace(*cp))
            cp++;
         sscanf(cp, "%g", &y);
         instruction->point[0].y = y;
         break;
      case EGPathInstruction.ClosePath:
      case EGPathInstruction.ClosePathAlias:
         break;
      }

      delete(&data);

      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TGPathInstruction>) {
      instruction = (TGPathInstruction *)src;
      string      = (CString *)dest;
      CString(string).clear();
      if (instruction->instruction != EGPathInstruction.Repeat) {
         CString(string).printf("%c", *EGPathInstruction(instruction->instruction).name());
      }

      switch (instruction->instruction) {
      case EGPathInstruction.Repeat:
         /*>>>for now, treat repeat as a move/line */
      case EGPathInstruction.MoveToAbs:
      case EGPathInstruction.MoveToRel:
      case EGPathInstruction.LineToAbs:
      case EGPathInstruction.LineToRel:
      case EGPathInstruction.QuadToShortAbs:
      case EGPathInstruction.QuadToShortRel:
         CString(string).printf_append("%g,%g", instruction->point[0].x, instruction->point[0].y);
         break;
      case EGPathInstruction.CurveToAbs:
      case EGPathInstruction.CurveToRel:
         CString(string).printf_append("%g,%g,%g,%g,%g,%g", instruction->point[0].x, instruction->point[0].y,
                                                             instruction->point[1].x, instruction->point[1].y,
                                                             instruction->point[2].x, instruction->point[2].y);
         break;
      case EGPathInstruction.CurveToShortAbs:
      case EGPathInstruction.CurveToShortRel:
      case EGPathInstruction.QuadToAbs:
      case EGPathInstruction.QuadToRel:
         CString(string).printf_append("%g,%g,%g,%g", instruction->point[0].x, instruction->point[0].y,
                                                       instruction->point[1].x, instruction->point[1].y);
         break;
      case EGPathInstruction.ArcToAbs:
      case EGPathInstruction.ArcToRel:
         CString(string).printf_append("%g,%g,%g,%d,%d,%g,%g", instruction->point[0].x, instruction->point[0].y,
                                                                instruction->point[1].x, 
                                                                (((int)instruction->point[1].y) & 1) ? 1 : 0, 
                                                                (((int)instruction->point[1].y) & 2) ? 1 : 0,
                                                                instruction->point[2].x, instruction->point[2].y);
          break;
      case EGPathInstruction.HLineToAbs:
      case EGPathInstruction.HLineToRel:
         CString(string).printf_append("%g", instruction->point[0].x);
         break;
      case EGPathInstruction.VLineToAbs:
      case EGPathInstruction.VLineToRel:
         CString(string).printf_append("%g", instruction->point[0].y);
         break;
      case EGPathInstruction.ClosePath:
      case EGPathInstruction.ClosePathAlias:
         break;
      default:
         break;
      }

      return TRUE;
   }
   return FALSE;
}

void CGPath::new(void) {
   class:base.new();

   ARRAY(&this->instruction).new();
   /*>>>hack till defaults are in place */
   CGPrimitive(this)->stroke = GCOL_NONE;
}/*CGPath::new*/

void CGPath::CGPath(TGColour stroke, TGColour fill) {
   CGPrimitive(this).CGPrimitive(stroke, fill);
}/*CGPath::CGPath*/

void CGPath::delete(void) {
   class:base.delete();

   ARRAY(&this->instruction).delete();
}/*CGPath::delete*/

void CGPath::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGColour stroke, fill;

   _virtual_CGPrimitive_draw(CGPrimitive(this), canvas, extent, mode);

   switch (mode) {
   case EGObjectDrawMode.Draw:
      fill   = CGPrimitive(this)->fill;
      stroke = CGPrimitive(this)->stroke;
      CGCanvas(canvas).stroke_style_set(CGPrimitive(this)->stroke_width,
                                        (ARRAY<int> *)&CGPrimitive(this)->stroke_dasharray,
                                        CGPrimitive(this)->stroke_dashoffset,
                                        CGPrimitive(this)->stroke_miterlimit,
                                        CGPrimitive(this)->stroke_linecap,
                                        CGPrimitive(this)->stroke_linejoin);
      break;
   case EGObjectDrawMode.Select:
      fill   = GCOL_NONE;
      stroke = GCOL_WHITE;
      break;
   default:
      return;
   }
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->fill_opacity);
   CGCanvas(canvas).NATIVE_draw_path(fill, TRUE,
                                      ARRAY(&this->instruction).count(),
                                      ARRAY(&this->instruction).data());
   CGCanvas(canvas).opacity_set(CGPrimitive(this)->stroke_opacity);
   CGCanvas(canvas).NATIVE_draw_path(stroke, FALSE,
                                      ARRAY(&this->instruction).count(),
                                      ARRAY(&this->instruction).data());
}/*CGPath::draw*/

void CGPath::extent_set(CGCanvas *canvas) {
   double x = 0, y = 0;
   TPoint point;
   int i;
   TGPathInstruction *instruction;
   EGPathInstruction ins_op, ins_last_op = EGPathInstruction.NoOp;

   CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);

   for (i = 0; i < ARRAY(&this->instruction).count(); i++) {
      instruction = &ARRAY(&this->instruction).data()[i];

      ins_op = instruction->instruction == EGPathInstruction.Repeat ? ins_last_op : instruction->instruction;
      ins_last_op = ins_op;

      switch (ins_op) {
      case EGPathInstruction.MoveToAbs:
         x = instruction->point[0].x;
         y = instruction->point[0].y;
         break;
      case EGPathInstruction.MoveToRel:
         x += instruction->point[0].x;
         y += instruction->point[0].y;
         break;
      case EGPathInstruction.ClosePath:
      case EGPathInstruction.ClosePathAlias:
         break;
      case EGPathInstruction.LineToAbs:
         x = instruction->point[0].x;
         y = instruction->point[0].y;
         break;
      case EGPathInstruction.LineToRel:
         x += instruction->point[0].x;
         y += instruction->point[0].y;
         break;
      case EGPathInstruction.HLineToAbs:
         x = instruction->point[0].x;
         break;
      case EGPathInstruction.HLineToRel:
         x += instruction->point[0].x;
         break;
      case EGPathInstruction.VLineToAbs:
         y = instruction->point[0].y;
         break;
      case EGPathInstruction.VLineToRel:
         y += instruction->point[0].y;
         break;
      default:
         break;
      }
      point.x = (int)(x > 0 ? (x + 0.5) : (x - 0.5));
      point.y = (int)(y > 0 ? (y + 0.5) : (y - 0.5));
      CGCoordSpace(&canvas->coord_space).point_utov(&point);
      CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 1, &point);
   }
   CGCoordSpace(&canvas->coord_space).extent_extend(&CGObject(this)->extent, CGPrimitive(this)->stroke_width / 2);
}/*CGPath::extent_set*/

bool CGPath::transform_assimilate(TGTransform *transform) {
   bool result = FALSE;
   int i;
   TGPathInstruction *instruction;
   EGPathInstruction ins_op, ins_last_op = EGPathInstruction.NoOp;

   switch (transform->type) {
   case EGTransform.scale:
      for (i = 0; i < ARRAY(&this->instruction).count(); i++) {
         instruction = &ARRAY(&this->instruction).data()[i];
         ins_op = instruction->instruction == EGPathInstruction.Repeat ? ins_last_op : instruction->instruction;
         ins_last_op = ins_op;

         instruction->point[0].x *= transform->t.scale.sx;
         instruction->point[0].y *= transform->t.scale.sy;
      }
      result = TRUE;
      break;
   case EGTransform.translate:
      for (i = 0; i < ARRAY(&this->instruction).count(); i++) {
         instruction = &ARRAY(&this->instruction).data()[i];
         ins_op = instruction->instruction == EGPathInstruction.Repeat ? ins_last_op : instruction->instruction;
         ins_last_op = ins_op;

         switch (ins_op) {
         case EGPathInstruction.MoveToAbs:
            instruction->point[0].x += transform->t.translate.tx;
            instruction->point[0].y += transform->t.translate.ty;
            break;
         case EGPathInstruction.MoveToRel:
            break;
         case EGPathInstruction.ClosePath:
         case EGPathInstruction.ClosePathAlias:
            break;
         case EGPathInstruction.LineToAbs:
            instruction->point[0].x += transform->t.translate.tx;
            instruction->point[0].y += transform->t.translate.ty;
            break;
         case EGPathInstruction.LineToRel:
            break;
         case EGPathInstruction.HLineToAbs:
            break;
         case EGPathInstruction.HLineToRel:
            break;
         case EGPathInstruction.VLineToAbs:
            break;
         case EGPathInstruction.VLineToRel:
            break;
         default:
            break;
         }
      }
      result = TRUE;
      break;
   default:
      break;
   }

   class:base.transform_assimilate(transform);

   return result;
}/*CGPath::transform_assimilate*/

void CGText::new(void) {
   class:base.new();

   new(&this->text).CString(NULL);
   CString(&this->text).encoding_set(EStringEncoding.UTF16);
   new(&this->font_family).CGFontFamily();
//   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGText,font_family>, TRUE);
//   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGText,font_size>, TRUE);
}/*CGText::new*/

void CGText::CGText(TGColour fill,
                     double x, double y, const char *text) {
   CGPrimitive(this).CGPrimitive(GCOL_NONE, fill);
   this->x      = x;
   this->y      = y;
   CString(&this->text).set(text);
   CString(&this->text).encoding_set(EStringEncoding.UTF16);
}/*CGText::CGText*/

void CGTextSpan::CGTextSpan(void) {
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGText,font_family>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGText,font_size>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,fill>, TRUE);
}/*CGTextSpan::CGTextSpan*/

void CGText::delete(void) {
   delete(&this->font_family);
   delete(&this->text);

   class:base.delete();
}/*CGText::delete*/

TParameter __parameter_CGText = {
   &class(CGText),
   PT_Element,
   &__attributetype_CGText,
};

bool ATTRIBUTE:convert<CGText>(CObjPersistent *object,
                               const TAttributeType *dest_type, const TAttributeType *src_type,
                               int dest_index, int src_index,
                               void *dest, const void *src) {
   CGText *value;
   CString *string;
   CObject *child, *next;
   short *character;
   CGText *dest_text;

   /*>>>should not need this! */
   if (dest_type == &ATTRIBUTE:type<int>) {
      *((int *)dest) = 0;
      return TRUE;
   }
   
   if (dest_type == &ATTRIBUTE:type<CGText> && src_type == NULL) {
      memset(dest, 0, sizeof(CGText));
      return FALSE;
   }
   if (dest_type == &ATTRIBUTE:type<CGText> && src_type == &ATTRIBUTE:type<CGText>) {
      *((CGText *)dest) = *((CGText *)src);
      return FALSE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CGText>) {
      return FALSE;
   }

   if (dest_type == &ATTRIBUTE:type<CGText> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (CGText *)dest;
      string = (CString *)src;
      CString(string).encoding_set(EStringEncoding.UTF16);                  

      child = CObject(value).child_first();
      while (child) {
         next = CObject(value).child_next(child);
         if (CObject(child).obj_class() == &class(CGTextSpan)) {
            delete(child);
         }
         child = next;
      }

      /* Process character by character, not ideal */
      character = (short *)ARRAY(&string->data).data();
      dest_text = value;
      CString(&dest_text->text).clear();
      while (*character) {
         if (*character == '\015' || *character == '\012') {
            dest_text = NULL;
         }
         else {
            if (!dest_text) {
               dest_text = (CGText *)new.CGTextSpan();
               CObject(value).child_add(CObject(dest_text));
               CObjPersistent(dest_text).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke>, TRUE);
               CObjPersistent(dest_text).attribute_default_set(ATTRIBUTE<CGPrimitive,fill>, TRUE);
               CObjPersistent(dest_text).attribute_default_set(ATTRIBUTE<CGText,font_family>, TRUE);
               CObjPersistent(dest_text).attribute_default_set(ATTRIBUTE<CGText,font_weight>, TRUE);
               CObjPersistent(dest_text).attribute_default_set(ATTRIBUTE<CGText,font_style>, TRUE);
               CObjPersistent(dest_text).attribute_default_set(ATTRIBUTE<CGText,font_size>, TRUE);
               CObjPersistent(dest_text).attribute_default_set(ATTRIBUTE<CGText,text_anchor>, TRUE);
               CObjPersistent(dest_text).attribute_default_set(ATTRIBUTE<CGText,dominant_baseline>, TRUE);
               CObjPersistent(dest_text).attribute_default_set(ATTRIBUTE<CGText,text_decoration>, TRUE);
            }
            CString(&dest_text->text).append(*character);
         }
         character++;
      }

      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<CGText>) {
      value  = (CGText *)src;
      string = (CString *)dest;

      CString(string).set_string(&value->text);
      child = CObject(value).child_first();
      while (child) {
         if (CObject(child).obj_class() == &class(CGTextSpan)) {
            if (CString(&CGText(child)->text).length()) {
               CString(string).append('\015');
               CString(string).append('\012');
               CString(string).append_string(&CGText(child)->text);
            }
         }
         child = CObject(value).child_next(child);
      }

      return TRUE;
   }

   return FALSE;
}

void CGText::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGColour fill;
   TRect rect;
   CObject *child;

   class:base.draw(canvas, extent, mode);

   CGCanvas(canvas).font_set(&this->font_family, this->font_size,
                             this->font_style, this->font_weight, this->text_decoration);
   CGCanvas(canvas).NATIVE_text_align_set(this->text_anchor, this->dominant_baseline);

   switch (mode) {
   case EGObjectDrawMode.Draw:
      CGCanvas(canvas).stroke_style_set(CGPrimitive(this)->stroke_width,
                                        (ARRAY<int> *)&CGPrimitive(this)->stroke_dasharray,
                                        CGPrimitive(this)->stroke_dashoffset,
                                        CGPrimitive(this)->stroke_miterlimit,
                                        CGPrimitive(this)->stroke_linecap,
                                        CGPrimitive(this)->stroke_linejoin);
      fill = CGPrimitive(this)->fill;
      CGCanvas(canvas).opacity_set(CGPrimitive(this)->fill_opacity);
      CGCanvas(canvas).draw_text_string(fill, CGText(this)->x, CGText(this)->y, &this->text);
      break;
   case EGObjectDrawMode.Select:
      fill = GCOL_WHITE;
      CGCanvas(canvas).NATIVE_text_extent_get_string(this->x, this->y, &this->text,
                                              &rect);
      CGCanvas(canvas).draw_rectangle(fill, FALSE, rect.point[0].x, rect.point[0].y, rect.point[1].x, rect.point[1].y);
      child = CObject(this).child_first();
      while (child) {
         if (CObject(child).obj_class() == &class(CGTextSpan) && !CString(&CGText(child)->text).empty()) {
            CGCanvas(canvas).NATIVE_text_extent_get_string(CGText(child)->x, CGText(child)->y, &CGText(child)->text,
                                                            &rect);
            CGCanvas(canvas).draw_rectangle(fill, FALSE, rect.point[0].x, rect.point[0].y, rect.point[1].x, rect.point[1].y);
         }
         child = CObject(this).child_next(child);
      }
      return;
   default:
      return;
   }
}/*CGText::draw*/

void CGText::extent_set(CGCanvas *canvas) {
   TPoint point[4];
   TRect rect;
   TGTransform transform;
   CObject *child;
   
   CLEAR(&transform);
   transform.type = EGTransform.scale;
   transform.t.scale.sx = 10;
   transform.t.scale.sy = 10;

   CGCanvas(canvas).NATIVE_enter(TRUE);
   CGCoordSpace(&canvas->coord_space).push();
   CGCoordSpace(&canvas->coord_space).transform_apply(&transform, FALSE);
   CGCanvas(canvas).font_set(&this->font_family, this->font_size,
                             this->font_style, this->font_weight, this->text_decoration);
   CGCanvas(canvas).NATIVE_text_align_set(this->text_anchor, this->dominant_baseline);

   CGCanvas(canvas).NATIVE_text_extent_get_string(this->x, this->y, &this->text,
                                                  &rect);
   CGCoordSpace(&canvas->coord_space).pop();
   CGCanvas(canvas).NATIVE_enter(FALSE);

   point[0].x = rect.point[0].x;
   point[0].y = rect.point[0].y;
   point[1].x = rect.point[1].x;
   point[1].y = rect.point[0].y;
   point[2].x = rect.point[1].x;
   point[2].y = rect.point[1].y;
   point[3].x = rect.point[0].x;
   point[3].y = rect.point[1].y;

   CGCoordSpace(&canvas->coord_space).point_array_utov(4, point);
   CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);
   CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 4, point);

   child = CObject(this).child_first();
   while (child) {
      if (CObject(child).obj_class() == &class(CGTextSpan) && !CString(&CGText(child)->text).empty()) {
         CGPrimitive(child).extent_set(canvas);
         CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 2, CGObject(child)->extent.point);
      }
      child = CObject(this).child_next(child);
   }
}/*CGText::extent_set*/

void CGText::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(1);

   ARRAY(point).data()[0].x = this->x;
   ARRAY(point).data()[0].y = this->y;
}/*CGText::cpoint_get*/

void CGText::cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new) {
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGText,x>);
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGText,y>);

   switch (index) {
   case -1:
      this->x = ARRAY(point).data()[0].x;
      this->y = ARRAY(point).data()[0].y;
      break;
   default:
      ARRAY(point).data()[index] = *point_new;
      this->x = ARRAY(point).data()[0].x;
      this->y = ARRAY(point).data()[0].y;
      break;
   }
   CObjPersistent(this).attribute_update_end();
}/*CGText::cpoint_set*/

void CGText::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObject *child;

   class:base.notify_attribute_update(attribute, changing);

   if (changing) {
      return;
   }

   child = CObject(this).child_first();
   while (child) {
      if (CObject(child).obj_class() == &class(CGTextSpan)) {
         CObjPersistent(child).attribute_update(ATTRIBUTE<CGText,x>);
         CObjPersistent(child).attribute_update(ATTRIBUTE<CGText,y>);
         CObjPersistent(child).attribute_update(ATTRIBUTE<CGTextSpan,dy>);
         CObjPersistent(child).attribute_set_double(ATTRIBUTE<CGTextSpan,dy>, this->font_size);
         CObjPersistent(child).attribute_set_double(ATTRIBUTE<CGText,x>, this->x);
         CObjPersistent(child).attribute_default_set(ATTRIBUTE<CGText,x>, FALSE);
         CObjPersistent(child).attribute_default_set(ATTRIBUTE<CGText,y>, TRUE);
         CObjPersistent(child).attribute_default_set(ATTRIBUTE<CGTextSpan,dy>, FALSE);
         CObjPersistent(child).attribute_update_end();
      }
      child = CObject(this).child_next(child);
   }
}/*CGText::notify_attribute_update*/

bool CGText::transform_assimilate(TGTransform *transform) {
   bool result = FALSE;
   double font_scale;

   switch (transform->type) {
   case EGTransform.scale:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGText,x>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGText,y>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGText,font_size>);
      this->x = this->x * transform->t.scale.sx;
      this->y = this->y * transform->t.scale.sy;
      font_scale = transform->t.scale.sx > transform->t.scale.sy ? transform->t.scale.sy : transform->t.scale.sx;
      //this->font_size = this->font_size * font_scale;
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGText,font_size>, (int)((double)this->font_size * font_scale));
      CObjPersistent(this).attribute_update_end();
      result = TRUE;
      break;
   case EGTransform.translate:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGText,x>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGText,y>);
      this->x += transform->t.translate.tx;
      this->y += transform->t.translate.ty;
      CObjPersistent(this).attribute_update_end();
      result = TRUE;
      break;
   default:
      break;
   }
   class:base.transform_assimilate(transform);

   return result;
}/*CGText::transform_assimilate*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
