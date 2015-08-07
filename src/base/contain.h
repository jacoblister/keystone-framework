#ifndef I_CONTAINER
#define I_CONTAINER

#include "environ.h"
#include "parameter.h"

/*>>>obj_sid if debug build*/
/*>>>LISTITEM, preprocessor needs to be smarter*/
typedef struct tagLISTITEM {
   struct tagLISTITEM *next, *previous;
} TListItem;

typedef struct tagLIST {
   TListItem *first, *last;            /* macro's depend on this being first */
   int itemsize;
   int count;
} TList;

void LIST_new(TList *this, int itemsize);
void *LIST_item_add(TList *this, TListItem *item,
                    TListItem *ref, bool front);
void *LIST_n_item(TList *this, int n);
int  LIST_item_n(TList *this, TListItem *item);
bool LIST_item_exists(TList *this, TListItem *item);
void LIST_item_remove(TList *this, TListItem *item);
void LIST_delete(TList *this);

/*----------------------------- Dynamic Arrays -----------------------------*/

#if DEBUG
#define ARRAY_DEBUG ulong obj_sid;
#else
#define ARRAY_DEBUG
#endif

typedef struct {
   ulong obj_sid;
   int element_size;
   int allocated, used;
   void *data;
} TArray;

/*>>>find a method of allocating array memory dynamically using OBJ_mem_alloc()*/
void ARRAY_new(TArray *this, int element_size);     /*>>>should pass a reference to native type, not just native type size */
void ARRAY_delete(TArray *this);
void ARRAY_copy(TArray *this, TArray *src);
void ARRAY_used_set(TArray *this, int count);
void ARRAY_item_remove(TArray *this, void *item);
void ARRAY_sort_type(TArray *this, TAttributeType *element_type, bool descending);

/*------------------------------- Bitfields --------------------------------*/

/*>>>variable size bitfields*/
typedef struct {
   byte data[4];
   int count;
} TBitfield;

static inline void BITFIELD_new(TBitfield *this) {}
static inline void BITFIELD_delete(TBitfield *this) {}
static inline int BITFIELD_count(TBitfield *this) {
   return this->count;
}
static inline void BITFIELD_count_set(TBitfield *this, int count) {
   if (count > 48) {
      ASSERT("BITFIELD_count:count > 48");
   }
   this->count = count;
}
static inline void BITFIELD_set(TBitfield *this, int index, bool value) {
   if (value) {
      this->data[index >> 3] |= (1 << (index & 7));
   }
   else {
      this->data[index >> 3] &= ~(1 << (index & 7));
   }
}
static inline bool BITFIELD_get(TBitfield *this, int index) {
   return (this->data[index >> 3] & (1 << (index & 7))) ? TRUE : FALSE;
}

/*----------------------------- Vectors  ------------------------------*/

typedef struct {
   void *array_data;
   int element_size;
   int capacity, count;
   ulong obj_sid;
} TVector;

static inline int VECTOR_assert(TVector *this, int index_begin, int index_end, char *message) {
   if (index_begin >= this->count || index_end >= this->count) {
      ASSERT(message);
   }
   return TRUE;
}

void VECTOR_new(TVector *this, int element_size, int static_capacity);
void VECTOR_delete(TVector *this);
#endif