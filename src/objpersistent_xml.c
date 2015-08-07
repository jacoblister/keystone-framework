/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "objpersistent.c"
#include "objcontainer.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CObjXMLComment : CObjPersistent {
 private:
   void new(void);
   void delete(void);
 public:
   void CObjXMLComment(const char *text);
   ATTRIBUTE<CString comment>;
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <ctype.h>
#include "file.c"
#define XML_STATIC
#include "../expat/src/expat.h"
#undef XML_STATIC

/* >>> best remove this class and add method to XML state load/store class */
class CStringXML : CString {
 private:
   void delete(void);
 public:
   void CStringXML(const char *str);
   void expand(CString *src, bool newline);
   void contract(void);
};

void CStringXML::CStringXML(const char *str) {
   CString(this).CString(str);
}/*CStringXML::CStringXML*/

void CStringXML::delete(void) {
   class:base.delete();
}/*CStringXML::delete*/

void CStringXML::expand(CString *src, bool newline) {
   /*>>>incomplete implementation of named character expansion.  Should
     handle all XML named characters */
   int index = 0;
   int length;
   char insert[10];
   unsigned short *wcp;

   CString(this).clear();
   CString(this).encoding_set(EStringEncoding.ASCII);
   CString(src).encoding_set(EStringEncoding.UTF16);

   length = CString(src).length();
   wcp = CString(src).wstring();

   while (index < length) {
      switch (wcp[index]) {
      case 0x0A: /*>>>kludge*/
         if (newline) {
            CString(this).appendn("&#10;", 5);
            break;
         }
         goto def;
      case 0x0D: /*>>>kludge*/
         CString(this).appendn("&#13;", 5);
         break;
      case 0x22: /*>>>kludge*/
         CString(this).appendn("&quot;", 6);
         break;
      case '&':
         CString(this).appendn("&amp;", 5);
         break;
      case '<':
         CString(this).appendn("&lt;", 4);
         break;
      case '>':
         CString(this).appendn("&gt;", 4);
         break;
      def:
      default:
      if (wcp[index] >= 128) {
         sprintf(insert, "&#%d;", wcp[index]);
          CString(this).appendn(insert, strlen(insert));
         break;
      }
      else {
         insert[0] = (char) wcp[index];
         CString(this).appendn(&insert[0], 1);
      }
         break;
      }
      index++;
   }
}/*CStringXML::expand*/

void CObjXMLComment::new(void) {
    new(&this->comment).CString(NULL);
    class:base.new();
}/*CObjXMLComment::new*/

void CObjXMLComment::CObjXMLComment(const char *text) {
    CString(&this->comment).set(text);
}/*CObjXMLComment::CObjXMLComment*/

void CObjXMLComment::delete(void) {
    delete(&this->comment);
    class:base.delete();
}/*CObjXMLComment::delete*/

typedef struct {
   enum {None, Object, Element, ElementObject} type;
   CObjPersistent *object;
   const TParameter *parameter;
   CStringXML *char_data;
} TXMLLoadObject;
STACK:typedef<TXMLLoadObject>;

/* XML State Loader */
class CXMLStateLoader : CObject {
 private:
   const char *default_namespace;
   XML_Parser parser;
   CFile *file;
   CObjPersistent *obj_root;
   TXMLLoadObject load_object;
   bool end_element; /*>>>kludge, used to detect text after children of element (should not occur) */
   STACK<TXMLLoadObject> load_object_stack;
//   CStringXML char_data;
   bool comments;
 public:
   void new(void);
   void CXMLStateLoader(void);
   void delete(void);
   void load_stream(CObjPersistent **obj_root, CIOObject *stream, const char *default_namespace, bool comments);
   EXCEPTION<LoadError>;
};

Local void CXMLStateLoader__xml_startElement(void *userData, const char *name,
                                  const char **atts);
Local void CXMLStateLoader__xml_endElement(void *userData, const char *name);
Local void CXMLStateLoader__xml_characterData(void *userData, const char *s, int len);
Local void CXMLStateLoader__xml_commentHandler(void *userData, const XML_Char *data);

