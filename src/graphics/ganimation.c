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
#include "../objdataserver.c"
#include "gobject.c"
#include "gpalette.c"
#include "gcanvas.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

ENUM:typedef<EGAnimFill> {
   {freeze},
   {remove},
};

ENUM:typedef<EGAnimCalcMode> {
   {discrete},
   {linear},
   {paced},
   {spline}
};

ENUM:typedef<EGAnimRepeatCount> {
   {1},
   {indefinite},
};

ENUM:typedef<EGAnimState> {
   {Inactive},
   {Active},
   {Frozen},
   {Inhibited},
};

class CGAnimation : CObjPersistent {
 private:
   void new(void);
   void delete(void);

   bool reset_pending;
   bool time_based;
   double animation_value;
   CString value;

   EGAnimState state;

   static inline bool time_attr_update(void);
   static inline void binding_set(CXPath *binding);
   virtual void binding_default_set(void);

   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual bool transform_assimilate(TGTransform *transform);
 public:
   void CGAnimation(void);
   bool is_base_animation(void);

   ATTRIBUTE<CXPath binding> {
      CGAnimation(this).binding_set(data);
   };
   ATTRIBUTE<int begin> {
      if (data)
         this->begin = *data;

       CGAnimation(this).time_attr_update();
   };
   ATTRIBUTE<int dur> {
      if (data)
         this->dur = *data;

       CGAnimation(this).time_attr_update();
   };
   ATTRIBUTE<int end> {
      if (data)
         this->end = *data;

      CGAnimation(this).time_attr_update();
   };
   ATTRIBUTE<EGAnimRepeatCount repeatCount> {
      if (data)
         this->repeatCount = *data;
      else
         this->repeatCount = EGAnimRepeatCount.1;
   };
   ATTRIBUTE<EGAnimFill fill>;
   ATTRIBUTE<EGAnimCalcMode calcMode> {
      if (data)
         this->calcMode = *data;
      else
         this->calcMode = EGAnimCalcMode.linear;
   };

   void CGAnimation(void);

   virtual bool event(CEvent *event);
   virtual void animation_resolve_value(double relative_time);
   virtual void animation_resolve(void);
   virtual void reset(bool animated);
};

static inline bool CGAnimation::time_attr_update(void) {
   bool begin_default = CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGAnimation,begin>);
   bool dur_default   = CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGAnimation,dur>);
   bool end_default   = CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGAnimation,end>);

   if (!begin_default && !dur_default && !end_default) {
      return FALSE;
   }

   if (dur_default) {
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGAnimation,dur>);
      this->dur = this->end - this->begin;
      CObjPersistent(this).attribute_update_end();
   }
   if (end_default) {
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGAnimation,end>);
      this->end = this->begin + this->dur;
      CObjPersistent(this).attribute_update_end();
   }
   if (begin_default) {
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGAnimation,begin>);
      this->begin = this->end - this->dur;
      CObjPersistent(this).attribute_update_end();
   }

   return TRUE;
};

static inline void CGAnimation::binding_set(CXPath *data) {
   if (data) {
      ATTRIBUTE:convert<CXPath>(CObjPersistent(this), &ATTRIBUTE:type<CXPath>, &ATTRIBUTE:type<CXPath>,
                                -1, -1,
                                &this->binding, data);
   }
   else {
      CGAnimation(this).binding_default_set();
   }
}/*CGAnimation::binding_set*/

ARRAY:typedef<CGAnimation *>;

class CGAnimBind : CGAnimation {
 private:
   void new(void);
   void delete(void);

   int int_value;
   CString original_string_value;
 public:
   ALIAS<"svg:bind">;

   ATTRIBUTE<TAttributePtr attributeName>;

   void CGAnimBind(TAttributePtr *attributeName);

   virtual void animation_resolve(void);
   virtual void reset(bool animated);
};

class CGAnimAnimate : CGAnimation {
 private:
   void new(void);
   void delete(void);

   int int_value;
   CString original_string_value;
   virtual bool transform_assimilate(TGTransform *transform);
 public:
   ALIAS<"svg:animate">;

   ATTRIBUTE<int from>;
   ATTRIBUTE<int to>;

