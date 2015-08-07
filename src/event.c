/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework_base.c"
#include "syncobject.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

#define STATE_TRACE FALSE

class CEvent : CSyncObject {
 private:
   void delete(void);
 public:
   CObject *source;
   
   void CEvent(void);
   virtual void print(void);
};

ENUM:typedef<EEventKeyType> {
   {down},
   {up},
   {repeat},
};

/* just a subset for now */
ENUM:typedef<EEventKey> {
   {None},    
   {Ascii},
   {Enter},
   {Escape},
   {Left},
   {Right},
   {Up},
   {Down},
   {Backspace},
   {Insert},
   {Delete},
   {Home},
   {End},
   {PgUp},
   {PgDn},
   {Tab},
   {Function},
   {Space},   
};

ENUM:typedef<EEventModifier> {
   {ctrlKey},
   {shiftKey},
   {altKey},
   {metaKey},
};

typedef struct {
   EEventKey key;
   char value;
   int modifier;                        /* EEventModifer bitfield*/
} TKeyboardKey;

ATTRIBUTE:typedef<TKeyboardKey>;

class CEventKey : CEvent {
 private:
   byte native_data[16];     
   
   void delete(void);
 public:
   EEventKeyType type;
   EEventKey key;
   char value;
   int modifier;                        /* EEventModifer bitfield*/
 
   void CEventKey(EEventKeyType type, EEventKey key, char value, int modifier);
};

/* Finite State Machine */

ENUM:typedef<EEventState> {
   {init},
   {enter},
   {leave},
   {parent_get},
};

typedef void* (*STATE)(CObject *this, CEvent *event);

class CEventState : CEvent {
 public:
   EEventState type;
   bool final;
   void CEventState(EEventState type);
};

class CFsm : CObject {
 private:
   STATE top(CEvent *event);

   STATE state;
   bool in_transition;
   STATE pending_state;

#if STATE_TRACE 
   void state_trace(STATE state, bool enter); 
#endif 

   void transition_initial(STATE state);
   void transition(STATE state);
   STATE parent_find(STATE stateA, STATE stateB);
   STATE child_find(STATE state_parent, STATE state_child); 
 public:
   void init(void);
   void CFsm(CObject *parent, STATE initial);
   void event(CEvent *event);
   static inline STATE state(void);
   bool in_state(STATE state);
};

STATE CFsm::state(void) {
   return this->state;
}

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CEvent::CEvent(void) {
}/*CEvent::CEvent*/

void CEvent::delete(void) {
}/*CEvent::delete*/

void CEvent::print(void) {
   printf("event\n");
}/*CEvent::print*/

#define KEYBOARD_STR_CTRL   "Ctrl"
#define KEYBOARD_STR_ALT    "Alt"
#define KEYBOARD_STR_SHIFT  "Shift"
#define KEYBOARD_STR_ENTER  "Enter"
#define KEYBOARD_STR_DELETE "Del"

bool ATTRIBUTE:convert<TKeyboardKey>(CObjPersistent *object,
                                     const TAttributeType *dest_type, const TAttributeType *src_type,
                                     int dest_index, int src_index,
                                     void *dest, const void *src) {
   TKeyboardKey *value;
   CString *string;
   char temp[80], *cp;

   if (dest_type == &ATTRIBUTE:type<TKeyboardKey> && src_type == NULL) {
      memset(dest, 0, sizeof(TKeyboardKey));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TKeyboardKey> && src_type == &ATTRIBUTE:type<TKeyboardKey>) {
      *((TKeyboardKey *)dest) = *((TKeyboardKey *)src);       
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TKeyboardKey>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TKeyboardKey> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (TKeyboardKey *)dest;
      string = (CString *)src;
      memset(value, 0, sizeof(TKeyboardKey));
       
      strcpy(temp, CString(string).string());
      cp = strtok(temp, "+");
      while (cp) {
          if (strcmp(cp, KEYBOARD_STR_CTRL) == 0) {
              value->modifier |= (1 << EEventModifier.ctrlKey);
          }
          else if (strcmp(cp, KEYBOARD_STR_ALT) == 0) {
              value->modifier |= (1 << EEventModifier.altKey);              
          }
          else if (strcmp(cp, KEYBOARD_STR_SHIFT) == 0) {
              value->modifier |= (1 << EEventModifier.shiftKey);                            
          }
          else if (strcmp(cp, KEYBOARD_STR_ENTER) == 0) {
              value->key = EEventKey.Enter;
          }
          else if (strcmp(cp, KEYBOARD_STR_DELETE) == 0) {
              value->key = EEventKey.Delete;
          }
          else {
             value->key = EEventKey.Ascii;              
             value->value = *cp;
          }
          cp = strtok(NULL, "+");
      }          
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TKeyboardKey>) {
      value  = (TKeyboardKey *)src;
      string = (CString *)dest;
      CString(string).printf("%c", value->value);
      return TRUE;
   }
   return FALSE;
}

void CEventKey::CEventKey(EEventKeyType type, EEventKey key, char value, int modifier) {
   this->type     = type;
   this->key      = key;
   this->value    = value;
   this->modifier = modifier;   
}/*CEventKey::CEventKey*/

