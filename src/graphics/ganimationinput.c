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
#include "ganimation.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGAnimateInput : CGAnimation {
 private:
   void new(void);     
   void delete(void);      
 public:
   void CGAnimateInput(void);
};

class CGAnimateInputEvent : CGAnimateInput {
 public:
   ALIAS<"svg:animateInputEvent">;
   ATTRIBUTE<EEventGObject event_type, "eventType">;

   void CGAnimateInputEvent(void);

   virtual void event(CEvent *event);
};

ENUM:typedef<EGSliderOrientType> {
   {horizontal},
   {vertical},
};

class CGAnimateInputSlider : CGAnimateInput {
 private:
   bool active;
 public:
   ALIAS<"svg:animateInputSlider">;
   ATTRIBUTE<EGSliderOrientType orient> {
       if (data) {
           this->orient = *data;
       }
       else {
           this->orient = EGSliderOrientType.vertical;
       }
   };    
   ATTRIBUTE<int min>;
   ATTRIBUTE<int max>;

   void CGAnimateInputSlider(void);

   virtual void event(CEvent *event);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include "glayout.c"

void CGAnimateInput::new(void) {
   class:base.new();
}/*CGAnimateInput::new*/

void CGAnimateInput::delete(void) {
   class:base.delete();    
}/*CGAnimateInput::delete*/

void CGAnimateInputEvent::CGAnimateInputEvent(void) {
}/*CGAnimateInputEvent::CGAnimateInputEvent*/

bool CGAnimateInputEvent::event(CEvent *event) {
   CEventPointer *pointer;
   CGCanvas *canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
   CString set_value;

   if (CObject(event).obj_class() == &class(CEventKey) && CEventKey(event)->type == EEventKeyType.down) {
      if (CEventKey(event)->key == EEventKey.Ascii && CEventKey(event)->value == ' ') {
         new(&set_value).CString(NULL);
         CXPath(&CGAnimation(this)->binding).assignment_get_string(&set_value);
         if (CString(&set_value).empty()) {
             CString(&set_value).set("TRUE");
         }
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_set_string_element_index(CGAnimation(this)->binding.object.attr.attribute,
                                                                                                   CGAnimation(this)->binding.object.attr.element,
                                                                                                   CGAnimation(this)->binding.object.attr.index,
                                                                                                   &set_value);
         delete(&set_value);                                                                                                   
         /*>>>cheating, out of order */
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update(CGAnimation(this)->binding.object.attr.attribute);
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update_end();
      }
   }

   if (CObject(event).obj_class() == &class(CEventPointer)) {
      /* only support click event type for now */
      if (CGAnimation(this)->binding.object.attr.attribute && 
          (this->event_type == EEventGObject.click || this->event_type == EEventGObject.clickRight)) {
         CGCanvas(canvas).pointer_cursor_set(EGPointerCursor.pointer);
      }
      else {
         return FALSE;
      }

      pointer = CEventPointer(event);
      switch (pointer->type) {
      case EEventPointer.Move:
         return TRUE;
      case EEventPointer.LeftDown:
      case EEventPointer.LeftDClick:
         if (this->event_type == EEventGObject.click)
            goto set;
         return FALSE;
      case EEventPointer.RightDown:
      case EEventPointer.RightDClick:
         if (this->event_type == EEventGObject.clickRight)
            goto set;
         return FALSE;
      set:
         new(&set_value).CString(NULL);
         CXPath(&CGAnimation(this)->binding).assignment_get_string(&set_value);
         if (CString(&set_value).empty()) {
             CString(&set_value).set("TRUE");
         }
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_set_string_element_index(CGAnimation(this)->binding.object.attr.attribute,
                                                                                             CGAnimation(this)->binding.object.attr.element,
                                                                                             CGAnimation(this)->binding.object.attr.index,
                                                                                             &set_value);
         delete(&set_value);                                                                                                                                                                                                
         /*>>>cheating, out of order */
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update(CGAnimation(this)->binding.object.attr.attribute);
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update_end();
         return TRUE;
      default:
         break;      
      }
      return FALSE;
   }
   return FALSE;
}/*CGAnimateInputEvent::event*/

/*>>>very crude test implementation*/
bool CGAnimateInputSlider::event(CEvent *event) {
   TCoord position;
   CEventPointer *pointer;
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   int set_value;
   TCoord value;
   TRect *extent;

   if (CObject(event).obj_class() == &class(CEventPointer)) {
      /* only support click event type for now */
      if (CGAnimation(this)->binding.object.attr.attribute) {
         CGCanvas(layout).pointer_cursor_set(EGPointerCursor.pointer);
      }
      else {
         return FALSE;
      }

      pointer = CEventPointer(event);
      switch (pointer->type) {
      case EEventPointer.LeftUp:
         CGCanvas(layout).pointer_capture(FALSE);
         CGLayout(layout).selected_held_set(NULL);
         this->active = FALSE;
         break;
      case EEventPointer.LeftDown:
         CGLayout(layout).selected_held_set(CObjPersistent(this));
         CGCanvas(layout).pointer_capture(TRUE);
         this->active = TRUE;
         /*fallthrough*/
      case EEventPointer.Move:
         if (this->active) {
            extent = &CGObject(CObject(this).parent())->extent;
            switch (this->orient) {
            case EGSliderOrientType.vertical:
               position = pointer->position.y;
               if (position < extent->point[0].y) {
                  position = extent->point[0].y;
               }
               if (position > extent->point[1].y) {
                  position = extent->point[1].y;
               }
               value = 1-((float)(position - extent->point[0].y) / (float)(extent->point[1].y - extent->point[0].y));
               break;
            case EGSliderOrientType.horizontal:
               position = pointer->position.x;
               if (position < extent->point[0].x) {
                  position = extent->point[0].x;
               }
               if (position > extent->point[1].x) {
                  position = extent->point[1].x;
               }
               value = ((float)(position - extent->point[0].x) / (float)(extent->point[1].x - extent->point[0].x));
               break;
            default:
               value = 0;
               break;
            }
            set_value = (int)(value * (this->max - this->min) + this->min);

            CObjPersistent(CGAnimation(this)->binding.object.object).attribute_set_int_element_index(CGAnimation(this)->binding.object.attr.attribute,
                                                                                                     CGAnimation(this)->binding.object.attr.element,
                                                                                                     CGAnimation(this)->binding.object.attr.index,
                                                                                                     set_value);
            /*>>>cheating, out of order */
            CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update(CGAnimation(this)->binding.object.attr.attribute);
            CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update_end();
         }
         break;
       default:
         break;
      }
      return TRUE;
   }
   return FALSE;
}/*CGAnimateInputSlider::event*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/