   ATTRIBUTE<TAttributePtr attributeName>;

   void CGAnimAnimate(TAttributePtr *attributeName);

   virtual void animation_resolve(void);
   virtual void reset(bool animated);
};

/*>>>consider using this as the base class for CGAnimAnimate*/
class CGAnimSet : CGAnimation {
 private:
   void new(void);
   void delete(void);

   int int_value;
   CString original_string_value;
 public:
   ALIAS<"svg:set">;

   ATTRIBUTE<TAttributePtr attributeName>;

   ATTRIBUTE<CString to> {
      if (data) {
         CString(&this->to).set_string(data);
      }
      else {
         CString(&this->to).set("1");      
      }
   };

   virtual void animation_resolve(void);
   virtual void reset(bool animated);
};

class CGAnimMotion : CGAnimation {
 public:
   ALIAS<"svg:animateMotion">;
};

class CGAnimColour : CGAnimation {
 private:
   void new(void);
   void delete(void);
   void attribute_target_set(void);
   void attribute_values_refresh(void);

   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);

   TGColour colour_value;   
   TGColour original_colour;
 public:
   ALIAS<"svg:animateColor">;

   void CGAnimColour(TAttributePtr *attributeName);

   ATTRIBUTE<ARRAY<TGColour> values> {
      if (attribute_element == -1 && !data) {
         CGAnimColour(this).attribute_target_set();
//>>>?       this->from = this->original_colour;

         ARRAY(&this->values).used_set(2);
         ARRAY(&this->values).data()[0] = this->from;
         ARRAY(&this->values).data()[1] = this->to;
      }
      if (attribute_element != -1) {
         ARRAY(&this->values).data()[attribute_element] = *data;
      }
   };
   ATTRIBUTE<TGColour from> {
      if (data) {
         this->from = *data;
      }
      CGAnimColour(this).attribute_values_refresh();
   };
   ATTRIBUTE<TGColour to> {
      if (data) {
         this->to = *data;
      }
      CGAnimColour(this).attribute_values_refresh();
   };
   ATTRIBUTE<TAttributePtr attributeName> {
      if (data)
         this->attributeName = *data;

      CGAnimColour(this).attribute_target_set();
   };

   virtual void animation_resolve(void);
   virtual void reset(bool animated);
};

class CGAnimTransform : CGAnimation {
 private:
   void new(void);
   void delete(void);   
   TGTransform *animate_transform;

   int int_value;
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual bool transform_assimilate(TGTransform *transform);
 public:
   ALIAS<"svg:animateTransform">;

   ATTRIBUTE<TAttributePtr attributeName>;
   ATTRIBUTE<EGTransform type> {
      if (data)
         this->type = *data;
      this->from.type = this->type;
      this->to.type   = this->type;
   };
   ATTRIBUTE<TGTransformData from>;
   ATTRIBUTE<TGTransformData to>;

   virtual void animation_resolve(void);
   virtual void reset(bool animated);
};

/*=========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CGAnimation::new(void) {
   class:base.new();

   new(&this->value).CString(NULL);
   CString(&this->value).encoding_set(EStringEncoding.UTF16);
   new(&this->binding).CXPath(NULL, NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimation,binding>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimation,begin>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimation,dur>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimation,end>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimation,calcMode>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimation,repeatCount>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimation,fill>, TRUE);
}/*CGAnimation::new*/

void CGAnimation::CGAnimation(void) {
}/*CGAnimation::CGAnimation*/

void CGAnimation::delete(void) {
   delete(&this->binding);
   delete(&this->value);

   class:base.delete();
}/*CGAnimation::delete*/

bool CGAnimation::transform_assimilate(TGTransform *transform) {
   return FALSE;
}

void CGAnimation::binding_default_set(void) {}

bool CGAnimation::event(CEvent *event) {
   return FALSE;
}/*CGAnimation::event*/

