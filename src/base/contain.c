/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/
#include "environ.h"
#include "contain.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MALLOC_DEBUG TRUE
#if MALLOC_DEBUG

int container_malloc_log_active = 0;
pthread_t container_malloc_log_threadid;

void container_malloc_log(int active) {
    container_malloc_log_threadid = pthread_self();
    printf("(%ld) log activate %d\n", container_malloc_log_threadid, active);
    container_malloc_log_active = active;
}

static void malloc_log(const char *msg) {
   char logmsg[100];

   sprintf(logmsg, "(%ld) %s", pthread_self(), msg);
   if (container_malloc_log_active && container_malloc_log_threadid == pthread_self()) {
      ASSERT(logmsg);
   }
}
#else
void container_malloc_log(int active) {}
#endif

/*------------------------------ Linked Lists ------------------------------*/

Global void LIST_new(TList *this, int itemsize) {
   this->first = NULL;
   this->last = NULL;
   this->itemsize = itemsize;
   this->count = 0;
}/*LIST_new*/

Global void LIST_delete(TList *this) {
   TListItem *nextitem, *item = this->first;

   /* traverse the list, deleting each item in turn */
   while (item) {
      nextitem = item->next;
      LIST_item_remove(this, item);
      item = nextitem;
   }
}/*LIST_delete*/

Global void *LIST_item_add(TList *this, TListItem *item,
                           TListItem *ref, bool front) {
   TListItem *lp;

   if (ref && item == ref) {
      ASSERT("LIST_add_item:reference and item same object");
   }

   if (!item) {
      if (this->itemsize) {
#if MALLOC_DEBUG
         malloc_log("LIST_add_item:malloc");
#endif
         item = malloc(this->itemsize);
         memset(item, 0, this->itemsize);
      }
      else
        return NULL;
   }

   if (ref) {
      /*reference item provided, insert before/after this*/
      if (front) {
         item->next     = ref;
         item->previous = ref->previous;
         ref->previous  = item;

         if (item->previous) {
            item->previous->next = item;
         }
         else {
            this->first = item;
         }
      }
      else {
         item->next     = ref->next;
         item->previous = ref;
         ref->next      = item;

         if (item->next) {
            item->next->previous = item;
         }
         else {
            this->last = item;
         }
      }
   }
   else {
      if (front) {
         lp = this->last;
         if (!this->last)
            this->last = item;
         else {
            while (lp->previous)
              lp = lp->previous;
            lp->previous = item;
         }
         item->previous = NULL;
         item->next     = lp;
         this->first    = item;
      }
      else {
         lp = this->first;
         if (!this->first)
           this->first = item;
         else {
            while (lp->next)
              lp = lp->next;
            lp->next = item;
         }
         item->next     = NULL;
         item->previous = lp;
         this->last     = item;
      }
   }

   this->count++;

   return item;
}/*LIST_item_add*/

Global void LIST_item_remove(TList *this, TListItem *item) {
   if (this->first != this->last) {
      if (item == this->first) {
         this->first = item->next;
         item->next->previous = NULL;
      }
      else {
         item->previous->next = item->next;
         if (item != this->last)
           item->next->previous = item->previous;
         else
           this->last = item->previous;
      }
   }
   else {
      /* Only one item in list, none now */
      this->first = NULL;
      this->last  = NULL;
   }

   if (this->itemsize) {
#if MALLOC_DEBUG
      malloc_log("LIST_item_remove:free");
#endif
     free(item);
   }

   this->count--;
}/*LIST_item_remove*/

Global void *LIST_n_item(TList *this, int n) {
   TListItem *ip = this->first;
   if (this->count == 0)
     return NULL;

   while (n) {
      ip = ip->next;
      if (ip == NULL)
        return NULL;
      n--;
   }
   return ip;
}/*LIST_n_item*/

Global int LIST_item_n(TList *this, TListItem *item) {
   int index = 0;
   TListItem *ip = this->first;

   while (ip) {
      if (ip == item)
        return index;
      ip = ip->next;
      index++;
   }

   return -1;
}/*LIST_item_n*/

Global bool LIST_item_exists(TList *this, TListItem *item) {
   TListItem *ip = this->first;

   while (ip) {
      if (ip == item)
        return TRUE;
      ip = ip->next;
   }
   return FALSE;
}/*LIST_item_exists*/

/*----------------------------- Dynamic Arrays -----------------------------*/

Global void ARRAY_new(TArray *this, int element_size) {
   CLEAR(this);
#if DEBUG
   this->obj_sid = SID_OBJ_SPECIAL;
#endif
   this->element_size = element_size;
}/*ARRAY_new*/

Global void ARRAY_delete(TArray *this) {
#if DEBUG
   if (this->obj_sid == SID_OBJ_DELETED) {
      ASSERT("ARRAY_copy:array object already deleted");
   }
   if (this->obj_sid != SID_OBJ_SPECIAL) {
      ASSERT("ARRAY_copy:bad array object referenced");
   }
   this->obj_sid = SID_OBJ_DELETED;
#endif
   if (this->data) {
#if MALLOC_DEBUG
      malloc_log("ARRAY_delete:free");
#endif
      free(this->data);
      this->data = NULL;
   }
}/*ARRAY_delete*/

