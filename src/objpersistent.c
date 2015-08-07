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
#include "cstring.c"
#include "objcontainer.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

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

/*! \addtogroup object_trees
 *  <h1>The Title</h1>
 *  \par Dummy Tree
 *  object trees <b>hang</b> down
 *  \code
 *  class CDummy {
 *     void new(void);
 *  }
 *  \endcode
 *  <img src="file:///c:/temp/svgview_linux.png">
 *  @{
 */

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
   void selection_update(void);         /*>>> move into CObjServer*/
   void selection_update_end(void);     /*>>> move into CObjServer*/

   /* attribute get/set wrappers */
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

   /*>>>better method of specifying mask for this*/
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

 /*! @} */

/* XLink */
ENUM:typedef<EXLinkShow> {
   {new},                               /*!< open new window for link */   
   {replace},                           /*!< use current window for link */
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
}/*CObjPersistent::root_set*/

static inline CObjPersistent *CXLink::object_get(void) {
   return CObjPersistent(CObject(this).child_first());
}/*CXLink::object_get*/

ATTRIBUTE:typedef<CXLink>;

/* XPath */

class CObjClient;

ENUM:typedef<EPathOperation> {
   {none}, 
   {exists},                            /*!< address node exists */      
   {count},                             /*!< number if items in selection */
   {not},                               /*!< boolean 'not' of result evaluation */
   {neg},                               /*!< integer negation of result evaluation */
   {op_delete, "delete"},               /*!< perform deletion of selection on action */
   {string},                            /*!< static text, eg, "string('text')" */
   {alias},                             /*!< alias name of selected object class */ 
};

/*>>>should inherit from CSelection */
class CXPath : CSelection {
 private:
   CObjPersistent *root;

   CString path;
   TObjectPtr initial;

   TObjectPtr object;

   int index;                           /*>>>should be a list of variables */
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
//   void selection_set(CSelection *selection);
//   void selection_get(CSelection *selection);
   void resolve(void);
   int get_int(void);
   void get_string(CString *result);

   static inline const char* assignment(void);
   bool assignment_get_string(CString *result);
};

static inline void CXPath::initial_set(TObjectPtr *initial) {
   this->initial = *initial;
}/*CXPath::initial_set*/

static inline void CXPath::root_set(CObjPersistent *root) {
   this->root = root;
}/*CXPath::root_set*/

static inline void CXPath::index_set(int index) {
   this->index = index;
}/*CXPath::index_set*/

static inline const char* CXPath::assignment(void) {
   return this->assignment;
}/*CXPath::assignment*/

ATTRIBUTE:typedef<CXPath>;

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <ctype.h>
#include "cencode.h"
#include "cdecode.h"

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
   /*>>>kludge, no_expand should be defined along with attribute declaration */
   (&ATTRIBUTE:type<CBinData>)->no_expand = TRUE;

   new(&this->header).CString("base64");
   ARRAY(&this->data).new();
}/*CBinData::new*/

void CBinData::CBinData(void) {
}/*CBinData::CBinData*/

void CBinData::delete(void) {
   ARRAY(&this->data).delete();
   delete(&this->header);
}/*CBinData::delete*/

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
         /* for now, just treat contents as header */
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
//          CString(string).set(CString(&bindata->header).string());
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
//   ARRAY<const TAttribute *> attribute;
   int i, count;

//   ARRAY(&attribute).new();
   count = CObjPersistent(this).attribute_list(NULL, TRUE, TRUE, FALSE);

   BITFIELD(&this->attribute_default).count_set(count);
   for (i = 0; i < count; i++) {
      BITFIELD(&this->attribute_default).set(i, FALSE);
   }
//   ARRAY(&attribute).delete();
}/*CObjPersistent::new*/

void CObjPersistent::CObjPersistent(void) {
}/*CObjPersistent::CObjPersistent*/

void CObjPersistent::delete(void) {
}/*CObjPersistent::delete*/

CObjPersistent *CObjPersistent::child_first(void) {
   return CObjPersistent(this).child_tree_next(this);
}/*CObjPersistent::child_first*/

CObjPersistent *CObjPersistent::child_last(void) {
   return NULL;
}/*CObjPersistent::child_last*/

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
}/*CObjPersistent::child_next*/