void CGAnimation::animation_resolve_value(double relative_time) {
//   this->time_based = this->binding.object.object == NULL;
   this->time_based = !CString(&this->binding.path).length();
   /*>>>animation repeat, remove/fill etc*/

   if (CObject(this).obj_class() == &class(CGAnimBind)) {
      this->state = EGAnimState.Active;
      return;
   }

   if (this->time_based) {
      if (relative_time < this->begin) {
         this->state = EGAnimState.Inactive;
      }
      else if (this->repeatCount == EGAnimRepeatCount.indefinite) {
         while (relative_time > this->begin + this->dur) {
            relative_time -= this->dur;
         }
         this->state = EGAnimState.Active;
      }
      else {
         this->state = EGAnimState.Active;
         if (relative_time > this->begin + this->dur) {
            relative_time = (double)this->begin + this->dur;
            this->state = EGAnimState.Frozen;
         }
      }
      this->animation_value = relative_time;
   }
   else {
      this->animation_value = CXPath(&CGAnimation(this)->binding).get_int();
      this->state = (this->animation_value < this->begin || this->animation_value > this->end)
         ? EGAnimState.Inactive : EGAnimState.Active;
   }
}/*CGAnimation::animation_resolve_value*/

void CGAnimation::animation_resolve(void) {
}/*CGAnimation::animation_resolve*/

void CGAnimation::reset(bool animated) {
//   this->time_based = this->binding.object.object == NULL;
   this->time_based = !CString(&this->binding.path).length();   
   this->state = EGAnimState.Inactive;

   if (animated) {
      CGAnimation(this)->reset_pending = TRUE;
   }
}/*CGAnimation::reset*/

void CGAnimBind::new(void) {
   class:base.new();

   new(&this->original_string_value).CString(NULL);
   CString(&this->original_string_value).encoding_set(EStringEncoding.UTF16);

   CGAnimation(this)->reset_pending = TRUE;
}/*CGAnimBind::new*/

void CGAnimBind::CGAnimBind(TAttributePtr *attributeName) {
   if (attributeName) {
      this->attributeName = *attributeName;
   }
}/*CGAnimBind::CGAnimBind*/

void CGAnimBind::delete(void) {
   delete(&this->original_string_value);

   class:base.delete();
}/*CGAnimBind::delete*/

void CGAnimBind::animation_resolve(void) {
   CObjPersistent *object = CObjPersistent(CObject(this).parent());
   CString value;
   int child_count;
   int int_value;

   if (!this->attributeName.attribute)
      return;

   if (((this->attributeName.attribute->type != PT_AttributeArray &&
        (this->attributeName.attribute->type == PT_AttributeEnum ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<int> ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<bool>)) ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<TGTransform> ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<TUNIXTime>)) {
      int_value = CXPath(&CGAnimation(this)->binding).get_int();

      if (this->int_value != int_value || CGAnimation(this)->reset_pending) {
         CObjPersistent(object).attribute_update(this->attributeName.attribute);
         CObjPersistent(object).attribute_set(this->attributeName.attribute, this->attributeName.element,
                                              this->attributeName.index, &ATTRIBUTE:type<int>, &int_value);
         CObjPersistent(object).attribute_update_end();
         this->int_value = int_value;
         CGAnimation(this)->reset_pending = FALSE;
      }
   }
   else {
      new(&value).CString(NULL);
      CString(&value).encoding_set(EStringEncoding.UTF16);
      if (CGAnimation(this)->binding.operation == EPathOperation.count) {
         child_count = CObject(CGAnimation(this)->binding.object.object).child_count();
         CString(&value).printf("%d", child_count);
      }

//      CObjPersistent(CGAnimation(this)->binding.object.object).attribute_get_string_element_index(CGAnimation(this)->binding.object.attribute,
//                                                                                                    CGAnimation(this)->binding.object.attribute_element,
//                                                                                                    CGAnimation(this)->binding.object.attribute_index,
//                                                                                                    &value);
      CXPath(&CGAnimation(this)->binding).get_string(&value);
      CString(&value).encoding_set(EStringEncoding.UTF16);

      if (!CString(&CGAnimation(this)->value).match(CString(&value))) {
         CString(&CGAnimation(this)->value).set_string(&value);
         CObjPersistent(object).attribute_update(this->attributeName.attribute);
         CObjPersistent(object).attribute_set_string_element_index(this->attributeName.attribute,
                                                                   this->attributeName.element, this->attributeName.index,
                                                                   &value);
         CObjPersistent(object).attribute_update_end();
      }
      delete(&value);
   }
}/*CGAnimBind::animation_resolve*/