void CXMLStateLoader::new(void) {
//   new(&this->char_data).CStringXML(NULL);
//   CString(&this->char_data).encoding_set(EStringEncoding.UTF16);
}/*CXMLStateLoader::new*/

void CXMLStateLoader::CXMLStateLoader(void) {
   STACK(&this->load_object_stack).new();
}/*CXMLStateLoader::CXMLStateLoader*/

void CXMLStateLoader::delete(void) {
   STACK(&this->load_object_stack).delete();
//   delete(&this->char_data);
}/*CXMLStateLoader::delete*/

void CXMLStateLoader::load_stream(CObjPersistent **obj_root, CIOObject *stream, const char *default_namespace, bool comments) {
   /*>>>streaming read not working properly*/
   ARRAY<byte> buf;                     /*>>>use thread buffer*/
   bool done;
   static char load_error[80];

   this->default_namespace = default_namespace;
   this->comments    = comments;
   this->obj_root    = *obj_root;
   this->parser      = XML_ParserCreate(NULL);
//   this->parser      = XML_ParserCreateNS(NULL, '#');

   ARRAY(&buf).new();
   ARRAY(&buf).used_set(1000);
   XML_SetUserData(this->parser, this);
   XML_SetElementHandler(this->parser, CXMLStateLoader__xml_startElement, CXMLStateLoader__xml_endElement);
   XML_SetCharacterDataHandler(this->parser, CXMLStateLoader__xml_characterData);
   XML_SetCommentHandler(this->parser, CXMLStateLoader__xml_commentHandler);
//   XML_SetParamEntityParsing(this->parser, XML_PARAM_ENTITY_PARSING_ALWAYS);
   do {
      int len;
      len = CIOObject(stream).read_data(&buf, ARRAY(&buf).count());
      done = CIOObject(stream).eof();
      if (!XML_Parse(this->parser, (char *)ARRAY(&buf).data(), len, done)) {
         XML_ParserFree(this->parser);
         sprintf(load_error,
                 "%s at line\n",// %d\n",
                 XML_ErrorString(XML_GetErrorCode(this->parser))
);//                 XML_GetCurrentLineNumber(this->parser));
         throw(CObject(this), EXCEPTION<CXMLStateLoader,LoadError>, load_error);
         /*>>>kludge, just make sure this dosn't return*/
         exit(0);
      }
   } while (!done);
   XML_ParserFree(this->parser);

   ARRAY(&buf).delete();

   *obj_root = this->obj_root;
}/*CXMLStateLoader::load_stream*/

