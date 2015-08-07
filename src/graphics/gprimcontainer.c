/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/
/*>>>
  Symbols implementation in need of overhaul
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "../framework_base.c"
#include "gprimitive.c"
#include "glayout.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGUse;
ARRAY:typedef<CGAnimation *>;

class CGLayoutSymbol : CGLayout {
   void new(void);
   void CGLayoutSymbol(int width, int height, CObjServer *obj_server, CObjPersistent *data_source);
};

class CGParameterExport : CObjPersistent {
 private:
   void new(void);
   void delete(void);
 public:
   ALIAS<"svg:export">;

   ATTRIBUTE<TAttributePtr attribute_name, "attributeName">;
   ATTRIBUTE<CString export_name, "exportName">;

   void CGParameterExport(TAttributePtr *attributeName);
};

ARRAY:typedef<CGParameterExport *>;

ENUM:typedef<EParameterState> {
   {present},
   {presentUsed},
   {created},
};

class CGParameter : CObjPersistent {
 private:
   EParameterState state;
   const TAttributeType *data_type;
   void new(void);
   void delete(void);

   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);

   virtual void value_initial_assimilate(const void *data);
   virtual void value_set(CString *data);
   virtual void value_export_set(CGParameterExport *export);
 public:
   ALIAS<"svg:parameter">;
   ATTRIBUTE<CString name>;
   ATTRIBUTE<CString value> {
      CGParameter(this).value_set(data);  
   };
   bool value_initial_valid;
   CString value_initial;
   ATTRIBUTE<CString export_name, "exportName">;
   void CGParameter(const char *name);
};

class CGParameters : CObjPersistent {
 private:
   void new(void);
   void delete(void);
 public:
   ALIAS<"svg:parameters">;
   ATTRIBUTE<CString object_name, "name"> {
      if (data) {
         CString(&this->object_name).set_string(data);
      }
      else {
         CString(&this->object_name).clear();
      }
      if (!CObjPersistent(this).state_loading()) {
         CGParameters(this).export_defaults();
      }
   };
   void CGParameters(void);
   virtual CObjClass *parameter_class(void);
   CGParameter *parameter_find(const char *name);
   void clear(void);
   void rebind(void);

   void export_defaults(void);
};

class CGPrimContainer : CGPrimitive {
 private:
   void new(void);
   void delete(void);

   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);

   virtual void show(bool show);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void extent_set(CGCanvas *canvas);
   virtual void transform_assimilate(TGTransform *transform);
   double stroke_width_max;
 public:
   ATTRIBUTE<bool disabled, , PO_INHERIT>;
   ATTRIBUTE<CGFontFamily font_family, "font-family", PO_INHERIT>;
   ATTRIBUTE<EGFontStyle font_style, "font-style", PO_INHERIT>;
   ATTRIBUTE<EGFontWeight font_weight, "font-weight", PO_INHERIT>;
   ATTRIBUTE<int font_size, "font-size", PO_INHERIT>;
   ATTRIBUTE<EGTextAnchor text_anchor, "text-anchor", PO_INHERIT>;
   ATTRIBUTE<EGTextBaseline dominant_baseline, "dominant-baseline", PO_INHERIT>;
   ATTRIBUTE<CString tooltiptext>;

   void CGPrimContainer(void);
};

class CGSymbol : CGPrimContainer {
 private:
   void new(void);
   void delete(void);

   CXPath initial_path;
 public:
   ALIAS<"svg:symbol">;
   ATTRIBUTE<TCoord ref_x, "refX">;
   ATTRIBUTE<TCoord ref_y, "refY">;

   void CGSymbol(void);
};

ENUM:typedef<EGMarkerUnits> {
   {strokeWidth},
   {userSpaceOnUse},
};

ENUM:typedef<EGMarkerOrient> {
   {automatic, "auto"},
   {none, "0"},
};

class CGMarker : CGSymbol {
 private:
   void new(void);
   void delete(void);
 public:
   ALIAS<"svg:marker">;
   ATTRIBUTE<TRect view_box, "viewBox">;
   ATTRIBUTE<EGMarkerUnits marker_units, "markerUnits">;
   ATTRIBUTE<TCoord marker_width, "markerWidth">;
   ATTRIBUTE<TCoord marker_height, "markerHeight">;
   ATTRIBUTE<EGMarkerOrient orient>;

   void CGMarker(void);
};

class CGGroup : CGPrimContainer {
 private:
   void delete(void);
 public:
   ALIAS<"svg:g">;

   void CGGroup(void);

//   virtual void show(bool show);
};

ENUM:typedef<EGUseState> {
   {reloadNeeded},
   {notFound},
   {loading},
   {rebindNeeded},
   {resolved},
};

class CGUse : CGPrimContainer {
 private:
   EGUseState state;
   CGParameters *active_parameters;
   TGColour colour_background;

   void new(void);
   void delete(void);
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_all_update(bool changing);

   void extent_set(CGCanvas *canvas);

   CObjPersistent *symbol_resolve(void);
   void realize(CObjPersistent *object);
   void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   void resolve_children(void);
 public:
   ALIAS<"svg:use">;
   OPTION<nochild>;

   ELEMENT:OBJECT<CGParameters parameters, "svg:parameters">;
   ATTRIBUTE<CXLink link, "xlink:href"> {
      if (data) {
         __attributetypeconvert_CXLink(NULL, &__attributetype_CXLink, &__attributetype_CXLink, -1, -1,
                                              (void *)&this->link, (void *)data);
         this->state = EGUseState.reloadNeeded;
      }
   };
   ATTRIBUTE<int x>;
   ATTRIBUTE<int y>;
   ATTRIBUTE<int width>;
   ATTRIBUTE<int height>;
   ATTRIBUTE<int index>;
   ATTRIBUTE<CXPath symbolPath> {
      if (data) {
         CXPath(&this->symbolPath).path_set(CString(&data->path).string());
      }
      else {
         CXPath(&this->symbolPath).path_set(".");
      }
   };

   void CGUse(void);
   virtual void resolve(void);
};

class CGArray : CGUse {
 private:
   void new(void);
   void delete(void);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);

   /*>>>would be better to just use XPath once XPath supports multiple object selections */
   virtual void selection_update(void);
 public:
   ALIAS<"svg:array">;
   OPTION<nochild>;

   ATTRIBUTE<CXPath selection>;
   ATTRIBUTE<int elements>;
   ATTRIBUTE<int x_spacing, "spacingX">;
   ATTRIBUTE<int y_spacing, "spacingY">;

   void CGArray(void);
   virtual void resolve(void);
};