void CGAnimBind::reset(bool animated) {
   CObjPersistent *object = CObjPersistent(CObject(this).parent());

   class:base.reset(animated);

   if (animated) {
      CObjPersistent(object).attribute_get(this->attributeName.attribute, this->attributeName.element,
                                           this->attributeName.index, &ATTRIBUTE:type<CString>,
                                           &this->original_string_value);
      CGAnimation(this)->reset_pending = TRUE;
      CString(&CGAnimation(this)->value).clear();
   }
   else {
      CObjPersistent(object).attribute_set(this->attributeName.attribute, this->attributeName.element,
                                           this->attributeName.index, &ATTRIBUTE:type<CString>,
                                           &this->original_string_value);
   }
}/*CGAnimBind::reset*/

void CGAnimAnimate::new(void) {
   class:base.new();

   new(&this->original_string_value).CString(NULL);

   CGAnimation(this)->reset_pending = TRUE;
}/*CGAnimAnimate::new*/

void CGAnimAnimate::CGAnimAnimate(TAttributePtr *attributeName) {
   if (attributeName) {
      this->attributeName = *attributeName;
   }
}/*CGAnimAnimate::CGAnimAnimate*/

void CGAnimAnimate::delete(void) {
   delete(&this->original_string_value);

   class:base.delete();
}/*CGAnimAnimate::delete*/

bool CGAnimAnimate::transform_assimilate(TGTransform *transform) {
   switch (transform->type) {
   case EGTransform.scale:
      if (!this->attributeName.attribute) {
         return FALSE;
      }
      if (strcmp(this->attributeName.attribute->name, "width") == 0) {
         this->from = (int)(this->from * transform->t.scale.sx);
         this->to = (int)(this->to * transform->t.scale.sx);
         return TRUE;
      }
      else if (strcmp(this->attributeName.attribute->name, "height") == 0) {
         this->from = (int)(this->from * transform->t.scale.sy);
         this->to = (int)(this->to * transform->t.scale.sy);
         return TRUE;
      }
   default:
      break;
   }

   return FALSE;
}/*CGAnimAnimate::transform_assimilate*/

void CGAnimAnimate::animation_resolve(void) {
   CGObject *object = CGObject(CObject(this).parent());
   CString value;
   int child_count, int_value;
   double float_value;

   if (!this->attributeName.attribute)
      return;

   if (this->attributeName.attribute->type != PT_AttributeArray &&
       (this->attributeName.attribute->type == PT_AttributeEnum ||
        this->attributeName.attribute->data_type == &ATTRIBUTE:type<int> ||
        this->attributeName.attribute->data_type == &ATTRIBUTE:type<bool> ||
        this->attributeName.attribute->data_type == &ATTRIBUTE:type<TCoord> ||
        this->attributeName.attribute->data_type == &ATTRIBUTE:type<TGTransform>/*>>>hack!*/)) {
      float_value = CGAnimation(this)->animation_value;

//      if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGAnimation,begin>)) {
         if (CGAnimation(this)->dur) {
            if (float_value < CGAnimation(this)->begin) {
               float_value = CGAnimation(this)->begin;
            }
            if (float_value > CGAnimation(this)->end) {
               float_value = CGAnimation(this)->end;
            }

            float_value -= CGAnimation(this)->begin;
            /* Do scaling */
            /*>>>hack!*/
            float_value = (this->to - this->from) * float_value / (CGAnimation(this)->dur) + this->from;
         }
         else {
            /* special rule for 0 duration */
            float_value = float_value == CGAnimation(this)->begin ? this->to : this->from;
         }
//      }

      int_value = (int)(float_value + 0.5);
      if (this->int_value != int_value || CGAnimation(this)->reset_pending) {
         CObjPersistent(object).attribute_update(this->attributeName.attribute);
         CObjPersistent(object).attribute_set(this->attributeName.attribute, this->attributeName.element,
                                              this->attributeName.index, &ATTRIBUTE:type<int>, &int_value);
         CObjPersistent(object).attribute_update_end();
         this->int_value = int_value;
         CGAnimation(this)->reset_pending = FALSE;
      }
   }
   else {
      new(&value).CString(NULL);
      if (CGAnimation(this)->binding.operation == EPathOperation.count) {
         child_count = CObject(CGAnimation(this)->binding.object.object).child_count();
         CString(&value).printf("%d", child_count);
      }

//      CObjPersistent(CGAnimation(this)->binding.object.object).attribute_get_string_element_index(CGAnimation(this)->binding.object.attribute,
//                                                                                                    CGAnimation(this)->binding.object.attribute_element,
//                                                                                                    CGAnimation(this)->binding.object.attribute_index,
//                                                                                                    &value);
      CXPath(&CGAnimation(this)->binding).get_string(&value);

      if (CString(&CGAnimation(this)->value).strcmp(CString(&value).string()) != 0) {
         CString(&CGAnimation(this)->value).set(CString(&value).string());
         CObjPersistent(object).attribute_update(this->attributeName.attribute);
         CObjPersistent(object).attribute_set_string_element_index(this->attributeName.attribute,
                                                                   this->attributeName.element, this->attributeName.index,
                                                                   &value);
         CObjPersistent(object).attribute_update_end();
      }
      delete(&value);
   }

//>>>probably don't need this at all
//      if (CGAnimation(this)->binding.attribute && CGAnimation(this)->binding.attribute->method) {
//          (*dest->attribute->method)(dest->object, FALSE);
//      }
//   }
}/*CGAnimAnimate::animation_resolve*/