/*>>>exceptions!*/
Local void CXMLStateLoader__xml_startElement(void *userData, const char *name,
                                             const char **atts) {
   CXMLStateLoader *this = CXMLStateLoader(userData);
   TXMLLoadObject *parent;
   const CObjClass *obj_class;
   const TAttribute *attribute;
   CStringXML value;
   int i;

   parent = STACK(&this->load_object_stack).count() ?
      &STACK(&this->load_object_stack).data()[STACK(&this->load_object_stack).count() - 1]
      : NULL;

   if (parent && parent->type == Element) {
      ASSERT("CXMLStateLoader__xml_startElement: element has unsupported subelements");
   }

   new(&value).CStringXML(NULL);
   this->load_object.parameter = NULL;
   this->load_object.char_data = new.CStringXML(NULL);
   CString(this->load_object.char_data).encoding_set(EStringEncoding.UTF16);

   if (parent && (parent->type != None) && (attribute = CObjPersistent(parent->object).element_find(name)) != NULL) {
      switch (attribute->type) {
      case PT_Element:
         this->load_object.type = Element;
         this->load_object.object = parent->object;
         this->load_object.parameter = attribute;
         CObjPersistent(parent->object).attribute_default_set(attribute, FALSE);
         break;
      case PT_ElementObject:
         this->load_object.type = ElementObject;
         this->load_object.object = CObjPersistent(&((byte *)parent->object)[attribute->offset]);
         CObjPersistent(parent->object).attribute_default_set(attribute, FALSE);
         break;
      default:
         break;
      }
   }
   else {
      if (!strchr(name, ':') && this->default_namespace) {
         CString(&value).printf("%s:%s", this->default_namespace, name);
      }
      else {
         CString(&value).printf("%s", name);
      }

      obj_class = CFramework(&framework).obj_class_find(CString(&value).string());
      if (obj_class && !CObjClass_is_derived(&class(CObjPersistent), obj_class)) {
         ASSERT("CXMLStateLoader__xml_startElement: non persistent class specified");
      }

      /*>>>check root object class matches stored class*/
      if (!parent && this->obj_root) {
         this->load_object.type = Object;
         this->load_object.object = this->obj_root;
      }
      else {
         if (obj_class) {
            this->load_object.type = Object;
            this->load_object.object =
               CObjPersistent(new.class(obj_class));
            CObjPersistent(this->load_object.object).CObjPersistent();
            for (i = 0; i < BITFIELD(&this->load_object.object->attribute_default).count(); i++) {
               BITFIELD(&this->load_object.object->attribute_default).set(i, TRUE);
            }
            this->load_object.parameter = CObjPersistent(this->load_object.object).data_find();
            CObject(parent->object).child_add(CObject(this->load_object.object));
         }
         else {
            this->load_object.type = None;
            this->load_object.object = NULL;
            this->load_object.parameter = NULL;
            CString(&value).printf("CXMLStateLoader__xml_startElement: unhandled element <%s/>", name);
            WARN(CString(&value).string());
         }
      }
   }

   if (this->load_object.object) {
      CObjPersistent(this->load_object.object).notify_all_update(TRUE);

      /* set attributes */
      while (atts[0]) {
         /* Object attribute */
         attribute = CObjPersistent(this->load_object.object).attribute_find(atts[0]);
         if (attribute) {
//            switch (attribute->type) {
//             case AT_Attribute:
//             case AT_Array:
               /*>>>search for OPERS_XML_STR_VALUE attribute rather than
                * assuming it is the first one*/
               CString(&value).set(atts[1]);
               CObjPersistent(this->load_object.object).attribute_default_set(attribute, FALSE);
               CObjPersistent(this->load_object.object).attribute_set_string(attribute, CString(&value));
               CObjPersistent(this->load_object.object).attribute_update(attribute);
//               break;
//             case AT_Object:
//               if (this->obj_elementobject_current) {
//                  ASSERT("CXMLStateLoader__xml_startElement: nested object components, unsupported");
//               }
//               this->obj_elementobject_current = CObjPersistent(&((byte *)this->load_object.object)[attribute->offset]);
//               break;
//             default:
//               break;
//            }
         }
         else {
            CString(&value).printf("CXMLStateLoader__xml_startElement: unhandled attribute <%s %s=\"%s\"/>", name, atts[0], atts[1]);
            WARN(CString(&value).string());
//         WARN("CXMLStateLoader__xml_startElement: unhandled attribute");
         }
      atts += 2;
      }

      CObjPersistent(this->load_object.object).attribute_update_end();
   }

//   CString(&this->char_data).set(NULL);
//   CString(&this->char_data).encoding_set(EStringEncoding.UTF16);

   delete(&value);

   STACK(&this->load_object_stack).push(this->load_object);
   this->end_element = FALSE;
}/*CXMLStateLoader__xml_startElement*/

