/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "environ.h"

#include "contain.h"
#include "exception.h"
#include "parameter.h"
#include "text.h"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CObject;

typedef enum {
   IT_Class,
   IT_Object,
   IT_ObjectDef,
} EInitType;

typedef struct {
   unsigned int sid;
   EInitType type;
} TInit;

/*>>>, only exception to 'C' vs 'T' naming prefix,
 * think about this*/

//struct CPERSPARAMETERSTABLE;            /*>>> foward delcation to persitent objects, a little icky*/

struct tag_CObjClass;
LIST:typedef<struct tag_CObjClass>;

/*>>> class CObjClass? */
typedef struct tag_CObjClass {
   const TInit init;
   const struct tag_CObjClass *base;
   const void *vtbl;
   const TParameter **ptbl;
   const char *name;
   const char *alias;
   const char *displayname;
   const int size;
   const int option;

   LIST<struct tag_CObjClass> child;
//   struct CPERSPARAMETERSTABLE *ptable;
} CObjClass;

typedef struct tag_TObjectDef {
   TInit init;
   const struct tag_CObject *obj;
   const struct tag_CObjClass *obj_class;
   void *data;
} TObjectDef;

void CObjClass_new(CObjClass *this);
bool CObjClass_valid_cast(const CObjClass *this, CObject *obj);
const CObjClass *CObjClass_size_lookup(const CObjClass *this, int size);
bool CObjClass_is_derived(const CObjClass *this, const CObjClass *derived);
int CObjClass_degrees(const CObjClass *this, const CObjClass *derived);
static inline const char *CObjClass_name(const CObjClass *this) {
   return this->name;
}
static inline const char *CObjClass_alias(const CObjClass *this) {
   return this->alias ? this->alias : this->name;
}
static inline const char *CObjClass_alias_short(const CObjClass *this) {
   char *cp;
   
   if (!this->alias) {
      return this->name;
   }
   
   cp = strchr(this->alias, ':');
   return cp ? &cp[1] : this->alias;
}
static inline const char *CObjClass_displayname(const CObjClass *this) {
   return this->displayname ? this->displayname : CObjClass_alias_short(this);
}

LIST:typedef<CObject>;

ENUM:typedef<EObjectLinkage> {
   {ChildAdd}, {ChildRemove}, {ChildRemovePre}, {ParentSet}, {ParentRelease}, {ParentReleasePre}
};

/*! The root of all evil */

class CObject : {
 private:
   TListItem base;
   const CObjClass *obj_class;
#if DEBUG
   ulong obj_sid;
#endif
   CObject *parent;
   LIST<CObject> child;
   bool mem_allocated;
   CObject *_construct(const CObjClass *obj_class);
   void _xdestroy(void);
   virtual void delete(void);
 protected:
   virtual void new(void);

 public:
   static inline const CObjClass *obj_class(void);
   static inline bool exists(void);
//   static inline void *mem_alloc(int size);
//   static inline void mem_free(void *data);
//   static inline int index(void);
   static inline int child_count(void);
   static inline void child_add(CObject *child);
   static inline void child_add_index(CObject *child, int index);
   static inline void child_add_front(CObject *child);
   static inline void child_add_before(CObject *child, CObject *sibling);
   static inline void child_add_after(CObject *child, CObject *sibling); 
   static inline void child_remove(CObject *child);
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   static inline int child_index(CObject *child);
   static inline void parent_set(CObject *parent);
   static inline CObject *parent(void);
   CObject *parent_find(const CObjClass *obj_class);
   static inline CObject *child_first(void);
   static inline CObject *child_last(void);
   static inline CObject *child_next(CObject *child);
   static inline CObject *child_previous(CObject *child);
   static inline CObject *child_n(int n);
   static inline CObject *child_tree_first(void);
   static inline CObject *child_tree_last(void);
   CObject *child_tree_next(CObject *child);
   CObject *child_tree_previous(CObject *child);   
//   static inline CObject *child_find(int id);
   bool is_child(CObject *object);