class CGLink : CGUse {
 private:
   void new(void);
   void delete(void);
   
   virtual void resolve(void);
 public:
   ALIAS<"svg:a">;

//   ATTRIBUTE<CXLink link, "xlink:href">;
   ATTRIBUTE<EXLinkShow show, "xlink:show"> {
      if (data) {
         this->show = *data;
      }
      else {
         this->show = EXLinkShow.replace;
      }
   };

   void CGLink(void);

   bool event(CEvent *event);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CGLayoutSymbol::new(void) {
   CGLayout(this)->frame_length = -1;   /* Indicates layout starts frozen */
   CGCanvas(this)->translate_palette = TRUE;

   class:base.new();
};
void CGLayoutSymbol::CGLayoutSymbol(int width, int height, CObjServer *obj_server, CObjPersistent *data_source) {
   CGLayout(this).CGLayout(width, height, obj_server, data_source);
};


void CGPrimitive::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
//   static TRect extent; /*>>>hack, get last position from object server*/
   double stroke_width_max;
#if 0
   CGPrimContainer *container = CGPrimContainer(CObject(this).parent_find(&class(CGPrimContainer)));
   if (container) {
      CObjPersistent(container).notify_attribute_update(NULL, changing);
      return;
   }
#endif

   if (CObjClass_is_derived(&class(CGPrimContainer), CObject(this).obj_class())) {
      stroke_width_max = CGPrimContainer(this)->stroke_width_max;
   }
   else {
     stroke_width_max = this->stroke_width;
   }

   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   if (layout) {
      if (changing) {
         CGCanvas(layout).transform_set_gobject(CGObject(this), FALSE);
         CGObject(this).extent_set(CGCanvas(layout));
         memcpy(&this->redraw_extent, &CGObject(this)->extent, sizeof(TRect));
      }
      else {
//>>>?         CGCanvas(layout).queue_draw(&extent);
         CGCanvas(layout).transform_set_gobject(CGObject(this), FALSE);
         CGObject(this).extent_set(CGCanvas(layout));
         CGCoordSpace(&CGCanvas(layout)->coord_space).extent_add(&this->redraw_extent, 1, &CGObject(this)->extent.point[0]);
         CGCoordSpace(&CGCanvas(layout)->coord_space).extent_add(&this->redraw_extent, 1, &CGObject(this)->extent.point[1]);
         /*>>>quick hack, get redraw extent*/
//>>>         CGCoordSpace(&CGCanvas(layout)->coord_space).extent_extend(&extent, (int)(stroke_width_max / 2));

         CGCoordSpace(&CGCanvas(layout)->coord_space).reset();
         CGCanvas(layout).transform_viewbox();
         CGCanvas(layout).point_array_utov(2, this->redraw_extent.point);
         CGCanvas(layout).queue_draw(&this->redraw_extent);
      }
   }
}/*CGPrimitive::notify_attribute_update*/

bool CGPrimitive::transform_assimilate(TGTransform *transform) {
   return TRUE;
}/*CGPrimitive::transform_assimilate*/

void CGSymbol::new(void) {
   class:base.new();

   new(&this->initial_path).CXPath(NULL, NULL);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGSymbol,ref_x>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGSymbol,ref_y>, TRUE);
}/*CGSymbol::new*/

void CGSymbol::CGSymbol(void) {
}/*CGSymbol::CGSymbol*/

void CGSymbol::delete(void) {
   class:base.delete();
   delete(&this->initial_path);
}/*CGSymbol::delete*/