CObjPersistent *CObjPersistent::child_previous(CObjPersistent *child) {
   return NULL;
}/*CObjPersistent::child_previous*/

CObjPersistent *CObjPersistent::child_tree_first(void) {
   return CObjPersistent(this).child_tree_next(this);
}/*CObjPersistent::child_tree_first*/

CObjPersistent *CObjPersistent::child_tree_last(void) {
   return NULL;
}/*CObjPersistent::child_tree_last*/

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
}/*CObjPersistent::child_tree_next*/

CObjPersistent *CObjPersistent::child_tree_previous(CObjPersistent *child) {
   return NULL;
}/*CObjPersistent::child_tree_previous*/

int state_loading;

bool CObjPersistent::state_loading(void) {
   /*>>>hack! not thread safe*/
   return CObjPersistent(this).state_xml_loading() || (state_loading != 0);
}/*CObjPersistent::state_loading*/

CObjPersistent *CObjPersistent::copy_fast(CObjPersistent *dest, bool copy_child, bool copy_fast) {
   /*>>>need better attribute get/set/copy*/
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

   /* copy object attributes */
   ARRAY(&attr_list).new();
   CObjPersistent(this).attribute_list(&attr_list, TRUE, TRUE, FALSE);
   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      attribute = ARRAY(&attr_list).data()[i];
      attr_src  = (void *)(((byte *)this) + attribute->offset);
      attr_dest = (void *)(((byte *)object_copy) + attribute->offset);

      switch (attribute->type) {
      case PT_AttributeArray:
         /*>>>arrays for set types other than CString*/
         array_src = (ARRAY<byte> *)attr_src;
         array_dest = (ARRAY<byte> *)attr_dest;

         ARRAY(array_dest).used_set(ARRAY(array_src).count());
         for (j = 0; j < ARRAY(array_src).count(); j++) {
            attr_src = (void *)&ARRAY(array_src).data()[attribute->data_type->size * j];
            attr_dest = (void *)&ARRAY(array_dest).data()[attribute->data_type->size * j];

            /* set the destination attribute */
            result = (*attribute->data_type->convert)(object_copy, attribute->data_type, attribute->data_type,
                                                      ATTR_INDEX_VALUE, ATTR_INDEX_VALUE,
                                                      attr_dest, attr_src);
            if (!result) {
               /* conversion has failed, copy data directly */
               /*>>>copy data through string */
               memcpy(attr_dest, attr_src, attribute->data_type->size);
            }
         }
         break;
      case PT_AttributeEnum:
         memcpy(attr_dest, attr_src, sizeof(int));
         break;
      case PT_ElementObject:
         /* copy element data */
         CObjPersistent(attr_src).copy_fast(CObjPersistent(attr_dest), FALSE, copy_fast);

          /*copy element children*/
         child = CObjPersistent(CObject(attr_src).child_first());
         while (child) {
            child_copy = CObjPersistent(child).copy_fast(NULL, TRUE, copy_fast);
            CObject(attr_dest).child_add(CObject(child_copy));
            child = CObjPersistent(CObject(attr_src).child_next(CObject(child)));
         }
         break;
      default:
         /* set the destination attribute */
         result = (*attribute->data_type->convert)(object_copy, attribute->data_type, attribute->data_type,
                                                   ATTR_INDEX_VALUE, ATTR_INDEX_VALUE,
                                                   attr_dest, attr_src);
         if (!result) {
            /* conversion has failed, copy data through string */
            result = CObjPersistent(this).attribute_default_get(attribute);
            CObjPersistent(object_copy).attribute_default_set(attribute, result);

            new(&data).CString(NULL);
            CObjPersistent(this).attribute_get_string(attribute, &data);
            CObjPersistent(object_copy).attribute_set_string(attribute, &data);
            delete(&data);
         }
         break;
      }/*switch*/

      /* set attribute default status */
      if (copy_fast) {
         BITFIELD(&object_copy->attribute_default).set(attribute->index, BITFIELD(&this->attribute_default).get(attribute->index));
      }
      else {
         result = CObjPersistent(this).attribute_default_get(attribute);
         CObjPersistent(object_copy).attribute_default_set(attribute, result);
      }
   }
   ARRAY(&attr_list).delete();

   /*>>>why ignore no children flag?*/
   if (copy_child /*&& !CObject(this).obj_class()->option*/) {
//   if (copy_child /*&& !CObject(this).obj_class()->option*/) {
      /* copy objects children */
      child = CObjPersistent(CObject(this).child_first());
      while (child) {
         child_copy = CObjPersistent(child).copy_fast(NULL, TRUE, copy_fast);
         CObject(object_copy).child_add(CObject(child_copy));
         /*>>>perform refresh elsewhere*/
         if (!copy_fast) {
            CObjPersistent(child_copy).attribute_refresh(FALSE);
         }
         child = CObjPersistent(CObject(this).child_next(CObject(child)));
      }
   }
   
   return object_copy;
}/*CObjPersistent::copy_fast*/

