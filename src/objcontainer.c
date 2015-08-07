/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework_base.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

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
}/*CSelection::count*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CSelection::new(void) {
   ARRAY(&this->selection).new();
}/*CSelection::new*/

void CSelection::CSelection(bool multi) {
   this->multi = multi;
}/*CSelection::CSelection*/

void CSelection::delete(void) {
   ARRAY(&this->selection).delete();
}/*CSelection::delete*/

ARRAY<TObjectPtr> *CSelection::selection(void) {
   return &this->selection;
}/*CSelection::selection*/

void CSelection::clear(void) {
   int i;
   TObjectPtr *item;

   for (i = 0; i < ARRAY(&this->selection).count(); i++) {
      item = &ARRAY(&this->selection).data()[i];
   }

   ARRAY(&this->selection).used_set(0);
}/*CSelection::clear*/

void CSelection::add(CObjPersistent *object, TAttribute *attribute) {
   TObjectPtr item;

   if (CSelection(this).selected(object, attribute)) {
       /* Don't reselect item if already selected */
       return;
   }

   CLEAR(&item);

   item.object    = object;
   item.attr.attribute = attribute;
   ARRAY(&this->selection).item_add(item);
}/*CSelection::add*/

void CSelection::remove(CObjPersistent *object, TAttribute *attribute) {
   TObjectPtr item;

   CLEAR(&item);
   item.object    = object;
   item.attr.attribute = attribute;
   ARRAY(&this->selection).item_remove(item);
}/*CSelection::remove*/

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
}/*CSelection::selected*/

void CSelection::delete_selected(void) {
   TObjectPtr *item;
   int i;

   for (i = 0; i < ARRAY(&this->selection).count(); i++) {
      item = &ARRAY(&this->selection).data()[i];
      delete(item->object);
   }
   CSelection(this).clear();
}/*CSelection::delete_selected*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