void CGMarker::new(void) {
   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,view_box>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,marker_units>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,marker_width>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,marker_height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,orient>, TRUE);
}/*CGMarker::new*/

void CGMarker::CGMarker(void) {
}/*CGMarker::CGMarker*/

void CGMarker::delete(void) {
   class:base.delete();
}/*CGMarker::delete*/

void CGPrimContainer::new(void) {
   class:base.new();

   new(&this->font_family).CGFontFamily();
   new(&this->tooltiptext).CString(NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimContainer,font_family>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimContainer,font_style>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimContainer,font_weight>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimContainer,font_size>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimContainer,text_anchor>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimContainer,dominant_baseline>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimContainer,tooltiptext>, TRUE);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,fill>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimContainer,disabled>, TRUE);
}/*CGPrimContainer::new*/

void CGPrimContainer::CGPrimContainer(void) {
}/*CGPrimContainer::CGPrimContainer*/

void CGPrimContainer::delete(void) {
   delete(&this->tooltiptext);
   delete(&this->font_family);

   class:base.delete();
}/*CGPrimContainer::delete*/

void CGPrimContainer::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGCanvas *canvas;

   if (linkage == EObjectLinkage.ChildAdd) {
      /*>>>canvas pointer in native object, a bit kludgy*/
      canvas = CGCanvas(CGObject(this)->native_object);

      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
         if (canvas) {
            CGCanvas(canvas).queue_draw(NULL);
         }
         //>>>CGObject(object).show(TRUE);
      }
   }

   /*>>>quick hack*/
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);
}/*CGPrimContainer::notify_object_linkage*/

void CGPrimContainer::show(bool show) {
   CObject *object = CObject(this).child_first();

   /* show all children of container object */
   while (object) {
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
         CGObject(object).show(show);
      }
      object = CObject(this).child_next(object);
   }
}/*CGPrimContainer::show_all*/

void CGPrimContainer::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   CObject *object = CObject(this).child_first();
   CGCanvas *object_canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
#ifdef TEST   
   TRect gobj_extent, canvas_extent;
#endif

   /*>>>hack*/
   if (!object_canvas) {
      object_canvas = canvas;
   }

   CGObject(this)->native_object = (void *)object_canvas;

   while (object) {
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())
          && CGObject(object)->visibility == EGVisibility.visible) {
#ifdef TEST
         if (extent) {
            gobj_extent = CGObject(object)->extent;
            CGCoordSpace(&canvas->coord_space).push();
            CGCoordSpace(&canvas->coord_space).reset();
            CGCanvas(canvas).transform_viewbox();
            canvas_extent = *extent;
            CGCanvas(canvas).point_array_utov(2, gobj_extent.point);
            CGCoordSpace(&canvas->coord_space).pop();
            if (TRect_overlap(&canvas_extent, &gobj_extent)) {
               CGCoordSpace(&canvas->coord_space).push();
               CGCanvas(canvas).transform_prepend_gobject(CGObject(object));
               CGObject(object).draw(canvas, extent, mode);
               CGCoordSpace(&canvas->coord_space).pop();
            }
         }
         else {
            CGCoordSpace(&canvas->coord_space).push();
            CGCanvas(canvas).transform_prepend_gobject(CGObject(object));
            CGObject(object).draw(canvas, extent, mode);
            CGCoordSpace(&canvas->coord_space).pop();
         }
#else
         CGCoordSpace(&canvas->coord_space).push();
         CGCanvas(canvas).transform_prepend_gobject(CGObject(object));
         CGObject(object).draw(canvas, extent, mode);
         CGCoordSpace(&canvas->coord_space).pop();
#endif
      }
      object = CObject(this).child_next(object);
   }
}/*CGPrimContainer::draw*/

void CGPrimContainer::extent_set(CGCanvas *canvas) {
   CObject *child = CObject(this).child_first();

   CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);
   this->stroke_width_max = 0;
   while (child) {
      if (CObjClass_is_derived(&class(CGObject), CObject(child).obj_class())) {
#ifdef TEST
         CGCoordSpace(&canvas->coord_space).push();
         CGCanvas(canvas).transform_prepend_gobject(CGObject(child));
#else
         if (CObject(this).obj_class() == &class(CGMarker)) {
            CGCoordSpace(&canvas->coord_space).push();
            CGCanvas(canvas).transform_prepend_gobject(CGObject(child));
         }
         else {
            CGCanvas(canvas).transform_set_gobject(CGObject(child), FALSE);
         }
#endif
         CGObject(child).extent_set(canvas);
         if (CGObject(child)->extent.point[1].x >= CGObject(child)->extent.point[0].x &&
             CGObject(child)->extent.point[1].y >= CGObject(child)->extent.point[0].y) {
            CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 2, CGObject(child)->extent.point);
         }
         if (CObjClass_is_derived(&class(CGPrimitive), CObject(child).obj_class())) {
            if (CGPrimitive(child)->stroke_width > this->stroke_width_max) {
               this->stroke_width_max = CGPrimitive(child)->stroke_width;
            }
         }
#ifdef TEST
         CGCoordSpace(&canvas->coord_space).pop();
#else
         if (CObject(this).obj_class() == &class(CGMarker)) {
            CGCoordSpace(&canvas->coord_space).pop();
         }
#endif
      }
      child = CObject(this).child_next(child);
   }
}/*CGPrimContainer::extent_set*/