void CGAnimAnimate::reset(bool animated) {
   CGObject *object = CGObject(CObject(this).parent());

   class:base.reset(animated);

   if (!this->attributeName.attribute)
      return;

   if (animated) {
      CObjPersistent(object).attribute_get(this->attributeName.attribute, this->attributeName.element,
                                           this->attributeName.index, &ATTRIBUTE:type<CString>,
                                           &this->original_string_value);
      CGAnimation(this)->reset_pending  = TRUE;
   }
   else {
      CObjPersistent(object).attribute_set(this->attributeName.attribute, this->attributeName.element,
                                           this->attributeName.index, &ATTRIBUTE:type<CString>,
                                           &this->original_string_value);
   }
}/*CGAnimAnimate::reset*/

void CGAnimSet::new(void) {
   class:base.new();

   new(&this->original_string_value).CString(NULL);
   new(&this->to).CString(NULL);
}/*CGAnimSet::new*/

void CGAnimSet::delete(void) {
   delete(&this->to);
   delete(&this->original_string_value);

   class:base.delete();
}/*CGAnimSet::delete*/

void CGAnimSet::animation_resolve(void) {
   CGObject *object = CGObject(CObject(this).parent());
   int int_value;

   if (!this->attributeName.attribute)
      return;

   if (this->attributeName.attribute->type == PT_AttributeEnum ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<TCoord> ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<int> ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<bool>) {

//>>>      int_value = CXPath(&CGAnimation(this)->binding).get_int();
      if (CGAnimation(this)->time_based) {
         int_value = (int)CGAnimation(this)->animation_value;
      }
      else {
         int_value = CXPath(&CGAnimation(this)->binding).get_int();
      }

#if 1
      int_value = (int_value >= CGAnimation(this)->begin &&
                   int_value <= CGAnimation(this)->begin + CGAnimation(this)->dur) ? 1 : 0;
#else
      /*>>>kludgy, should account for 'fill' animation attribute */
      if (CGAnimation(this)->dur) {
         int_value = (int_value >= CGAnimation(this)->begin) ? 1 : 0;
      }
#endif

      if (this->int_value != int_value || CGAnimation(this)->reset_pending) {
         CObjPersistent(object).attribute_update(this->attributeName.attribute);
#if 1
         CObjPersistent(object).attribute_set(this->attributeName.attribute, this->attributeName.element,
                                              this->attributeName.index, &ATTRIBUTE:type<CString>,
                                              int_value ? &this->to : &this->original_string_value);
#else         
         CObjPersistent(object).attribute_set(this->attributeName.attribute, this->attributeName.element,
                                              this->attributeName.index, &ATTRIBUTE:type<int>, &int_value);
#endif                                              
         CObjPersistent(object).attribute_update_end();
         this->int_value = int_value;
         CGAnimation(this)->reset_pending = FALSE;
      }
   }
}/*CGAnimSet::animation_resolve*/