   EXCEPTION<InvalidCast>;
   EXCEPTION<MethodBadInput,MethodBadOutput,MethodInternalError>;
};

CObject *CObject_construct_new(CObject *this, const CObjClass *obj_class);
void CObject_construct_delete(CObject *this);

static inline const CObjClass *CObject::obj_class(void) {
   return this->obj_class;
}

static inline bool CObject::exists(void) {
   return this && this->obj_sid == SID_OBJ;
}

//static inline void *CObject::mem_alloc(int size) {
//   return calloc(1, size);
//}

//static inline void CObject::mem_free(void *data) {
//   free(data);
//}
//>>>static inline int CObject::index(void) {
//   return this->base.index;
//}
static inline int CObject::child_count(void) {
   return LIST(&this->child).count();
}
static inline void CObject::child_add(CObject *child) {
   if (!child)
      return;

   if (child->parent)
     ASSERT("CObject::child_add():child already has parent");
   LIST(&this->child).add(child, -1);
   child->parent = this;

   CObject(child).notify_object_linkage(EObjectLinkage.ParentSet, this);
   CObject(this).notify_object_linkage(EObjectLinkage.ChildAdd, child);
}

static inline void CObject::child_add_index(CObject *child, int index) {
   if (!child)
      return;

   if (child->parent)
     ASSERT("CObject::child_add_index():child already has parent");
   LIST(&this->child).add(child, index);
   child->parent = this;

   CObject(child).notify_object_linkage(EObjectLinkage.ParentSet, this);
   CObject(this).notify_object_linkage(EObjectLinkage.ChildAdd, child);
}

static inline void CObject::child_add_front(CObject *child) {
   if (!child)
      return;

   if (child->parent)
     ASSERT("CObject::child_add():child already has parent");
   LIST(&this->child).add_front(child, -1);
   child->parent = this;

   CObject(child).notify_object_linkage(EObjectLinkage.ParentSet, this);
   CObject(this).notify_object_linkage(EObjectLinkage.ChildAdd, child);
}

static inline void CObject::child_add_before(CObject *child, CObject *sibling) {
//   ASSERT("CObject::child_add_before():not implemented");
   if (!child)
      return;

   if (child->parent)
     ASSERT("CObject::child_add_before():child already has parent");
//   LIST(&this->child).insert_front(child, &sibling->base);
   if (!sibling) {
      LIST(&this->child).add(child, -1);
   }
   else {
      LIST(&this->child).insert_front(child, &sibling->base);
   }
   child->parent = this;

   CObject(child).notify_object_linkage(EObjectLinkage.ParentSet, this);
   CObject(this).notify_object_linkage(EObjectLinkage.ChildAdd, child);
}

static inline void CObject::child_add_after(CObject *child, CObject *sibling) {
   if (!child)
      return;

   if (child->parent)
     ASSERT("CObject::child_add_after():child already has parent");
   
   if (!sibling) {
      LIST(&this->child).add_front(child, -1);
   }
   else {
      LIST(&this->child).insert(child, &sibling->base);
   }
   child->parent = this;

   CObject(child).notify_object_linkage(EObjectLinkage.ParentSet, this);
   CObject(this).notify_object_linkage(EObjectLinkage.ChildAdd, child);
}

static inline void CObject::child_remove(CObject *child) {
   CObject(this).notify_object_linkage(EObjectLinkage.ChildRemovePre, child);

   LIST(&this->child).remove(child);

   CObject(this).notify_object_linkage(EObjectLinkage.ChildRemove, child);
   CObject(child).notify_object_linkage(EObjectLinkage.ParentRelease, this);

   child->parent = NULL;
}

static inline int CObject::child_index(CObject *child) {
   CObject *object;
   int i = 0;
   
   object = CObject(this).child_first();
   while (object) {
      if (object == child) {
         return i;
      }
      i++;
      object = CObject(this).child_next(object);
   }

   return -1;
}