bool CGPrimContainer::transform_assimilate(TGTransform *transform) {
   bool result;

   CObjPersistent(this).attribute_update(ATTRIBUTE<CGObject,transform>);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGObject,transform>, FALSE);
   result = CGObject(this).transform_list_apply(&CGObject(this)->transform, transform);
   CObjPersistent(this).attribute_update_end();

   return result;
}/*CGPrimContainer::transform_assimilate*/

void CGGroup::CGGroup(void) {
}/*CGGroup::CGGroup*/

void CGGroup::delete(void) {
    class:base.delete();
}/*CGGroup::delete*/

void CGParameters::new(void) {
   new(&this->object_name).CString(NULL);
   class:base.new();
}/*CGParameters::new*/

void CGParameters::CGParameters(void) {
}/*CGParameters::CGParameters*/

void CGParameters::delete(void) {
   delete(&this->object_name);
   class:base.delete();
}/*CGParameters::delete*/

CObjClass *CGParameters::parameter_class(void) {
   return &class(CGParameter);
}/*CGParameters::parameter_class*/

void CGParameters::clear(void) {
   CObject *child, *next;

   child = CObject(this).child_first();
   while (child) {
      next = CObject(this).child_next(CObject(child));
      delete(child);
      child = next;
   }
}/*CGParameters::clear*/

void CGParameters::rebind(void) {
   CGParameter *parameter, *parameter_next;
   CGParameterExport *export = NULL;
   CObjPersistent *parent = CObjPersistent(CObject(this).parent()), *child, *next;
   byte *data_set;
   bool result;
   const TAttributeType *data_type;
   
   parameter = CGParameter(CObject(this).child_first());
   while (parameter) {
      parameter->state = EParameterState.present;
      parameter = CGParameter(CObject(this).child_next(CObject(parameter)));
   }

   data_set = CFramework(&framework).scratch_buffer_allocate();

   child = CObjPersistent(parent).child_tree_first();
   while (child) {
      if (CObjClass_is_derived(&class(CGParameterExport), CObject(child).obj_class()) &&
          CObject(child).parent() != CObject(parent) &&
          CGParameterExport(child)->attribute_name.attribute) {
         export = CGParameterExport(child);
         parameter = CGParameters(this).parameter_find(CString(&export->export_name).string());

         if (parameter) {
            if (parameter->state == EParameterState.present) {
               parameter->state = EParameterState.presentUsed;
            }
         }
         else {
            parameter = CGParameter(new.class(CGParameters(this).parameter_class()));
            parameter->state = EParameterState.created;
            CObjPersistent(parameter).attribute_set_string(ATTRIBUTE<CGParameter,name>, &export->export_name);
            CObject(this).child_add(CObject(parameter));
         }

         /*>>>ensure only one data type per parameter used */
         data_type = export->attribute_name.attribute->data_type;
         if (CObjClass_is_derived(&class(CGParameter), CObject(CObject(export).parent()).obj_class())) {
            if (CGParameter(CObject(export).parent())->value_initial_valid) {
               parameter->data_type = CGParameter(CObject(export).parent())->data_type;
            }
         }
         else {
            parameter->data_type = data_type;
         }
         result = (*parameter->data_type->convert)(CObjPersistent(this), parameter->data_type, NULL,
                                        -1, -1,
                                        data_set, NULL);
         result = (*parameter->data_type->convert)(CObjPersistent(CObject(export).parent()), parameter->data_type, data_type,
                                        -1, -1,
                                        data_set, (byte *)CObjPersistent(CObject(export).parent()) + export->attribute_name.attribute->offset);
         if (!parameter->value_initial_valid) {
            CGParameter(parameter).value_initial_assimilate(data_set);
         }
         if (CObjPersistent(parameter).attribute_default_get(ATTRIBUTE<CGParameter,value>)) {
            CObjPersistent(parameter).attribute_default_set(ATTRIBUTE<CGParameter,value>, TRUE);
         }
         if (parameter->state == EParameterState.presentUsed ||
             parameter->state == EParameterState.created)  {
            CGParameter(parameter).value_export_set(export);
         }
         if (!CObjPersistent(parameter).attribute_default_get(ATTRIBUTE<CGParameter,value>)) {
            CObjPersistent(CObject(export).parent()).attribute_default_set(export->attribute_name.attribute, FALSE);
         }

         result = (*parameter->data_type->convert)(CObjPersistent(this), NULL, parameter->data_type,
                                        -1, -1,
                                        NULL, data_set);
      }

      /* next object */
      if (CObjClass_is_derived(&class(CGParameters), CObject(child).obj_class())) {
         /* skip over unused parameters */
         if (child == CObjPersistent(this)) {
            /* this symbol's parameters, skip over to symbol content */
            child = CObjPersistent(CObject(child).parent()).child_next(child);
         }
         else {
            child = CObjPersistent(parent).child_tree_next(child);
         }
      }
      else {
         next = CObjPersistent(parent).child_tree_next(child);
         if (next && CObjClass_is_derived(&class(CGSymbol), CObject(next).obj_class())
                  && CObject(child).parent_find(&class(CGParameters))) {
            /* skip over symbol content */
            if (!CObjClass_is_derived(&class(CGParameters), CObject(child).obj_class())) {
               child = CObjPersistent(CObject(child).parent_find(&class(CGParameters)));
            }
            CGParameters(child).rebind();
            child = CObjPersistent(CObject(child).parent());
            child = CObjPersistent(CObject(child).parent()).child_next(child);
         }
         else {
            child = next;
         }
      }
   }

   CFramework(&framework).scratch_buffer_release(data_set);

   if (export) {
      /* remove unused parameters, >>>only perform if rebind found active parameters, kludge */
      parameter = CGParameter(CObject(this).child_first());
      while (parameter) {
         parameter->value_initial_valid = TRUE;
//         if (CObjPersistent(parameter).attribute_default_get(ATTRIBUTE<CGParameter,value>)) {
//            CObjPersistent(parameter).attribute_default_set(ATTRIBUTE<CGParameter,value>, TRUE);
//         }
         parameter_next = CGParameter(CObject(this).child_next(CObject(parameter)));
         if (parameter->state == EParameterState.present) {
            delete(parameter);
         }
         parameter = parameter_next;
      }
   }
   
   CGParameters(this).export_defaults();   
}/*CGParameters::rebind*/