void CGAnimSet::reset(bool animated) {
   CGObject *object = CGObject(CObject(this).parent());

   class:base.reset(animated);

   if (animated) {
      CGAnimation(this)->reset_pending = TRUE;
      CObjPersistent(object).attribute_get(this->attributeName.attribute, this->attributeName.element,
                                           this->attributeName.index, &ATTRIBUTE:type<CString>,
                                           &this->original_string_value);

   }
   else {
      CObjPersistent(object).attribute_set(this->attributeName.attribute, this->attributeName.element,
                                           this->attributeName.index, &ATTRIBUTE:type<CString>,
                                           &this->original_string_value);
   }
}/*CGAnimSet::reset*/

extern const TAttribute __parameter_CGPrimitive_fill;

void CGAnimColour::new(void) {
   /*>>>quick hack*/
   TAttribute *attr = ATTRIBUTE<CGAnimColour,values>;
   attr->delim    = ";";

   ARRAY(&this->values).new();

   class:base.new();

   this->attributeName.attribute = ATTRIBUTE<CGPrimitive,fill>;
   this->attributeName.element   = -1;
   this->attributeName.index     = -1;
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimColour,from>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimColour,to>,   TRUE);
}/*CGAnimColour::new*/

void CGAnimColour::CGAnimColour(TAttributePtr *attributeName) {
   if (attributeName) {
      this->attributeName = *attributeName;
   }
}/*CGAnimColour::CGAnimColour*/

void CGAnimColour::delete(void) {
   ARRAY(&this->values).delete();

   class:base.delete();
}/*CGAnimColour::delete*/

void CGAnimColour::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   switch (linkage) {
   case EObjectLinkage.ParentSet:
      CObjPersistent(this).attribute_refresh(FALSE);
//      CGAnimColour(this).attribute_target_set();
//      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimColour,from>, TRUE);
      break;
   default:
      break;
   }

   class:base.notify_object_linkage(linkage, object);
}/*CGAnimColour::notify_object_linkage*/

void CGAnimColour::attribute_target_set(void) {
   CGObject *gobject = CGObject(CObject(this).parent());

   if (gobject) {
      CObjPersistent(gobject).attribute_get(this->attributeName.attribute, this->attributeName.element,
                                            this->attributeName.index, &ATTRIBUTE:type<TGColour>,
                                            &this->original_colour);
   }
}/*CGAnimColour::attribute_target_set*/

void CGAnimColour::attribute_values_refresh(void) {
   if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGAnimColour,from>)) {
      this->from = this->to;
   }
   if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGAnimColour,to>)) {
      this->to = this->from;
   }
   if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGAnimColour,values>)) {
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimColour,values>, TRUE);
   }
}/*CGAnimColour::attribute_values_refresh*/

void CGAnimColour::reset(bool animated) {
   CGObject *gobject = CGObject(CObject(this).parent());

   class:base.reset(animated);

   CGAnimation(this)->reset_pending = TRUE;
   this->colour_value = GCOL_UNDEFINED;

   if (animated) {
      CGAnimColour(this).attribute_target_set();
   }
   else {
      CObjPersistent(gobject).attribute_set(this->attributeName.attribute, this->attributeName.element,
                                            this->attributeName.index, &ATTRIBUTE:type<TGColour>,
                                            &this->original_colour);
   }
}/*CGAnimColour::reset*/

