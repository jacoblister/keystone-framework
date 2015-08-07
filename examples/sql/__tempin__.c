# 1 "/home/jacob/keystone/examples/sql/client.c"
                                                                              
MODULE::IMPORT                                                                
                                                                              

# 1 "/home/jacob/keystone/src/base/framework.c"








  

# 1 "/home/jacob/keystone/src/framework_base.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              

#include "/home/jacob/keystone/src/base/environ.h"
#include "/home/jacob/keystone/src/base/contain.h"
#include "/home/jacob/keystone/src/base/exception.h"
#include "/home/jacob/keystone/src/base/parameter.h"
#include "/home/jacob/keystone/src/base/text.h"

                                             
ARRAY:typedef<char>;
ARRAY:typedef<char *>;
ARRAY:typedef<byte>;
ARRAY:typedef<int>;

# 1 "/home/jacob/keystone/src/object.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              

#include "/home/jacob/keystone/src/base/environ.h"

#include "/home/jacob/keystone/src/base/contain.h"
#include "/home/jacob/keystone/src/base/exception.h"
#include "/home/jacob/keystone/src/base/parameter.h"
#include "/home/jacob/keystone/src/base/text.h"

                                                                              
MODULE::INTERFACE                                                             
                                                                              

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


                     



struct tag_CObjClass;
LIST:typedef<struct tag_CObjClass>;

                         
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

   if (!child)
      return;

   if (child->parent)
     ASSERT("CObject::child_add_before():child already has parent");

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
}                             
static inline CObject *CObject::child_tree_last(void) {
    CObject *last = this;
    
    while (CObject(last).child_last()) {
       last = CObject(last).child_last();
    }

    return last != this ? last : NULL; 
}                            





                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

typedef struct {
   CObject *obj;
   int obj_current_class_count;

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
}                         

Global bool CObjClass_is_derived(const CObjClass *this, const CObjClass *derived) {
   while (derived) {
      if (derived == this)
        break;
      derived = derived->base;
   }
   return derived != NULL;
}                        

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
}                     

Global bool CObjClass_valid_cast(const CObjClass *this, CObject *obj) {
   const CObjClass *obj_class;
   

   if (obj == NULL) {

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
}                        

Global CObject *CObject_construct_new(CObject *this, const CObjClass *obj_class) {
   ASSERT("OBJ_construct_new:not implemented");
   return NULL;
}                         

Global void CObject_construct_delete(CObject *this) {
   ASSERT("OBJ_construct_delete:not implemented");
}                            

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


   CObject(this).new();

   return this;
}                       

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

                                
   child = CObject(this).child_first();
   while (child) {
      child_next = CObject(this).child_next(child);
      delete(child);
      child = child_next;
   }





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
}                   

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
}                        

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
}                            

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
}                                

bool CObject::is_child(CObject *object) {
   while (object) {
      if (object == this)
         return TRUE;

      object = CObject(object).parent();
   }

   return FALSE;
}                     

#if 0









#endif

                                                                              
MODULE::END                                                                   
                                                                              
# 28 "/home/jacob/keystone/src/framework_base.c"

                                                                              
MODULE::INTERFACE                                                             
                                                                              

                                                                 

#include "time.h"
typedef unsigned long TUNIXTime;

#define KEYSTONE_LICENSE_LENGTH 200

                                  
ENUM:typedef<ETLSIndex> {
   {This}
};

class CApplication;
class CThread;
class CMemFileDirectory;
class CGWindow;

ARRAY:typedef<CObjClass *>;
ARRAY:typedef<TObjectDef *>;
ARRAY:typedef<CGWindow *>;

#define KEYSTONE_SCRATCH_BUFFERS 16

class CFramework : CObject {
 private:
   CThread *thread_main;
   void *last_object;
   bool executing;
   CObject *modal_object;
   int modal_count;

   CObjClass         *obj_class_current;


   ARRAY<CObjClass *> obj_class;
   ARRAY<TObjectDef *> auto_object;
   ARRAY<CGWindow *> window;
   CGWindow *active_window;           
   CGWindow *modal_window;
   bool full_graphics;
   CApplication *application;

   void *scratch_buffer[KEYSTONE_SCRATCH_BUFFERS];
#if OS_Win32
   ARRAY<int> tls_index;
#endif
   void new(void);
   int execute(int argc, char *argv[]);
   void delete(void);
   void initdata_search(void);
   CApplication *construct_auto_object(void);
   void NATIVE_new(void);
   void NATIVE_delete(void);
   void NATIVE_main_iteration(void);
   void NATIVE_main(void);
   void NATIVE_kill(void);
   bool display_device_resolution(int *res_x, int *res_y);
 public:
   CMemFileDirectory *memfile_directory;
   
                                       
   char xul_font_family[256];
   int xul_font_size;
   int xul_font_weight;
   int xul_font_style;

   void main(void);
   void kill(void);
   void NATIVE_modal_wait(CObject *object);
   void NATIVE_modal_exit(CObject *object);
   void beep(void);

   void *scratch_buffer_allocate(void);
   void scratch_buffer_release(void *buffer);
   
   void debug_message(char *format, ...);

   CObjClass *obj_class_find(const char *class_name);
   void obj_class_list_all(void);
   static inline CThread *thread_main(void);
   CThread *thread_this(void);
       
   static inline CGWindow *window_active(void);
   static inline void window_active_set(CGWindow *window);
   
   void time_time(TUNIXTime *result);
   void time_time_local(TUNIXTime *result);   
   struct tm* time_localtime(TUNIXTime *time);

   void license_info(char *result);
   void license_components(char *result);
};

extern CFramework framework;

static inline CThread *CFramework::thread_main(void) {
   return this->thread_main;
}

static inline CGWindow *CFramework::window_active(void) {
    return this->active_window;
}

static inline void CFramework::window_active_set(CGWindow *window) {
    this->active_window = window;
}

void FRAMEWORK_sync_init(void);
void FRAMEWORK_sync_deinit(void);



                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

#include <time.h>

void CFramework::time_time(TUNIXTime *result) {
   time_t time_current;
   time(&time_current);
   *result = (TUNIXTime)time_current;
}

time_t mktime_gmt(struct tm *ptm) {
#if OS_Win32	
   return _mkgmtime(ptm);
#else
   return 0;
#endif	
}

void CFramework::time_time_local(TUNIXTime *result) {
   struct tm local;
   time_t time_current;
   
   time(&time_current);
   local = *(localtime(&time_current));
   *result = (TUNIXTime)mktime_gmt(&local);

}

struct tm* CFramework::time_localtime(TUNIXTime *time) {
   time_t time_current = *time;
   return localtime(&time_current);
}

                                                                              
MODULE::END                                                                   
                                                                              

# 12 "/home/jacob/keystone/src/base/framework.c"
# 1 "/home/jacob/keystone/src/main.c"






   









  

                                                                              
MODULE::IMPORT                                                                
                                                                              

                                                        




# 1 "/home/jacob/keystone/src/cstring.c"









  
                                                                              
MODULE::IMPORT                                                                
                                                                              





                                                               
#include <stdarg.h>

                                                                              
MODULE::INTERFACE                                                             
                                                                              








ENUM:typedef<EStringEncoding> {
   {ASCII}, {UTF16}
};

                               
                                        
class CString : CObject {
 private:
   void delete(void);

   EStringEncoding encoding;
   ARRAY<char> data;

 public:
   void CString(const char *str);
   void encoding_set(EStringEncoding encoding);
   void set(const char *str);
   void setn(const char *str, int n);
   void set_string(const CString *str);
   void append(unsigned short character);
   void appendn(const char *str, int n);
   void append_string(const CString *str);
   static inline void clear(void);
   static inline bool empty(void);
   void printf(const char *format, ...);
   void printf_append(const char *format, ...);
   void printf_prepend(const char *format, ...);
   void printf_insert(const char *start, int count, char *format, ...);
   void insert(int after, const char *str);
   void erase(int index, int count);
   void extract(CString *dest, const char *begin, const char *end);
   bool tokenise(CString *dest, const char *delim, bool strip_token);
   static inline char* string(void);
   static inline unsigned short* wstring(void);
   static inline bool match(CString *str);
   static inline int strcmp(const char *str);
   static inline int strncmp(const char *str, int n);
   static inline char *strchr(char c);
   static inline char *strrchr(char c);
   char *strmatch(const char *c);
   static inline int length(void);
   void print(void);
};

ARRAY:typedef<CString>;

static inline void CString::clear(void) {
   ARRAY(&this->data).used_set(0);
}                  

static inline bool CString::empty(void) {
   return ARRAY(&this->data).count() == 0;
}                  

static inline char *CString::string(void) {
   if (this->encoding != EStringEncoding.ASCII) {

     CString(this).encoding_set(EStringEncoding.ASCII);
   }
   return ARRAY(&this->data).count() ? ARRAY(&this->data).data() : "";
}                   

static inline unsigned short *CString::wstring(void) {

   if (this->encoding != EStringEncoding.UTF16) {
      ASSERT("CString::wstring - wrong encoding)");
   }

   return ARRAY(&this->data).count() ? (unsigned short *)ARRAY(&this->data).data() : (unsigned short *)"\0\0";
}                    

static inline bool CString::match(CString *str) {
   int i;

   if (this->encoding != str->encoding ||
      CString(this).length() != CString(str).length()) {
      return FALSE;
   }

   for (i = 0; i < ARRAY(&this->data).count(); i++) {
      if (ARRAY(&this->data).data()[i] != ARRAY(&str->data).data()[i]) {
         return FALSE;
      }
   }

   return TRUE;
}                  

static inline int CString::strcmp(const char *str) {
   if (this->encoding != EStringEncoding.ASCII) {
      ASSERT("CString::strcmp - wrong encoding)");
   }

   if (CString(this).empty()) {
      return -1;
   }
   else {
      return strcmp(ARRAY(&this->data).data(), str);
   }
}                   

static inline int CString::strncmp(const char *str, int n) {
   if (this->encoding != EStringEncoding.ASCII) {
      ASSERT("CString::strncmp - wrong encoding)");
   }

   if (CString(this).empty()) {
      return -1;
   }
   else {
      return strncmp(ARRAY(&this->data).data(), str, n);
   }
}                    

static inline char *CString::strchr(char c) {
   if (this->encoding != EStringEncoding.ASCII) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (!ARRAY(&this->data).data()) {
      return NULL;
   }
   return strchr(ARRAY(&this->data).data(), c);
}                   

static inline char *CString::strrchr(char c) {
   if (this->encoding != EStringEncoding.ASCII) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (!ARRAY(&this->data).data()) {
      return NULL;
   }
   return strrchr(ARRAY(&this->data).data(), c);
}                    

static inline int CString::length(void) {
   if (this->encoding != EStringEncoding.ASCII) {
      return ARRAY(&this->data).count() ? ARRAY(&this->data).count() / 2 - 1 : 0;
   }
   else {
      return ARRAY(&this->data).count() ? (strlen(ARRAY(&this->data).data())) : 0;
   }
}                   

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              


#define STATIC_BUFFER_SIZE 50000

#if 0





#endif

void CString::CString(const char *str) {
   ARRAY(&this->data).new();
   CString(this).set(str);
}                    

void CString::delete(void) {
   ARRAY(&this->data).delete();
}                   

void CString::encoding_set(EStringEncoding encoding) {
                                                                                
                                        
   int i, count;

   if (this->encoding == EStringEncoding.ASCII && encoding == EStringEncoding.UTF16) {
      count = ARRAY(&this->data).count();
      ARRAY(&this->data).used_set(count * 2);
      for (i = count - 1; i >= 0; i--) {
         ARRAY(&this->data).data()[i * 2] = ARRAY(&this->data).data()[i];
      }
      for (i = 0; i < count; i++) {
         ARRAY(&this->data).data()[i * 2 + 1] = '\0';
      }
   }
   else if (this->encoding == EStringEncoding.UTF16 && encoding == EStringEncoding.ASCII) {
                                        
      count = ARRAY(&this->data).count() / 2;
      for (i = 0; i < count; i++) {
         ARRAY(&this->data).data()[i] = ARRAY(&this->data).data()[i * 2];
      }
      ARRAY(&this->data).used_set(count);
   }

   this->encoding = encoding;
}                         

void CString::set(const char *str) {
   this->encoding = EStringEncoding.ASCII;

   if (str) {
      ARRAY(&this->data).data_set(strlen(str) + 1, str);
   }
   else {
      ARRAY(&this->data).used_set(0);
   }
}                

void CString::setn(const char *str, int n) {
   if (this->encoding) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (str) {
      ARRAY(&this->data).used_set(n + 1);
      strncpy(ARRAY(&this->data).data(), str, n);
      ARRAY(&this->data).data()[n] = '\0';
   }
}                 

void CString::set_string(const CString *str) {
   EStringEncoding encoding;

   ARRAY(&this->data).data_set(ARRAY(&str->data).count(), ARRAY(&str->data).data());
   encoding = this->encoding;
   this->encoding = str->encoding;
   CString(this).encoding_set(encoding);
}                       

char *CString::strmatch(const char *c) {
                    
   return CString(this).strchr(']');
}                     

void CString::append(unsigned short character) {
   CString(this).appendn((const char *)&character, this->encoding ? 2 : 1);
}                   

void CString::appendn(const char *str, int n) {
   int used;
   if (str) {
      used = ARRAY(&this->data).count();
      if (used == 0) {
         used = 2;
      }
      ARRAY(&this->data).used_set(used + n);


      memcpy(&ARRAY(&this->data).data()[used - 2], str, n);
      ARRAY(&this->data).data()[used + n - 2] = '\0';
      ARRAY(&this->data).data()[used + n - 1] = '\0';
   }
}                    

void CString::append_string(const CString *str) {
   EStringEncoding encoding;
   int used;

   encoding = this->encoding;
   this->encoding = str->encoding;
   CString(this).encoding_set(encoding);

   used = CString(this).length() * (this->encoding ? 2 : 1);
   ARRAY(&this->data).used_set(used + ARRAY(&str->data).count());
   memcpy(&ARRAY(&this->data).data()[used], ARRAY(&str->data).data(), ARRAY(&str->data).count());
}                       

void CString::printf(const char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   if (this->encoding) {
                                        
      this->encoding = 0;
   }

   va_start(args, format);

   vsprintf(output, format, args);
   CString(this).set(output);

   CFramework(&framework).scratch_buffer_release(output);
}                   

void CString::printf_append(const char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   if (this->encoding != EStringEncoding.ASCII) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   va_start(args, format);

   sprintf(output, "%s", CString(this).string());
   vsprintf(output + strlen(output), format, args);
   CString(this).set(output);

   CFramework(&framework).scratch_buffer_release(output);
}                          

void CString::printf_insert(const char *start, int count, char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   if (this->encoding != EStringEncoding.ASCII) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   va_start(args, format);

   strncpy(output, CString(this).string(), start - CString(this).string());
   vsprintf(output + (start - CString(this).string()), format, args);
   sprintf(output + strlen(output), "%s", CString(this).string() + (start - CString(this).string() + count));
   CString(this).set(output);

   CFramework(&framework).scratch_buffer_release(output);
}                          

void CString::printf_prepend(const char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   if (this->encoding != EStringEncoding.ASCII) {

       CString(this).encoding_set(EStringEncoding.ASCII);
   }

   va_start(args, format);

   vsprintf(output, format, args);
   sprintf(output + strlen(output), "%s", CString(this).string());
   CString(this).set(output);

   CFramework(&framework).scratch_buffer_release(output);
}                           

void CString::insert(int after, const char *str) {
   char *cp;

   if (this->encoding) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   ARRAY(&this->data).used_set(ARRAY(&this->data).count() + strlen(str));
   cp = CString(this).string();

   memmove(cp + after + 1 + strlen(str), cp + after + 1, strlen(cp + after + 1) + 1);
   memcpy(cp + after + 1, str, strlen(str));
}                   

void CString::erase(int index, int count) {
   char *cp = CString(this).string() + index;

   if (this->encoding) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   memmove(cp, cp  + 1, strlen(cp + 1) + 1);
   ARRAY(&this->data).used_set(ARRAY(&this->data).count() - count);
}                  

void CString::extract(CString *dest, const char *begin, const char *end) {
   if (this->encoding) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (end < begin)
     ASSERT("CString::end < begin)");

   ARRAY(&dest->data).used_set(end - begin + 2);
   memmove(ARRAY(&dest->data).data(), begin, end - begin + 1);

   ARRAY(&dest->data).data()[end - begin + 1] = '\0';
}                    

bool CString::tokenise(CString *dest, const char *delim, bool strip_token) {
   char *end = NULL;
   bool token = FALSE;

   if (this->encoding) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (!CString(this).length()) {
      CString(dest).clear();
      return FALSE;
   }

   end = CString(this).string() + (strip_token ? 0 : 1);
   while (*(end + 1)) {
      if (strchr(delim, *end)) {
         end -= (strip_token ? 0 : 1);
         token = TRUE;
         break;
      }
      end++;
   }

   CString(this).extract(dest, CString(this).string(), end);
   if (token && strip_token) {
      CString(dest).setn(CString(dest).string(), CString(dest).length() - 1);
   }
   CString(this).set(strlen(end) ? end + 1 : end);

   return TRUE;
}                     

void CString::print(void) {
   if (this->encoding) {

      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   printf("%s\n", CString(this).string());
}                  

                                                                              
MODULE::END                                                                   
                                                                              
# 29 "/home/jacob/keystone/src/main.c"
# 1 "/home/jacob/keystone/src/datetime.c"








  
                                                                              
MODULE::IMPORT                                                                
                                                                              

                                         
#include <time.h>



                                                                              
MODULE::INTERFACE                                                             
                                                                              

                        
                                                    



ATTRIBUTE:typedef<TUNIXTime>;

ENUM:typedef<EUNIXTimePart> {
   {Year},
   {Month},
   {Day},
   {Hour},
   {Minute},
   {Second},
   {DayOfWeek},
};

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

# 1 "/home/jacob/keystone/src/objpersistent.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              



# 1 "/home/jacob/keystone/src/objcontainer.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              



                                                                              
MODULE::INTERFACE                                                             
                                                                              

class CObjPersistent;

typedef struct {
   const TAttribute *attribute;
   int element;
   int index;
} TAttributePtr;

typedef struct {
   CObjPersistent *object;
   TAttributePtr attr;
} TObjectPtr;

ARRAY:typedef<TObjectPtr>;

class CSelection : CObject {
 private:
   void new(void); 
   void delete(void);
   ARRAY<TObjectPtr> selection;
   bool multi;
 public:
   void CSelection(bool multi);

   ARRAY<TObjectPtr> *selection(void);
   void clear(void);
   void add(CObjPersistent *object, TAttribute *attribute);
   void remove(CObjPersistent *object, TAttribute *attribute);
   bool selected(CObjPersistent *object, TAttribute *attribute);
   static inline int count(void);
   void delete_selected(void);
};

static inline int CSelection::count(void) {
   return ARRAY(&this->selection).count();
}                     

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CSelection::new(void) {
   ARRAY(&this->selection).new();
}                   

void CSelection::CSelection(bool multi) {
   this->multi = multi;
}                          

void CSelection::delete(void) {
   ARRAY(&this->selection).delete();
}                      

ARRAY<TObjectPtr> *CSelection::selection(void) {
   return &this->selection;
}                         

void CSelection::clear(void) {
   int i;
   TObjectPtr *item;

   for (i = 0; i < ARRAY(&this->selection).count(); i++) {
      item = &ARRAY(&this->selection).data()[i];
   }

   ARRAY(&this->selection).used_set(0);
}                     

void CSelection::add(CObjPersistent *object, TAttribute *attribute) {
   TObjectPtr item;

   if (CSelection(this).selected(object, attribute)) {
                                                    
       return;
   }

   CLEAR(&item);

   item.object    = object;
   item.attr.attribute = attribute;
   ARRAY(&this->selection).item_add(item);
}                   

void CSelection::remove(CObjPersistent *object, TAttribute *attribute) {
   TObjectPtr item;

   CLEAR(&item);
   item.object    = object;
   item.attr.attribute = attribute;
   ARRAY(&this->selection).item_remove(item);
}                      

bool CSelection::selected(CObjPersistent *object, TAttribute *attribute) {
   TObjectPtr *item;
   int i;

   for (i = 0; i < ARRAY(&this->selection).count(); i++) {
      item = &ARRAY(&this->selection).data()[i];
      if (item->object == object && item->attr.attribute == attribute) {
         return TRUE;
      }
   }
   return FALSE;
}                        

void CSelection::delete_selected(void) {
   TObjectPtr *item;
   int i;

   for (i = 0; i < ARRAY(&this->selection).count(); i++) {
      item = &ARRAY(&this->selection).data()[i];
      delete(item->object);
   }
   CSelection(this).clear();
}                               

                                                                              
MODULE::END                                                                   
                                                                              


# 18 "/home/jacob/keystone/src/objpersistent.c"

                                                                              
MODULE::INTERFACE                                                             
                                                                              

#define ATTR_ELEMENT_VALUE -1
#define ATTR_INDEX_VALUE   -1
#define ATTR_INDEX_DEFAULT -2

class CIOObject;

class CBinData : CObject {
 private:
   CString header;
   ARRAY<byte> data;

   void new(void);
   void delete(void);
 public:
   void CBinData(void);
};

const typedef CObjClass *TClassPtr;

typedef int op_compare;

ARRAY:typedef<const TAttribute *>;
ARRAY:typedef<int>;
ARRAY:typedef<bool>;
ARRAY:typedef<float>;
ARRAY:typedef<double>;
ARRAY:typedef<short>;
ARRAY:typedef<word>;
ARRAY:typedef<byte>;

ATTRIBUTE:typedef<op_compare>;

ATTRIBUTE:typedef<int>;
ATTRIBUTE:typedef<bool>;
ATTRIBUTE:typedef<float>;
ATTRIBUTE:typedef<double>;
ATTRIBUTE:typedef<short>;
ATTRIBUTE:typedef<word>;
ATTRIBUTE:typedef<byte>;
ATTRIBUTE:typedef<char>;
ATTRIBUTE:typedef<CString>;
ATTRIBUTE:typedef<TBitfield>;
ATTRIBUTE:typedef<TObjectPtr>;
ATTRIBUTE:typedef<TClassPtr>;
ATTRIBUTE:typedef<TAttributePtr>;
ATTRIBUTE:typedef<CBinData>;












   

class CObjPersistent : CObject {
 private:
   TBitfield attribute_default;

   void new(void);
   void delete(void);
   void attribute_set(const TAttribute *attribute, int attribute_element, int attribute_index, const TAttributeType *type, const void *data);
   void attribute_get(const TAttribute *attribute, int attribute_element, int attribute_index, const TAttributeType *type, void *data);

   void attribute_default_refresh_all(CObjPersistent *object, const TAttribute *attribute);

   void attribute_string_process(const TAttribute *attribute, CString *data);
   void state_xml_store_obj(CIOObject *stream, const char *default_namespace, int depth);

   CObjPersistent *copy_fast(CObjPersistent *dest, bool copy_child, bool copy_fast);
 protected:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_selection_update(bool changing);
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_all_update(bool changing);
 public:
   void CObjPersistent(void);
   CObjPersistent *copy(bool copy_child);
   CObjPersistent *copy_clone(CObjPersistent *dest, bool copy_child);
   
   CObjPersistent *child_first(void);
   CObjPersistent *child_last(void);
   CObjPersistent *child_next(CObjPersistent *child);
   CObjPersistent *child_previous(CObjPersistent *child);   
   CObjPersistent *child_tree_first(void);
   CObjPersistent *child_tree_last(void);
   CObjPersistent *child_tree_next(CObjPersistent *child);
   CObjPersistent *child_tree_previous(CObjPersistent *child);   

   void attribute_update(const TAttribute *attribute);
   void attribute_update_end(void);
   void selection_update(void);                                     
   void selection_update_end(void);                                 

                                   
   void attribute_set_string(const TAttribute *attribute, const CString *string);
   void attribute_set_string_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, const CString *string);
   void attribute_get_string(const TAttribute *attribute, CString *string);
   void attribute_get_string_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, CString *string);
   void attribute_set_text(const TAttribute *attribute, const char *text);
   void attribute_set_text_element_index(const TAttribute *attribute, int attribtue_element, int attribute_index, const char *text);
   int attribute_get_int(const TAttribute *attribute);
   int attribute_get_int_element_index(const TAttribute *attribute, int attribute_element, int attribute_index);
   void attribute_set_float(const TAttribute *attribute, float value);
   float attribute_get_float(const TAttribute *attribute);
   void attribute_set_double(const TAttribute *attribute, double value);
   double attribute_get_double(const TAttribute *attribute);
   void attribute_set_int(const TAttribute *attribute, int value);
   void attribute_set_int_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, int value);

   void attribute_refresh(bool refresh_child);
   int  attribute_array_count(const TAttribute *attribute);
   bool attribute_default_get(const TAttribute *attribute);
   void attribute_default_set(const TAttribute *attribute, bool is_default);
   bool attribute_set_inherit(const TAttribute *attribute);

                                                   
   int attribute_list(ARRAY<const TAttribute *> *result,
                      bool list_attribute, bool list_elements, bool list_menu);
   int  attribute_index_find(const TAttribute *attribute);
   const TAttribute *attribute_find(const char *attribute_name);
   const TParameter *element_find(const char *element_name);
   const TParameter *data_find(void);

   bool child_alias_sibling_index(CObjPersistent *child, int *index);
   CObjPersistent *child_alias_sibling_index_find(const char *alias, int index);
   CObjPersistent *child_alias_sibling_match(const char *alias, const char *attr_name, const char *attr_value, int *index_result);
   CObjPersistent *child_element_class_find(CObjClass *obj_class, const TAttribute **attribute_result);

   void state_xml_load(CIOObject *stream, const char *default_namespace, bool comments);
   bool state_xml_loading(void);   
   bool state_xml_load_file(const char *filename, const char *default_namespace, bool comments, CString *message);
   void state_xml_store(CIOObject *stream, const char *default_namespace);
   
   bool state_loading(void);   
   
   EXCEPTION<BadAttribute, BadConversion, XMLWriteError, AttributeTypeMismatch, SetError>;   
};

          

           
ENUM:typedef<EXLinkShow> {
   {new},                                                                  
   {replace},                                                              
};

class CXLink : CObject {
 private:
   CObjPersistent *root;

   CObjPersistent *object;
   CString value;

   void new(void);
   void delete(void);

   void link_choose(void);
 public:
   void CXLink(CObjPersistent *root, CObjPersistent *object);
   static inline void root_set(CObjPersistent *root);
   CObjPersistent *object_find(CObjPersistent *object, const char *name);
   CObjPersistent *object_resolve(void);

   static inline CObjPersistent *object_get(void);
   void link_set(const char *link);
   void link_get(CString *link);
   bool link_get_file(CString *link);
   bool link_get_path(CString *link);
};

static inline void CXLink::root_set(CObjPersistent *root) {
   this->root = root;
}                            

static inline CObjPersistent *CXLink::object_get(void) {
   return CObjPersistent(CObject(this).child_first());
}                      

ATTRIBUTE:typedef<CXLink>;

           

class CObjClient;

ENUM:typedef<EPathOperation> {
   {none}, 
   {exists},                                                             
   {count},                                                                 
   {not},                                                                         
   {neg},                                                                            
   {op_delete, "delete"},                                                              
   {string},                                                                     
   {alias},                                                                         
};

                                      
class CXPath : CSelection {
 private:
   CObjPersistent *root;

   CString path;
   TObjectPtr initial;

   TObjectPtr object;

   int index;                                                                
   int factor;
   int offset;
   EPathOperation operation;
   const char *assignment, *assignment_end;

   void new(void);
   void delete(void);
 public:
   void CXPath(CObjPersistent *initial, CObjPersistent *root);
   void initial_set_path(const char *path);
   static inline void initial_set(TObjectPtr *initial);
   static inline void root_set(CObjPersistent *root);
   static inline void index_set(int index);
   void root_set_default(CObjPersistent *parent);
   void set(CObjPersistent *object, const TAttribute *attribute, int index);
   bool match(CObjPersistent *object, const TAttribute *attribute, int index);
   void path_set(const char *path);
   void path_get(CString *path);


   void resolve(void);
   int get_int(void);
   void get_string(CString *result);

   static inline const char* assignment(void);
   bool assignment_get_string(CString *result);
};

static inline void CXPath::initial_set(TObjectPtr *initial) {
   this->initial = *initial;
}                       

static inline void CXPath::root_set(CObjPersistent *root) {
   this->root = root;
}                    

static inline void CXPath::index_set(int index) {
   this->index = index;
}                     

static inline const char* CXPath::assignment(void) {
   return this->assignment;
}                      

ATTRIBUTE:typedef<CXPath>;

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

#include <ctype.h>
#include "/home/jacob/keystone/src/cencode.h"
#include "/home/jacob/keystone/src/cdecode.h"

bool ATTRIBUTE:convert<op_compare>(CObjPersistent *object,
                                   const TAttributeType *dest_type, const TAttributeType *src_type,
                                   int dest_index, int src_index,
                                   void *dest, const void *src) {
   return FALSE;
}

bool ATTRIBUTE:convert<int>(CObjPersistent *object,
                            const TAttributeType *dest_type, const TAttributeType *src_type,
                            int dest_index, int src_index,
                            void *dest, const void *src) {
   int *value;
   CString *string;

   if (dest_type == &ATTRIBUTE:type<int> && src_type == NULL) {
      memset(dest, 0, sizeof(int));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<int>) {
      *((int *)dest) = *((int *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<int>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (int *)dest;
      string = (CString *)src;
      *value = atoi(CString(string).string());
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<int>) {
      value  = (int *)src;
      string = (CString *)dest;
      CString(string).printf("%d", *value);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<op_compare> && src_type == &ATTRIBUTE:type<int>) {
      if (*((int *)dest) == *((int *)src)) *((int *)object) = 0;
      else if (*((int *)dest) < *((int *)src)) *((int *)object) = -1;
      else *((int *)object) = 1;
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<bool>(CObjPersistent *object,
                             const TAttributeType *dest_type, const TAttributeType *src_type,
                             int dest_index, int src_index,
                             void *dest, const void *src) {
   bool *value;
   CString *string;

   if (dest_type == &ATTRIBUTE:type<bool> && src_type == NULL) {
      memset(dest, 0, sizeof(bool));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<bool> && src_type == &ATTRIBUTE:type<bool>) {
      *((bool *)dest) = *((bool *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<bool>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<bool> && src_type == &ATTRIBUTE:type<int>) {
      *((bool *)dest) = *((int *)src);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<bool>) {
      *((int *)dest) = *((bool *)src);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<bool> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (bool *)dest;
      string = (CString *)src;
      switch (toupper(CString(string).string()[0])) {
      case '1':
      case 'T':
         *value = TRUE;
         break;
      case 'I':
         *value = !*value;
         break;
      case '0':
      case 'F':
         *value = FALSE;
         break;
      default:
         return FALSE;
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<bool>) {
      value  = (bool *)src;
      string = (CString *)dest;
      CString(string).set(*value ? "TRUE" : "FALSE");
      return TRUE;
   }
   return FALSE;
}

bool ATTRIBUTE:convert<float>(CObjPersistent *object,
                              const TAttributeType *dest_type, const TAttributeType *src_type,
                              int dest_index, int src_index,
                              void *dest, const void *src) {
   float *value;
   CString *string;


   if (dest_type == &ATTRIBUTE:type<float> && src_type == NULL) {
      memset(dest, 0, sizeof(float));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<float> && src_type == &ATTRIBUTE:type<float>) {
      *((float *)dest) = (float)*((float *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<float>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<float> && src_type == &ATTRIBUTE:type<int>) {
      *((float *)dest) = (float)*((int *)src);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<float>) {
      *((int *)dest) = (int)*((float *)src);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<float> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (float *)dest;
      string = (CString *)src;
      *value = (float)atof(CString(string).string());
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<float>) {
      value  = (float *)src;
      string = (CString *)dest;
      CString(string).printf("%g", *value);
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<double>(CObjPersistent *object,
                              const TAttributeType *dest_type, const TAttributeType *src_type,
                              int dest_index, int src_index,
                              void *dest, const void *src) {
   double *value;
   CString *string;

   if (dest_type == &ATTRIBUTE:type<double> && src_type == NULL) {
      memset(dest, 0, sizeof(double));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<double> && src_type == &ATTRIBUTE:type<double>) {
      *((double *)dest) = (double)*((double *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<double>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<double> && src_type == &ATTRIBUTE:type<int>) {
      *((double *)dest) = (double)*((int *)src);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<double>) {
      *((int *)dest) = (int)*((double *)src);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<double> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (double *)dest;
      string = (CString *)src;
      *value = (double)atof(CString(string).string());
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<double>) {
      value  = (double *)src;
      string = (CString *)dest;
      CString(string).printf("%g", *value);
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<short>(CObjPersistent *object,
                              const TAttributeType *dest_type, const TAttributeType *src_type,
                              int dest_index, int src_index,
                              void *dest, const void *src) {
   short *value;
   int int_value;
   CString *string;


   if (dest_type == &ATTRIBUTE:type<short> && src_type == NULL) {
      memset(dest, 0, sizeof(short));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<short> && src_type == &ATTRIBUTE:type<short>) {
      *((short *)dest) = *((short *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<short>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<short> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (short *)dest;
      string = (CString *)src;
      *value = (short)atoi(CString(string).string());
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<short>) {
      value  = (short *)src;
      string = (CString *)dest;
      int_value = (int) *value;
      CString(string).printf("%d", int_value);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<short> && src_type == &ATTRIBUTE:type<int>) {
      *(short *)dest = (short) (*(int *)src);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<short>) {
      *(int *)dest = (int) (*(short *)src);
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<word>(CObjPersistent *object,
                             const TAttributeType *dest_type, const TAttributeType *src_type,
                             int dest_index, int src_index,
                             void *dest, const void *src) {
   word *value;
   int int_value;
   CString *string;

   if (dest_type == &ATTRIBUTE:type<word> && src_type == NULL) {
      memset(dest, 0, sizeof(word));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<word> && src_type == &ATTRIBUTE:type<word>) {
      *((word *)dest) = *((word *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<word>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<word> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (word *)dest;
      string = (CString *)src;
      *value = (word)atoi(CString(string).string());
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<word>) {
      value  = (word *)src;
      string = (CString *)dest;
      int_value = (int) *value;
      CString(string).printf("%d", int_value);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<word> && src_type == &ATTRIBUTE:type<int>) {
      *(word *)dest = (word) (*(int *)src);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<word>) {
      *(int *)dest = (int) (*(word *)src);
      return TRUE;
   }

   return FALSE;
}


bool ATTRIBUTE:convert<byte>(CObjPersistent *object,
                             const TAttributeType *dest_type, const TAttributeType *src_type,
                             int dest_index, int src_index,
                             void *dest, const void *src) {
   byte *value;
   int int_value;
   CString *string;

   if (dest_type == &ATTRIBUTE:type<byte> && src_type == NULL) {
      memset(dest, 0, sizeof(byte));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<byte> && src_type == &ATTRIBUTE:type<byte>) {
      *((byte *)dest) = *((byte *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<byte>) {
      memset(dest, 0, sizeof(byte));
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<byte> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (byte *)dest;
      string = (CString *)src;
      *value = (byte)atoi(CString(string).string());
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<byte>) {
      value  = (byte *)src;
      string = (CString *)dest;
      int_value = (int) *value;
      CString(string).printf("%d", int_value);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<byte> && src_type == &ATTRIBUTE:type<int>) {
      *(byte *)dest = (byte) (*(int *)src);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<byte>) {
      *(int *)dest = (int) (*(byte *)src);
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<char>(CObjPersistent *object,
                              const TAttributeType *dest_type, const TAttributeType *src_type,
                              int dest_index, int src_index,
                              void *dest, const void *src) {
   char *value;
   CString *string;

   if (dest_type == &ATTRIBUTE:type<char> && src_type == NULL) {
      memset(dest, 0, sizeof(char));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<char> && src_type == &ATTRIBUTE:type<char>) {
      *((char *)dest) = *((char *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<char>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<char> && src_type == &ATTRIBUTE:type<int>) {
      *((char *)dest) = (char)*((int *)src);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<char>) {
      *((int *)dest) = *((char *)src);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<char> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (char *)dest;
      string = (CString *)src;
      if (CString(string).length()) {
          *value = CString(string).string()[0];
      }
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<char>) {
      value  = (char *)src;
      string = (CString *)dest;
      CString(string).printf("%c", *value);
      return TRUE;
   }
   return FALSE;
}

bool ATTRIBUTE:convert<CString>(CObjPersistent *object,
                                const TAttributeType *dest_type, const TAttributeType *src_type,
                               int dest_index, int src_index,
                                void *dest, const void *src) {
   if (dest_type == &ATTRIBUTE:type<CString> && src_type == NULL) {
      memset(dest, 0, sizeof(CString));
      new(dest).CString(NULL);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<CString>) {
      CString(dest).set_string((CString *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CString> ) {
      delete(src);
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<TBitfield>(CObjPersistent *object,
                                  const TAttributeType *dest_type, const TAttributeType *src_type,
                                  int dest_index, int src_index,
                                  void *dest, const void *src) {
   TBitfield *bitfield;
   CString *string;
   bool *value, new_value;
   int i;

   if (dest_type == &ATTRIBUTE:type<TBitfield> && src_type == NULL) {
      bitfield = (TBitfield *)dest;
      BITFIELD(bitfield).new();
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TBitfield> && src_type == &ATTRIBUTE:type<TBitfield>) {
      memcpy(dest, src, sizeof(TBitfield));
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TBitfield>) {
      bitfield = (TBitfield *)src;
      BITFIELD(bitfield).delete();
      return TRUE;
   }

   if ((dest_type == &ATTRIBUTE:type<bool> || dest_type == &ATTRIBUTE:type<int>) &&
       src_type == &ATTRIBUTE:type<TBitfield>) {
      bitfield = (TBitfield *)src;
      value    = (bool *)dest;

      *value = BITFIELD(bitfield).get(src_index);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TBitfield> &&
       (src_type == &ATTRIBUTE:type<bool> || src_type == &ATTRIBUTE:type<int>)) {
      value    = (bool *)src;
      bitfield = (TBitfield *)dest;

      BITFIELD(bitfield).set(dest_index, *value);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TBitfield> && src_type == &ATTRIBUTE:type<CString>) {
      bitfield = (TBitfield *)dest;
      string   = (CString *)src;

      if (dest_index == -1) {
         BITFIELD(bitfield).count_set(CString(string).length());
         for (i = 0; i < CString(string).length(); i++) {
            BITFIELD(bitfield).set(BITFIELD(bitfield).count() - i - 1, CString(string).string()[i] == '#');
         }
      }
      else {
         new_value = BITFIELD(bitfield).get(dest_index);
         switch (toupper(CString(string).string()[0])) {
         case 'T':
            new_value = TRUE;
            break;
         case 'I':
            new_value = !new_value;
            break;
         case 'F':
            new_value = FALSE;
            break;
         default:
            return FALSE;
         }
         BITFIELD(bitfield).set(dest_index, new_value);
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TBitfield>) {
      bitfield = (TBitfield *)src;
      string   = (CString *)dest;
      CString(string).clear();
      for (i = 0; i < BITFIELD(bitfield).count(); i++) {
         CString(string).printf_append(BITFIELD(bitfield).get(BITFIELD(bitfield).count() - i - 1) ? "#" : ".");
      }
      return TRUE;
   }
   return FALSE;
}

bool ATTRIBUTE:convert<TClassPtr>(CObjPersistent *object,
                                  const TAttributeType *dest_type, const TAttributeType *src_type,
                                  int dest_index, int src_index,
                                  void *dest, const void *src) {
   TClassPtr *value;
   CString *string;

   if (dest_type == &ATTRIBUTE:type<TClassPtr> && src_type == NULL) {
      memset(dest, 0, sizeof(TClassPtr));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TClassPtr> && src_type == &ATTRIBUTE:type<TClassPtr>) {
      *((TClassPtr *)dest) = *((TClassPtr *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TClassPtr>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TClassPtr> && src_type == &ATTRIBUTE:type<CString>) {
      value = (TClassPtr *)dest;
      string = (CString *)src;
      *value = CFramework(&framework).obj_class_find(CString(string).string());
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TClassPtr>) {
      value  = (TClassPtr *)src;
      string = (CString *)dest;
      if (!(*value)) {
         CString(string).printf("(none)");
      }
      else {
         CString(string).printf("%s", CObjClass_alias(*value));
      }
      return TRUE;
   }
   return FALSE;
}

void CBinData::new(void) {
                                                                               
   (&ATTRIBUTE:type<CBinData>)->no_expand = TRUE;

   new(&this->header).CString("base64");
   ARRAY(&this->data).new();
}                 

void CBinData::CBinData(void) {
}                      

void CBinData::delete(void) {
   ARRAY(&this->data).delete();
   delete(&this->header);
}                    

bool ATTRIBUTE:convert<CBinData>(CObjPersistent *object,
                                  const TAttributeType *dest_type, const TAttributeType *src_type,
                                  int dest_index, int src_index,
                                  void *dest, const void *src) {
   CBinData *bindata;
   CString *string;
   const char *cp;

   int codelength;
   int plainlength;
   base64_decodestate dec_state;
   base64_encodestate enc_state;

   if (dest_type == &ATTRIBUTE:type<CBinData> && src_type == NULL) {
      return FALSE;
   }
   if (dest_type == &ATTRIBUTE:type<CBinData> && src_type == &ATTRIBUTE:type<CBinData>) {
      return FALSE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CBinData>) {
      return FALSE;
   }

   if (dest_type == &ATTRIBUTE:type<CBinData> && src_type == &ATTRIBUTE:type<CString>) {
      bindata = (CBinData *)dest;
      string = (CString *)src;

      cp = CString(string).strchr(',');
      if (cp) {
         CString(string).extract(&bindata->header, CString(string).string(), cp - 1);
         cp++;
      }
      else {
                                                     
         CString(&bindata->header).set(CString(string).string());
         return TRUE;
      }

      base64_init_decodestate(&dec_state);
      ARRAY(&bindata->data).used_set(CString(string).length());
      plainlength = base64_decode_block(cp, strlen(cp), (char *)ARRAY(&bindata->data).data(), &dec_state);
      ARRAY(&bindata->data).used_set(plainlength);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<CBinData>) {
      bindata = (CBinData *)src;
      string = (CString *)dest;

      CString(string).clear();
      if (CString(&bindata->header).length()) {

          CString(string).set_string(&bindata->header);
      }
      if (ARRAY(&bindata->data).count() && src_index == -1) {
         CString(string).printf_append(",\n");
         plainlength = CString(string).length();
         base64_init_encodestate(&enc_state);
         ARRAY(&string->data).used_set(plainlength + ARRAY(&bindata->data).count() * 2);
         codelength = base64_encode_block((char *)ARRAY(&bindata->data).data(), ARRAY(&bindata->data).count(), ARRAY(&string->data).data() + plainlength, &enc_state);
         codelength += base64_encode_blockend((char *)ARRAY(&string->data).data() + codelength + plainlength, &enc_state);
         ARRAY(&string->data).data()[codelength + plainlength] = '\0';
         codelength++;
         ARRAY(&string->data).used_set(codelength + plainlength);
      }
      return TRUE;
   }
   return FALSE;
}

void CObjPersistent::new(void) {

   int i, count;


   count = CObjPersistent(this).attribute_list(NULL, TRUE, TRUE, FALSE);

   BITFIELD(&this->attribute_default).count_set(count);
   for (i = 0; i < count; i++) {
      BITFIELD(&this->attribute_default).set(i, FALSE);
   }

}                       

void CObjPersistent::CObjPersistent(void) {
}                                  

void CObjPersistent::delete(void) {
}                          

CObjPersistent *CObjPersistent::child_first(void) {
   return CObjPersistent(this).child_tree_next(this);
}                               

CObjPersistent *CObjPersistent::child_last(void) {
   return NULL;
}                              

CObjPersistent *CObjPersistent::child_next(CObjPersistent *child) {
   int i;
   ARRAY<const TAttribute *> attribute;
   CObjPersistent *next = NULL;
   
   ARRAY(&attribute).new();

   CObjPersistent(this).attribute_list(&attribute, FALSE, TRUE, FALSE);   
   for (i = 0; i < ARRAY(&attribute).count(); i++) {
      if (child == (CObjPersistent *)(((byte *)this) + ARRAY(&attribute).data()[i]->offset)) {
         if (i < ARRAY(&attribute).count() - 1) {
            next = (CObjPersistent *)(((byte *)this) + ARRAY(&attribute).data()[i + 1]->offset);
         }
         else {
            next = (CObjPersistent *)CObject(this).child_first();
         }
      }
   }
   
   if (!next) {
      next = (CObjPersistent *)CObject(this).child_next(CObject(child));
   }
   
   ARRAY(&attribute).delete();   
   
   if (!CObject_exists((CObject *)next)) {
      next = NULL;
   }
   
   return next;
}                              

CObjPersistent *CObjPersistent::child_previous(CObjPersistent *child) {
   return NULL;
}                                  

CObjPersistent *CObjPersistent::child_tree_first(void) {
   return CObjPersistent(this).child_tree_next(this);
}                                    

CObjPersistent *CObjPersistent::child_tree_last(void) {
   return NULL;
}                                   

CObjPersistent *CObjPersistent::child_tree_next(CObjPersistent *child) {
   ARRAY<const TAttribute *> attribute;
   CObjPersistent *next = NULL;
   
   ARRAY(&attribute).new();
   CObjPersistent(child).attribute_list(&attribute, FALSE, TRUE, FALSE);
   if (ARRAY(&attribute).count()) {
      next = CObjPersistent((((byte *)child) + ARRAY(&attribute).data()[0]->offset));
   }
   if (!next) {
      next = CObjPersistent(CObject(child).child_first());
   }
   
   while (!next) {
      if (!CObject(child).parent() || child == this) {
         break; 
      }
      next = CObjPersistent(CObject(child).parent()).child_next(child);
      child = CObjPersistent(CObject(child).parent());       
   }       
  
   ARRAY(&attribute).delete();   
   
   return next;
}                                   

CObjPersistent *CObjPersistent::child_tree_previous(CObjPersistent *child) {
   return NULL;
}                                       

int state_loading;

bool CObjPersistent::state_loading(void) {
                               
   return CObjPersistent(this).state_xml_loading() || (state_loading != 0);
}                                 

CObjPersistent *CObjPersistent::copy_fast(CObjPersistent *dest, bool copy_child, bool copy_fast) {
                                            
   const TAttribute *attribute;
   ARRAY<const TAttribute *> attr_list;
   void *attr_src, *attr_dest;
   int i, j;
   bool result;
   const CObjClass *obj_class = CObject(this).obj_class();
   CObjPersistent *child, *child_copy, *object_copy;
   ARRAY<byte> *array_src, *array_dest;
   CString data;
   
   if (dest) {
      if (CObject(this).obj_class() != CObject(dest).obj_class()) {
         ASSERT("CObjPersistent::copy_fast():source and destination classes do not match");
      }
      object_copy = dest;
   }
   else {
      object_copy = CObjPersistent(new.class(obj_class));
   }

   CObjPersistent(object_copy).CObjPersistent();

                               
   ARRAY(&attr_list).new();
   CObjPersistent(this).attribute_list(&attr_list, TRUE, TRUE, FALSE);
   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      attribute = ARRAY(&attr_list).data()[i];
      attr_src  = (void *)(((byte *)this) + attribute->offset);
      attr_dest = (void *)(((byte *)object_copy) + attribute->offset);

      switch (attribute->type) {
      case PT_AttributeArray:
                                                       
         array_src = (ARRAY<byte> *)attr_src;
         array_dest = (ARRAY<byte> *)attr_dest;

         ARRAY(array_dest).used_set(ARRAY(array_src).count());
         for (j = 0; j < ARRAY(array_src).count(); j++) {
            attr_src = (void *)&ARRAY(array_src).data()[attribute->data_type->size * j];
            attr_dest = (void *)&ARRAY(array_dest).data()[attribute->data_type->size * j];

                                               
            result = (*attribute->data_type->convert)(object_copy, attribute->data_type, attribute->data_type,
                                                      ATTR_INDEX_VALUE, ATTR_INDEX_VALUE,
                                                      attr_dest, attr_src);
            if (!result) {
                                                              
                                               
               memcpy(attr_dest, attr_src, attribute->data_type->size);
            }
         }
         break;
      case PT_AttributeEnum:
         memcpy(attr_dest, attr_src, sizeof(int));
         break;
      case PT_ElementObject:
                                
         CObjPersistent(attr_src).copy_fast(CObjPersistent(attr_dest), FALSE, copy_fast);

                                   
         child = CObjPersistent(CObject(attr_src).child_first());
         while (child) {
            child_copy = CObjPersistent(child).copy_fast(NULL, TRUE, copy_fast);
            CObject(attr_dest).child_add(CObject(child_copy));
            child = CObjPersistent(CObject(attr_src).child_next(CObject(child)));
         }
         break;
      default:
                                            
         result = (*attribute->data_type->convert)(object_copy, attribute->data_type, attribute->data_type,
                                                   ATTR_INDEX_VALUE, ATTR_INDEX_VALUE,
                                                   attr_dest, attr_src);
         if (!result) {
                                                                 
            result = CObjPersistent(this).attribute_default_get(attribute);
            CObjPersistent(object_copy).attribute_default_set(attribute, result);

            new(&data).CString(NULL);
            CObjPersistent(this).attribute_get_string(attribute, &data);
            CObjPersistent(object_copy).attribute_set_string(attribute, &data);
            delete(&data);
         }
         break;
      }          

                                        
      if (copy_fast) {
         BITFIELD(&object_copy->attribute_default).set(attribute->index, BITFIELD(&this->attribute_default).get(attribute->index));
      }
      else {
         result = CObjPersistent(this).attribute_default_get(attribute);
         CObjPersistent(object_copy).attribute_default_set(attribute, result);
      }
   }
   ARRAY(&attr_list).delete();

                                      
   if (copy_child                                          ) {

                                 
      child = CObjPersistent(CObject(this).child_first());
      while (child) {
         child_copy = CObjPersistent(child).copy_fast(NULL, TRUE, copy_fast);
         CObject(object_copy).child_add(CObject(child_copy));
                                         
         if (!copy_fast) {
            CObjPersistent(child_copy).attribute_refresh(FALSE);
         }
         child = CObjPersistent(CObject(this).child_next(CObject(child)));
      }
   }
   
   return object_copy;
}                             

CObjPersistent *CObjPersistent::copy(bool copy_child) {
   return CObjPersistent(this).copy_fast(NULL, copy_child, FALSE);
}                        

CObjPersistent *CObjPersistent::copy_clone(CObjPersistent *dest, bool copy_child) {
   CObjPersistent *result;

   state_loading++;
   result = CObjPersistent(this).copy_fast(dest, copy_child, FALSE);
   state_loading--;   
   
   return result;
}                              

void CObjPersistent::attribute_update(const TAttribute *attribute) {
                                           

   CObjPersistent(this).notify_attribute_update(NULL, TRUE);
}                                    

void CObjPersistent::attribute_update_end(void) {
   CObjPersistent(this).notify_attribute_update(NULL, FALSE);
}                                        

void CObjPersistent::attribute_refresh(bool refresh_child) {
   ARRAY<const TAttribute *> attr_list;
   const TAttribute *attribute;
   int i;
   CObjPersistent *child;

   ARRAY(&attr_list).new();

   CObjPersistent(this).attribute_list(&attr_list, TRUE, FALSE, FALSE);

   CObjPersistent(this).attribute_update(NULL);
   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      attribute = ARRAY(&attr_list).data()[i];
      if (CObjPersistent(this).attribute_default_get(attribute)) {
         CObjPersistent(this).attribute_default_set(attribute, TRUE);
      }
   }
   CObjPersistent(this).attribute_update_end();

   ARRAY(&attr_list).delete();

   if (refresh_child) {
       child = CObjPersistent(CObject(this).child_first());
       while (child) {
           CObjPersistent(child).attribute_refresh(TRUE);
           child = CObjPersistent(CObject(this).child_next(CObject(child)));
       }
   }
}                                     

int CObjPersistent::attribute_array_count(const TAttribute *attribute) {
   ARRAY<byte> *array;

   if (attribute->type != PT_AttributeArray) {
      throw(CObject(this), EXCEPTION<CObjPersistent,AttributeTypeMismatch>, "Attribute Type Mismatch");
   }

   array = (ARRAY<byte> *)(((byte *)this) + attribute->offset);

   return ARRAY(array).count();
}                                         

bool CObjPersistent::attribute_default_get(const TAttribute *attribute) {
   int i = CObjPersistent(this).attribute_index_find(attribute);

   return BITFIELD(&this->attribute_default).get(i);
}                                         

void CObjPersistent::attribute_default_set(const TAttribute *attribute, bool is_default) {
   int i = CObjPersistent(this).attribute_index_find(attribute);
   bool set_default = TRUE;

   BITFIELD(&this->attribute_default).set(i, is_default);

   if (is_default) {
                                     
      if (attribute->options & PO_INHERIT) {
         set_default = !CObjPersistent(this).attribute_set_inherit(attribute);
      }
      if (attribute && attribute->method && set_default) {
         (*attribute->method)(this, NULL, -1);
      }
   }
}                                         

void CObjPersistent::attribute_default_refresh_all(CObjPersistent *object, const TAttribute *attribute) {
   CObjPersistent *child;
   const TAttribute *attr_object;

   if (object != this) {
      attr_object = CObjPersistent(object).attribute_find(attribute->name);
      if (attr_object && CObjPersistent(object).attribute_default_get(attr_object)) {
         CObjPersistent(object).attribute_update(attr_object);
         CObjPersistent(object).attribute_default_set(attr_object, TRUE);
         CObjPersistent(object).attribute_update_end();
      };
   }

                         
   child = CObjPersistent(CObject(object).child_first());
   while (child) {
      CObjPersistent(this).attribute_default_refresh_all(child, attribute);
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }
}                                                 

void CObjPersistent::attribute_string_process(const TAttribute *attribute, CString *data) {
   int index;
   int level = 0;
   int delim_count = 0;
   int length;

   if (attribute->options & PO_ELEMENT_STRIPSPACES) {
                                                                                   
      index = 0;
      length = 0;
      while (index < CString(data).length()) {
         switch (CString(data).string()[index]) {
         case '(':
            level++;
            break;
         case ')':
            level--;
            break;
         case ' ':
            if (level || length) {
               CString(data).erase(index, 1);
               continue;
            }
            length++;
            break;
         default:
            length = 0;
            break;
         }
         index++;
      }
   }

   if (attribute->options & PO_ELEMENT_PAIR) {
                                            
      index = 0;
      while (index < CString(data).length()) {
         if (CString(data).string()[index] == ' ' || CString(data).string()[index] == ',') {
            delim_count++;
            CString(data).string()[index] = delim_count & 1 ? ',' : ' ';
         }
         index++;
      }
   }

}                                            

void CObjPersistent::attribute_set(const TAttribute *attribute, int attribute_element, int attribute_index, const TAttributeType *type, const void *data) {
   bool result;
   ARRAY<byte> *array, *array_src;
   CString element;
   int i;
   byte *data_set;

   data_set = CFramework(&framework).scratch_buffer_allocate();

   switch (attribute->type) {
   case PT_Attribute:
   case PT_Element:
   case PT_Data:
      if (attribute->method) {
         if (attribute->data_type == type) {
            (*attribute->method)(this, (void *)data, -1);
            result = TRUE;
         }
         else {
                                           
                                                                                   
            result = (*attribute->data_type->convert)(this, attribute->data_type, NULL,
                                                      -1, -1,
                                                      data_set, NULL);
            if (!result) {
               ASSERT("Conversion failed");
            }
            result = (*attribute->data_type->convert)(this, attribute->data_type, attribute->data_type,
                                                      -1, -1,
                                                      data_set, (void *)(((byte *)this) + attribute->offset));
            if (!result) {
               ASSERT("Conversion failed");
            }
            result = (*attribute->data_type->convert)(this, attribute->data_type, type,
                                                      attribute_index, ATTR_INDEX_VALUE,
                                                      data_set, data);
            if (result) {
               (*attribute->method)(this, data_set, -1);
            }

            result = (*attribute->data_type->convert)(this, NULL, attribute->data_type,
                                                      -1, -1,
                                                      NULL, data_set);
            if (!result) {
               ASSERT("Conversion failed");
            }
         }
      }
      else {
         result = (*attribute->data_type->convert)(this, attribute->data_type, type,
                                                   attribute_index, ATTR_INDEX_VALUE,
                                                   (void *)(((byte *)this) + attribute->offset), data);
         if (!result) {
            result = (*type->convert)(this, attribute->data_type, type,
                                      attribute_index, ATTR_INDEX_VALUE,
                                      (void *)(((byte *)this) + attribute->offset), data);
         }

         if (!result && attribute->data_type == type) {
                                    
            memcpy((void *)(((byte *)this) + attribute->offset), data, type->size);
            result = TRUE;
         }
      }
      break;
   case PT_AttributeArray:
      array = (ARRAY<byte> *)(((byte *)this) + attribute->offset);
      if (attribute_element != -1) {
         if (ARRAY(array).count() <= attribute_element) {
            ARRAY(array).used_set(attribute_element + 1);
         }

                                        
         memcpy(data_set, (void *)&ARRAY(array).data()[attribute->data_type->size * attribute_element], attribute->data_type->size);

         result =
            (*attribute->data_type->convert)(this, attribute->data_type, type,
                                             attribute_index, ATTR_INDEX_VALUE,
                                             data_set,
                                             data);
         if (!result && attribute->data_type == type) {
                                    
            memcpy(data_set, data, type->size);
            result = TRUE;
         }

         if (attribute->method) {
            (*attribute->method)(this, (void *)data_set, attribute_element);
         }
         else {
            memcpy((void *)&ARRAY(array).data()[attribute->data_type->size * attribute_element], data_set, attribute->data_type->size);
         }
      }
      else {
                                               
         if (type == &ATTRIBUTE:type<CString>) {
                                                    
             result =
                (*attribute->data_type->convert)(this,
                                                 attribute->data_type->array, &ATTRIBUTE:type<CString>,
                                                 attribute_index, ATTR_INDEX_VALUE,
                                                 (void *)array,
                                                 (void *)data);
            if (!result) {
               new(&element).CString(NULL);
               ARRAY(array).used_set(0);

                                       
               while (*CString(data).string() == ' ') {
                  CString(data).printf("%s", &CString(data).string()[1]);
               }
               CObjPersistent(this).attribute_string_process(attribute, CString(data));

               i = 0;
               while (CString(data).tokenise(&element, attribute->delim ? attribute->delim : " ",
                                             !(attribute->options & PO_NODELIMSTRIP))) {
                  ARRAY(array).used_set(ARRAY(array).count() + 1);
                  result =
                     (*attribute->data_type->convert)(this,
                                                      attribute->data_type, type,
                                                      attribute_index, ATTR_INDEX_VALUE,
                                                      (void *)&ARRAY(array).data()[attribute->data_type->size * i],
                                                      (void *)&element);
                  if (!result) {
                     break;
                  }
                  i++;
               }
               delete(&element);
            }
         }
         else {
            if (attribute->data_type == type) {
               array_src = (ARRAY<byte> *)((byte *)data);
               ARRAY(array).used_set(ARRAY(array_src).count());
               memcpy(ARRAY(array).data(), ARRAY(array_src).data(), ARRAY(array).count() * type->size);
            }
            else {
               throw(CObject(this), EXCEPTION<CObjPersistent,BadConversion>, "Unsupported Array Set Conversion");
            }
         }
      }
      break;
   case PT_AttributeEnum:
      i = 0;
      result = FALSE;
      if (type == &ATTRIBUTE:type<CString>) {
         i = ENUM_decode(attribute->enum_count, attribute->enum_name, CString(data).string());
         if (i >= 0) {
            result = TRUE;
         }
      }
      else if (type == &ATTRIBUTE:type<int> ||
               type == &ATTRIBUTE:type<bool>) {
         i = *(int *)data;
         result = TRUE;
      }

      if (result) {
         if (attribute->method) {
            (*attribute->method)(this, &i, -1);
         }
         else {
           *((int *)(((byte *)this) + attribute->offset)) = i;
         }
      }
      break;
   default:
     result = FALSE;
   }

   CFramework(&framework).scratch_buffer_release(data_set);

#if 0
   if (!result) {
      throw(CObject(this), EXCEPTION<CObjPersistent,BadConversion>, "Bad Set Conversion");
   }
#endif

   if (attribute->options & PO_INHERIT) {
                               
      CObjPersistent(this).attribute_default_refresh_all(this, attribute);
   }
}                                 

bool CObjPersistent::attribute_set_inherit(const TAttribute *attribute) {
   CObjPersistent *parent;
   const TAttribute *attr_parent;

   parent = CObjPersistent(CObject(this).parent());
   attr_parent = NULL;
   while (parent) {
      attr_parent = CObjPersistent(parent).attribute_find(attribute->name);
      if (attr_parent && !CObjPersistent(parent).attribute_default_get(attr_parent)) {
                                                                                
         CObjPersistent(this).attribute_set(attribute, -1, -1, attr_parent->data_type, (void *)(((byte *)parent) + attr_parent->offset));
         return TRUE;
      }
      parent = CObjPersistent(CObject(parent).parent());
   }

   return FALSE;
}                                         

void CObjPersistent::attribute_get(const TAttribute *attribute, int attribute_element, int attribute_index, const TAttributeType *type, void *data) {
   bool result;
   CString element, *result_data;
   ARRAY<byte> *array;
   void *src;
   int i;

   switch (attribute->type) {
   case PT_Attribute:
   case PT_Element:
   case PT_Data:
      result = (*attribute->data_type->convert)(this, type, attribute->data_type,
                                                ATTR_INDEX_VALUE, attribute_index,
                                                data, (void *)(((byte *)this) + attribute->offset));
      if (!result) {
         result = (*type->convert)(this, type, attribute->data_type,
                                   ATTR_INDEX_VALUE, attribute_index,
                                   data, (void *)(((byte *)this) + attribute->offset));
      }
      if (!result && attribute->data_type == type) {
                                 
         memcpy(data, (void *)(((byte *)this) + attribute->offset), type->size);
         result = TRUE;
      }
      break;
   case PT_AttributeArray:
      array = (ARRAY<byte> *)(((byte *)this) + attribute->offset);
      result_data = (CString *)data;

      if (type == &ATTRIBUTE:type<CString> && attribute_element == ATTR_ELEMENT_VALUE && attribute_index == ATTR_INDEX_VALUE) {
         new(&element).CString(NULL);

         CString(result_data).clear();
         result = TRUE;
         for (i = 0; i < ARRAY(array).count(); i++) {
            result =
               (*attribute->data_type->convert)(this, type, attribute->data_type,
                                                ATTR_INDEX_VALUE, attribute_index,
                                               (void *)&element,
                                               (void *)&ARRAY(array).data()[attribute->data_type->size * i]);
            if (i == 0) {
               CString(result_data).printf("%s", CString(&element).string());
            }
            else {
               CString(result_data).printf_append("%s%s",
                                                  attribute->delim ? attribute->delim : " ",
                                                  CString(&element).string());
            }
         }
         delete(&element);
      }
      else {
         if (attribute_element >= ARRAY(array).count()) {
                                                             
            i = 0;
            result = (*attribute->data_type->convert)(this, &ATTRIBUTE:type<int>, attribute->data_type,
                                                      ATTR_INDEX_VALUE, attribute_index,
                                                      data, (void *)&i);
            return;
         }

         if (attribute_element == ATTR_ELEMENT_VALUE) {
            src = array;
            result = (*attribute->data_type->array->convert)(this, type, attribute->data_type->array,
                                                             ATTR_INDEX_VALUE, attribute_index,
                                                             data, src);
         }
         else {
            src = ARRAY(array).data() + (attribute_element * attribute->data_type->size);
            result = (*attribute->data_type->convert)(this, type, attribute->data_type,
                                                   ATTR_INDEX_VALUE, attribute_index,
                                                   data, src);
         }
         if (!result && attribute->data_type == type) {
                                    
            memcpy(data, src, type->size);
            result = TRUE;
         }
      }
      break;
   case PT_AttributeEnum:
      result = FALSE;
      if (type == &ATTRIBUTE:type<CString>) {
         CString(data).printf("%s", attribute->enum_name[*((int *)(((byte *)this) + attribute->offset))]);
         result = TRUE;
      }
      if (type == &ATTRIBUTE:type<int>) {
         *((int *)data) = *((int *)(((byte *)this) + attribute->offset));
         result = TRUE;
      }
      break;
   default:
      result = FALSE;
   }
                
   if (!result) {
      throw(CObject(this), EXCEPTION<CObjPersistent,BadConversion>, "Bad Get Conversion");
   }
}                                 

void CObjPersistent::attribute_set_string(const TAttribute *attribute, const CString *string) {
   CObjPersistent(this).attribute_set(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<CString>, (const void *)string);
}                                        

void CObjPersistent::attribute_set_string_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, const CString *string) {
   CObjPersistent(this).attribute_set(attribute, attribute_element, attribute_index, &ATTRIBUTE:type<CString>, (const void *)string);
}                                                      

void CObjPersistent::attribute_get_string(const TAttribute *attribute, CString *string) {
   CString(string).clear();
   CObjPersistent(this).attribute_get(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<CString>, (void *)string);
}                                        

void CObjPersistent::attribute_get_string_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, CString *string) {
   CString(string).clear();
   CObjPersistent(this).attribute_get(attribute, attribute_element, attribute_index, &ATTRIBUTE:type<CString>, (void *)string);
}                                                      

void CObjPersistent::attribute_set_text(const TAttribute *attribute, const char *text) {
   CString string;

   new(&string).CString(text);
   CObjPersistent(this).attribute_set_string(attribute, &string);
   delete(&string);
}                                      

void CObjPersistent::attribute_set_text_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, const char *text) {
   CString string;

   new(&string).CString(text);
   CObjPersistent(this).attribute_set_string_element_index(attribute, attribute_element, attribute_index, &string);
   delete(&string);
}                                                    

int CObjPersistent::attribute_get_int(const TAttribute *attribute) {
   int value;
   CObjPersistent(this).attribute_get(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<int>, (void *)&value);
   return value;
}                                     

int CObjPersistent::attribute_get_int_element_index(const TAttribute *attribute, int attribute_element, int attribute_index) {
   int value;
   CObjPersistent(this).attribute_get(attribute, attribute_element, attribute_index, &ATTRIBUTE:type<int>, (void *)&value);
   return value;
}                                                   

float CObjPersistent::attribute_get_float(const TAttribute *attribute) {
   float value;
   CObjPersistent(this).attribute_get(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<float>, (void *)&value);
   return value;
}                                       

double CObjPersistent::attribute_get_double(const TAttribute *attribute) {
   double value;
   CObjPersistent(this).attribute_get(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<double>, (void *)&value);
   return value;
}                                        

void CObjPersistent::attribute_set_int(const TAttribute *attribute, int value) {
   CObjPersistent(this).attribute_set(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<int>, (void *)&value);
}                                     

void CObjPersistent::attribute_set_int_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, int value) {
   CObjPersistent(this).attribute_set(attribute, attribute_element, attribute_index, &ATTRIBUTE:type<int>, (void *)&value);
}                                                   

void CObjPersistent::attribute_set_float(const TAttribute *attribute, float value) {
   CObjPersistent(this).attribute_set(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<float>, (void *)&value);
}                                       

void CObjPersistent::attribute_set_double(const TAttribute *attribute, double value) {
   CObjPersistent(this).attribute_set(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<double>, (void *)&value);
}                                        

int CObjPersistent::attribute_list(ARRAY<const TAttribute *> *result,
                                    bool list_attribute, bool list_elements, bool list_menu) {
   const CObjClass *obj_class, *obj_class_current;
   const TParameter **parameter;
   int count = 0;
   
   if (result) {
      ARRAY(result).used_set(0);
   }

   obj_class = &class(CObjPersistent);
   obj_class_current = &class(CObjPersistent);
   while (obj_class) {
      if (obj_class->ptbl) {
         parameter = obj_class->ptbl;
         while (*parameter) {
            switch ((*parameter)->type) {
            case PT_Attribute:
            case PT_AttributeEnum:
            case PT_AttributeArray:
            case PT_Data:
            case PT_Element:            
               if (list_attribute) {
                  count++;
                  if (result) {
                     ARRAY(result).item_add(*parameter);
                  }
               }
               break;
            case PT_Menu:
               if (list_menu) {
                  count++;
                  if (result) {
                     ARRAY(result).item_add(*parameter);
                  }
               }
               break;
            case PT_ElementObject:                           
               if (list_elements) {
                  count++;
                  if (result) {
                     ARRAY(result).item_add(*parameter);
                  }
               }
               break;
            default:
               break;
            }
            parameter++;
         }
      }

      obj_class = CObject(this).obj_class();
      while (obj_class) {
         if (obj_class && obj_class->base == obj_class_current) {
            obj_class_current = obj_class;
            break;
         }
         obj_class = obj_class->base;
      }
   }
   return count;
}                                  

int CObjPersistent::attribute_index_find(const TAttribute *attribute) {
   int i = 0;
   const CObjClass *obj_class, *obj_class_current;
   const TParameter **parameter;

   if (attribute->index > 0) {
                                                   
       return attribute->index - 1;
   }

   obj_class = &class(CObjPersistent);
   obj_class_current = &class(CObjPersistent);
   while (obj_class) {
      if (obj_class->ptbl) {
         parameter = obj_class->ptbl;
         while (*parameter) {
            if (attribute == *parameter) {
               ((TAttribute *)attribute)->index = i + 1;
               return i;
            }
            i++;
            parameter++;
         }
      }

      obj_class = CObject(this).obj_class();
      while (obj_class) {
         if (obj_class && obj_class->base == obj_class_current) {
            obj_class_current = obj_class;
            break;
         }
         obj_class = obj_class->base;
      }
   }

   return ATTR_INDEX_VALUE;
}                                        

const TAttribute *CObjPersistent::attribute_find(const char *attribute_name) {
   const TAttribute **attribute;
   const CObjClass *obj_class = CObject(this).obj_class();

   while (obj_class) {
      if (obj_class->ptbl) {
         attribute = obj_class->ptbl;
         while (*attribute) {
            if (attribute_name) {
               if (strcmp((*attribute)->name, attribute_name) == 0) {
                  return (*attribute);
               }
            }
            else {
               if ((*attribute)->type == PT_Data) {
                  return (*attribute);
               }
            }
         attribute++;
         }
      }
      obj_class = obj_class->base;
   }
   return NULL;
}                                  

const TParameter *CObjPersistent::element_find(const char *element_name) {
   const TParameter **parameter;
   const CObjClass *obj_class = CObject(this).obj_class();
   const char *short_element_name;
   const char *short_parameter_name;

   while (obj_class) {
      if (obj_class->ptbl) {
         parameter = obj_class->ptbl;
         while (*parameter) {
            if (((*parameter)->type == PT_Element || (*parameter)->type == PT_ElementObject)
                && strcmp((*parameter)->name, element_name) == 0) {
               return (*parameter);
            }
         parameter++;
         }
      }
      obj_class = obj_class->base;
   }

                                                             
   short_element_name = strchr(element_name, ':');
   short_element_name = short_element_name ? short_element_name + 1 : element_name;
   if (short_element_name) {
      obj_class = CObject(this).obj_class();
      while (obj_class) {
         if (obj_class->ptbl) {
            parameter = obj_class->ptbl;
            while (*parameter) {
               if (((*parameter)->type == PT_Element || (*parameter)->type == PT_ElementObject)) {
                  short_parameter_name = strchr((*parameter)->name, ':');
                  if (short_parameter_name && strcmp(short_parameter_name + 1, short_element_name) == 0) {
                     return (*parameter);
                  }
               }
            parameter++;
            }
         }
         obj_class = obj_class->base;
      }
   }
   return NULL;
}                                

const TParameter *CObjPersistent::data_find(void) {
   const TParameter **parameter;
   const CObjClass *obj_class = CObject(this).obj_class();

   while (obj_class) {
      if (obj_class->ptbl) {
         parameter = obj_class->ptbl;
         while (*parameter) {
            if ((*parameter)->type == PT_Data) {
               return (*parameter);
            }
         parameter++;
         }
      }
      obj_class = obj_class->base;
   }
   return NULL;
}                             

bool CObjPersistent::child_alias_sibling_index(CObjPersistent *child, int *index) {
   bool result = FALSE, child_found = FALSE;
   CObjPersistent *object;

   *index = 0;
   object = CObjPersistent(CObject(this).child_first());
   while (object) {
      if (object == child) {
         child_found = TRUE;
      }
      if (strcmp((char *)CObjClass_alias(CObject(child).obj_class()),
                 (char *)CObjClass_alias(CObject(object).obj_class())) == 0) {
         if (!child_found) {
            (*index)++;
         }
         if (object != child) {
            result = TRUE;
         }
      }
      object = CObjPersistent(CObject(this).child_next(CObject(object)));
   }
   return result;
}                                             

CObjPersistent *CObjPersistent::child_alias_sibling_index_find(const char *alias, int index) {
   const TAttribute *attr;
   ARRAY<const TAttribute *> attribute;
   int i;
   CObjPersistent *result = NULL;
   const char *cp;

   ARRAY(&attribute).new();
   CObjPersistent(this).attribute_list(&attribute, TRUE, TRUE, FALSE);
   for (i = 0; i < ARRAY(&attribute).count(); i++) {
      attr = ARRAY(&attribute).data()[i];
      if (attr->type == PT_ElementObject) {
         cp = strchr(attr->name, ':');
         if (cp) {
            cp++;
         }
         if (strcmp(attr->name, alias) == 0 ||
             (cp && strcmp(cp, alias) == 0)) {
            if (index == 0) {
               result = CObjPersistent((((byte *)this) + attr->offset));
               break;
            }
            index--;
         }
      }
   }
   ARRAY(&attribute).delete();

   if (result) {
      return result;
   }

   result = CObjPersistent(CObject(this).child_first());

   while (result) {
      if (strcmp(CObjClass_alias(CObject(result).obj_class()), alias) == 0) {
         if (index == 0) {
            return result;
         }
         index--;
      }
      result = CObjPersistent(CObject(this).child_next(CObject(result)));
   }

   return NULL;
}                                                  

CObjPersistent *CObjPersistent::child_alias_sibling_match(const char *alias, const char *attr_name, const char *attr_value, int *index_result) {
   CObjPersistent *result = NULL;
   const TAttribute *attribute = NULL;
   CString value;
   
   *index_result = 0;

   result = CObjPersistent(CObject(this).child_first());
   if (result) {
      attribute = CObjPersistent(result).attribute_find(attr_name);                                             
   }
   if (!result || !attribute) {
      *index_result = -1;
      return NULL;
   }

   new(&value).CString(NULL);

   while (result) {
      if (strcmp(CObjClass_alias(CObject(result).obj_class()), alias) == 0) {
         CObjPersistent(result).attribute_get_string(attribute, &value);
         if (CString(&value).strcmp(attr_value) == 0) {
            break;
         }
         (*index_result)++;
      }
      result = CObjPersistent(CObject(this).child_next(CObject(result)));
   }
   
   delete(&value);
   
   if (!result) {
      *index_result = -1;
   }
   
   return result;
}                                             

CObjPersistent *CObjPersistent::child_element_class_find(CObjClass *obj_class, const TAttribute **attribute_result) {
   const TAttribute *attr;
   ARRAY<const TAttribute *> attribute;
   int i;
   CObjPersistent *object, *result = NULL;

   ARRAY(&attribute).new();
   CObjPersistent(this).attribute_list(&attribute, TRUE, TRUE, FALSE);
   for (i = 0; i < ARRAY(&attribute).count(); i++) {
      attr = ARRAY(&attribute).data()[i];
      if (attr->type == PT_ElementObject) {
         object = (CObjPersistent *)(((byte *)this) + attr->offset);
         if (CObject_exists((CObject *)object) &&
            CObjClass_is_derived(obj_class, CObject(object).obj_class())) {
            result = CObjPersistent((((byte *)this) + attr->offset));
            if (attribute_result) {
               *attribute_result = attr;
            }
         }
      }
   }
   ARRAY(&attribute).delete();

   return result;
}                                            

bool ATTRIBUTE:convert<CXLink>(CObjPersistent *object,
                               const TAttributeType *dest_type, const TAttributeType *src_type,
                               int dest_index, int src_index,
                               void *dest, const void *src) {
   CString *string;
   CXLink *x_link, *x_link_dest;

   if (dest_type == &ATTRIBUTE:type<CXLink> && src_type == NULL) {
      new(dest).CXLink(NULL, NULL);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CXLink> && src_type == &ATTRIBUTE:type<CXLink>) {
      x_link = (CXLink *)src;
      x_link_dest = (CXLink *)dest;

      x_link_dest->root = x_link->root;
      x_link_dest->object = x_link->object;

      CString(&x_link_dest->value).set(CString(&x_link->value).string());
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CXLink>) {
      delete(src);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<CXLink>) {
      string = (CString *)dest;
      x_link = (CXLink *)src;
      CXLink(x_link).link_get(string);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CXLink> && src_type == &ATTRIBUTE:type<CString>) {
      x_link = (CXLink *)dest;
      string = (CString *)src;
      if (dest_index != -1) {
          CXLink(x_link).link_choose();
          return TRUE;
      }

                                         
      while (CObject(object).parent()) {
         object = CObjPersistent(CObject(object).parent());
      }
      CXLink(x_link).root_set(object);
      CXLink(x_link).link_set(CString(string).string());
      return TRUE;
   }

   return FALSE;
}

void CXLink::new(void) {
   new(&this->value).CString(NULL);
}               

void CXLink::CXLink(CObjPersistent *root, CObjPersistent *object) {
}                  

void CXLink::delete(void) {
   delete(&this->value);
}                  

CObjPersistent *CXLink::object_find(CObjPersistent *object, const char *name) {
   bool match;
   CObjPersistent *child, *result = NULL;
   const TAttribute *attribute;
   CString string;
   ARRAY<const TAttribute *> attr_list;
   int i;

   attribute = CObjPersistent(object).attribute_find("id");
   if (attribute) {
      new(&string).CString(NULL);
      CObjPersistent(object).attribute_get_string(attribute, &string);
      match = CString(&string).strcmp(name) == 0;
      delete(&string);

      if (match) {
         return object;
      }
   }

   ARRAY(&attr_list).new();
   CObjPersistent(object).attribute_list(&attr_list, TRUE, TRUE, FALSE);
   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      attribute = ARRAY(&attr_list).data()[i];
      if (attribute->type == PT_ElementObject) {
         child = CObjPersistent(&((byte *)object)[attribute->offset]);
         result = CXLink(this).object_find(child, name);
         if (result) {
            break;
         }
      }
   }
   ARRAY(&attr_list).delete();
   if (result) {
      return result;
   }

   child = CObjPersistent(CObject(object).child_first());
   while (child) {
      result = CXLink(this).object_find(child, name);
      if (result) {
         return result;
      }
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }

   return NULL;
}                       

CObjPersistent *CXLink::object_resolve(void) {

   const char *cp;


   if (!this->object) {
      this->object = NULL;
      cp = CString(&this->value).strchr('#');
      if (cp && this->root) {
         this->object = CXLink(this).object_find(this->root, cp + 1);
      }
   }

   return this->object;
}                          

void CXLink::link_set(const char *link) {
                                     
    const char *cp;

    cp = strrchr(link, '\\');
    if (!cp) {
       cp = strrchr(link, '/');
    }
    if (cp) {
        link = cp + 1;
    }

   CString(&this->value).printf("%s", link);

   CXLink(this).object_resolve();
}                    

void CXLink::link_get(CString *link) {
   CString(link).printf(CString(&this->value).string());
}                    

bool CXLink::link_get_file(CString *link) {
   const char *cp;

   if (!CString(&this->value).length()) {
      return FALSE;
   }

   cp = CString(&this->value).strchr('#');
   if (cp) {
      if (cp == &CString(&this->value).string()[0]) {
         return FALSE;
      }

      CString(link).setn(CString(&this->value).string(), cp - CString(&this->value).string());
   }
   else {
      CString(link).set(CString(&this->value).string());
   }
   return TRUE;
}                         

bool CXLink::link_get_path(CString *link) {
   return FALSE;
}                         

void CXPath::new(void) {
   class:base.new();

   new(&this->path).CString(NULL);
}               

void CXPath::CXPath(CObjPersistent *initial, CObjPersistent *root) {
   this->initial.object = initial;
   this->root = root;
   this->object.attr.element = ATTR_ELEMENT_VALUE;
   this->object.attr.index   = ATTR_INDEX_VALUE;

   CSelection(this).CSelection(TRUE);
}                  

void CXPath::delete(void) {
   delete(&this->path);

   class:base.delete();
}                  

void CXPath::initial_set_path(const char *path) {
   CString old_path;

   new(&old_path).CString(CString(&this->path).string());
   CString(&this->path).set(path);
   CXPath(this).resolve();
   this->initial = this->object;

   CString(&this->path).set(CString(&old_path).string());
   delete(&old_path);
}                            

void CXPath::set(CObjPersistent *object, const TAttribute *attribute, int index) {
   this->object.object = object;
   this->object.attr.attribute = attribute;
   this->object.attr.index = index;
}               

bool CXPath::match(CObjPersistent *object, const TAttribute *attribute, int index) {
   return this->object.object == object && this->object.attr.attribute == attribute && this->object.attr.index == index;
}                 

void CXPath::path_set(const char *path) {
   CString(&this->path).set(path);
   CXPath(this).resolve();
}                    

extern TParameter __parameter_CGText;
extern CObjClass class_CGText;

void CXPath::resolve(void) {
   CString temp;
   CString section;
   int array_index, depth = 0;
   char *index_start, *index_end;
   char *path_start, *path_end = NULL;
   bool root_search;
   char operation[20];

   this->object.object = NULL;
   this->object.attr.attribute = NULL;
   this->object.attr.element = ATTR_ELEMENT_VALUE;
   this->object.attr.index = ATTR_INDEX_VALUE;
   this->factor = 0;
   this->assignment = NULL;

   if (!this->root && !this->initial.object)
      return;

   if (!CString(&this->path).length())
      return;

   new(&temp).CString(CString(&this->path).string());
   new(&section).CString(NULL);

   this->operation = EPathOperation.none;
   path_start = CString(&temp).strchr('(');
   path_end = path_start;
   while (path_end && path_end != CString(&temp).string()) {
      if (*path_end == '=') {
         path_start = NULL;
      }
      path_end--;
   }
   if (path_start) {
      memset(operation, 0, sizeof(operation));
      strncpy(operation, CString(&temp).string(), path_start - CString(&temp).string());
      this->operation = ENUM:decode(EPathOperation, operation);

      path_end = CString(&temp).strchr(')');
      CString(&temp).setn(path_start + 1, path_end - path_start - 1);
   }

   this->object.object = this->initial.object;
   root_search = CString(&temp).string()[0] == '/';
   if (root_search) {
      CString(&temp).tokenise(&section, "/ ", TRUE);
   }

   this->assignment = NULL;
   this->assignment_end = NULL;
   
   while (CString(&temp).tokenise(&section, "/ ", TRUE)) {
                                
      index_start = CString(&section).strchr('=');
      if (index_start && (strchr(index_start, '[') || !strchr(index_start, ']'))) {
         CString(&section).setn(CString(&section).string(), index_start - CString(&section).string());
         this->assignment = CString(&this->path).strchr('=') + 1;
         this->assignment_end = this->assignment + strlen(this->assignment) - 1;
         if (path_start && *this->assignment_end == ')') {
            this->assignment_end--;
         }
      }

      array_index = -1;
      index_start = CString(&section).strchr('[');
      if (index_start) {
         index_end = CString(&section).strmatch(index_start);
         *index_start = '\0';
         index_start++;
         if (*index_start == '$') {
            array_index = this->index;
         }
         else if (*index_start == '@' && !this->assignment) {
            *index_end = '\0';
            index_end = strchr(index_start, '=');
            if (index_end) {
               *index_end = '\0';
               index_end++;
               CObjPersistent(this->object.object).child_alias_sibling_match(CString(&section).string(), index_start + 1, index_end, &array_index);               if (array_index == -1) {
                  this->object.object = NULL;
               }
            }
         }
         else {
            array_index = atoi(index_start);
         }
         CString(&section).setn(CString(&section).string(),
                                index_start - CString(&section).string() - 1);
      }

      if (depth == 0 && root_search) {
         this->object.object = this->root;
         if (!this->object.object) {
            delete(&section);
            delete(&temp);
            return;
         }
      }
      else if (this->object.object) {
         if (CString(&section).strcmp("..") == 0) {
            this->object.object = CObjPersistent(CObject(this->object.object).parent());
         }
         else if (CString(&section).string()[0] == '.' || this->operation == EPathOperation.string) {
            this->object = this->initial;
         }
         else if (CString(&section).string()[0] == '*') {
            this->factor = atoi(&CString(&section).string()[1]);
         }
         else if (CString(&section).string()[0] == '@') {
            this->object.attr.element = array_index;
            index_start = CString(&section).strchr('.');
            if (index_start) {
               index_start++;
               if (*index_start == '$') {
                  this->object.attr.index = this->index;
               }
               else {
                  this->object.attr.index = atoi(index_start);               
               }
               CString(&section).setn(CString(&section).string(),
                                      index_start - CString(&section).string() - 1);
            }
            this->object.attr.attribute = CObjPersistent(this->object.object).attribute_find(&CString(&section).string()[1]);
         }
         else {
            if (this->object.object) {
               this->object.object =
                  CObjPersistent(this->object.object).child_alias_sibling_index_find(CString(&section).string(),
                                                                                     array_index == -1 ? 0 : array_index);
            }
         }
      }
      depth++;
      if (!this->object.object) {
         break;
      }
   }

   if (this->object.object && !this->object.attr.attribute) {
      if (CObject(this->object.object).obj_class() == &class(CGText)) {
                                                                              
         this->object.attr.attribute = &__parameter_CGText;
      }
      else {
                                                               
         this->object.attr.attribute = CObjPersistent(this->object.object).attribute_find(NULL);
      }
   }

   delete(&section);
   delete(&temp);
}                   

void CXPath::path_get(CString *string) {
   CObjPersistent *object = this->object.object;
   CObjPersistent *root, *root_object = NULL;
   int index;

   if (this->operation == EPathOperation.string) {
      CString(string).set(CString(&this->path).string());
      return;
   }

   if (!object) {
      CString(string).printf("(unresolved!!!)");
      return;
   }

   CString(string).clear();
   if (this->assignment) {
      CString(string).printf_prepend("=%s", this->assignment);
   }
   if (this->factor) {
      CString(string).printf_prepend(" *%d", this->factor);
   }
   if (this->object.attr.attribute) {
      if (this->object.attr.index != ATTR_INDEX_VALUE) {
         CString(string).printf_prepend(".%d", this->object.attr.index);
      }
      if (this->object.attr.element != ATTR_ELEMENT_VALUE) {
         CString(string).printf_prepend("[%d]", this->object.attr.element);
      }
      CString(string).printf_prepend("/@%s", this->object.attr.attribute->name);
   }

   while (object) {
      root = this->root;
      if (object == root) {
         root_object = object;
      }
      object = CObjPersistent(CObject(object).parent());
   }

   object = this->object.object;
   while (object) {
      if (CObject(object).parent()) {
         if (CObjPersistent(CObject(object).parent()).child_alias_sibling_index(object, &index)) {
            CString(string).printf_prepend("[%d]", index);
         }
      }

      CString(string).printf_prepend("/%s", (char *)CObjClass_alias(CObject(object).obj_class()));

      if (object == root_object)
         break;

      object = CObjPersistent(CObject(object).parent());
   }

   switch (this->operation) {
   case EPathOperation.none:
      break;
   default:
      CString(string).printf_prepend("%s(", EPathOperation(this->operation).name());
      CString(string).printf_append(")");
      break;
   }
}                    

int CXPath::get_int(void) {
   int value;
   CString string;
   CString assignment;
   bool result;
   ARRAY<byte> *array;
   
   if (this->operation == EPathOperation.exists) {
                                                                                          
      return this->object.attr.attribute ? 1 : 0;
   }

   if (!this->object.attr.attribute && this->operation != EPathOperation.count) {
                                                  
      return (this->operation == EPathOperation.not);
   }

   if (this->assignment) {
                                                         
      new(&string).CString(NULL);
      new(&assignment).CString(NULL);
      CObjPersistent(this->object.object).attribute_get(this->object.attr.attribute,
                                                        this->object.attr.element,
                                                        this->object.attr.index,
                                                        &ATTRIBUTE:type<CString>,
                                                        (void *)&string);
      CXPath(this).assignment_get_string(&assignment);                                                        
      result = CString(&string).match(&assignment);




      if (this->operation == EPathOperation.not) {
         result = !result;
      }

      delete(&assignment);
      delete(&string);
      return result;
   }
   else {
      if (this->operation == EPathOperation.count) {
         if (this->object.attr.attribute && this->object.attr.attribute->type == PT_AttributeArray) {
            array = (ARRAY<byte> *)(((byte *)this->object.object) + this->object.attr.attribute->offset);
            value = ARRAY(array).count();
         }
         else {
            value = CObject(this->object.object).child_count();
         }
      }




      else {
         CObjPersistent(this->object.object).attribute_get(this->object.attr.attribute,
                                                           this->object.attr.element,
                                                           this->object.attr.index,
                                                           &ATTRIBUTE:type<int>,
                                                           (void *)&value);
      }

      if (this->factor) {
         value *= this->factor;
      }
      if (this->operation == EPathOperation.not) {
         value = !value;
      }
      else if (this->operation == EPathOperation.neg) {
         value = -value;
      }
   }

   return value;
}                   

void CXPath::get_string(CString *result) {
   const char *str_start, *str_end, *cp;

   switch (this->operation) {
   case EPathOperation.none:
      if (!this->object.attr.attribute && this->operation != EPathOperation.count) {
                                                     

         CString(result).clear();
         return;
      }

      CObjPersistent(this->object.object).attribute_get_string_element_index(this->object.attr.attribute,
                                                                             this->object.attr.element,
                                                                             this->object.attr.index,
                                                                             result);
      break;
   case EPathOperation.string:
                                        
      CString(result).clear();
      str_start = CString(&this->path).strchr(0x27);
      str_end = NULL;
      if (str_start) {
         str_start++;
         str_end = CString(&this->path).strrchr(0x27);
      }
      if (str_start && str_end) {
         cp = str_start;
         CString(result).clear();
         while (cp != str_end) {
            if (cp[0] == '\'' && cp[1] == '$' && cp[2] == 'i' && cp[3] =='\'') {
               CString(result).printf_append("%d", this->index);
               cp += 4;
            }
            else {
               CString(result).printf_append("%c", *cp);
               cp++;
            }
         }
      }
      break;
   case EPathOperation.alias:

      CString(result).set(CObjClass_displayname(CObject(this->object.object).obj_class()));
      break;
   default:
      CString(result).set("???");
      return;
      break;
   }
}                      

bool CXPath::assignment_get_string(CString *result) {
   const TAttribute *attribute;
   const char *cp;

   CString(result).clear();
   if (!this->assignment) {
       return FALSE;
   }

   if (this->assignment[0] == '@') {
      attribute = CObjPersistent(this->initial.object).attribute_find(&this->assignment[1]);
      if (!attribute)
         return FALSE;

      CObjPersistent(this->initial.object).attribute_get_string(attribute, result);
   }
   else {
      cp = this->assignment;
      CString(result).clear();
      while (cp != (this->assignment_end + 1) && *cp != '\0') {
         if (cp[0] == '$' && cp[1] == 'i') {
            CString(result).printf_append("%d", this->index);
            cp += 2;
         }
         else {
            CString(result).printf_append("%c", *cp);
            cp++;
         }
      }
   }
   return TRUE;
}                                 


                                                                              
MODULE::END                                                                   
                                                                              

# 45 "/home/jacob/keystone/src/datetime.c"

extern time_t mktime_gmt(struct tm *ptm);











  

bool ATTRIBUTE:convert<TUNIXTime>(CObjPersistent *object,
                                  const TAttributeType *dest_type, const TAttributeType *src_type,
                                  int dest_index, int src_index,
                                  void *dest, const void *src) {
   TUNIXTime *value;
   int int_value;
   time_t value_time;
   CString *string;
   struct tm *timeinfo, mk_timeinfo;
   int result, dow;

   if (dest_type == &ATTRIBUTE:type<TUNIXTime> && src_type == NULL) {
      memset(dest, 0, sizeof(TUNIXTime));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TUNIXTime> && src_type == &ATTRIBUTE:type<TUNIXTime>) {
      *((TUNIXTime *)dest) = *((TUNIXTime *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TUNIXTime>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TUNIXTime> && src_type == &ATTRIBUTE:type<int>) {
      value = (TUNIXTime *)dest;
      value_time = *value;
      if (dest_index != -1) {
         timeinfo = gmtime(&value_time);
         switch (dest_index){
         case EUNIXTimePart.DayOfWeek:
            break;
         case EUNIXTimePart.Year:
            timeinfo->tm_year = *((int *)src) - 1900;
            break;
         case EUNIXTimePart.Month:
            timeinfo->tm_mon = *((int *)src) - 1;
            break;
         case EUNIXTimePart.Day:
            timeinfo->tm_mday = *((int *)src);
            break;
         case EUNIXTimePart.Hour:
            timeinfo->tm_hour = *((int *)src);
            break;
         case EUNIXTimePart.Minute:
            timeinfo->tm_min = *((int *)src);
            break;
         case EUNIXTimePart.Second:
            timeinfo->tm_sec = *((int *)src);
            break;
         }
         value_time = mktime_gmt(timeinfo);
         *value = (TUNIXTime)value_time;

         if (dest_index == EUNIXTimePart.DayOfWeek) {
            dow = timeinfo->tm_wday;
                                                              
            dow = (dow + 6) % 7;

            *value -= dow * (60 * 60 * 24);
            *value += (*((int *)src) - 1) * (60 * 60 * 24);
         }
      }
      else {
         *((TUNIXTime *)dest) = *((int *)src);
      }
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<TUNIXTime>) {
      value = (TUNIXTime *)src;
      value_time = *value;
      timeinfo = gmtime(&value_time);
      if (src_index != -1) {
         switch (src_index){
         case EUNIXTimePart.Year:
            result = timeinfo->tm_year + 1900;
            break;
         case EUNIXTimePart.Month:
            result = timeinfo->tm_mon + 1;
            break;
         case EUNIXTimePart.Day:
            result = timeinfo->tm_mday;
            break;
         case EUNIXTimePart.Hour:
            result = timeinfo->tm_hour;
            break;
         case EUNIXTimePart.Minute:
            result = timeinfo->tm_min;
            break;
         case EUNIXTimePart.Second:
            result = timeinfo->tm_sec;
            break;
         default:
            result = 0;
            break;
         }
         *((int *)dest) = result;
      }
      else {
         *((int *)dest) = *((TUNIXTime *)src);
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TUNIXTime> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (TUNIXTime *)dest;
      value_time = *value;
      string = (CString *)src;

      CString(string).encoding_set(EStringEncoding.ASCII);

      if (!CString(string).length()) {
         *value = 0;
      }
      else {
         if (dest_index != -1) {
            int_value = atoi(CString(src).string());
            return ATTRIBUTE:convert<TUNIXTime>(object, &ATTRIBUTE:type<TUNIXTime>, &ATTRIBUTE:type<int>,
                                                dest_index, src_index,
                                                dest, (const void *)&int_value);
         }
         else {
            if (!CString(string).strchr('-') && !CString(string).strchr(':')) {
                                                   
               *value = atoi(CString(string).string());
            }
            else {
               CLEAR(&mk_timeinfo);
               sscanf(CString(string).string(),
                     "%4d-%02d-%02d %02d:%02d:%02d",
                     &mk_timeinfo.tm_year, &mk_timeinfo.tm_mon, &mk_timeinfo.tm_mday,
                     &mk_timeinfo.tm_hour, &mk_timeinfo.tm_min, &mk_timeinfo.tm_sec);

               mk_timeinfo.tm_year -= 1900;
               mk_timeinfo.tm_mon -= 1;
               if (mk_timeinfo.tm_year <= 0 || mk_timeinfo.tm_mon < 0) {
                                                           
               }
               else {
                  value_time = mktime_gmt(&mk_timeinfo);
                  *value = (TUNIXTime)value_time;
               }
            }
         }
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TUNIXTime>) {
      value = (TUNIXTime *)src;
      value_time = *value;
      string = (CString *)dest;

      if (*value == 0) {
         CString(string).clear();
      }
      else {
         timeinfo = gmtime(&value_time);
         if (src_index != -1) {
            switch (src_index){
            case EUNIXTimePart.Year:
               result = timeinfo->tm_year + 1900;
               break;
            case EUNIXTimePart.Month:
               result = timeinfo->tm_mon + 1;
               break;
            case EUNIXTimePart.Day:
               result = timeinfo->tm_mday;
               break;
            case EUNIXTimePart.Hour:
               result = timeinfo->tm_hour;
               break;
            case EUNIXTimePart.Minute:
               result = timeinfo->tm_min;
               break;
            case EUNIXTimePart.Second:
               result = timeinfo->tm_sec;
               break;
            default:
               result = 0;
               break;
            }
            CString(string).printf(src_index == 0 ? "%04d" : "%02d", result);
         }
         else {
            CString(string).printf("%4d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
                                   timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
         }
      }
      return TRUE;
   }
   return FALSE;
}

                                                                              
MODULE::END                                                                   
                                                                              


# 30 "/home/jacob/keystone/src/main.c"

# 1 "/home/jacob/keystone/src/serverstream.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              





# 1 "/home/jacob/keystone/src/syncobject.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              





                                                                              
MODULE::INTERFACE                                                             
                                                                              

#include <fcntl.h>
#ifdef OS_Win32
#include <io.h>
#define read(fd, buffer, count) _read(fd, buffer, count)
#define write(fd, buffer, count) _write(fd, buffer, count)
#define lseek(fd, offset, origin) _lseek(fd, offset, origin)
#define open(filename, oflag, pmode) _open(filename, oflag, pmode)
#define close(fd) _close(fd)
#endif
#ifdef OS_Linux
#define O_BINARY 0
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <sys/stat.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

class CEvent;
class CHsm;

class CSyncObject : CObjPersistent {
 private:
   bool signaled;
 public:
   static inline bool signaled(void);
   static inline void signal(bool signaled);
};

static inline bool CSyncObject::signaled(void) {
   return this->signaled;
};

static inline void CSyncObject::signal(bool signaled) {
   this->signaled = signaled;
};


                                              
class CMutex : CSyncObject {
 private:    
   byte native_data[16];    
 
   void new(void);
   void delete(void);    
 public:
   void CMutex(void);

   int lock(void);
   int trylock(void);
   int unlock(void);
};

ARRAY:typedef<CSyncObject *>;

typedef void (*THREAD_METHOD)(CObject *this, void *data);

class CTimer : CSyncObject {
 private:
   void delete(void);
   int interval_ms;
   THREAD_METHOD method;
   CObject *method_object;
   void *method_data;
   int timer_id;
 public:
   void CTimer(int interval_ms, CObject *object, THREAD_METHOD method, void *data);
};

class CThread : CSyncObject {
 private:
   CTimer *process_timer;
   byte native_data[16];
   THREAD_METHOD method;
   CObject *method_object;
   void *method_data;

                     

   void new(void);

   virtual int entry(void);
 public:
   void CThread(void);

   void send_event(CHsm *receiver, CEvent *event);
   void post_event(CHsm *receiver, CEvent *event);


   bool spawn(const char *process_name);
   void sleep(int time_ms);
   static inline void yield(void);
   CSyncObject* select_object(ARRAY<CSyncObject *> *object, int timeout_ms);
   int wait(void);
   void realtime_set(bool realtime);

   EXCEPTION<CPUFault_Ill, CPUFault_FPE, CPUFault_Segv>;
};

static inline void CThread::yield(void) {
   CThread(this).sleep(0);
}                  

class CThreadMain : CThread {
 private:
   void new(void);
};

class CIOObject : CSyncObject {
 private:
   int handle;
 public:
   void CIOObject(void);
   void open_handle(int handle); 
   int open(const char *name, int oflag);
   int close(void);
   static inline int read(void *buf, int count);
   static inline int write(void *buf, int count);
   static inline int length(void);
   static inline off_t lseek(off_t offset, int whence);

   virtual int eof(void);
   virtual int read_data(ARRAY<byte> *buffer, int count);
   virtual int write_data(void *buf, int count);
   virtual void write_string(char *buffer);
   virtual int read_string(CString *string);
   int ioctl(int request, void *argp);
};

static inline int CIOObject::read(void *buf, int count) {
   return read(this->handle, buf, count);
};

static inline int CIOObject::write(void *buf, int count) {
   return write(this->handle, buf, count);
};

static inline int CIOObject::length(void) {
   int result;
   result = lseek(this->handle, 0, SEEK_END);
   lseek(this->handle, 0, SEEK_SET);
   return result;
};

static inline off_t CIOObject::lseek(off_t offset, int whence) {
   return lseek(this->handle, offset, whence);
};

class CSerial : CIOObject {
};

class CIOMemory : CIOObject {
 private:
   byte *data;
   int size;
   int read_position;
 public:
   void CIOMemory(void *data, int size);

   virtual int eof(void);
   virtual int read_data(ARRAY<byte> *buffer, int count);
};

      

#if 0
class CObjClientStream : CObjClient {
   CStream *stream;
};
#endif

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CIOObject::CIOObject(void) {
}                        

void CIOObject::open_handle(int handle) {
   this->handle = handle;
}                          

int CIOObject::open(const char *name, int oflag) {
   this->handle = name ? open(name, oflag, S_IREAD | S_IWRITE) : STDOUT;
   return this->handle;
}                   

int CIOObject::close(void) {
   return close(this->handle);
}                    

int CIOObject::eof(void) {
   bool result = FALSE;
   long end_pos, cur_pos;

   cur_pos = lseek(this->handle, 0L, SEEK_CUR);

   if (cur_pos < 0)
      return TRUE;

   end_pos = lseek(this->handle, 0L, SEEK_END);

   result = cur_pos >= end_pos;

   lseek(this->handle, cur_pos, SEEK_SET);

   return result;
};

int CIOObject::read_data(ARRAY<byte> *buffer, int count) {
   int read_count = read(this->handle, ARRAY(buffer).data(), count);



   return read_count;
}                        

int CIOObject::write_data(void *buf, int count) {
   return CIOObject(this).write(buf, count);
}                         

void CIOObject::write_string(char *buffer) {
   write(this->handle, buffer, strlen(buffer));
}                           

int CIOObject::read_string(CString *string) {
   return 0; 
}                          

#ifdef OS_Linux
int CIOObject::ioctl(int request, void *argp) {
   return ioctl(this->handle, request, argp);
}                    
#endif

int CThread::entry(void) {
   return 0;
}

void CIOMemory::CIOMemory(void *data, int size) {
   this->read_position = 0;
   this->data = data;
   this->size = size;
}                        

int CIOMemory::eof(void) {
   return this->read_position >= this->size;
}                  

int CIOMemory::read_data(ARRAY<byte> *buffer, int count) {
   if (count + this->read_position > this->size) {
      count = this->size - this->read_position;
   }

   memcpy(ARRAY(buffer).data(), this->data + this->read_position, count);
   this->read_position += count;

   return count;
}                        

OBJECT<CThreadMain, thread_main>;

                                                                              
MODULE::END                                                                   
                                                                              


# 20 "/home/jacob/keystone/src/serverstream.c"
# 1 "/home/jacob/keystone/src/objdataserver.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              



# 1 "/home/jacob/keystone/src/event.c"
                                                                              
MODULE::IMPORT                                                                
                                                                              




                                                                              
MODULE::INTERFACE                                                             
                                                                              

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
   int modifier;                                                   
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
   int modifier;                                                   
 
   void CEventKey(EEventKeyType type, EEventKey key, char value, int modifier);
};

                          

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

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CEvent::CEvent(void) {
}                  

void CEvent::delete(void) {
}                  

void CEvent::print(void) {
   printf("event\n");
}                 

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
}                        

void CEventKey::delete(void) {
}                     

void CEventState::CEventState(EEventState type) {
   this->type = type;
}                            

void CFsm::CFsm(CObject *parent, STATE initial) {
   CObject(this).parent_set(parent);
   this->state  = initial;
}              

STATE CFsm::top(CEvent *event) {
   return NULL;
}             

void CFsm::init(void) {
   CEventState event;
   STATE state = NULL;

   new(&event).CEventState(EEventState.init);

   while (state != this->state) {
      state = this->state;
      CFsm(this).event(CEvent(&event));
   }



   state = this->state;
   this->state = (STATE)&CFsm:top;
   CFsm(this).transition(state);

   delete(&event);
}              

void CFsm::event(CEvent *event) {
   STATE state = this->state;

   while (state) {
      state = (*state)(CObject(this).parent(), event);
   }
}               

#if STATE_TRACE
void CFsm::state_trace(STATE state, bool enter) {
   printf("%s: %s::%x\n", enter ? "ENTER" : "LEAVE", CObjClass_name(CObject(CObject(this).parent()).obj_class()), state);
}                          
    
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
}                    

STATE CFsm::parent_find(STATE stateA, STATE stateB) {
                                                 
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
}                     

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
}                    

void CFsm::transition_initial(STATE state) {
   this->state = state;
}                            

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
}                  

                                                                              
MODULE::END                                                                   
                                                                              
# 18 "/home/jacob/keystone/src/objdataserver.c"




                                                                              
MODULE::INTERFACE                                                             
                                                                              

ARRAY:typedef<CObjPersistent *>;
class CObjServer;

ENUM:typedef<EEventObjPersistent> {
   {AllUpdate},
   {AllDelete},
   {ObjectNew},
   {ObjectDelete},
   {AttributeSet},
   {AttributeUpdateEnd},
   {SelectionUpdate},
};

class CEventObjPersistent : CEvent {
 private:
   EEventObjPersistent type;

   CString path;
   CString value;

   void new(void);
   void delete(void);
 public:
   void CEventObjPersistent(EEventObjPersistent type, const char *path, const char *value);
};

class CObjClient : CObject {
 private:
   CObjServer *server;
   CObject *host;
   ARRAY<CObjPersistent *> object_root;                                   
   CThread *thread;
   bool disable;


   void new(void);
   void delete(void);
 public:
   void CObjClient(CObjServer *server, CObject *host);

   void disable(bool disable);
   void object_root_add(CObjPersistent *object_root);
   static inline bool object_subscribed(CObject *object);

   virtual void notify_all_update(void);
   virtual void notify_object_child_add(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_child_remove(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_selection_update(CObjPersistent *object, bool changing);
};

static inline bool CObjClient::object_subscribed(CObject *object) {
   int i;

   if (this->disable) {
      return FALSE;
   }

   while (object) {
      for (i = 0; i < ARRAY(&this->object_root).count(); i++) {
         if (object == CObject(ARRAY(&this->object_root).data()[i])) {
            return TRUE;
         }
      }
      object = CObject(object).parent();
   }
   return FALSE;
};

ARRAY:typedef<CObjClient *>;

typedef struct {
   CObjPersistent *object;
   CThread *thread;
} TObjectThreadOwner;

typedef struct {
   enum {attribute_update, child_add, child_remove} type;
   CThread *dest_thread;
   CObjPersistent *object;
   TAttribute *attribute;
   int index;
} TServerEvent;

ARRAY:typedef<TObjectThreadOwner *>;
ARRAY:typedef<CEventObjPersistent>;

class CObjServer : CObjPersistent {
 private:
   bool disable;

   CObjPersistent *server_root;
   ARRAY<CObjClient *> client;
   ARRAY<TObjectThreadOwner *> thread_owner;
   ARRAY<CEventObjPersistent> obj_event;



   CSelection *selection;

   void new(void);
   void delete(void);

   void attribute_update(CObjPersistent *object,
                         ARRAY<const TAttribute *> *attribute);
   void child_add(CObjPersistent *object, CObjPersistent *child);
   void child_remove(CObjPersistent *object, CObjPersistent *child);
   void selection_update(CObjPersistent *object, bool changing);
 public:
   void CObjServer(CObjPersistent *object);
   void disable(bool disable);





   void server_root_set(CObjPersistent *object);
   int object_root_count(CObjPersistent *object);
   CThread *thread_owner_find(CObjPersistent *object);
   void client_add(CObjClient *client);
   void client_remove(CObjClient *client);

   static inline void selection_set(CSelection *selection);
   static inline CSelection *selection_get(void);

   void root_selection_update(void);
   void root_selection_update_end(void);

   void undo(void);
   void redo(void);
};

static inline void CObjServer::selection_set(CSelection *selection) {
   this->selection = selection;
}                             

static inline CSelection *CObjServer::selection_get(void) {
   return this->selection;
}                             

ARRAY:typedef<CObjServer *>;

class CFrameworkServer : CObjPersistent {
 private:
   ARRAY<CObjServer *> obj_server;

   void new(void);
   void delete(void);
 public:
   void CFrameworkServer(void);

   void obj_server_add(CObjServer *obj_server);
   void obj_server_remove(CObjServer *obj_server);
   CObjServer *object_obj_server(CObjPersistent *object);
   CObjClient *object_obj_client(CObjPersistent *object);
};

extern CFrameworkServer framework_server;

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CFrameworkServer::new(void) {
   ARRAY(&this->obj_server).new();
}                         

void CFrameworkServer::delete(void) {
   ARRAY(&this->obj_server).delete();
}                            

void CFrameworkServer::obj_server_add(CObjServer *obj_server) {
   ARRAY(&this->obj_server).item_add(obj_server);
}                                    

void CFrameworkServer::obj_server_remove(CObjServer *obj_server) {
   ARRAY(&this->obj_server).item_remove(obj_server);
}                                       

CObjServer *CFrameworkServer::object_obj_server(CObjPersistent *object) {
   int i;
   CObjServer *server;
   CObjPersistent *root;

   for (i = ARRAY(&this->obj_server).count() - 1; i >=0; i--) {
      server = ARRAY(&this->obj_server).data()[i];
      root = server->server_root;

      if ((root == object) || CObject(root).is_child(CObject(object))) {
         return server;
      }
   }

   return NULL;
}                                       

CObjClient *CFrameworkServer::object_obj_client(CObjPersistent *object) {
   int i, j;
   CObjServer *server;
   CObjClient *client;

   for (i = 0; i < ARRAY(&this->obj_server).count(); i++) {
      server = ARRAY(&this->obj_server).data()[i];
      for (j = 0; j < ARRAY(&server->client).count(); j++) {
         client = ARRAY(&server->client).data()[j];

         if (CObject(client->host).is_child(CObject(object))) {
            return client;
         }
      }
   }

   return NULL;
}                                       

OBJECT<CFrameworkServer, framework_server>;

void CXPath::root_set_default(CObjPersistent *parent) {
   CObjClient *client = CFrameworkServer(&framework_server).object_obj_client(parent);
   CObjServer *server = NULL;
   TObjectPtr initial;

   if (client) {
      server = client->server;
   }
   if (server) {
      CXPath(this).root_set(server->server_root);
      CLEAR(&initial);
      initial.object = ARRAY(&client->object_root).data()[0];
      initial.attr.element = -1;
      initial.attr.index   = -1;
      CXPath(this).initial_set(&initial);
   }
}                            

bool ATTRIBUTE:convert<CXPath>(CObjPersistent *object,
                               const TAttributeType *dest_type, const TAttributeType *src_type,
                               int src_index, int dest_index,
                               void *dest, const void *src) {
   CString *string;
   CXPath *x_path;
   CObjClient *client = CFrameworkServer(&framework_server).object_obj_client(object);
   CObjServer *server = NULL;
   TObjectPtr initial;

   if (client) {
      server = client->server;
   }

   if (dest_type == &ATTRIBUTE:type<CXPath> && src_type == NULL) {
      new(dest).CXPath(NULL, NULL);
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CXPath> && src_type == &ATTRIBUTE:type<CXPath>) {
      x_path = (CXPath *)src;

      CXPath(dest)->root = CXPath(x_path)->root;
      CXPath(dest)->initial = CXPath(x_path)->initial;

      CXPath(dest).path_set(CString(&CXPath(x_path)->path).string());

      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CXPath> ) {
      delete(src);
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<CXPath>) {
      string = (CString *)dest;
      x_path = (CXPath *)src;
      if (server) {
         CXPath(x_path).path_get(string);
      }
      else {
         CString(string).set("(no server)");
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CXPath> && src_type == &ATTRIBUTE:type<CString>) {
      x_path = (CXPath *)dest;
      string = (CString *)src;
      if (server) {
         CXPath(x_path).root_set(server->server_root);
      }

                    
      if (client) {
         CLEAR(&initial);
         initial.object = ARRAY(&client->object_root).data()[0];
         CXPath(x_path).initial_set(&initial);
      }

      CXPath(x_path).path_set(CString(string).string());
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<TObjectPtr>(CObjPersistent *object,
                                   const TAttributeType *dest_type, const TAttributeType *src_type,
                                   int dest_index, int src_index,
                                   void *dest, const void *src) {
   CXPath path;
   TObjectPtr *objptr;
   CString *string;
   CObjClient *client = CFrameworkServer(&framework_server).object_obj_client(object);
   CObjServer *server = NULL;


   if (client) {
      server = client->server;
   }

   if (dest_type == &ATTRIBUTE:type<TObjectPtr> && src_type == NULL) {
      memset(dest, 0, sizeof(TObjectPtr));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TObjectPtr> && src_type == &ATTRIBUTE:type<TObjectPtr>) {
      *((TObjectPtr *)dest) = *((TObjectPtr *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TObjectPtr> ) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> &&
      src_type == &ATTRIBUTE:type<TObjectPtr>) {
      objptr = (TObjectPtr *)src;
      if (objptr->object) {
         new(&path).CXPath(NULL, server->server_root);
         CXPath(&path).set(objptr->object, objptr->attr.attribute, -1);
         CXPath(&path).path_get(dest);
         delete(&path);
      }
      else {
         CString(dest).set("none");
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TObjectPtr> &&
       src_type == &ATTRIBUTE:type<CString>) {
      string = (CString *)src;

      new(&path).CXPath(NULL, server->server_root);
      objptr = (TObjectPtr *)dest;

      CXPath(&path).initial_set(dest);
      CXPath(&path).path_set(CString(string).string());
      *objptr = path.object;
      delete(&path);
      return TRUE;
   }

   return FALSE;
}

bool ATTRIBUTE:convert<TAttributePtr>(CObjPersistent *object,
                                      const TAttributeType *dest_type, const TAttributeType *src_type,
                                      int dest_index, int src_index,
                                      void *dest, const void *src) {
   TAttributePtr *attrptr;
   CString *string;
   const char *element_ptr, *index_ptr;

   if (dest_type == &ATTRIBUTE:type<TAttributePtr> && src_type == NULL) {
      memset(dest, 0, sizeof(TAttributePtr));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TAttributePtr> && src_type == &ATTRIBUTE:type<TAttributePtr>) {
      memcpy(dest, src, sizeof(TAttributePtr));
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TAttributePtr>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> &&
      src_type == &ATTRIBUTE:type<TAttributePtr>) {
      attrptr = (TAttributePtr *)src;
      if (attrptr->attribute) {
         CString(dest).set(attrptr->attribute->name);
         if (attrptr->element != -1) {
            CString(dest).printf_append("[%d]", attrptr->element);
         }
         if (attrptr->index != -1) {
            CString(dest).printf_append(".%d", attrptr->index);
         }
      }
      else {
         CString(dest).set("none");
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TAttributePtr> &&
       src_type == &ATTRIBUTE:type<CString>) {
      string = (CString *)src;
      attrptr = (TAttributePtr *)dest;

      attrptr->attribute = NULL;
      attrptr->element = -1;
      attrptr->index = -1;
      if (!CObject(object).parent()) {
         return TRUE;
      }

      index_ptr = strchr(CString(string).string(), '.');
      if (index_ptr) {
         attrptr->index = atoi(index_ptr + 1);
         CString(string).setn(CString(string).string(), index_ptr - CString(string).string());
      }
      element_ptr = strchr(CString(string).string(), '[');
      if (element_ptr) {
         attrptr->element = atoi(element_ptr + 1);
         CString(string).setn(CString(string).string(), element_ptr - CString(string).string());
      }
      attrptr->attribute = CObjPersistent(CObject(object).parent()).attribute_find(CString(string).string());

      return TRUE;
   }

   return FALSE;
}

void CEventObjPersistent::new(void) {
   new(&this->path).CString(NULL);
   new(&this->value).CString(NULL);
}                            

void CEventObjPersistent::CEventObjPersistent(EEventObjPersistent type, const char *path, const char *value) {
   this->type = type;
   CString(&this->path).set(path);
   CString(&this->value).set(value);
}                                            

void CEventObjPersistent::delete(void) {
   delete(&this->value);
   delete(&this->path);
}                               



void CObjClient::new(void) {
   ARRAY(&this->object_root).new();
}                   

void CObjClient::CObjClient(CObjServer *server, CObject *host) {
   if (!server) {
      ASSERT("no server\n");
   }

   this->server = server;
   this->host = host;
   this->thread = CFramework(&framework).thread_this();

   CObjServer(server).client_add(this);

                       
   CObjClient(this).notify_all_update();
}                          

void CObjClient::delete(void) {
   ARRAY(&this->object_root).delete();

   CObjServer(this->server).client_remove(this);
}                      

void CObjClient::disable(bool disable) {
   this->disable = disable;
}                       

void CObjClient::object_root_add(CObjPersistent *object_root) {
   if (ARRAY(&this->object_root).count() != 0) {
      WARN("CObjClient::object_root_add - single root only supported");
   }

   ARRAY(&this->object_root).item_add(object_root);
}                               

void CObjClient::notify_all_update(void) {}
void CObjClient::notify_object_child_add(CObjPersistent *object, CObjPersistent *child) {}
void CObjClient::notify_object_child_remove(CObjPersistent *object, CObjPersistent *child) {}
void CObjClient::notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing) {}
void CObjClient::notify_selection_update(CObjPersistent *object, bool changing) {}

void CObjServer::new(void) {
   ARRAY(&this->client).new();
   ARRAY(&this->thread_owner).new();

   CFrameworkServer(&framework_server).obj_server_add(this);
}                     

void CObjServer::CObjServer(CObjPersistent *object) {
   if (object) {
      CObjServer(this).server_root_set(object);
   }
}                          

void CObjServer::delete(void) {
   ARRAY(&this->thread_owner).delete();
   ARRAY(&this->client).delete();

   CFrameworkServer(&framework_server).obj_server_remove(this);
}                      

void CObjServer::disable(bool disable) {
   this->disable = disable;
}                       

void CObjServer::server_root_set(CObjPersistent *object) {
   this->server_root = object;
}                               

int CObjServer::object_root_count(CObjPersistent *object) {
   CObjPersistent *root;
   int count = 0;

   while (object) {
      root = this->server_root;
      if (object == root) {
         count++;
      }
      object = CObjPersistent(CObject(object).parent());
   }

   return count;
}                                 

CThread *CObjServer::thread_owner_find(CObjPersistent *object) {
   CThread *result = CFramework(&framework).thread_main();

   return result;
}                                 

void CObjServer::client_add(CObjClient *client) {
   ARRAY(&this->client).item_add(client);
}                          

void CObjServer::client_remove(CObjClient *client) {
   ARRAY(&this->client).item_remove(client);
}                             

void CObjServer::attribute_update(CObjPersistent *object,
                                  ARRAY<const TAttribute *> *attribute) {
}                                

void CObjServer::child_add(CObjPersistent *object, CObjPersistent *child) {
   CObjClient *client;
   int i;

   if (this->disable)
      return;

   CObjServer(this).attribute_update(object, NULL);

   for (i = 0; i < ARRAY(&this->client).count(); i++) {
      client = ARRAY(&this->client).data()[i];

      if (CObjClient(client).object_subscribed(CObject(object))) {
         CObjClient(client).notify_object_child_add(object, child);
      }
   }

   CObjServer(this).attribute_update(object, NULL);
}                         

void CObjServer::child_remove(CObjPersistent *object, CObjPersistent *child) {
   CObjClient *client;
   int i, count;

   if (this->disable)
     return;

   CObjServer(this).attribute_update(object, NULL);

   for (i = 0; i < ARRAY(&this->client).count(); i++) {
      count = ARRAY(&this->client).count();
      client = ARRAY(&this->client).data()[i];

      if (CObjClient(client).object_subscribed(CObject(             child))) {
         CObjClient(client).notify_object_child_remove(object, CObjPersistent(child));
      }
      
                                                                              
      if (count != ARRAY(&this->client).count()) {
         i--;
         count = ARRAY(&this->client).count();
      }
   }
}                            

void CObjServer::selection_update(CObjPersistent *object, bool changing) {
   CObjClient *client;
   int i;

   if (this->disable)
      return;

   for (i = 0; i < ARRAY(&this->client).count(); i++) {
      client = ARRAY(&this->client).data()[i];

      if (CObjClient(client).object_subscribed(CObject(object))) {
         CObjClient(client).notify_selection_update(object, changing);
      }
   }
}                                

void CObjServer::root_selection_update(void) {
   CObjPersistent(this->server_root).notify_selection_update(TRUE);
}                           

void CObjServer::root_selection_update_end(void) {
   CObjPersistent(this->server_root).notify_selection_update(FALSE);
}                               

void CObjPersistent::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CObjServer *server;

   if (!CObjClass_is_derived(&class(CObjPersistent), CObject(object).obj_class())) {
      return;
   }

   server = CFrameworkServer(&framework_server).object_obj_server(CObjPersistent(object));

   if (server && server->disable)
      return;

   if (server) {
      switch (linkage) {
      case EObjectLinkage.ChildAdd:
         CObjServer(server).child_add(this, CObjPersistent(object));
         break;
      case EObjectLinkage.ChildRemovePre:
         CObjServer(server).child_remove(this, CObjPersistent(object));
         break;
      default:
         break;
      }
   }
}                                         

void CObjPersistent::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObjClient *client;
   int i;
   CObjServer *server = CFrameworkServer(&framework_server).object_obj_server(CObjPersistent(this));

   if (server && server->disable)
      return;

   if (server                 ) {
      for (i = 0; i < ARRAY(&server->client).count(); i++) {
         client = ARRAY(&server->client).data()[i];

         if (CObjClient(client).object_subscribed(CObject(this))) {
            CObjClient(client).notify_object_attribute_update(this, NULL, changing);
         }
      }

      CObjServer(server).attribute_update(this, attribute);
   }
}                                           

void CObjPersistent::notify_selection_update(bool changing) {
   CObjServer *server = CFrameworkServer(&framework_server).object_obj_server(CObjPersistent(this));

   if (server && server->disable)
     return;

   if (server) {
      CObjServer(server).selection_update(this, changing);
   }
}                                           

void CObjPersistent::notify_all_update(bool changing) {
   CObjClient *client;
   int i;
   CObjServer *server = CFrameworkServer(&framework_server).object_obj_server(CObjPersistent(this));

   if ((server && server->disable) || changing)
      return;

   if (server) {
      for (i = ARRAY(&server->client).count() - 1; i >= 0; i--) {
         client = ARRAY(&server->client).data()[i];

         if (CObjClient(client).object_subscribed(CObject(this))) {
            CObjClient(client).notify_all_update();
         }
      }
   }
}                                     

                                                                              
MODULE::END                                                                   
                                                                              


# 21 "/home/jacob/keystone/src/serverstream.c"


                                                                              
MODULE::INTERFACE                                                             
                                                                              

#define DUMP_DEBUG FALSE

class CObjClientStream : CObjClient {
 private:
   CObjPersistent *resolving;
   CIOObject *connection;

   void event_add(CEventObjPersistent *event);
   void event_attribute_add(CObjPersistent *object, const TAttribute *attribute, int index);

   void object_add(CObjPersistent *object, CObjPersistent *child);
   void object_send(CObjPersistent *object);

   virtual void notify_all_update(void);
   virtual void notify_object_child_add(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_child_remove(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute);

#if 0
   virtual void stream_object_new(void);
   virtual void stream_object_delete(void);
   virtual void stream_object_set(void);
#endif
 public:
   void CObjClientStream(CObjServer *server, CObject *host, CIOObject *connection);
   void process_pending(void);
};

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CObjClientStream::CObjClientStream(CObjServer *server, CObject *host, CIOObject *connection) {
   this->connection = connection;

   CObjClient(this).CObjClient(server, host);
}                                      

void CObjClientStream::process_pending(void) {
   CString message;
   CEventObjPersistent event;
   CString type_string;
   CString path_string, value;
   CXPath path;

   CObjPersistent *object;
   const CObjClass *obj_class;

   new(&message).CString(NULL);

   new(&type_string).CString(NULL);
   new(&path_string).CString(NULL);
   new(&value).CString(NULL);

   new(&path).CXPath(NULL, CObjClient(this)->server->server_root);

                        
   CIOObject(this->connection).read_string(&message);
#if DUMP_DEBUG
   printf("<%s\n", CString(&message).string());
#endif


   CString(&message).tokenise(&type_string, ":", TRUE);
   CString(&message).tokenise(&path_string, ":", TRUE);
   CString(&message).tokenise(&value, ":", TRUE);

   new(&event).CEventObjPersistent(ENUM:decode(EEventObjPersistent,
                                               CString(&type_string).string()),
                                   CString(&path_string).string(),
                                   CString(&value).string());



   CXPath(&path).path_set(CString(&path_string).string());

   this->resolving = path.object.object;
   switch(event.type) {
   case EEventObjPersistent.AllDelete:
      break;
   case EEventObjPersistent.ObjectNew:
      obj_class = CFramework(&framework).obj_class_find(CString(&event.value).string());
      if (!obj_class) {
         ASSERT("CObjServerStream: class not found");
      }
      if (!CObjClass_is_derived(&class(CObjPersistent), obj_class)) {
         ASSERT("CObjServerStream: non persistent class specified");
      }

      object = CObjPersistent(new.class(obj_class));
      CObjPersistent(object).CObjPersistent();
      CObject(path.object.object).child_add(CObject(object));
      break;
   case EEventObjPersistent.ObjectDelete:
      delete(path.object.object);
      break;
   case EEventObjPersistent.AttributeSet:
      if (path.object.object) {
         CObjPersistent(path.object.object).attribute_set_text(path.object.attribute,
                                                               CString(&event.value).string());
         CObjPersistent(path.object.object).attribute_update_end();
      }
      break;
   case EEventObjPersistent.AttributeUpdateEnd:
      break;
   case EEventObjPersistent.SelectionUpdate:
      break;
   default:
      break;
   }

   this->resolving = NULL;


   delete(&event);

   delete(&path);

   delete(&value);
   delete(&path_string);
   delete(&type_string);

   delete(&message);
}                                     

void CObjClientStream::event_add(CEventObjPersistent *event) {
   CString message;

   if (this->connection) {
      new(&message).CString(NULL);

      CString(&message).printf("%s", EEventObjPersistent(event->type).name());
      if (!CString(&event->path).empty()) {
         CString(&message).printf_append(":%s", CString(&event->path).string());
      }
      if (!CString(&event->value).empty()) {
         CString(&message).printf_append(":%s", CString(&event->value).string());
      }
#if DUMP_DEBUG
      printf(">%s\n", CString(&message).string());
#endif
      CIOObject(this->connection).write_string(CString(&message).string());

      delete(&message);
   }

   delete(event);
}                               

void CObjClientStream::event_attribute_add(CObjPersistent *object, const TAttribute *attribute, int index) {
   CEventObjPersistent event;
   CXPath path;
   CString path_string, value;

   new(&path).CXPath(NULL, CObjClient(this)->server->server_root);
   new(&path_string).CString(NULL);
   new(&value).CString(NULL);

   CXPath(&path).set(object, attribute, index);
   CXPath(&path).path_get(&path_string);
   CObjPersistent(object).attribute_get_string(attribute, &value);

   new(&event).CEventObjPersistent(EEventObjPersistent.AttributeSet,
                                   CString(&path_string).string(),
                                   CString(&value).string());

   CObjClientStream(this).event_add(&event);

   delete(&value);
   delete(&path_string);
   delete(&path);
}                                         

void CObjClientStream::object_add(CObjPersistent *object, CObjPersistent *child) {
   CXPath path;
   CString path_string;
   CEventObjPersistent event;

   new(&path).CXPath(NULL, CObjClient(this)->server->server_root);
   new(&path_string).CString(NULL);

   CXPath(&path).set(object, NULL, 0);
   CXPath(&path).path_get(&path_string);

   new(&event).CEventObjPersistent(EEventObjPersistent.ObjectNew,
                                   CString(&path_string).string(),
                                   CObjClass_alias(CObject(child).obj_class()));
   CObjClientStream(this).event_add(&event);

   CObjClientStream(this).object_send(child);

   delete(&path_string);
   delete(&path);
}                                

void CObjClientStream::object_send(CObjPersistent *object) {
   int i;
   ARRAY<const TAttribute *> attr_list;

                                                        
   ARRAY(&attr_list).new();
   CObjPersistent(object).attribute_list(&attr_list, TRUE, FALSE, FALSE);
   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      if (!CObjPersistent(object).attribute_default_get(ARRAY(&attr_list).data()[i])) {
         CObjClientStream(this).event_attribute_add(object, ARRAY(&attr_list).data()[i], -1);
      }
   }
   ARRAY(&attr_list).delete();
}                                 

void CObjClientStream::notify_object_child_add(CObjPersistent *object, CObjPersistent *child) {
   if (this->resolving == object)
      return;

   CObjClientStream(this).object_add(object, child);
}                                             

void CObjClientStream::notify_all_update(void) {
   CObject *object;
   CObjPersistent *object_root;
   CEventObjPersistent event;
   int i;

   new(&event).CEventObjPersistent(EEventObjPersistent.AllDelete, NULL, NULL);
   CObjClientStream(this).event_add(&event);

   for (i = 0; i < ARRAY(&CObjClient(this)->object_root).count(); i++) {
      object_root = ARRAY(&CObjClient(this)->object_root).data()[i];

      object = CObject(object_root).child_first();

      while (object) {
         CObjClientStream(this).object_add(object_root, CObjPersistent(object));
         object = CObject(object_root).child_next(object);
      }
   }
}                                       

void CObjClientStream::notify_object_child_remove(CObjPersistent *object, CObjPersistent *child) {
   CXPath path;
   CString path_string;
   CEventObjPersistent event;

   if (this->resolving == child)
      return;

   new(&path).CXPath(NULL, CObjClient(this)->server->server_root);
   new(&path_string).CString(NULL);

   CXPath(&path).set(child, NULL, 0);
   CXPath(&path).path_get(&path_string);

   new(&event).CEventObjPersistent(EEventObjPersistent.ObjectDelete,
                                   CString(&path_string).string(),
                                   NULL);
   CObjClientStream(this).event_add(&event);

   delete(&path_string);
   delete(&path);
}                                                

void CObjClientStream::notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute) {
   if (this->resolving == object &&
      CObjServer(CObjClient(this)->server).object_root_count(object) <= 1) {
      return;
   }

   CObjClientStream(this).object_send(object);
}                                                    

                                                                              
MODULE::END                                                                   
                                                                              


# 32 "/home/jacob/keystone/src/main.c"


# 1 "/home/jacob/keystone/src/file.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              




#define ZLIB_FILE TRUE

                                                                              
MODULE::INTERFACE                                                             
                                                                              

class CFile : CIOObject {
 private:
   void delete(void);
 public:
   void CFile(void);
};

ENUM:typedef<EFileLockMode> {
   {none}, {locked}, {loadOnly}
};

#if ZLIB_FILE 
class CFileGZ : CFile {
 private:
   byte lock_check;
   byte lock_check_read;
   void *gz_handle;
   EFileLockMode lock_mode;
   int position;
   bool open_write;
   void delete(void);
 public:
   void CFileGZ(EFileLockMode lock_mode);

   int open(const char *name, const char *mode);
   int close(void);
   virtual int eof(void);
   virtual int read_data(ARRAY<byte> *buffer, int count);
   virtual int write_data(void *buf, int count);
   virtual void write_string(char *buffer);
   virtual int read_string(CString *string);
};
#endif

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              



void CFile::CFile(void) {
   CIOObject(this).CIOObject();
}                

void CFile::delete(void) {
}                 

#if ZLIB_FILE 
#include "/home/jacob/keystone/src/base/../zlib/zlib.h"

void CFileGZ::CFileGZ(EFileLockMode lock_mode) {
   this->lock_mode = lock_mode;
}                    

void CFileGZ::delete(void) {
}                   

int CFileGZ::open(const char *name, const char *mode) {
   this->gz_handle = gzopen(name, mode);
   if (strchr(mode, 'w') || strchr(mode, 'W')) {
      this->open_write = TRUE;
   }
   else {
      this->open_write = FALSE;
   }
   this->position = 0;
   this->lock_check = 0; 
   return this->gz_handle ? 1 : -1;
}                 

int CFileGZ::close(void) {
   int result;
  
   if (this->open_write) {
      gzwrite(this->gz_handle, &this->lock_check, 1); 
   }
   else {
      if (this->lock_mode != EFileLockMode.none && !gzdirect(this->gz_handle)) {
         result = gzclose(this->gz_handle);
         return result != 0 ? result : this->lock_check != this->lock_check_read;
      }
      if (this->lock_mode == EFileLockMode.loadOnly && gzdirect(this->gz_handle)) {
         gzclose(this->gz_handle);
         return -1;
      }
   }
   return gzclose(this->gz_handle);
}                  

int CFileGZ::eof(void) {
   int erno;
   const char *errmsg;

   errmsg = gzerror(this->gz_handle, &erno);
   return gzeof(this->gz_handle) || erno != 0;
}                

int CFileGZ::read_data(ARRAY<byte> *buffer, int count) {
   int read_count;
   int i;
   int erno;
   const char *errmsg;
   
   read_count = gzread(this->gz_handle, ARRAY(buffer).data(), count);
   errmsg = gzerror(this->gz_handle, &erno);
   if (read_count && CFileGZ(this).eof()) {
      this->lock_check_read = ARRAY(buffer).data()[read_count - 1];
      read_count--;
   }
   if (this->lock_mode != EFileLockMode.none && !gzdirect(this->gz_handle)) {
      for (i = 0; i < read_count; i++) {

         ARRAY(buffer).data()[i] = ARRAY(buffer).data()[i] ^ 0xFF;
         this->lock_check ^= ARRAY(buffer).data()[i];
      }
   }
   this->position += read_count;   
   
   return read_count;
}                      

int CFileGZ::write_data(void *buf, int count) {
   char *buffer = (char *)buf;
   ARRAY<byte> data;
   int i, written;

   if (this->lock_mode != EFileLockMode.none) {
      if (!count) {
         return 0;
      }
      ARRAY(&data).new();
      ARRAY(&data).used_set(count);
      for (i = 0; i < count; i++) {
         this->lock_check ^= buffer[i];
         ARRAY(&data).data()[i] = buffer[i] ^ 0xFF;
      }
      written = gzwrite(this->gz_handle, ARRAY(&data).data(), count); 
      this->position += written;
      ARRAY(&data).delete();
   }
   else {
      written = gzwrite(this->gz_handle, buffer, strlen(buffer)); 
   }
   
   return written;
}                       

void CFileGZ::write_string(char *buffer) {
   CFileGZ(this).write_data(buffer, strlen(buffer));
}                         

int CFileGZ::read_string(CString *string) {
   return 0;
}                        

#endif

                                                                              
MODULE::END                                                                   
                                                                              


# 35 "/home/jacob/keystone/src/main.c"
# 1 "/home/jacob/keystone/src/memfile.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              



                                                                              
MODULE::INTERFACE                                                             
                                                                              

typedef struct {
   const char *name;
   int size;
   void *data;
} TMemFileDef;

class CMemFile : CObjPersistent {
 private:
   void new(void);
   void delete(void);

   TMemFileDef *memfile_def;

   void notify_object_linkage(EObjectLinkage linkage, CObject *object);
 public:
   ATTRIBUTE<CString name>;
};

ARRAY:typedef<CMemFile *>;

class CMemFileDirectory : CObjPersistent {
 private:
   ARRAY<CMemFile *> memfile;

   void new(void);
   void delete(void);
 public:
   void CMemFileDirectory(void);

   void memfile_add(CMemFile *memfile);
   CMemFile *memfile_find(const char *name);
};

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

CMemFileDirectory fw_memfile_directory;

void CMemFileDirectory::new(void) {
   ARRAY(&this->memfile).new();
}                          

void CMemFileDirectory::CMemFileDirectory(void) {
}                                        

void CMemFileDirectory::delete(void) {
   ARRAY(&this->memfile).delete();
}                             

void CMemFileDirectory::memfile_add(CMemFile *memfile) {
   ARRAY(&this->memfile).item_add(memfile);
}                                  

CMemFile *CMemFileDirectory::memfile_find(const char *name) {
   CMemFile *memfile;
   int i;

   for (i = 0; i < ARRAY(&this->memfile).count(); i++) {
      memfile = ARRAY(&this->memfile).data()[i];
      if (strcmp(memfile->memfile_def->name, name) == 0) {
         return memfile;
      }
   }

   return NULL;
}                                   

void CMemFile::new(void) {
}                 

void CMemFile::delete(void) {
}                    

void CMemFile::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   this->memfile_def = (TMemFileDef *)object;

   CMemFileDirectory(&fw_memfile_directory).memfile_add(this);
}                                   

                                                                              
MODULE::END                                                                   
                                                                              


# 36 "/home/jacob/keystone/src/main.c"
# 1 "/home/jacob/keystone/src/graphics/graphics.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              

# 1 "/home/jacob/keystone/src/graphics/gobject.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              





# 1 "/home/jacob/keystone/src/graphics/gpalette.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              





                                                                              
MODULE::INTERFACE                                                             
                                                                              

ENUM:typedef<EGColourNamed> {
   {aliceblue}, {antiquewhite}, {aqua}, {aquamarine}, {azure}, {beige}, {bisque}, {black},
   {blanchedalmond}, {blue}, {blueviolet}, {brown}, {burlywood}, {cadetblue}, {chartresue}, {chocolate},
   {coral}, {cornflowerblue}, {cornsilk}, {crimson}, {cyan}, {darkblue}, {darkcyan}, {darkgoldenrod},
   {darkgray}, {darkgreen}, {darkgrey}, {darkkhaki}, {darkmagenta}, {darkolivegreen}, {darkorange}, {darkorchid},
   {darkred}, {darksalmon}, {darkseagreen}, {darksaltblue}, {darkslategray}, {darkslategrey}, {darkturquoise}, {darkviolet},
   {deeppink}, {deepskyblue}, {dimgray}, {dimgrey}, {dodgerblue}, {firebrick}, {floralwhite}, {forestgreen},
   {fuchsia}, {gainsboro}, {ghostwhite}, {gold}, {goldenrod}, {gray}, {grey}, {green},
   {greenyellow}, {honeydew}, {hotpink}, {indianred}, {indigo}, {ivory}, {khaki}, {lavender},
   {lavenderblush}, {lawngreen}, {lemonchiffon}, {lightblue}, {lightcoral}, {lightcyan}, {lightgoldenrodyellow}, {lightgray},
   {lightgreen}, {lightgrey},
   {lightpink}, {lightsalmon}, {lightseagreen}, {lightskyblue}, {lightslategray}, {lightslategrey}, {lightsteelblue}, {lightyellow},
   {lime}, {limegreen}, {linen}, {magenta}, {maroon}, {mediumaquamarine}, {mediumblue}, {mediumorchid},
   {mediumpurple}, {mediumseagreen}, {mediumslateblue}, {mediumspringgreen}, {mediumturquoise}, {mediumvioletred}, {midnightblue}, {mintcream},
   {mistyrose}, {moccasin}, {navyjowhite}, {navy}, {oldlace}, {olive}, {olivedrab}, {orange},
   {orangered}, {orchid}, {palegoldenrod}, {palegreen}, {paleturquoise}, {palevioletred}, {papayawhip}, {peachpuff},
   {peru}, {pink}, {plum}, {powderblue}, {purple}, {red}, {rosybrown}, {royalblue},
   {saddlebrown}, {salmon}, {sandybrown}, {seagreen}, {seashell}, {sienna}, {silver}, {skyblue},
   {slateblue}, {slategray}, {slategrey}, {snow}, {springgreen}, {steelblue}, {tan}, {teal},
   {thistle}, {tomato}, {turquoise}, {violet}, {wheat}, {white}, {whitesmoke}, {yellow},
   {yellowgreen}
};

typedef ulong TGColour;
ATTRIBUTE:typedef<TGColour>;
ARRAY:typedef<TGColour>;

#define GCOL_TYPE_RGB              0x00000000
#define GCOL_TYPE_INDEX            0x10000000
#define GCOL_TYPE_INDEXB           0x20000000
#define GCOL_TYPE_NAMED            0x30000000
#define GCOL_TYPE_NONE             0x40000000
#define GCOL_TYPE_DIALOG           0x50000000
#define GCOL_TYPE_GRADIENT         0x60000000
#define GCOL_TYPE_MASK             0xF0000000
#define GCOL_TYPE(colour)          ((colour) & GCOL_TYPE_MASK)
#define GCOL_RGB(red, green, blue) (GCOL_TYPE_RGB   | ((ulong)(((red) & 0xFFL) << 16) | ((ulong)((green) & 0xFFL) << 8) | (ulong)((blue) & 0xFFL)))
#define GCOL_RGB_RED(colour)       ((colour >> 16) & 0xFFL)
#define GCOL_RGB_GREEN(colour)     ((colour >> 8) & 0xFFL)
#define GCOL_RGB_BLUE(colour)      ((colour) & 0xFFL)
#define GCOL_INDEX(index)          (GCOL_TYPE_INDEX | (index & ~GCOL_TYPE_MASK))
#define GCOL_INDEX_INDEX(colour)   (int) ((colour) & ~GCOL_TYPE_MASK)
#define GCOL_INDEXB(index)         (GCOL_TYPE_INDEXB | (index & ~GCOL_TYPE_MASK))
#define GCOL_INDEXB_INDEX(colour)  (int) ((colour) & ~GCOL_TYPE_MASK)
#define GCOL_GRADIENT(index)       (GCOL_TYPE_GRADIENT | (index & ~GCOL_TYPE_MASK))
#define GCOL_GRADIENT_INDEX(colour) (int) ((colour) & ~GCOL_TYPE_MASK)

#define GCOL_NAMED_VALUE(index)    (GCOL_TYPE_NAMED | (index & ~GCOL_TYPE_MASK))
#define GCOL_NAMED_INDEX(colour)   (int) ((colour) & ~GCOL_TYPE_MASK)
#define GCOL_NAMED_NAME(colour)    (EGColourNamed) ((colour) & ~GCOL_TYPE_MASK)
#define GCOL_NONE                  GCOL_TYPE_NONE
#define GCOL_DIALOG                GCOL_TYPE_DIALOG
#define GCOL_UNDEFINED             0xFFFFFFFF



#define GCOL_BLACK                 GCOL_RGB(0x00, 0x00, 0x00)
#define GCOL_WHITE                 GCOL_RGB(0xFF, 0xFF, 0xFF)

class CGPalette : CObjPersistent {
 private:
   void delete(void); 
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
 public:
   ALIAS<"svg:palette">;
   ATTRIBUTE<ARRAY<TGColour> colour, "color"> {
      if (!data) {
                                                     
         ARRAY(&this->colour).used_set(64);
         memset(ARRAY(&this->colour).data(), 0, sizeof(TGColour) * 64);
         ARRAY(&this->colour).data()[0] = GCOL_RGB(0,0,0);
         ARRAY(&this->colour).data()[1] = GCOL_RGB(0,0,255);
         ARRAY(&this->colour).data()[2] = GCOL_RGB(0,255,255);
         ARRAY(&this->colour).data()[3] = GCOL_RGB(0,255,0);
         ARRAY(&this->colour).data()[4] = GCOL_RGB(192,0,192);
         ARRAY(&this->colour).data()[5] = GCOL_RGB(255,0,0);
         ARRAY(&this->colour).data()[6] = GCOL_RGB(255,255,0);
         ARRAY(&this->colour).data()[7] = GCOL_RGB(255,255,255);
         ARRAY(&this->colour).data()[8] = GCOL_RGB(0,0,128);
         ARRAY(&this->colour).data()[9] = GCOL_RGB(0,128,128);
         ARRAY(&this->colour).data()[10] = GCOL_RGB(0,128,0);
         ARRAY(&this->colour).data()[11] = GCOL_RGB(128,0,128);
         ARRAY(&this->colour).data()[12] = GCOL_RGB(128,0,0);
         ARRAY(&this->colour).data()[13] = GCOL_RGB(192,192,0);
         ARRAY(&this->colour).data()[14] = GCOL_RGB(192,192,192);
         ARRAY(&this->colour).data()[15] = GCOL_RGB(128,128,128);
      }
      else {
         ARRAY(&this->colour).data()[attribute_element] = *data;
      }
   };

   void CGPalette(ARRAY<TGColour> *colour);




};

extern bool GCOL_PALETTE_FIND(CObjPersistent *object, CGPalette **palette, CGPalette **paletteB, bool *translate);
TGColour GCOL_CONVERT_RGB(CGPalette *palette, CGPalette *paletteB, TGColour colour);
TGColour GCOL_CONVERT_RGB_OBJECT(CObjPersistent *object, TGColour colour);

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

#define GCOL_STRING_NONE "none"
#define GCOL_STRING_RGB "rgb"
#define GCOL_STRING_INDEX "index"
#define GCOL_STRING_INDEXB "indexB"
#define GCOL_STRING_GRADIENT "gradient"

TGColour GColourNamed[] = {
   0xF0F8FF, 0xFAEBD7, 0x00FFFF, 0x7FFFD4, 0xF0FFFF, 0xF4F4DC, 0xFFE4C4, 0x000000,
   0xFFEBCD, 0x0000FF, 0x822BE2, 0xA52A2A, 0xDEB887, 0x5F9EA0, 0x7FFF00, 0xD2691E,
   0xFF7F50, 0x6495ED, 0xFFF8DC, 0xDC143C, 0x00FFFF, 0x00008B, 0x008B8B, 0xB8860B,
   0xA9A9A9, 0x006400, 0xA9A9A9, 0xBDD76B, 0x8B008B, 0x556B2F, 0xFF8C00, 0x9932CC,
   0x8B0000, 0xE9967A, 0x8FBCFF, 0x483D8B, 0x2F4F4F, 0x2F4F4F, 0x00CED1, 0x9400D3,
   0xFF1493, 0x00BFFF, 0x696969, 0x696969, 0x1E90FF, 0xB22222, 0xFFFAF4, 0x228B22,
   0xFF00FF, 0xDCDCDC, 0xF8F8FF, 0xFFD700, 0xDAA520, 0x808080, 0x808080, 0x008000,
   0xADFF2F, 0xF0FFF0, 0xFF69B4, 0xCD5C5C, 0x4B0082, 0xFFFFF0, 0xF0E68C, 0xE6E6FA,
   0xFFF0F5, 0x7CFC00, 0xFFFACD, 0xADD8E6, 0xF08080, 0xE0FFFF, 0xFAFAD2, 0xD3D3D3,
   0x90EE90, 0xD3D3D3,
   0xFFB6C1, 0xFFA07A, 0x20B2AA, 0x87CEFA, 0x778899, 0x778899, 0xB0C4DE, 0xFFFFE0,
   0x00FF00, 0x32CD32, 0xFAF0E6, 0xFF00FF, 0x800000, 0x66CDAA, 0x0000CD, 0xBA55D3,
   0x9370DB, 0x3CB371, 0x7B68EE, 0x00FA9A, 0x48D1CC, 0xC71585, 0x191970, 0xF5FFFA,
   0xFFE4E1, 0xFFE4B5, 0xFFDEAD, 0x000080, 0xFDF5E6, 0x808000, 0x6B8E23, 0xFFA500,
   0xFF4500, 0xDA70D6, 0xEEE8AA, 0x98FB98, 0xAFEEEE, 0xDB7093, 0xFFEFD5, 0xFFDAB9,
   0xCD853F, 0xFFC0CB, 0xDDA0DD, 0xB0E0E6, 0x800080, 0xFF0000, 0xBC8F8F, 0x4169E1,
   0x8B4513, 0xFA8072, 0xF4A460, 0x2E8B57, 0xFFF5EE, 0xA0522D, 0xC0C0C0, 0x87CEEB,
   0x6A5ACD, 0x708090, 0x708090, 0xFFFAFA, 0x00FF7F, 0x4682B4, 0xD2B48C, 0x008080,
   0xD8BFD8, 0xFF6347, 0x40E0D0, 0xEE82EE, 0xF5DEB3, 0xFFFFFF, 0xF5F5F5, 0xFFFF00,
   0x9ACD32
};

bool ATTRIBUTE:convert<TGColour>(CObjPersistent *object,
                                 const TAttributeType *dest_type, const TAttributeType *src_type,
                                 int dest_index, int src_index,
                                 void *dest, const void *src) {
   TGColour *colour, rgb_colour;
   CString *string;
   int *value;
   int red, green, blue;
   bool channel_float;
   double channel_value;
   int i;
   const char *cp;
   CGPalette *palette, *paletteB;
   bool translate;

   if (dest_type == &ATTRIBUTE:type<TGColour> && src_type == NULL) {
      memset(dest, 0, sizeof(TGColour));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TGColour> && src_type == &ATTRIBUTE:type<TGColour>) {
      *((TGColour *)dest) = *((TGColour *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TGColour>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TGColour> && src_type == &ATTRIBUTE:type<CString>) {
      string = (CString *)src;
      colour = (TGColour *)dest;

      if (CString(string).length() == 0) {
                                        
         return TRUE;
      }

      if (dest_index == -1) {

         if (CString(string).string()[0] == '#') {
                                             
            if (CString(string).length() == 4) {
               sscanf(CString(string).string(), "#%1x%1x%1x", &red, &green, &blue);
               red = red * 255 / 15;
               green = green * 255 / 15;
               blue = blue * 255 / 15;
            }
            else {
               sscanf(CString(string).string(), "#%02x%02x%02x", &red, &green, &blue);
            }
            *colour = GCOL_RGB(red, green, blue);
            return TRUE;
         }

                            
         if (CString(string).strcmp(GCOL_STRING_NONE) == 0) {
            *colour = GCOL_NONE;
            return TRUE;
         }

                           
         if (CString(string).strncmp(GCOL_STRING_RGB, strlen(GCOL_STRING_RGB)) == 0) {
            *colour = GCOL_RGB(0xFF, 0, 0);
            cp = CString(string).strchr('(');
            channel_float = strchr(cp, '%') != NULL;
            red = 0;
            green = 0;
            blue = 0;
            if (cp) {
               cp++;
               if (channel_float) {
                  channel_value = atof(cp) * 255 / 100;
               }
               else {
                  channel_value = atof(cp);
               }
               red = (int)channel_value;
            }
            cp = strchr(cp, ',');
            if (cp) {
               cp++;
               if (channel_float) {
                  channel_value = atof(cp) * 255 / 100;
               }
               else {
                  channel_value = atof(cp);
               }
               green = (int)channel_value;
            }
            cp = strchr(cp, ',');
            if (cp) {
               cp++;
               if (channel_float) {
                  channel_value = atof(cp) * 255 / 100;
               }
               else {
                  channel_value = atof(cp);
               }
               blue = (int)channel_value;
            }
            *colour = GCOL_RGB(red, green, blue);
            return TRUE;
         }

                                      
         if (CString(string).strncmp(GCOL_STRING_INDEXB, strlen(GCOL_STRING_INDEXB)) == 0) {
            cp = CString(string).strchr('(');
            if (!cp) {
               cp = CString(string).strchr('[');
            }
            if (cp) {
               *colour = GCOL_TYPE_INDEXB | atoi(cp + 1);




               return TRUE;
            }
            return FALSE;
         }
                                     
         if (CString(string).strncmp(GCOL_STRING_INDEX, strlen(GCOL_STRING_INDEX)) == 0) {
            cp = CString(string).strchr('(');
            if (!cp) {
               cp = CString(string).strchr('[');
            }
            if (cp) {
               *colour = GCOL_TYPE_INDEX | atoi(cp + 1);
               if (CObject(object).obj_class() != &class(CGPalette) &&
                  GCOL_PALETTE_FIND(object, &palette, &paletteB, &translate) && translate) {
                  *colour = GCOL_CONVERT_RGB(palette, paletteB, *colour);
               }
               return TRUE;
            }
            return FALSE;
         }
                               
         if (CString(string).strncmp(GCOL_STRING_GRADIENT, strlen(GCOL_STRING_GRADIENT)) == 0) {
            cp = CString(string).strchr('(');
            if (!cp) {
               cp = CString(string).strchr('[');
            }
            if (cp) {
               *colour = GCOL_TYPE_GRADIENT | atoi(cp + 1);
               return TRUE;
            }
            return FALSE;
         }

                                       
         for (i = 0; i < EGColourNamed.COUNT(); i++) {
            if (CString(string).strcmp(EGColourNamed(i).name()) == 0) {
               *colour = GCOL_TYPE_NAMED | i;
               return TRUE;
            }
         }
      }
      else {
         i = atoi(CString(string).string());
         if (i < 0) {
            i = 0;
         }
         if (i > 0xFF) {
            i = 0xFF;
         }
         switch (GCOL_TYPE(*colour)) {
         case GCOL_TYPE_RGB:
            rgb_colour = *colour;
            break;
         case GCOL_TYPE_GRADIENT:
            rgb_colour = 0;
            break;
         case GCOL_TYPE_INDEX:
         case GCOL_TYPE_INDEXB:
            rgb_colour = GCOL_CONVERT_RGB_OBJECT(object, *colour);
            break;
         case GCOL_TYPE_NAMED:
            rgb_colour = GColourNamed[GCOL_NAMED_INDEX(*colour)];
            break;
         case GCOL_TYPE_NONE:
            rgb_colour = 0;
            break;
         default:
            return FALSE;
         }

         switch(dest_index) {
         case 0:
            *colour = GCOL_RGB(i,
                               GCOL_RGB_GREEN(rgb_colour),
                               GCOL_RGB_BLUE(rgb_colour));
            break;
         case 1:
            *colour = GCOL_RGB(GCOL_RGB_RED(rgb_colour),
                               i,
                               GCOL_RGB_BLUE(rgb_colour));
            break;
         case 2:
            *colour = GCOL_RGB(GCOL_RGB_RED(rgb_colour),
                               GCOL_RGB_GREEN(rgb_colour),
                               i);
            break;
         default:
            return FALSE;
         }
         return TRUE;
      }
      return FALSE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TGColour>) {
      colour = (TGColour *)src;
      string = (CString *)dest;

      if (src_index == -1) {
         switch (GCOL_TYPE(*colour)) {
         case GCOL_TYPE_RGB:
            CString(string).printf("#%02x%02x%02x",
                                   GCOL_RGB_RED(*colour), GCOL_RGB_GREEN(*colour), GCOL_RGB_BLUE(*colour));
            break;
         case GCOL_TYPE_INDEX:
            CString(string).printf(GCOL_STRING_INDEX"(%d)", GCOL_INDEX_INDEX(*colour));
            return TRUE;
         case GCOL_TYPE_INDEXB:
            CString(string).printf(GCOL_STRING_INDEXB"(%d)", GCOL_INDEXB_INDEX(*colour));
            return TRUE;
         case GCOL_TYPE_GRADIENT:
            CString(string).printf(GCOL_STRING_GRADIENT"(%d)", GCOL_GRADIENT_INDEX(*colour));
            return TRUE;
         default:
            CString(string).printf("?");
            return FALSE;
         case GCOL_TYPE_NAMED:
                                              
            CString(string).printf("%s", EGColourNamed((*colour) & ~GCOL_TYPE_MASK).name());
            break;
         case GCOL_TYPE_NONE:
            CString(string).printf(GCOL_STRING_NONE);
            break;
         }
         return TRUE;
      }
      else {
         switch (GCOL_TYPE(*colour)) {
         case GCOL_TYPE_RGB:
            rgb_colour = *colour;
            break;
         case GCOL_TYPE_INDEX:
         case GCOL_TYPE_INDEXB:
            rgb_colour = GCOL_CONVERT_RGB_OBJECT(object, *colour);
            break;
         case GCOL_TYPE_NAMED:
            rgb_colour = GColourNamed[GCOL_NAMED_INDEX(*colour)];
            break;
         case GCOL_TYPE_NONE:
            rgb_colour = 0;
            break;
         default:
            return FALSE;
         }

         switch (src_index) {
         case 0:
            CString(string).printf("%d", GCOL_RGB_RED(rgb_colour));
            break;
         case 1:
            CString(string).printf("%d", GCOL_RGB_GREEN(rgb_colour));
            break;
         case 2:
            CString(string).printf("%d", GCOL_RGB_BLUE(rgb_colour));
            break;
         default:
            return FALSE;
         }
         return TRUE;
      }
   }

   if (dest_type == &ATTRIBUTE:type<TGColour> && src_type == &ATTRIBUTE:type<int>) {
      value = (int *)src;
      colour = (TGColour *)dest;

      switch (GCOL_TYPE(*colour)) {
      case GCOL_TYPE_RGB:
         rgb_colour = *colour;
         break;
      case GCOL_TYPE_INDEX:
      case GCOL_TYPE_INDEXB:
         rgb_colour = GCOL_CONVERT_RGB_OBJECT(object, *colour);
         break;
      case GCOL_TYPE_NAMED:
         rgb_colour = GColourNamed[GCOL_NAMED_INDEX(*colour)];
         break;
      case GCOL_TYPE_NONE:
         rgb_colour = 0;
         break;
      default:
         return FALSE;
      }

      switch(dest_index) {
      case -1:
         *colour = GCOL_RGB(*value, *value, *value);
         break;
      case 0:
         *colour = GCOL_RGB(*value,
                            GCOL_RGB_GREEN(rgb_colour),
                            GCOL_RGB_BLUE(rgb_colour));
         break;
      case 1:
         *colour = GCOL_RGB(GCOL_RGB_RED(rgb_colour),
                            *value,
                            GCOL_RGB_BLUE(rgb_colour));
         break;
      case 2:
         *colour = GCOL_RGB(GCOL_RGB_RED(rgb_colour),
                            GCOL_RGB_GREEN(rgb_colour),
                            *value);
         break;
      default:
         return FALSE;
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<TGColour>) {
      colour = (TGColour *)src;
      value = (int *)dest;

      switch (GCOL_TYPE(*colour)) {
      case GCOL_TYPE_RGB:
         rgb_colour = *colour;
         break;
      case GCOL_TYPE_INDEX:
      case GCOL_TYPE_INDEXB:
         rgb_colour = GCOL_CONVERT_RGB_OBJECT(object, *colour);
         break;
      case GCOL_TYPE_NAMED:
         rgb_colour = GColourNamed[GCOL_NAMED_INDEX(*colour)];
         break;
      case GCOL_TYPE_NONE:
         rgb_colour = 0;
         break;
      default:
         return FALSE;
      }

      switch(src_index) {
      case -1:
         *value = (GCOL_RGB_RED(rgb_colour) + GCOL_RGB_GREEN(rgb_colour) + GCOL_RGB_BLUE(rgb_colour)) / 3;
         break;
      case 0:
         *value = GCOL_RGB_RED(rgb_colour);
         break;
      case 1:
         *value = GCOL_RGB_GREEN(rgb_colour);
         break;
      case 2:
         *value = GCOL_RGB_BLUE(rgb_colour);
         break;
      default:
         return FALSE;
      }
      return TRUE;
   }

   return FALSE;
}

void CGPalette::CGPalette(ARRAY<TGColour> *palette) {
   ARRAY(&this->colour).new();
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPalette,colour>, TRUE);




}                        

void CGPalette::delete(void) {
   ARRAY(&this->colour).delete();
}                     

TGColour GCOL_CONVERT_RGB_OBJECT(CObjPersistent *object, TGColour colour) {
   ARRAY<TGColour> *palette = NULL, *paletteB = NULL;
   const TAttribute *attribute, *attributeB;

   attribute = CObjPersistent(object).attribute_find("palette");
   if (attribute) {
      palette = (ARRAY<TGColour> *)(((byte *)object) + attribute->offset);
   }
   attributeB = CObjPersistent(object).attribute_find("paletteB");
   if (attributeB) {
      paletteB = (ARRAY<TGColour> *)(((byte *)object) + attributeB->offset);
   }

   switch (GCOL_TYPE(colour)) {
   case GCOL_TYPE_INDEX:
      if (!palette) {
         return GCOL_BLACK;
      }
      if (GCOL_INDEX_INDEX(colour) >= ARRAY(palette).count()) {
         return GCOL_BLACK;
      }
      if (GCOL_TYPE(ARRAY(palette).data()[GCOL_INDEX_INDEX(colour)]) == GCOL_TYPE_RGB) {
         return ARRAY(palette).data()[GCOL_INDEX_INDEX(colour)];
      }
      return GCOL_BLACK;
   case GCOL_TYPE_INDEXB:
      if (!paletteB) {
         return GCOL_BLACK;
      }
      if (GCOL_INDEXB_INDEX(colour) >= ARRAY(paletteB).count()) {
         return GCOL_BLACK;
      }
      if (GCOL_TYPE(ARRAY(paletteB).data()[GCOL_INDEX_INDEX(colour)]) == GCOL_TYPE_RGB) {
         return ARRAY(paletteB).data()[GCOL_INDEX_INDEX(colour)];
      }
      return GCOL_BLACK;
   case GCOL_TYPE_NAMED:
      return GColourNamed[GCOL_NAMED_NAME(colour)];
   case GCOL_TYPE_DIALOG:
   default:
      return colour;
   }
}

TGColour GCOL_CONVERT_RGB(CGPalette *palette, CGPalette *paletteB, TGColour colour) {
   switch (GCOL_TYPE(colour)) {
   case GCOL_TYPE_INDEX:
      if (!palette) {
         return GCOL_BLACK;
      }
      if (GCOL_INDEX_INDEX(colour) >= ARRAY(&palette->colour).count()) {
         return GCOL_BLACK;
      }
      if (GCOL_TYPE(ARRAY(&palette->colour).data()[GCOL_INDEX_INDEX(colour)]) != GCOL_TYPE_INDEX) {
          return GCOL_CONVERT_RGB(palette, paletteB, ARRAY(&palette->colour).data()[GCOL_INDEX_INDEX(colour)]);
      }
      return GCOL_BLACK;
   case GCOL_TYPE_INDEXB:
      if (!paletteB) {
         return GCOL_BLACK;
      }
      if (GCOL_INDEX_INDEX(colour) >= ARRAY(&paletteB->colour).count()) {
         return GCOL_BLACK;
      }
      if (GCOL_TYPE(ARRAY(&paletteB->colour).data()[GCOL_INDEX_INDEX(colour)]) != GCOL_TYPE_INDEX) {
          return GCOL_CONVERT_RGB(palette, paletteB, ARRAY(&paletteB->colour).data()[GCOL_INDEX_INDEX(colour)]);
      }
      return GCOL_BLACK;
   case GCOL_TYPE_NAMED:
      return GColourNamed[GCOL_NAMED_NAME(colour)];
   case GCOL_TYPE_DIALOG:
   default:
      return colour;
   }
}

                                                                              
MODULE::END                                                                   
                                                                              
# 20 "/home/jacob/keystone/src/graphics/gobject.c"
# 1 "/home/jacob/keystone/src/graphics/gcoord.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              



                                                                              
MODULE::INTERFACE                                                             
                                                                              





typedef double TCoord;
ARRAY:typedef<TCoord>;
ATTRIBUTE:typedef<TCoord>;

typedef struct {
   TCoord x, y;
} TPoint;
ARRAY:typedef<TPoint>;
ATTRIBUTE:typedef<TPoint>;

typedef struct {
   TPoint point[2];
} TRect;
ATTRIBUTE:typedef<TRect>;

static inline bool TRect_overlap(TRect *a, TRect *b) {
   return !(a->point[1].x < b->point[0].x ||
            a->point[0].x > b->point[1].x ||
            a->point[1].y < b->point[0].y ||
            a->point[0].y > b->point[1].y);
}                 

ENUM:typedef<EGTransform> {
   {none}, {matrix}, {translate}, {scale}, {rotate}, {skewX}, {skewY},
};

typedef struct {
   EGTransform type;
   union {
      struct {
         double data[2][3];
      } matrix;
      struct {
         double tx, ty;
         bool ty_implied;
      } translate;
      struct {
         double sx, sy;
         bool sy_implied;
      } scale;
      struct {
         double rotate_angle;
         double cx, cy;
         bool cxy_used;
      } rotate;
      struct {
         double skew_angle;
      } skew;
   } t;
} TGTransform;

ARRAY:typedef<TGTransform>;
ATTRIBUTE:typedef<TGTransform>;

typedef TGTransform TGTransformData;
ATTRIBUTE:typedef<TGTransformData>;

typedef struct {
   double data[3][3];
} TGMatrix;

typedef struct {
   TGMatrix matrix;
   TPoint scaling;
   double rotation;
} TGCoordState;

STACK:typedef<TGCoordState>;

class CGCoordSpace : CObject {
 private:
   TGMatrix matrix;
   TPoint scaling;
   double rotation;
   STACK<TGCoordState> state_stack;

   void new(void);
   void delete(void);

   double vector_difference_angle(TPoint *v1, TPoint *v2);


   
   void matrix_invert(TGMatrix *matrix, TGMatrix *result);
   void matrix_multiply(TGMatrix *matrix, bool prepend);

   void scaling_recalc(void);
 public:
   void CGCoordSpace(void);
   void reset(void);
   void push(void);
   void pop(void);
   void transform_apply(TGTransform *transform, bool prepend);
   void transform_get_current(TGTransform *result);
   void point_array_utov(int count, TPoint *point);
   void point_array_vtou(int count, TPoint *point);
   static inline void point_utov(TPoint *point);
   static inline void point_vtou(TPoint *point);

   double measure_vector(TPoint *vector);
   static inline double scaling_x(void);
   static inline double scaling_y(void);
   static inline double scaling(void);
   static inline double rotation(void);

   void arc_endpoints(double cx, double cy, double rx, double ry,
                      double x_axis_rotation, double th0, double thd,
                      double *out_startx, double *out_starty, 
                      double *out_endx, double *out_endy, 
                      int *out_large_arc_flag, int *out_sweep_flag);
   void arc_position(double curx, double cury, double rx, double ry,
                     double x_axis_rotation, int large_arc_flag, int sweep_flag, double x, double y,
                     double *out_cx, double *out_cy, double *out_rx, double *out_ry,
                     double *out_th0, double *out_thd);

   void extent_reset(TRect *rect);
   bool extent_empty(TRect *rect);
   void extent_add(TRect *rect, int count, TPoint *point);
   void extent_extend(TRect *rect, TCoord extend);

   bool area_inside_rect(ARRAY<TPoint> *area, TRect *rect);
   bool rect_inside_area(TRect *rect, ARRAY<TPoint> *area);
};

static inline void CGCoordSpace::point_utov(TPoint *point) {
   CGCoordSpace(this).point_array_utov(1, point);
}

static inline void CGCoordSpace::point_vtou(TPoint *point) {
   CGCoordSpace(this).point_array_vtou(1, point);
}

static inline double CGCoordSpace::scaling_x(void) {
   return this->scaling.x;
}

static inline double CGCoordSpace::scaling_y(void) {
   return this->scaling.y;
}

static inline double CGCoordSpace::rotation(void) {
   return this->rotation;
}

static inline double CGCoordSpace::scaling(void) {
   return this->scaling.x > this->scaling.y ? this->scaling.y : this->scaling.x;
}
                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

#include <math.h>




bool ATTRIBUTE:convert<TCoord>(CObjPersistent *object,
                                const TAttributeType *dest_type, const TAttributeType *src_type,
                                int dest_index, int src_index,
                                void *dest, const void *src) {
   TCoord *value;
   CString *string;
   int *int_value;
   double *double_value;

   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == NULL) {
      memset(dest, 0, sizeof(TCoord));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == &ATTRIBUTE:type<TCoord>) {
      *((TCoord *)dest) = *((TCoord *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TCoord>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (TCoord *)dest;
      string = (CString *)src;
      *value = atof(CString(string).string());
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == &ATTRIBUTE:type<int>) {
      value  = (TCoord *)dest;
      int_value = (int *)src;
      *value = *int_value;
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<TCoord>) {
      int_value = (int *)dest;
      value  = (TCoord *)src;
      *int_value = (int)*value;
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TCoord> && src_type == &ATTRIBUTE:type<double>) {
      value  = (TCoord *)dest;
      double_value = (double *)src;
      *value = *double_value;
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<double> && src_type == &ATTRIBUTE:type<TCoord>) {
      double_value = (double *)dest;
      value = (TCoord *)src;
      *double_value = (double)*value;
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TCoord>) {
      value  = (TCoord *)src;
      string = (CString *)dest;
      CString(string).printf("%g", *value);
      return TRUE;
   }
   return FALSE;
}

bool ATTRIBUTE:convert<TPoint>(CObjPersistent *object,
                               const TAttributeType *dest_type, const TAttributeType *src_type,
                               int dest_index, int src_index,
                               void *dest, const void *src) {
   TPoint *point;
   CString *string;
   const char *cp;

   if (dest_type == &ATTRIBUTE:type<TPoint> && src_type == NULL) {
      memset(dest, 0, sizeof(TPoint));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TPoint> && src_type == &ATTRIBUTE:type<TPoint>) {
      *((TPoint *)dest) = *((TPoint *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TPoint>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TPoint> && src_type == &ATTRIBUTE:type<CString>) {
      point  = (TPoint *)dest;
      string = (CString *)src;
      point->x = atof(CString(string).string());
      point->y = 0;
      cp = strchr(CString(string).string(), ',');
      if (!cp) {
         cp = strchr(CString(string).string(), ' ');
      }
      if (cp) {
         point->y = atof(cp + 1);
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TPoint>) {
      point  = (TPoint *)src;
      string = (CString *)dest;
      CString(string).printf("%g,%g", point->x, point->y);
      return TRUE;
   }
   return FALSE;
}

bool ATTRIBUTE:convert<TRect>(CObjPersistent *object,
                              const TAttributeType *dest_type, const TAttributeType *src_type,
                              int dest_index, int src_index,
                              void *dest, const void *src) {
   TRect *rect;
   CString *string;
   TCoord value;

   if (dest_type == &ATTRIBUTE:type<TRect> && src_type == NULL) {
      memset(dest, 0, sizeof(TRect));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TRect> && src_type == &ATTRIBUTE:type<TRect>) {
      *((TRect *)dest) = *((TRect *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TRect>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TRect> && src_type == &ATTRIBUTE:type<CString>) {
      rect   = (TRect *)dest;
      string = (CString *)src;
      if (dest_index == -1) {
         sscanf(CString(string).string(), "%lf %lf %lf %lf",
                &rect->point[0].x, &rect->point[0].y, &rect->point[1].x, &rect->point[1].y);
      }
      else {
         value = atof(CString(string).string());
         switch (dest_index) {
         case 0:
            rect->point[0].x = value;
            break;
         case 1:
            rect->point[0].y = value;
            break;
         case 2:
            rect->point[1].x = value;
            break;
         case 3:
            rect->point[1].y = value;
            break;
         }

      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TRect>) {
      rect   = (TRect *)src;
      string = (CString *)dest;
      if (src_index == -1) {
         CString(string).printf("%g %g %g %g", rect->point[0].x, rect->point[0].y,
                                                rect->point[1].x, rect->point[1].y);
      }
      else {
         switch (src_index) {
         case 0:
            value = rect->point[0].x;
            break;
         case 1:
            value = rect->point[0].y;
            break;
         case 2:
            value = rect->point[1].x;
            break;
         case 3:
            value = rect->point[1].y;
            break;
         default:
            value = 0;
            break;
         }
         CString(string).printf("%g", value);
      }
      return TRUE;
   }
   return FALSE;
}

static bool TGTransformData_string_read(TGTransform *transform, CString *string) {
   int i, j;
   CString value;
   bool result;

   new(&value).CString(NULL);

   result = TRUE;
   switch (transform->type) {
   case EGTransform.none:
      break;
   case EGTransform.matrix:
      for (j = 0; j < 2; j++) {
         for (i = 0; i < 3; i++) {
            if (!result || !CString(string).tokenise(&value, ",", TRUE)) {
               result = FALSE;
               break;
            }
            transform->t.matrix.data[j][i]  = (double) atof(CString(&value).string());
         }
      }
      break;
   case EGTransform.translate:
      if (!CString(string).tokenise(&value, ",", TRUE)) {
         result = FALSE;
         break;
      }
      transform->t.translate.tx = (double) atof(CString(&value).string());
      if (CString(string).tokenise(&value, ",", TRUE)) {
         transform->t.translate.ty = (double) atof(CString(&value).string());
         transform->t.translate.ty_implied = FALSE;
      }
      else {
         transform->t.translate.ty = 0;
         transform->t.translate.ty_implied = TRUE;
      }
      break;
   case EGTransform.scale:
      if (!CString(string).tokenise(&value, ",", TRUE)) {
         result = FALSE;
         break;
      }
      transform->t.scale.sx = (double) atof(CString(&value).string());
      if (CString(string).tokenise(&value, ",", TRUE)) {
         transform->t.scale.sy = (double) atof(CString(&value).string());
         transform->t.scale.sy_implied = FALSE;
      }
      else {
         transform->t.scale.sy = transform->t.scale.sx;
         transform->t.scale.sy_implied = TRUE;
      }
      break;
   case EGTransform.rotate:
      if (!CString(string).tokenise(&value, ",", TRUE)) {
         result = FALSE;
         break;
      }
      transform->t.rotate.rotate_angle = (double) atof(CString(&value).string());
      if (CString(string).tokenise(&value, ",", TRUE)) {
         transform->t.rotate.cx = (double) atof(CString(&value).string());
         if (CString(string).tokenise(&value, ",", TRUE)) {
            transform->t.rotate.cy = (double) atof(CString(&value).string());
         }
         else {
            result = FALSE;
            break;
         }
      transform->t.rotate.cxy_used = TRUE;
      }
      else {
         transform->t.rotate.cxy_used = FALSE;
      }
      break;
   case EGTransform.skewX:
   case EGTransform.skewY:
      if (!CString(string).tokenise(&value, ",", TRUE)) {
         result = FALSE;
         break;
      }
      transform->t.skew.skew_angle = (double) atof(CString(&value).string());
      break;
   default:
      result = FALSE;
      break;
   }

   delete(&value);

   return result;
}                               

static void TGTransformData_string_write(TGTransform *transform, CString *string) {
   switch (transform->type) {
   case EGTransform.none:
      break;
   case EGTransform.matrix:
      CString(string).printf_append("%g,%g,%g,%g,%g,%g",
                                    transform->t.matrix.data[0][0], transform->t.matrix.data[0][1], transform->t.matrix.data[0][2],
                                    transform->t.matrix.data[1][0], transform->t.matrix.data[1][1], transform->t.matrix.data[1][2]);
      break;
   case EGTransform.translate:
      CString(string).printf_append("%g", transform->t.translate.tx);
      if (!transform->t.translate.ty_implied) {
         CString(string).printf_append(",%g", transform->t.translate.ty);
      }
      break;
   case EGTransform.scale:
      CString(string).printf_append("%g", transform->t.scale.sx);
      if (!transform->t.scale.sy_implied) {
         CString(string).printf_append(",%g", transform->t.scale.sy);
      }
      break;
   case EGTransform.rotate:
      CString(string).printf_append("%g", transform->t.rotate.rotate_angle);
      if (transform->t.rotate.cxy_used) {
         CString(string).printf_append(",%g,%g",
                                       transform->t.rotate.cx, transform->t.rotate.cy);
      }
      break;
   case EGTransform.skewX:
      CString(string).printf_append("%g", transform->t.skew.skew_angle);
      break;
   case EGTransform.skewY:
      CString(string).printf_append("%g", transform->t.skew.skew_angle);
      break;
   }
}                                

bool ATTRIBUTE:convert<TGTransform>(CObjPersistent *object,
                                    const TAttributeType *dest_type, const TAttributeType *src_type,
                                    int dest_index, int src_index,
                                    void *dest, const void *src) {
   TGTransform *transform;
   CString *string, data;
   char *t_begin, *t_end;
   bool result;
   int *int_value;

   if (dest_type == &ATTRIBUTE:type<TGTransform> && src_type == NULL) {
      memset(dest, 0, sizeof(TGTransform));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TGTransform> && src_type == &ATTRIBUTE:type<TGTransform>) {
      *((TGTransform *)dest) = *((TGTransform *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TGTransform>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TGTransform> && src_type == &ATTRIBUTE:type<int>) {
      transform = (TGTransform *)dest;
      int_value = (int *)src;

      switch (transform->type) {
      case EGTransform.translate:
         switch (dest_index) {
         case 0:
            transform->t.translate.tx = (double) *int_value;
            return TRUE;
         case 1:
            transform->t.translate.ty = (double) *int_value;
            return TRUE;
         }
      default:
         break;
      }
      return FALSE;
   }

   if (dest_type == &ATTRIBUTE:type<TGTransform> && src_type == &ATTRIBUTE:type<CString>) {
      transform = (TGTransform *)dest;
      string = (CString *)src;

      t_begin = CString(string).strchr('(');
      t_end   = CString(string).strchr(')');
      if (!t_begin || !t_end) {
         return FALSE;
      }

      new(&data).CString(NULL);
      CString(string).extract(&data, CString(string).string(), t_begin - 1);
      transform->type = ENUM:decode(EGTransform, CString(&data).string());
      CString(string).extract(&data, t_begin + 1, t_end - 1);

      result = TGTransformData_string_read(transform, &data);
      delete(&data);
      return result;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TGTransform>) {
      transform = (TGTransform *)src;
      string = (CString *)dest;

      CString(string).printf("%s", EGTransform(transform->type).name());
                             

      CString(string).printf_append("(");

      TGTransformData_string_write(transform, string);

      CString(string).printf_append(")");
      return TRUE;
   }
   return FALSE;
}

bool ATTRIBUTE:convert<TGTransformData>(CObjPersistent *object,
                                        const TAttributeType *dest_type, const TAttributeType *src_type,
                                        int dest_index, int src_index,
                                        void *dest, const void *src) {
   TGTransform *transform;
   CString *string;

   if (dest_type == &ATTRIBUTE:type<TGTransformData> && src_type == NULL) {
      memset(dest, 0, sizeof(TGTransformData));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TGTransformData> && src_type == &ATTRIBUTE:type<TGTransformData>) {
      memcpy(dest, src, sizeof(TGTransformData));
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TGTransformData>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TGTransformData> && src_type == &ATTRIBUTE:type<CString>) {
      transform = (TGTransform *)dest;
      string = (CString *)src;

      return TGTransformData_string_read(transform, string);
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TGTransformData>) {
      transform = (TGTransform *)src;
      string = (CString *)dest;

      TGTransformData_string_write(transform, string);
      return TRUE;
   }
   return FALSE;
};

void CGCoordSpace::new(void) {
   STACK(&this->state_stack).new();
}                     

void CGCoordSpace::CGCoordSpace(void) {
   CGCoordSpace(this).reset();
}                              

void CGCoordSpace::delete(void) {
   STACK(&this->state_stack).delete();
}                        

void CGCoordSpace::reset(void) {
                      
   this->matrix.data[0][0] = 1; this->matrix.data[0][1] = 0; this->matrix.data[0][2] = 0;
   this->matrix.data[1][0] = 0; this->matrix.data[1][1] = 1; this->matrix.data[1][2] = 0;
   this->matrix.data[2][0] = 0; this->matrix.data[2][1] = 0; this->matrix.data[2][2] = 1;

   this->rotation = 0;
}                       

void CGCoordSpace::push(void) {
   TGCoordState state;

   state.matrix = this->matrix;
   state.scaling = this->scaling;
   state.rotation = this->rotation;

   STACK(&this->state_stack).push(state);
}                      

void CGCoordSpace::pop(void) {
   TGCoordState state;

   state = STACK(&this->state_stack).pop();
   this->rotation  = state.rotation;   
   this->scaling = state.scaling;   
   this->matrix = state.matrix;
}                     

double CGCoordSpace::vector_difference_angle(TPoint *v1, TPoint *v2) {
   double result;
   
   result = (atan2(v2->y,v2->x) - atan2(v1->y,v1->x)) * (360 / (2*PI));
   
   return result;
}                                         

void CGCoordSpace::matrix_invert(TGMatrix *matrix, TGMatrix *result) {
   int i, j, k;
   double e;

   memcpy(result, matrix, sizeof(TGMatrix));

   for (k = 0; k < 3; k++) {
      e = result->data[k][k];
      result->data[k][k] = 1.0;
      if (e == 0.0) {
         ASSERT("CGCoordSpace::matrix_invert - Matrix inversion error");
       }
      for (j = 0; j < 3; j++) {
         result->data[j][k] = result->data[j][k] / e;
      }
      for (i = 0; i < 3; i++) {
         if (i != k) {
            e = result->data[k][i];
            result->data[k][i] = 0.0;
            for (j = 0; j < 3; j++) {
               result->data[j][i] = result->data[j][i] - e * result->data[j][k];
            }
         }
      }
   }
}                               

void CGCoordSpace::matrix_multiply(TGMatrix *matrix, bool prepend) {
   int x, y, i;
   TGMatrix result;

   CLEAR(&result);

   for (y = 0; y < 3; y++) {
      for (x = 0; x < 3; x++) {
         for (i = 0; i < 3; i++) {
            if (prepend) {
               result.data[y][x] += matrix->data[i][x] * this->matrix.data[y][i];
            }
            else {
               result.data[y][x] += this->matrix.data[i][x] * matrix->data[y][i];
            }
         }
      }
   }

   memcpy(&this->matrix, &result, sizeof(TGMatrix));
}                                 

void CGCoordSpace::transform_apply(TGTransform *transform, bool prepend) {
   TGMatrix matrix;
   TGTransform temp;

   CLEAR(&matrix);
   matrix.data[2][2] = 1;
   switch (transform->type) {
   case EGTransform.none:
      return;                                              
   case EGTransform.matrix:
      matrix.data[0][0] = transform->t.matrix.data[0][0];
      matrix.data[0][1] = transform->t.matrix.data[0][1];
      matrix.data[0][2] = transform->t.matrix.data[0][2];
      matrix.data[1][0] = transform->t.matrix.data[1][0];
      matrix.data[1][1] = transform->t.matrix.data[1][1];
      matrix.data[1][2] = transform->t.matrix.data[1][2];
      break;
   case EGTransform.translate:
      matrix.data[0][0] = 1;
      matrix.data[1][1] = 1;
      matrix.data[0][2] = transform->t.translate.tx;
      matrix.data[1][2] = transform->t.translate.ty;
      break;
   case EGTransform.scale:
      matrix.data[0][0] = transform->t.scale.sx;
      matrix.data[1][1] = transform->t.scale.sy;
      break;
   case EGTransform.rotate:
      temp.type = EGTransform.translate;
      if (transform->t.rotate.cxy_used) {
         temp.t.translate.tx = prepend ? transform->t.rotate.cx : -transform->t.rotate.cx;
         temp.t.translate.ty = prepend ? transform->t.rotate.cy : -transform->t.rotate.cy;
         CGCoordSpace(this).transform_apply(&temp, prepend);
      }
      
      this->rotation += transform->t.rotate.rotate_angle;      

      matrix.data[0][0] = (double)  cos(transform->t.rotate.rotate_angle * (2*PI) / 360);
      matrix.data[0][1] = (double) -sin(transform->t.rotate.rotate_angle * (2*PI) / 360);
      matrix.data[1][0] = (double)  sin(transform->t.rotate.rotate_angle * (2*PI) / 360);
      matrix.data[1][1] = (double)  cos(transform->t.rotate.rotate_angle * (2*PI) / 360);

      CGCoordSpace(this).matrix_multiply(&matrix, prepend);

      if (transform->t.rotate.cxy_used) {
         temp.t.translate.tx = prepend ? -transform->t.rotate.cx : transform->t.rotate.cx;
         temp.t.translate.ty = prepend ? -transform->t.rotate.cy : transform->t.rotate.cy;
         CGCoordSpace(this).transform_apply(&temp, prepend);
      }
      return;
   case EGTransform.skewX:
      matrix.data[0][0] = 1;
      matrix.data[1][1] = 1;
      matrix.data[0][1] = (double) tan(transform->t.skew.skew_angle * (2*PI) / 360);
      break;
   case EGTransform.skewY:
      matrix.data[0][0] = 1;
      matrix.data[1][1] = 1;
      matrix.data[1][0] = (double) tan(transform->t.skew.skew_angle * (2*PI) / 360);
      break;
   }

   CGCoordSpace(this).matrix_multiply(&matrix, prepend);
}                                 

void CGCoordSpace::transform_get_current(TGTransform *result) {
   CLEAR(result);
   result->type = EGTransform.matrix;
   
   result->t.matrix.data[0][0] = this->matrix.data[0][0];
   result->t.matrix.data[0][1] = this->matrix.data[0][1];
   result->t.matrix.data[0][2] = this->matrix.data[0][2];
   result->t.matrix.data[1][0] = this->matrix.data[1][0];
   result->t.matrix.data[1][1] = this->matrix.data[1][1];
   result->t.matrix.data[1][2] = this->matrix.data[1][2];
}                                       

void CGCoordSpace::scaling_recalc(void) {
   TPoint point[2];

   point[0].x = 0;
   point[0].y = 0;
   point[1].x = 1000;
   point[1].y = 0;
   this->scaling.x = CGCoordSpace(this).measure_vector(point) / 1000;

   point[0].x = 0;
   point[0].y = 0;
   point[1].x = 0;
   point[1].y = 1000;
   this->scaling.y = CGCoordSpace(this).measure_vector(point) / 1000;
}                                

double CGCoordSpace::measure_vector(TPoint *point) {
   double x_length, y_length, length;

   CGCoordSpace(this).point_array_utov(2, point);
   x_length = (double) (point[1].x - point[0].x);
   y_length = (double) (point[1].y - point[0].y);
   length = (double)sqrt((x_length * x_length) + (y_length * y_length));

   return length;
}                                

void CGCoordSpace::point_array_utov(int count, TPoint *point) {
   TPoint result;

   while (count) {
      result.x = (point->x * this->matrix.data[0][0]) +
                 (point->y * this->matrix.data[0][1]) +
                 (1        * this->matrix.data[0][2]);
      result.y = (point->x * this->matrix.data[1][0]) +
                 (point->y * this->matrix.data[1][1]) +
                 (1        * this->matrix.data[1][2]);
      *point = result;
      point++;
      count--;
   }
}                                  

void CGCoordSpace::point_array_vtou(int count, TPoint *point) {
   TPoint result;
   TGMatrix inverse;

   CGCoordSpace(this).matrix_invert(&this->matrix, &inverse);

   while (count) {
      result.x = (point->x * inverse.data[0][0]) +
                  (point->y * inverse.data[0][1]) +
                  (1        * inverse.data[0][2]);
      result.y = (point->x * inverse.data[1][0]) +
                  (point->y * inverse.data[1][1]) +
                  (1        * inverse.data[1][2]);
      *point = result;
      point++;
      count--;
   }
}                                  

void CGCoordSpace::arc_endpoints(double cx, double cy, double rx, double ry,
                                 double x_axis_rotation, double th0, double thd,
                                 double *out_startx, double *out_starty, 
                                 double *out_endx, double *out_endy, 
                                 int *out_large_arc_flag, int *out_sweep_flag) {
   *out_startx = (sin(th0 * (PI / 180.0)) * rx) + cx;
   *out_starty = -(cos(th0 * (PI / 180.0)) * ry) + cy;

   *out_endx = (sin((th0 + thd) * (PI / 180.0)) * rx) + cx;
   *out_endy = -(cos((th0 + thd) * (PI / 180.0)) * ry) + cy;   
   
   *out_large_arc_flag = thd > 180;
   *out_sweep_flag = 1;
}                               

void CGCoordSpace::arc_position(double curx, double cury, double rx, double ry,
                                double x_axis_rotation, int large_arc_flag, int sweep_flag, double x, double y,
                                double *out_cx, double *out_cy, double *out_rx, double *out_ry,
                                double *out_th0, double *out_thd) {
   double sin_th, cos_th;
   double a00, a01, a10, a11;
   double dotp, mag;
   double x0, y0, x1, y1, xc, yc;
   double d;

   sin_th = sin (x_axis_rotation * (PI / 180.0));
   cos_th = cos (x_axis_rotation * (PI / 180.0));
   a00 = (curx - x) / 2;
   a01 = (cury - y) / 2;
   x1 = (a00 * cos_th) + (a01 * sin_th);
   y1 = (a00 * -sin_th) + (a01 * cos_th);

                      
   d = (x1*x1)/(rx*rx) + (y1*y1)/(ry*ry);
   if (d > 1) {
      rx = sqrt(d) * rx;
      ry = sqrt(d) * ry;
   }
                           
   d = ((rx*rx)*(ry*ry)) - ((rx*rx)*(y1*y1)) - ((ry*ry)*(x1*x1));
   d /= ((rx*rx)*(y1*y1)) + ((ry*ry)*(x1*x1));
   d = d < 0 ? 0 : d;               
   d = sqrt(d);
   if (sweep_flag == large_arc_flag) {
      d = -d;
   }
   x0 = ((rx*y1)/ry) * d;
   y0 = -((ry*x1)/rx) * d;

   xc = (cos_th * x0) + (-sin_th * y0);
   yc = (sin_th * x0) + (cos_th * y0);
   xc += (curx + x) / 2;
   yc += (cury + y) / 2;
   *out_cx = xc;
   *out_cy = yc;

                                        
   a00 = 1;
   a01 = 0;
   a10 = (x1-x0)/rx;
   a11 = (y1-y0)/ry;
   dotp = a00*a10 + a01*a11;
   mag = sqrt(a00*a00 + a01*a01) * sqrt(a10*a10 + a11*a11);
   d = dotp/mag > -1 ? dotp/mag : -1;
   *out_th0 = acos(d) / (PI / 180);
   if ((a00*a11 - a01*a10) < 0) {
      *out_th0 = -*out_th0;
   }
   a00 = (x1-x0)/rx;
   a01 = (y1-y0)/ry;
   a10 = (-x1-x0)/rx;
   a11 = (-y1-y0)/ry;
   dotp = a00*a10 + a01*a11;
   mag = sqrt(a00*a00 + a01*a01) * sqrt(a10*a10 + a11*a11);

   d = dotp/mag > -1 ? dotp/mag : -1;
   *out_thd = acos(d) / (PI / 180);
   if ((a00*a11 - a01*a10) < 0) {
      *out_thd = -*out_thd;
   }
   if (!sweep_flag && *out_thd > 0) {
      *out_thd -= 360;
   }
   if (sweep_flag && *out_thd < 0) {
      *out_thd += 360;
   }

  *out_rx = rx;
  *out_ry = ry;
}                              

void CGCoordSpace::extent_reset(TRect *rect) {
                                                                
   rect->point[0].x = 10000;
   rect->point[0].y = 10000;
   rect->point[1].x = -10000;
   rect->point[1].y = -10000;
}                              

bool CGCoordSpace::extent_empty(TRect *rect) {
   return (rect->point[0].x == 10000 &&
            rect->point[0].y == 10000 &&
            rect->point[1].x == -10000 &&
            rect->point[1].y == -10000);
}                              

void CGCoordSpace::extent_add(TRect *rect, int count, TPoint *point) {
   int i;

   for (i = 0; i < count; i++) {
      if (point[i].x < rect->point[0].x) {
         rect->point[0].x = point[i].x;
      }
      if (point[i].x > rect->point[1].x) {
         rect->point[1].x = point[i].x;
      }
      if (point[i].y < rect->point[0].y) {
         rect->point[0].y = point[i].y;
      }
      if (point[i].y > rect->point[1].y) {
         rect->point[1].y = point[i].y;
      }
   }
}                            

void CGCoordSpace::extent_extend(TRect *rect, TCoord extent) {
   rect->point[0].x -= extent;
   rect->point[0].y -= extent;
   rect->point[1].x += extent;
   rect->point[1].y += extent;
}                               

bool CGCoordSpace::area_inside_rect(ARRAY<TPoint> *area, TRect *rect) {
   if (ARRAY(area).count() == 0 || CGCoordSpace(this).extent_empty(rect)) {
      return FALSE;
   }
   else {
                                                   
      if (rect->point[0].x <= ARRAY(area).data()[0].x &&
         rect->point[1].x >= ARRAY(area).data()[2].x &&
         rect->point[0].y <= ARRAY(area).data()[0].y &&
         rect->point[1].y >= ARRAY(area).data()[2].y) {
         return TRUE;
      }
   }
   return FALSE;
}                                  

bool CGCoordSpace::rect_inside_area(TRect *rect, ARRAY<TPoint> *area) {
   if (ARRAY(area).count() == 0 || CGCoordSpace(this).extent_empty(rect)) {
      return FALSE;
   }
   else {
                                                   
      if (rect->point[0].x >= ARRAY(area).data()[0].x &&
         rect->point[1].x <= ARRAY(area).data()[2].x &&
         rect->point[0].y >= ARRAY(area).data()[0].y &&
         rect->point[1].y <= ARRAY(area).data()[2].y) {
         return TRUE;
      }
   }
   return FALSE;
}                                  

                                                                              
MODULE::END                                                                   
                                                                              
# 21 "/home/jacob/keystone/src/graphics/gobject.c"

                                                                              
MODULE::INTERFACE                                                             
                                                                              

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
   TRect extent;                                                                                 
   void *native_object;                                                                            

   void new(void);
   void delete(void); 

   virtual void show(bool show);
   static inline void visibility_set(EGVisibility visibility);
 public:
   void CGObject(void);

   ATTRIBUTE<CString id>;
   ATTRIBUTE<EGVisibility visibility, ,PO_INHERIT> {
      if (data) {
         *data = *data ? 1 : 0;                
         CGObject(this).visibility_set(*data);
      }
      else {
         CGObject(this).visibility_set(EGVisibility.visible);
      }
   };
   ATTRIBUTE<ARRAY<TGTransform> transform, ,PO_ELEMENT_STRIPSPACES>;
 protected:
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);

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

};

static inline void CGObject::visibility_set(EGVisibility visibility) {
   CGObject *parent = CGObject(CObject(this).parent_find(&class(CGObject)));
   
   if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGObject,visibility>) || !parent) {
      this->visibility = visibility;
   }
   else {
      this->visibility = parent->visibility ? visibility : FALSE;
   }
}                            

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CEventPointer::CEventPointer(EEventPointer type, double x, double y, int modifier) {
   this->type       = type;
   this->position.x = x;
   this->position.y = y;
   this->modifier   = modifier;
}                                

void CEventDraw::CEventDraw(CGCanvas *canvas, TRect *extent) {
   CEvent(this).CEvent();
   this->canvas = canvas;
   this->extent = extent;
}                          

void CGObject::new(void) {
   class:base.new();
   this->visibility = EGVisibility.visible;                                      
   ARRAY(&this->transform).new();
   new(&this->id).CString(NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGObject,visibility>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGObject,id>, TRUE);
}                 

void CGObject::CGObject(void) {
}                      

void CGObject::delete(void) {
   delete(&this->id);
   ARRAY(&this->transform).delete();

   class:base.delete();    
}                    

                                              
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
}                              

bool CGObject::transform_list_apply(ARRAY<TGTransform> *list, TGTransform *transform) {
   TGTransform *object_transform = NULL, *object_scale, tr_rotate = {EGTransform.none};
   int count = ARRAY(list).count();
   int form = -1;
   bool has_rotation = FALSE;

   if (count >= 1 && ARRAY(list).data()[count - 1].type == EGTransform.rotate) {
                                                                                   
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
}                                  

void CGObject::extent_set(CGCanvas *canvas) {
   CLEAR(&this->extent);
}                        

void CGObject::show(bool show) {}
void CGObject::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {}
bool CGObject::event(CEvent *event) { return FALSE; }

void CGObject::create_point_set(TPoint *point) {}
void CGObject::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(0);
}                        

void CGObject::cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new) {}
bool CGObject::transform_assimilate(TGTransform *transform) {
   return TRUE;
}

bool CGObject::transform_reduce(void) {
   bool result;
   int count;
   TGTransform *transform;
   ARRAY<TGTransform> tlist;
                                                                    
   
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

                                                                              
MODULE::END                                                                   
                                                                              
# 16 "/home/jacob/keystone/src/graphics/graphics.c"
# 1 "/home/jacob/keystone/src/graphics/gwindow.c"








  
                                                                              
MODULE::IMPORT                                                                
                                                                              





# 1 "/home/jacob/keystone/src/graphics/glayout.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              





# 1 "/home/jacob/keystone/src/graphics/gcanvas.c"








  
                                                                              
MODULE::IMPORT                                                                
                                                                              




# 1 "/home/jacob/keystone/src/graphics/gxulelement.c"








  
                                                                              
MODULE::IMPORT                                                                
                                                                              









                                                                              
MODULE::INTERFACE                                                             
                                                                              

class CGLayout;
class CGTree;
class CGWindow;
class CGWindowChild;

class CGXULElement : CGObject {
 private:
   void new(void);
   void delete(void);

   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void show(bool show);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual bool keyboard_input(void);
   virtual void cpoint_get(ARRAY<TPoint> *point);
   virtual void cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new);
   virtual void create_point_set(TPoint *point);
   virtual bool transform_assimilate(TGTransform *transform);
   virtual void extent_set(CGCanvas *canvas);
   virtual void binding_resolve(void);
   virtual CGObject *render_primative(bool for_print);
   virtual void NATIVE_draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
   void* NATIVE_get_background(void);

                                                                    
   bool NATIVE_focus_current(void);
   void NATIVE_focus_in(void);
   void NATIVE_focus_out(void);
   bool updating;
   
                                                   
   bool repeat_event(CEvent *event);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
   void utov(CGCanvas *canvas, TPoint *result);
   bool allocated_position(TRect *result);
 public:
   ATTRIBUTE<TCoord x>;
   ATTRIBUTE<TCoord y>;
   ATTRIBUTE<TCoord width>;
   ATTRIBUTE<TCoord height>;

   ATTRIBUTE<int flex> {
      if (data)
         this->flex = *data;
      else
         this->flex = 1;
   };
   ATTRIBUTE<TCoord min_width,  "min-width">;
   ATTRIBUTE<TCoord min_height, "min-height">;
   ATTRIBUTE<TCoord max_width,  "max-width">;
   ATTRIBUTE<TCoord max_height, "max-height">;
   ATTRIBUTE<bool disabled, , PO_INHERIT>;
   ATTRIBUTE<bool isdefault, "default">;
   ATTRIBUTE<CString tooltiptext>;
   ATTRIBUTE<CXPath binding>;

   void CGXULElement(int x, int y, int width, int height);
};

class CGSpacer : CGXULElement {
 public:
   ALIAS<"xul:spacer">;

   void CGSpacer(int x, int y, int width, int height);
};

class CGContainer : CGXULElement {
 private:
   bool allow_child_repos;                                             

   void new(void);
   void delete(void);

   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);

   virtual void show(bool show);
 public:
   void CGContainer(void);
};

class CGPacker : CGContainer {
 private:
   void delete(void); 
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
 public:
   void CGPacker(void);
};

ENUM:typedef<EGBoxOrientType> {
   {horizontal},
   {vertical},
};

ENUM:typedef<EGBoxPackType> {
   {start},
   {center},
   {end}
};

ENUM:typedef<EGBoxAlignType> {
   {start},
   {center},
   {end},
   {baseline},
   {stretch}
};

class CGBox : CGPacker {
 private:
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);

   CObject *all_child_next(CObject *child);
 public:
   ALIAS<"xul:box">;

   ATTRIBUTE<EGBoxOrientType orient>;
   ATTRIBUTE<EGBoxPackType pack>;
   ATTRIBUTE<EGBoxAlignType align>;

   void CGBox(void);
};

class CGVBox : CGBox {
 private:
   void new(void);
 public:
   ALIAS<"xul:vbox">;

    void CGVBox(void);
};

class CGHBox : CGBox {
 private:
   void new(void);
 public:
   ALIAS<"xul:hbox">;

   void CGHBox(void);
};

class CGStack : CGPacker {
 private:
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
 public:
   ALIAS<"xul:stack">;

    void CGStack(void);
};

class CGDeck : CGPacker {
 public:
   ALIAS<"xul:deck">;

    void CGDeck(void);
};

class CGGridCols : CObjPersistent {
 public:
   ALIAS<"xul:columns">;

   void CGGridCols(void);
};

class CGGridCol: CObjPersistent {
 public:
   ALIAS<"xul:column">;

   void CGGridCol(void);
};

class CGGridRows : CObjPersistent {
 public:
   ALIAS<"xul:rows">;

   void CGGridRows(void);
};

class CGGridRow: CObjPersistent {
 public:
   ALIAS<"xul:row">;

   void CGGridRow(void);
};

class CGGrid : CGPacker {



 public:
   ALIAS<"xul:grid">;

   ELEMENT:OBJECT<CGGridCols grid_cols, "xul:columns">;
   ELEMENT:OBJECT<CGGridRows grid_rows, "xul:rows">;

   void CGGrid(void);
};






ENUM:typedef<EGXULDirType> {
   {ltr},
   {rtl},
};

class CGButton : CGXULElement {
 private:
   void new(void);
   void delete(void);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:button">;

   ATTRIBUTE<EGXULDirType dir> {
       if (data) {
           this->dir = *data;
       }
       else {
           this->dir = EGXULDirType.ltr;
       }
   };
   ATTRIBUTE<CString label>;
   ATTRIBUTE<CString src>;

   void CGButton(int x, int y, int width, int height, const char *label);
};

class CGColourPicker : CGButton {
 private:
   void new(void);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:colorpicker">;

   ATTRIBUTE<TGColour colour, "color">;
   ATTRIBUTE<bool native_picker, "nativePicker"> {
      if (data) {
         this->native_picker = *data;
      }
      else {
         this->native_picker = FALSE;
      }
   };

   void CGColourPicker(int x, int y, int width, int height, TGColour colour);
};

class CGDatePickerButton : CGButton {
 private:
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:datepickerbutton">;

   ATTRIBUTE<TUNIXTime time>;

   void CGDatePickerButton(int x, int y, int width, int height, TUNIXTime time);
};


class CGToggleButton : CGButton {
 private:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:togglebutton">;
   ATTRIBUTE<bool toggled>;
   void CGToggleButton(int x, int y, int width, int height, char *label, bool toggled);
};

class CGCheckBox : CGButton {
 private:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:checkbox">;
   ATTRIBUTE<bool checked>;
   void CGCheckBox(int x, int y, int width, int height, char *label, bool checked);
};

class CGRadioButton : CGButton {
 private:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:radio">;
   ATTRIBUTE<bool selected>;
   void CGRadioButton(int x, int y, int width, int height, char *label, bool selected);
};

class CGSlider : CGXULElement {
 private:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:slider">;

   ATTRIBUTE<int minimum>;
   ATTRIBUTE<int maximum>;
   ATTRIBUTE<int position>;

   void CGSlider(int x, int y, int width, int height);
};

class CGTextLabel : CGXULElement {
 private:
   void new(void);
   void delete(void);
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:text">;

   ATTRIBUTE<CString value>;
   ATTRIBUTE<TGColour colour, "color">;
   ATTRIBUTE<TGColour background_colour, "background-color">;
   ATTRIBUTE<int font_size, "font-size">;
   ATTRIBUTE<EGXULDirType dir> {
       if (data) {
           this->dir = *data;
       }
       else {
           this->dir = EGXULDirType.ltr;
       }
   };

   void CGTextLabel(int x, int y, int width, int height, const char *text);
};

ENUM:typedef<EGProgressMode> {
   {determined},
   {undetermined},
};

class CGProgressMeter : CGXULElement {
 private:
   virtual void binding_resolve(void); 
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing); 
   virtual void NATIVE_allocate(CGLayout *layout);
 public:
   ALIAS<"xul:progressmeter">;
   ATTRIBUTE<EGProgressMode mode> {
      this->mode = data ? *data : EGProgressMode.determined;
   };
   ATTRIBUTE<int value>;   
};

class CGDatePicker : CGXULElement {
 private:
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:datepicker">;

   ATTRIBUTE<TUNIXTime time>;

   void CGDatePicker(int x, int y, int width, int height, TUNIXTime time);
};


                                                                                                              
class CGStatusBarPanel : CGTextLabel {
 public:
   ALIAS<"xul:statusbarpanel">;
   void CGStatusBarPanel(int x, int y, int width, int height, const char *text);
};

#if !OS_PalmOS

ENUM:typedef<EGTextBoxType> {
   {normal},
   {nodefault},   
   {autocomplete},
   {password},
   {numeric},
   {numericSpace},                                         
   {timed}
};

class CGTextBox : CGXULElement {
 private:
   bool modified;

   void new(void);
   void delete(void);
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:textbox">;

   ATTRIBUTE<TGColour colour, "color"> {
       if (data) {
           this->colour = *data;
       }
       else {
           this->colour = CGXULElement(this)->isdefault ? (GCOL_TYPE_NAMED | EGColourNamed.darkgrey) : GCOL_BLACK;
       }
   };
   ATTRIBUTE<TGColour background_colour, "background-color"> {
       if (data) {
           this->background_colour = *data;
       }
       else {
           this->background_colour = GCOL_NONE;
       }
   };
   ATTRIBUTE<CString value>;
   ATTRIBUTE<EGTextBoxType type> {
       if (data) {
           this->type = *data;
       }
       else {
           this->type = EGTextBoxType.normal;
       }
   };
   ATTRIBUTE<EGXULDirType dir> {
       if (data) {
           this->dir = *data;
       }
       else {
           this->dir = EGXULDirType.ltr;
       }
   };
   ATTRIBUTE<int maxlength> {
       if (data) {
           this->maxlength = *data;
       }
       else {
           this->maxlength = -1;
       }
   };
   ATTRIBUTE<bool multiline> {
       if (data) {
           this->multiline = *data;
       }
       else {
           this->multiline = FALSE;
       }
   };

   void CGTextBox(int x, int y, int width, int height, char *text);
};

ENUM:typedef<EGScrollBarUpdateType> {
   {normal},
   {nodrag},
};

class CGScrollBar : CGXULElement {
 private:
   void new(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
 public:
   ALIAS<"xul:scrollbar">;
   ATTRIBUTE<EGBoxOrientType orient>;
   ATTRIBUTE<int curpos>;
   ATTRIBUTE<int maxpos>;
   ATTRIBUTE<int range>;
   ATTRIBUTE<EGScrollBarUpdateType update_type, "updateType"> {
       if (data) {
           this->update_type = *data;
       }
       else {
           this->update_type = EGScrollBarUpdateType.normal;
       }
   };

   void CGScrollBar(int x, int y, int width, int height);
};

class CGScrolledArea : CGContainer {
 private:
   void new(void);
   void delete(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);

   CGScrollBar scroll_horiz;
   CGScrollBar scroll_vert;
 public:
   void CGScrolledArea(int x, int y, int width, int height);

   ALIAS<"xul:scrolledarea">;


};

                                           
ENUM:typedef<EGListItemType> {
   {normal},
   {radio},
   {checkbox},
};

class CGListItem : CGObject {
 private:
   void new(void);
   void delete(void);

   void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   static inline void value_int_default(void);
 public:
   ALIAS<"xul:listitem">;

   ATTRIBUTE<int value_int, "valueInt"> {
      if (data) {
         this->value_int = *data;
      }
      else {
         CGListItem(this).value_int_default();



      }
   };
   ATTRIBUTE<CString value, "value"> {
      if (data) {
         CString(&this->value).set_string(data);
      }
      else {
         CString(&this->value).set_string(&this->label);
      }
      CGListItem(this).value_int_default();

   };
   ATTRIBUTE<EGListItemType type> {
      if (data) {
         this->type = *data;
      }
      else {
         this->type = EGListItemType.normal;
      }
   };
   ATTRIBUTE<bool disabled>;
   ATTRIBUTE<CString label>;
   ATTRIBUTE<char accesskey> {
      if (data) {
         this->accesskey = *data;
      }
      else {
         this->accesskey = 0;
      }
   };
   ATTRIBUTE<int id>;
   ATTRIBUTE<bool checked>;
   ATTRIBUTE<bool isdefault> {
      if (data) {
         this->isdefault = *data;
      }
      else {
         this->isdefault = FALSE;
      }
   };
   ATTRIBUTE<TGColour colour, "color"> {
      if (data) {
         this->colour = *data;
      }
      else {
         this->colour = this->isdefault ? (GCOL_TYPE_NAMED | EGColourNamed.darkgrey) : GCOL_BLACK;
      }
   };

   void CGListItem(const char *label, int id, bool disabled, bool checked);
};

static inline void CGListItem::value_int_default(void) {
   if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGListItem,value_int>))
      return;
   if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGListItem,value>)) {
      if (CObject(this).parent()) {
         this->value_int = CObject(CObject(this).parent()).child_index(CObject(this));
      }
      else {
         this->value_int = atoi(CString(&this->value).string());
      }
   }
}                                 


class CGMenuItem : CGListItem {
 private:
   void new(void);
   void delete(void);

   CObjPersistent *menu_object;
   const TAttribute *menu_attribute;
 public:
   void CGMenuItem(const char *label, const char *acceltext, int id, bool disabled, bool checked);
   static inline void attribute_set(CObjPersistent *object, const TAttribute *attribute);

   ALIAS<"xul:menuitem">;
   ATTRIBUTE<TKeyboardKey accelerator>;
   ATTRIBUTE<CString acceltext>;
};

class CGMenuSeparator : CGMenuItem {
 public:
   void CGMenuSeparator(void);

   ALIAS<"xul:menuseparator">;
};

static inline void CGMenuItem::attribute_set(CObjPersistent *object, const TAttribute *attribute) {
   this->menu_object = object;
   this->menu_attribute = attribute;
};

class CGListBox : CGXULElement {
 private:
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
   virtual void NATIVE_allocate(CGLayout *layout);


   void *items;

 public:
   ALIAS<"xul:listbox">;
   ATTRIBUTE<int item>;

   void CGListBox(int x, int y, int width, int height);
};

class CGListMenuPopup : CObjPersistent {
 public:
   ALIAS<"xul:menupopup">;

   void CGListMenuPopup(void);
};

class CGMenuList : CGXULElement {
 private:
   ATTRIBUTE<int item>;

   void new(void);
   void delete(void);

   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   void NATIVE_list_set(int enum_count, const char **enum_name);
   virtual void NATIVE_allocate(CGLayout *layout);

   void *strings;

   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:menulist">;

   ELEMENT:OBJECT<CGListMenuPopup menupopup, "xul:menupopup">;
   ATTRIBUTE<TGColour colour, "color"> {
       if (data) {
           this->colour = *data;
       }
       else {
           this->colour = CGXULElement(this)->isdefault ? (GCOL_TYPE_NAMED | EGColourNamed.darkgrey) : GCOL_BLACK;
       }
   };
   ATTRIBUTE<TGColour background_colour, "background-color"> {
       if (data) {
           this->background_colour = *data;
       }
       else {
           this->background_colour = GCOL_WHITE;
       }
   };

   void CGMenuList(int x, int y, int width, int height);
};

class CObjClientGTree : CObjClient {
 private:
   CGTree *gtree;
   virtual void notify_all_update(void);
   virtual void notify_object_child_add(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_child_remove(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_selection_update(CObjPersistent *object, bool changing);
   void *NATIVE_item_object_find(CObjPersistent *object, void *root);
 public:
   void CObjClientGTree(CObjServer *obj_server, CObject *host,
                        CObjPersistent *object_root, CGTree *gtree);
};

class CGTreeCols : CObjPersistent {
 public:
   ALIAS<"xul:treecols">;

   void CGTreeCols(void);
};

class CGTreeCol : CObjPersistent {
 private:
   int position;

   void new(void);
   void delete(void);
 public:
   ALIAS<"xul:treecol">;
   ATTRIBUTE<CString label>;

   void CGTreeCol(const char *label);
};

class CGTree : CGXULElement {
 private:
   void new(void);
   void delete(void);

   CObjClientGTree client;
   TAttribute *attribute_selected;

   virtual void NATIVE_allocate(CGObjCanvas *canvas);

   void NATIVE_XULElement_fill(void *parent, CObjPersistent *object, int index);
#if OS_Win32
   void *hwnd_header;
   void *hwnd_tree;
#endif
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);

   void NATIVE_XULElement_update_object(void *node, CObjPersistent *object);
   void *tree_widget;

   bool selection_resolving;
 public:
   ALIAS<"xul:tree">;

   ATTRIBUTE<bool attribute_hide, "attribute-hide">;
   ELEMENT:OBJECT<CGTreeCols tree_cols, "xul:treecols">;

   void CGTree(CObjServer *obj_server, CObjPersistent *object_root, int x, int y, int width, int height);
   void column_add(const char *label);
};

class CGTabBox : CGContainer {
  private:
   virtual void NATIVE_allocate(CGLayout *layout);
#if OS_Win32
   void *wndproc;
#endif
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
  public:
   ALIAS<"xul:tabbox">;
   ATTRIBUTE<int selectedIndex>;

   void CGTabBox(int x, int y, int width, int height);
};

ENUM:typedef<EGSplitterType> {
   {Vertical},
   {Horizontal},
};

class CGSplitter : CGContainer {
 private:
   int old_position;
   int drag_mode;

   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
#if OS_Win32
#endif
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:splitter">;

   void CGSplitter(int x, int y, int width, int height);

   ATTRIBUTE<EGSplitterType type>;
   ATTRIBUTE<int position>;
};

#endif

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CGXULElement::new(void) {
   new(&this->binding).CXPath(NULL, NULL);
   new(&this->tooltiptext).CString(NULL);

   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,binding>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,disabled>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,isdefault>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,flex>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,tooltiptext>, TRUE);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>,  TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,max_width>,  TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,max_height>, TRUE);
}                     

void CGXULElement::CGXULElement(int x, int y, int width, int height) {
   CGXULElement(this)->x      = x;
   CGXULElement(this)->y      = y;
   CGXULElement(this)->width  = width;
   CGXULElement(this)->height = height;

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>,  TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,max_width>,  TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,max_height>, TRUE);

   if (width == 0 && height == 0) {
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,x>, TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,y>, TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>,  TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, TRUE);
   }
}                              

void CGXULElement::delete(void) {
   CGXULElement(this).show(FALSE);

   delete(&this->tooltiptext);
   delete(&this->binding);

   class:base.delete();
}                        

void CGXULElement::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(2);

   ARRAY(point).data()[0].x = this->x;
   ARRAY(point).data()[0].y = this->y;
   ARRAY(point).data()[1].x = this->x + this->width;
   ARRAY(point).data()[1].y = this->y + this->height;
}                            

void CGXULElement::cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new) {
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
}                            

void CGXULElement::create_point_set(TPoint *point) {
   this->x      = point[0].x;
   this->y      = point[0].y;
   this->width  = point[1].x - point[0].x;
   this->height = point[1].y - point[0].y;
}                                  

bool CGXULElement::transform_assimilate(TGTransform *transform) {
   switch (transform->type) {
   case EGTransform.scale:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      this->x = this->x * transform->t.scale.sx;
      this->y = this->y * transform->t.scale.sy;
      this->width = this->width * transform->t.scale.sx;
      this->height = this->height * transform->t.scale.sy;
      CObjPersistent(this).attribute_update_end();
      return TRUE;
   case EGTransform.translate:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,y>);
      this->x += (int) transform->t.translate.tx;
      this->y += (int) transform->t.translate.ty;
      CObjPersistent(this).attribute_update_end();
      return TRUE;
   default:
      break;
   }
   return FALSE;
}                                      

void CGXULElement::binding_resolve(void) {}
CGObject *CGXULElement::render_primative(bool for_print) { return NULL; }



void CGContainer::new(void) {
   class:base.new();
}                    

void CGContainer::CGContainer(void) {
   CGXULElement(this).CGXULElement(0, 0, 0, 0);
}                            

void CGContainer::delete(void) {
   class:base.delete();
}                       

void CGContainer::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
                    
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);

   switch (linkage) {
   case EObjectLinkage.ParentSet:
      CGObject(this).show(TRUE);
      break;
   case EObjectLinkage.ParentReleasePre:      
      CGObject(this).show(FALSE);   
      break;
   case EObjectLinkage.ParentRelease:

      break;
   default:
      break;
   }
}                                      

void CGContainer::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);
}                                        

void CGContainer::show(bool show) {
   CObject *object = CObject(this).child_first();

   if (show && !CObjClass_is_derived(&class(CGPacker), CObject(this).obj_class())) {
      class:base.show(show);
   }

   if (!show || !CObjClass_is_derived(&class(CGTabBox), CObject(this).obj_class())) {
                                                 
      while (object) {
         if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
            CGObject(object).show(show);
         }
         object = CObject(this).child_next(object);
      }
   }

   if (!show && !CObjClass_is_derived(&class(CGPacker), CObject(this).obj_class())) {
      class:base.show(show);
   }
}                     

void CGContainer::NATIVE_allocate(CGLayout *layout) {
   class:base.NATIVE_allocate(layout);
}                                

void CGContainer::NATIVE_release(CGLayout *layout) {
   class:base.NATIVE_release(layout);
}                               

void CGPacker::CGPacker(void) {
   CGContainer(this).CGContainer();
}                      

void CGPacker::delete(void) {
}                    

void CGPacker::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);
}                                     

void CGPacker::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
                                           
}                  

void CGBox::CGBox(void) {
   CGPacker(this).CGPacker();
}                

void CGBox::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
                    
   _virtual_CGContainer_notify_object_linkage(CGContainer(this), linkage, object);

   switch (linkage) {
   case EObjectLinkage.ChildAdd:
   case EObjectLinkage.ChildRemove:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_update_end();
      break;
   default:
      break;
   }
}                                

void CGBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObject *object;
   int i;
   TCoord size, pos = 0, a, flex;
   TCoord element_size, container_size, flex_total;

   class:base.notify_attribute_update(attribute, changing);

   if (!CObject(this).child_count() || changing)
      return;

   flex_total = pos = a = 0;
   pos = this->orient == EGBoxOrientType.horizontal ? CGXULElement(this)->x : CGXULElement(this)->y;

   container_size = this->orient == EGBoxOrientType.horizontal ?
                    CGXULElement(this)->width : CGXULElement(this)->height;
   object = CGBox(this).all_child_next(NULL);
   while (object) {
      if (CObjPersistent(object).attribute_default_get(this->orient == EGBoxOrientType.horizontal ?
                                                       ATTRIBUTE<CGXULElement,width> : ATTRIBUTE<CGXULElement,height>)) {
         flex_total += CGXULElement(object)->flex ? CGXULElement(object)->flex : 1;
      }
      else {
         element_size = this->orient == EGBoxOrientType.horizontal ?
                        CGXULElement(object)->width : CGXULElement(object)->height;
         container_size -= element_size;
      }
      object = CGBox(this).all_child_next(object);
   }

   object = CGBox(this).all_child_next(NULL);
   while (object) {
      element_size = 0;
      size = this->orient == EGBoxOrientType.horizontal ? CGXULElement(this)->width : CGXULElement(this)->height;

      if (!CObjPersistent(object).attribute_default_get(this->orient == EGBoxOrientType.horizontal ?
                                                        ATTRIBUTE<CGXULElement,width> : ATTRIBUTE<CGXULElement,height>)) {
         element_size = this->orient == EGBoxOrientType.horizontal ?
                        CGXULElement(object)->width : CGXULElement(object)->height;
      }
      else {
         element_size = 0;
         flex = CGXULElement(object)->flex ? CGXULElement(object)->flex : 1;
         for (i = 0; i < flex; i++) {
            while (a < container_size) {
               a += flex_total;
               element_size++;
            }
            a -= container_size;
         }
      }

      switch (this->orient) {
      case EGBoxOrientType.horizontal:
         CGXULElement(object)->x = pos;
         CGXULElement(object)->y = CGXULElement(this)->y;
         CGXULElement(object)->width = element_size;
         if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,height>)) {
            CGXULElement(object)->height = CGXULElement(this)->height;
         }
         break;
      case EGBoxOrientType.vertical:
         CGXULElement(object)->x = CGXULElement(this)->x;
         CGXULElement(object)->y = pos;
         if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,width>)) {
            CGXULElement(object)->width = CGXULElement(this)->width;
         }
         CGXULElement(object)->height = element_size;
         break;
      }
      CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(object).attribute_update_end();

      pos += element_size;

      object = CGBox(this).all_child_next(object);
   }
}                                  

CObject *CGBox::all_child_next(CObject *child) {
   CObject *object;

   object = child;
   do {
      if (!object) {
         object = CObject(this).child_first();
      }
      else {
         if (!CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
                                       
            object = CObject(object).child_first();
         }
         else {
            object = NULL;
         }
         if (!object) {
            object = CObject(CObject(child).parent()).child_next(CObject(child));
         }
         while (!object) {
            object = CObject(child).parent();
            child = object;
            if (object == CObject(this)) {
               return NULL;
            }
            object = CObject(CObject(object).parent()).child_next(CObject(object));
         }
      }

      if (object == NULL)
         return NULL;

      child = object;
   } while (!CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class()));



   return object;
}                         

void CGVBox::CGVBox(void) {
   CGBox(this).CGBox();
}                  

void CGVBox::new(void) {
   _virtual_CGXULElement_new((CGXULElement *)this);

   CGBox(this)->orient = EGBoxOrientType.vertical;
}                                   

void CGHBox::CGHBox(void) {
   CGBox(this).CGBox();
}                  

void CGHBox::new(void) {
   _virtual_CGXULElement_new((CGXULElement *)this);

   CGBox(this)->orient = EGBoxOrientType.horizontal;
}               

void CGStack::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
                    
   _virtual_CGContainer_notify_object_linkage(CGContainer(this), linkage, object);

   switch (linkage) {
   case EObjectLinkage.ChildAdd:
   case EObjectLinkage.ChildRemove:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_update_end();
      break;
   default:
      break;
   }
}                                  

void CGStack::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObject *object;

   class:base.notify_attribute_update(attribute, changing);

   if (!CObject(this).child_count() || changing)
      return;

   object = CObject(this).child_first();
   while (object) {
       if (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
          if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,x>)) {
             CGXULElement(object)->x = CGXULElement(this)->x;
          }
          if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,y>)) {
             CGXULElement(object)->y = CGXULElement(this)->y;
          }
          if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,width>)) {
             CGXULElement(object)->width = CGXULElement(this)->width;
          }
          if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,height>)) {
             CGXULElement(object)->height = CGXULElement(this)->height;
          }
          CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,x>);
          CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,y>);
          CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,width>);
          CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,height>);
          CObjPersistent(object).attribute_update_end();
       }
       object = CObject(this).child_next(CObject(object));
   }
}                                    

void CGButton::new(void) {
   class:base.new();

   new(&this->label).CString(NULL);
   new(&this->src).CString(NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGButton,dir>, TRUE);
}                 

void CGButton::CGButton(int x, int y, int width, int height, const char *label) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   CString(&this->label).set(label);
}                      

void CGButton::delete(void) {
   delete(&this->src);
   delete(&this->label);

   class:base.delete();
}                    

void CGToggleButton::CGToggleButton(int x, int y, int width, int height, char *label, bool toggled) {
   CGButton(this).CGButton(x, y, width, height, label);
   this->toggled = toggled;
}                                  

void CGCheckBox::CGCheckBox(int x, int y, int width, int height, char *label, bool checked) {
   CGButton(this).CGButton(x, y, width, height, label);
   this->checked = checked;
}                          

void CGToggleButton::binding_resolve(void) {
   bool value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      switch (CGXULElement(this)->binding.object.attr.index) {
      case ATTR_INDEX_VALUE:
      default:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                           CGXULElement(this)->binding.object.attr.element,
                                                                                                           CGXULElement(this)->binding.object.attr.index);
         break;
      case ATTR_INDEX_DEFAULT:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);
         break;
      }

      if (this->toggled != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGToggleButton,toggled>);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGToggleButton,toggled>, value ? "TRUE" : "FALSE");
         CObjPersistent(this).attribute_update_end();
      }
   }
}                                   

void CGCheckBox::binding_resolve(void) {
   bool value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      switch (CGXULElement(this)->binding.object.attr.index) {
      case ATTR_INDEX_VALUE:
      default:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                           CGXULElement(this)->binding.object.attr.element,
                                                                                                           CGXULElement(this)->binding.object.attr.index);
         break;
      case ATTR_INDEX_DEFAULT:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);
         break;
      }

      if (this->checked != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGCheckBox,checked>);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGCheckBox,checked>, value ? "TRUE" : "FALSE");
         CObjPersistent(this).attribute_update_end();
      }
   }
}                               

void CGRadioButton::CGRadioButton(int x, int y, int width, int height, char *label, bool selected) {
   CGButton(this).CGButton(x, y, width, height, label);
   this->selected = selected;
}                                

void CGRadioButton::binding_resolve(void) {
   bool value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      switch (CGXULElement(this)->binding.object.attr.index) {
      case ATTR_INDEX_VALUE:
      default:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                           CGXULElement(this)->binding.object.attr.element,
                                                                                                           CGXULElement(this)->binding.object.attr.index);
         break;
      case ATTR_INDEX_DEFAULT:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);
         break;
      }

      if (this->selected != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRadioButton,selected>);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGRadioButton,selected>, value ? "TRUE" : "FALSE");
         CObjPersistent(this).attribute_update_end();
      }
   }
}                                  

void CGColourPicker::new(void) {
   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGColourPicker,native_picker>, TRUE);
}                       

void CGColourPicker::CGColourPicker(int x, int y, int width, int height, TGColour colour) {
   CGButton(this).CGButton(x, y, width, height, NULL);
   this->colour = colour;
}                                  

void CGColourPicker::binding_resolve(void) {
   TGColour value;
   bool is_default;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute &&
       !CXPath(&CGXULElement(this)->binding).assignment()) {
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get(CGXULElement(this)->binding.object.attr.attribute,
                                                                              CGXULElement(this)->binding.object.attr.element,
                                                                              CGXULElement(this)->binding.object.attr.index,
                                                                              &ATTRIBUTE:type<TGColour>, (void *)&value);
      is_default = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);

      if (this->colour != value || CGXULElement(this)->isdefault != is_default) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGColourPicker,colour>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,isdefault>);
         CObjPersistent(this).attribute_set(ATTRIBUTE<CGColourPicker,colour>, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<TGColour>, &value);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,isdefault>, is_default ? "TRUE" : "FALSE");
         CObjPersistent(this).attribute_update_end();
      }
   }
}                                   

void CGDatePickerButton::CGDatePickerButton(int x, int y, int width, int height, TUNIXTime time) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   this->time = time;
}                                          

void CGDatePickerButton::binding_resolve(void) {
   TUNIXTime time = 0;
   CString label;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      new(&label).CString(NULL);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get(CGXULElement(this)->binding.object.attr.attribute,
                                                                              CGXULElement(this)->binding.object.attr.element,
                                                                              CGXULElement(this)->binding.object.attr.index,
                                                                              &ATTRIBUTE:type<TUNIXTime>, (void *)&time);
      if (this->time != time) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGDatePickerButton,time>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGButton,label>);      
         CObjPersistent(this).attribute_set(ATTRIBUTE<CGDatePickerButton,time>, -1, -1, &ATTRIBUTE:type<TUNIXTime>, (void *)&time);
         CObjPersistent(this).attribute_set(ATTRIBUTE<CGButton,label>, -1, -1, &ATTRIBUTE:type<TUNIXTime>, (void *)&time);      
         CObjPersistent(this).attribute_update_end();
      }
      delete(&label);
   }
}                                       

void CGSlider::CGSlider(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);

   this->minimum = 0;
   this->maximum = 100;
}                      

void CGSlider::binding_resolve(void) {
   int value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      value =
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                            CGXULElement(this)->binding.object.attr.element,
                                                                                            CGXULElement(this)->binding.object.attr.index);

      if (this->position != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGSlider,position>);
         CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGSlider,position>, value);
         CObjPersistent(this).attribute_update_end();
      }
   }
}                             

void CGTextLabel::new(void) {
   new(&this->value).CString(NULL);

   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextLabel,colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextLabel,background_colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextLabel,font_size>, TRUE);
}                    

void CGTextLabel::CGTextLabel(int x, int y, int width, int height, const char *text) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   CString(&this->value).set(text);
}                            

void CGTextLabel::delete(void) {
   class:base.delete();

   delete(&this->value);
}                       

void CGTextLabel::binding_resolve(void) {
   CString value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      new(&value).CString(NULL);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                            CGXULElement(this)->binding.object.attr.element,
                                                                                            CGXULElement(this)->binding.object.attr.index,
                                                                                            &value);
      if (CString(&this->value).strcmp(CString(&value).string()) != 0) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGTextLabel,value>);
         CObjPersistent(this).attribute_set_string(ATTRIBUTE<CGTextLabel,value>, &value);
         CObjPersistent(this).attribute_update_end();
      }

      delete(&value);
   }
}                                

void CGProgressMeter::binding_resolve(void) {
   int value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      value =
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                   CGXULElement(this)->binding.object.attr.element,
                                                                                                   CGXULElement(this)->binding.object.attr.index);

      if (this->value != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGProgressMeter,value>);
         CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGProgressMeter,value>, value);
         CObjPersistent(this).attribute_update_end();
      }
   }
}                                    

void CGDatePicker::CGDatePicker(int x, int y, int width, int height, TUNIXTime time) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   this->time = time;
}                              

void CGDatePicker::binding_resolve(void) {
   TUNIXTime time = 0;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get(CGXULElement(this)->binding.object.attr.attribute,
                                                                              CGXULElement(this)->binding.object.attr.element,
                                                                              CGXULElement(this)->binding.object.attr.index,
                                                                              &ATTRIBUTE:type<TUNIXTime>, (void *)&time);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGDatePicker,time>);
      CObjPersistent(this).attribute_set(ATTRIBUTE<CGDatePicker,time>, -1, -1, &ATTRIBUTE:type<TUNIXTime>, (void *)&time);
      CObjPersistent(this).attribute_update_end();
   }
}                                 

void CGStatusBarPanel::CGStatusBarPanel(int x, int y, int width, int height, const char *text) {
    CGTextLabel(this).CGTextLabel(x, y, width, height, text);
}                                      

#if !OS_PalmOS

void CGTextBox::new(void) {
   class:base.new();

   new(&this->value).CString(NULL);
   CString(&this->value).encoding_set(EStringEncoding.UTF16);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,type>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,background_colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,dir>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,maxlength>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,multiline>, TRUE);
}                  

void CGTextBox::CGTextBox(int x, int y, int width, int height, char *text) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   CString(&this->value).set(text);
   CString(&this->value).encoding_set(EStringEncoding.UTF16);
}                        

void CGTextBox::delete(void) {
   class:base.delete();

   delete(&this->value);
}                     

void CGTextBox::binding_resolve(void) {
   CString value;
   bool is_default;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      new(&value).CString(NULL);
      CString(&value).encoding_set(EStringEncoding.UTF16);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                   CGXULElement(this)->binding.object.attr.element,
                                                                                                   CGXULElement(this)->binding.object.attr.index,
                                                                                                   &value);
      CString(&value).encoding_set(EStringEncoding.UTF16);
      is_default = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);

                                                      
      if ((is_default != CGXULElement(this)->isdefault ||
          !CString(&this->value).match(&value)) 
          && !this->modified) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGTextBox,value>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,isdefault>);
         CObjPersistent(this).attribute_set_string(ATTRIBUTE<CGTextBox,value>, &value);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,isdefault>, is_default ? "TRUE" : "FALSE");

         if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGTextBox,colour>)) {
             CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,colour>, TRUE);
         }
         if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGTextBox,background_colour>)) {
             CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,background_colour>, TRUE);
         }

         CObjPersistent(this).attribute_update_end();
      }

      delete(&value);
   }
}                              

void CGScrollBar::new(void) {
   class:base.new();
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGScrollBar,update_type>,TRUE);
}                    

void CGScrollBar::CGScrollBar(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}                            

void CGScrollBar::binding_resolve(void) {
   int value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                          CGXULElement(this)->binding.object.attr.element,
                                                                                                          CGXULElement(this)->binding.object.attr.index);
      if (this->curpos != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
         CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGScrollBar,curpos>, value);
         CObjPersistent(this).attribute_update_end();
      }
   }
}                                

void CGScrolledArea::new(void) {
    class:base.new();
}                       

void CGScrolledArea::CGScrolledArea(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}                                  

void CGScrolledArea::delete(void) {
    class:base.delete();
}                          

void CGScrolledArea::NATIVE_allocate(CGLayout *layout) {
   CGObject(this)->native_object = (void *)-1;
}                                   

void CGScrolledArea::NATIVE_release(CGLayout *layout) {
   CGObject(this)->native_object = NULL;
}                                  

void CGListItem::new(void) {
   class:base.new();

   new(&this->label).CString(NULL);
   new(&this->value).CString(NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,value>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,value_int>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,disabled>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,id>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,accesskey>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,checked>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,isdefault>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,type>, TRUE);
}                   

void CGListItem::CGListItem(const char *label, int id, bool disabled, bool checked) {
   CString(&this->label).set(label);
   this->id = id;
   this->disabled = disabled;
   this->checked = checked;
}                          

void CGListItem::delete(void) {
   delete(&this->value);
   delete(&this->label);

   class:base.delete();
}                      

void CGMenuItem::new(void) {
   new(&this->acceltext).CString(NULL);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuItem,acceltext>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuItem,accelerator>, TRUE);

   class:base.new();
}                   

void CGMenuItem::CGMenuItem(const char *label, const char *acceltext, int id, bool disabled, bool checked) {
   CGListItem(this).CGListItem(label, id, disabled, checked);
   if (acceltext) {
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuItem,acceltext>, FALSE);
      CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGMenuItem,acceltext>, acceltext);
   }
}                          

void CGMenuItem::delete(void) {
   class:base.delete();

   delete(&this->acceltext);
}                      

void CGMenuSeparator::CGMenuSeparator(void) {
}                                    

void CGListMenuPopup::CGListMenuPopup(void) {
}                                    

void CGMenuList::new(void) {
   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuList,colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuList,background_colour>, TRUE);

   new(&this->menupopup).CGListMenuPopup();
   CObject(&this->menupopup).parent_set(CObject(this));
}                   

void CGMenuList::CGMenuList(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}                          

void CGMenuList::delete(void) {
   delete(&this->menupopup);

   class:base.delete();
}                      

void CGMenuList::binding_resolve(void) {
   int i, index = -2, int_value;
   bool is_default;
   CString value;
   CGMenuItem *item;
   int int_binding = FALSE;
   
   item = CGMenuItem(CObject(&this->menupopup).child_first());
   while (item) {
      if (!CObjPersistent(item).attribute_default_get(ATTRIBUTE<CGListItem,value_int>)) {
         int_binding = TRUE;
      }
      item = CGMenuItem(CObject(&this->menupopup).child_next(CObject(item)));
   }
 
   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      if (int_binding || !CObject(&this->menupopup).child_count() ||
          CGXULElement(this)->binding.object.attr.attribute->data_type == &ATTRIBUTE:type<int>) {
         int_value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                    CGXULElement(this)->binding.object.attr.element,
                                                                                                    CGXULElement(this)->binding.object.attr.index);
         if (!CObject(&this->menupopup).child_count()) {
            index = int_value;
         }
         else {
            i = 0;
            item = CGMenuItem(CObject(&this->menupopup).child_first());
            while (item) {
               if (CGListItem(item)->value_int == int_value) {
                  index = i;
                  break;
               }
               i++;
               item = CGMenuItem(CObject(&this->menupopup).child_next(CObject(item)));
            }
         }
      }
      else {
         new(&value).CString(NULL);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                               CGXULElement(this)->binding.object.attr.element,
                                                                                               CGXULElement(this)->binding.object.attr.index,
                                                                                               &value);
         i = 0;
         item = CGMenuItem(CObject(&this->menupopup).child_first());
         while (item) {
            if (CString(&CGListItem(item)->value).strcmp(CString(&value).string()) == 0) {
               index = i;
               break;
            }
            i++;
            item = CGMenuItem(CObject(&this->menupopup).child_next(CObject(item)));
         }
         delete(&value);
      }
      is_default = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);

      if (index != -2) {
         if (is_default != CGXULElement(this)->isdefault ||
             index != this->item) {
            CObjPersistent(this).attribute_update(ATTRIBUTE<CGMenuList,item>);
            CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,isdefault>);
            CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGMenuList,item>, ATTR_ELEMENT_VALUE, ATTR_INDEX_VALUE, index);
            CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,isdefault>, is_default ? "TRUE" : "FALSE");
            CObjPersistent(this).attribute_update_end();
         }
      }

      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGMenuList,colour>)) {
         CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuList,colour>, TRUE);
      }
      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGMenuList,background_colour>)) {
         CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuList,background_colour>, TRUE);
      }
#if 0
                                                               
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,disabled>);
      CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,disabled>, is_default ? "TRUE" : "FALSE");
      CObjPersistent(this).attribute_update_end();
#endif
   }
}                               

void CGListBox::CGListBox(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}                        

void CGListBox::binding_resolve(void) {
   int i, index = -1;
   bool is_default;
   CString value;
   CGListItem *item;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      if (CGXULElement(this)->binding.object.attr.attribute->type == PT_AttributeEnum ||
          CGXULElement(this)->binding.object.attr.attribute->data_type == &ATTRIBUTE:type<int>) {
         index = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                           CGXULElement(this)->binding.object.attr.element,
                                                                                                           CGXULElement(this)->binding.object.attr.index);
      }
      else {
         new(&value).CString(NULL);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                               CGXULElement(this)->binding.object.attr.element,
                                                                                               CGXULElement(this)->binding.object.attr.index,
                                                                                               &value);
         i = 0;
         item = CGListItem(CObject(this).child_first());
         while (item) {
            if (CString(&CGListItem(item)->label).strcmp(CString(&value).string()) == 0) {
               index = i;
               break;
            }
            i++;
            item = CGListItem(CObject(this).child_next(CObject(item)));
         }
         delete(&value);
      }
      is_default = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);

      if (index != -1) {
         if (is_default != CGXULElement(this)->isdefault ||
             index != this->item) {
            CObjPersistent(this).attribute_update(ATTRIBUTE<CGListBox,item>);
            CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGListBox,item>, ATTR_ELEMENT_VALUE, ATTR_INDEX_VALUE, index);
            CObjPersistent(this).attribute_update_end();
         }
      }
#if 0
                                                               
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,disabled>);
      CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,disabled>, is_default ? "TRUE" : "FALSE");
      CObjPersistent(this).attribute_update_end();
#endif
   }
}                               

void CObjClientGTree::CObjClientGTree(CObjServer *obj_server, CObject *host,
                                      CObjPersistent *object_root, CGTree *gtree) {
   this->gtree = gtree;
   CObjClient(this).CObjClient(obj_server, host);
   CObjClient(this).object_root_add(object_root);
}                                    

void CGTreeCols::CGTreeCols(void) {
   CGTreeCol *tree_col;

   tree_col = new.CGTreeCol("Attribute");
   CObject(this).child_add(CObject(tree_col));
   tree_col = new.CGTreeCol("Value");
   CObject(this).child_add(CObject(tree_col));
}                          

void CGTreeCol::new(void) {
   new(&this->label).CString(NULL);
}                  

void CGTreeCol::CGTreeCol(const char *label) {
   CString(&this->label).set(label);
}                        

void CGTreeCol::delete(void) {
   delete(&this->label);
}                     

void CGTree::new(void) {
   new(&this->tree_cols).CGTreeCols();

   class:base.new();
}               

void CGTree::CGTree(CObjServer *obj_server, CObjPersistent *object_root, int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   new(&this->client).CObjClientGTree(obj_server, CObject(this), object_root, this);
}                  

void CGTree::delete(void) {
   delete(&this->client);

   delete(&this->tree_cols);

   class:base.delete();
}                  

void CGTree::column_add(const char *label) {
   CGTreeCol *tree_col = new.CGTreeCol(label);
   CObject(&this->tree_cols).child_add(CObject(tree_col));
}                      

void CGTabBox::CGTabBox(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}                      

void CGTabBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObject *child;
   TCoord min_width = 0, min_height = 0;
                                       

   if (!changing) {
      child = CObject(this).child_tree_first();
      while (child) {
         if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class())) {
            min_width = CGXULElement(child)->min_width > min_width ? CGXULElement(child)->min_width : min_width;
            min_height = CGXULElement(child)->min_height > min_height ? CGXULElement(child)->min_height : min_height;
         }
         child = CObject(this).child_tree_next(child);
      }

      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,min_width>)) {
         CGXULElement(this)->min_width  = min_width;
      }
      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,min_height>)) {
         CGXULElement(this)->min_height = min_height;
      }
   }

   class:base.notify_attribute_update(attribute, changing);
}                                     

void CGSplitter::CGSplitter(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);

   this->position = 220;
}                          

void CGSplitter::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   switch (linkage) {
   case EObjectLinkage.ChildAdd:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGSplitter,position>);
      CObjPersistent(this).attribute_update_end();
      break;
   default:
      break;
   }
}                                     

#endif

                                                                              
MODULE::END                                                                   
                                                                              
# 18 "/home/jacob/keystone/src/graphics/gcanvas.c"


                                                                              
MODULE::INTERFACE                                                             
                                                                              

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
   int content_width, content_height;                                                       
   int allocated_width, allocated_height;                                          
   int scroll_width, scroll_height;                                                  
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
                                                                         
      if (data)
         this->zoom = *data;
      else
         this->zoom = 100;

      CGCanvas(this).NATIVE_size_allocate();


   };
   ATTRIBUTE<double yAspect> {
                                                                         
      if (data)
         this->yAspect = *data;


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
}                         

static inline void CGCanvas::stroke_style_set(TCoord stroke_width, ARRAY<int> *stroke_dasharray, int stroke_dashoffset,
                                              int stroke_miterlimit,
                                              EGStrokeLineCap stroke_linecap, EGStrokeLineJoin stroke_linejoin) {
   double scale_stroke_width;

   scale_stroke_width =          (stroke_width * CGCoordSpace(&this->coord_space).scaling() + 0.5);
                              
   CGCanvas(this).NATIVE_stroke_style_set(scale_stroke_width, stroke_dasharray, stroke_dashoffset,
                                          stroke_miterlimit, stroke_linecap, stroke_linejoin);
}                              

static inline void CGCanvas::font_set(CGFontFamily *font_family, int font_size,
                                      EGFontStyle font_style, EGFontWeight font_weight,
                                      EGTextDecoration text_decoration) {
   int scale_font_size;

   CGCoordSpace(&this->coord_space).scaling_recalc();
   scale_font_size = (int)((double)font_size * CGCoordSpace(&this->coord_space).scaling());

   CGCanvas(this).NATIVE_font_set(font_family, scale_font_size, font_style, font_weight, text_decoration);
}                      

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

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

bool GCOL_PALETTE_FIND(CObjPersistent *object, CGPalette **palette, CGPalette **paletteB, bool *translate) {
   CGCanvas *canvas = CGCanvas(CObject(object).parent_find(&class(CGCanvas)));

   if (canvas) {
      *palette = canvas->palette;
      *paletteB = canvas->paletteB;
      *translate = canvas->translate_palette;
      return TRUE;
   }
   return FALSE;
}                     



void CImageLink::CImageLink(void) {
                                                                               
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
}                              

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
}                            

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
}                      

void CGBitmap::delete(void) {
   CGBitmap(this).NATIVE_release(void);
}                    

void CGImage::new(void) {
   class:base.new();

   new(&this->imagelink).CImageLink();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGImage,width>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGImage,height>, TRUE);
}                

void CGImage::CGImage(void) {
}                    

void CGImage::delete(void) {
   delete(&this->imagelink);

   if (this->loaded) {
      delete(&this->bitmap);
   }

   class:base.delete();
}                   

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
}                 

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
}                       

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
}                                 

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
}                      

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
}                        

void CGImage::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGCanvas *canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
   static TRect extent;                                                  

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
}                                    

void CGImage::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGCanvas *canvas;

   if (linkage == EObjectLinkage.ParentSet) {
      canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
      if (canvas) {
         CGImage(this).load_file(canvas);
      }
   }

   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);
}                                  

void CGGradientStop::CGGradientStop(void) {
}                                  

void CGLinearGradient::CGLinearGradient(void) {
}                                      

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
}                 

void CGCanvas::CGCanvas(int width, int height) {
   CGXULElement(this).CGXULElement(0, 0, width, height);
   this->colour_background = GCOL_WHITE;

}                      

void CGCanvas::delete(void) {


   class:base.delete();

   delete(&this->font_family);
   ARRAY(&this->stroke_dasharray).delete();
   delete(&this->coord_space);
}                    

void CGCanvas::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);

   if (!changing) {
      CGCanvas(this).view_update(FALSE);
   }
   CGCanvas(this).queue_draw(NULL);
}                                     

void CGCanvas::colour_background_set(TGColour colour) {
   this->colour_background = colour;
                  
}                                   

                                                   
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
}                                         

void CGCanvas::transform_viewbox(void) {
   TGTransform transform;
   int space;
   TCoord scale_x, scale_y;

                                             

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
}                               

void CGCanvas::point_array_utov(int count, TPoint *point) {
   CGCoordSpace(&this->coord_space).point_array_utov(count, point);
}                              

void CGCanvas::point_array_vtou(int count, TPoint *point) {
   CGCoordSpace(&this->coord_space).point_array_vtou(count, point);
}                              

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

                                                                                 
                                                                

   if (this->component_reposition == 1) {
      this->component_reposition = 2;
      CGCanvas(this).component_reposition(CObjPersistent(this));
      this->component_reposition = 1;
   }
}                         

void CGCanvas::transform_set_gobject(CGObject *gobject, bool viewbox) {
                                                              
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
}                                   

void CGCanvas::transform_prepend_gobject(CGObject *gobject) {
                                                              
   TGTransform *transform;
   int i;

   for (i = 0; i < ARRAY(&CGObject(gobject)->transform).count(); i++) {
      transform = &ARRAY(&CGObject(gobject)->transform).data()[i];
      CGCoordSpace(&CGCanvas(this)->coord_space).transform_apply(transform, TRUE);
   }
}                                   

void CGCanvas::draw_gobject_transformed(CGCanvas *canvas, CGObject *gobject, TRect *extent, EGObjectDrawMode mode) {
   TRect gobj_extent, canvas_extent;
   int i;
   TGTransform *transform;

   if (gobject->visibility == EGVisibility.visible) {
      CGCoordSpace(&this->coord_space).push();

                                                                                   
      if (ARRAY(&gobject->transform).count()) {
         for (i = 0; i < ARRAY(&CGObject(gobject)->transform).count(); i++) {
            transform = &ARRAY(&CGObject(gobject)->transform).data()[i];
            CGCoordSpace(&CGCanvas(this)->coord_space).transform_apply(transform, TRUE);
         }
         CGCoordSpace(&CGCanvas(this)->coord_space).scaling_recalc();
      }



                                                                           
      canvas->coord_space.matrix    = this->coord_space.matrix;
      canvas->coord_space.scaling.x = this->coord_space.scaling.x;
      canvas->coord_space.scaling.y = this->coord_space.scaling.y;
      canvas->coord_space.rotation  = this->coord_space.rotation;

                                                  
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
}                                      

void CGCanvas::redraw(TRect *extent, EGObjectDrawMode mode) {
   CGCoordSpace(&this->coord_space).reset();
   CGCanvas(this).draw(this, extent, mode);

   CGCanvas(this).transform_viewbox();
}                    

void CGCanvas::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {

   CGCanvas(this).opacity_set(1.0);
  CGCanvas(this).draw_rectangle(CGCanvas(this)->colour_background,
                                TRUE, extent->point[0].x, extent->point[0].y, extent->point[1].x, extent->point[1].y);


}                  

void CGCanvas::component_reposition(CObjPersistent *object) {
   CObjPersistent *child = CObjPersistent(CObject(object).child_first());

   while (child) {
      if (!CObjClass_is_derived(&class(CGCanvas), CObject(child).obj_class())) {
         CGCanvas(this).component_reposition(child);
      }

      if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class()) &&
          CGObject(child)->native_object) {




         CObjPersistent(child).attribute_update_end();
      }
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }
}                                  





void CGCanvasBitmap::CGCanvasBitmap(int width, int height) {
   new(&this->bitmap).CGBitmap();

   this->bitmap.width  = width;
   this->bitmap.height = height;

   CGBitmap(&this->bitmap).NATIVE_allocate();
}                                  

void CGCanvasBitmap::delete(void) {
   CGBitmap(&this->bitmap).NATIVE_release();

   delete(&this->bitmap);

   class:base.delete();
}                          

void CGCanvasBitmap::resize(int width, int height) {
   if (width  != this->bitmap.width ||
       height != this->bitmap.height) {
      CGBitmap(&this->bitmap).NATIVE_release();
      this->bitmap.width  = width;
      this->bitmap.height = height;
      CGBitmap(&this->bitmap).NATIVE_allocate();
   }
}                                  

void CGCanvasPrint::new(void) {
   class:base.new();

   new(&this->printer).CString(NULL);
   new(&this->outfile).CString(NULL);
}                      

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
}                                

void CGCanvasPrint::delete(void) {
   delete(&this->outfile);
   delete(&this->printer);

   class:base.delete();
}                         

void CGCanvasPrint::print_document(void) {
   CObject *object = CObject(this).child_first();

   CGCanvasPrint(this).NATIVE_allocate(NULL);



   CGCanvas(this).view_update(FALSE);

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
}                                 

void CGPalette::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGCanvas *canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));

   if (canvas && !changing) {
      CGCanvas(canvas).queue_draw(NULL);
   }

   class:base.notify_attribute_update(attribute, changing);
}                                      

                                                                              
MODULE::END                                                                   
                                                                              
# 20 "/home/jacob/keystone/src/graphics/glayout.c"
# 1 "/home/jacob/keystone/src/graphics/ganimation.c"








  
                                                                              
MODULE::IMPORT                                                                
                                                                              









                                                                              
MODULE::INTERFACE                                                             
                                                                              

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
}                            

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

                                                                             
MODULE::IMPLEMENTATION                                                        
                                                                              

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
}                    

void CGAnimation::CGAnimation(void) {
}                            

void CGAnimation::delete(void) {
   delete(&this->binding);
   delete(&this->value);

   class:base.delete();
}                       

bool CGAnimation::transform_assimilate(TGTransform *transform) {
   return FALSE;
}

void CGAnimation::binding_default_set(void) {}

bool CGAnimation::event(CEvent *event) {
   return FALSE;
}                      

void CGAnimation::animation_resolve_value(double relative_time) {

   this->time_based = !CString(&this->binding.path).length();
                                           

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
}                                        

void CGAnimation::animation_resolve(void) {
}                                  

void CGAnimation::reset(bool animated) {

   this->time_based = !CString(&this->binding.path).length();   
   this->state = EGAnimState.Inactive;

   if (animated) {
      CGAnimation(this)->reset_pending = TRUE;
   }
}                      

void CGAnimBind::new(void) {
   class:base.new();

   new(&this->original_string_value).CString(NULL);
   CString(&this->original_string_value).encoding_set(EStringEncoding.UTF16);

   CGAnimation(this)->reset_pending = TRUE;
}                   

void CGAnimBind::CGAnimBind(TAttributePtr *attributeName) {
   if (attributeName) {
      this->attributeName = *attributeName;
   }
}                          

void CGAnimBind::delete(void) {
   delete(&this->original_string_value);

   class:base.delete();
}                      

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
}                                 

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
}                     

void CGAnimAnimate::new(void) {
   class:base.new();

   new(&this->original_string_value).CString(NULL);

   CGAnimation(this)->reset_pending = TRUE;
}                      

void CGAnimAnimate::CGAnimAnimate(TAttributePtr *attributeName) {
   if (attributeName) {
      this->attributeName = *attributeName;
   }
}                                

void CGAnimAnimate::delete(void) {
   delete(&this->original_string_value);

   class:base.delete();
}                         

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
}                                       

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
        this->attributeName.attribute->data_type == &ATTRIBUTE:type<TGTransform>            )) {
      float_value = CGAnimation(this)->animation_value;


         if (CGAnimation(this)->dur) {
            if (float_value < CGAnimation(this)->begin) {
               float_value = CGAnimation(this)->begin;
            }
            if (float_value > CGAnimation(this)->end) {
               float_value = CGAnimation(this)->end;
            }

            float_value -= CGAnimation(this)->begin;
                            
                        
            float_value = (this->to - this->from) * float_value / (CGAnimation(this)->dur) + this->from;
         }
         else {
                                             
            float_value = float_value == CGAnimation(this)->begin ? this->to : this->from;
         }


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






}                                    

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
}                        

void CGAnimSet::new(void) {
   class:base.new();

   new(&this->original_string_value).CString(NULL);
   new(&this->to).CString(NULL);
}                  

void CGAnimSet::delete(void) {
   delete(&this->to);
   delete(&this->original_string_value);

   class:base.delete();
}                     

void CGAnimSet::animation_resolve(void) {
   CGObject *object = CGObject(CObject(this).parent());
   int int_value;

   if (!this->attributeName.attribute)
      return;

   if (this->attributeName.attribute->type == PT_AttributeEnum ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<TCoord> ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<int> ||
      this->attributeName.attribute->data_type == &ATTRIBUTE:type<bool>) {


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
}                                

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
}                    

extern const TAttribute __parameter_CGPrimitive_fill;

void CGAnimColour::new(void) {
                    
   TAttribute *attr = ATTRIBUTE<CGAnimColour,values>;
   attr->delim    = ";";

   ARRAY(&this->values).new();

   class:base.new();

   this->attributeName.attribute = ATTRIBUTE<CGPrimitive,fill>;
   this->attributeName.element   = -1;
   this->attributeName.index     = -1;
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimColour,from>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGAnimColour,to>,   TRUE);
}                     

void CGAnimColour::CGAnimColour(TAttributePtr *attributeName) {
   if (attributeName) {
      this->attributeName = *attributeName;
   }
}                              

void CGAnimColour::delete(void) {
   ARRAY(&this->values).delete();

   class:base.delete();
}                        

void CGAnimColour::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   switch (linkage) {
   case EObjectLinkage.ParentSet:
      CObjPersistent(this).attribute_refresh(FALSE);


      break;
   default:
      break;
   }

   class:base.notify_object_linkage(linkage, object);
}                                       

void CGAnimColour::attribute_target_set(void) {
   CGObject *gobject = CGObject(CObject(this).parent());

   if (gobject) {
      CObjPersistent(gobject).attribute_get(this->attributeName.attribute, this->attributeName.element,
                                            this->attributeName.index, &ATTRIBUTE:type<TGColour>,
                                            &this->original_colour);
   }
}                                      

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
}                                          

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
}                       

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
}                                   

void CGAnimTransform::new(void) {
   class:base.new();

   CGAnimation(this)->reset_pending = TRUE;
}                        

void CGAnimTransform::delete(void) {
   class:base.delete();
}                           

void CGAnimTransform::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   int count;
   switch (linkage) {
   case EObjectLinkage.ParentSet:
      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGAnimTransform,type>)) {
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGAnimTransform,attributeName>, "transform");
      }
                                                       
      count = ARRAY(&CGObject(object)->transform).count();
      ARRAY(&CGObject(object)->transform).used_set(count + CObject(object).child_count());
      ARRAY(&CGObject(object)->transform).used_set(count);
      break;
   default:
      break;
   }
   class:base.notify_object_linkage(linkage, object);
}                                          

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
}                                         

void CGAnimTransform::animation_resolve(void) {
   int int_value, begin, dur;
   CGObject *object = CGObject(CObject(this).parent());





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
}                                      

void CGAnimTransform::reset(bool animated) {
   CGObject *object = CGObject(CObject(this).parent());
   
   class:base.reset(animated);

   if (animated) {
      CGAnimation(this)->reset_pending = TRUE;

                                                            
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
}                          

bool CGAnimation::is_base_animation(void) {
   const CObjClass *obj_class = CObject(this).obj_class();

   return (
      obj_class == &class(CGAnimation)    ||

      obj_class == &class(CGAnimAnimate)  ||
      obj_class == &class(CGAnimSet)      ||
      obj_class == &class(CGAnimMotion)   ||
      obj_class == &class(CGAnimColour)   ||
      obj_class == &class(CGAnimTransform)
      );
}                                  

                                                                              
MODULE::END                                                                   
                                                                              
# 21 "/home/jacob/keystone/src/graphics/glayout.c"


                                                                              
MODULE::INTERFACE                                                             
                                                                              

#define SVG_LOAD_ANIM_MAP TRUE
#define LAYOUT_TOOLTIP TRUE

class CGLayout;

class CGSelection : CGObject {
 private:
                     
   CFsm fsm;
   STATE state_choose(CEvent *event);
   STATE state_choose_key(CEvent *event);
   CObjServer *server;

 private:
   TRect area;
   bool bold_highlight;

   void pointer_event(CEventPointer *pointer, CObject *parent);
   void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);

   void new(void);
   void delete(void);
 public:
   CSelection selection;
   CSelection sibling_selection;

   void CGSelection(CGLayout *layout, CObjServer *server);
   static inline bool inside_area(TPoint *point, int extend);
   void select_clear(void);
   bool select_area(CObject *parent);
   bool select_input(void);
   bool select_sibling_next(void);
   bool select_sibling_previous(void);
   bool select_sibling_first(void);
   bool select_sibling_last(void);
   void clear(void);
   void delete_selected(void);
   void selection_update(void);
};

static inline bool CGSelection::inside_area(TPoint *point, int extend) {
                                    
   if (point->x >= (this->area.point[0].x - extend) && point->x <= (this->area.point[1].x + extend) &&
       point->y >= (this->area.point[0].y - extend) && point->y <= (this->area.point[1].y + extend))
      return TRUE;

   return FALSE;
}                           

class CObjClientGLayout : CObjClient {
 private:
   void new(void);
   void delete(void);

   void iterate(void);
   void resolve_animation(void *data);

   CGLayout *glayout;
   virtual void notify_select(void);
   virtual void notify_all_update(void);
   virtual void notify_object_child_add(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_child_remove(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing);

                                                             
   ARRAY<CObjPersistent *> pending_object;                                               
 public:
   void CObjClientGLayout(CObjServer *obj_server, CObject *host, CObjPersistent *object_root, CGLayout *glayout);
};

                                  
class CGDefs : CGObject {
 private:
   void new(void);
   void delete(void);
   virtual void show(bool show);
 public:
   ALIAS<"svg:defs">;
   ELEMENT:OBJECT<CGPalette palette>;

   void CGDefs(void);
};

ENUM:typedef<EGLayoutRender> {
   {normal}, {buffered}, {none}
};

class CGLayout : CGCanvas {
 private:
                     
   CFsm fsm;
   STATE state_freeze(CEvent *event);
   STATE state_animate(CEvent *event);

 private:
   bool disable_child_alloc;
   CMutex update_mutex;
#if LAYOUT_TOOLTIP
   CGObject *tooltip_gobject;
   CGObject *tooltip_layout;
   CGObject *tooltip_window;
#endif
   EFileLockMode load_locked;
   TPoint hover_position;
   int hover_timeout;
   double relative_time;
   CTimer iterate_timer;
   ARRAY<CObjPersistent *> time_animation;                                                     
   ARRAY<CObjPersistent *> data_animation;                                                      
   void iterate(void);
   int frame_length;
   CObjPersistent *selected_held;

   CGSelection hover_gselection;
   CGSelection key_gselection;

   EGLayoutRender render;
   CGCanvasBitmap canvas_bitmap;

   CObjClientGLayout client;
   void new(void);
   void delete(void);   
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_file_load(const char *filename);


   void show_children(bool show);
   CGLayout *find_visible_child(void);
   void key_gselection_set(CGObject *object);
   void key_gselection_refresh(void);

   void animation_resolve_inhibit(CObjPersistent *object);
   void animation_resolve(CObjPersistent *object);
   void animation_reset(CObjPersistent *object, bool animated);
   void animation_buildmap(CObjPersistent *object);
   void animation_add(CObjPersistent *object);
   void animation_remove(CObjPersistent *object);

   static inline void selected_held_set(CObjPersistent *object);

                                        
   CObject *exports_child_tree_first(CObject *parent);
   CObject *exports_child_tree_next(CObject *parent, CObject *child);
 public:
   ALIAS<"svg:svg">;
   ELEMENT:OBJECT<CGDefs defs>;                              
   ATTRIBUTE<CString xmlns> {
      if (data) {
         CString(&this->xmlns).set(CString(data).string());
      }
      else {
         CString(&this->xmlns).set("http:\057\057www.w3.org""\057""2000\057svg");
      }
   };
   ATTRIBUTE<CString xmlns_xlink, "xmlns:xlink"> {
      if (data) {
         CString(&this->xmlns).set(CString(data).string());
      }
      else {
         CString(&this->xmlns_xlink).set("http:\057\057www.w3.org""\057""1999\057xlink");
      }
   };
   ELEMENT<CString title> {
      if (data) {
         CString(&this->title).set_string(data);
      }
      else {
         CString(&this->title).clear();
      }
   };
   CString filename;


   void CGLayout(int width, int height, CObjServer *obj_server, CObjPersistent *data_source);

   static inline void render_set(EGLayoutRender render);

   CObjPersistent *child_find_by_id(char *id);

   virtual void clear_all(void);
   bool load_svg_file(const char *filename, CString *error_result);

                 
   bool inplace_edit(CGObject *object);

   virtual void show(bool show);
   virtual void redraw(TRect *extent, EGObjectDrawMode mode);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void event(CEvent *event);

   void animate(void);
};

static inline void CGLayout::render_set(EGLayoutRender render) {
   this->render = render;
}                        

static inline void CGLayout::selected_held_set(CObjPersistent *object) {
   this->selected_held = object;
}                               

class CGLayoutTab : CGLayout {
 private:
   void new(void);
   void delete(void);
 public:
   ALIAS<"xul:tabpanel">;
   ATTRIBUTE<CString label>;

   void CGLayoutTab(const char *name, int width, int height, CObjServer *obj_server, CObjPersistent *data_source);
};

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

# 1 "/home/jacob/keystone/src/graphics/gprimcontainer.c"








  


  

                                                                              
MODULE::IMPORT                                                                
                                                                              


# 1 "/home/jacob/keystone/src/graphics/gprimitive.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              








                                                                              
MODULE::INTERFACE                                                             
                                                                              

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

   void point_add(TPoint *point);                                       
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
}                         

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              



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
}                    

void CGPrimitive::CGPrimitive(TGColour stroke, TGColour fill) {
   this->stroke = stroke;
   this->fill = fill;
}                            

void CGPrimitive::delete(void) {
   ARRAY(&this->stroke_dasharray).delete();

   class:base.delete();
}                       

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
}                            

void CGPrimitive::marker_transform(CGCanvas *canvas, CGMarker *marker, TPoint *position, TCoord angle, bool enter) {
   TGTransform transform;
   double scale = 1;
   static bool def_stroke = FALSE, def_fill = FALSE;               
   
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
}                                 

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
}                     

void CGRect::new(void) {
   class:base.new();
}               

void CGRect::CGRect(TGColour stroke, TGColour fill,
                     double x, double y, double width, double height) {
   CGPrimitive(this).CGPrimitive(stroke, fill);
   this->x      = x;
   this->y      = y;
   this->width  = width;
   this->height = height;
}                  

void CGRect::delete(void) {
   class:base.delete();
}                  

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
}                

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
}                      

void CGRect::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(2);

   ARRAY(point).data()[0].x = this->x;
   ARRAY(point).data()[0].y = this->y;
   ARRAY(point).data()[1].x = this->x + this->width;
   ARRAY(point).data()[1].y = this->y + this->height;
}                      

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
}                      

void CGRect::create_point_set(TPoint *point) {
   this->x      = point[0].x;
   this->y      = point[0].y;
   this->width  = point[1].x - point[0].x;
   this->height = point[1].y - point[0].y;
}                            

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
}                                

void CGRect::position_polarize(void) {
   if (this->width < 0) {
      this->x += this->width;
      this->width = -this->width;
   }
   if (this->height < 0) {
      this->y += this->height;
      this->height = -this->height;
   }
}                             

void CGEllipse::CGEllipse(TGColour stroke, TGColour fill,
                           double cx, double cy, double rx, double ry) {
   CGPrimitive(this).CGPrimitive(stroke, fill);
   this->cx = cx;
   this->cy = cy;
   this->rx = rx;
   this->ry = ry;
}                        

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
}                   

void CGEllipse::extent_set(CGCanvas *canvas) {
                                                                                       
   TPoint point[2];
   point[0].x = this->cx - this->rx;
   point[0].y = this->cy - this->ry;
   point[1].x = this->cx + this->rx;
   point[1].y = this->cy + this->ry;

   CGCoordSpace(&canvas->coord_space).point_array_utov(2, point);
   CGCoordSpace(&canvas->coord_space).extent_reset(&CGObject(this)->extent);
   CGCoordSpace(&canvas->coord_space).extent_add(&CGObject(this)->extent, 2, point);
   CGCoordSpace(&canvas->coord_space).extent_extend(&CGObject(this)->extent, CGPrimitive(this)->stroke_width / 2);   
}                         

void CGEllipse::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(3);

   ARRAY(point).data()[0].x = this->cx;
   ARRAY(point).data()[0].y = this->cy;
   ARRAY(point).data()[1].x = this->cx - this->rx;
   ARRAY(point).data()[1].y = this->cy;
   ARRAY(point).data()[2].x = this->cx;
   ARRAY(point).data()[2].y = this->cy - this->ry;
}                         

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
}                         

void CGEllipse::create_point_set(TPoint *point) {
   this->cx = point[0].x;
   this->cy = point[0].y;
   this->rx = point[1].x - point[0].x;
   this->ry = point[1].y - point[0].y;
}                               

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
}                                   

void CGCircle::CGCircle(TGColour stroke, TGColour fill,
                        int cx, int cy, int r) {
   CGPrimitive(this).CGPrimitive(stroke, fill);
   CGEllipse(this)->cx = cx;
   CGEllipse(this)->cy = cy;
   this->r = r;
}                      

void CGCircle::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   class:base.draw(canvas, extent, mode);
}                  

void CGCircle::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(2);

   ARRAY(point).data()[0].x = CGEllipse(this)->cx;
   ARRAY(point).data()[0].y = CGEllipse(this)->cy;
   ARRAY(point).data()[1].x = CGEllipse(this)->cx - this->r;
   ARRAY(point).data()[1].y = CGEllipse(this)->cy;
}                        

void CGLine::new(void) {
   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLine,marker_start>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLine,marker_end>, TRUE);      
}               

void CGLine::CGLine(TGColour stroke,
                     double x1, double y1, double x2, double y2) {
   CGPrimitive(this).CGPrimitive(stroke, GCOL_NONE);
   this->x1 = x1;
   this->y1 = y1;
   this->x2 = x2;
   this->y2 = y2;
}                  

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
}                

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
}                      

void CGLine::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(2);

   ARRAY(point).data()[0].x = this->x1;
   ARRAY(point).data()[0].y = this->y1;
   ARRAY(point).data()[1].x = this->x2;
   ARRAY(point).data()[1].y = this->y2;
}                      

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
}                      

void CGLine::create_point_set(TPoint *point) {
   this->x1 = point[0].x;
   this->y1 = point[0].y;
   this->x2 = point[1].x;
   this->y2 = point[1].y;
}                            

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
}                                

void CGPolyLine::new(void) {
   TAttribute *attr = ATTRIBUTE<CGPolyLine,point>;
                    
   attr->options |= (PO_ELEMENT_STRIPSPACES | PO_ELEMENT_PAIR);
   attr->delim = " ";

   class:base.new();

   ARRAY(&this->point).new();
                                          
   CGPrimitive(this)->stroke = GCOL_NONE;
   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPolyLine,marker_start>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPolyLine,marker_end>, TRUE);      
}                   

void CGPolyLine::CGPolyLine(TGColour stroke, TGColour fill,
                            int point_count, TPoint *point) {
   CGPrimitive(this).CGPrimitive(stroke, fill);

   ARRAY(&this->point).data_set(point_count, point);
}                          

void CGPolyLine::delete(void) {
   class:base.delete();

   ARRAY(&this->point).delete();
}                      

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
}                    

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
}                          

void CGPolyLine::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(ARRAY(&this->point).count());

   memcpy(ARRAY(point).data(), ARRAY(&this->point).data(), ARRAY(&this->point).count() * sizeof(TPoint));
}                          

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
}                          

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
}                                    

void CGPolyLine::point_add(TPoint *point) {
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGPolyLine,point>);
   ARRAY(&this->point).item_add(*point);
   CObjPersistent(this).attribute_update_end();
}                         

void CGPolygon::CGPolygon(TGColour stroke, TGColour fill,
                          int point_count, TPoint *point) {
   CGPolyLine(this).CGPolyLine(stroke, fill, point_count, point);

}                        

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
}                   

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
                                   
      while (*cp && isspace(*cp)) {
         cp++;
      }

      i = 0;
      while (*cp) {
         cpe = cp + 1;
                                                
         while (*cpe && isspace(*cpe)) {
            cpe++;
         }
                                                     
         cps = NULL;
         delim_count = 0;

                                           
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

      if (instruction->instruction == EGPathInstruction.Repeat &&
          CString(string).length() == 1) {
          instruction->instruction = EGPathInstruction.NoOp;
      }
      CString(&data).set(CString(string).string() + 1);
      switch (instruction->instruction) {
      case EGPathInstruction.NoOp:
         break;
      case EGPathInstruction.Repeat:
                                                     
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
                                          
   CGPrimitive(this)->stroke = GCOL_NONE;
}               

void CGPath::CGPath(TGColour stroke, TGColour fill) {
   CGPrimitive(this).CGPrimitive(stroke, fill);
}                  

void CGPath::delete(void) {
   class:base.delete();

   ARRAY(&this->instruction).delete();
}                  

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
}                

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
}                      

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
}                                

void CGText::new(void) {
   class:base.new();

   new(&this->text).CString(NULL);
   CString(&this->text).encoding_set(EStringEncoding.UTF16);
   new(&this->font_family).CGFontFamily();


}               

void CGText::CGText(TGColour fill,
                     double x, double y, const char *text) {
   CGPrimitive(this).CGPrimitive(GCOL_NONE, fill);
   this->x      = x;
   this->y      = y;
   CString(&this->text).set(text);
   CString(&this->text).encoding_set(EStringEncoding.UTF16);
}                  

void CGTextSpan::CGTextSpan(void) {
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGText,font_family>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGText,font_size>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,stroke>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPrimitive,fill>, TRUE);
}                          

void CGText::delete(void) {
   delete(&this->font_family);
   delete(&this->text);

   class:base.delete();
}                  

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
}                

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
}                      

void CGText::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(1);

   ARRAY(point).data()[0].x = this->x;
   ARRAY(point).data()[0].y = this->y;
}                      

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
}                      

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
}                                   

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
}                                

                                                                              
MODULE::END                                                                   
                                                                              

# 20 "/home/jacob/keystone/src/graphics/gprimcontainer.c"


                                                                              
MODULE::INTERFACE                                                             
                                                                              

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

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CGLayoutSymbol::new(void) {
   CGLayout(this)->frame_length = -1;                                       
   CGCanvas(this)->translate_palette = TRUE;

   class:base.new();
};
void CGLayoutSymbol::CGLayoutSymbol(int width, int height, CObjServer *obj_server, CObjPersistent *data_source) {
   CGLayout(this).CGLayout(width, height, obj_server, data_source);
};


void CGPrimitive::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));

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

         CGCanvas(layout).transform_set_gobject(CGObject(this), FALSE);
         CGObject(this).extent_set(CGCanvas(layout));
         CGCoordSpace(&CGCanvas(layout)->coord_space).extent_add(&this->redraw_extent, 1, &CGObject(this)->extent.point[0]);
         CGCoordSpace(&CGCanvas(layout)->coord_space).extent_add(&this->redraw_extent, 1, &CGObject(this)->extent.point[1]);
                                             


         CGCoordSpace(&CGCanvas(layout)->coord_space).reset();
         CGCanvas(layout).transform_viewbox();
         CGCanvas(layout).point_array_utov(2, this->redraw_extent.point);
         CGCanvas(layout).queue_draw(&this->redraw_extent);
      }
   }
}                                        

bool CGPrimitive::transform_assimilate(TGTransform *transform) {
   return TRUE;
}                                     

void CGSymbol::new(void) {
   class:base.new();

   new(&this->initial_path).CXPath(NULL, NULL);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGSymbol,ref_x>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGSymbol,ref_y>, TRUE);
}                 

void CGSymbol::CGSymbol(void) {
}                      

void CGSymbol::delete(void) {
   class:base.delete();
   delete(&this->initial_path);
}                    

void CGMarker::new(void) {
   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,view_box>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,marker_units>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,marker_width>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,marker_height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMarker,orient>, TRUE);
}                 

void CGMarker::CGMarker(void) {
}                      

void CGMarker::delete(void) {
   class:base.delete();
}                    

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
}                        

void CGPrimContainer::CGPrimContainer(void) {
}                                    

void CGPrimContainer::delete(void) {
   delete(&this->tooltiptext);
   delete(&this->font_family);

   class:base.delete();
}                           

void CGPrimContainer::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGCanvas *canvas;

   if (linkage == EObjectLinkage.ChildAdd) {
                                                          
      canvas = CGCanvas(CGObject(this)->native_object);

      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
         if (canvas) {
            CGCanvas(canvas).queue_draw(NULL);
         }

      }
   }

                    
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);
}                                          

void CGPrimContainer::show(bool show) {
   CObject *object = CObject(this).child_first();

                                              
   while (object) {
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
         CGObject(object).show(show);
      }
      object = CObject(this).child_next(object);
   }
}                             

void CGPrimContainer::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   CObject *object = CObject(this).child_first();
   CGCanvas *object_canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
#ifdef TEST   
   TRect gobj_extent, canvas_extent;
#endif

              
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
}                         

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
}                               

bool CGPrimContainer::transform_assimilate(TGTransform *transform) {
   bool result;

   CObjPersistent(this).attribute_update(ATTRIBUTE<CGObject,transform>);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGObject,transform>, FALSE);
   result = CGObject(this).transform_list_apply(&CGObject(this)->transform, transform);
   CObjPersistent(this).attribute_update_end();

   return result;
}                                         

void CGGroup::CGGroup(void) {
}                    

void CGGroup::delete(void) {
    class:base.delete();
}                   

void CGParameters::new(void) {
   new(&this->object_name).CString(NULL);
   class:base.new();
}                     

void CGParameters::CGParameters(void) {
}                              

void CGParameters::delete(void) {
   delete(&this->object_name);
   class:base.delete();
}                        

CObjClass *CGParameters::parameter_class(void) {
   return &class(CGParameter);
}                                 

void CGParameters::clear(void) {
   CObject *child, *next;

   child = CObject(this).child_first();
   while (child) {
      next = CObject(this).child_next(CObject(child));
      delete(child);
      child = next;
   }
}                       

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

                       
      if (CObjClass_is_derived(&class(CGParameters), CObject(child).obj_class())) {
                                          
         if (child == CObjPersistent(this)) {
                                                                       
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
                                                                                               
      parameter = CGParameter(CObject(this).child_first());
      while (parameter) {
         parameter->value_initial_valid = TRUE;



         parameter_next = CGParameter(CObject(this).child_next(CObject(parameter)));
         if (parameter->state == EParameterState.present) {
            delete(parameter);
         }
         parameter = parameter_next;
      }
   }
   
   CGParameters(this).export_defaults();   
}                        

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
}                                

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
}                                 

void CGParameterExport::new(void) {
   new(&this->export_name).CString(NULL);
}                          

void CGParameterExport::CGParameterExport(TAttributePtr *attributeName) {
   if (attributeName) {
      this->attribute_name = *attributeName;
   }
}                                        

void CGParameterExport::delete(void) {
   delete(&this->export_name);
}                             

void CGParameter::new(void) {
   new(&this->name).CString(NULL);
   new(&this->value).CString(NULL);
   new(&this->value_initial).CString(NULL);
   new(&this->export_name).CString(NULL);

   this->data_type = &ATTRIBUTE:type<CString>;

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGParameter,value>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGParameter,export_name>, TRUE);
}                    

void CGParameter::CGParameter(const char *name) {
   CString(&this->name).set(name);
}                            

void CGParameter::delete(void) {
   delete(&this->export_name);
   delete(&this->value_initial);
   delete(&this->value);
   delete(&this->name);
}                       

void CGParameter::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGUse *use = CGUse(CObject(this).parent_find(&class(CGUse)));

   if (!CObjPersistent(this).state_loading() && !changing && use && use->state == EGUseState.resolved) {
      CGParameters(use->active_parameters).rebind();
   }

   class:base.notify_attribute_update(attribute, changing);
}                                        

void CGParameter::value_initial_assimilate(const void *data) {
                                        
   CString value;

   new(&value).CString(NULL);
   (*this->data_type->convert)(NULL, &ATTRIBUTE:type<CString>, this->data_type,
                               -1, -1,
                               (void *)&value, data);

   CString(&this->value_initial).set_string(CString(&value));

   delete(&value);
}                                         

void CGParameter::value_set(CString *data) {
   if (data) {
      CString(&this->value).set_string(data);
   }
   else {
      CString(&this->value).set_string(&this->value_initial);
   }
}                          

void CGParameter::value_export_set(CGParameterExport *export) {
   CObjPersistent(CObject(export).parent()).attribute_set_string(export->attribute_name.attribute, &this->value);
}                                 

void CGUse::new(void) {
   class:base.new();

   this->active_parameters = &this->parameters;
   this->colour_background = GCOL_NONE;

   new(&this->link).CXLink(NULL, NULL);
   new(&this->symbolPath).CXPath(NULL, NULL);

   new(&this->parameters).CGParameters();
   CObject(&this->parameters).parent_set(CObject(this));
}              

void CGUse::CGUse(void) {
}                

void CGUse::delete(void) {
   delete(&this->parameters);

   delete(&this->symbolPath);
   delete(&this->link);

   class:base.delete();
}                 

void CGUse::realize(CObjPersistent *object) {
   CGSymbol *parent_symbol = CGSymbol(CObject(object).parent_find(&class(CGSymbol)));

                   
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
}                  

CObjPersistent *CGUse::symbol_resolve(void) {
                                                                
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
                                                              
             symbol = CObjPersistent(CObject(&CGLayout(layout)->defs).child_first());
         }
         if (symbol) {
            symbol = CObjPersistent(symbol).copy_fast(NULL, TRUE, TRUE);
         }

         if (!symbol) {
                                                  
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
}                         

void CGUse::resolve(void) {
   CObjPersistent *symbol;
   CGSymbol *parent_symbol = CGSymbol(CObject(this).parent_find(&class(CGSymbol)));
   CGLayout *layout_parent = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   CObjPersistent *object;
   CGDefs *defs = CGDefs(CObject(this).parent_find(&class(CGDefs)));
   CObject *child;

   if (defs || !layout_parent || CObject(this).parent_find(&class(CGLayoutSymbol))) {

                                    
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

      CObject(object).parent_set(CObject(this));             

      CXPath(&CGUse(this)->symbolPath).root_set_default(CObjPersistent(this));
      CXPath(&CGUse(this)->symbolPath).index_set(this->index);
      CXPath(&CGUse(this)->symbolPath).resolve();

      CXPath(&CGSymbol(object)->initial_path).root_set_default(CObjPersistent(this));
      if (parent_symbol) {
         CXPath(&CGSymbol(object)->initial_path).initial_set(&parent_symbol->initial_path.initial);
      }
      CXPath(&CGSymbol(object)->initial_path).index_set(this->index);
      CXPath(&CGSymbol(object)->initial_path).initial_set_path(CString(&CGUse(this)->symbolPath.path).string());


      CObject(object).parent_set(NULL);             
      CObject(this).child_add(CObject(object));
#if 1
                                                          
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
}                  

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
}                           

void CGUse::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   class:base.notify_object_linkage(linkage, object);

   if (linkage == EObjectLinkage.ParentSet) {
      CGUse(this).notify_all_update(FALSE);
   }
}                                

void CGUse::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
}                                  

void CGUse::notify_all_update(bool changing) {
   if (!changing && this->state == EGUseState.reloadNeeded) {
      CGUse(this).resolve();
   }
}                            

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
}                     

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
}               

void CGArray::new(void) {
   new(&this->selection).CXPath(NULL, NULL);
   class:base.new();
}                

void CGArray::delete(void) {
   class:base.delete();
   delete(&this->selection);
}                   

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
}                             

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

         CObject(object).parent_set(CObject(this));             
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

         }

         CObject(object).parent_set(NULL);             
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


                              
   CObjPersistent(layout).attribute_update(ATTRIBUTE<CGXULElement,width>);
   CObjPersistent(layout).attribute_update(ATTRIBUTE<CGXULElement,height>);
   CObjPersistent(layout).attribute_update_end();


   CGObject(this).show(TRUE);
}                    

void CGArray::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   if (!changing) {
      CGUse(this)->state = EGUseState.reloadNeeded;
      CGUse(this).resolve();
   }
   class:base.notify_attribute_update(attribute, changing);
}                                    

void CGLink::new(void) {
   class:base.new();
   

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGUse,x>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGUse,y>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGUse,width>, TRUE);   
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGUse,height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLink,show>, TRUE);
}               

void CGLink::CGLink(void) {
}                  

void CGLink::delete(void) {



   class:base.delete();
}                  

void CGLink::resolve(void) {
}                   


                                                                              
MODULE::END                                                                   
                                                                              

# 245 "/home/jacob/keystone/src/graphics/glayout.c"
#if LAYOUT_TOOLTIP

#endif
# 1 "/home/jacob/keystone/src/graphics/ganimationinput.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              




                                                                              
MODULE::INTERFACE                                                             
                                                                              

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

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              



void CGAnimateInput::new(void) {
   class:base.new();
}                       

void CGAnimateInput::delete(void) {
   class:base.delete();    
}                          

void CGAnimateInputEvent::CGAnimateInputEvent(void) {
}                                            

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
                                       
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update(CGAnimation(this)->binding.object.attr.attribute);
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update_end();
      }
   }

   if (CObject(event).obj_class() == &class(CEventPointer)) {
                                                 
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
                                       
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update(CGAnimation(this)->binding.object.attr.attribute);
         CObjPersistent(CGAnimation(this)->binding.object.object).attribute_update_end();
         return TRUE;
      default:
         break;      
      }
      return FALSE;
   }
   return FALSE;
}                              

                                     
bool CGAnimateInputSlider::event(CEvent *event) {
   TCoord position;
   CEventPointer *pointer;
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   int set_value;
   TCoord value;
   TRect *extent;

   if (CObject(event).obj_class() == &class(CEventPointer)) {
                                                 
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
}                               

                                                                              
MODULE::END                                                                   
                                                                              
# 249 "/home/jacob/keystone/src/graphics/glayout.c"

                                   
bool CGObject::keyboard_input(void) {
   CObject *child;

   if (CGObject(this)->visibility != EGVisibility.visible) {
      return FALSE;
   }

   child = CObject(this).child_first();
   while (child) {
      if (CObjClass_is_derived(&class(CGAnimateInput), CObject(child).obj_class())) {
         return TRUE;
      }
      child = CObject(this).child_next(CObject(child));
   }

   return FALSE;
}                            

void CGObject::select_area_get(CGCanvas *canvas, ARRAY<TPoint> *point) {
   TPoint *pt;
   TCoord extend = 20, length;                                                   
   TRect extent;

   if (CGCoordSpace(&canvas->coord_space).extent_empty(&this->extent)) {
      ARRAY(point).used_set(0);
   }
   else {
      extent = this->extent;
      length = extent.point[1].x - extent.point[0].x;
      if (length < extend) {
         extent.point[0].x -= (extend - length) / 2;
         extent.point[1].x += (extend - length) / 2;
      }
      length = extent.point[1].y - extent.point[0].y;
      if (length < extend) {
         extent.point[0].y -= (extend - length) / 2;
         extent.point[1].y += (extend - length) / 2;
      }

      ARRAY(point).used_set(4);
      pt = &ARRAY(point).data()[0];
      pt->x = extent.point[0].x;
      pt->y = extent.point[0].y;
      pt = &ARRAY(point).data()[1];
      pt->x = extent.point[1].x;
      pt->y = extent.point[0].y;
      pt = &ARRAY(point).data()[2];
      pt->x = extent.point[1].x;
      pt->y = extent.point[1].y;
      pt = &ARRAY(point).data()[3];
      pt->x = extent.point[0].x;
      pt->y = extent.point[1].y;
   }
}                             

void CGAnimation::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));

   switch (linkage) {
   case EObjectLinkage.ParentRelease:
      if (layout) {
         CGLayout(layout).animation_remove(CObjPersistent(this));
      }
      break;
   case EObjectLinkage.ParentSet:
      if (layout) {
         CGLayout(layout).animation_add(CObjPersistent(this));
      }
      break;
   default:
      break;
   }
   class:base.notify_object_linkage(linkage, object);
}                                      

bool CGXULElement::keyboard_input(void) {
   if (CObjClass_is_derived(&class(CGBox), CObject(this).obj_class())) {
       return FALSE;
   }
   if (CObjClass_is_derived(&class(CGTextLabel), CObject(this).obj_class())) {
       return FALSE;
   }
   if (CObjClass_is_derived(&class(CGScrollBar), CObject(this).obj_class())) {
       return FALSE;
   }
   if (CObjClass_is_derived(&class(CGScrolledArea), CObject(this).obj_class())) {
       return FALSE;
   }
   if (CObjClass_is_derived(&class(CGSplitter), CObject(this).obj_class())) {
       return FALSE;
   }
   if (this->disabled) {
       return FALSE;
   }
   if (CGObject(this)->visibility == EGVisibility.hidden) {
       return FALSE;
   }

   return TRUE;
}                                

TGColour CGCanvas::colour_background_default(void) {
    if (CObject(this).obj_class() == &class(CGLayoutTab)) {
       return GCOL_NONE;
    }
    return GCOL_WHITE;
}                                       

void CGSelection::new(void) {
   new(&this->fsm).CFsm(CObject(this), (STATE)&CGSelection::state_choose);

   new(&this->selection).CSelection(TRUE);
   new(&this->sibling_selection).CSelection(TRUE);
}                    

void CGSelection::CGSelection(CGLayout *layout, CObjServer *server) {
   CObject(this).parent_set(CObject(layout));

   this->server = server;
}                            

void CGSelection::delete(void) {
   delete(&this->sibling_selection);
   delete(&this->selection);
}                       

void CGSelection::pointer_event(CEventPointer *event, CObject *parent) {
   ARRAY<TObjectPtr> *selection;
   CGObject *gobject;
   CObject *child, *next_child;
   CGLayout *layout = CGLayout(CObject(this).parent());
   bool handled, key_clear;
   int i;

#if LAYOUT_TOOLTIP
   CGRect *border_rect;
   CGText *text;
   CGWindow *parent_window = CGWindow(CObject(layout).parent_find(&class(CGWindow)));
   TPoint position;
   int width, height;
   bool selected_items;
   const TAttribute *tooltip_attr;
   CString tooltip_text;

   selected_items = CGSelection(this).select_area(parent);

   selection = CSelection(&this->sibling_selection).selection();
   gobject = ARRAY(selection).count() ? CGObject(ARRAY(selection).data()[0].object) : NULL;

   if (event->type == EEventPointer.Leave && layout->tooltip_window) {
      delete(layout->tooltip_window);
      layout->tooltip_window = NULL;
      layout->tooltip_gobject = NULL;
   }

   if (event->type != EEventPointer.Hover) {

      if (layout->tooltip_window && (!gobject ||
          !(CObject(gobject).is_child(CObject(layout->tooltip_gobject)) || CObject(layout->tooltip_gobject).is_child(CObject(gobject))))) {
         delete(layout->tooltip_window);
         layout->tooltip_window = NULL;
         layout->tooltip_gobject = NULL;
      }
   }

   new(&tooltip_text).CString(NULL);
   tooltip_attr = NULL;
   if (gobject) {

                                                           
      tooltip_attr = CObjPersistent(gobject).attribute_find("tooltiptext");
      if (tooltip_attr) {
         CObjPersistent(gobject).attribute_get_string(tooltip_attr, &tooltip_text);

         if (!CString(&tooltip_text).length()) {
             tooltip_attr = NULL;
         }
      }
   }

   if (parent_window && event->type == EEventPointer.Hover && CString(&tooltip_text).length()) {
      layout->tooltip_gobject = gobject;
      position = event->position;
      CGCanvas(layout).point_array_utov(1, &position);
      CGCanvas(layout).point_ctos(&position);
      if (layout->tooltip_window) {
         delete(layout->tooltip_window);
         layout->tooltip_window = NULL;
      }

      layout->tooltip_layout = (CGObject *) new.CGLayout(0, 0, NULL, NULL);
      CObjPersistent(layout->tooltip_layout).attribute_default_set(ATTRIBUTE<CGCanvas,colour_background>, FALSE);
      CGCanvas(layout->tooltip_layout)->colour_background = (GCOL_TYPE_NAMED | EGColourNamed.lightgoldenrodyellow);

      layout->tooltip_window = (CGObject *) new.CGWindow(NULL, CGCanvas(layout->tooltip_layout), parent_window);

      text = new.CGText(GCOL_BLACK, 4, 12, CString(&tooltip_text).string());

      CObjPersistent(text).attribute_default_set(ATTRIBUTE<CGText,font_family>, TRUE);
      CObjPersistent(text).attribute_set_int(ATTRIBUTE<CGText,font_size>, 12);
      CObject(layout->tooltip_layout).child_add(CObject(text));
      CGObject(text).extent_set(CGCanvas(layout->tooltip_layout));
      width = (int)(CGObject(text)->extent.point[1].x - CGObject(text)->extent.point[0].x + 6);
      height = (int)(CGObject(text)->extent.point[1].y - CGObject(text)->extent.point[0].y + 2);
      border_rect = new.CGRect(GCOL_BLACK, GCOL_NONE, 0, 0, width - 1, height - 1);
      CObject(layout->tooltip_layout).child_add(CObject(border_rect));

      CGWindow(layout->tooltip_window).position_size_set((int)position.x, (int)position.y + 16, width, height);
      CGWindow(layout->tooltip_window).show(TRUE);
   }
   delete(&tooltip_text);




#endif

   if (layout->selected_held) {
      if (CObjClass_is_derived(&class(CGAnimation), CObject(layout->selected_held).obj_class())) {
         CGAnimation(layout->selected_held).event(CEvent(event));
      }
      return;
   }

   CGSelection(this)->area.point[0].x = event->position.x;
   CGSelection(this)->area.point[0].y = event->position.y;
   CGSelection(this)->area.point[1].x = event->position.x;
   CGSelection(this)->area.point[1].y = event->position.y;

   key_clear = FALSE;
   if (selected_items) {
      selection = CSelection(&this->sibling_selection).selection();

      gobject = NULL;
      for (i = ARRAY(selection).count() - 1; i >= 0; i--) {
         gobject = CGObject(ARRAY(selection).data()[i].object);


                                                                                  
         if (CGObject(gobject).keyboard_input() && event->type == EEventPointer.LeftDown) {
            CGLayout(layout).key_gselection_set(CGObject(gobject));
         }

                                                             
         handled = FALSE;
         child = CObject(gobject).child_first();
         while (child && !handled) {
         next_child = CObject(gobject).child_next(child);
            if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class())) {
               handled = CGAnimation(child).event(CEvent(event));
            }
            else if (CObjClass_is_derived(&class(CGLink), CObject(child).obj_class())) {
               handled = CGObject(child).event(CEvent(event));
            }
            child = next_child;
         }

                                     
         if (!handled) {
            handled = CGObject(gobject).event(CEvent(event));
         }

                                                                                  
         if (event->type == EEventPointer.LeftUp && CGObject(gobject).keyboard_input()) {
            key_clear = TRUE;
         }

         if (handled) {
            return;
         }
      }




      if (gobject && (!CObjClass_is_derived(&class(CGXULElement), CObject(gobject).obj_class()) && key_clear)) {
         CGLayout(layout).key_gselection_set(NULL);
      }
      
      if (gobject && (!CObjClass_is_derived(&class(CGXULElement), CObject(gobject).obj_class()) || 
          CObjClass_is_derived(&class(CGContainer), CObject(gobject).obj_class()))) {

                                                      
         if (!tooltip_attr) {
                                                                  
           CGSelection(this).pointer_event(event, CObject(gobject));
         }
      }
   }
   else {
                                                            
                                                 
   CGCanvas(layout).pointer_cursor_set(EGPointerCursor.default);
   }
}                              

void CGSelection::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   bool in_key = CFsm(&this->fsm).in_state((STATE)&CGSelection::state_choose_key);
   ARRAY<int> dash_array;
   TObjectPtr *object_selection;
   CGObject *gobject;

    if (CSelection(&this->selection).count() != 1)
      return;

   object_selection = &ARRAY(CSelection(&this->selection).selection()).data()[0];

   if (in_key && CObjClass_is_derived(&class(CGObject), CObject(object_selection->object).obj_class()) &&
       !CObjClass_is_derived(&class(CGXULElement), CObject(object_selection->object).obj_class())) {
      gobject = CGObject(object_selection->object);
      if (this->bold_highlight) {
         CGCanvas(canvas).NATIVE_stroke_style_set(5, NULL, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
         CGCanvas(canvas).xor_write_set(TRUE);
         CGCanvas(canvas).draw_rectangle(GCOL_WHITE, FALSE,
                                         gobject->extent.point[0].x, gobject->extent.point[0].y,
                                         gobject->extent.point[1].x, gobject->extent.point[1].y);
         CGCanvas(canvas).xor_write_set(FALSE);
      }
      else {
         ARRAY(&dash_array).new();
         ARRAY(&dash_array).used_set(2);
         ARRAY(&dash_array).data()[0] = 1;
         ARRAY(&dash_array).data()[1] = 1;
         CGCanvas(canvas).NATIVE_stroke_style_set(1, &dash_array, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
          CGCanvas(canvas).xor_write_set(TRUE);
         CGCanvas(canvas).draw_rectangle(GCOL_WHITE, FALSE,
                                         gobject->extent.point[0].x, gobject->extent.point[0].y,
                                         gobject->extent.point[1].x, gobject->extent.point[1].y);
                              
         CGCanvas(canvas).xor_write_set(FALSE);
         ARRAY(&dash_array).delete();
      }
   }
}                     

STATE CGSelection::state_choose(CEvent *event) {
   CEventPointer *pointer;
   CGLayout *layout = CGLayout(CObject(this).parent());

   if (CObject(event).obj_class() == &class(CEventPointer)) {
      pointer = CEventPointer(event);

                                                       
      CGCoordSpace(&CGCanvas(layout)->coord_space).reset();
      CGCanvas(layout).transform_viewbox();
      CGCanvas(layout).point_array_vtou(1, (TPoint *)&pointer->position);

      CGSelection(this).pointer_event(CEventPointer(event), CObject(layout));
   }

   return (STATE)&CFsm:top;
}                             

STATE CGSelection::state_choose_key(CEvent *event) {
   CGLayout *layout = CGLayout(CObject(this).parent()), *parent_layout;
   CObjPersistent *child = NULL, *next, *test;
   int i = 0, j, count;
   int diff_a = 0, diff_b = 0, diff_test_a = 0, diff_test_b = 0;
   bool handled;
   static TRect extent;

   if (CObject(event).obj_class() == &class(CEventKey)) {
      if (CEventKey(event)->type != EEventKeyType.down ||
         CEventKey(event)->modifier & (1 << EEventModifier.ctrlKey) ||
         CEventKey(event)->modifier & (1 << EEventModifier.altKey)) {
         return (STATE)&CFsm:top;
      }

                                                                 
      CGSelection(this).select_input();
      count = ARRAY(CSelection(&this->sibling_selection).selection()).count();

      if (!count) {
         parent_layout = CGLayout(CObject(layout).parent_find(&class(CGLayout)));
         if (parent_layout && event->source != CObject(parent_layout)) {
                                                    
            ARRAY(CSelection(&this->selection).selection()).used_set(0);
            CGLayout(parent_layout).key_gselection_set(CGObject(layout));
                                              
            if (CEventKey(event)->key == EEventKey.Up) {
                CEventKey(event)->key = EEventKey.Tab;
                CEventKey(event)->modifier = (1 << EEventModifier.shiftKey);
            }
                                              
            if (CEventKey(event)->key == EEventKey.Down) {
               CEventKey(event)->key = EEventKey.Tab;
               CEventKey(event)->modifier = 0;
            }
            CGLayout(parent_layout).event(event);
         }
         return (STATE)&CFsm:top;
      }
      else {
         if (CSelection(&this->selection).count() != 1) {
            child = NULL;
         }
         else {
            for (i = 0; i < count; i++) {
               child = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i].object;
               if (child == ARRAY(CSelection(&this->selection).selection()).data()[0].object) {
                  break;
               }
            }
            if (i == count) {
               child = NULL;
            }
         }
      }

      switch (CEventKey(event)->key) {
      case EEventKey.Up:
      case EEventKey.Down:
      case EEventKey.Tab:
      case EEventKey.Home:
      case EEventKey.End:
         if (child) {
            if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class())) {
               CGXULElement(child).NATIVE_focus_out();
               CGSelection(this).select_input();
            }
         }
         break;
      default:
         break;
      }

      next = child;
      switch (CEventKey(event)->key) {
      case EEventKey.Left:
         if (child &&
             (CObjClass_is_derived(&class(CGTextBox), CObject(child).obj_class()) ||
              CObjClass_is_derived(&class(CGTabBox), CObject(child).obj_class()))) {
             break;
         }
      case EEventKey.Up:
         if (!child) {
            next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[(count - 1)].object;
            goto update;
         }
         goto move;
      case EEventKey.Right:
         if (child &&
             (CObjClass_is_derived(&class(CGTextBox), CObject(child).obj_class()) ||
              CObjClass_is_derived(&class(CGTabBox), CObject(child).obj_class()))) {
             break;
         }
      case EEventKey.Down:
         if (!child) {
            next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object;
            goto update;
         }
         goto move;
      move:
         next = NULL;

                                     
         if (CObjClass_is_derived(&class(CGTabBox), CObject(child).obj_class()) &&
             CEventKey(event)->key == EEventKey.Down) {
            next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i + 1].object;
            goto update;
         }
         if (CObjClass_is_derived(&class(CGLayoutTab), CObject(child).obj_class())) {
            child = CObjPersistent(CObject(child).parent());
         }

         for (j = 0; j < count; j++) {
            test = ARRAY(CSelection(&this->sibling_selection).selection()).data()[j].object;
            if (CObjClass_is_derived(&class(CGLayoutTab), CObject(test).obj_class())) {
               continue;
            }
            switch (CEventKey(event)->key) {
            case EEventKey.Up:
               diff_test_a = (int)CGObject(child)->extent.point[0].y - (int)CGObject(test)->extent.point[0].y;
               diff_test_b = abs((int)CGObject(test)->extent.point[0].x - (int)CGObject(child)->extent.point[0].x);
               break;
            case EEventKey.Down:
               diff_test_a = (int)CGObject(test)->extent.point[0].y - (int)CGObject(child)->extent.point[0].y;
               diff_test_b = abs((int)CGObject(test)->extent.point[0].x - (int)CGObject(child)->extent.point[0].x);
               break;
            case EEventKey.Left:
               diff_test_a = (int)CGObject(child)->extent.point[0].x - (int)CGObject(test)->extent.point[0].x;
               diff_test_b = abs((int)CGObject(test)->extent.point[0].y - (int)CGObject(child)->extent.point[0].y);
               break;
            case EEventKey.Right:
               diff_test_a = (int)CGObject(test)->extent.point[0].x - (int)CGObject(child)->extent.point[0].x;
               diff_test_b = abs((int)CGObject(test)->extent.point[0].y - (int)CGObject(child)->extent.point[0].y);
               break;
            default:
               break;
            }

            if (diff_test_a > 0) {
                                
               if (!next || diff_test_b < diff_b ||
                   ((diff_test_b == diff_b) && (diff_test_a < diff_a))) {
                                                       
                  next = test;
                  diff_a = diff_test_a;
                  diff_b = diff_test_b;
               }
            }
         }

         if (next && CObjClass_is_derived(&class(CGTabBox), CObject(next).obj_class()) &&
            CEventKey(event)->key == EEventKey.Up) {
            next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i - 1].object;
         }
         goto update;
      case EEventKey.Tab:
         next = NULL;
         if ((CEventKey(event)->modifier)) {
            if (!child) {
               next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[(count - 1)].object;
            }
            else {
               next = i > 0 ? ARRAY(CSelection(&this->sibling_selection).selection()).data()[i - 1].object : NULL;
            }
         }
         else {
            if (!child) {
               next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object;
            }
            else {
               next = i < (count - 1) ? ARRAY(CSelection(&this->sibling_selection).selection()).data()[i + 1].object : NULL;
            }
         }
         goto update;
      case EEventKey.Home:
         next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object;
         goto update;
      case EEventKey.End:
         next = ARRAY(CSelection(&this->sibling_selection).selection()).data()[(count - 1)].object;
         goto update;
      update:
                                             
         if (child) {
            if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class())) {
            }
            else {
               parent_layout = CGLayout(CObject(child).parent_find(&class(CGLayout)));
               extent = CGObject(child)->extent;
               CGCoordSpace(&CGCanvas(parent_layout)->coord_space).reset();
               CGCanvas(parent_layout).transform_viewbox();
               CGCanvas(parent_layout).point_array_utov(2, extent.point);
               CGCanvas(parent_layout).queue_draw(&extent);
            }
         }

         if (!next) {
            parent_layout = CGLayout(CObject(layout).parent_find(&class(CGLayout)));
            if (parent_layout) {
                                                       
               ARRAY(CSelection(&this->selection).selection()).used_set(0);
               CGLayout(parent_layout).key_gselection_set(CGObject(layout));

                                                 
               if (CEventKey(event)->key == EEventKey.Up) {
                   CEventKey(event)->key = EEventKey.Tab;
                   CEventKey(event)->modifier = (1 << EEventModifier.shiftKey);
               }
                                                 
               if (CEventKey(event)->key == EEventKey.Down) {
                   CEventKey(event)->key = EEventKey.Tab;
                   CEventKey(event)->modifier = 0;
               }
               CGLayout(parent_layout).event(event);
            }
            else {
                                                                     
               ARRAY(CSelection(&this->selection).selection()).used_set(1);
               ARRAY(CSelection(&this->selection).selection()).data()[0].object = next;
               CGLayout(layout).event(event);
            }
            return (STATE)&CFsm:top;
         }

         if (CObjClass_is_derived(&class(CGCanvas), CObject(next).obj_class())) {
                                                   
            ARRAY(CSelection(&this->selection).selection()).used_set(0);
            CGLayout(next).key_gselection_set(NULL);
            CGLayout(next).event(event);
            return (STATE)&CFsm:top;
         }

                                  
         if (!CObjClass_is_derived(&class(CGXULElement), CObject(next).obj_class())) {
             parent_layout = CGLayout(CObject(next).parent_find(&class(CGLayout)));
                                                                                       
             CGLayout(parent_layout).key_gselection_set(CGObject(next));
             extent = CGObject(next)->extent;
             CGCoordSpace(&CGCanvas(parent_layout)->coord_space).reset();
             CGCanvas(parent_layout).transform_viewbox();
             CGCanvas(parent_layout).point_array_utov(2, extent.point);
             CGCanvas(parent_layout).queue_draw(&extent);
         }
         if (CObjClass_is_derived(&class(CGXULElement), CObject(next).obj_class())) {
            CGXULElement(next).NATIVE_focus_in();
         }
         ARRAY(CSelection(&this->selection).selection()).used_set(1);
         ARRAY(CSelection(&this->selection).selection()).data()[0].object = next;
         break;
      default:
                                                
         if (child) {
            handled = FALSE;
            next = CObjPersistent(CObject(child).child_first());
            while (next && !handled) {
               if (CObjClass_is_derived(&class(CGAnimation), CObject(next).obj_class())) {
                  handled = CGAnimation(next).event(CEvent(event));
               }
               next = CObjPersistent(CObject(child).child_next(CObject(next)));
            }
         }
         break;
      }
   }
   return (STATE)&CFsm:top;
}                                 

void CGSelection::select_clear(void) {
   CObjServer(this->server).root_selection_update();
   CSelection(&this->selection).clear();
   CObjServer(this->server).root_selection_update_end();
}                             

bool CGSelection::select_area(CObject *parent) {
   CObject *object;
   CObjPersistent *embedded;
   CGLayout *layout = CGLayout(CObject(this).parent());
   bool server_update = FALSE;
   ARRAY<TPoint> point;

                                                                                  
   if (this->server && CObject(this->server->server_root).is_child(parent)) {
      server_update = TRUE;
   }

   CSelection(&this->sibling_selection).clear();

                                     
   if (CSelection(&this->selection).count()) {
      if (server_update) {
         CObjServer(this->server).root_selection_update();
      }
      CSelection(&this->selection).clear();
      if (server_update) {
         CObjServer(this->server).root_selection_update_end();
      }
   }

   ARRAY(&point).new();

   embedded = CObjPersistent(parent).child_element_class_find(&class(CGPrimContainer), NULL);

                                                                
   if (server_update) {
      CObjServer(this->server).root_selection_update();
   }
   object = CObject(parent).child_last();
   while (object) {
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {

                                                                                                 
         if (!CFsm(&layout->fsm).in_state((STATE)&CGLayout::state_animate) ||
             (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class()))) {
            CGCanvas(layout).transform_set_gobject(CGObject(object), FALSE);
            CGObject(object).extent_set(CGCanvas(layout));
         }
         if (!CGCoordSpace(&CGCanvas(layout)->coord_space).extent_empty(&CGObject(object)->extent) &&
             this->area.point[0].x <= CGObject(object)->extent.point[0].x &&
             this->area.point[1].x >= CGObject(object)->extent.point[1].x &&
             this->area.point[0].y <= CGObject(object)->extent.point[0].y &&
             this->area.point[1].y >= CGObject(object)->extent.point[1].y) {
             CSelection(&this->selection).add(CObjPersistent(object), NULL);
         }
      }
      if (object == CObject(embedded)) {
         object = NULL;
      }
      else {
         object = CObject(parent).child_previous(object);
         if (!object) {
            object = CObject(embedded);
         }
      }
   }
   if (server_update)
      CObjServer(this->server).root_selection_update_end();

   if (CSelection(&this->selection).count() == 0) {
                                                                                               
      object = CObject(parent).child_last();
      while (object) {
         if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class()) &&
             CGObject(object)->visibility != EGVisibility.hidden) {





            CGObject(object).select_area_get(CGCanvas(layout), &point);
            if (CGCoordSpace(&CGCanvas(layout)->coord_space).rect_inside_area(&this->area, &point)) {
                CSelection(&this->sibling_selection).add(CObjPersistent(object), NULL);
            }
         }

         if (object == CObject(embedded)) {
            object = NULL;
         }
         else {
            object = CObject(parent).child_previous(object);
            if (!object) {
               object = CObject(embedded);
            }
         }
      }
   }
   ARRAY(&point).delete();

   if (CSelection(&this->sibling_selection).count()) {
      if (server_update) {
         CObjServer(this->server).root_selection_update();
      }
      CSelection(&this->selection).add(ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object, NULL);
      if (server_update) {
         CObjServer(this->server).root_selection_update_end();
      }
      return TRUE;
   }
   return CSelection(&this->selection).count() != 0;
}                            

bool CGSelection::select_input(void) {
                                                          
   CObjPersistent *child;
   CObject *parent;
   CGLayout *layout = CGLayout(CObject(this).parent());

   CSelection(&this->sibling_selection).clear();

   child = CObjPersistent(CObject(layout).child_tree_first());
   while (child) {
      if (CObjClass_is_derived(&class(CGObject), CObject(child).obj_class()) &&
         (CGObject(child).keyboard_input()                                                                             ) ) {
         CSelection(&this->sibling_selection).add(CObjPersistent(child), NULL);
      }
      if (CObjClass_is_derived(&class(CGCanvas), CObject(child).obj_class())) {
                                         
         parent = CObject(child).parent();
         if (CObjClass_is_derived(&class(CGTabBox), CObject(parent).obj_class())) {
            child = CObjPersistent(CObject(CObject(parent).parent()).child_next(CObject(parent)));
         }
         else {
            child = CObjPersistent(CObject(parent).child_next(CObject(child)));
         }
      }
      else {
         if (CObjClass_is_derived(&class(CGTabBox), CObject(child).obj_class())) {
            child = CObjPersistent(CObject(child).child_n(CGTabBox(child)->selectedIndex));
         }
         else {
            child = CObjPersistent(CObject(layout).child_tree_next(CObject(child)));
         }
      }







   }

   return TRUE;
}                             

bool CGSelection::select_sibling_next(void) {
   int i, count;
   CObjPersistent *object;

   count = ARRAY(CSelection(&this->sibling_selection).selection()).count();

   for (i = 0; i < count - 1; i++) {
      object = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i].object;
      if (object == ARRAY(CSelection(&this->selection).selection()).data()[0].object) {
         object = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i + 1].object;
         CObjServer(this->server).root_selection_update();
         CSelection(&this->selection).clear();
         CSelection(&this->selection).add(object, NULL);
         CObjServer(this->server).root_selection_update_end();
         return TRUE;
      }
   }
   return FALSE;
}                                    

bool CGSelection::select_sibling_previous(void) {
   int i, count;
   CObjPersistent *object;

   count = ARRAY(CSelection(&this->sibling_selection).selection()).count();

   for (i = count - 1; i > 0; i--) {
      object = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i].object;
      if (object == ARRAY(CSelection(&this->selection).selection()).data()[0].object) {
         object = ARRAY(CSelection(&this->sibling_selection).selection()).data()[i - 1].object;
         CObjServer(this->server).root_selection_update();
         CSelection(&this->selection).clear();
         CSelection(&this->selection).add(object, NULL);
         CObjServer(this->server).root_selection_update_end();
         return TRUE;
      }
   }
   return FALSE;
}                                        

bool CGSelection::select_sibling_first(void) {
   int count;

   count = ARRAY(CSelection(&this->sibling_selection).selection()).count();
   if (count) {
      CObjServer(this->server).root_selection_update();
      CSelection(&this->selection).clear();
      CSelection(&this->selection).add(ARRAY(CSelection(&this->sibling_selection).selection()).data()[0].object, NULL);
      CObjServer(this->server).root_selection_update_end();
      return TRUE;
   }
   return FALSE;
}                                     

bool CGSelection::select_sibling_last(void) {
   int count;

   count = ARRAY(CSelection(&this->sibling_selection).selection()).count();
   if (count) {
      CObjServer(this->server).root_selection_update();
      CSelection(&this->selection).clear();
      CSelection(&this->selection).add(ARRAY(CSelection(&this->sibling_selection).selection()).data()[count - 1].object, NULL);
      CObjServer(this->server).root_selection_update_end();
      return TRUE;
   }
   return FALSE;
}                                    

void CGSelection::clear(void) {
   CSelection(&this->selection).clear();
}                      

void CGSelection::delete_selected(void) {


   if (this->server) {
      CObjServer(this->server).root_selection_update();
   }
   CSelection(&this->selection).delete_selected();
   if (this->server) {
      CObjServer(this->server).root_selection_update_end();
   }
}                                

void CGSelection::selection_update(void) {
   CObjPersistent *object;
   int count, i;
   CGLayout *layout = CGLayout(CObject(this).parent());

   count = ARRAY(CSelection(&this->selection).selection()).count();
   for (i = 0; i < count; i++) {
      object = ARRAY(CSelection(&this->selection).selection()).data()[i].object;
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {

                                                                                              
         if (!CFsm(&layout->fsm).in_state((STATE)&CGLayout::state_animate)) {
            CGCanvas(layout).transform_set_gobject(CGObject(object), FALSE);
            CGObject(object).extent_set(CGCanvas(layout));
         }

         if (i == 0) {
            this->area = CGObject(object)->extent;
         }
         else {
            if (CGObject(object)->extent.point[0].x < this->area.point[0].x)
               this->area.point[0].x = CGObject(object)->extent.point[0].x;
            if (CGObject(object)->extent.point[0].y < this->area.point[0].y)
               this->area.point[0].y = CGObject(object)->extent.point[0].y;
            if (CGObject(object)->extent.point[1].x > this->area.point[1].x)
               this->area.point[1].x = CGObject(object)->extent.point[1].x;
            if (CGObject(object)->extent.point[1].y > this->area.point[1].y)
               this->area.point[1].y = CGObject(object)->extent.point[1].y;
         }
      }
   }

   CGCanvas(layout).queue_draw(NULL);
}                                 

void CObjClientGLayout::new(void) {
   ARRAY(&this->pending_object).new();
   ARRAY(&this->pending_object).used_set(1000);
   ARRAY(&this->pending_object).used_set(0);
   class:base.new();
}                          

void CObjClientGLayout::CObjClientGLayout(CObjServer *obj_server, CObject *host,
                                          CObjPersistent *object_root, CGLayout *glayout) {
   this->glayout = glayout;
   CObjClient(this).CObjClient(obj_server, host);
   CObjClient(this).object_root_add(object_root);
}                                        

void CObjClientGLayout::delete(void) {
   class:base.delete();
   ARRAY(&this->pending_object).delete();
}                             

void CObjClientGLayout::notify_select(void) {
}                                    

void CObjClientGLayout::notify_all_update(void) {
}                                        

void CObjClientGLayout::iterate(void) {
   int i;
   CObjPersistent *object;

   CMutex(&this->glayout->update_mutex).lock();
#if 0
   for (i = 0; i < ARRAY(&this->pending_object).count(); i++) {
      CObjClientGLayout(this).resolve_animation(ARRAY(&this->pending_object).data()[i]);
   }
#else
   if (ARRAY(&this->pending_object).count()) {


      for (i = 0; i < ARRAY(&this->glayout->data_animation).count(); i++) {
         object = ARRAY(&this->glayout->data_animation).data()[i];
         CGLayout(this->glayout).animation_resolve(object);
      }
   }
#endif
   ARRAY(&this->pending_object).used_set(0);
   CMutex(&this->glayout->update_mutex).unlock();
}                              

void CObjClientGLayout::resolve_animation(void *data) {
   CObjPersistent *object = CObjPersistent(data);
   CObjPersistent *data_object;
   int i;

   if (CFsm(&CGLayout(this->glayout)->fsm).in_state((STATE)&CGLayout::state_animate)) {
      if (object) {
         for (i = 0; i < ARRAY(&this->glayout->data_animation).count(); i++) {
            data_object = ARRAY(&this->glayout->data_animation).data()[i];
            if (CObjClass_is_derived(&class(CGXULElement), CObject(data_object).obj_class())) {
               if (CGXULElement(data_object)->binding.object.object == object) {
                  CGLayout(this->glayout).animation_resolve(data_object);
               }
            }
            if (CObjClass_is_derived(&class(CGAnimation), CObject(data_object).obj_class())) {
               if (CGAnimation(data_object)->binding.object.object == object) {
                  CGLayout(this->glayout).animation_resolve(data_object);
               }
            }
         }
      }
      else {
         CGLayout(this->glayout).animate();
      }
   }
}                                        

void CObjClientGLayout::notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing) {
   if (CFsm(&this->glayout->fsm).in_state((STATE)&CGLayout::state_animate)) {
      if (!CMutex(&this->glayout->update_mutex).trylock()) {
         ARRAY(&this->pending_object).item_add(object);

         CMutex(&this->glayout->update_mutex).unlock();
      }
      else {

      }
   }
}                                                     

void CObjClientGLayout::notify_object_child_add(CObjPersistent *object, CObjPersistent *child) {
   if (CFsm(&this->glayout->fsm).in_state((STATE)&CGLayout::state_animate)) {
      ARRAY(&this->pending_object).item_add(NULL);
   }
}                                              

void CObjClientGLayout::notify_object_child_remove(CObjPersistent *object, CObjPersistent *child) {

                                                                          
   if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class()) ||
       CObjClass_is_derived(&class(CGPrimitive), CObject(child).obj_class())) {
      if (object == ARRAY(&CObjClient(this)->object_root).data()[0]) {
                                 
         return;
      }
                                                                                          
      if (child == ARRAY(&CObjClient(this)->object_root).data()[0]) {
         CGLayout(this->glayout).show(FALSE);
         delete(this->glayout);
         return;
      }
   }

                                                         
   if (CFsm(&this->glayout->fsm).in_state((STATE)&CGLayout::state_animate)) {
      ARRAY(&this->pending_object).item_add(NULL);
   }
}                                                 

void CGDefs::new(void) {
   class:base.new();
   new(&this->palette).CGPalette(NULL);
   CObjPersistent(&this->palette).attribute_default_set(ATTRIBUTE<CGPalette,colour>, FALSE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGDefs,palette>, TRUE);
   CObject(&this->palette).parent_set(CObject(this));
}               

void CGDefs::CGDefs(void) {
}                  

void CGDefs::delete(void) {
   delete(&this->palette);
   class:base.delete();
}                  

void CGDefs::show(bool show) {
}                

void CGLayout::new(void) {
   class:base.new();

   new(&this->update_mutex).CMutex();

   CGCanvas(this).CGCanvas(0, 0);

   ARRAY(&this->time_animation).new();
   ARRAY(&this->data_animation).new();

   new(&this->defs).CGDefs();
   CObject(&this->defs).parent_set(CObject(this));
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLayout,defs>, TRUE);

   CGCanvas(this).palette_set(&this->defs.palette);

   new(&this->hover_gselection).CGSelection(this, NULL);
   new(&this->key_gselection).CGSelection(this, NULL);
   CFsm(&this->key_gselection.fsm).transition((STATE)&CGSelection::state_choose_key);

   if (this->frame_length != -1) {
      this->frame_length = 25;
   }
   new(&this->fsm).CFsm(CObject(this), this->frame_length == -1 ? (STATE)&CGLayout::state_freeze : (STATE)&CGLayout::state_animate);
   CFsm(&this->fsm).init();

   new(&this->filename).CString(NULL);
   new(&this->title).CString(NULL);
   new(&this->xmlns).CString(NULL);
   new(&this->xmlns_xlink).CString(NULL);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLayout,xmlns>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLayout,xmlns_xlink>, TRUE);

   new(&this->canvas_bitmap).CGCanvasBitmap(1, 1);
                                             

}                 

void CGLayout::CGLayout(int width, int height, CObjServer *obj_server, CObjPersistent *data_source) {
   if (width != 0 && height != 0) {
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>,  FALSE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGXULElement,width>, width);
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGXULElement,height>, height);
   }
   if (obj_server) {

      new(&this->client).CObjClientGLayout(obj_server, CObject(this), data_source, this);
   }

   this->hover_gselection.server = obj_server;
}                      

void CGLayout::delete(void) {
   CEventPointer event;

                                                                   
   delete(&this->canvas_bitmap);

   if (CGObject(this)->native_object) {
                                                                   
      if (CGXULElement(this)->width != 0 && CGXULElement(this)->height != 0) {
         new(&event).CEventPointer(EEventPointer.Leave, 0, 0, 0);
         CGObject(this).event(CEvent(&event));
         delete(&event);
      }
   }

   CFsm(&this->fsm).transition((STATE)&CGLayout::state_freeze);
   CGLayout(this).key_gselection_set(NULL);

   class:base.delete();

   if (CObject_exists((CObject *)&this->client)) {
      delete(&this->client);
   }
   
   delete(&this->defs);

   delete(&this->xmlns_xlink);
   delete(&this->xmlns);
   delete(&this->title);
   delete(&this->filename);


   delete(&this->hover_gselection);
   delete(&this->key_gselection);
   delete(&this->fsm);
   ARRAY(&this->data_animation).delete();
   ARRAY(&this->time_animation).delete();

   delete(&this->update_mutex);
}                    

void CGLayout::notify_file_load(const char *filename) {}

void CGLayout::iterate(void) {
   CObjPersistent *object;
   int i;
   CEventPointer event;

   if (this->hover_timeout > 0) {
       this->hover_timeout -= this->frame_length;
       if (this->hover_timeout <= 0) {
                                                   
          new(&event).CEventPointer(EEventPointer.Hover, (int)this->hover_position.x, (int)this->hover_position.y, 0);
          CGObject(this).event(CEvent(&event));
          delete(&event);
       }
   }

   this->relative_time += ((double)this->frame_length) / 1000;

   if (CObject_exists((CObject *)&this->client)) {
       CObjClientGLayout(&this->client).iterate();
   }
   for (i = 0; i < ARRAY(&this->time_animation).count(); i++) {
      object = ARRAY(&this->time_animation).data()[i];
      CGLayout(this).animation_resolve(object);
   }
}                     

void CGLayout::clear_all(void) {
   CObject *child, *next;
   
   CGLayout(this).show_children(FALSE);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,x>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,y>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,colour_background>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,zoom>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,viewBox>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGCanvas,preserveAspectRatio>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGLayout,title>, TRUE);

   CSelection(&this->key_gselection.selection).clear();
   CSelection(&this->hover_gselection.selection).clear();
   
   child = CObject(this).child_first();
   while (child) {
      next = CObject(this).child_next(child);
      delete(child);
      child = next;
   }

   child = CObject(&this->defs).child_first();
   while (child) {
      next = CObject(&this->defs).child_next(child);
      delete(child);
      child = next;
   }
}                       

#define MEMFILE_PREFIX "memfile:"

bool CGLayout::load_svg_file(const char *filename, CString *error_result) {
   CFileGZ file;
   TException *exception;
   bool result = TRUE, close_result = 1, usefile = FALSE;
   CIOObject *io_object;
   CIOMemory io_memory;
   CMemFile *memfile = NULL;
   bool in_animate = CFsm(&CGLayout(this)->fsm).in_state((STATE)&CGLayout::state_animate);
   
   CGCanvas(this).pointer_cursor_set(EGPointerCursor.wait);

#if SVG_LOAD_ANIM_MAP
                                                                                   
   if (in_animate) {
      CFsm(&CGLayout(this)->fsm).transition((STATE)&CGLayout::state_freeze);
   }
#endif

   CGCanvas(this).queue_draw_disable(TRUE);

   try {
      if (strncmp(filename, MEMFILE_PREFIX, strlen(MEMFILE_PREFIX)) == 0) {
         memfile = CMemFileDirectory(framework.memfile_directory).memfile_find(filename + strlen(MEMFILE_PREFIX));
         if (!memfile) {
            throw(CObject(framework.memfile_directory), EXCEPTION<>, "File Not Found");
         }

         new(&io_memory).CIOMemory(memfile->memfile_def->data, memfile->memfile_def->size);
         io_object = CIOObject(&io_memory);
      }
      else {
         usefile = TRUE;
#if 1
         new(&file).CFileGZ(this->load_locked);
         if (CFileGZ(&file).open(filename, "rb") == -1) {
             throw(CObject(&file), EXCEPTION<>, "File Not Found");
         }
#else
         new(&file).CFile();
         if (CIOObject(&file).open(filename, O_RDONLY) == -1) {
             throw(CObject(&file), EXCEPTION<>, "File Not Found");
         }
#endif
         io_object = CIOObject(&file);
      }

      ARRAY(&this->time_animation).used_set(0);
      ARRAY(&this->data_animation).used_set(0);

      CGLayout(this).clear_all();
      CObjPersistent(this).state_xml_load(CIOObject(io_object), "svg", FALSE);
   }
   exception = catch(NULL, EXCEPTION<>) {
      result = FALSE;
      if (error_result) {
         CString(error_result).set(exception->message);
      }
   }
   finally {
                                                                    
      if (memfile)
        delete(&io_memory);
#if 1
      if (usefile)
        close_result = CFileGZ(&file).close() == 0;
#else
      if (usefile)
        close_result = CIOObject(&file).close() == 0;
#endif
      if (result && !close_result && error_result)
         CString(error_result).set("invalid file");
      if (result && !close_result)
         result = close_result;
      if (usefile)
        delete(&file);
   }

   if (!result) {
      CGLayout(this).clear_all();
   }

#if 1
                                                                                
   if (CGXULElement(this)->width == 100 && CGXULElement(this)->height == 100) {
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>, TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, TRUE);
      CObjPersistent(this).attribute_update_end();
      CGCanvas(this).NATIVE_size_allocate();
      WARN("CGLayout::load_svg_file(): unsupported width/height/viewBox set, alignment changed");
   }
#else
                                                                                
   if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,width>) &&
       !CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,height>) &&
       !CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGCanvas,viewBox>)) {

      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>, TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, TRUE);
      CObjPersistent(this).attribute_update_end();
      CGCanvas(this).NATIVE_size_allocate();
      WARN("CGLayout::load_svg_file(): unsupported width/height/viewBox set, alignment changed");
   }
#endif

   CGCanvas(this).pointer_cursor_set(EGPointerCursor.default);

#if SVG_LOAD_ANIM_MAP
                                                                                   
   if (in_animate) {
      CFsm(&CGLayout(this)->fsm).transition((STATE)&CGLayout::state_animate);
   }






#endif
   if (result) {
      CGLayout(this).notify_file_load(filename);
      CString(&this->filename).set(filename);
   }

   CGCanvas(this).queue_draw_disable(FALSE);
   CGCanvas(this).queue_draw(NULL);

   return result;
}                           

STATE CGLayout::state_freeze(CEvent *event) {
   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
                                           
         CSelection(&this->key_gselection.selection).clear();
         break;
      case EEventState.leave:
         break;
      default:
         break;
      }
   }
   return (STATE)&CFsm:top;
}                          

STATE CGLayout::state_animate(CEvent *event) {
   CGLayout *parent_layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));

   if (CObject(event).obj_class() == &class(CEventState)) {
      switch (CEventState(event)->type) {
      case EEventState.enter:
         this->relative_time = 0;

         CGLayout(this).animation_reset(CObjPersistent(this), TRUE);
         CGLayout(this).animation_resolve(CObjPersistent(this));
         CGLayout(this).animation_buildmap(CObjPersistent(this));
         CGSelection(&this->key_gselection).select_input();
         new(&this->iterate_timer).CTimer(this->frame_length, CObject(this), (THREAD_METHOD)&CGLayout::iterate, NULL);
         CGCanvas(this).queue_draw(NULL);
         return NULL;
      case EEventState.leave:
         delete(&this->iterate_timer);

         CGLayout(this).animation_reset(CObjPersistent(this), FALSE);
         return NULL;
      default:
         break;
      }
   }

   if (CObject(event).obj_class() == &class(CEventKey)) {
      if (parent_layout &&
           (CEventKey(event)->key == EEventKey.Enter ||
            CEventKey(event)->key == EEventKey.Escape ||
         CEventKey(event)->key == EEventKey.Function)) {
         CGObject(parent_layout).event(CEvent(event));
      }
   }

   if (CObject(event).obj_class() == &class(CEventPointer) &&
      CEventPointer(event)->type != EEventPointer.Hover &&
      !(CEventPointer(event)->position.x == this->hover_position.x && CEventPointer(event)->position.y == this->hover_position.y)) {
      this->hover_position.x = CEventPointer(event)->position.x;
      this->hover_position.y = CEventPointer(event)->position.y;
      this->hover_timeout = 400;
   }
   if (CObject(event).obj_class() == &class(CEventPointer) && CEventPointer(event)->type == EEventPointer.Leave) {
      this->hover_timeout = 0;
   }

                                
   CFsm(&CGSelection(&this->key_gselection)->fsm).event(event);
   CFsm(&CGSelection(&this->hover_gselection)->fsm).event(event);

   return (STATE)&CFsm:top;
}                           

void CGLayout::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CObject *parent;

   switch (linkage) {
   case EObjectLinkage.ChildAdd:
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
         CGCanvas(this).transform_set_gobject(CGObject(object), TRUE);
         CGObject(object).extent_set(CGCanvas(this));
         CGCanvas(this).queue_draw(&CGObject(object)->extent);
      }
      break;
   case EObjectLinkage.ParentSet:
      CGObject(this).show(TRUE);
      
                                                                 
      if (!CObject_exists((CObject *)&this->client)) {
         parent = object;
         while (parent) {
            if (CObjClass_is_derived(&class(CGLayout), CObject(parent).obj_class())) {
               if (CObject_exists((CObject *)&CGLayout(parent)->client)) {
                  new(&this->client).CObjClientGLayout(CObjClient(&CGLayout(parent)->client)->server,
                                                       CObject(this),
                                                       ARRAY(&CObjClient(&CGLayout(parent)->client)->object_root).data()[0],
                                                       this);
                  CGCanvas(this)->component_reposition = CGCanvas(parent)->component_reposition;
                  if (this->render != EGLayoutRender.buffered) {
                     this->render = CGLayout(parent)->render;
                  }
                  break;
               }
            }
            parent = CObject(parent).parent();
         }
      }
      break;
   case EObjectLinkage.ParentRelease:
      CGObject(this).show(FALSE);      
      break;
   default:
      CGCanvas(this).queue_draw(NULL);
      break;
   }

                    
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);
}                                   

void CGLayout::show_children(bool show) {
   CObjPersistent *object;

   if (!CObject_exists((CObject *)&this->defs) || !CGObject(this)->native_object) {
      return;
   }   
   
   if (this->disable_child_alloc) {
      show = FALSE;
   }
   
   object = CObjPersistent(this).child_first();

   while (object) {
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class()) &&
          !CObjClass_is_derived(&class(CGDefs), CObject(object).obj_class())) {
         CGObject(object).show(show);
         CGCanvas(this).transform_set_gobject(CGObject(object), FALSE);
         CGObject(object).extent_set(CGCanvas(this));
      }
      object = CObjPersistent(this).child_next(object);
   }
}                           

CGLayout *CGLayout::find_visible_child(void) {
   CObject *object = CObject(this).child_tree_first();
   CGLayout *result = this;

   while (object) {
      if (CObjClass_is_derived(&class(CGTabBox), CObject(object).obj_class())) {
         result = CGLayout(CObject(object).child_n(CGTabBox(object)->selectedIndex));
         if (result) {
            result = CGLayout(result).find_visible_child();
            return result;
         }
      }
      object = CObject(this).child_tree_next(object);
   }

   return result;
}                                

void CGLayout::show(bool show) {
   CObject *object = CObject(this).child_first();
   CEventPointer event;

   CGCanvas(this).NATIVE_show(show, FALSE);

   if (show) {
      _virtual_CGXULElement_show(CGXULElement(this), show);
   }

                                              
   CGLayout(this).show_children(show);

   if (!show) {
      if (CGObject(this)->native_object && CGXULElement(this)->width && CGXULElement(this)->height) {
         new(&event).CEventPointer(EEventPointer.Leave, 0, 0, 0);
         CGObject(this).event(CEvent(&event));
         delete(&event);
      }
      _virtual_CGXULElement_show(CGXULElement(this), show);
   }

                 
   object = CObject(this).parent();
   if (object && CObject(object).obj_class() == &class(CGVBox)) {
      _virtual_CGXULElement_show(CGXULElement(this), show);
   }

   CGCanvas(this).NATIVE_show(show, TRUE);
}                  

void CGLayout::redraw(TRect *extent, EGObjectDrawMode mode) {
   CGCanvas *dest_canvas;
   bool viewbox_default;

   _virtual_CGXULElement_draw((CGXULElement *)this, CGCanvas(this), extent, mode);

               




   switch (this->render) {
   case EGLayoutRender.normal:
      dest_canvas = CGCanvas(this);
      break;
   case EGLayoutRender.buffered:
      CGCanvasBitmap(&this->canvas_bitmap).resize(CGCanvas(this)->allocated_width, CGCanvas(this)->allocated_height);

                                                        
      viewbox_default = CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGCanvas,viewBox>);
      CObjPersistent(&this->canvas_bitmap).attribute_default_set(ATTRIBUTE<CGCanvas,viewBox>, viewbox_default);
      CGXULElement(&this->canvas_bitmap)->width = CGXULElement(this)->width;
      CGXULElement(&this->canvas_bitmap)->height = CGXULElement(this)->height;
      CGCanvas(&this->canvas_bitmap)->content_width = CGCanvas(this)->content_width;
      CGCanvas(&this->canvas_bitmap)->content_height = CGCanvas(this)->content_height;
      CGCanvas(&this->canvas_bitmap)->allocated_width = CGCanvas(this)->allocated_width;
      CGCanvas(&this->canvas_bitmap)->allocated_height = CGCanvas(this)->allocated_height;
      CGCanvas(&this->canvas_bitmap)->viewBox = CGCanvas(this)->viewBox;
      CGCanvas(&this->canvas_bitmap)->zoom = CGCanvas(this)->zoom;
      CGCanvas(&this->canvas_bitmap)->yAspect = CGCanvas(this)->yAspect;
      CGCanvas(&this->canvas_bitmap)->preserveAspectRatio = CGCanvas(this)->preserveAspectRatio;
      CGCanvas(&this->canvas_bitmap)->view_origin = CGCanvas(this)->view_origin;
      CGCanvas(&this->canvas_bitmap)->palette = CGCanvas(this)->palette;
      CGCanvas(&this->canvas_bitmap)->paletteB = CGCanvas(this)->paletteB;
      CGCanvas(&this->canvas_bitmap)->render_mode_canvas = CGCanvas(this)->render_mode_canvas;
      CGCanvas(&this->canvas_bitmap)->render_mode = CGCanvas(this)->render_mode;
      CGCanvas(&this->canvas_bitmap)->oem_font = CGCanvas(this)->oem_font;

      CGCanvasBitmap(&this->canvas_bitmap).NATIVE_allocate(NULL);
      CGCanvas(&this->canvas_bitmap).NATIVE_enter(TRUE);

      dest_canvas = CGCanvas(&this->canvas_bitmap);
      break;
   default:
      return;
   }

   CGCanvas(dest_canvas).opacity_set(1.0);
   CGCoordSpace(&CGCanvas(this)->coord_space).reset();
   CGCoordSpace(&CGCanvas(dest_canvas)->coord_space).reset();
   if (extent) {
      CGCanvas(dest_canvas).draw_rectangle(CGCanvas(this)->native_erase_background_colour,
                                           TRUE, extent->point[0].x, extent->point[0].y, extent->point[1].x, extent->point[1].y);
   }
   else {
      CGCanvas(dest_canvas).draw_rectangle(CGCanvas(this)->native_erase_background_colour,
                                           TRUE, 0, 0, CGCanvas(this)->allocated_width, CGCanvas(this)->allocated_height);
   }
   CGCoordSpace(&CGCanvas(this)->coord_space).reset();
   CGCoordSpace(&CGCanvas(dest_canvas)->coord_space).reset();
   CGCanvas(this).transform_viewbox();
   CGCanvas(dest_canvas).transform_viewbox();
   CGCanvas(this).draw(CGCanvas(dest_canvas), extent, mode);




   CGCoordSpace(&CGCanvas(dest_canvas)->coord_space).reset();

                                 
   if (CGCanvas(this)->preserveAspectRatio.aspect != EGPreserveAspectRatio.oneToOne) {
      if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,width>)) {
         CGCanvas(dest_canvas).draw_rectangle(GCOL_RGB(245,245,245), TRUE,
                                              CGXULElement(this)->width, 0, 2000, 2000);
      }
      if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,height>)) {
         CGCanvas(dest_canvas).draw_rectangle(GCOL_RGB(245,245,245), TRUE,
                                              0, CGXULElement(this)->height, 2000, 2000);
      }
   }

   switch (this->render) {
   case EGLayoutRender.buffered:
      CGCanvas(&this->canvas_bitmap).NATIVE_enter(FALSE);
      CGCanvasBitmap(&this->canvas_bitmap).NATIVE_release(NULL);
      CGCoordSpace(&CGCanvas(this)->coord_space).reset();
      CGCanvas(this).NATIVE_draw_bitmap(&this->canvas_bitmap.bitmap, 0, 0);
   default:
      break;
   }
}                    

void CGLayout::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   CObject *object = CObject(this).child_first();

   while (object) {
                                                                   
      if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class()) &&
          !CObjClass_is_derived(&class(CGCanvas), CObject(object).obj_class())) {
         CGCanvas(this).draw_gobject_transformed(canvas, CGObject(object), extent, mode);
      }
      if (CObjClass_is_derived(&class(CGCanvas), CObject(object).obj_class()) &&
          !CGObject(object)->native_object) {
         CGCanvas(this).draw_gobject_transformed(canvas, CGObject(object), extent, mode);
      }
      object = CObject(this).child_next(object);
   }

   CGSelection(&this->hover_gselection).draw(canvas, extent, mode);
   CGSelection(&this->key_gselection).draw(canvas, extent, mode);
}                  

bool CGLayout::event(CEvent *event) {
                                           
   CFsm(&CGLayout(this)->fsm).event(event);
   return TRUE;
}                   

void CGLayout::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObject *object;

   class:base.notify_attribute_update(attribute, changing);

                                    
   if (CGCanvas(this)->component_reposition == -1 && changing) {
      return;
   }




   object = CObject(this).child_first();
   while (object) {
      if (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class()) &&
         CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,x>)) {
         CGXULElement(object)->x      = 0;
         CGXULElement(object)->y      = 0;
         CGXULElement(object)->width  = CGXULElement(this)->width;
         CGXULElement(object)->height = CGXULElement(this)->height;
         CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,x>);
         CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,y>);
         CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,width>);
         CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,height>);
         CObjPersistent(object).attribute_update_end();
      }
      object = CObject(this).child_next(object);
   }
}                                     

void CGLayout::animation_resolve_inhibit(CObjPersistent *object) {
   CObjPersistent *child, *reset_anim = NULL;
   bool inhibit;

                                                                      
   inhibit = FALSE;
   child = CObjPersistent(CObject(object).child_last());
   while (child) {
      if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class())) {
         if (CGAnimation(child)->state == EGAnimState.Inhibited) {
            CGAnimation(child)->state = EGAnimState.Active;
         }

         if (inhibit && CGAnimation(child)->time_based && CGAnimation(child)->state == EGAnimState.Active) {
            CGAnimation(child)->state = EGAnimState.Inhibited;


                            
            if (reset_anim) {
               CGAnimation(reset_anim).reset(TRUE);
               CGAnimation(reset_anim)->state = EGAnimState.Active;
            }
         }

                                                          
         if (!CGAnimation(child)->time_based && CGAnimation(child)->state == EGAnimState.Active) {
            inhibit = TRUE;
            reset_anim = child;
         }
      }
      child = CObjPersistent(CObject(object).child_previous(CObject(child)));
   }
}                                       

void CGLayout::animation_resolve(CObjPersistent *object) {
   CObjPersistent *child;



   if (object == CObjPersistent(this)) {
      child = CObjPersistent(this).child_element_class_find(&class(CGPrimContainer), NULL);
      if (child) {
         CGLayout(this).animation_resolve(child);
      }
   }

   child = CObjPersistent(CObject(object).child_first());
   while (child) {

      if (!CObjClass_is_derived(&class(CGScrolledArea), CObject(child).obj_class())) {
         CGLayout(this).animation_resolve(child);
      }
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }

   if (CObjClass_is_derived(&class(CGAnimation), CObject(object).obj_class())) {
#if 0
                                      
      anim_state = CGAnimation(object)->state;
      CGAnimation(object).animation_resolve_value(this->relative_time);
      if (anim_state == EGAnimState.Inhibited) {
         CGAnimation(object)->state = anim_state;
      }
      if (anim_state != CGAnimation(object)->state) {
         CGLayout(this).animation_resolve_inhibit(CObjPersistent(CObject(object).parent()));
      }
      if (CGAnimation(object)->state == EGAnimState.Active ||
         anim_state == EGAnimState.Active) {
         CGAnimation(object).animation_resolve();
      }
#else
      CGAnimation(object).animation_resolve_value(this->relative_time);
      CGAnimation(object).animation_resolve();
#endif
   }
   else if (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
      CGXULElement(object).binding_resolve();
   }
}                               

void CGLayout::animation_reset(CObjPersistent *object, bool animated) {
   CObjPersistent *child;
                                                                                        

   if (object == CObjPersistent(this)) {
      child = CObjPersistent(this).child_element_class_find(&class(CGPrimContainer), NULL);
      if (child) {
         CGLayout(this).animation_reset(child, animated);
      }
   }

   child = CObjPersistent(CObject(object).child_first());

                                           
#if 0
   if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
      CGCanvas(this).transform_set_gobject(CGObject(object), FALSE);
      CGObject(object).extent_set(CGCanvas(this));
   }
#endif

   while (child) {
      if (!CObjClass_is_derived(&class(CGLayout), CObject(child).obj_class())) {
         CGLayout(this).animation_reset(child, animated);
      }
      if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class())                           ) {
         CGAnimation(child).reset(animated);
      }
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }

}                             

void CGLayout::animation_buildmap(CObjPersistent *object) {
   CObjPersistent *child;

   if (object == CObjPersistent(this)) {
      ARRAY(&this->time_animation).used_set(0);
      ARRAY(&this->data_animation).used_set(0);

      child = CObjPersistent(this).child_element_class_find(&class(CGPrimContainer), NULL);
      if (child) {
         CGLayout(this).animation_buildmap(child);
      }
   }

   child = CObjPersistent(CObject(object).child_first());
   while (child) {


      if (!CObjClass_is_derived(&class(CGScrolledArea), CObject(child).obj_class())) {
         CGLayout(this).animation_buildmap(child);
         if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class())                                                  ) {
            ARRAY(&this->data_animation).item_add(child);
         }
      }
      if (CObjClass_is_derived(&class(CGAnimation), CObject(child).obj_class())) {
         CXPath(&CGAnimation(child)->binding).resolve();

         CGAnimation(child).animation_resolve_value(0);                                
         if (CGAnimation(child)->time_based) {
            ARRAY(&this->time_animation).item_add(child);
                                                                                               
            ARRAY(&this->data_animation).item_add(child);
         }
         else {
            ARRAY(&this->data_animation).item_add(child);
         }
      }
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }

}                                

void CGLayout::animation_add(CObjPersistent *object) {
   if (!CFsm(&this->fsm).in_state((STATE)&CGLayout::state_animate)) {
       return;
   }

   if (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
      ARRAY(&this->data_animation).item_add(object);
   }
   if (CObjClass_is_derived(&class(CGAnimation), CObject(object).obj_class())) {
      CGAnimation(object).animation_resolve_value(0);                                
      if (CGAnimation(object)->time_based) {
         ARRAY(&this->time_animation).item_add(object);
      }
      else {
         ARRAY(&this->data_animation).item_add(object);
      }
   }
}                           

void CGLayout::animation_remove(CObjPersistent *object) {
   int i;
   CObjPersistent *anim;

   if (!CFsm(&this->fsm).in_state((STATE)&CGLayout::state_animate)) {
       return;
   }

   for (i = 0; i < ARRAY(&this->data_animation).count(); i++) {
      anim = ARRAY(&this->data_animation).data()[i];
      if (anim == object) {
         ARRAY(&this->data_animation).item_remove(object);
         break;
      }
   }
   for (i = 0; i < ARRAY(&this->time_animation).count(); i++) {
      anim = ARRAY(&this->time_animation).data()[i];
      if (anim == object) {
         ARRAY(&this->time_animation).item_remove(object);
         break;
      }
   }
}                              

void CGLayout::animate(void) {
   CGLayout(this).animation_resolve(CObjPersistent(this));
}                     

void CGLayoutTab::new(void) {
   class:base.new();



   new(&this->label).CString(NULL);
}                    

void CGLayoutTab::CGLayoutTab(const char *name, int width, int height, CObjServer *obj_server, CObjPersistent *data_source) {
   CGLayout(this).CGLayout(width, height, obj_server, data_source);

   CString(&this->label).set(name);
}                            

void CGLayoutTab::delete(void) {
   delete(&this->label);

   class:base.delete();
}                       

                                                                              
MODULE::END                                                                   
                                                                              
# 19 "/home/jacob/keystone/src/graphics/gwindow.c"





                                                                              
MODULE::INTERFACE                                                             
                                                                              

class CGCanvas;

class CGMenu : CGObject {
 private:

   CGMenuItem *selected;

   void notify_all_update(bool changing);

   void NATIVE_allocate(void);
   void NATIVE_release(void);
   void NATIVE_item_allocate(CGMenuItem *parent);
   void NATIVE_item_update(CGMenuItem *item);

   CGMenuItem *child_find(CObject *menu, const char *label);
   void menu_item_select(CGMenuItem *item);
 public:
   void CGMenu(void);

   void menu_item_add(CObjPersistent *object, const TAttribute *attribute);
   CGMenuItem *menu_item_find(const char *path);
};

class CGMenuPopup : CGMenu {

 private:
   bool selection_done;

 public:
   void CGMenuPopup(void);
   CGMenuItem *execute(CGCanvas *canvas);
};

                                                              
class CGWindow : CGContainer {
 private:
   CGWindow *parent;
   CGObject *input_focus;
   bool nodecoration;
   bool disable_close;
   bool topmost;
   void *native_data[2];

   void new(void);
   void delete(void);
   void NATIVE_new(void);
   void NATIVE_delete(void);
   void NATIVE_show(bool show);
   void NATIVE_title_set(const char *title);
   void NATIVE_extent_set(TRect *extent);
   void NATIVE_maximize(bool maximize);
   void NATIVE_fullscreen(bool fullscreen);
   void NATIVE_opacity(double opacity);
   void NATIVE_show_help(CString *help_topic);
   void NATIVE_monitor_area(TRect *result);

   bool modal;

   void *vbox;

   bool event(CEvent *event);
 public:
   ATTRIBUTE<CString title> {
      CString(&this->title).set(CString(data).string());
      CGWindow(this).NATIVE_title_set(CString(&this->title).string());
   };
   ELEMENT:OBJECT<CGMenu menu>;
   ATTRIBUTE<TRect placement>;
   ATTRIBUTE<bool maximized> {
      this->maximized = *data;
      CGWindow(this).NATIVE_maximize(this->maximized);
   };
   ATTRIBUTE<bool fullscreen> {
      this->fullscreen = *data;
      CGWindow(this).NATIVE_fullscreen(this->fullscreen);
   };
   void CGWindow(const char *title, CGCanvas *canvas, CGWindow *parent);

   void topmost(bool topmost, bool stick);
   void disable_close(bool disable);
   void disable_maximize(bool disable);
   void disable_minimize(bool disable);
   void modal(bool modal);

   virtual bool load(CGLayout *layout, const char *file_name, bool warn_dialog);

   void size_set(int width, int height);
   void position_set(int x, int y);
   void position_size_set(int x, int y, int width, int height);

   void object_menu_add(CObjPersistent *object, bool update);
   void object_menu_remove(CObjPersistent *object);

   virtual bool notify_request_close(void);
   virtual void show(bool show);
};

class CGLayoutDialog : CGLayout {
 private:
                            
      STATE state_animate_dialog(CEvent *event);
 public:
   ALIAS<"svg:svg">;
   void CGLayoutDialog(void);
};

class CGWindowDialog : CGWindow {
 private:
   CGLayoutDialog layout;
   CGLayout layout_buttons;
   CGVBox vbox;
   CGVBox hbox;
   bool wait;

   void new(void);
   void delete(void);
   CGButton button[5];

   virtual bool notify_request_close(void);
 public:
   void CGWindowDialog(const char *title, CGCanvas *canvas, CGWindow *parent);
   void button_add(const char *label, int id);
   int execute(void);
   int execute_wait(void);
   int wait(bool wait);
   int close(void);

   ATTRIBUTE<int button_press> {
      this->button_press = *data;
      if (this->wait) {
         CFramework(&framework).NATIVE_modal_exit(CObject(this));
      }
   };
};

class CGDialogMessage : CGWindowDialog {
 private:
   CGLayout layout;
   CGTextLabel text;
   void delete(void);
 public:
   void CGDialogMessage(const char *title, const char *message, CGWindow *parent);
};

                    

class CGChooseColour : CObjPersistent {
 private:
   bool in_update;
   TObjectPtr object;

   void new(void);
   void delete(void);

   static inline void colour_change(bool is_default);
   static inline void colour_set(TGColour colour);
 public:
   ATTRIBUTE<ARRAY<TGColour> palette>;
   ATTRIBUTE<ARRAY<TGColour> paletteB>;
   ATTRIBUTE<TGColour colour> {
      if (data) {
         this->colour = *data;
         CGChooseColour(this).colour_change(FALSE);
      }
      else {
         CGChooseColour(this).colour_change(TRUE);
      }
   };
   ATTRIBUTE<EGColourNamed colour_name> {
      if (data) {
         this->colour_name = *data;
      }

      CGChooseColour(this).colour_set(GCOL_NAMED_VALUE(this->colour_name));
   };
   ATTRIBUTE<bool colour_default> {
      CGChooseColour(this).colour_change(TRUE);
   };

   void CGChooseColour(TObjectPtr *object);
};

static inline void CGChooseColour::colour_change(bool is_default) {
   if (this->in_update) {
      return;
   }
   this->in_update = TRUE;

   if (is_default) {
      CObjPersistent(this->object.object).attribute_default_set(this->object.attr.attribute, TRUE);
   }
   else {
      CObjPersistent(this->object.object).attribute_default_set(this->object.attr.attribute, FALSE);
      CObjPersistent(this->object.object).attribute_set(this->object.attr.attribute,
                                                        this->object.attr.element,
                                                        this->object.attr.index,
                                                        &ATTRIBUTE:type<TGColour>, (void *)&this->colour);
   }
                                 
   CObjPersistent(this->object.object).attribute_update(this->object.attr.attribute);
   CObjPersistent(this->object.object).attribute_update_end();

                                 
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGChooseColour,colour>);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGChooseColour,colour>, is_default);
   CObjPersistent(this->object.object).attribute_get(this->object.attr.attribute,
                                                      this->object.attr.element,
                                                      this->object.attr.index,
                                                      &ATTRIBUTE:type<TGColour>, (void *)&this->colour);
   CObjPersistent(this).attribute_update_end();
   this->in_update = FALSE;
};

static inline void CGChooseColour::colour_set(TGColour colour) {
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGChooseColour,colour>, FALSE);
   CObjPersistent(this).attribute_set(ATTRIBUTE<CGChooseColour,colour>, -1, ATTR_INDEX_VALUE,
                                      &ATTRIBUTE:type<TGColour>, &colour);



};

class CGLayoutChooseColour : CGLayoutTab {
 private:
   CObjServer obj_server;
   CGChooseColour colour;

   void delete(void);
 public:
   void CGLayoutChooseColour(const char *label, TObjectPtr *object, CGPalette *palette, CGPalette *paletteB);
   static inline TGColour colour_get(void);
};

static inline TGColour CGLayoutChooseColour::colour_get(void) {
   return this->colour.colour;
}                                    

class CGDialogChooseColour : CGWindowDialog {
 private:
   bool native_dialog;
   CGLayoutChooseColour layout;
   TObjectPtr *object;
   CGPalette *palette;
   CGPalette *paletteB;

   int execute(void);
 public:
   void CGDialogChooseColour(const char *title, TObjectPtr *object, CGPalette *palette, CGPalette *paletteB, CGWindow *parent,
                             bool native_dialog);
   static inline TGColour colour_get(void);
};

static inline TGColour CGDialogChooseColour::colour_get(void) {
   return CGLayoutChooseColour(&this->layout).colour_get();
}                                    

class CGDialogChooseDate : CGWindowDialog {
 private:
   void delete(void);
   bool immediate;
   
   CObjServer obj_server; 
   TObjectPtr object;
   CGLayout layout;

   int execute(void);
   static inline void set_date(TUNIXTime *date);
   static inline void set_midnight();
 public:
   void CGDialogChooseDate(const char *title, bool immedate, TObjectPtr *object, CGWindow *parent);
   
   ATTRIBUTE<bool set_midnight> {
      CGDialogChooseDate(this).set_midnight();
   };
   ATTRIBUTE<TUNIXTime date> {
      if (data) {
         this->date = *data;
      }
      CGDialogChooseDate(this).set_date(&this->date);
   };
};

static inline void CGDialogChooseDate::set_date(TUNIXTime *date) {
   if (this->immediate) {
      CObjPersistent(this->object.object).attribute_set(this->object.attr.attribute,
                                                        this->object.attr.element,
                                                        this->object.attr.index,
                                                        &ATTRIBUTE:type<TUNIXTime>, (void *)date);
      CObjPersistent(this->object.object).attribute_update(this->object.attr.attribute);
      CObjPersistent(this->object.object).attribute_update_end();
   }
}                                

static inline void CGDialogChooseDate::set_midnight(void) {
   CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGDialogChooseDate,date>, -1, 3, 0);
   CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGDialogChooseDate,date>, -1, 4, 0);   
   CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGDialogChooseDate,date>, -1, 5, 0);   
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGDialogChooseDate,date>);
   CObjPersistent(this).attribute_update_end();
}                                    

ENUM:typedef<EDialogFileSelectMode> {
   {open}, {save}
};

class CGDialogFileSelect : CGWindow {
 private:
   EDialogFileSelectMode mode;
   const char *title;
   CGWindow *parent;
   int exec_result;

   void new(void);
   void delete(void);
 public:
   void CGDialogFileSelect(const char *title, EDialogFileSelectMode mode, CGWindow *parent);
   bool execute(CString *filename);

   ELEMENT:OBJECT<CGListMenuPopup filter>;
};

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

                                   
void CGLayout::key_gselection_set(CGObject *object) {
   CGWindow *window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
   CObjPersistent *cur_object = NULL;
   TRect extent;

   if (!window) {
      return;
   }

   if (CSelection(&this->key_gselection.selection).count()) {
      cur_object = ARRAY(CSelection(&this->key_gselection.selection).selection()).data()[0].object;
   }

   if (window->input_focus) {
      CSelection(&CGLayout(window->input_focus)->key_gselection.selection).clear();
   }

   CSelection(&this->key_gselection.selection).clear();

                                                                                         
   if (cur_object && !CObjClass_is_derived(&class(CGXULElement), CObject(cur_object).obj_class())) {
      extent = CGObject(cur_object)->extent;
      CGCoordSpace(&CGCanvas(this)->coord_space).reset();
      CGCanvas(this).transform_viewbox();
      CGCanvas(this).point_array_utov(2, extent.point);
      CGCoordSpace(&CGCanvas(this)->coord_space).extent_extend(&extent, 3);
      CGCanvas(this).queue_draw(&extent);
   }

   window->input_focus = NULL;
   if (object) {
      window->input_focus = CGObject(this);

      CSelection(&this->key_gselection.selection).add(CObjPersistent(object), NULL);

      if (!CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
         extent = CGObject(object)->extent;
         CGCoordSpace(&CGCanvas(this)->coord_space).reset();
         CGCanvas(this).transform_viewbox();
         CGCanvas(this).point_array_utov(2, extent.point);
         CGCoordSpace(&CGCanvas(this)->coord_space).extent_extend(&extent, 3);
         CGCanvas(this).queue_draw(&extent);
      }
   }
}                                

void CGLayout::key_gselection_refresh(void) {
   TObjectPtr *object_selection;

   if (CSelection(&this->key_gselection.selection).count() != 1) {
      return;
   }
   object_selection = &ARRAY(CSelection(&this->key_gselection.selection).selection()).data()[0];

   if (CObjClass_is_derived(&class(CGXULElement), CObject(object_selection->object).obj_class())) {
      CGXULElement(object_selection->object).NATIVE_focus_in();
   }
}                                    

void CXLink::link_choose(void) {
   CString filename;
   CGDialogFileSelect file_sel;
   bool result;

   new(&filename).CString(NULL);
   CXLink(this).link_get_file(&filename);

   new(&file_sel).CGDialogFileSelect("Choose File...", EDialogFileSelectMode.open, NULL);
   result = CGDialogFileSelect(&file_sel).execute(&filename);
   delete(&file_sel);
   if (result) {
      CXLink(this).link_set(CString(&filename).string());
   }

   delete(&filename);
}                       

void CGXULElement::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
                                                       

                                                     
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));

   switch (linkage) {
   case EObjectLinkage.ParentRelease:
      if (layout) {
         CGLayout(layout).animation_remove(CObjPersistent(this));
         CGLayout(layout).key_gselection_set(NULL);
      }
      break;
   case EObjectLinkage.ParentSet:
      if (layout) {
         CGLayout(layout).animation_add(CObjPersistent(this));
      }
      break;
   default:
      break;
   }
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);
}                                       

void CGXULElement::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   TGColour stroke, fill;
   CString classname;

   class:base.draw(canvas, extent, mode);

   CGXULElement(this).NATIVE_draw(canvas, extent, mode);

   if (CGObject(this)->native_object)
      return;

   switch (mode) {
   case EGObjectDrawMode.Draw:
      fill   = GCOL_BLACK;
      stroke = (GCOL_TYPE_NAMED | EGColourNamed.palevioletred);
      CGCanvas(canvas).stroke_style_set(0, NULL, 0, 0, 0, 0);
      break;
   case EGObjectDrawMode.Select:
      fill   = GCOL_NONE;
      stroke = GCOL_WHITE;
      break;
   default:
      return;
   }
   CGCanvas(canvas).draw_rectangle(stroke, FALSE, this->x, this->y,
                                   this->x + this->width - 1, this->y + this->height - 1);
   CGCanvas(canvas).draw_line(stroke, this->x, this->y,
                              this->x + this->width - 1, this->y + this->height - 1);
   CGCanvas(canvas).draw_line(stroke, this->x, this->y + this->height - 1,
                              this->x + this->width - 1, this->y);

   new(&classname).CString(NULL);
   CString(&classname).printf("Arial");
   CGCanvas(canvas).font_set((CGFontFamily *)&classname, 14,
                             EGFontStyle.normal, EGFontWeight.bold, EGTextDecoration.none);
   CString(&classname).printf("<%s/>", CObjClass_alias_short(CObject(this).obj_class()));
   CGCanvas(canvas).NATIVE_text_align_set(EGTextAnchor.middle, EGTextBaseline.middle);
   CGCanvas(canvas).draw_text(GCOL_RGB(255,255,255), this->x + (this->width / 2) + 1, this->y + (this->height / 2) + 1,
                              CString(&classname).string());
   CGCanvas(canvas).draw_text(fill, this->x + (this->width / 2), this->y + (this->height / 2),
                              CString(&classname).string());
   delete(&classname);
}                      

void CGListItem::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);

   switch (linkage) {
   case EObjectLinkage.ParentSet:
      CObjPersistent(this).attribute_refresh(FALSE);
      break;
   default:
      break;
   }
}                                     

void CGListItem::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGMenu *menu;

                    
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);

   if (!changing) {
      menu = CGMenu(CObject(this).parent_find(&class(CGMenu)));
      if (menu) {
         CGMenu(menu).NATIVE_item_update(CGMenuItem(this));
      }
   }
}                           

bool CGLink::event(CEvent *event) {
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   CString file_name;


   CGLayout *link_layout;
   CGWindow *link_window;

   if (CObject(event).obj_class() == &class(CEventPointer)) {
      switch (CEventPointer(event)->type) {
      case EEventPointer.Move:
         CGCanvas(layout).pointer_cursor_set(EGPointerCursor.pointer);
         break;
      case EEventPointer.LeftDown:
         new(&file_name).CString(NULL);
         CXLink(&CGUse(this)->link).link_get(&file_name);

         if (this->show == EXLinkShow.new) {
            link_layout = new.CGLayout(0, 0, CObjClient(&layout->client)->server,
            ARRAY(&CObjClient(&layout->client)->object_root).data()[0]);

            CGLayout(link_layout).render_set(EGLayoutRender.buffered);

            CFsm(&CGLayout(link_layout)->fsm).transition((STATE)&CGLayout::state_freeze);

            link_window = new.CGWindow("Link Window", CGCanvas(link_layout), NULL);
            CGWindow(link_window).load(link_layout, CString(&file_name).string(), TRUE);

            CGObject(link_window).show(TRUE);
            CGObject(link_layout).show(TRUE);
            CFsm(&CGLayout(link_layout)->fsm).transition((STATE)&CGLayout::state_animate);
         }
         else {
            link_window = CGWindow(CObject(layout).parent_find(&class(CGWindow)));
            CGWindow(link_window).load(layout, CString(&file_name).string(), TRUE);
         }
         delete(&file_name);

         break;
      default:
         break;
      }
      return TRUE;
   }

   return FALSE;
}                 

void CGMenu::CGMenu(void) {
}                  

void CGMenu::notify_all_update(bool changing) {
   if (!changing) {
                             
      CGMenu(this).NATIVE_item_allocate(NULL);
   }
}                             

CGMenuItem *CGMenu::child_find(CObject *menu, const char *label) {
   CGMenuItem *child;

   child = CGMenuItem(CObject(menu).child_first());
   while (child) {
      if (CString(&CGListItem(child)->label).strcmp(label) == 0) {
         return child;
      }
      child = CGMenuItem(CObject(menu).child_next(CObject(child)));
   }
                          
   return NULL;
}                      

void CGMenu::menu_item_select(CGMenuItem *item) {
   if (item->menu_object && item->menu_attribute && item->menu_attribute->method) {
      (*item->menu_attribute->method)(item->menu_object, FALSE, -1);
   }
}                            

CGMenuItem *CGMenu::menu_item_find(const char *path) {
   CString temp, section;
   CObject *menu_current = CObject(this);
   CGMenuItem *item_current = NULL;

   new(&temp).CString(path);
   new(&section).CString(NULL);

   CString(&temp).tokenise(&section, "/", TRUE);
   while (CString(&temp).tokenise(&section, "/", TRUE)) {
      item_current = CGMenu(this).child_find(menu_current, CString(&section).string());
      menu_current = CObject(item_current);
   }

   delete(&section);
   delete(&temp);

   return item_current;
}                          

void CGMenu::menu_item_add(CObjPersistent *object, const TAttribute *attribute) {
   CString temp, section;
   CObject *menu_current = CObject(this);
   CGMenuItem *item_current = NULL;

   new(&temp).CString(attribute->path);
   new(&section).CString(NULL);

   CString(&temp).tokenise(&section, "/", TRUE);
   while (CString(&temp).tokenise(&section, "/", TRUE)) {
      item_current = CGMenu(this).child_find(menu_current, CString(&section).string());
      if (!item_current) {
         if (CString(&section).string()[0] == '-') {
            item_current = (CGMenuItem *)new.CGMenuSeparator();
         }
         else {
            item_current = new.CGMenuItem(CString(&section).string(), NULL, 0, FALSE, FALSE);
         }
         CGMenuItem(item_current).attribute_set(object, attribute);
         CObject(menu_current).child_add(CObject(item_current));
      }
      menu_current = CObject(item_current);
   }
   if (item_current && attribute->delim) {
      CObjPersistent(item_current).attribute_default_set(ATTRIBUTE<CGMenuItem,accelerator>, FALSE);
      CObjPersistent(item_current).attribute_set_text(ATTRIBUTE<CGMenuItem,accelerator>, attribute->delim);

                                                           
      CObjPersistent(item_current).attribute_default_set(ATTRIBUTE<CGMenuItem,acceltext>, FALSE);
      CObjPersistent(item_current).attribute_set_text(ATTRIBUTE<CGMenuItem,acceltext>, attribute->delim);
   }

   delete(&section);
   delete(&temp);
}                         

void CGMenuPopup::CGMenuPopup(void) {
   CGMenu(this).CGMenu();
}                            

void CGWindow::new(void) {
                    
   _virtual_CGXULElement_new(CGXULElement(this));

   new(&this->title).CString(NULL);
   new(&this->menu).CGMenu();
   CGObject(&this->menu)->visibility = EGVisibility.visible;
   CObject(&this->menu).parent_set(CObject(this));

   CGObject(this)->visibility = EGVisibility.visible;

   ARRAY(&framework.window).item_add(this);
}                 

void CGWindow::CGWindow(const char *title, CGCanvas *canvas, CGWindow *parent) {
   CGXULElement(this).CGXULElement(0, 0, 0, 0);
   CString(&this->title).set(title);

   this->input_focus = CGObject(canvas);
   this->parent = parent;

                                          
   if (!title) {
      this->nodecoration = TRUE;
   }

   CObject(this).child_add(CObject(canvas));
   CGWindow(this).NATIVE_new();
   CGWindow(this).NATIVE_title_set(title);
   if (parent) {
                                                             
      CGWindow(this).disable_maximize(TRUE);
      CGWindow(this).disable_minimize(TRUE);      
   }

                
   CObject(this).child_remove(CObject(canvas));
   CObject(this).child_add(CObject(canvas));
}                      

void CGWindow::delete(void) {

   delete(&this->menu);
   delete(&this->title);


   CGWindow(this).NATIVE_delete();

   ARRAY(&framework.window).item_remove(this);
}                    

bool CGWindow::notify_request_close(void) {
   return TRUE;
}                                  

bool CGWindow::load(CGLayout *layout, const char *file_name, bool warn_dialog) {
                                      
   return TRUE;
}                  

void CGWindow::show(bool show) {
   CGWindow(this).NATIVE_show(show);
}                  

bool CGWindow::event(CEvent *event) {
   CObject *child;
   const TAttribute *attribute;
   CString helpFile;
   CGLayout *layout;

   if (CObject(event).obj_class() == &class(CEventKey)) {
      if (CEventKey(event)->type == EEventKeyType.down) {
         child = CObject(&this->menu).child_tree_first();
         while (child) {
            if (CObject(child).obj_class() == &class(CGMenuItem))  {
                if ((CGMenuItem(child)->accelerator.key != EEventKey.None) && !CGListItem(child)->disabled) {
                   if (CEventKey(event)->key == CGMenuItem(child)->accelerator.key &&
                       toupper(CEventKey(event)->value) == toupper(CGMenuItem(child)->accelerator.value) &&
                       CEventKey(event)->modifier == CGMenuItem(child)->accelerator.modifier) {
                       CGMenu(&this->menu).menu_item_select(CGMenuItem(child));
                       return FALSE;
                   }
                }
            }
            child = CObject(&this->menu).child_tree_next(child);
         }
#if 1
                                      
         if (CEventKey(event)->key == EEventKey.Function && CEventKey(event)->value == 1) {
            layout = CGLayout(CObject(this).child_first());
            layout = CGLayout(layout).find_visible_child();

            if (CString(&CGObject(layout)->id).length()) {

               CGWindow(this).NATIVE_show_help(&CGObject(layout)->id);
            }
            else {
               attribute = CObjPersistent(framework.application).attribute_find("defaultHelp");
               new(&helpFile).CString(NULL);
               CObjPersistent(framework.application).attribute_get_string(attribute, &helpFile);
               CGWindow(this).NATIVE_show_help(&helpFile);
               delete(&helpFile);
            }
            return FALSE;
         }
#endif

      }

      if (!this->input_focus) {
          WARN("CGWindow::event - no focus layout");
      }
      else {

         CGObject(this->input_focus).event(CEvent(event));
      }
   }
   return FALSE;
}                   

void CGWindow::object_menu_add(CObjPersistent *object, bool update) {
   const TAttribute *attribute;
   ARRAY<const TAttribute *> attr_list;
   int i;

   ARRAY(&attr_list).new();
   CObjPersistent(object).attribute_list(&attr_list, FALSE, FALSE, TRUE);

   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      attribute = ARRAY(&attr_list).data()[i];
      CGMenu(&this->menu).menu_item_add(object, attribute);
   }

   ARRAY(&attr_list).delete();

                
   if (update) {
      CObjPersistent(&this->menu).notify_all_update(FALSE);
   }
}                             

void CGLayoutDialog::CGLayoutDialog(void) {
    CGLayout(this).CGLayout(0, 0, NULL, NULL);
}                                  

STATE CGLayoutDialog::state_animate_dialog(CEvent *event) {
    CGWindowDialog *window = CGWindowDialog(CObject(this).parent());






   if (CObject(event).obj_class() == &class(CEventKey) && CEventKey(event)->type == EEventKeyType.down) {
      switch (CEventKey(event)->key) {
      case EEventKey.Enter:
         if (!CGWindow(window)->disable_close) {
            CObjPersistent(window).attribute_set_int(ATTRIBUTE<CGWindowDialog,button_press>, 1);
         }
         break;
       case EEventKey.Escape:
         if (!CGWindow(window)->disable_close) {
            CObjPersistent(window).attribute_set_int(ATTRIBUTE<CGWindowDialog,button_press>, 0);
         }
         break;
       default:
         break;
      }
   }
   return (STATE)&CGLayout::state_animate;
}                                        

void CGWindowDialog::new(void) {
   class:base.new();
}                       

void CGWindowDialog::delete(void) {
   class:base.delete();
}                          

void CGWindowDialog::CGWindowDialog(const char *title, CGCanvas *canvas, CGWindow *parent) {
   int width, height;



   new(&this->layout).CGLayoutDialog();
   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayoutDialog::state_animate_dialog);

   width  = CObjPersistent(canvas).attribute_get_int(ATTRIBUTE<CGXULElement,min_width>);
   height = CObjPersistent(canvas).attribute_get_int(ATTRIBUTE<CGXULElement,min_height>) + 22;
   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_width>, width);


   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_height>, height);



   CGLayout(&this->layout).render_set(EGLayoutRender.none);
   CGCanvas(&this->layout).colour_background_set(GCOL_DIALOG);
   new(&this->vbox).CGVBox();
   new(&this->layout_buttons).CGLayout(0, 30, NULL, NULL);
   CGCanvas(&this->layout_buttons).colour_background_set(GCOL_DIALOG);
   CGLayout(&this->layout_buttons).render_set(EGLayoutRender.none);
   CObjPersistent(&this->layout_buttons).attribute_set_int(ATTRIBUTE<CGXULElement,height>, 22);
   CObjPersistent(&this->layout_buttons).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);

   new(&this->hbox).CGHBox();
   CObject(&this->layout_buttons).child_add(CObject(&this->hbox));

   CObject(&this->vbox).child_add(CObject(canvas));
   CObject(&this->vbox).child_add(CObject(&this->layout_buttons));
   CObject(&this->layout).child_add(CObject(&this->vbox));

   CGWindow(this).CGWindow(title, CGCanvas(&this->layout), parent);
}                                  

bool CGWindowDialog::notify_request_close(void) {
                                 
   CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGWindowDialog,button_press>, 0);
   return FALSE;
}                                        

void CGWindowDialog::button_add(const char *label, int id) {
   int index = CObject(&this->hbox).child_count();
   CString temp;

   new(&temp).CString(NULL);
   CString(&temp).printf("%d", id);

   new(&this->button[index]).CGButton(0, 0, 0, 0, label);
   CObject(&this->hbox).child_add(CObject(&this->button[index]));
   CXPath(&CGXULElement(&this->button[index])->binding).set(CObjPersistent(this), ATTRIBUTE<CGWindowDialog,button_press>, -1);

                                         
   CObjPersistent(CGXULElement(&this->button[index])).attribute_set_string(ATTRIBUTE<CGObject,id>, &temp);

   CXPath(&CGXULElement(&this->button[index])->binding)->assignment =
   CString(&CGObject(&this->button[index])->id).string();

   delete(&temp);
}                              

int CGWindowDialog::execute(void) {
   int result;

   CGWindowDialog(this).execute_wait();
   result = CGWindowDialog(this).wait(TRUE);
   CGWindowDialog(this).close();

   return result;
}                           

int CGWindowDialog::execute_wait(void) {
   CObject *focus_element;
   CGLayout *layout = CGLayout(&this->layout).find_visible_child();

   focus_element = CObject(layout).child_tree_first();
   while (focus_element) {
      if (!CObjClass_is_derived(&class(CGLayout), CObject(focus_element).obj_class()) &&
         (CObjClass_is_derived(&class(CGObject), CObject(focus_element).obj_class()) && CGObject(focus_element).keyboard_input())) {
         break;
      }
      focus_element = CObject(layout).child_tree_next(focus_element);
   }

   this->button_press = -1;
   CGObject(&this->layout).show(TRUE);
   if (focus_element) {
      CGXULElement(focus_element).NATIVE_focus_in();
   }



   CGLayout(&this->layout).animate();
   if (CGObject(this)->visibility) {
      CGObject(this).show(TRUE);
   }

   CGWindow(this).modal(TRUE);

   return 0;
}                                

int CGWindowDialog::wait(bool wait) {
   this->wait = wait;
   if (wait) {
      CGLayout(&this->layout).animate();
      CFramework(&framework).NATIVE_modal_wait(CObject(this));
   }
   else {
      CFramework(&framework).NATIVE_modal_exit(CObject(this));
   }

   return this->button_press;
}                        

int CGWindowDialog::close(void) {
   CGWindow(this).modal(FALSE);
   CGWindow(this).show(FALSE);
   CGLayout(&this->layout).show_children(FALSE);
   CFsm(&CGLayout(&this->layout)->fsm).transition((STATE)&CGLayout::state_freeze);

   return 0;
}                         

void CGDialogMessage::delete(void) {
   class:base.delete();
}                           

void CGDialogMessage::CGDialogMessage(const char *title, const char *message, CGWindow *parent) {
   new(&this->layout).CGLayout(0, 0, NULL, NULL);
   CGCanvas(&this->layout).colour_background_set(GCOL_DIALOG);

   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>,  FALSE);
   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_width>,  320);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_height>, 100);
   new(&this->text).CGTextLabel(10, 40, 300, 22, message);
   CObject(&this->layout).child_add(CObject(&this->text));

   CGWindowDialog(this).CGWindowDialog(title, CGCanvas(&this->layout), parent);
   CGWindowDialog(this).button_add("OK", 1);
}                                    

void CGChooseColour::new(void) {
   ARRAY(&this->palette).new();
   ARRAY(&this->paletteB).new();
}                       

void CGChooseColour::CGChooseColour(TObjectPtr *object) {
   bool is_default;
   this->object = *object;

   is_default = CObjPersistent(this->object.object).attribute_default_get(this->object.attr.attribute);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGChooseColour,colour>, is_default);
   CObjPersistent(this->object.object).attribute_get(this->object.attr.attribute,
                                                      this->object.attr.element,
                                                      this->object.attr.index,
                                                      &ATTRIBUTE:type<TGColour>, (void *)&this->colour);
}                                  

void CGChooseColour::delete(void) {
   ARRAY(&this->paletteB).delete();
   ARRAY(&this->palette).delete();
}                          

void CGLayoutChooseColour::CGLayoutChooseColour(const char *label, TObjectPtr *object, CGPalette *palette, CGPalette *paletteB) {
   new(&this->colour).CGChooseColour(object);
   new(&this->obj_server).CObjServer(CObjPersistent(&this->colour));

   CGLayoutTab(this).CGLayoutTab(label, 0, 0, CObjServer(&this->obj_server), CObjPersistent(&this->colour));

   if (palette) {
      CGCanvas(this).palette_set(palette);
      ARRAY(&this->colour.palette).copy(&palette->colour);
   }
   if (paletteB) {
      CGCanvas(this).palette_B_set(paletteB);
      ARRAY(&this->colour.paletteB).copy(&paletteB->colour);
   }

   CGLayout(this).load_svg_file("memfile:colour.svg", NULL);


   CGCanvas(this).colour_background_set(               GCOL_NONE);
}                                              

void CGLayoutChooseColour::delete(void) {
   class:base.delete();

   delete(&this->obj_server);
   delete(&this->colour);
}                                

void CGDialogChooseColour::CGDialogChooseColour(const char *title, TObjectPtr *object,
                                                CGPalette *palette, CGPalette *paletteB,
                                                CGWindow *parent, bool native_dialog) {
   this->native_dialog = native_dialog;
   if (this->native_dialog) {
      CGWindow(this)->parent = parent;
      this->object = object;
      this->palette = palette;
      this->paletteB = paletteB;
   }
   else {
      new(&this->layout).CGLayoutChooseColour(NULL, object, palette, paletteB);
      CGCanvas(&this->layout).colour_background_set(GCOL_DIALOG);
      CGWindowDialog(this).CGWindowDialog(title, CGCanvas(&this->layout), parent);
   }
}                                              

void CGDialogChooseDate::CGDialogChooseDate(const char *title, bool immediate, TObjectPtr *object, CGWindow *parent) {
   CGVBox *vbox;
   CGDatePicker *date_picker;
   CGHBox *hbox;
   CGTextBox *textbox;
   CGButton *button;
   
   this->immediate = immediate;
   this->object = *object;
   CObjPersistent(this->object.object).attribute_get(this->object.attr.attribute,
                                                     this->object.attr.element,
                                                     this->object.attr.index,
                                                     &ATTRIBUTE:type<TUNIXTime>, (void *)&this->date);
  
   new(&this->obj_server).CObjServer(CObjPersistent(this));
   new(&this->layout).CGLayout(0, 0, &this->obj_server, CObjPersistent(this));
   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_width>, 190);   
   CObjPersistent(&this->layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, FALSE);
   CObjPersistent(&this->layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_height>, 176);   
   CGCanvas(&this->layout).colour_background_set(GCOL_DIALOG);
   
   vbox = new.CGVBox();
   CObject(&this->layout).child_add(CObject(vbox));
   date_picker = new.CGDatePicker(0, 0, 190, 154, 0);

   CXPath(&CGXULElement(date_picker)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,date>, -1);
   CObject(vbox).child_add(CObject(date_picker));
   hbox = new.CGHBox();
   CObjPersistent(hbox).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);
   CObjPersistent(hbox).attribute_set_int(ATTRIBUTE<CGXULElement,height>, 22);   
   CObject(vbox).child_add(CObject(hbox));
   button = new.CGButton(0, 0, 0, 0, "midnight");
   CObject(hbox).child_add(CObject(button));
   CXPath(&CGXULElement(button)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,set_midnight>, -1);   

   textbox = new.CGTextBox(0, 0, 0, 0, "0");
   textbox->maxlength = 2;
   textbox->type = EGTextBoxType.numeric;
   CObject(hbox).child_add(CObject(textbox));

   CXPath(&CGXULElement(textbox)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,date>, 3);   
   textbox = new.CGTextBox(0, 0, 0, 0, "0");
   textbox->maxlength = 2;
   textbox->type = EGTextBoxType.numeric;
   CObject(hbox).child_add(CObject(textbox));

   CXPath(&CGXULElement(textbox)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,date>, 4);   
   textbox = new.CGTextBox(0, 0, 0, 0, "0");
   textbox->maxlength = 2;
   textbox->type = EGTextBoxType.numeric;
   CObject(hbox).child_add(CObject(textbox));

   CXPath(&CGXULElement(textbox)->binding).set(CObjPersistent(this), ATTRIBUTE<CGDialogChooseDate,date>, 5);   
     
   CGWindowDialog(this).CGWindowDialog(title, CGCanvas(&this->layout), parent);
}                                          

void CGDialogChooseDate::delete(void) {
   if (!this->immediate && CGWindowDialog(this)->button_press == 1) {
      CObjPersistent(this->object.object).attribute_set(this->object.attr.attribute,
                                                        this->object.attr.element,
                                                        this->object.attr.index,
                                                        &ATTRIBUTE:type<TUNIXTime>, (void *)&this->date);
      CObjPersistent(this->object.object).attribute_update(this->object.attr.attribute);
      CObjPersistent(this->object.object).attribute_update_end();
   }

   delete(&this->obj_server);
   class:base.delete();
}                              

void CGDialogFileSelect::new(void) {
   new(&this->filter).CGListMenuPopup();
}                           

void CGDialogFileSelect::CGDialogFileSelect(const char *title, EDialogFileSelectMode mode, CGWindow *parent) {
   this->parent = parent;
   this->title = title;
   this->mode = mode;
}                                          

void CGDialogFileSelect::delete(void) {
   delete(&this->filter);
}                              

                                                                              
MODULE::END                                                                   
                                                                              
# 17 "/home/jacob/keystone/src/graphics/graphics.c"





# 1 "/home/jacob/keystone/src/graphics/gproperties.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              












                                                                              
MODULE::INTERFACE                                                             
                                                                              

class CGLicenseFill : CObject {
 public:
   void CGLicenseFill(void);
   void fill_layout(CGLayout *layout);
};

class CGPropertiesFill : CObject {
 public:
   void CGPropertiesFill(void);
   void fill_layout(CGLayout *layout, CObjServer *obj_server, CObjPersistent *object);
};

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CGLicenseFill::CGLicenseFill(void) {
}                                

void CGLicenseFill::fill_layout(CGLayout *layout) {
   CGVBox *vbox;
   CGTextLabel *label;
   char text[KEYSTONE_LICENSE_LENGTH];
   char *token;

   CObjPersistent(layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>,  FALSE);
   CObjPersistent(layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, FALSE);
   CObjPersistent(layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_width>,  300);
   CObjPersistent(layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_height>, 150);
   CObjPersistent(layout).attribute_default_set(ATTRIBUTE<CGCanvas,colour_background>, TRUE);


   vbox = new.CGVBox();
   CObject(layout).child_add(CObject(vbox));

   CFramework(&framework).license_info(text);
   token = strtok(text, "\n");
   while (token) {
      label = new.CGTextLabel(0, 0, 350, 15, token);
      CObject(vbox).child_add(CObject(label));

      token = strtok(NULL, "\n");
   }

   label = new.CGTextLabel(0, 0, 350, 15, "");
   CObject(vbox).child_add(CObject(label));
   label = new.CGTextLabel(0, 0, 350, 15, "Aggregate Components:");
   CObject(vbox).child_add(CObject(label));

   CFramework(&framework).license_components(text);
   token = strtok(text, "\n");
   while (token) {
      label = new.CGTextLabel(0, 0, 350, 15, token);
      CObject(vbox).child_add(CObject(label));

      token = strtok(NULL, "\n");
   }
}                                

void CGPropertiesFill::CGPropertiesFill(void) {
}                                      

void CGPropertiesFill::fill_layout(CGLayout *layout, CObjServer *obj_server, CObjPersistent *object) {
   const TAttribute *attribute;
   CGTextLabel *label;
   CGTextBox *text_box;
   CGVBox *vbox;
   CGHBox *hbox;
   int i = 0;
   ARRAY<const TAttribute *> attr_list;

   CGCanvas(layout).colour_background_set(GCOL_NONE);

   ARRAY(&attr_list).new();
   CObjPersistent(object).attribute_list(&attr_list, TRUE, FALSE, FALSE);

   vbox = new.CGVBox();
   CObject(layout).child_add(CObject(vbox));

   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      attribute = ARRAY(&attr_list).data()[i];

      hbox = new.CGHBox();
      CGXULElement(hbox)->height = 22;
      CObjPersistent(hbox).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);
      CObject(vbox).child_add(CObject(hbox));

      label = new.CGTextLabel(0, 0, 120, 20, attribute->name);
      CObject(hbox).child_add(CObject(label));

      text_box = new.CGTextBox(0, 0, 0, 22, "???");
      CObjPersistent(text_box).attribute_default_set(ATTRIBUTE<CGXULElement,width>, TRUE);
      CGXULElement(text_box)->flex = 1;
      CObject(hbox).child_add(CObject(text_box));
      CXPath(&CGXULElement(text_box)->binding).set(object, attribute, ATTR_INDEX_VALUE);
   }

   ARRAY(&attr_list).delete();

   CGLayout(layout).animate(); 
}                                 

                                                                              
MODULE::END                                                                   
                                                                              
# 23 "/home/jacob/keystone/src/graphics/graphics.c"




                                                                              
MODULE::INTERFACE                                                             
                                                                              

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

                                                                              
MODULE::END                                                                   
                                                                              
# 37 "/home/jacob/keystone/src/main.c"
# 1 "/home/jacob/keystone/src/extras/extras.c"








  

                                                                              
MODULE::IMPORT                                                                
                                                                              

# 1 "/home/jacob/keystone/src/extras/ggrid.c"
                                                                              
MODULE::IMPORT                                                                
                                                                              



                                                                              
MODULE::INTERFACE                                                             
                                                                              

class CGGridEditCell : CObjPersistent {
 private:
   int index;

   void new(void);
   void delete(void);

   virtual void rebind(void);
   virtual void update(void);
 public:
   void CGGridEditCell(int index);

   ATTRIBUTE<CString value>;
   ATTRIBUTE<bool edit>;
   ATTRIBUTE<TGColour colour>;
   ATTRIBUTE<TGColour bg_colour>;
};

class CGGridEditCellBound : CGGridEditCell {
 private:
   void new(void);
   void delete(void);

   virtual void rebind(void);
   virtual void update(void);
 public:
   void CGGridEditCellBound(int index);

   ATTRIBUTE<CXPath binding>;
};

ARRAY:typedef<CGGridEditCell *>;

class CGGridEditRow : CObjPersistent {
 private:
   bool deleted;
   int index;
   int sort_index;
   unsigned long long rowid;

   void new(void);
   void delete(void);
   virtual void rebind(void);

   ARRAY<CGGridEditCell *> cell;
 public:
   void CGGridEditRow(int index);
};

class CGGridEditRowBound : CGGridEditRow {
 private:
   void new(void);
   void delete(void);
   virtual void rebind(void);
 public:
   ALIAS<"xul:gridrow">;
   ATTRIBUTE<CXPath path>;

   void CGGridEditRowBound(int index);
};

ARRAY:typedef<CGGridEditRow>;
ATTRIBUTE:typedef<CGGridEditRow *>;
ARRAY:typedef<CGGridEditRow *>;

ARRAY:typedef<CGGridEditRowBound>;
ARRAY:typedef<CGGridEditRowBound *>;

ENUM:typedef<EGGridEditSortOrder> {
   {ascending},
   {descending},
};

class CGGridEditRows : CObjPersistent {
 private:
   void new(void);
   void delete(void);
   void rebind(void);
   void sort(void);

   ARRAY<CGGridEditRow *> row_index;
   ARRAY<CGGridEditRow *> index;
 public:
   ALIAS<"xul:gridrows">;
   ATTRIBUTE<int count>;
   ATTRIBUTE<int height>;
   ATTRIBUTE<bool headings> {
      this->headings = data ? *data : TRUE;
   };

   void CGGridEditRows(void);
};

class CGGridEditColumn : CObjPersistent {
 private:
  void new(void);
  void delete(void);
 public:
   void CGGridEditColumn(void);

   ALIAS<"xul:gridcolumn">;
   ATTRIBUTE<int width>;
   ATTRIBUTE<CString heading>;
   ATTRIBUTE<CXPath binding>;
   ATTRIBUTE<bool editable> {
      this->editable = data ? *data : TRUE;
   };
};

class CGGridEditColumns : CObjPersistent {
 private:
  void new(void);
  void delete(void);
  int allocated_width;
 public:
   void CGGridEditColumns(void);

   ALIAS<"xul:gridcolumns">;
   ATTRIBUTE<bool headings> {
      if (data) {
         this->headings = *data;
      }
      else {
         this->headings = TRUE;
      }
   };
   ATTRIBUTE<CXPath path>;
   ATTRIBUTE<bool sortable> {
      this->sortable = data ? *data : TRUE;
   };
   ATTRIBUTE<int sort_index, "sortIndex"> {
      this->sort_index = data ? *data : -1;
   };
   ATTRIBUTE<EGGridEditSortOrder sort_direction, "sortDirection">;
};

class CGGridEdit;

class CGGridEditLayout : CGLayout {
 private:
   CGXULElement *cell_edit;

   void set_font(CGCanvas *canvas);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   void navigate_event(CGGridEditRow *row, CGGridEditColumn *column, CEvent *event);
   virtual bool event(CEvent *event);
   void new(void);
   void delete(void);
   void cell_extent(int row_index, int column_index, TRect *result);
 public:
   void CGGridEditLayout(CGGridEdit *grid_edit);

   void cell_edit(CGGridEditCell *cell);
   void cell_edit_update(void);

   int rows_visible;
   ATTRIBUTE<int curpos_x> {
      if (data) {
         this->curpos_x = *data;
         CGGridEditLayout(this).cell_edit_update();
      }
   };
   ATTRIBUTE<int curpos_y> {
      if (data) {
         this->curpos_y = *data;
         CGGridEditLayout(this).cell_edit_update();
      }
   };
};

ENUM:typedef<EGGridEditState> {
   {select},
   {edit},
};

ENUM:typedef<EGGridEditCellEdit> {
   {allocate},
   {release},
   {del},
};

class CGGridEdit : CGScrolledArea {
 private:
   CObjServer *obj_server;
   CObjPersistent *data_source;
   CGGridEditLayout layout;
   CSelection selection;
   EGGridEditState state;
   bool notify_selection;

   virtual bool keyboard_input(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_all_update(bool changing);
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);

   virtual void notify_cell_edit(EGGridEditCellEdit action, CGGridEditCell *cell, CGXULElement **edit_control);
   int column_max_width(CGGridEditColumn *column);

   void new(void);
   void delete(void);
 public:
   ALIAS<"xul:gridedit">;
   ELEMENT:OBJECT<CGGridEditColumns columns, "xul:gridcolumns">;
   ELEMENT:OBJECT<CGGridEditRows rows, "xul:gridrows">;
   void CGGridEdit(CObjServer *obj_server, CObjPersistent *data_source);
   void clear(void);
};

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

#define GRID_SCROLL_SIZE 18
#define GRID_ROW_HEADING_WIDTH 40

void CGGridEditCell::new(void) {
   new(&this->value).CString(NULL);

   this->colour = GCOL_BLACK;
   this->bg_colour = GCOL_WHITE;
}                       

void CGGridEditCell::CGGridEditCell(int index) {
   this->index = index;
}                                  

void CGGridEditCell::delete(void) {
   delete(&this->value);
}                          

void CGGridEditCell::rebind(void) {}
void CGGridEditCell::update(void) {}

void CGGridEditCellBound::new(void) {
   class:base.new();

   new(&this->binding).CXPath(NULL, NULL);
}                            

void CGGridEditCellBound::CGGridEditCellBound(int index) {
   CGGridEditCell(this).CGGridEditCell(index);
}                                            

void CGGridEditCellBound::delete(void) {
   delete(&this->binding);

   class:base.delete();
}                               

void CGGridEditCellBound::rebind(void) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent_find(&class(CGGridEdit)));
   CGGridEditColumn *column = CGGridEditColumn(CObject(&grid->columns).child_n(CGGridEditCell(this)->index));

   CXPath(&this->binding).path_set(CString(&column->binding.path).string());

   CGGridEditCellBound(this).update();
}                               

void CGGridEditCellBound::update(void) {



          if (this->binding.object.object) {
      CXPath(&this->binding).get_string(&CGGridEditCell(this)->value);
      if (this->binding.object.object && this->binding.object.attr.attribute) {
         CGGridEditCell(this)->colour = CObjPersistent(this->binding.object.object).attribute_default_get(this->binding.object.attr.attribute)
            ? GCOL_RGB(0xA0, 0xA0, 0xA0) : GCOL_BLACK;
      }
   }
}                               

void CGGridEditColumns::new(void) {
   new(&this->path).CXPath(NULL, NULL);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGGridEditColumns,headings>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGGridEditColumns,sortable>, TRUE);
   class:base.new();
}                          

void CGGridEditColumns::CGGridEditColumns(void) {
}                                        

void CGGridEditColumns::delete(void) {
   class:base.delete();
   delete(&this->path);
}                             

void CGGridEditColumn::new(void) {
   new(&this->binding).CXPath(NULL, NULL);
   new(&this->heading).CString(NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGGridEditColumn,editable>, TRUE);
   class:base.new();
}                         

void CGGridEditColumn::CGGridEditColumn(void) {
}                                      

void CGGridEditColumn::delete(void) {
   class:base.delete();
   delete(&this->heading);
   delete(&this->binding);
}                            

void CGGridEditRow::new(void) {
   ARRAY(&this->cell).new();
}                      

void CGGridEditRow::CGGridEditRow(int index) {
   this->index = index;
   this->sort_index = index;
}                                

void CGGridEditRow::delete(void) {
   int i;
   CGGridEditCell *cell;

   for (i = 0; i < ARRAY(&this->cell).count(); i++) {
      cell = ARRAY(&this->cell).data()[i];
      delete(cell);
   }

   ARRAY(&this->cell).delete();
}                         

void CGGridEditRow::rebind(void) {
   int i;
   CGGridEditCell *cell;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent_find(&class(CGGridEdit)));
                               

   ARRAY(&CGGridEditRow(this)->cell).used_set(CObject(&grid->columns).child_count());
   for (i = 0; i < CObject(&grid->columns).child_count(); i++) {
      cell = new.CGGridEditCell(i);
      CObject(cell).parent_set(CObject(this));
      ARRAY(&CGGridEditRow(this)->cell).data()[i] = CGGridEditCell(cell);
   }
}                         

void CGGridEditRowBound::new(void) {
   class:base.new();

   new(&this->path).CXPath(NULL, NULL);
}                           

void CGGridEditRowBound::CGGridEditRowBound(int index) {
   CGGridEditRow(this).CGGridEditRow(index);
}                                          

void CGGridEditRowBound::delete(void) {
   delete(&this->path);

   class:base.delete();
}                              

void CGGridEditRowBound::rebind(void) {
   int i;
   CGGridEditCellBound *cell;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent_find(&class(CGGridEdit)));
                               

   ARRAY(&CGGridEditRow(this)->cell).used_set(CObject(&grid->columns).child_count());
   for (i = 0; i < CObject(&grid->columns).child_count(); i++) {
      cell = new.CGGridEditCellBound(i);
      CObject(cell).parent_set(CObject(this));
      ARRAY(&CGGridEditRow(this)->cell).data()[i] = CGGridEditCell(cell);

      CXPath(&cell->binding).initial_set(&this->path.object);
      CGGridEditCell(cell).rebind();
   }
}                              

void CGGridEditRows::new(void) {
   ARRAY(&this->row_index).new();
   ARRAY(&this->index).new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGGridEditRows,headings>, TRUE);
}                       

void CGGridEditRows::CGGridEditRows(void) {
}                                  

void CGGridEditRows::delete(void) {
   ARRAY(&this->index).delete();
   ARRAY(&this->row_index).delete();
}                          

void CGGridEditRows::rebind(void) {
   int i;
   CGGridEditRow *row;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());

                                 
   for (i = 0; i < ARRAY(&this->row_index).count(); i++) {
      row = ARRAY(&this->row_index).data()[i];
      delete(row);
   }

   ARRAY(&this->row_index).used_set(this->count);
   for (i = 0; i < this->count; i++) {
      row = (CGGridEditRow *) new.CGGridEditRowBound(i);
      ARRAY(&this->row_index).data()[i] = row;
      CObject(row).parent_set(CObject(this));

      CXPath(&CGGridEditRowBound(row)->path).initial_set(&grid->columns.path.initial);
      CXPath(&CGGridEditRowBound(row)->path).index_set(i);
      CXPath(&CGGridEditRowBound(row)->path).path_set(CString(&grid->columns.path.path).string());
      CGGridEditRow(row).rebind();
   }

   CGGridEditRows(this).sort();
}                          


bool ATTRIBUTE:convert<CGGridEditRow *>(CObjPersistent *object,
                                        const TAttributeType *dest_type, const TAttributeType *src_type,
                                        int dest_index, int src_index,
                                        void *dest, const void *src) {
   CGGridEditRow *row_src, *row_dest;
   CGGridEditCell *cell_src, *cell_dest;
   CGGridEdit *grid;
   int *result;

   if (dest_type == &ATTRIBUTE:type<CGGridEditRow *> && src_type == NULL) {
      memset(dest, 0, sizeof(CGGridEditRow *));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<CGGridEditRow *> && src_type == &ATTRIBUTE:type<CGGridEditRow *>) {
      *((CGGridEditRow **)dest) = *((CGGridEditRow **)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<CGGridEditRow *>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<op_compare> && src_type == &ATTRIBUTE:type<CGGridEditRow *>) {
      result = (int *)object;
      row_src  = *(CGGridEditRow **)src;
      row_dest = *(CGGridEditRow **)dest;
      grid = CGGridEdit(CObject(row_src).parent_find(&class(CGGridEdit)));
      if (grid->columns.sort_index == -1) {
         if (row_dest->index == row_src->index) {
            *result = 0;
         }
         else if (row_dest->index < row_src->index) {
            *result = -1;
         }
         else {
            *result = 1;
         }
      }
      else {
                                                                     
         cell_dest = ARRAY(&row_dest->cell).data()[grid->columns.sort_index];
         cell_src = ARRAY(&row_src->cell).data()[grid->columns.sort_index];

         *result = CString(&cell_dest->value).strcmp(CString(&cell_src->value).string());

      }
      return TRUE;
   }

   return FALSE;
}

void CGGridEditRows::sort(void) {
   CGGridEditRow *row;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   int i;

   ARRAY(&this->index).used_set(0);
   for (i = 0; i < ARRAY(&this->row_index).count(); i++) {
      row = ARRAY(&this->row_index).data()[i];
      if (!row->deleted) {
         ARRAY(&this->index).item_add(row);
      }
   }

   ARRAY(&this->index).sort_type(&ATTRIBUTE:type<CGGridEditRow *>, grid->columns.sort_direction);

   for (i = 0; i < ARRAY(&this->index).count(); i++) {
      row = ARRAY(&this->index).data()[i];
      row->sort_index = i;
   }
}                        

void CGGridEditLayout::new(void) {
   class:base.new();
}                         

void CGGridEditLayout::CGGridEditLayout(CGGridEdit *grid_edit) {
   CGLayout(this).CGLayout(0, 0, grid_edit->obj_server, grid_edit->data_source);
}                                      

void CGGridEditLayout::delete(void) {
   class:base.delete();
}                            

void CGGridEditLayout::set_font(CGCanvas *canvas) {
   CGFontFamily font_family;

   new(&font_family).CGFontFamily();
   CString(&font_family).set("Arial");
   CGCanvas(canvas).NATIVE_font_set(&font_family, 12, EGFontStyle.normal, EGFontWeight.normal, EGTextDecoration.none);
   CGCanvas(canvas).NATIVE_text_align_set(EGTextAnchor.start, EGTextBaseline.central);
   delete(&font_family);
}                              

void CGGridEditLayout::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditColumn *column;
   CGGridEditRow *row;
   CGGridEditCell *cell;
   TPoint sort_point[3];
   int row_index, row_start, row_end;
   int column_index, column_start, column_end, x, y;
   int column_width_total = 0, row_height_total;
   TRect cell_extent;
   char str[10];

   if (!extent) {
      extent = &CGObject(canvas)->extent;
   }

   CGCanvas(canvas).stroke_style_set(1, NULL, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
   CGCanvas(canvas).draw_rectangle((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    TRUE, 0, 0, CGXULElement(this)->width, CGXULElement(this)->height);

   row_start = this->curpos_y;
   row_end = this->curpos_y + this->rows_visible + 1;
   if (row_end > grid->rows.count - 1) {
      row_end = grid->rows.count - 1;
   }
   column_start = this->curpos_x;
   column_end = CObject(&grid->columns).child_count() - 1;

   x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
   column = CGGridEditColumn(CObject(&grid->columns).child_n(column_start));
   for (column_index = column_start; column_index <= column_end; column_index++) {
      x += column->width;
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                 x, extent->point[0].y, x,                       (grid->rows.count + (grid->columns.headings ? 1 : 0)) * grid->rows.height);
      column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
   }

   CGGridEditLayout(this).set_font(canvas);

   y = grid->columns.headings ? grid->rows.height : 0;
   for (row_index = row_start; row_index <= row_end; row_index++) {
      row = ARRAY(&grid->rows.index).data()[row_index];
      x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
      column = CGGridEditColumn(CObject(&grid->columns).child_n(column_start));
      for (column_index = column_start; column_index <= column_end; column_index++) {
         cell = ARRAY(&row->cell).data()[column_index];
         CGGridEditCell(cell).update();                                                                                     
         CGCanvas(canvas).draw_rectangle(cell->bg_colour,
                                         TRUE, x + 1, y + 1, x + column->width - 1, y + grid->rows.height);
         CGCanvas(canvas).draw_text(cell->colour, x + 3, y + grid->rows.height / 2 + 1, CString(&cell->value).string());
         x += column->width;
         column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
      }
      y += grid->rows.height;
      column_width_total = x;
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                 extent->point[0].x, y,                       x, y);
   }
   row_height_total = y;

                       
   if (CSelection(&grid->selection).count() && !this->cell_edit) {
      CGCanvas(canvas).stroke_style_set(3, NULL, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
      if (CObjClass_is_derived(&class(CGGridEditCell), CObject((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object).obj_class())) {
         cell = CGGridEditCell((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object);
         row = CGGridEditRow(CObject(cell).parent());

         CGGridEditLayout(this).cell_extent(row->sort_index, cell->index, &cell_extent);
         CGCanvas(canvas).draw_rectangle(GCOL_TYPE_NAMED | EGColourNamed.black, FALSE,
                                         cell_extent.point[0].x, cell_extent.point[0].y,
                                         cell_extent.point[1].x, cell_extent.point[1].y);
      }
      if (CObjClass_is_derived(&class(CGGridEditRow), CObject((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object).obj_class())) {
         row = CGGridEditRow((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object);

         CGGridEditLayout(this).cell_extent(row->sort_index, -1, &cell_extent);
         CGCanvas(canvas).draw_rectangle(GCOL_TYPE_NAMED | EGColourNamed.black, FALSE,
                                         cell_extent.point[0].x, cell_extent.point[0].y,
                                         cell_extent.point[1].x, cell_extent.point[1].y);
      }
   }

                      
   CGCanvas(canvas).NATIVE_text_align_set(EGTextAnchor.middle, EGTextBaseline.central);
   CGCanvas(canvas).stroke_style_set(1, NULL, 0, 0, EGStrokeLineCap.butt, EGStrokeLineJoin.miter);
   if (grid->columns.headings) {
      CGCanvas(canvas).draw_rectangle((GCOL_TYPE_NAMED | EGColourNamed.lavender), TRUE,
                                      extent->point[0].x, 0, column_width_total, grid->rows.height);
      x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
      column = CGGridEditColumn(CObject(&grid->columns).child_n(column_start));
      for (column_index = column_start; column_index <= column_end; column_index++) {
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    x + 1, 0, x + column->width - 1, 0);
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    x + 1, 0, x + 1, grid->rows.height);
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                    x + column->width, 0, x + column->width, grid->rows.height);
         CGCanvas(canvas).draw_text(GCOL_TYPE_NAMED | EGColourNamed.black, x + column->width / 2, grid->rows.height / 2 + 1, CString(&column->heading).string());

         if (column_index == grid->columns.sort_index) {
            sort_point[0].x = x + 10;
            sort_point[0].y = grid->rows.height / 2 + (grid->columns.sort_direction == EGGridEditSortOrder.ascending ? -4 : 4);
            sort_point[1].x = x + 14;
            sort_point[1].y = grid->rows.height / 2 + (grid->columns.sort_direction == EGGridEditSortOrder.ascending ? 4 : -4);
            sort_point[2].x = x + 6;
            sort_point[2].y = grid->rows.height / 2 + (grid->columns.sort_direction == EGGridEditSortOrder.ascending ? 4 : -4);
            CGCanvas(canvas).draw_polygon(GCOL_TYPE_NAMED | EGColourNamed.lightgrey, TRUE, 3, sort_point, TRUE);
         }

         x += column->width;
         column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
      }
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                 extent->point[0].x, grid->rows.height,                       x, grid->rows.height);
   }

   if (grid->rows.headings) {
      y = grid->columns.headings ? grid->rows.height : 0;

      CGCanvas(canvas).draw_rectangle((GCOL_TYPE_NAMED | EGColourNamed.lavender), TRUE,
                                      extent->point[0].x, y, GRID_ROW_HEADING_WIDTH, row_height_total);
      for (row_index = row_start; row_index <= row_end; row_index++) {
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    0, y + 1, GRID_ROW_HEADING_WIDTH - 1, y + 1);
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.white),
                                    0, y + 1, 0, y + grid->rows.height - 1);
         CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                    0, y + grid->rows.height, GRID_ROW_HEADING_WIDTH, y + grid->rows.height);
         sprintf(str, "%d", (ARRAY(&grid->rows.index).data()[row_index])->index);
         CGCanvas(canvas).draw_text(GCOL_TYPE_NAMED | EGColourNamed.black, GRID_ROW_HEADING_WIDTH / 2, y + (grid->rows.height / 2) + 1,
                                    str);
         y += grid->rows.height;
      }
      CGCanvas(canvas).draw_line((GCOL_TYPE_NAMED | EGColourNamed.lightgrey),
                                 GRID_ROW_HEADING_WIDTH, extent->point[0].y, GRID_ROW_HEADING_WIDTH, y);
   }
}                          

void CGGridEditLayout::navigate_event(CGGridEditRow *row, CGGridEditColumn *column, CEvent *event) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditCell *cell = NULL;
   CSelection *selection;
   CObjPersistent *object;
   bool key_enter_event;

   if (!row) {
      return;
   }
   
   if (column) {
      cell = ARRAY(&row->cell).data()[CObject(CObject(column).parent()).child_index(CObject(column))];
      object = CObjPersistent(cell);
   }
   else {
      object = CObjPersistent(row);
   }

   key_enter_event =
      event && CObject(event).obj_class() == &class(CEventKey) &&
      !this->cell_edit &&
      (CEventKey(event)->key == EEventKey.Enter || CEventKey(event)->key == EEventKey.Ascii);

   if (event && ((CObject(event).obj_class() == &class(CEventPointer) && CEventPointer(event)->type == EEventPointer.LeftDClick) ||
       key_enter_event)) {
      if (column && CSelection(&grid->selection).count()) {
         if (cell == CGGridEditCell((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object) &&
             column->editable) {
            CGGridEditLayout(this).cell_edit(NULL);
            CGGridEditLayout(this).cell_edit(cell);

            if (CObject(event).obj_class() == &class(CEventKey) && CEventKey(event)->key != EEventKey.Enter) {
               CGXULElement(this->cell_edit).repeat_event(event);
            }
         }
      }
   }
   else if (event && CObject(event).obj_class() == &class(CEventKey) &&
      this->cell_edit && CEventKey(event)->key == EEventKey.Escape) {
                                                           
      CGGridEditLayout(this).cell_edit(NULL);
   }
   else {
      if (row && (!CSelection(&grid->selection).count() ||
                   object != (&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object)) {
         CGGridEditLayout(this).cell_edit(NULL);
         CSelection(&grid->selection).clear();
         CSelection(&grid->selection).add(object, NULL);
         CGCanvas(this).queue_draw(NULL);                                   

                                                         
         if (CObject(row).obj_class() == &class(CGGridEditRowBound)) {
            object = CGGridEditRowBound(row)->path.initial.object;
            while (object) {
               if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
                  break;
               }
               object = CObjPersistent(CObject(object).parent());
            }
         }

         selection = CObjServer(grid->obj_server).selection_get();
         if (grid->notify_selection && selection && object) {
            CObjServer(grid->obj_server).root_selection_update();
            CSelection(selection).clear();
            CSelection(selection).add(object, NULL);
            CObjServer(grid->obj_server).root_selection_update_end();
         }
      }
   }
}                                    

bool CGGridEditLayout::event(CEvent *event) {
   int row_index = -1, column_index = -1, x;
   CGGridEditRow *row = NULL;
   CGGridEditColumn *column = NULL;
   CGGridEditCell *cell = NULL;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CObjPersistent *object = NULL;
   int newpos = this->curpos_y;

   if (CObject(event).obj_class() == &class(CEventKey) && CEventKey(event)->type == EEventKeyType.down) {
                                                                        
      if (CSelection(&grid->selection).count()) {
         object = (&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object;
      }
      if (object && CObjClass_is_derived(&class(CGGridEditRow), CObject(object).obj_class())) {
         row = CGGridEditRow(object);
      }
      if (object && CObjClass_is_derived(&class(CGGridEditCell), CObject(object).obj_class())) {
         cell = CGGridEditCell(object);
         row = CGGridEditRow(CObject(cell).parent());
         column_index = cell->index;
         column = CGGridEditColumn(CObject(&grid->columns).child_n(cell->index));
      }

      if (row) {
         row_index = row->sort_index;
      }
      else {
         row_index = 0;
      }

      if (!column) {
         column_index = 0;
      }

      switch (CEventKey(event)->key) {
      case EEventKey.Delete:
         if (row && !column && CObject(row).obj_class() == &class(CGGridEditRowBound)) {
            CSelection(&grid->selection).clear();
            if (CGGridEditRowBound(row)->path.object.object) {
               delete(CGGridEditRowBound(row)->path.object.object);
            }
            row->deleted = TRUE;
            grid->rows.count--;
            CGGridEditRows(&grid->rows).sort();
            CGCanvas(this).queue_draw(NULL);                                   
         }
         break;
      case EEventKey.Up:
         if (row_index > 0) {
            row_index--;
            row = ARRAY(&grid->rows.index).data()[row_index];
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
            CGGridEditLayout(this).navigate_event(row, column, event);
            if (row_index < this->curpos_y) {
               newpos = row_index;
            }
         }
         break;
      case EEventKey.Down:
      key_down:
         if (row_index < ARRAY(&grid->rows.index).count() - 1) {
            row_index++;
            row = ARRAY(&grid->rows.index).data()[row_index];
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
            CGGridEditLayout(this).navigate_event(row, column, event);
            if (row_index >= this->curpos_y + this->rows_visible) {
               newpos = row_index - this->rows_visible + 1;
            }
         }
         break;
      case EEventKey.Left:
         if (column_index > 0) {
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index -1));
            CGGridEditLayout(this).navigate_event(row, column, event);
         }
         break;
      case EEventKey.Right:
         if (column_index < CObject(&grid->columns).child_count() - 1) {
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index + 1));
            CGGridEditLayout(this).navigate_event(row, column, event);
         }
         break;
      case EEventKey.Enter:
         if (this->cell_edit)
            goto key_down;
                             
      case EEventKey.Escape:
      case EEventKey.Ascii:
         CGGridEditLayout(this).navigate_event(row, column, event);
         break;
      case EEventKey.Tab:
         if (CEventKey(event)->modifier) {
            if (column_index > 0) {
               column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index - 1));
            }
            else {
               column = CGGridEditColumn(CObject(&grid->columns).child_last());
                  if (row_index > 0) {
                     row = ARRAY(&grid->rows.index).data()[row_index - 1];
               }
               else {
                  break;
               }
            }
         }
         else {
            if (column_index < CObject(&grid->columns).child_count() - 1) {
               column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index + 1));
            }
            else {
               column = CGGridEditColumn(CObject(&grid->columns).child_first());
               if (row_index < ARRAY(&grid->rows.index).count() - 1) {
                  row = ARRAY(&grid->rows.index).data()[row_index + 1];
               }
               else {
                  break;
               }
            }
         }
         CGGridEditLayout(this).navigate_event(row, column, event);
         break;
      case EEventKey.PgUp:
         if (row_index > this->rows_visible) {
            newpos = this->curpos_y - this->rows_visible;
            row = ARRAY(&grid->rows.index).data()[row_index - this->rows_visible];
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
            CGGridEditLayout(this).navigate_event(row, column, event);
         }
         break;
      case EEventKey.PgDn:
         if (row_index < ARRAY(&grid->rows.index).count() - this->rows_visible) {
            newpos = this->curpos_y + this->rows_visible;
            row = ARRAY(&grid->rows.index).data()[row_index + this->rows_visible];
            column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
            CGGridEditLayout(this).navigate_event(row, column, event);
         }
         break;
      case EEventKey.Home:
         newpos = 0;
         row = ARRAY(&grid->rows.index).data()[0];
         column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
         CGGridEditLayout(this).navigate_event(row, column, event);
         break;
      case EEventKey.End:
         newpos = CGScrollBar(&CGScrolledArea(grid)->scroll_vert)->maxpos;
         row = ARRAY(&grid->rows.index).data()[ARRAY(&grid->rows.index).count() - 1];
         column = CGGridEditColumn(CObject(&grid->columns).child_n(column_index));
         CGGridEditLayout(this).navigate_event(row, column, event);
         break;
      default:
         break;
      }
   }
   if (CObject(event).obj_class() == &class(CEventPointer)) {
      switch (CEventPointer(event)->type) {
      case EEventPointer.ScrollDown:
         newpos = this->curpos_y + 3;
         break;
      case EEventPointer.ScrollUp:
         newpos = this->curpos_y - 3;
         break;
      case EEventPointer.LeftDown:
      case EEventPointer.LeftDClick:
         CGXULElement(this).NATIVE_focus_in();
         CGLayout(this).key_gselection_set(CGObject(this));

         row_index = (int)(CEventPointer(event)->position.y / grid->rows.height) + this->curpos_y - grid->columns.headings;
         x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
         column = CGGridEditColumn(CObject(&grid->columns).child_n(this->curpos_x));
         column_index = this->curpos_x;
         while (column) {
            if (CEventPointer(event)->position.x >= x && CEventPointer(event)->position.x < x + column->width) {
               break;
            }
            column_index++;
            x += column->width;
            column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
         }

         if (grid->columns.headings && (int)(CEventPointer(event)->position.y / grid->rows.height) == 0 &&
             grid->columns.sortable) {
            if (!column) {
               column_index = -1;
            }
            if (column_index == grid->columns.sort_index) {
               grid->columns.sort_direction = !grid->columns.sort_direction;
            }
            else {
               grid->columns.sort_index = column_index;
               grid->columns.sort_direction = EGGridEditSortOrder.ascending;
            }
            CGGridEditRows(&grid->rows).sort();
            CGCanvas(this).queue_draw(NULL);                                   
         }
         else if (row_index < grid->rows.count) {
            if (row_index >= 0) {
               row = ARRAY(&grid->rows.index).data()[row_index];
               CGGridEditLayout(this).navigate_event(row, column, event);
            }
         }
         break;
      default:
         break;
      }
   }

   if (newpos != this->curpos_y) {
      if (newpos > CGScrollBar(&CGScrolledArea(grid)->scroll_vert)->maxpos) {
         newpos = CGScrollBar(&CGScrolledArea(grid)->scroll_vert)->maxpos;
      }
      if (newpos < 0) {
         newpos = 0;
      }
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGGridEditLayout,curpos_y>);
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGGridEditLayout,curpos_y>, newpos);
      CObjPersistent(this).attribute_update_end();
      CObjPersistent(&CGScrolledArea(grid)->scroll_vert).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
      CObjPersistent(&CGScrolledArea(grid)->scroll_vert).attribute_set_int(ATTRIBUTE<CGScrollBar,curpos>, newpos);
      CObjPersistent(&CGScrolledArea(grid)->scroll_vert).attribute_update_end();
   }

   if (CObject(event).obj_class() == &class(CEventKey)) {
      switch (CEventKey(event)->key) {
      case EEventKey.Escape:
         break;
      default:
         return TRUE;
      }
   }
   return class:base.event(event);
}                           

void CGGridEditLayout::cell_edit(CGGridEditCell *cell) {
   TRect cell_extent;
   CGGridEditRow *row;
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditCell *current_cell;
   CGWindow *window = CGWindow(CObject(this).parent_find(&class(CGWindow)));

   if (this->cell_edit) {
      current_cell = CGGridEditCell((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object);
      CGXULElement(this->cell_edit).NATIVE_focus_out();
      CGXULElement(window).NATIVE_focus_in();             
      CGGridEdit(grid).notify_cell_edit(EGGridEditCellEdit.release, current_cell, &this->cell_edit);

      delete(this->cell_edit);
      this->cell_edit = NULL;
      CGLayout(this).key_gselection_set(CGObject(this));
   }
   if (cell && CObject(cell).obj_class() == &class(CGGridEditCellBound)) {
      CGGridEdit(grid).notify_cell_edit(EGGridEditCellEdit.allocate, cell, NULL);

      row = CGGridEditRow(CObject(cell).parent());
      CGGridEditLayout(this).cell_extent(row->sort_index, cell->index, &cell_extent);
      if (CGGridEditCellBound(cell)->binding.object.object &&
          CGGridEditCellBound(cell)->binding.object.attr.attribute->type == PT_AttributeEnum) {
         if (CObjPersistent(CGGridEditCellBound(cell)->binding.object.object).attribute_default_get(CGGridEditCellBound(cell)->binding.object.attr.attribute)) {
                                                                
            return;
         }
         this->cell_edit = (CGXULElement *)new.CGMenuList((int)cell_extent.point[0].x, (int)(cell_extent.point[0].y - 1),
                                                          (int)(cell_extent.point[1].x - cell_extent.point[0].x + 1),
                                                          (int)(cell_extent.point[1].y - cell_extent.point[0].y + 2));
      }
      else {
         this->cell_edit = (CGXULElement *)new.CGTextBox((int)cell_extent.point[0].x, (int)(cell_extent.point[0].y - 1),
                                                         (int)(cell_extent.point[1].x - cell_extent.point[0].x + 1),
                                                         (int)(cell_extent.point[1].y - cell_extent.point[0].y + 2), NULL);
      }
      CObject(this).child_add(CObject(this->cell_edit));
      CXPath(&CGXULElement(this->cell_edit)->binding).set(CGGridEditCellBound(cell)->binding.object.object,
                                                          CGGridEditCellBound(cell)->binding.object.attr.attribute,
                                                          CGGridEditCellBound(cell)->binding.object.attr.index);
      CGXULElement(this->cell_edit).binding_resolve();
      CGObject(this->cell_edit).show(TRUE);
      CGXULElement(this->cell_edit).NATIVE_focus_in();
   }
}                               

void CGGridEditLayout::cell_edit_update(void) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditRow *row;
   CGGridEditCell *cell;
   TRect cell_extent;

   if (this->cell_edit) {
      cell = CGGridEditCell((&ARRAY(CSelection(&grid->selection).selection()).data()[0])->object);                                           
      row = CGGridEditRow(CObject(cell).parent());
      CGGridEditLayout(this).cell_extent(row->sort_index, cell->index, &cell_extent);

      CGXULElement(this->cell_edit)->x      = cell_extent.point[0].x;
      CGXULElement(this->cell_edit)->y      = cell_extent.point[0].y - 1;
      CObjPersistent(this->cell_edit).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(this->cell_edit).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(this->cell_edit).attribute_update_end();
   }
}                                      

void CGGridEditLayout::cell_extent(int row_index, int column_index, TRect *result) {
   CGGridEdit *grid = CGGridEdit(CObject(this).parent());
   CGGridEditColumn *column;
   int column_cur;

   result->point[0].x = grid->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
   column = CGGridEditColumn(CObject(&grid->columns).child_first());
   column_cur = 0;
   if (column_index == -1) {
      result->point[1].x = result->point[0].x;
      while (column) {
         result->point[1].x -= (column_cur < this->curpos_x) ? column->width : 0;
         result->point[1].x += column->width;
         column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
         column_cur++;
      }
   }
   else {
      while (column) {
         result->point[0].x -= (column_cur < this->curpos_x) ? column->width : 0;
         if (column_cur == column_index) {
            break;
         }
         result->point[0].x += column->width;
         column = CGGridEditColumn(CObject(&grid->columns).child_next(CObject(column)));
         column_cur++;
      }
      result->point[1].x = result->point[0].x + column->width;
   }

   result->point[0].y = (row_index - this->curpos_y + (grid->columns.headings ? 1 : 0)) * grid->rows.height;
   result->point[1].y = (row_index - this->curpos_y + (grid->columns.headings ? 2 : 1)) * grid->rows.height;
}                                 

void CGGridEdit::new(void) {
   class:base.new();

   this->notify_selection = TRUE;

   new(&this->columns).CGGridEditColumns();
   CObject(&this->columns).parent_set(CObject(this));
   new(&this->rows).CGGridEditRows();
   CObject(&this->rows).parent_set(CObject(this));
   new(&this->selection).CSelection(FALSE);

   CGContainer(this)->allow_child_repos = TRUE;
}                   

void CGGridEdit::CGGridEdit(CObjServer *obj_server, CObjPersistent *data_source) {
   this->obj_server = obj_server;
   this->data_source = data_source;
}                          

void CGGridEdit::delete(void) {
   delete(&this->selection);
   delete(&this->rows);
   delete(&this->columns);

   class:base.delete();
}                      

void CGGridEdit::clear(void) {
   CObject *child, *next;

   CSelection(&this->selection).clear();

   child = CObject(&this->columns).child_first();
   while (child) {
      next = CObject(&this->columns).child_next(child);
      delete(child);
      child = next;
   }
   this->rows.count = 0;
   CGGridEditRows(&this->rows).rebind();
}                     

bool CGGridEdit::keyboard_input(void) {
   return TRUE;
}

void CGGridEdit::notify_cell_edit(EGGridEditCellEdit action, CGGridEditCell *cell, CGXULElement **edit_control) { }

void CGGridEdit::NATIVE_allocate(CGLayout *layout) {
   CGObject(this)->native_object = (void *)-1;




   new(&this->layout).CGGridEditLayout(this);
   CObject(&this->layout).parent_set(CObject(this));
   CGLayout(&this->layout).render_set(EGLayoutRender.buffered);
   CGXULElement(&this->layout).NATIVE_allocate(layout);
   CGObject(&this->layout).show(TRUE);

   new(&CGScrolledArea(this)->scroll_horiz).CGScrollBar(0, 0, 0, 0);
   CObject(&CGScrolledArea(this)->scroll_horiz).parent_set(CObject(this));
   CGXULElement(&CGScrolledArea(this)->scroll_horiz).NATIVE_allocate(layout);
   CGObject(&CGScrolledArea(this)->scroll_horiz).show(TRUE);
   CXPath(&CGXULElement(&CGScrolledArea(this)->scroll_horiz)->binding).set(CObjPersistent(&this->layout), ATTRIBUTE<CGGridEditLayout,curpos_x>, -1);

   new(&CGScrolledArea(this)->scroll_vert).CGScrollBar(0, 0, 0, 0);
   CGScrolledArea(this)->scroll_vert.orient = EGBoxOrientType.vertical;
   CObject(&CGScrolledArea(this)->scroll_vert).parent_set(CObject(this));
   CGXULElement(&CGScrolledArea(this)->scroll_vert).NATIVE_allocate(layout);
   CGObject(&CGScrolledArea(this)->scroll_vert).show(TRUE);
   CXPath(&CGXULElement(&CGScrolledArea(this)->scroll_vert)->binding).set(CObjPersistent(&this->layout), ATTRIBUTE<CGGridEditLayout,curpos_y>, -1);
}                               

void CGGridEdit::NATIVE_release(CGLayout *layout) {
   CGObject(this)->native_object = NULL;

   delete(&CGScrolledArea(this)->scroll_horiz);
   delete(&CGScrolledArea(this)->scroll_vert);

   CGXULElement(&this->layout).NATIVE_release(layout);
   delete(&this->layout);
}                              

int CGGridEdit::column_max_width(CGGridEditColumn *column) {
   CGGridEditRow *row;
   CGGridEditCell *cell;
   int row_index, column_index, width, max_width = 0;
   TRect text_extent;

   CGCanvas(&this->layout).NATIVE_enter(TRUE);
   CGGridEditLayout(&this->layout).set_font(CGCanvas(&this->layout));

   column_index = CObject(&this->columns).child_index(CObject(column));
   for (row_index = 0; row_index < ARRAY(&this->rows.index).count(); row_index++) {
      row = ARRAY(&this->rows.index).data()[row_index];
      cell = ARRAY(&row->cell).data()[column_index];
      CGGridEditCell(cell).update();
      CGCanvas(&this->layout).NATIVE_text_extent_get_string(0, 0, &cell->value, &text_extent);
      width = (int)(text_extent.point[1].x - text_extent.point[0].x);
      max_width = width > max_width ? width : max_width;
   }
   CGCanvas(&this->layout).NATIVE_enter(FALSE);

   max_width = max_width < 120 ? 120 : max_width + 5;

   return max_width;
}                                

void CGGridEdit::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGGridEditColumn *column;
   int width;
   int columns_visible;

   if (changing)
      return;

   if (CGObject(this)->native_object) {
      columns_visible = 0;
      width = this->rows.headings ? GRID_ROW_HEADING_WIDTH : 0;
      column = CGGridEditColumn(CObject(&this->columns).child_last());
      column = column ? CGGridEditColumn(CObject(&this->columns).child_previous(CObject(column))) : NULL;
      while (column) {
         if (CObjPersistent(column).attribute_default_get(ATTRIBUTE<CGGridEditColumn,width>)) {
            column->width = CGGridEdit(this).column_max_width(column);
         }
         width += column->width;
         if (width <= CGXULElement(this)->width) {
            columns_visible++;
         }
         column = CGGridEditColumn(CObject(&this->columns).child_previous(CObject(column)));
      }
      column = CGGridEditColumn(CObject(&this->columns).child_last());
      if (column) {
         if (CObjPersistent(column).attribute_default_get(ATTRIBUTE<CGGridEditColumn,width>)) {
            column->width = (int)(CGXULElement(this)->width - width);
            if (column->width < 120) {
               column->width = 120;
            }
         }
         width += column->width;
      }
      if (width <= CGXULElement(this)->width) {
         columns_visible++;
      }
      this->columns.allocated_width = width;
      CGXULElement(&CGScrolledArea(this)->scroll_horiz)->x      = CGXULElement(this)->x;
      CGXULElement(&CGScrolledArea(this)->scroll_horiz)->y      = CGXULElement(this)->y + CGXULElement(this)->height - GRID_SCROLL_SIZE;
      CGXULElement(&CGScrolledArea(this)->scroll_horiz)->width  = CGXULElement(this)->width - GRID_SCROLL_SIZE;
      CGXULElement(&CGScrolledArea(this)->scroll_horiz)->height = GRID_SCROLL_SIZE;
      CGScrollBar(&CGScrolledArea(this)->scroll_horiz)->curpos  = 0;
      CGScrollBar(&CGScrolledArea(this)->scroll_horiz)->maxpos  = CObject(&this->columns).child_count() - columns_visible;
      CGScrollBar(&CGScrolledArea(this)->scroll_horiz)->range   = CObject(&this->columns).child_count() - 1;
      CGObject(&CGScrolledArea(this)->scroll_horiz)->visibility = columns_visible < CObject(&this->columns).child_count();
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGObject,visibility>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGScrollBar,maxpos>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGScrollBar,range>);
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update_end();

      CGXULElement(&CGScrolledArea(this)->scroll_vert)->x      = CGXULElement(this)->x + CGXULElement(this)->width - GRID_SCROLL_SIZE;
      CGXULElement(&CGScrolledArea(this)->scroll_vert)->y      = CGXULElement(this)->y;
      CGXULElement(&CGScrolledArea(this)->scroll_vert)->width  = GRID_SCROLL_SIZE;
      CGXULElement(&CGScrolledArea(this)->scroll_vert)->height = CGXULElement(this)->height - (CGObject(&CGScrolledArea(this)->scroll_horiz)->visibility ? GRID_SCROLL_SIZE : 0);
      this->layout.rows_visible = (int)(CGXULElement(this)->height / this->rows.height - (this->columns.headings ? 1 : 0));
      if (this->rows.count > this->layout.rows_visible &&
         CGScrollBar(&CGScrolledArea(this)->scroll_vert)->curpos > (this->rows.count - this->layout.rows_visible)) {
         this->layout.curpos_y = this->rows.count - this->layout.rows_visible;
         CGScrollBar(&CGScrolledArea(this)->scroll_vert)->curpos = this->layout.curpos_y;
      }

      CGScrollBar(&CGScrolledArea(this)->scroll_vert)->maxpos  = this->rows.count - this->layout.rows_visible;
      CGScrollBar(&CGScrolledArea(this)->scroll_vert)->range   = this->rows.count - 1;
      CGObject(&CGScrolledArea(this)->scroll_vert)->visibility = this->layout.rows_visible < this->rows.count;
      CObjPersistent(&CGScrolledArea(this)->scroll_horiz).attribute_update(ATTRIBUTE<CGObject,visibility>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGScrollBar,maxpos>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update(ATTRIBUTE<CGScrollBar,range>);
      CObjPersistent(&CGScrolledArea(this)->scroll_vert).attribute_update_end();

      CGXULElement(&this->layout)->x      = CGXULElement(this)->x;
      CGXULElement(&this->layout)->y      = CGXULElement(this)->y;
      CGXULElement(&this->layout)->width  = CGXULElement(this)->width - (CGObject(&CGScrolledArea(this)->scroll_vert)->visibility ? GRID_SCROLL_SIZE : 0);
      CGXULElement(&this->layout)->height = CGXULElement(this)->height - (CGObject(&CGScrolledArea(this)->scroll_horiz)->visibility ? GRID_SCROLL_SIZE : 0);
      CObjPersistent(&this->layout).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(&this->layout).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(&this->layout).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(&this->layout).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(&this->layout).attribute_update_end();
   }

   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
}                                       

void CGGridEdit::notify_all_update(bool changing) {
   if (!changing) {
      CGGridEditRows(&this->rows).rebind();
   }

}                                 

void CGGridEdit::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGLayout *layout;
   switch (linkage) {
   case EObjectLinkage.ParentSet:
      layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
      this->obj_server = CObjClient(&layout->client)->server;
      this->data_source = ARRAY(&CObjClient(&layout->client)->object_root).data()[0];
      break;
   default:
      break;
   }
}                                     

                                                                              
MODULE::END                                                                   
                                                                              

# 16 "/home/jacob/keystone/src/extras/extras.c"
# 1 "/home/jacob/keystone/src/extras/sqlclient.c"
                                                                              
MODULE::IMPORT                                                                
                                                                              



                                                                              
MODULE::INTERFACE                                                             
                                                                              

#include "/home/jacob/keystone/src/extras/sqlconn.h"








typedef struct {
   CObject *notify_object;
   SQL_NOTIFY_RESULT notify_method;
   bool notify_abort;
} TSQLExecRequest;

ARRAY:typedef<TSQLExecRequest>;
ARRAY:typedef<TSQLExecRequest *>;

class CSQLClient : CObjPersistent {
 private:
   struct SQLConn *sqlconn;
   void new(void);   
   void delete(void);
 public:
   void CSQLClient(void);
   virtual int open(const char *db_filename);
   virtual TSQLExecRequest *exec(const char *sql_statement, CObject *notify_object, SQL_NOTIFY_RESULT notify_method);
   virtual int close(void);
   static inline void exec_abort(TSQLExecRequest *exec_request);
   
   ATTRIBUTE<CString database_name>;
};

static inline void CSQLClient::exec_abort(TSQLExecRequest *exec_request) {
   exec_request->notify_abort = TRUE;
}                          

class CSQLClientLocal : CSQLClient {
 private:
   void new(void);   
   void delete(void);
   struct SQLConn *sqlconn_server;   
 
   ARRAY<byte> client_message;
   ARRAY<byte> server_message;
   void write_client_message(int count, const byte *data);
   void write_server_message(int count, const byte *data);
   
   CObject *notify_object;
   SQL_NOTIFY_RESULT notify_method;
 public:
   void CSQLClientLocal(void); 
   virtual int open(const char *db_filename);
   virtual TSQLExecRequest *exec(const char *sql_statement, CObject *notify_object, SQL_NOTIFY_RESULT notify_method);
   virtual int close(void);
};

                            



class CGLayoutSQLTest : CGLayout {
 private:
   void new(void);
   void delete(void);
 
   CObjServer obj_server;    
   CSQLClient *sql_client;
   
   void notify_sql_result(int result, int rows, int columns, const char **data);
   bool notify_request_close(void);
 
   CGGridEdit *grid;
 public:
   void CGLayoutSQLTest(CSQLClient *client, const char *db_query, const char *db_error);
   void notify_sql_result(int result, int rows, int columns, const char **data);
 
   ATTRIBUTE<CString database_name>;   
   ATTRIBUTE<CString sql_query>;
   ATTRIBUTE<bool sql_execute> {
      CSQLClient(this->sql_client).exec(CString(&this->sql_query).string(), 
                                        CObject(this), (SQL_NOTIFY_RESULT)&CGLayoutSQLTest::notify_sql_result);
   };
   ATTRIBUTE<CString sql_error>;
};

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

void CSQLClient::new(void) {
   new(&this->database_name).CString(NULL);

   class:base.new();
}                   

void CSQLClient::CSQLClient(void) {
   sqlconn_new(&this->sqlconn, SQL_Local);
}                          

void CSQLClient::delete(void) {
   class:base.delete();
   
   delete(&this->database_name);   

   sqlconn_delete(this->sqlconn);   
}                      

int CSQLClient::open(const char *db_filename) {
  CString(&this->database_name).set(db_filename);
  return sqlconn_open(this->sqlconn, db_filename);
}                    

TSQLExecRequest *CSQLClient::exec(const char *sql_statement, CObject *notify_object, SQL_NOTIFY_RESULT notify_method) {
   sqlconn_exec(this->sqlconn, sql_statement, notify_object, notify_method);
   return NULL;
}                    

int CSQLClient::close(void) {
  return sqlconn_close(this->sqlconn);
}                     

void CSQLClientLocal::new(void) {
   ARRAY(&this->client_message).new();
   ARRAY(&this->server_message).new();
   class:base.new();
}                        

void CSQLClientLocal::CSQLClientLocal(void) {
   sqlconn_new(&CSQLClient(this)->sqlconn, SQL_Client);
   sqlconn_new(&this->sqlconn_server, SQL_Server);   
}                                    

void CSQLClientLocal::delete(void) {
   class:base.delete();
   sqlconn_delete(this->sqlconn_server);      

   ARRAY(&this->server_message).delete();
   ARRAY(&this->client_message).delete();
}                        

void CSQLClientLocal::write_client_message(int count, const byte *data) {
   int i;
   
   if (data) {
      for (i = 0; i < count; i++) {
         ARRAY(&this->client_message).item_add(data[i]);
      }
   }
   else {





      
      sqlconn_server_message(this->sqlconn_server, ARRAY(&this->client_message).count(), ARRAY(&this->client_message).data(), 
                             CObject(this), (SQL_WRITE_MESSAGE)&CSQLClientLocal::write_server_message);
                            
      ARRAY(&this->client_message).used_set(0);
   }
}                                         

void CSQLClientLocal::write_server_message(int count, const byte *data) {
   int i;
   
   if (data) {
      for (i = 0; i < count; i++) {
         ARRAY(&this->server_message).item_add(data[i]);
      }
   }
   else {





      
      sqlconn_client_message(CSQLClient(this)->sqlconn, ARRAY(&this->server_message).count(), ARRAY(&this->server_message).data(), 
                             this->notify_object, this->notify_method);

      ARRAY(&this->server_message).used_set(0);
   }
}                                         

int CSQLClientLocal::open(const char *db_filename) {
   sqlconn_message_open(CSQLClient(this)->sqlconn, db_filename, CObject(this), (SQL_WRITE_MESSAGE)&CSQLClientLocal::write_client_message);
   return 0;
}                         

TSQLExecRequest *CSQLClientLocal::exec(const char *sql_statement, CObject *notify_object, SQL_NOTIFY_RESULT notify_method) {
   this->notify_object = notify_object;
   this->notify_method = notify_method;
   sqlconn_message_exec(CSQLClient(this)->sqlconn, sql_statement, CObject(this), (SQL_WRITE_MESSAGE)&CSQLClientLocal::write_client_message);
   
   return NULL;
}                         

int CSQLClientLocal::close(void) {
   sqlconn_message_close(CSQLClient(this)->sqlconn, CObject(this), (SQL_WRITE_MESSAGE)&CSQLClientLocal::write_client_message);
   return 0;   
}                          

extern unsigned char _file_sqltest_svg[];

void CGLayoutSQLTest::notify_sql_result(int result, int rows, int columns, const char **data) {
   int i, j;
   CGGridEditColumn *column;
   CGGridEditRow *row;
   CGGridEditCell *cell;
   const char *cell_data;

   CGGridEdit(this->grid).clear();
   
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGLayoutSQLTest,sql_error>);
   CString(&this->sql_error).set(result == 0 ? NULL : data[0]);
   CObjPersistent(this).attribute_update_end();   
   
   if (result != 0) {

      CObjPersistent(this->grid).attribute_update(NULL);
      CObjPersistent(this->grid).attribute_update_end();
      return;
   }

                              
   this->grid->columns.sort_index = -1;   
   this->grid->columns.sort_direction = EGGridEditSortOrder.ascending;
   for (j = 0; j < columns; j++) {
      column = new.CGGridEditColumn();
      column->width = 120;
      column->editable = FALSE;
      CString(&column->heading).set(data[j]);
      CObject(&this->grid->columns).child_add(CObject(column));
   }
   this->grid->rows.count = rows;
   CGGridEditRows(&this->grid->rows).rebind();
   
   for (i = 0; i < rows; i++) {
      row = ARRAY(&this->grid->rows.row_index).data()[i];
      for (j = 0; j < columns; j++) {
         column = CGGridEditColumn(CObject(&this->grid->columns).child_n(j));
       
         cell = ARRAY(&row->cell).data()[j];
         
         cell_data = data[(i + 1) * columns + j];
         CString(&cell->value).set(cell_data ? cell_data : "NULL");
         
         if (CString(&cell->value).length() * 8 >= column->width) {
            column->width = CString(&cell->value).length() * 8;
         }
      }
   }
   
   CObjPersistent(this->grid).attribute_update(NULL);
   CObjPersistent(this->grid).attribute_update_end();

}                                      

void CGLayoutSQLTest::new(void) {
   new(&this->database_name).CString(NULL);
   new(&this->sql_query).CString(NULL);   
   new(&this->sql_error).CString(NULL);   

   class:base.new();
}                        

unsigned char *link_me = &_file_sqltest_svg[0];

void CGLayoutSQLTest::CGLayoutSQLTest(CSQLClient *sql_client, const char *db_query, const char *db_error) {
   CObject *child;

   this->sql_client = sql_client;
   
   CString(&this->database_name).set_string(&sql_client->database_name);
   CString(&this->sql_query).set(db_query);
   CString(&this->sql_error).set(db_error);   

   new(&this->obj_server).CObjServer(CObjPersistent(this));

   CGLayout(this).CGLayout(0, 0, &this->obj_server, CObjPersistent(this));
   CGLayout(this).load_svg_file("memfile:sqltest.svg", NULL);   
   child = CObject(this).child_tree_first();
   while (child) {
      if (CObjClass_is_derived(&class(CGGridEdit), CObject(child).obj_class())) {
         this->grid = CGGridEdit(child);
      }
      child = CObject(this).child_tree_next(CObject(child));
   }
}                                    

void CGLayoutSQLTest::delete(void) {
   class:base.delete();

   delete(&this->sql_error);   
   delete(&this->sql_query);      
   delete(&this->database_name);   
   
   delete(&this->obj_server);
}                           

                                                                              
MODULE::END                                                                   
                                                                              

# 17 "/home/jacob/keystone/src/extras/extras.c"

                                                                              
MODULE::INTERFACE                                                             
                                                                              

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

                                                                              
MODULE::END                                                                   
                                                                              
# 38 "/home/jacob/keystone/src/main.c"

                      
#if 0
CClassLib *CLASSLIB_new(CClassLib *this);
bool CLASSLIB_load(CClassLib *this, CObjClass *base, char *filename);
#endif

                                                                              
MODULE::INTERFACE                                                             
                                                                              

              
ARRAY:typedef<CString>;

class CApplication : CObjPersistent {
 private:
   void new(void);
   void delete(void);
   ATTRIBUTE<CString default_help, "defaultHelp">;
   ATTRIBUTE<CString home_path, "homePath">;
   void NATIVE_set_home_path(char *argv0);
 public:

   void CApplication(void);
   virtual int main(ARRAY<CString> *args);
   virtual bool notify_event(CObject *dest, CEvent *event);
};

                      
#if 0
CClassLib *CLASSLIB_new(CClassLib *this);
bool CLASSLIB_load(CClassLib *this, CObjClass *base, char *filename);
#endif

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

#include "/home/jacob/keystone/version.h"

                          
CObjClass *class_primitive = &class(CGPrimitive);
CObjClass *class_primcontainer = &class(CGPrimContainer);
CObjClass *class_animinput = &class(CGAnimateInput);

extern CMemFileDirectory fw_memfile_directory;

int CApplication::main(ARRAY<CString> *args) {
   return 0;
}                      

bool CApplication::notify_event(CObject *dest, CEvent *event) {
   return FALSE;
}                              

void CApplication::new(void) {
   new(&this->default_help).CString(NULL);
   new(&this->home_path).CString(NULL);
}                     

void CApplication::CApplication(void) {
}                              

void CApplication::delete(void) {
   delete(&this->home_path);
   delete(&this->default_help);
}                        

extern char *app_initdata_begin, *app_initdata_end, *app_uninitdata_begin;

CFramework framework;
CMutex framework_scratch_mutex;

static byte keystone_scratch_buffer[KEYSTONE_SCRATCH_BUFFERS][65536];

void *CFramework::scratch_buffer_allocate(void) {
   int i;
   void *result = NULL;

   CMutex(&framework_scratch_mutex).lock();

   for (i = 0; i < KEYSTONE_SCRATCH_BUFFERS; i++) {
      if (!this->scratch_buffer[i]) {
         this->scratch_buffer[i] = keystone_scratch_buffer[i];
         result = this->scratch_buffer[i];
         break;
      }
   }

   CMutex(&framework_scratch_mutex).unlock();

   if (!result) {
      ASSERT("FRAMEWORK:Out of scratch bufferes");
   }

   return result;
}                                       

void CFramework::scratch_buffer_release(void *buffer) {
   int i;
   CMutex(&framework_scratch_mutex).lock();

   for (i = 0; i < KEYSTONE_SCRATCH_BUFFERS; i++) {
      if (buffer == this->scratch_buffer[i]) {
         this->scratch_buffer[i] = NULL;
         break;
      }
   }

   if (i == KEYSTONE_SCRATCH_BUFFERS) {
      ASSERT("FRAMEWORK:scratch buffer release failed");
   }

   CMutex(&framework_scratch_mutex).unlock();
}                                      







CObjClass *CFramework::obj_class_find(const char *class_name) {
   CObjClass *obj_class;
   int i;
   const char *cp;

   for (i = 0; i < ARRAY(&framework.obj_class).count(); i++) {
      obj_class = ARRAY(&framework.obj_class).data()[i];

      if (strcmp(obj_class->name, class_name) == 0 ||
         (obj_class->alias && strcmp(obj_class->alias, class_name) == 0))
         return obj_class;
   }

                                                                         

                                             
   for (i = 0; i < ARRAY(&framework.obj_class).count(); i++) {
      obj_class = ARRAY(&framework.obj_class).data()[i];

      cp = strchr(class_name, ':');
      cp = cp ? cp + 1 : class_name;

      if (strcmp(CObjClass_alias_short(obj_class), cp              ) == 0)
         return obj_class;
   }

   return NULL;
}                              

void CFramework::obj_class_list_all(void) {
   CObjClass *obj_class;
   int i;

   for (i = 0; i < ARRAY(&framework.obj_class).count(); i++) {
      obj_class = ARRAY(&framework.obj_class).data()[i];
      printf("%s\n", obj_class->name);
   }
}                                  

void CFramework::initdata_search(void) {
   char *dp, *data_end;


   data_end = (char *)&app_uninitdata_begin;

   dp = (char *)&app_initdata_begin;

   while (dp < data_end) {
      if (*((unsigned int *)dp) == SID_INIT) {
         switch (((TInit *)dp)->type) {
         case IT_Class:
            this->obj_class_current = (CObjClass *)dp;
            ARRAY(&this->obj_class).item_add(this->obj_class_current);
            break;
         case IT_ObjectDef:
            ARRAY(&this->auto_object).item_add((TObjectDef *)dp);
            break;
         default:
            break;
         }
      }
      dp += sizeof(unsigned int);
   }
}                               

CApplication *CFramework::construct_auto_object(void) {
   TObjectDef *obj_def;
   CApplication *application = NULL;
   int i;

   for (i = 0; i < ARRAY(&framework.auto_object).count(); i++) {
      obj_def = ARRAY(&framework.auto_object).data()[i];

                                               
      if (CObjClass_is_derived(&class(CApplication), obj_def->obj_class)) {
         application = (CApplication *)obj_def->obj;
      }

                                         
      new(obj_def->obj).class(obj_def->obj_class);

                                              
      if (obj_def->data) {
         CObject((CObject *)obj_def->obj).notify_object_linkage(EObjectLinkage.ParentSet, (void *)obj_def->data);
      }
   }

   return application;
}                                     

void CFramework::new(void) {
   int i;
   TObjectDef *odef;

   new(&fw_memfile_directory).CMemFileDirectory();
   this->memfile_directory = &fw_memfile_directory;

   ARRAY(&framework.obj_class).new();
   ARRAY(&framework.auto_object).new();
   ARRAY(&framework.window).new();

   CFramework(this).initdata_search();

                                 
   for (i = 0; i < ARRAY(&framework.auto_object).count(); i++) {
      odef = ARRAY(&framework.auto_object).data()[i];
   }



   CFramework(this).NATIVE_new();

   new(&framework_scratch_mutex).CMutex();
}                   

void CFramework::delete(void) {
   int i;
   TObjectDef *odef;

   delete(&framework_scratch_mutex);

   CFramework(this).NATIVE_delete();

   for (i = 0; i < ARRAY(&framework.auto_object).count(); i++) {
      odef = ARRAY(&framework.auto_object).data()[i];

      odef++;
   }

   ARRAY(&framework.window).delete();
   ARRAY(&framework.auto_object).delete();
   ARRAY(&framework.obj_class).delete();


}                      

void CFramework::license_info(char *result) {
   sprintf(result, "%s", VERSION_STRING"\n");
   sprintf(result + strlen(result), "Platform: ");
#if OS_Win32
   sprintf(result + strlen(result), "Win32\n");
#endif
#if OS_Linux
   sprintf(result + strlen(result), "GNU/Linux\n");
#endif
   sprintf(result + strlen(result), "Copyright (c) 2006, Jacob Lister, Abbey Systems\n");
   sprintf(result + strlen(result), "Rights reserved, refer to\n");
   sprintf(result + strlen(result), "http:/");
   sprintf(result + strlen(result), "/www.keystoneframework.org/license.html\n");
}                            

void CFramework::license_components(char *result) {
   sprintf(result,
            "Expat v1.95.7\n"
            );
}                                  

void CFramework::main(void) {
   if (framework.executing)
     ASSERT("FRAMEWORK_execute:Framework already executing");

#if 1 
                                                  
   framework.executing = TRUE;
   CFramework(this).NATIVE_main();
   framework.executing = FALSE;
#else
                                              
   try {
      framework.executing = TRUE;
      CFramework(this).NATIVE_main();
   }
   catch(NULL, EXCEPTION<CGWindow,NoWindows>) {
   }
   finally {
      framework.executing = FALSE;
   }
#endif
}                    

void CFramework::kill(void) {
   if (!framework.executing)
     ASSERT("FRAMEWORK_kill:Framework not executing");
   CFramework(this).NATIVE_kill();
}                    

int CFramework::execute(int argc, char *argv[]) {
   ARRAY<CString> args;
   CString *string;
   int i, result;

   ARRAY(&args).new();
   for (i = 0; i < argc; i++) {
                                           
      ARRAY(&args).item_add_empty();
      string = ARRAY(&args).item_last();
      new(string).CString(argv[i]);
   }

   framework.application = CFramework(&framework).construct_auto_object();

   if (argc) {
      CApplication(framework.application).NATIVE_set_home_path(argv[0]);
   }

   if (!framework.application) {
      ASSERT("FRAMEWORK: Application Object not found");
   }

   result = CApplication(framework.application).main(&args);

   for (i = 0; i < ARRAY(&args).count(); i++) {
      delete(CString(&ARRAY(&args).data()[i]));
   }
   ARRAY(&args).delete();


   exit(result);
}                       

                                                                              
MODULE::END                                                                   
                                                                              


# 13 "/home/jacob/keystone/src/base/framework.c"

# 6 "/home/jacob/keystone/examples/sql/client.c"
# 1 "../../src/socket.c"








  
                                                                              
MODULE::IMPORT                                                                
                                                                              




                                                                              
MODULE::INTERFACE                                                             
                                                                              


                                  
#if OS_Win32
#include <io.h>
#include <fcntl.h>
#include <winsock2.h>
#include "/home/jacob/keystone/src/win32/native.h"
#endif
#if OS_Linux
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "/home/jacob/keystone/src/linux/native.h"
#endif

#if !OS_PalmOS
class CSocket : CIOObject {
 public:
   void CSocket(int domain, int type, int protocol);

   virtual void write_string(char *buffer);
   virtual void read_string(CString *string);

   int bind(struct sockaddr *my_addr, int addrlen);
   int connect(struct sockaddr *my_addr, int addrlen);
   CSocket *accept(CSocket *socket, struct sockaddr *my_addr, socklen_t *addrlen);
   int listen(int backlog);
   int send(const void *msg, size_t len, int flags);
   int sendto(const char *buf, int len, int flags, const struct sockaddr *to, int tolen);
   int recv(char *buf, int len, int flags);
   int recvfrom(char *buf, int len, int flags, struct sockaddr *from, socklen_t *fromlen);
   void close(void);
};
#endif

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

#if !OS_PalmOS
void CSocket::CSocket(int domain, int type, int protocol) {
   CIOObject(this)->handle = socket(domain, type, protocol);
}                    

void CSocket::write_string(char *buffer) {
   int n;
                                                     
   n = CSocket(this).send(buffer, strlen(buffer) + 1, 0);
   if (n != (int)(strlen(buffer) + 1)) {
      ASSERT("incomplete send");
   }
}                         

int CSocket::read_string(CString *string) {
   int i, n, recv_count;
   char message[2000];
   bool done = FALSE;

   CString(string).clear();

   while (!done) {
      n = CSocket(this).recv(message, sizeof(message), MSG_PEEK);                    
      if (n <= 0) {
          return n;
      }

      for (i = 0; i < n; i++) {
         if (message[i] == '\0') {
            done = TRUE;
            break;
         }
      }
      i++;

      recv_count = 0;
      while (i > 0) {
         n = CSocket(this).recv(message + recv_count, i, 0);
         if (n <= 0) {
             return n;
         }
         i -= n;
         recv_count += n;
      }
      CString(string).appendn(message, recv_count);
   }
   return 0;
}                        

int CSocket::bind(struct sockaddr *my_addr, int addrlen) {
   return bind(CIOObject(this)->handle, my_addr, addrlen);
}                 

int CSocket::connect(struct sockaddr *my_addr, int addrlen) {
   return connect(CIOObject(this)->handle, my_addr, addrlen);
}                    

CSocket *CSocket::accept(CSocket *socket, struct sockaddr *my_addr, socklen_t *addrlen) {
                          
   if (!socket) {
      socket = CSocket(new.class(&class(CSocket)));
   }

   memcpy(socket, this, sizeof(CSocket));

   CIOObject(socket)->handle = accept(CIOObject(this)->handle, my_addr, addrlen);
   return socket;
}                   

int CSocket::listen(int backlog) {
   return listen(CIOObject(this)->handle, backlog);
}                   

int CSocket::send(const void *msg, size_t len, int flags) {
   return send(CIOObject(this)->handle, msg, len, flags);
}                 

int CSocket::sendto(const char *buf, int len, int flags, const struct sockaddr *to, int tolen) {
   return sendto(CIOObject(this)->handle, buf, len, flags, to, tolen);
}                   

int CSocket::recv(char *buf, int len, int flags) {
   return recv(CIOObject(this)->handle, buf, len, flags);
}                 

int CSocket::recvfrom(char *buf, int len, int flags, struct sockaddr *from, socklen_t *fromlen) {
   return recvfrom(CIOObject(this)->handle, buf, len, flags, from, fromlen);
}                     

void CSocket::close(void) {

}                  

#endif

                                                                              
MODULE::END                                                                   
                                                                              


# 7 "/home/jacob/keystone/examples/sql/client.c"

                                                                              
MODULE::INTERFACE                                                             
                                                                              

class CSQLClientApp : CApplication {
 private:
   CSocket socket;
   struct SQLConn *sql_conn;
   
   byte tx_buf[1000];
   int tx_buf_count;
   byte rx_buf[500000];
   int rx_buf_count;
   void write_message(int count, const unsigned char *data);
   void notify_result(int result, int rows, int columns, const char **data);
 public:
   void main(ARRAY<CString> *args);
};

                                                                              
MODULE::IMPLEMENTATION                                                        
                                                                              

#include <windows.h>
bool socket_init(void) {
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
   
   return (WSAStartup(wVersionRequested, &wsaData) != SOCKET_ERROR);
}

#define PORT 		0x1234
#define DIRSIZE 	8192

OBJECT<CSQLClientApp, sqlclientapp>;

void CSQLClientApp::write_message(int count, const unsigned char *data) {
   if (count == 0) {
      CSocket(&this->socket).send(this->tx_buf, this->tx_buf_count, 0);
      this->tx_buf_count = 0;
   }
   else {
      memcpy(&this->tx_buf[this->tx_buf_count], data, count);
      this->tx_buf_count += count;
   }
}                                

void CSQLClientApp::notify_result(int result, int rows, int columns, const char **data) {
   printf("sql result %d, rows=%d cols=%d\n", result, rows, columns);
}                                

int CSQLClientApp::main(ARRAY<CString> *args) {
   char dir[DIRSIZE];  
	int cc, fromlen, tolen;
	int addrlen;
	struct sockaddr_in sin;
	struct sockaddr_in pin;
	struct hostent *hp;	
   CSocket *current;
   int repeat = 1;

	if (!socket_init()) {
		printf("can't init");
		exit(1);
		}
      
   if (ARRAY(args).count() < 3) {
      printf("usage 'client <hostname> <sqlstatement>'\n");
      exit(1);
   }
   
   if (ARRAY(args).count() > 3) {
      repeat = atoi(CString(&ARRAY(args).data()[3]).string());
   }
   
	if ((hp = gethostbyname(CString(&ARRAY(args).data()[1]).string())) == 0) {
		perror("gethostbyname");
		exit(1);
	}		
   
	memset(&pin, 0, sizeof(sin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	pin.sin_port = htons(PORT);
	
   new(&this->socket).CSocket(AF_INET, SOCK_STREAM, 0);	
   
   if (CSocket(&this->socket).connect((struct sockaddr *) &pin, sizeof(pin)) == -1) {	
		perror("connect");
		exit(1);
	}
	
   sqlconn_new(&this->sql_conn, SQL_Client);

   sqlconn_message_open(this->sql_conn, "log2.db3", (void *)this, (SQL_WRITE_MESSAGE)&CSQLClientApp::write_message);
   this->rx_buf_count = CSocket(&this->socket).recv(this->rx_buf, sizeof(this->rx_buf), 0);
   sqlconn_client_message(this->sql_conn, this->rx_buf_count, this->rx_buf, (void *)this, (SQL_NOTIFY_RESULT)&CSQLClientApp::notify_result);
   
   sqlconn_message_exec(this->sql_conn, "BEGIN", (void *)this, (SQL_WRITE_MESSAGE)&CSQLClientApp::write_message);
   this->rx_buf_count = CSocket(&this->socket).recv(this->rx_buf, sizeof(this->rx_buf), 0);
   sqlconn_client_message(this->sql_conn, this->rx_buf_count, this->rx_buf, (void *)this, (SQL_NOTIFY_RESULT)&CSQLClientApp::notify_result);

	while (this->rx_buf_count > 0 && repeat) {
      sqlconn_message_exec(this->sql_conn, CString(&ARRAY(args).data()[2]).string(), (void *)this, (SQL_WRITE_MESSAGE)&CSQLClientApp::write_message);
      this->rx_buf_count = CSocket(&this->socket).recv(this->rx_buf, sizeof(this->rx_buf), 0);
      if (this->rx_buf_count > 0) {
         sqlconn_client_message(this->sql_conn, this->rx_buf_count, this->rx_buf, (void *)this, (SQL_NOTIFY_RESULT)&CSQLClientApp::notify_result);
      }
      repeat--;
   }

   sqlconn_message_exec(this->sql_conn, "END", (void *)this, (SQL_WRITE_MESSAGE)&CSQLClientApp::write_message);
   this->rx_buf_count = CSocket(&this->socket).recv(this->rx_buf, sizeof(this->rx_buf), 0);
   sqlconn_client_message(this->sql_conn, this->rx_buf_count, this->rx_buf, (void *)this, (SQL_NOTIFY_RESULT)&CSQLClientApp::notify_result);
	
   CSocket(&this->socket).close();
   
   delete(&this->socket);
   
   return 0;
}                       

                                                                              
MODULE::END                                                                   
                                                                              