CGParameter *CGParameters::parameter_find(const char *name) {
   CGParameter *parameter;
   const char *pname;

   if (name[0] == '*') {
      name++;
   }

   parameter = CGParameter(CObject(this).child_first());
   while (parameter) {
      pname = CString(&parameter->name).string();
      if (pname[0] == '*') {
         pname++;
      }
      if (strcmp(name, pname) == 0) {
         return parameter;
      }
      parameter = CGParameter(CObject(this).child_next(CObject(parameter)));
   }

   return NULL;
}/*CGParameters::parameter_find*/

void CGParameters::export_defaults(void) {
   CGParameter *parameter;
   CGParameterExport *parameter_export;
   TObjectPtr object_ptr;
   CObject *child;

   parameter = CGParameter(CObject(this).child_first());
   while (parameter) {
      while (CObject(parameter).child_count() != 0) {
         child = CObject(parameter).child_first();
         delete(child);
      }

      if (!CString(&this->object_name).empty()) {
         object_ptr.object = CObjPersistent(parameter);
         object_ptr.attr.attribute = ATTRIBUTE<CGParameter,value>;
         object_ptr.attr.element = -1;
         object_ptr.attr.index = -1;
         parameter_export = new.CGParameterExport(&object_ptr.attr);
         CString(&parameter_export->export_name).printf("%s#%s", CString(&this->object_name).string(), CString(&parameter->name).string());

         CObject(parameter).child_add(CObject(parameter_export));
      }

      parameter = CGParameter(CObject(this).child_next(CObject(parameter)));
   }
}/*CGParameters::export_defaults*/

void CGParameterExport::new(void) {
   new(&this->export_name).CString(NULL);
}/*CGParameterExport::new*/

void CGParameterExport::CGParameterExport(TAttributePtr *attributeName) {
   if (attributeName) {
      this->attribute_name = *attributeName;
   }
}/*CGParameterExport::CGParameterExport*/

void CGParameterExport::delete(void) {
   delete(&this->export_name);
}/*CGParameterExport::delete*/

void CGParameter::new(void) {
   new(&this->name).CString(NULL);
   new(&this->value).CString(NULL);
   new(&this->value_initial).CString(NULL);
   new(&this->export_name).CString(NULL);

   this->data_type = &ATTRIBUTE:type<CString>;

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGParameter,value>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGParameter,export_name>, TRUE);
}/*CGParameter::new*/

void CGParameter::CGParameter(const char *name) {
   CString(&this->name).set(name);
}/*CGParameter::CGParameter*/

void CGParameter::delete(void) {
   delete(&this->export_name);
   delete(&this->value_initial);
   delete(&this->value);
   delete(&this->name);
}/*CGParameter::delete*/

void CGParameter::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGUse *use = CGUse(CObject(this).parent_find(&class(CGUse)));

   if (!CObjPersistent(this).state_loading() && !changing && use && use->state == EGUseState.resolved) {
      CGParameters(use->active_parameters).rebind();
   }

   class:base.notify_attribute_update(attribute, changing);
}/*CGParameter::notify_attribute_update*/

void CGParameter::value_initial_assimilate(const void *data) {
   /* base implementation for strings */
   CString value;

   new(&value).CString(NULL);
   (*this->data_type->convert)(NULL, &ATTRIBUTE:type<CString>, this->data_type,
                               -1, -1,
                               (void *)&value, data);

   CString(&this->value_initial).set_string(CString(&value));

   delete(&value);
}/*CGParameter::value_initial_assimilate*/