Local void CXMLStateLoader__xml_endElement(void *userData, const char *name) {
   CXMLStateLoader *this = CXMLStateLoader(userData);
   unsigned short uni_char;

   this->load_object = STACK(&this->load_object_stack).pop();
   if (this->load_object.type != None) {
      if (this->load_object.parameter) {
         /* Strip trailing spaces >>>Unicode version kludge */
         if (CString(this->load_object.char_data).length()) {
            uni_char = CString(this->load_object.char_data).wstring()[CString(this->load_object.char_data).length() - 1];
            while (uni_char < 0x100 && isspace(uni_char)) {
               CString(this->load_object.char_data).wstring()[CString(this->load_object.char_data).length() - 1] = '\0';
               ARRAY(&CString(this->load_object.char_data)->data).used_set(ARRAY(&CString(this->load_object.char_data)->data).count() - 2);
               uni_char = CString(this->load_object.char_data).wstring()[CString(this->load_object.char_data).length() -  1];
            }
         }
         CObjPersistent(this->load_object.object).attribute_set_string(this->load_object.parameter, CString(this->load_object.char_data));
         CObjPersistent(this->load_object.object).attribute_default_set(this->load_object.parameter, FALSE);
         CObjPersistent(this->load_object.object).attribute_update(this->load_object.parameter);
      }
      CObjPersistent(this->load_object.object).attribute_refresh(FALSE);
      CObjPersistent(this->load_object.object).notify_all_update(FALSE);
   }
   delete(this->load_object.char_data);

   if (STACK(&this->load_object_stack).count()) {
      this->load_object = STACK(&this->load_object_stack).pop();
      STACK(&this->load_object_stack).push(this->load_object);
   }
//   this->end_element = TRUE;
}/*CXMLStateLoader__xml_endElement*/