CObjPersistent *CObjPersistent::copy(bool copy_child) {
   return CObjPersistent(this).copy_fast(NULL, copy_child, FALSE);
}/*CObjPersistent::copy*/

CObjPersistent *CObjPersistent::copy_clone(CObjPersistent *dest, bool copy_child) {
   CObjPersistent *result;

   state_loading++;
   result = CObjPersistent(this).copy_fast(dest, copy_child, FALSE);
   state_loading--;   
   
   return result;
}/*CObjPersistent::copy_clone*/

void CObjPersistent::attribute_update(const TAttribute *attribute) {
   /*>>>store list of changing attributes*/

   CObjPersistent(this).notify_attribute_update(NULL, TRUE);
}/*CObjPersistent::attribute_update*/

void CObjPersistent::attribute_update_end(void) {
   CObjPersistent(this).notify_attribute_update(NULL, FALSE);
}/*CObjPersistent::attribute_update_end*/

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
}/*CObjPersistent::attribute_refresh*/

int CObjPersistent::attribute_array_count(const TAttribute *attribute) {
   ARRAY<byte> *array;

   if (attribute->type != PT_AttributeArray) {
      throw(CObject(this), EXCEPTION<CObjPersistent,AttributeTypeMismatch>, "Attribute Type Mismatch");
   }

   array = (ARRAY<byte> *)(((byte *)this) + attribute->offset);

   return ARRAY(array).count();
}/*CObjPersistent::attribute_array_count*/

bool CObjPersistent::attribute_default_get(const TAttribute *attribute) {
   int i = CObjPersistent(this).attribute_index_find(attribute);

   return BITFIELD(&this->attribute_default).get(i);
}/*CObjPersistent::attribute_default_get*/

void CObjPersistent::attribute_default_set(const TAttribute *attribute, bool is_default) {
   int i = CObjPersistent(this).attribute_index_find(attribute);
   bool set_default = TRUE;

   BITFIELD(&this->attribute_default).set(i, is_default);

   if (is_default) {
      /*handle inherited parameters*/
      if (attribute->options & PO_INHERIT) {
         set_default = !CObjPersistent(this).attribute_set_inherit(attribute);
      }
      if (attribute && attribute->method && set_default) {
         (*attribute->method)(this, NULL, -1);
      }
   }
}/*CObjPersistent::attribute_default_set*/

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

   /* refresh children */
   child = CObjPersistent(CObject(object).child_first());
   while (child) {
      CObjPersistent(this).attribute_default_refresh_all(child, attribute);
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }
}/*CObjPersistent::attribute_default_refresh_all*/