void CGParameter::value_set(CString *data) {
   if (data) {
      CString(&this->value).set_string(data);
   }
   else {
      CString(&this->value).set_string(&this->value_initial);
   }
}/*CGParameter::value_set*/

void CGParameter::value_export_set(CGParameterExport *export) {
   CObjPersistent(CObject(export).parent()).attribute_set_string(export->attribute_name.attribute, &this->value);
}/*CGParameter::value_export_set*/

void CGUse::new(void) {
   class:base.new();

   this->active_parameters = &this->parameters;
   this->colour_background = GCOL_NONE;

   new(&this->link).CXLink(NULL, NULL);
   new(&this->symbolPath).CXPath(NULL, NULL);

   new(&this->parameters).CGParameters();
   CObject(&this->parameters).parent_set(CObject(this));
}/*CGUse::new*/

void CGUse::CGUse(void) {
}/*CGUse::CGUse*/

void CGUse::delete(void) {
   delete(&this->parameters);

   delete(&this->symbolPath);
   delete(&this->link);

   class:base.delete();
}/*CGUse::delete*/

void CGUse::realize(CObjPersistent *object) {
   CGSymbol *parent_symbol = CGSymbol(CObject(object).parent_find(&class(CGSymbol)));

   /* symbolPath */
   if (parent_symbol) {
      if (CObjClass_is_derived(&class(CGAnimation), CObject(object).obj_class())) {
         CXPath(&CGAnimation(object)->binding).root_set_default(object);
         CXPath(&CGAnimation(object)->binding).initial_set(&parent_symbol->initial_path.initial);
         CXPath(&CGAnimation(object)->binding).index_set(parent_symbol->initial_path.index);
         CXPath(&CGAnimation(object)->binding).resolve();
      }
      if (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
         CXPath(&CGXULElement(object)->binding).root_set_default(object);
         CXPath(&CGXULElement(object)->binding).initial_set(&parent_symbol->initial_path.initial);
         CXPath(&CGXULElement(object)->binding).index_set(parent_symbol->initial_path.index);
         CXPath(&CGXULElement(object)->binding).resolve();
      }
   }
}/*CGUse::realize*/

CObjPersistent *CGUse::symbol_resolve(void) {
   /*>>>copied symbol is never released, memory leak to fix...*/
   CObjPersistent *symbol = NULL, *child_copy;
   CObject *child;
   CString filename;
   CGLayoutSymbol *layout;
   CGLayout *layout_parent = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   bool filename_valid = FALSE;

   CGUse(this)->colour_background = GCOL_NONE;

   new(&filename).CString(NULL);

   if (CXLink(&this->link).link_get_file(&filename)) {
      filename_valid = TRUE;
//      if (layout_parent &&
//          CString(&layout_parent->filename).length() && CString(&filename).match(&layout_parent->filename)) {
//         filename_valid = FALSE;
//      }
   }

   if (filename_valid) {
      layout = new.CGLayoutSymbol(0, 0, NULL, NULL);
      CGLayout(layout)->load_locked = layout_parent ? layout_parent->load_locked : EFileLockMode.locked;

      if (CGLayout(layout).load_svg_file(CString(&filename).string(), NULL)) {
         CGUse(this)->colour_background = GCOL_CONVERT_RGB(CGCanvas(layout)->palette, CGCanvas(layout_parent)->paletteB, CGCanvas(layout)->colour_background);

         CXLink(&this->link).root_set(CObjPersistent(layout));
         this->link.object = NULL;
         symbol = CXLink(&this->link).object_resolve();

         if (!symbol) {
             /* attempt to take first symbol in loaded file */
             symbol = CObjPersistent(CObject(&CGLayout(layout)->defs).child_first());
         }
         if (symbol) {
            symbol = CObjPersistent(symbol).copy_fast(NULL, TRUE, TRUE);
         }

         if (!symbol) {
             /* use document contents as symbol */
            symbol = (CObjPersistent *)new.CGSymbol();
            child = CObject(layout).child_first();
            while (child) {
               child_copy = CObjPersistent(child).copy_fast(NULL, TRUE, TRUE);
               CObject(symbol).child_add(CObject(child_copy));
               child = CObject(layout).child_next(child);
            }
         }

         CObject(&this->link).child_add(CObject(symbol));
      }
      delete(layout);
   }
   else {
      symbol = CXLink(&this->link).object_resolve();
   }
   delete(&filename);

   return symbol;
}/*CGUse::symbol_resolve*/