static inline void CObject::parent_set(CObject *parent) {this->parent = parent; }
static inline CObject *CObject::parent(void) {return this->parent; }
static inline CObject *CObject::child_first(void) {
   return LIST(&this->child).first();
}
static inline CObject *CObject::child_last(void) {
   return LIST(&this->child).last();
}
static inline CObject *CObject::child_next(CObject *child) {
   return LIST(&this->child).next(child);
}
static inline CObject *CObject::child_previous(CObject *child) {
   return LIST(&this->child).previous(child);
}
static inline CObject *CObject::child_n(int n) {
   CObject *child;

   child = CObject(this).child_first();
   while (child && n) {
      child = CObject(this).child_next(child);
      n--;
   }
   return child;
}
static inline CObject *CObject::child_tree_first(void) {
    return CObject(this).child_first();
}/*CObject::child_tree_first*/
static inline CObject *CObject::child_tree_last(void) {
    CObject *last = this;
    
    while (CObject(last).child_last()) {
       last = CObject(last).child_last();
    }

    return last != this ? last : NULL; 
}/*CObject::child_tree_last*/

//static inline CObject *CObject::child_find(int index) {
//   return LIST(&this->child).find(index);
//}

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

typedef struct {
   CObject *obj;
   int obj_current_class_count;
//   OBJ_MESSAGE_CALLBACK obj_message_callback; /*>>>not threadsafe!*/
   byte obj_message_callback_data[16];
} TConstruct;

TConstruct construct;

Global const CObjClass *CObjClass_size_lookup(const CObjClass *this, int size) {
   const CObjClass *result = this;

   while (result) {
      if (result->size == size)
        break;
       result = result->base;
   }
   return result;
}/*CObjClass_size_lookup*/

Global bool CObjClass_is_derived(const CObjClass *this, const CObjClass *derived) {
   while (derived) {
      if (derived == this)
        break;
      derived = derived->base;
   }
   return derived != NULL;
}/*CObjClass_is_derived*/

Global int CObjClass_degrees(const CObjClass *this, const CObjClass *derived) {
   int i = 0;

   while (derived) {
      if (derived == this)
        break;
      derived = derived->base;
      i++;
   }
   if (!derived)
     ASSERT("OCLASS_degrees: not a derived class");

   return i;
}/*CObjClass_degrees*/

Global bool CObjClass_valid_cast(const CObjClass *this, CObject *obj) {
   const CObjClass *obj_class;
   

   if (obj == NULL) {
      //WARN("FRAMEWORK:OBJCLASS_cast_check - NULL pointer");
      return TRUE;
   }

#if 1
   if (obj->obj_sid == SID_OBJ_DELETED) {
      throw(obj, EXCEPTION<CObject,InvalidCast>,
            "Invalid Cast:deleted object referenced");
   }
   else if (obj->obj_sid == SID_OBJ) {
   }
   else {
      throw(obj, EXCEPTION<CObject,InvalidCast>, "Invalid Cast:Not an object");       
   }
#else   
   switch (obj->obj_sid) {
    case SID_OBJ_DELETED:
      throw(obj, EXCEPTION<CObject,InvalidCast>,
            "Invalid Cast:deleted object referenced");
      break;
    case SID_OBJ:
      break;
    default:
      throw(obj, EXCEPTION<CObject,InvalidCast>, "Invalid Cast:Not an object");
   }
#endif   

   obj_class = obj->obj_class;
   while (obj_class) {
      if (obj_class == this)
        break;

      obj_class = obj_class->base;
   }

   if (!obj_class)
      throw(obj, EXCEPTION<CObject,InvalidCast>, "Invalid Cast");

   return TRUE;
}/*CObjClass_valid_cast*/

Global CObject *CObject_construct_new(CObject *this, const CObjClass *obj_class) {
   ASSERT("OBJ_construct_new:not implemented");
   return NULL;
}/*CObject_construct_new*/

Global void CObject_construct_delete(CObject *this) {
   ASSERT("OBJ_construct_delete:not implemented");
}/*CObject_construct_delete*/