void CObjPersistent::attribute_string_process(const TAttribute *attribute, CString *data) {
   int index;
   int level = 0;
   int delim_count = 0;
   int length;

   if (attribute->options & PO_ELEMENT_STRIPSPACES) {
      /* strip all spaces inside brackets, and reduce whitespace to single space */
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
      /*>>>dirty hack, can do much better */
      index = 0;
      while (index < CString(data).length()) {
         if (CString(data).string()[index] == ' ' || CString(data).string()[index] == ',') {
            delim_count++;
            CString(data).string()[index] = delim_count & 1 ? ',' : ' ';
         }
         index++;
      }
   }

}/*CObjPersistent::attribute_string_process*/

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
            /* set data to current value */
            /* Create temporary instance of this type, copy and set, the release */
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
            /* copy data directly */
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

         /* set data to current value */
         memcpy(data_set, (void *)&ARRAY(array).data()[attribute->data_type->size * attribute_element], attribute->data_type->size);

         result =
            (*attribute->data_type->convert)(this, attribute->data_type, type,
                                             attribute_index, ATTR_INDEX_VALUE,
                                             data_set,
                                             data);
         if (!result && attribute->data_type == type) {
            /* copy data directly */
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
         /*>>>shouldn't destroy input string */
         if (type == &ATTRIBUTE:type<CString>) {
             /* try to convert entire array first */
             result =
                (*attribute->data_type->convert)(this,
                                                 attribute->data_type->array, &ATTRIBUTE:type<CString>,
                                                 attribute_index, ATTR_INDEX_VALUE,
                                                 (void *)array,
                                                 (void *)data);
            if (!result) {
               new(&element).CString(NULL);
               ARRAY(array).used_set(0);

               /*strip leading spaces*/
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
   //>>>return error
#if 0
   if (!result) {
      throw(CObject(this), EXCEPTION<CObjPersistent,BadConversion>, "Bad Set Conversion");
   }
#endif

   if (attribute->options & PO_INHERIT) {
      /* update children too */
      CObjPersistent(this).attribute_default_refresh_all(this, attribute);
   }
}/*CObjPersistent::attribute_set*/

bool CObjPersistent::attribute_set_inherit(const TAttribute *attribute) {
   CObjPersistent *parent;
   const TAttribute *attr_parent;

   parent = CObjPersistent(CObject(this).parent());
   attr_parent = NULL;
   while (parent) {
      attr_parent = CObjPersistent(parent).attribute_find(attribute->name);
      if (attr_parent && !CObjPersistent(parent).attribute_default_get(attr_parent)) {
        /*>>>should inherit from parent regardless of parent attribute default*/
         CObjPersistent(this).attribute_set(attribute, -1, -1, attr_parent->data_type, (void *)(((byte *)parent) + attr_parent->offset));
         return TRUE;
      }
      parent = CObjPersistent(CObject(parent).parent());
   }

   return FALSE;
}/*CObjPersistent::attribute_set_inherit*/

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
         /* copy data directly */
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
            /*>>>kludge, return 0 if read off end of array */
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
            /* copy data directly */
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
   /*>>>cthrow*/
   if (!result) {
      throw(CObject(this), EXCEPTION<CObjPersistent,BadConversion>, "Bad Get Conversion");
   }
}/*CObjPersistent::attribute_get*/

void CObjPersistent::attribute_set_string(const TAttribute *attribute, const CString *string) {
   CObjPersistent(this).attribute_set(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<CString>, (const void *)string);
}/*CObjPersistent::attribute_set_string*/

void CObjPersistent::attribute_set_string_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, const CString *string) {
   CObjPersistent(this).attribute_set(attribute, attribute_element, attribute_index, &ATTRIBUTE:type<CString>, (const void *)string);
}/*CObjPersistent::attribute_set_string_element_index*/

void CObjPersistent::attribute_get_string(const TAttribute *attribute, CString *string) {
   CString(string).clear();
   CObjPersistent(this).attribute_get(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<CString>, (void *)string);
}/*CObjPersistent::attribute_get_string*/

void CObjPersistent::attribute_get_string_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, CString *string) {
   CString(string).clear();
   CObjPersistent(this).attribute_get(attribute, attribute_element, attribute_index, &ATTRIBUTE:type<CString>, (void *)string);
}/*CObjPersistent::attribute_get_string_element_index*/

void CObjPersistent::attribute_set_text(const TAttribute *attribute, const char *text) {
   CString string;

   new(&string).CString(text);
   CObjPersistent(this).attribute_set_string(attribute, &string);
   delete(&string);
}/*CObjPersistent::attribute_set_text*/

void CObjPersistent::attribute_set_text_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, const char *text) {
   CString string;

   new(&string).CString(text);
   CObjPersistent(this).attribute_set_string_element_index(attribute, attribute_element, attribute_index, &string);
   delete(&string);
}/*CObjPersistent::attribute_set_text_element_index*/

int CObjPersistent::attribute_get_int(const TAttribute *attribute) {
   int value;
   CObjPersistent(this).attribute_get(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<int>, (void *)&value);
   return value;
}/*CObjPersistent::attribute_get_int*/