void CGUse::resolve(void) {
   CObjPersistent *symbol;
   CGSymbol *parent_symbol = CGSymbol(CObject(this).parent_find(&class(CGSymbol)));
   CGLayout *layout_parent = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   CObjPersistent *object;
   CGDefs *defs = CGDefs(CObject(this).parent_find(&class(CGDefs)));
   CObject *child;

   if (defs || !layout_parent || CObject(this).parent_find(&class(CGLayoutSymbol))) {
      /* object outside document contents, or inside non rendered defs,
         don't attempt to resolve */
      return;
   }
   
   this->state = EGUseState.loading;

   object = CObjPersistent(CObject(this).child_first());
   if (object) {
      delete(object);
   }

   symbol = CGUse(this).symbol_resolve();
   if (symbol) {
      object = CObjPersistent(symbol).copy_fast(NULL, TRUE, TRUE);

      CObject(object).parent_set(CObject(this)); /*>>>hack!*/

      CXPath(&CGUse(this)->symbolPath).root_set_default(CObjPersistent(this));
      CXPath(&CGUse(this)->symbolPath).index_set(this->index);
      CXPath(&CGUse(this)->symbolPath).resolve();

      CXPath(&CGSymbol(object)->initial_path).root_set_default(CObjPersistent(this));
      if (parent_symbol) {
         CXPath(&CGSymbol(object)->initial_path).initial_set(&parent_symbol->initial_path.initial);
      }
      CXPath(&CGSymbol(object)->initial_path).index_set(this->index);
      CXPath(&CGSymbol(object)->initial_path).initial_set_path(CString(&CGUse(this)->symbolPath.path).string());
//      CXPath(&CGSymbol(object)->initial_path).initial_set(&CGUse(this)->symbolPath.object);

      CObject(object).parent_set(NULL); /*>>>hack!*/
      CObject(this).child_add(CObject(object));
#if 1
      /* Force reloading of images, shouldn't need this */
      child = CObject(this).child_tree_first();
      while (child) {
         if (CObjClass_is_derived(&class(CGImage), CObject(child).obj_class())) {
            CGImage(child).load_file(CGCanvas(layout_parent));
         }
         child = CObject(this).child_tree_next(child);
      }
#endif
      this->state = EGUseState.resolved;
   }
   else {
      this->state = EGUseState.notFound;
   }
  
   CGUse(this).resolve_children();
   CGParameters(CGUse(this)->active_parameters).rebind();

   CGObject(this).extent_set(CGCanvas(layout_parent));
   if (!CObjPersistent(this).state_loading()) {
      CGLayout(layout_parent).animation_buildmap(CObjPersistent(layout_parent));
   }
}/*CGUse::resolve*/

void CGUse::resolve_children(void) {
   CObjPersistent *child;
   CGSymbol *parent_symbol;
   
   child = CObjPersistent(this).child_tree_first();
   while (child) {
      CGUse(this).realize(child);
      if (CObjClass_is_derived(&class(CGUse), CObject(child).obj_class()) &&
          CGUse(child)->state == EGUseState.reloadNeeded) {
         if (CObjPersistent(child).attribute_default_get(ATTRIBUTE<CGUse,index>)) {
            parent_symbol = CGSymbol(CObject(child).parent_find(&class(CGSymbol)));
            CObjPersistent(child).attribute_set_int(ATTRIBUTE<CGUse,index>, 
                                                    parent_symbol->initial_path.index);
         }
         CGUse(child).resolve();
      }
      child = CObjPersistent(this).child_tree_next(child);
   }
}/*CGUse::resolve_children*/

void CGUse::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   class:base.notify_object_linkage(linkage, object);

   if (linkage == EObjectLinkage.ParentSet) {
      CGUse(this).notify_all_update(FALSE);
   }
}/*CGUse::notify_object_linkage*/

void CGUse::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
}/*CGUse::notify_attribute_update*/

void CGUse::notify_all_update(bool changing) {
   if (!changing && this->state == EGUseState.reloadNeeded) {
      CGUse(this).resolve();
   }
}/*CGUse::notify_all_update*/

void CGUse::extent_set(CGCanvas *canvas) {
   TPoint point[2];

   if (this->state == EGUseState.notFound) {
      point[0].x = 0;
      point[0].y = 0;
      point[1].x = 10;
      point[1].y = 10;

      CGCoordSpace(&canvas->coord_space).point_array_utov(2, point);
      CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);
      CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 2, point);
   }
   else {
      class:base.extent_set(canvas);
   }
}/*CGUse::extent_set*/

void CGUse::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGTransform transform;

   CLEAR(&transform);
   transform.type = EGTransform.translate;
   transform.t.translate.tx = (float)this->x;
   transform.t.translate.ty = (float)this->y;
   CGCoordSpace(&CGCanvas(canvas)->coord_space).transform_apply(&transform, FALSE);

   if (this->state == EGUseState.notFound) {
      CGCanvas(canvas).stroke_style_set(0, NULL, 0, 0, 0, 0);
      CGCanvas(canvas).draw_rectangle((GCOL_TYPE_NAMED | EGColourNamed.palevioletred), FALSE,
                                      0, 0, 10, 10);
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.palevioletred),
                                  0, 0, 10, 10);
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.palevioletred),
                                  10, 0, 0, 10);
   }
   else {
      class:base.draw(canvas, extent, mode);
   }
}/*CGUse::draw*/

void CGArray::new(void) {
   new(&this->selection).CXPath(NULL, NULL);
   class:base.new();
}/*CGArray::new*/

void CGArray::delete(void) {
   class:base.delete();
   delete(&this->selection);
}/*CGArray::delete*/

