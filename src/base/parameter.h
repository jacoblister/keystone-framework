#ifndef I_PARAMETER
#define I_PARAMETER

#include "environ.h"

struct tag_CObject;
struct tag_CObjPersistent;
struct tag_CObjClass;
struct ATTRIBUTETYPE;

typedef bool (TATTRIBUTETYPE_CONVERT)(struct tag_CObjPersistent *object,
                                      const struct ATTRIBUTETYPE *dest_type,
                                      const struct ATTRIBUTETYPE *src_type,
                                      int dest_index, int src_index,
                                      void *dest, const void *src);

typedef bool (TATTRIBUTEMETHOD)(struct tag_CObjPersistent *object, void *data, int attribute_index);

typedef struct ATTRIBUTETYPE {
   int size;
   TATTRIBUTETYPE_CONVERT *convert;
   struct ATTRIBUTETYPE *array; 
   bool no_expand;
} TAttributeType;

typedef enum {
   PT_Attribute,
   PT_AttributeEnum,
   PT_AttributeArray,
   PT_Element,
   PT_ElementObject,
   PT_ElementObjectList,
   PT_Data,
   PT_Menu,
   PT_Void,
} EParameterType;

typedef enum {
   PO_INHERIT      = 1,
   PO_HASDEFAULT   = 2,
   PO_NODELIMSTRIP = 4,
   PO_ELEMENT_STRIPSPACES = 8,
   PO_ELEMENT_PAIR        = 16,
} EParameterOption;

typedef struct {
   struct tag_CObjClass *obj_class;
   EParameterType type;
   const TAttributeType *data_type;
   int enum_count;
   const char **enum_name;
   const char *name;
   int offset;
   TATTRIBUTEMETHOD *method;
   int options;
   const char *delim;
   const char *path;
   
   int index;                           /*>>>for quick reference*/
} TParameter;

typedef TParameter TAttribute;

#endif /*I_PARAMETER*/