int CObjPersistent::attribute_get_int_element_index(const TAttribute *attribute, int attribute_element, int attribute_index) {
   int value;
   CObjPersistent(this).attribute_get(attribute, attribute_element, attribute_index, &ATTRIBUTE:type<int>, (void *)&value);
   return value;
}/*CObjPersistent::attribute_get_int_element_index*/

float CObjPersistent::attribute_get_float(const TAttribute *attribute) {
   float value;
   CObjPersistent(this).attribute_get(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<float>, (void *)&value);
   return value;
}/*CObjPersistent::attribute_get_float*/

double CObjPersistent::attribute_get_double(const TAttribute *attribute) {
   double value;
   CObjPersistent(this).attribute_get(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<double>, (void *)&value);
   return value;
}/*CObjPersistent::attribute_get_double*/

void CObjPersistent::attribute_set_int(const TAttribute *attribute, int value) {
   CObjPersistent(this).attribute_set(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<int>, (void *)&value);
}/*CObjPersistent::attribute_set_int*/

void CObjPersistent::attribute_set_int_element_index(const TAttribute *attribute, int attribute_element, int attribute_index, int value) {
   CObjPersistent(this).attribute_set(attribute, attribute_element, attribute_index, &ATTRIBUTE:type<int>, (void *)&value);
}/*CObjPersistent::attribute_set_int_element_index*/

void CObjPersistent::attribute_set_float(const TAttribute *attribute, float value) {
   CObjPersistent(this).attribute_set(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<float>, (void *)&value);
}/*CObjPersistent::attribute_set_float*/

void CObjPersistent::attribute_set_double(const TAttribute *attribute, double value) {
   CObjPersistent(this).attribute_set(attribute, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<double>, (void *)&value);
}/*CObjPersistent::attribute_set_double*/

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
}/*CObjPersistent::attribute_list*/

int CObjPersistent::attribute_index_find(const TAttribute *attribute) {
   int i = 0;
   const CObjClass *obj_class, *obj_class_current;
   const TParameter **parameter;

   if (attribute->index > 0) {
       /*>>> used cached index after first lookup*/
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
}/*CObjPersistent::attribute_index_find*/

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
}/*CObjPersistent::attribute_find*/

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

   /* second pass, try to match element name, no namespace */
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
}/*CObjPersistent::element_find*/

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
}/*CObjPersistent::data_find*/

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
}/*CObjPersistent::child_alias_sibling_index*/

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
}/*CObjPersistent::child_alias_sibling_index_find*/

CObjPersistent *CObjPersistent::child_alias_sibling_match(const char *alias, const char *attr_name, const char *attr_value, int *index_result) {
   CObjPersistent *result = NULL;
   const TAttribute *attribute = NULL;
   CString value;
   
   *index_result = 0;

   result = CObjPersistent(CObject(this).child_first());
   if (result) {
      attribute = CObjPersistent(result).attribute_find(attr_name); /*>>>should really do this for each child */
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
}/*CObjPersistent::child_alias_sibling_match*/

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
}/*CObjPersistent::child_element_class_find*/

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

      /*>>> quick hack get root object */
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
}/*CXLink::new*/

void CXLink::CXLink(CObjPersistent *root, CObjPersistent *object) {
}/*CXLink::CXLink*/

void CXLink::delete(void) {
   delete(&this->value);
}/*CXLink::delete*/

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
}/*CXLink::object_find*/

CObjPersistent *CXLink::object_resolve(void) {
//   CObjPersistent *object;
   const char *cp;

//   object = CObjPersistent(CObject(this).child_first());
   if (!this->object) {
      this->object = NULL;
      cp = CString(&this->value).strchr('#');
      if (cp && this->root) {
         this->object = CXLink(this).object_find(this->root, cp + 1);
      }
   }

   return this->object;
}/*CXLink::object_resolve*/

void CXLink::link_set(const char *link) {
    /* strip full path >>>temp hack*/
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
}/*CXLink::link_set*/

void CXLink::link_get(CString *link) {
   CString(link).printf(CString(&this->value).string());
}/*CXLink::link_get*/

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
}/*CXLink::link_get_file*/

bool CXLink::link_get_path(CString *link) {
   return FALSE;
}/*CXLink::link_get_path*/

void CXPath::new(void) {
   class:base.new();

   new(&this->path).CString(NULL);
}/*CXPath::new*/