void CGArray::selection_update(void) {
   CObjPersistent *child;
   CSelection(&this->selection).clear();

   if (this->selection.object.object) {
      child = CObjPersistent(CObject(this->selection.object.object).child_first());
      while (child) {
         CSelection(&this->selection).add(child, NULL);
         child = CObjPersistent(CObject(this->selection.object.object).child_next(CObject(child)));
      }
      this->elements = CSelection(&this->selection).count();
   }
}/*CGArray::selection_update*/

void CGArray::resolve(void) {
   CObjPersistent *symbol;
   CGLayout *layout_parent = CGLayout(CObject(this).parent_find(&class(CGLayout)));   
   CGSymbol *parent_symbol = CGSymbol(CObject(this).parent_find(&class(CGSymbol)));
   CObjPersistent *object, *next_object;
   int i;
   TGTransform transform;
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   CGDefs *defs = CGDefs(CObject(this).parent_find(&class(CGDefs)));

   if (defs || !layout_parent || CObject(this).parent_find(&class(CGLayoutSymbol))) {
      /* object outside document contents, or inside non rendered defs,
         don't attempt to resolve */
      return;
   }

   CGUse(this)->state = EGUseState.loading;

   CGObject(this).show(FALSE);

   object = CObjPersistent(CObject(this).child_first());
   while (object) {
      next_object = CObjPersistent(CObject(this).child_next(CObject(object)));
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
         delete(object);
      }
      object = next_object;
   }

   CXPath(&this->selection).resolve();
   CGArray(this).selection_update();
   symbol = CGUse(this).symbol_resolve();

   if (symbol) {
      for (i = 0; i < this->elements; i++) {
         object = CObjPersistent(symbol).copy_fast(NULL, TRUE, TRUE);

         CLEAR(&transform);
         transform.type = EGTransform.translate;
         transform.t.translate.tx = (float)i * this->x_spacing;
         transform.t.translate.ty = (float)i * this->y_spacing;
         ARRAY(&CGObject(object)->transform).item_add(transform);

         CObject(object).parent_set(CObject(this)); /*>>>hack!*/
         CXPath(&CGUse(this)->symbolPath).root_set_default(CObjPersistent(this));
         CXPath(&CGUse(this)->symbolPath).index_set(CGUse(this)->index + i);
         CXPath(&CGUse(this)->symbolPath).resolve();
         CXPath(&CGSymbol(object)->initial_path).root_set_default(CObjPersistent(this));
         CXPath(&CGSymbol(object)->initial_path).index_set(CGUse(this)->index + i);
         if (parent_symbol) {
            CXPath(&CGSymbol(object)->initial_path).initial_set(&parent_symbol->initial_path.initial);
         }
         if (this->selection.object.object) {
            CXPath(&CGSymbol(object)->initial_path).initial_set(&ARRAY(&CSelection(&this->selection)->selection).data()[i]);
         }
         else {
            CXPath(&CGSymbol(object)->initial_path).initial_set_path(CString(&CGUse(this)->symbolPath.path).string());
//            CXPath(&CGSymbol(object)->initial_path).initial_set(&CGUse(this)->symbolPath.object);
         }

         CObject(object).parent_set(NULL); /*>>>hack!*/
         CObject(this).child_add(CObject(object));
      }
      CGUse(this)->state = EGUseState.resolved;      
   }
   else {
      CGUse(this)->state = EGUseState.notFound;   
   }
   
   CGUse(this).resolve_children();
   CGParameters(CGUse(this)->active_parameters).rebind();
   
   CGObject(this).extent_set(CGCanvas(layout_parent));
   if (!CObjPersistent(this).state_loading()) {
      CGLayout(layout_parent).animation_buildmap(CObjPersistent(layout_parent));
   }

   /* refresh layout, a little klunky.  Should at least only
      repaint affected area */
   CObjPersistent(layout).attribute_update(ATTRIBUTE<CGXULElement,width>);
   CObjPersistent(layout).attribute_update(ATTRIBUTE<CGXULElement,height>);
   CObjPersistent(layout).attribute_update_end();
//   CGCanvas(layout).queue_draw(NULL);

   CGObject(this).show(TRUE);
}/*CGArray::resolve*/

void CGArray::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   if (!changing) {
      CGUse(this)->state = EGUseState.reloadNeeded;
      CGUse(this).resolve();
   }
   class:base.notify_attribute_update(attribute, changing);
}/*CGArray::notify_attribute_update*/

void CGLink::new(void) {
   class:base.new();
   
//   new(&this->link).CXLink(NULL, NULL);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGUse,x>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGUse,y>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGUse,width>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGUse,height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLink,show>, TRUE);
}/*CGLink::new*/

void CGLink::CGLink(void) {
}/*CGLink::CGLink*/

void CGLink::delete(void) {
//   delete(&this->link);

//   _virtual_CGObject___destroy(CGObject(this));
   class:base.delete();
}/*CGLink::delete*/

void CGLink::resolve(void) {
}/*CGLink::resolve*/


/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