void CEventKey::delete(void) {
}/*CEventKey::delete*/

void CEventState::CEventState(EEventState type) {
   this->type = type;
}/*CEventState::CEventState*/

void CFsm::CFsm(CObject *parent, STATE initial) {
   CObject(this).parent_set(parent);
   this->state  = initial;
}/*CFsm::CFsm*/

STATE CFsm::top(CEvent *event) {
   return NULL;
}/*CFsm::top*/

void CFsm::init(void) {
   CEventState event;
   STATE state = NULL;

   new(&event).CEventState(EEventState.init);

   while (state != this->state) {
      state = this->state;
      CFsm(this).event(CEvent(&event));
   }

//   event.type = EEventState.enter;
//   CFsm(this).event(CEvent(&event));
   state = this->state;
   this->state = (STATE)&CFsm:top;
   CFsm(this).transition(state);

   delete(&event);
}/*CFsm::init*/

void CFsm::event(CEvent *event) {
   STATE state = this->state;

   while (state) {
      state = (*state)(CObject(this).parent(), event);
   }
}/*CFsm::event*/

#if STATE_TRACE
void CFsm::state_trace(STATE state, bool enter) {
   printf("%s: %s::%x\n", enter ? "ENTER" : "LEAVE", CObjClass_name(CObject(CObject(this).parent()).obj_class()), state);
}/*void CFsm::state_trace*/
    
#endif

void CFsm::transition(STATE state) {
   CEventState event, parent_event;
   STATE super_state, parent_state = CFsm(this).parent_find(this->state, state);

   if (this->in_transition) {
      if (this->pending_state) {
         ASSERT("state machine in alread in transistion");
      }
      this->pending_state = state;
      return;
   }
   this->in_transition = TRUE;
    
   new(&parent_event).CEventState(EEventState.parent_get);
    
   if (this->state) {
      new(&event).CEventState(EEventState.leave);
      super_state = this->state;
      if (super_state == state) {
#if STATE_TRACE
         CFsm(this).state_trace(super_state, FALSE);
#endif
         (*super_state)(CObject(this).parent(), CEvent(&event));
      }
      else {      
         while (super_state && super_state != parent_state) {
#if STATE_TRACE
            CFsm(this).state_trace(super_state, FALSE);
#endif
            (*super_state)(CObject(this).parent(), CEvent(&event));
            super_state = (*super_state)(CObject(this).parent(), CEvent(&parent_event));
         } 
      }
      delete(&event);
   } 

//   this->state = state;

   new(&event).CEventState(EEventState.enter);
   super_state = parent_state;
   while (super_state) {
      if (super_state == state ||
          (super_state != state && super_state != parent_state)) {
#if STATE_TRACE
         CFsm(this).state_trace(super_state, TRUE);
#endif
         event.final = super_state == state;
         (*super_state)(CObject(this).parent(), CEvent(&event));
      }
      super_state = CFsm(this).child_find(super_state, state);
   }
   delete(&event);
   delete(&parent_event);
   
this->state = state;   
   
   this->in_transition = FALSE;
   if (this->pending_state) {
       state = this->pending_state;
       this->pending_state = NULL;
       CFsm(this).transition(state);
   }
}/*CFsm::transition*/

STATE CFsm::parent_find(STATE stateA, STATE stateB) {
   /* Return common parent state of two states */
   STATE super_state_A = stateA;
   STATE super_state_B = stateB;    
   CEventState event;

   new(&event).CEventState(EEventState.parent_get);
   while (super_state_A) {
      super_state_B = stateB;
      while (super_state_B) {
         if (super_state_A == super_state_B) {
             delete(&event);
             return super_state_A;
         }             
         super_state_B = (*super_state_B)(CObject(this).parent(), CEvent(&event));                 
      }          
      super_state_A = (*super_state_A)(CObject(this).parent(), CEvent(&event));       
   }       
   delete(&event);

   return NULL;
}/*CFsm::parent_find*/

STATE CFsm::child_find(STATE state_parent, STATE state_child) {
   STATE state_next, state_result = NULL, super_state = state_child;
   CEventState event;    
    
   new(&event).CEventState(EEventState.parent_get);
   while (super_state) {
      state_next = (*super_state)(CObject(this).parent(), CEvent(&event));
      if (state_next == state_parent) {
          state_result = super_state;
          break;
      }
      super_state = state_next;
   }
   delete(&event); 
   
   return state_result;
}/*CFsm::child_find*/

void CFsm::transition_initial(STATE state) {
   this->state = state;
}/*CFsm::transition_initial*/

bool CFsm::in_state(STATE state) {
   STATE super_state = this->state;
   CEventState event;
   bool result = FALSE;

   new(&event).CEventState(EEventState.parent_get);
   while (super_state) {
      if (super_state == state) {
         result = TRUE;
      }
      super_state = (*super_state)(CObject(this).parent(), CEvent(&event));
    }
   delete(&event);

   return result;
}/*CFsm::in_state*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/