Local void CXMLStateLoader__xml_characterData(void *userData, const char *s, int len) {
   int i;
   unsigned short uni_char;
   char char8;
   CXMLStateLoader *this = CXMLStateLoader(userData);

   if (this->end_element) {
      return;
   }

   if (len >= 2 && (s[0] & 0xE0) == 0xC0 && (s[1] & 0xC0) == 0x80) {
      /* convert UTF-8 character */
      uni_char = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
      char8 = (char)uni_char;
//      CString(this->load_object.char_data).appendn(&char8, 1);
      CString(this->load_object.char_data).append(uni_char);
   }
   else if (len >= 3 && (s[0] & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80) {
      /*map certain >255 chars to 8 bits */
      uni_char = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
      switch (uni_char) {
      case 0x2026:
         char8 = 133;
         break;
      case 0x2022:
         char8 = 149;
         break;
      case 0x2013:
         char8 = 150;
         break;
      case 0x2018:
         char8 = 145;
         break;
      case 0x2019:
         char8 = 146;
         break;
      default:
         char8 = '?';
         break;
      }
//      CString(this->load_object.char_data).appendn(&char8, 1);
      CString(this->load_object.char_data).append(uni_char);
   }
   else {
      /* <128 plain ASCII character */
      for (i = 0; i < len; i++) {
         if (isspace(s[i]) && !CString(this->load_object.char_data).length()) {
            /* don't append leading spaces */
            continue;
         }
      uni_char = s[i];
      CString(this->load_object.char_data).append(uni_char);
      }
   //CString(this->load_object.char_data).appendn(s, len);
   }
}/*CXMLStateLoader__xml_characterData*/

Local void CXMLStateLoader__xml_commentHandler(void *userData, const XML_Char *data) {
   CXMLStateLoader *this = CXMLStateLoader(userData);
   CObjXMLComment *comment;
   TXMLLoadObject object;

   CLEAR(&object);
   if (STACK(&this->load_object_stack).count()) {
       object = STACK(&this->load_object_stack).top();
   }

   if (this->comments && object.object) {
       comment = new.CObjXMLComment(data);
       CObject(object.object).child_add(CObject(comment));
   }
}/*CXMLStateLoader__xml_commentHandler*/

int state_xml_loading;

bool CObjPersistent::state_xml_loading(void) {
   /*>>>hack! not thread safe*/
   return state_xml_loading != 0;
}/*CObjPersistent::state_xml_loading*/

void CObjPersistent::state_xml_load(CIOObject *stream, const char *default_namespace, bool comments) {
   CXMLStateLoader sload;

   state_xml_loading++;
   new(&sload).CXMLStateLoader();
   CXMLStateLoader(&sload).load_stream(&this, stream, default_namespace, comments);
   delete(&sload);
   state_xml_loading--;
}/*CObjPersistent::state_xml_load*/

bool CObjPersistent::state_xml_load_file(const char *filename, const char *default_namespace, bool comments, CString *message) {
   CFile file;
   bool result = TRUE;

   new(&file).CFile();

   if (CIOObject(&file).open(filename, O_RDONLY) == -1) {
       if (message) {
           CString(message).printf("file '%s' not found", filename);
       }
       result = FALSE;
   }
   if (result) {
      CObjPersistent(this).state_xml_load(CIOObject(&file), default_namespace, comments);
   }
   delete(&file);

   return result;
}/*CObjPersistent::state_xml_load_file*/

ARRAY:typedef<const TAttribute *>;

void CObjPersistent::state_xml_store_obj(CIOObject *stream, const char *default_namespace, int depth) {
   const TAttribute **attribute;
   int i, j;
   CObject *child;
   const CObjClass *obj_class, *obj_class_current;
   ARRAY<char> buffer;                     /*>>>need a FILE_write_printf() so this isn't needed*/
   ARRAY<const TAttribute *> element;
   const TAttribute *parameter_data = NULL;
   CString value;
   CStringXML value_expanded;
   LIST<CObject> *object_list;
   const char *class_alias;

   class_alias = CObjClass_alias(CObject(this).obj_class());
   if (default_namespace && strncmp(class_alias, default_namespace, strlen(default_namespace)) == 0) {
      class_alias = CObjClass_alias_short(CObject(this).obj_class());
   }

   new(&value).CString(NULL);
   new(&value_expanded).CStringXML(NULL);
   ARRAY(&buffer).new();
   ARRAY(&buffer).used_set(80);
   ARRAY(&element).new();

   for (i = 0; i < depth; i++) {CIOObject(stream).write_string("\t");}
   if (CObject(this).obj_class() == &class(CObjXMLComment)) {
       ARRAY(&buffer).used_set(CString(&CObjXMLComment(this)->comment).length() + 80);
       sprintf(ARRAY(&buffer).data(), "<!--%s--", CString(&CObjXMLComment(this)->comment).string());
       CIOObject(stream).write_string(ARRAY(&buffer).data());
   }
   else {
      sprintf(ARRAY(&buffer).data(), "<%s", class_alias);
      CIOObject(stream).write_string(ARRAY(&buffer).data());

      /*>>>should allow for derived class attributes to override base class ones
            of the same name */
      obj_class = &class(CObjPersistent);
      obj_class_current = &class(CObjPersistent);
      while (obj_class) {
         if (obj_class->ptbl) {
            for (attribute = obj_class->ptbl; *attribute; attribute++) {
               switch ((*attribute)->type) {
               case PT_Void:
                  break;
               case PT_AttributeArray:
                  if (!CObjPersistent(this).attribute_array_count(*attribute)) {
                     break;
                  }
               case PT_Attribute:
               case PT_AttributeEnum:
                  if (!CObjPersistent(this).attribute_default_get(*attribute)) {
                     CObjPersistent(this).attribute_get_string(*attribute, CString(&value));
                     if (!(*attribute)->data_type->no_expand) {
                        CStringXML(&value_expanded).expand(&value, TRUE);
                     }
                     else {
                        CStringXML(&value_expanded).expand(&value, FALSE);
//                        CString(&value_expanded).set_string(&value);
                     }
                     ARRAY(&buffer).used_set(CString(&value_expanded).length() + 80);
                     sprintf(ARRAY(&buffer).data(), " %s=\"%s\"",
                             (*attribute)->name, CString(&value_expanded).string());
                     CIOObject(stream).write_string(ARRAY(&buffer).data());
                   }
                   break;
               case PT_Element:
               case PT_ElementObject:
               case PT_ElementObjectList:
                  if (!CObjPersistent(this).attribute_default_get(*attribute)) {
                     ARRAY(&element).item_add(*attribute);
                  }
                  break;
               case PT_Data:
                  parameter_data = *attribute;
                  break;
               default:
                  break;
               }
            }
         }/*if*/

         obj_class = CObject(this).obj_class();
         while (obj_class) {
            if (obj_class && obj_class->base == obj_class_current) {
               obj_class_current = obj_class;
               break;
            }
            obj_class = obj_class->base;
         }
      }/*while*/
   }

   if ((CObject(this).child_count() && !CObject(this).obj_class()->option) ||
        ARRAY(&element).count() || parameter_data) {
      CIOObject(stream).write_string(">");
      if (CObject(this).child_count() || ARRAY(&element).count()) {
         CIOObject(stream).write_string("\n");
      }

      for (i = 0; i < ARRAY(&element).count(); i++) {
         attribute = &ARRAY(&element).data()[i];
         switch ((*attribute)->type) {
         case PT_Element:
            for (j = 0; j < depth + 1; j++) {CIOObject(stream).write_string("\t");}
            CObjPersistent(this).attribute_get_string(*attribute, CString(&value));
            CStringXML(&value_expanded).expand(&value, TRUE);
            ARRAY(&buffer).used_set(CString(&value_expanded).length() + 80);
            sprintf(ARRAY(&buffer).data(), "<%s>%s</%s>\n",
                    (*attribute)->name, CString(&value_expanded).string(), (*attribute)->name);
            CIOObject(stream).write_string(ARRAY(&buffer).data());
            break;
         case PT_ElementObject:
            CObjPersistent(&((byte *)this)[(*attribute)->offset]).state_xml_store_obj(stream, default_namespace, depth + 1);
            break;
         case PT_ElementObjectList:
            object_list = (LIST<CObject>*)(((byte *)this) + (*attribute)->offset);
            child = LIST(object_list).first();
            while (child) {
               CObjPersistent(child).state_xml_store_obj(stream, default_namespace, depth + 1);
               child = LIST(object_list).next(child);
            }
            break;
         default:
            throw(CObject(this), EXCEPTION<CObject,MethodInternalError>, "internal error");
         }
      }

      if (!CObject(this).obj_class()->option) {
#if 0
         child = CObject(this).child_first();
         while (child) {
         /*>>>check child is a persistent object before writing*/
            CObjPersistent(child).state_xml_store_obj(stream, default_namespace, depth + 1);
//>>>            if (!CObjPersistent(child).state_xml_store_obj(stream, depth + 1)) {
//               throw(CObject(this), EXCEPTION<CObjPersistent,XMLWriteError>, "XML Write Error");
//            }
            child = CObject(this).child_next(child);
         }
#endif

         if (parameter_data) {
            if (CObject(this).child_count() || ARRAY(&element).count()) {
               for (j = 0; j < depth + 1; j++) {CIOObject(stream).write_string("\t");}
            }
            CObjPersistent(this).attribute_get_string(parameter_data, CString(&value));
            CStringXML(&value_expanded).expand(&value, TRUE);
            CIOObject(stream).write_string(CString(&value_expanded).string());
            if (CObject(this).child_count() || ARRAY(&element).count()) {
               CIOObject(stream).write_string("\n");
            }
         }
#if 1
         child = CObject(this).child_first();
         while (child) {
         /*>>>check child is a persistent object before writing*/
            CObjPersistent(child).state_xml_store_obj(stream, default_namespace, depth + 1);
//>>>            if (!CObjPersistent(child).state_xml_store_obj(stream, depth + 1)) {
//               throw(CObject(this), EXCEPTION<CObjPersistent,XMLWriteError>, "XML Write Error");
//            }
            child = CObject(this).child_next(child);
         }
#endif
      }

      if (CObject(this).child_count() || ARRAY(&element).count()) {
         for (i = 0; i < depth; i++) {CIOObject(stream).write_string("\t");}
      }
      sprintf(ARRAY(&buffer).data(), "</%s>", class_alias);
      CIOObject(stream).write_string(ARRAY(&buffer).data());
   }
   else {
      if (CObject(this).obj_class() == &class(CObjXMLComment)) {
         CIOObject(stream).write_string(">");
      }
      else {
         CIOObject(stream).write_string("/>");
      }
   }

   CIOObject(stream).write_string("\n");

   ARRAY(&element).delete();
   ARRAY(&buffer).delete();
   delete(&value_expanded);
   delete(&value);
}/*CObjPersistent::state_xml_store_obj*/

void CObjPersistent::state_xml_store(CIOObject *stream, const char *default_namespace) {
   CIOObject(stream).write_string("<?xml version=\"1.0\"?>\n\n");

   CObjPersistent(this).state_xml_store_obj(stream, default_namespace, 0);
}/*CObjPersistent::state_xml_store*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