CObject *CObject::_construct(const CObjClass *obj_class) {
   if (this) {
#if DEBUG
      if (this->obj_sid == SID_OBJ) {
         WARN("OBJ_construct::reinit on possibly already initialized object memory");
      }
#endif
      memset(this, 0, obj_class->size);
   }
   else {
      this = calloc(1, obj_class->size);
      this->mem_allocated = TRUE;
   }

   construct.obj = this;

#if DEBUG
   this->obj_sid    = SID_OBJ;
#endif
   this->obj_class  = obj_class;
//   this->base.index = 0;

   CObject(this).new();

   return this;
}/*CObject::_construct*/

void CObject::new(void) {}
void CObject::delete(void) {}

void CObject::_xdestroy(void) {
   CObject *child, *child_next;

#if DEBUG
   if (this->obj_sid == SID_OBJ_DELETED)
     ASSERT("FRAMEWORK:OBJ_delete - object already deleted");

   CObjClass_valid_cast(this->obj_class, this);
#endif

   if (this->parent) {
      CObject(this->parent).notify_object_linkage(EObjectLinkage.ChildRemovePre, this);
      CObject(this).notify_object_linkage(EObjectLinkage.ParentReleasePre, this->parent);
   }

   /* delete objects children */
   child = CObject(this).child_first();
   while (child) {
      child_next = CObject(this).child_next(child);
      delete(child);
      child = child_next;
   }

//   if (this->parent) {
//      CObject(this->parent).notify_object_linkage(EObjectLinkage.ChildRemovePre, this);
//      CObject(this).notify_object_linkage(EObjectLinkage.ParentReleasePre, this->parent);
//   }
   CObject(this).delete();   
   if (this->parent) {
      LIST(&this->parent->child).remove(this);
      CObject(this->parent).notify_object_linkage(EObjectLinkage.ChildRemove, this);
      CObject(this).notify_object_linkage(EObjectLinkage.ParentRelease, this->parent);
      this->parent = NULL;
   }

   this->obj_sid = SID_OBJ_DELETED;

   if (this->mem_allocated) {
      free(this);
   }
}/*CObject::delete*/

void CObject::notify_object_linkage(EObjectLinkage linkage, CObject *object) {}

CObject *CObject::parent_find(const CObjClass *obj_class) {
   const CObjClass *parent_class;

   this = this->parent;
   while (this) {
      parent_class = this->obj_class;
      while (parent_class) {
         if (parent_class == obj_class)
            return this;
         parent_class = parent_class->base;
      }
      this = this->parent;
   }

   return NULL;
}/*CObject::parent_find*/

CObject *CObject::child_tree_next(CObject *child) {
   CObject *next;
    
   next = LIST(&child->child).first();
   if (next)
       return next;
  
   while (!next) {
       if (!child->parent || child == this) {
           break; 
       }
       next = LIST(&child->parent->child).next(child);       
       child = child->parent;              
   }       
   
   return next;
}/*CObject::child_tree_next*/

CObject *CObject::child_tree_previous(CObject *child) {
   CObject *object, *prev = NULL;
    
   object = child;

   child = LIST(&child->parent->child).previous(child);       
   while (child) {
       prev = child;
       child = LIST(&child->child).last(child);       
   }
   
   if (!prev) {
       if (!object->parent || object->parent == this) {       
           return NULL;
       }
       prev = object->parent;
   }       

   return prev;
}/*CObject::child_tree_previous*/

bool CObject::is_child(CObject *object) {
   while (object) {
      if (object == this)
         return TRUE;

      object = CObject(object).parent();
   }

   return FALSE;
}/*CObject::is_child*/

#if 0
//void CObject::message_callback(OBJ_MESSAGE_CALLBACK obj_message_callback,
//                                      void *data, int data_size) {
//   if (data_size > sizeof(construct.obj_message_callback_data)) {
//      ASSERT("FRAMEWORK_obj_message_callback:callback data too large");
//   }
//
//   construct.obj_message_callback = obj_message_callback;
//   memcpy(construct.obj_message_callback_data, data, data_size);
//}/*CObject::message_callback*/
#endif

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/