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
#include "../event.c"
#include "../objpersistent.c"
#include "../objcontainer.c"
#include "gpalette.c"
#include "gcoord.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGCanvas;
class CGObjCanvas;

ENUM:typedef<EEventGObject> {
   {focusin},
   {focusout},
   {activate},
   {click},
   {clickRight},
   {mousedown},
   {mouseup},
   {mouseover},
   {mousemove},
   {mouseout},
   {load},
};

ENUM:typedef<EEventPointer> {
   {Move,        "Move"         },
   {Hover,       "Hover"        },   
   {Leave,       "Leave"        },
   {LeftDown,    "Left Down"    },
   {LeftUp,      "Left Up"      },
   {LeftDClick,  "Left DClick"  },
   {RightDown,   "Right Down"   },
   {RightUp,     "Right Up"     },
   {RightDClick, "Right DClick" },
   {ScrollUp,    "Scroll Up"    },
   {ScrollDown,  "Scroll Down"  },   
};

ENUM:typedef<EGVisibility> {
   {hidden}, {visible}, {collapse}
};

class CGFontFamily : CString {
 public:
   void CGFontFamily(void);
};

ATTRIBUTE:typedef<CGFontFamily>;

ENUM:typedef<EGFontStyle> {
   {normal},
   {italic},
};

ENUM:typedef<EGFontWeight> {
   {normal},
   {bold},
   {bolder},
   {lighter},
};

ENUM:typedef<EGTextAnchor> {
   {start},
   {middle},
   {end},
};

ENUM:typedef<EGTextBaseline> {
   {alphabetic},
   {central},
   {middle},
//  {text_top, "text-top"},
//  {text_bottom, "text-bottom"},
   {text_before_edge, "text-before-edge"},
   {text_after_edge, "text-after-edge"},
};

#if 1
ENUM:typedef<EGTextDecoration> {
   {none},
   {underline},
};
#else
ENUM:typedef<EGTextDecoration> {
   {none},
   {underline},
   {overline},
   {line_through, "line-through"},
};
#endif

class CEventPointer : CEvent {
 public:
   EEventPointer type;
   TPoint position;
   int modifier;
   void CEventPointer(EEventPointer type, double x, double y, int modifier);
};

class CEventDraw : CEvent {
 public:
   CGCanvas *canvas;
   TRect *extent;

   void CEventDraw(CGCanvas *canvas, TRect *extent);
};

ENUM:typedef<EGObjectDrawMode> {
   {Draw}, {Select}, {HighLight}
};

class CGObject : CObjPersistent {
 private:
   TRect extent;                    /* Bordering rectangle which object takes in object canvas */
   void *native_object;             /* System specific UI object (win32 window, gtk widget, etc) */

   void new(void);
   void delete(void); 

   virtual void show(bool show);
   static inline void visibility_set(EGVisibility visibility);
 public:
   void CGObject(void);

   ATTRIBUTE<CString id>;
   ATTRIBUTE<EGVisibility visibility, ,PO_INHERIT> {
      if (data) {
         *data = *data ? 1 : 0; /*>>>for now */
         CGObject(this).visibility_set(*data);
      }
      else {
         CGObject(this).visibility_set(EGVisibility.visible);
      }
   };
   ATTRIBUTE<ARRAY<TGTransform> transform, ,PO_ELEMENT_STRIPSPACES>;
 protected:
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
//   virtual void render_export(CGCanvas *canvas, CGObject *parent);
   virtual void extent_set(CGCanvas *canvas);
   virtual bool event(CEvent *event);
   virtual bool keyboard_input(void);
   virtual void create_point_set(TPoint *point);
   virtual void cpoint_get(ARRAY<TPoint> *point);
   virtual void cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new);
   virtual void select_area_get(CGCanvas *canvas, ARRAY<TPoint> *point);
   virtual bool transform_assimilate(TGTransform *transform);
   bool transform_invert(TGTransform *transform);
   bool transform_list_apply(ARRAY<TGTransform> *list, TGTransform *transform);

   bool transform_reduce(void);
//   virtual void gselection_event(CGSelection *selection, TEvent *event);
};

static inline void CGObject::visibility_set(EGVisibility visibility) {
   CGObject *parent = CGObject(CObject(this).parent_find(&class(CGObject)));
   
   if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGObject,visibility>) || !parent) {
      this->visibility = visibility;
   }
   else {
      this->visibility = parent->visibility ? visibility : FALSE;
   }
}/*CGObject::visibility_set*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CEventPointer::CEventPointer(EEventPointer type, double x, double y, int modifier) {
   this->type       = type;
   this->position.x = x;
   this->position.y = y;
   this->modifier   = modifier;
}/*CEventPointer::CEventPointer*/

void CEventDraw::CEventDraw(CGCanvas *canvas, TRect *extent) {
   CEvent(this).CEvent();
   this->canvas = canvas;
   this->extent = extent;
}/*CEventDraw::CEventDraw*/

void CGObject::new(void) {
   class:base.new();
   this->visibility = EGVisibility.visible;   /*>>>should be handled be default*/
   ARRAY(&this->transform).new();
   new(&this->id).CString(NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGObject,visibility>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGObject,id>, TRUE);
}/*CGObject::new*/

void CGObject::CGObject(void) {
}/*CGObject::CGObject*/

void CGObject::delete(void) {
   delete(&this->id);
   ARRAY(&this->transform).delete();

   class:base.delete();    
}/*CGObject::delete*/