Global void ARRAY_copy(TArray *this, TArray *src) {
#if DEBUG
   if (this->obj_sid != SID_OBJ_SPECIAL) {
      ASSERT("ARRAY_copy:bad array object referenced");
   }
   if (this->element_size != src->element_size) {
      ASSERT("ARRAY_copy:element size does not match");
   }
#endif
   ARRAY_used_set(this, src->used);
   memcpy(this->data, src->data, this->used * this->element_size);
}/*ARRAY_copy*/

Global void ARRAY_used_set(TArray *this, int count) {
#if DEBUG
   if (this->obj_sid != SID_OBJ_SPECIAL) {
      ASSERT("ARRAY_used_set:bad array object referenced");
   }
#endif
   if (count > this->allocated) {
      if (!this->data) {
#if MALLOC_DEBUG
         malloc_log("ARRAY_used_set:malloc");
#endif
         this->data = malloc(count * this->element_size);
      }
      else {
#if MALLOC_DEBUG
         malloc_log("ARRAY_add_item:realloc");
#endif
         this->data = realloc(this->data, count * this->element_size);
      }
      this->allocated = count;
   }
   if (count && !this->data) {
      ASSERT("ARRAY_used_set:memory allocation failed");
   }

   this->used = count;
}/*ARRAY_used_set*/

Global void ARRAY_item_remove(TArray *this, void *item) {
   int i;
#if DEBUG
   if (this->obj_sid != SID_OBJ_SPECIAL) {
      ASSERT("ARRAY_used_set:bad array object referenced");
   }
#endif

   for (i = 0; i < this->used; i++) {
      if (memcmp((byte *)this->data + i * this->element_size, item, this->element_size) == 0) {
         memmove((byte *)this->data + i * this->element_size,
                (byte *)this->data + (i + 1) * this->element_size,
                (this->used - i - 1) * this->element_size);
                ARRAY_used_set(this, this->used - 1);
                return;
      }
   }
}/*ARRAY_item_remove*/

Local void ARRAY_sort_swap(TArray *this, TAttributeType *element_type, void *temp, int index_begin, int index_end) {
   (*element_type->convert)(NULL, element_type, element_type, -1, -1, temp, (byte*)this->data + index_begin * this->element_size);
   (*element_type->convert)(NULL, element_type, element_type, -1, -1, (byte *)this->data + index_begin * this->element_size,
                                                                      (byte *)this->data + index_end * this->element_size);
   (*element_type->convert)(NULL, element_type, element_type, -1, -1, (byte *)this->data + index_end * this->element_size, temp);
}/*ARRAY_sort_swap*/

extern TAttributeType __attributetype_op_compare;

Local void ARRAY_sort_quicksort(TArray *this, TAttributeType *element_type, bool descending, void *temp, int beg, int end) {
   bool ok;
   int result;

   if (end > beg + 1) {
      void *piv = (byte *)this->data + (beg * this->element_size);
      int l = beg + 1, r = end;

      while (l < r) {
         (*element_type->convert)(NULL, element_type, element_type, -1, -1, temp, piv);
         ok = (*element_type->convert)((struct tag_CObjPersistent *)&result,
                                       &__attributetype_op_compare, element_type,
                                       -1, -1,
                                       temp, (byte *)this->data + (l * this->element_size));
         if (!ok) {
            /*>>>fallback on string conversion!*/
         }

         if (!descending) {
            result = -result;
         }
         if (result <= 0) {
            l++;
         }
         else {
            ARRAY_sort_swap(this, element_type, temp, l, --r);
         }
      }
      ARRAY_sort_swap(this, element_type, temp, --l, beg);
      ARRAY_sort_quicksort(this, element_type, descending, temp, beg, l);
      ARRAY_sort_quicksort(this, element_type, descending, temp, r, end);
   }
}/*ARRAY_sort_quicksort*/

Global void ARRAY_sort_type(TArray *this, TAttributeType *element_type, bool descending) {
   void *temp = malloc(this->element_size);
   (*element_type->convert)(NULL, element_type, NULL, -1, -1, temp, NULL);

   ARRAY_sort_quicksort(this, element_type, descending, temp, 0, this->used);

   (*element_type->convert)(NULL, NULL, element_type, -1, -1, NULL, temp);
   free(temp);
}/*ARRAY_sort_type*/


/*----------------------------- Dynamic Arrays -----------------------------*/

void VECTOR_new(TVector *this, int element_size, int static_capacity);
void VECTOR_delete(TVector *this);

Global void VECTOR_new(TVector *this, int element_size, int static_capacity) {
   CLEAR(this);
#if DEBUG
   this->obj_sid = SID_OBJ_SPECIAL;
#endif
   this->element_size = element_size;
   this->capacity = static_capacity;
}/*VECTOR_new*/

Global void VECTOR_delete(TVector *this) {
#if DEBUG
   if (this->obj_sid == SID_OBJ_DELETED) {
      ASSERT("VECTOR_copy:array object already deleted");
   }
   if (this->obj_sid != SID_OBJ_SPECIAL) {
      ASSERT("VECTOR_copy:bad array object referenced");
   }
   this->obj_sid = SID_OBJ_DELETED;
#endif
}/*VECTOR_delete*/