void CGAnimColour::animation_resolve(void) {
   CGCanvas *canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
   CGObject *object = CGObject(CObject(this).parent());
   int int_value, begin, dur, index, offset;
   TGColour colour_value, colour_from, colour_to;

   if (object->visibility != EGVisibility.visible || !this->attributeName.attribute)
      return;

   begin = CGAnimation(this)->begin * 256;
   dur   = CGAnimation(this)->dur   * 256;

   int_value = (int) (CGAnimation(this)->animation_value * 256);
   if (int_value < begin) {
      int_value = -1;
   }
   else if (int_value > begin + dur) {
      int_value = CGAnimation(this)->fill == EGAnimFill.remove ? -1 : (ARRAY(&this->values).count() - 1) * 256;
   }
   else {
      if (dur == 0) {
         int_value = 0;
      }
      else {
         int_value = (int_value - begin) * ((ARRAY(&this->values).count() - 1) * 256) / dur;
      }
      if (int_value < 0) {
         int_value = 0;
      }
      if (int_value > (ARRAY(&this->values).count() - 1) * 256) {
         int_value = (ARRAY(&this->values).count() - 1) * 256;
      }
      if (CGAnimation(this)->calcMode == EGAnimCalcMode.discrete) {
         /*>>>not the same as Adobes implementation, close enough */
         int_value += 128;
         int_value -= int_value % 256;
      }
   }

   if (int_value == -1) {
      colour_value = this->original_colour;
   }
   else {
      index  = int_value / 256;
      offset = int_value % 256;
      colour_from = GCOL_CONVERT_RGB(canvas->palette, canvas->paletteB, ARRAY(&this->values).data()[index]);
      if (index >= ARRAY(&this->values).count() - 1) {
         colour_to = GCOL_NONE;
      }
      else {
         colour_to   = GCOL_CONVERT_RGB(canvas->palette, canvas->paletteB, ARRAY(&this->values).data()[index + 1]);
      }

      colour_value = GCOL_RGB(
         (GCOL_RGB_RED(colour_to)   * offset / 256) + (GCOL_RGB_RED(colour_from)   * (256 - offset) / 256),
         (GCOL_RGB_GREEN(colour_to) * offset / 256) + (GCOL_RGB_GREEN(colour_from) * (256 - offset) / 256),
         (GCOL_RGB_BLUE(colour_to)  * offset / 256) + (GCOL_RGB_BLUE(colour_from)  * (256 - offset) / 256)
      );
   }
   
   if (colour_value != this->colour_value || CGAnimation(this)->reset_pending) {
      CObjPersistent(object).attribute_update(this->attributeName.attribute);
      CObjPersistent(object).attribute_set(this->attributeName.attribute, this->attributeName.element,
                                           this->attributeName.index, &ATTRIBUTE:type<TGColour>,
                                           &colour_value);
      CObjPersistent(object).attribute_update_end();
      this->colour_value = colour_value;
      CGAnimation(this)->reset_pending = FALSE;
   }
}/*CGAnimColour::animation_resolve*/

void CGAnimTransform::new(void) {
   class:base.new();

   CGAnimation(this)->reset_pending = TRUE;
}/*CGAnimTransform::new*/

void CGAnimTransform::delete(void) {
   class:base.delete();
}/*CGAnimTransform::delete*/

void CGAnimTransform::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   int count;
   switch (linkage) {
   case EObjectLinkage.ParentSet:
      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGAnimTransform,type>)) {
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGAnimTransform,attributeName>, "transform");
      }
      /* Allocate extra object transform entries now */
      count = ARRAY(&CGObject(object)->transform).count();
      ARRAY(&CGObject(object)->transform).used_set(count + CObject(object).child_count());
      ARRAY(&CGObject(object)->transform).used_set(count);
      break;
   default:
      break;
   }
   class:base.notify_object_linkage(linkage, object);
}/*CGAnimTransform::notify_object_linkage*/

bool CGAnimTransform::transform_assimilate(TGTransform *transform) {
   switch (transform->type) {
   case EGTransform.scale:
      if (this->from.type == EGTransform.translate) {
         this->from.t.translate.tx *= transform->t.scale.sx;
         this->from.t.translate.ty *= transform->t.scale.sy;
      }
      if (this->to.type == EGTransform.translate) {
         this->to.t.translate.tx *= transform->t.scale.sx;
         this->to.t.translate.ty *= transform->t.scale.sy;
      }
      return TRUE;
   default:
      break;
   }

   return FALSE;
}/*CGAnimTransform::transform_assimilate*/