void CXPath::CXPath(CObjPersistent *initial, CObjPersistent *root) {
   this->initial.object = initial;
   this->root = root;
   this->object.attr.element = ATTR_ELEMENT_VALUE;
   this->object.attr.index   = ATTR_INDEX_VALUE;

   CSelection(this).CSelection(TRUE);
}/*CXPath::CXPath*/

void CXPath::delete(void) {
   delete(&this->path);

   class:base.delete();
}/*CXPath::delete*/

void CXPath::initial_set_path(const char *path) {
   CString old_path;

   new(&old_path).CString(CString(&this->path).string());
   CString(&this->path).set(path);
   CXPath(this).resolve();
   this->initial = this->object;

   CString(&this->path).set(CString(&old_path).string());
   delete(&old_path);
}/*CXPath::initial_set_path*/

void CXPath::set(CObjPersistent *object, const TAttribute *attribute, int index) {
   this->object.object = object;
   this->object.attr.attribute = attribute;
   this->object.attr.index = index;
}/*CXPath::set*/

bool CXPath::match(CObjPersistent *object, const TAttribute *attribute, int index) {
   return this->object.object == object && this->object.attr.attribute == attribute && this->object.attr.index == index;
}/*CXPath::match*/

void CXPath::path_set(const char *path) {
   CString(&this->path).set(path);
   CXPath(this).resolve();
}/*CXPath::path_set*/

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
      /* check for assignment */
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
         /* kludge, object only attribute type should be generated by class */
         this->object.attr.attribute = &__parameter_CGText;
      }
      else {
         /* set selected attribute to objects character data */
         this->object.attr.attribute = CObjPersistent(this->object.object).attribute_find(NULL);
      }
   }

   delete(&section);
   delete(&temp);
}/*CXPath::resolve*/

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
}/*CXPath::path_get*/

int CXPath::get_int(void) {
   int value;
   CString string;
   CString assignment;
   bool result;
   ARRAY<byte> *array;
   
   if (this->operation == EPathOperation.exists) {
      /* not ideal should return TRUE if object exists, but attribute was not addressed */
      return this->object.attr.attribute ? 1 : 0;
   }

   if (!this->object.attr.attribute && this->operation != EPathOperation.count) {
      /*>>>might be better to throw an exception*/
      return (this->operation == EPathOperation.not);
   }

   if (this->assignment) {
      /*>>>resolve assignment, don't just read directly*/
      new(&string).CString(NULL);
      new(&assignment).CString(NULL);
      CObjPersistent(this->object.object).attribute_get(this->object.attr.attribute,
                                                        this->object.attr.element,
                                                        this->object.attr.index,
                                                        &ATTRIBUTE:type<CString>,
                                                        (void *)&string);
      CXPath(this).assignment_get_string(&assignment);                                                        
      result = CString(&string).match(&assignment);
//      result = CString(&string).length() == this->assignment_end - this->assignment + 1;
//      if (result) {
//         result = (CString(&string).strncmp(this->assignment, (this->assignment_end - this->assignment + 1)) == 0);
//      }
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
//   else if (this->attribute->type == PT_AttributeArray) {
//     /* unsupported get of array type, return dummy result */
//     value = -1;
//   }
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
}/*CXPath::get_int*/

void CXPath::get_string(CString *result) {
   const char *str_start, *str_end, *cp;

   switch (this->operation) {
   case EPathOperation.none:
      if (!this->object.attr.attribute && this->operation != EPathOperation.count) {
         /*>>>might be better to throw an exception*/
//         CString(result).set("(unbound!!!)");
         CString(result).clear();
         return;
      }

      CObjPersistent(this->object.object).attribute_get_string_element_index(this->object.attr.attribute,
                                                                             this->object.attr.element,
                                                                             this->object.attr.index,
                                                                             result);
      break;
   case EPathOperation.string:
      /*>>>just static strings for now*/
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
//      CString(result).set(CObjClass_alias_short(CObject(this->object.object).obj_class()));
      CString(result).set(CObjClass_displayname(CObject(this->object.object).obj_class()));
      break;
   default:
      CString(result).set("???");
      return;
      break;
   }
}/*CXPath::get_string*/

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
}/*CXPath::assignment_get_string*/


/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