/*>>>these probably best belong in gcoord.c */
bool CGObject::transform_invert(TGTransform *transform) {
   switch (transform->type) {
   case EGTransform.translate:
      transform->t.translate.tx = -transform->t.translate.tx;
      transform->t.translate.ty = -transform->t.translate.ty;
      break;
   case EGTransform.scale:
      transform->t.scale.sx = 1 / transform->t.scale.sx;
      transform->t.scale.sy = 1 / transform->t.scale.sy;
      break;
   default:
      return FALSE;
   }
   return TRUE;
}/*CGObject::transform_invert*/

bool CGObject::transform_list_apply(ARRAY<TGTransform> *list, TGTransform *transform) {
   TGTransform *object_transform = NULL, *object_scale, tr_rotate = {EGTransform.none};
   int count = ARRAY(list).count();
   int form = -1;
   bool has_rotation = FALSE;

   if (count >= 1 && ARRAY(list).data()[count - 1].type == EGTransform.rotate) {
        /* preserver a single rotation at end of transform list alone if present */
      tr_rotate = ARRAY(list).data()[count - 1];
      ARRAY(list).used_set(count - 1);
      count--;
      has_rotation = TRUE;
   }

   if (count == 0) {
      form = 0;
   }
   else if (count == 1 && ARRAY(list).data()[0].type == EGTransform.translate) {
      form = 1;
   }
   else if (count == 1 && ARRAY(list).data()[0].type == EGTransform.scale) {
      form = 2;
   }
   else if (count == 2 && ARRAY(list).data()[0].type == EGTransform.translate &&
                           ARRAY(list).data()[1].type == EGTransform.scale) {
      form = 3;
   }

   switch (form) {
   case 0:
      ARRAY(list).item_add(*transform);
      break;
   case 1:
      if (transform->type == EGTransform.translate) {
          object_transform = ARRAY(list).data();
          object_transform->t.translate.tx += transform->t.translate.tx;
          object_transform->t.translate.ty += transform->t.translate.ty;
          break;
      }
      else {
         ARRAY(list).item_add_empty();
         object_transform = ARRAY(list).item_last();

         CLEAR(object_transform);
         object_transform->type = EGTransform.scale;
         object_transform->t.scale.sx = 1;
         object_transform->t.scale.sy = 1;
      }
      goto trans_scale;
   case 2:
      if (transform->type == EGTransform.scale) {
         object_transform = ARRAY(list).data();
         object_transform->t.scale.sx *= transform->t.scale.sx;
         object_transform->t.scale.sy *= transform->t.scale.sy;
         break;
      }
      else {
         ARRAY(list).item_add(ARRAY(list).data()[0]);
         object_transform = &ARRAY(list).data()[0];

         CLEAR(object_transform);
         object_transform->type = EGTransform.translate;
         object_transform->t.translate.tx = 0;
         object_transform->t.translate.ty = 0;
      }
      goto trans_scale;
   trans_scale:
   case 3:
      object_transform = &ARRAY(list).data()[0];
      object_scale     = &ARRAY(list).data()[1];

      if (transform->type == EGTransform.translate) {
         object_transform->t.translate.tx += transform->t.translate.tx;
         object_transform->t.translate.ty += transform->t.translate.ty;
      }

      if (transform->type == EGTransform.scale) {
         object_transform->t.translate.tx *= transform->t.scale.sx;
         object_transform->t.translate.ty *= transform->t.scale.sy;

         object_scale->t.scale.sx *= transform->t.scale.sx;
         object_scale->t.scale.sy *= transform->t.scale.sy;
      }
      break;
   default:
      return FALSE;
   }

   if (has_rotation) {
      ARRAY(list).item_add(tr_rotate);
   }

   return TRUE;
}/*CGObject::transform_list_apply*/

void CGObject::extent_set(CGCanvas *canvas) {
   CLEAR(&this->extent);
}/*CGObject::extent_set*/

void CGObject::show(bool show) {}
void CGObject::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {}
bool CGObject::event(CEvent *event) { return FALSE; }

void CGObject::create_point_set(TPoint *point) {}
void CGObject::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(0);
}/*CGObject::cpoint_get*/

void CGObject::cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new) {}
bool CGObject::transform_assimilate(TGTransform *transform) {
   return TRUE;
}

bool CGObject::transform_reduce(void) {
   bool result;
   int count;
   TGTransform *transform;
   ARRAY<TGTransform> tlist;
   /* attempt to assimilate transforms on this object into itself */
   
   ARRAY(&tlist).new();
   ARRAY(&tlist).data_set(ARRAY(&this->transform).count(), ARRAY(&this->transform).data());

   result = TRUE;
CObjPersistent(this).attribute_update(ATTRIBUTE<CGObject,transform>);
   ARRAY(&this->transform).used_set(0);
CObjPersistent(this).attribute_update_end();
   count = ARRAY(&tlist).count();    
   while (count) {
      transform = &ARRAY(&tlist).data()[count - 1];
      if (!CGObject(this).transform_assimilate(transform)) {
         break;
      }
      ARRAY(&tlist).used_set(count - 1);
      count = ARRAY(&tlist).count();
   }
   
   if (ARRAY(&tlist).count()) {
CObjPersistent(this).attribute_update(ATTRIBUTE<CGObject,transform>);
      ARRAY(&this->transform).data_set(ARRAY(&tlist).count(), ARRAY(&tlist).data());
CObjPersistent(this).attribute_update_end();
      result = FALSE;
   }

   ARRAY(&tlist).delete();

   return result;
};

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/