void CGAnimTransform::animation_resolve(void) {
   int int_value, begin, dur;
   CGObject *object = CGObject(CObject(this).parent());

//   /*>>>hack*/
//   if ((int)relative_time < CGAnimation(this)->begin)
//   return;

   begin = CGAnimation(this)->begin * 100;
   dur   = CGAnimation(this)->dur   * 100;

   if (CGAnimation(this)->time_based) {
      int_value = (int) (CGAnimation(this)->animation_value * 100);
   }
   else {
      int_value = CXPath(&CGAnimation(this)->binding).get_int() * 100;
   }

   if (dur) {
      if (int_value < begin) {
         int_value = begin;
      }
      if (int_value > begin + dur) {
         int_value = begin + dur;
      }
      int_value = (int_value - begin) * 100 / dur;
   }
   else {
      /* handling for 0 length duration*/
      int_value = (int_value == begin ? 100 : 0);
   }

   if (this->int_value != int_value || CGAnimation(this)->reset_pending) {
      CObjPersistent(object).attribute_update(ATTRIBUTE<CGObject,transform>);
      switch (this->type) {
      case EGTransform.translate:
         this->animate_transform->t.translate.tx =
            (this->to.t.translate.tx * int_value / 100) + (this->from.t.translate.tx * (100 - int_value) / 100);
         this->animate_transform->t.translate.ty =
            (this->to.t.translate.ty * int_value / 100) + (this->from.t.translate.ty * (100 - int_value) / 100);
         break;
      case EGTransform.scale:
         this->animate_transform->t.scale.sx =
            (this->to.t.scale.sx * int_value / 100) + (this->from.t.scale.sx * (100 - int_value) / 100);
         this->animate_transform->t.translate.ty =
            (this->to.t.scale.sy * int_value / 100) + (this->from.t.scale.sy * (100 - int_value) / 100);
         break;
      case EGTransform.rotate:
         this->animate_transform->t.rotate.rotate_angle =
            (this->to.t.rotate.rotate_angle * int_value / 100) + (this->from.t.rotate.rotate_angle * (100 - int_value) / 100);
         this->animate_transform->t.rotate.cx =
            (this->to.t.rotate.cx * int_value / 100) + (this->from.t.rotate.cx * (100 - int_value) / 100);
         this->animate_transform->t.rotate.cy =
            (this->to.t.rotate.cy * int_value / 100) + (this->from.t.rotate.cy * (100 - int_value) / 100);
         break;
      default:
         break;
      }
      CObjPersistent(object).attribute_update_end();
      this->int_value = int_value;
      CGAnimation(this)->reset_pending = FALSE;
   }
}/*CGAnimTransform::animation_resolve*/

void CGAnimTransform::reset(bool animated) {
   CGObject *object = CGObject(CObject(this).parent());
   
   class:base.reset(animated);

   if (animated) {
      CGAnimation(this)->reset_pending = TRUE;

      /*>>>should add transform to start, not end of list */
      ARRAY(&object->transform).item_add_empty();
      this->animate_transform = ARRAY(&object->transform).item_last();

      CLEAR(this->animate_transform);
      this->animate_transform->type = this->type;
      switch (this->type) {
      case EGTransform.scale:
         this->animate_transform->t.scale.sx = 1;
         this->animate_transform->t.scale.sy = 1;
         break;
      case EGTransform.rotate:
         this->animate_transform->t.rotate.cxy_used = TRUE;
         break;
      default:
         break;
      }
   }
   else {
      ARRAY(&object->transform).used_set(ARRAY(&object->transform).count() - 1);
      this->animate_transform = NULL;
   }
}/*CGAnimTransform::reset*/

bool CGAnimation::is_base_animation(void) {
   const CObjClass *obj_class = CObject(this).obj_class();

   return (
      obj_class == &class(CGAnimation)    ||
//      obj_class == &class(CGAnimBind)     ||
      obj_class == &class(CGAnimAnimate)  ||
      obj_class == &class(CGAnimSet)      ||
      obj_class == &class(CGAnimMotion)   ||
      obj_class == &class(CGAnimColour)   ||
      obj_class == &class(CGAnimTransform)
      );
}/*CGAnimation::is_base_animation*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/