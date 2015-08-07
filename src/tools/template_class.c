/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

#include "environ.h"
#include "../base/contain.h"
#include <ctype.h>

#include "keycc.h"
#include "template.h"
#include "../base/parameter.h"

#define MAX_ARG                   100
#define CLASS_MAX                 1000
#define CLASS_NAME                "class"
#define VIRTUAL_NAME              "virtual"
#define DESTROY_NAME              "delete"
#define ATTRIBUTE_TEMPLATE_NAME   "ATTRIBUTE"
#define ELEMENT_TEMPLATE_NAME     "ELEMENT"
#define MENU_TEMPLATE_NAME        "MENU"
#define DATA_TEMPLATE_NAME        "DATA"
#define ALIAS_TEMPLATE_NAME       "ALIAS"
#define OPTION_TEMPLATE_NAME      "OPTION"
#define PARAMETER_NAME            "__parameter"
#define ATTRIBUTETYPE_NAME        "__attributetype"
#define ATTRIBUTETYPEARRAY_NAME   "__attributetypearray"
#define ATTRIBUTETYPECONVERT_NAME "__attributetypeconvert"

typedef enum {
   MT_Static,
   MT_Virtual,
   MT_Inherited,
} EMethodType;

typedef struct {
   EMethodType type;
   bool implemented;
   int lineno;
   char line[MAX_LINE];
   char name[MAX_LINE];
   char return_type[MAX_ARG];
   char storage_class[MAX_ARG];
   int arg_count;
   char arg_type[40][MAX_ARG];
   char arg_name[40][MAX_ARG];
   char proto[MAX_ARG];
   char vmethod_name[MAX_ARG];
   char vmethod_overload[MAX_ARG];
} TClassMethod;

typedef struct {
   char name[MAX_ARG];
} TClassException;

typedef struct {
   char data_type[MAX_ARG];
   int enum_count;
   char name[MAX_ARG];
   char data[MAX_ARG];
   char method[MAX_ARG];
   char options[MAX_ARG];
   char delim[MAX_ARG];
   char path[MAX_ARG];
   EParameterType type;
   int lineno;
} TClassParameter;

typedef struct {
   int option;
   char name[MAX_ARG];
   char alias[MAX_ARG];
   char displayname[MAX_ARG];
   char base[MAX_ARG];
   bool no_base;
   TTemplate template;
   int method_count;
   TClassMethod *method;
   int exception_count;
   TClassException *exception;
   int parameter_count;
   TClassParameter *parameter;
   TClassParameter *parameter_current;
} TClass;

typedef struct {
   TClass *class_current, *class_method;
   int class_count;
   TClass class[CLASS_MAX];
   bool in_class;
   bool in_method;
   bool in_attribute;
   bool remove_statement;
   int class_depth;
   int method_depth;
   int attribute_depth;

   char attribute_method[10000];
} TTemplateClass;

TTemplateClass tclass;

Global bool CLASS_in_class(void) {
   return tclass.in_class;
}/*CLASS_in_class*/

Global bool CLASS_in_method(void) {
   return tclass.in_method;
}/*CLASS_in_method*/

Global bool CLASS_in_attribute(void) {
   return tclass.in_attribute;
}/*CLASS_in_attribute*/

Local void CLASS_type_find(char **start, char **end) {
   char *cp = *end;

   while (ismchar(*cp)) {
      cp--;
      if (cp == *start)
         return;
   }

   while (isspace(*cp)) {
      cp--;
      if (cp == *start)
         return;
   }

   *end = cp;
   if (*cp == '*') {
      while (*cp == '*' || isspace(*cp)) {
            cp--;
      }
   }
   while (ismchar(cp[-1]) || strncmp(cp - 7, "struct tag_", 11) == 0) {
      cp--;
   }
   *start = cp;
}/*CLASS_type_find*/

void CLASS_method_name_translate(TClass *this, char *method_name) {
   if (*method_name == '~' &&
         strcmp(&method_name[1], this->name) == 0) {
         strcpy(method_name, DESTROY_NAME);
      }
}/*CLASS_method_name_translate*/

TClassMethod *CLASS_method_find(TClass *this, char *name) {
   int i;
   TClassMethod *mp = this->method;

   for (i = 0; i < this->method_count; i++) {
      if (strcmp(mp->name, name) == 0)
         return mp;
      mp++;
   }

   return NULL;
}/*CLASS_method_find*/

void CLASS_method_name_set(TClass *this, TClassMethod *method) {
   sprintf(method->vmethod_name, "_virtual_%s_%s",
           this->name, method->name);
}/*CLASS_method_name_set*/

void CLASS_new(TClass *this, TClass *base) {
   int i;

   if (!base)
       return;

   /* copy virtual function definitions to derived class*/
   this->method = malloc(sizeof(TClassMethod) * base->method_count);

   for (i = 0; i < base->method_count; i++) {
      if (base->method[i].type == MT_Virtual ||
          base->method[i].type == MT_Inherited) {
         memcpy(&this->method[this->method_count], &base->method[i],
                sizeof(TClassMethod));
         this->method[this->method_count].type        = MT_Inherited;
         this->method[this->method_count].implemented = FALSE;
         CLASS_method_name_set(this, &this->method[this->method_count]);
         this->method_count++;
      }
   }
}/*CLASS_new*/

TClass *TCLASS_class_find(char *name) {
   int i;
   TClass *cp = tclass.class;

   for (i = 0; i < tclass.class_count; i++) {
      if (strcmp(cp->name, name) == 0)
         return cp;
      cp++;
   }

   return NULL;
}/*TCLASS_class_find*/

char *CNAME_template(struct TEMPLATE *this,
                     EBlockType type, TTemplateArgs *args,
                     int dest_length, char *dest,
                     int extra_length, char *extra) {
   int i;
   TClass *cp = TCLASS_class_find(this->name);
   TClassMethod *method;
   char method_name[MAX_LINE];
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }

   if (!(type == BT_Template && args->type == TT_Single) &&
      tclass.in_class && !tclass.in_attribute)
      return NULL;

//>>>   tclass.class_current = cp;

   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Object:
         sprintf(dest, "%s_%s(CAST_%s(%s)",
                 this->name, args->operation[0], this->name, args->object);
         break;
      case TT_Definition:
         if (args->argc == 0) {
            sprintf(dest, "%s_%s", this->name, args->operation[0]);
            return NULL;
         }
         switch (args->op_count) {
         case 0:
            sprintf(dest, "CAST_%s(%s)", this->name, args->argv[0]);
            return NULL;
         case 1:
            tclass.class_method = cp;
            strcpy(method_name, args->operation[0]);
            CLASS_method_name_translate(cp, method_name);

            *dest = '\0';
            method = CLASS_method_find(cp, method_name);
            if (!method || !method->implemented) {
               return "method without prototype";
            }
            if (method->type == MT_Virtual || method->type == MT_Inherited) {
                sprintf(dest + strlen(dest), "_virtual_");
            }
            sprintf(dest + strlen(dest), "%s_%s(%s *this",
                    this->name, method_name, this->name);

            tclass.in_method = TRUE;
            break;
         }
         break;
      case TT_Single:
         sprintf(dest, "struct tag_%s", this->name);
         return NULL;
      default:
         return "error";
      }

      if (args->argc && strcmp(args->argv[0], "void") != 0) {
         for (i = 0; i < args->argc; i++) {
            sprintf(dest + strlen(dest), ",%s", args->argv[i]);
         }
      }
      sprintf(dest + strlen(dest), ")");
      return NULL;
   case BT_BlockBegin:
      tclass.method_depth++;
      return NULL;
   case BT_BlockEnd:
      tclass.method_depth--;
      if (tclass.method_depth == 0) {
         tclass.in_method = FALSE;
      }
      return NULL;
   case BT_BlockEndPost:
      break;
   default:
      break;
   }

   return NULL;
}/*CNAME_template*/

typedef enum {
   MA_Public,
   MA_Private,
   MA_Protected,
} TMemberAccess;

typedef struct {
   TMemberAccess access;
   char *name;
} TMemberAccessHash;

TMemberAccessHash member_access_hash[] = {
   {MA_Public,    "public"},
   {MA_Private,   "private"},
   {MA_Protected, "protected"}
};

Local void CMETHOD_access_strip(char *line) {
   char *cp = line;
   char *access_start, *access_end;
   int i;
   TMemberAccessHash *access;
   bool access_space = FALSE;

   while (*cp) {
      for (i = 0; i < COUNTOF(member_access_hash); i++) {
         access = &member_access_hash[i];
         if (strncmp(cp, access->name, strlen(access->name)) == 0) {
            access_start = cp;
            cp += strlen(access->name);
            while (isspace(*cp)) {
               cp++;
            }
            if (*cp == ':') {
               access_space = TRUE;
               cp++;
            }
            access_end = cp - 1;

         memcpy(access_start, access_end + 1, strlen(access_end + 1) + 1);
         }
      }
      cp++;
   }
}/*CMETHOD_access_strip*/

Local bool CMETHOD_method_find(TClass *class, char *line, TClassMethod *result) {
   char *cp, *cp_end, *arg, *sc_begin;
   int i;

   memset(result, 0, sizeof(TClassMethod));
   result->implemented = TRUE;

   strcpy(result->line, line);

   cp = result->line;
   while (*cp) {
      if (strchr("<(", *cp)) {
         break;
      }
      cp++;
   }

   if (*cp == '\0')
      return FALSE;

   cp_end = strmatch(cp);
   if (!cp_end) {
      printf("keycc: incomplete method declaration in class\n");
      exit(1);
      }

   *cp = '\0';
   *cp_end = '\0';

   i = 0;

   arg = strtok(cp + 1, ",");
   while (1) {
      if (!arg)
         break;

      while (*arg == ' ') {
         arg++;
         }

      cp_end = arg + strlen(arg) - 1;
      while (ismchar(*cp_end)) {
         cp_end--;
      }
      if (cp_end == arg - 1) {
         result->arg_name[i][0] = '\0';
         strcpy(result->arg_type[i], arg);
      }
      else {
         strcpy(result->arg_name[i], cp_end + 1);
         strcpy(result->arg_type[i], arg);
         result->arg_type[i][cp_end + 1 - arg] = '\0';
      }

      if (strcmp(result->arg_type[i], "void")) {
         i++;
         }
      arg = strtok(NULL, ",");
      }
   result->arg_count = i;

   cp_end = cp;
   while (cp > result->line && ismchar(cp[-1])) {
      cp--;
   }

   strcpy(result->name, cp);
   CLASS_method_name_translate(class, result->name);

   if (cp == result->line)
      return TRUE;

   cp--;
   cp_end = cp;

   cp = sc_begin = result->line;
   result->type = MT_Static;
   while (cp != cp_end) {
      if (strncmp(cp, VIRTUAL_NAME, strlen(VIRTUAL_NAME)) == 0) {
         result->type        = MT_Virtual;
         cp += strlen(VIRTUAL_NAME);
         sc_begin = cp;
         break;
      }
      cp++;
   }
   if (cp == cp_end) {
      cp = result->line;
   }

   while (isspace(*cp)) {
      cp++;
   }

   if (cp >= cp_end) {
      result->return_type[0] = '\0';
   }
   else {
      cp = sc_begin;
      CLASS_type_find(&cp, &cp_end);
      strncpy(result->return_type, cp, cp_end - cp + 1);
      result->return_type[cp_end - cp + 1] = '\0';
      strncpy(result->storage_class, sc_begin, cp - sc_begin);
      result->storage_class[cp - sc_begin + 1] = '\0';
   }

   return TRUE;
}/*CMETHOD_method_find*/

void CMETHOD_args_print(TClassMethod *this, char *dest, bool name_only) {
   int i;

   for (i = 0; i < this->arg_count; i++) {
      sprintf(dest + strlen(dest), ", ");
      if (!name_only) {
         sprintf(dest + strlen(dest), "%s", this->arg_type[i]);
      }
      sprintf(dest + strlen(dest), "%s", this->arg_name[i]);
   }
}/*CMETHOD_args_print*/

#if OS_Linux
#include <unistd.h>
#endif

void CMETHOD_section_print(TClassMethod *this, char *dest) {
   /* code sections for the Palm, quick hack for now */
   char file_name[200];
   char *cp_start;
   char *cp_end;

   strcpy(file_name, PREPROCESS_current_filename(&preprocess));
   cp_end   = strrchr(file_name, '/');
   *cp_end  = '\0';
   cp_start = strrchr(file_name, '/') + 1;

   if (PREPROCESS_CCompiler(&preprocess) == CC_m68k_palmos_gcc) {
      if (strcmp(cp_start, "graphics") == 0) {
         sprintf(dest + strlen(dest), " __attribute__ ((section (\"");
         sprintf(dest + strlen(dest), "%s", cp_start);
         sprintf(dest + strlen(dest), "\")))");
      }
   }
}/*CMETHOD_section_print*/

char *CLASS_template(struct TEMPLATE *this,
                     EBlockType type, TTemplateArgs *args,
                     int dest_length, char *dest,
                     int extra_length, char *extra) {
   TClassMethod *method;
   TClassMethod method_dec;
   TClassException *exception;
   TClassParameter *parameter;
   int i;
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }
   
   if (tclass.in_class && type == BT_Template && args->op_count)  {
      return "class within class";
   }

   switch (type) {
   case BT_Template:
      tclass.remove_statement = FALSE;
      switch (args->type) {
      case TT_Object:
         /* eg class:base.method(a, b, c); */
         sprintf(dest, "_virtual_%s_%s((void *)this",
                 tclass.class_method->base,
                 /* *args->operation[0] == '~' ? DESTROY_NAME : */args->operation[0]);

         for (i = 0; i < args->argc; i++) {
            sprintf(dest + strlen(dest), ", %s", args->argv[i]);
         }

         sprintf(dest + strlen(dest), ")");
         return NULL;
      default:
         switch (args->op_count) {
         case 0:
            sprintf(dest, CLASS_NAME"_%s", args->argv[0]);
            return NULL;
         case 1:            
//            sprintf(dest, "struct tag_%s; typedef struct tag_%s %s", args->operation[0], args->operation[0], args->operation[0]);
            sprintf(dest, "struct tag_%s", args->operation[0]);
            tclass.class_current = TCLASS_class_find(args->operation[0]);
            if (!tclass.class_current) {
               tclass.class_depth = 0;
               tclass.class_current = &tclass.class[tclass.class_count];
               strcpy(tclass.class_current->name, args->operation[0]);
//               strcpy(tclass.class_current->base, args->operation[1]);
               //CLASS_new(tclass.class_current, TCLASS_class_find(args->operation[1]));
               tclass.class_count++;
            
               /* Register new class as template */
               if (!tclass.class_current->template.template_fn) {
                  strcpy(tclass.class_current->template.name, tclass.class_current->name);
                  tclass.class_current->template.template_fn = CNAME_template;
                  PREPROCESS_template_register(&preprocess, &tclass.class_current->template);
               }
            }
            return NULL;
         case 2:
            tclass.class_current = TCLASS_class_find(args->operation[0]);         
            if (!tclass.class_current) {
               tclass.class_current = &tclass.class[tclass.class_count];
               tclass.class_count++;
            }
            tclass.class_depth = 0;
            tclass.in_class = TRUE;
//            tclass.class_current = &tclass.class[tclass.class_count];
            strcpy(tclass.class_current->name, args->operation[0]);
            strcpy(tclass.class_current->base, args->operation[1]);
            CLASS_new(tclass.class_current, TCLASS_class_find(args->operation[1]));
  //          tclass.class_count++;

            sprintf(dest, "typedef struct tag_%s {", tclass.class_current->name);
            if (tclass.class_current->base[0] == '\0') {
               tclass.class_current->no_base = TRUE;
            }
            else {
               sprintf(dest + strlen(dest), "%s base;",
                       args->operation[1]);
            }
            return NULL;
         default:
            break;
         }
         break;
      }
      return "error";
   case BT_BlockBegin:
      if (tclass.class_depth == 0) {
         *tclass.attribute_method = '\0';
         *dest = '\0';

         /* Register new class as template */
         if (!tclass.class_current->template.template_fn) {         
            strcpy(tclass.class_current->template.name, tclass.class_current->name);
            tclass.class_current->template.template_fn = CNAME_template;
            PREPROCESS_template_register(&preprocess, &tclass.class_current->template);
         }
      }
      tclass.class_depth++;
      return NULL;
   case BT_BlockEnd:
      tclass.class_depth--;
      return NULL;
   case BT_BlockEndPost:
      if (tclass.class_depth != 0)
         return NULL;

      tclass.in_class = FALSE;

      sprintf(dest, " %s", tclass.class_current->name);

      *extra = '\0';

      /* generate virtual method info */
      for (i = 0; i < tclass.class_current->method_count; i++) {
         method = &tclass.class_current->method[i];
         if (method->type == MT_Virtual) {
            sprintf(extra + strlen(extra), "typedef %s (*%s)(struct tag_%s *this",
                    method->return_type, method->proto, tclass.class_current->name);
            CMETHOD_args_print(method, extra, FALSE);
            sprintf(extra + strlen(extra), ");\n");
         }
         if (method->type == MT_Virtual || method->type == MT_Inherited) {
            sprintf(extra + strlen(extra), "%s %s(struct tag_%s *this",
                    method->return_type, method->vmethod_name, tclass.class_current->name);
            CMETHOD_args_print(method, extra, FALSE);
            sprintf(extra + strlen(extra), ")");
            CMETHOD_section_print(method, extra + strlen(extra));
            sprintf(extra + strlen(extra), ";\n");
         }
      }

      sprintf(extra + strlen(extra), "typedef struct {\n");

      for (i = 0; i < tclass.class_current->method_count; i++) {
         method = &tclass.class_current->method[i];
         if (method->type == MT_Virtual || method->type == MT_Inherited) {
            sprintf(extra + strlen(extra),
                    "   %s %s;\n", method->proto, method->name);
         }
      }
      sprintf(extra + strlen(extra), "} TVtbl_%s;\n", tclass.class_current->name);

      /* virtual method table */
      if (args->current_file) {
         sprintf(extra + strlen(extra), "const TVtbl_%s vtbl_%s = {\n",
                 tclass.class_current->name, tclass.class_current->name);
         for (i = 0; i < tclass.class_current->method_count; i++) {
            method = &tclass.class_current->method[i];
            if (method->type == MT_Virtual || method->type == MT_Inherited) {
               sprintf(extra + strlen(extra), "   (%s)%s,\n",
                        method->proto, method->vmethod_overload);
            }
         }
         sprintf(extra + strlen(extra), "};\n");
      }
      else {
         sprintf(extra + strlen(extra), "extern TVtbl_%s vtbl_%s;\n",
                 tclass.class_current->name, tclass.class_current->name);
      }

      /* virtual method wrappers */
      for (i = 0; i < tclass.class_current->method_count; i++) {
         method = &tclass.class_current->method[i];

         switch (method->type) {
         case MT_Static:
            sprintf(extra + strlen(extra),
                    "%s %s %s_%s(struct tag_%s *this",
                    method->storage_class, method->return_type, tclass.class_current->name, method->name,
                    tclass.class_current->name);
            CMETHOD_args_print(method, extra, FALSE);

            sprintf(extra + strlen(extra), ")");
            CMETHOD_section_print(method, extra + strlen(extra));
            sprintf(extra + strlen(extra), ";\n");
            break;
         case MT_Virtual:
         case MT_Inherited:
sprintf(extra + strlen(extra), "#line %d\n", method->lineno);
            sprintf(extra + strlen(extra),
                    "static __inline %s %s_%s(struct tag_%s *this",
                    method->return_type, tclass.class_current->name, method->name,
                    tclass.class_current->name);
            CMETHOD_args_print(method, extra, FALSE);
            sprintf(extra + strlen(extra), ") {");

            sprintf(extra + strlen(extra),
                    "char *obj_class = *(void **)((char *)this + %d);",
                    PREPROCESS_cobjclass_offset(&preprocess));
            sprintf(extra + strlen(extra),
                    "TVtbl_%s *vtbl = *(TVtbl_%s **) (obj_class + %d);",
                    tclass.class_current->name, tclass.class_current->name,
                    PREPROCESS_cobjclass_vtbl_offset(&preprocess));

            if (strcmp(method->return_type, "void") != 0) {
               sprintf(extra + strlen(extra), "return ");
            }
            sprintf(extra + strlen(extra), "(*vtbl->%s)((void *)this",
                    method->name);
            CMETHOD_args_print(method, extra, TRUE);
            sprintf(extra + strlen(extra), ");");

            sprintf(extra + strlen(extra), "}\n");
            break;
         }
      }

      /* exceptions */
      if (tclass.class_current->exception_count) {
         sprintf(extra + strlen(extra), "typedef enum {\n");
         for (i = 0; i < tclass.class_current->exception_count; i++) {
            exception = &tclass.class_current->exception[i];
            sprintf(extra + strlen(extra), "   %s_exception_%s,\n",
                    tclass.class_current->name, exception->name);
         }
         sprintf(extra + strlen(extra), "} EException%s;\n",
                 tclass.class_current->name);
      }

      /* Class cast prototype */
      sprintf(extra + strlen(extra),
              "static __inline %s *CAST_%s(const void *obj);\n",
              tclass.class_current->name, tclass.class_current->name);
//      if (method) {
//         sprintf(extra + strlen(extra),
//                 "static __inline %s *CONSTRUCT_%s(void *obj);\n",
//                 tclass.class_current->name, tclass.class_current->name);
//      }
      
      if (!args->current_file) {
         sprintf(extra + strlen(extra), "extern ");
      }
      sprintf(extra + strlen(extra), "CObjClass "CLASS_NAME"_%s;\n", tclass.class_current->name);
      
      /* parameter table */
      sprintf(extra + strlen(extra), "%s", tclass.attribute_method);
      if (tclass.class_current->parameter_count) {
         if (args->current_file) {
            for (i = 0; i < tclass.class_current->parameter_count; i++) {
               parameter = &tclass.class_current->parameter[i];
               sprintf(extra + strlen(extra), "TParameter "PARAMETER_NAME"_%s_%s = ",
                       tclass.class_current->name, parameter->data);
               switch (parameter->type) {
               case PT_Attribute:
               case PT_AttributeArray:
               case PT_Element:
               case PT_Data:
               case PT_Menu:
                  sprintf(extra + strlen(extra),
                          "{&"CLASS_NAME"_%s, %d, &"ATTRIBUTETYPE_NAME"_%s, 0, 0, %s, (size_t)&(((%s *)0)->%s), (void *)%s, %s, %s, %s};\n",
                          tclass.class_current->name, parameter->type, parameter->data_type,
                          parameter->name, tclass.class_current->name, parameter->data,
                          strlen(parameter->method) ? parameter->method : "0",
                          strlen(parameter->options) ? parameter->options : "0",
                          strlen(parameter->delim) ? parameter->delim : "0",
                          strlen(parameter->path) ? parameter->path : "0");
                  break;
               case PT_AttributeEnum:
                  sprintf(extra + strlen(extra),
                          "{&"CLASS_NAME"_%s, %d, &"ATTRIBUTETYPE_NAME"_int, %d, _enumname__%s, %s, (size_t)&(((%s *)0)->%s), (void *)%s, %s};\n",
                          tclass.class_current->name, parameter->type, parameter->enum_count, parameter->data_type,
                          parameter->name, tclass.class_current->name, parameter->data,
                          strlen(parameter->method) ? parameter->method : "0",
                          strlen(parameter->options) ? parameter->options : "0");
                  break;
               case PT_ElementObject:
               case PT_ElementObjectList:
                  sprintf(extra + strlen(extra),
                          "{&"CLASS_NAME"_%s, %d, 0, 0, 0, %s, (size_t)&(((%s *)0)->%s)};\n",
                          tclass.class_current->name, parameter->type,
                          parameter->name, tclass.class_current->name, parameter->data);
                  break;
               default:
                  break;
               }
            }
            sprintf(extra + strlen(extra), "const TParameter *ptbl_%s[] = {\n",
                    tclass.class_current->name);
            for (i = 0; i < tclass.class_current->parameter_count; i++) {
               parameter = &tclass.class_current->parameter[i];
               sprintf(extra + strlen(extra), "&"PARAMETER_NAME"_%s_%s",
                       tclass.class_current->name, parameter->data);
               sprintf(extra + strlen(extra), ",\n");
            }
            sprintf(extra + strlen(extra), "0 };\n");
         }
         else {
            for (i = 0; i < tclass.class_current->parameter_count; i++) {
               parameter = &tclass.class_current->parameter[i];
               sprintf(extra + strlen(extra), "extern TParameter "PARAMETER_NAME"_%s_%s;\n",
                       tclass.class_current->name, parameter->data);
            }
         }
      }
      
      /* Class cast */
#if 0
      /*>>>choose this based on configuration option */
      sprintf(extra + strlen(extra),
              "static __inline %s *CAST_%s(void *obj) {\n"
              "   return (%s *)obj; \n}\n\n",
              tclass.class_current->name, tclass.class_current->name,
              tclass.class_current->name);
#else
      sprintf(extra + strlen(extra),
              "#line 70 \"object.c\"\n"
              "static __inline %s *CAST_%s(const void *obj) {"
              "   CObjClass_valid_cast(&class_%s, (CObject *)obj);"
              "   return (%s *)obj; }\n",
              tclass.class_current->name, tclass.class_current->name,
              tclass.class_current->name,
              tclass.class_current->name);
              
      method = NULL;     
      for (i = 0; i < tclass.class_current->method_count; i++) {
         if (strcmp(tclass.class_current->method[i].name, tclass.class_current->name) == 0) {
            method = &tclass.class_current->method[i];
            break;
         }
      }

      if (method) {
         sprintf(extra + strlen(extra), "static __inline %s *CONSTRUCT_%s(void *CONSTRUCT_obj", tclass.class_current->name, tclass.class_current->name);
         for (i = 0; i < method->arg_count; i++) {
            sprintf(extra + strlen(extra), ", %s %s", method->arg_type[i], method->arg_name[i]);
         }
         sprintf(extra + strlen(extra), ") {\n"
                                        "   CONSTRUCT_obj = CObject__construct(CONSTRUCT_obj, &class_%s);\n"
                                        "   %s_%s(CONSTRUCT_obj",
                                        tclass.class_current->name,                               
                                        tclass.class_current->name, tclass.class_current->name);
         for (i = 0; i < method->arg_count; i++) {
            sprintf(extra + strlen(extra), ",%s", method->arg_name[i]);
         }
         sprintf(extra + strlen(extra), ");\n   return CONSTRUCT_obj;}\n", method->arg_name[i]);
      }
//      sprintf(extra + strlen(extra),
//              "void *FRAMEWORK_last_object_set(void *object);\n"
//              "#line 109 \"object.c\"\n"              
//              "static __inline struct tag_%s *CONSTRUCT_%s(void *obj) {"
//              "   return (struct tag_%s *)FRAMEWORK_last_object_set(CObject__construct(obj, &class_%s)); }\n",
//              tclass.class_current->name, tclass.class_current->name,
//              tclass.class_current->name, tclass.class_current->name);
#endif

      /* class record */
      if (args->current_file) {
          sprintf(extra + strlen(extra), "CObjClass class_%s = {{%uUL, IT_Class},",
                  tclass.class_current->name, SID_INIT);
          if (strlen(tclass.class_current->base) == 0) {
             sprintf(extra + strlen(extra), " 0,");
          }
          else {
             sprintf(extra + strlen(extra), " &class_%s,",
                     tclass.class_current->base);
          }
          sprintf(extra + strlen(extra), " &vtbl_%s, ", tclass.class_current->name);
          if (tclass.class_current->parameter_count) {
             sprintf(extra + strlen(extra), "ptbl_%s, ", tclass.class_current->name);
          }
          else {
             sprintf(extra + strlen(extra), "0, ");
          }
          sprintf(extra + strlen(extra), "\"%s\", %s, %s, sizeof(%s), %d};\n",
                  tclass.class_current->name,
                  strlen(tclass.class_current->alias) ? tclass.class_current->alias : "0",
                  strlen(tclass.class_current->displayname) ? tclass.class_current->displayname : "0",
                  tclass.class_current->name,
                  tclass.class_current->option);
      }
      else {
         sprintf(extra + strlen(extra), " extern CObjClass class_%s;\n",
                 tclass.class_current->name);
      }
      return NULL;
   case BT_Statement:
      CMETHOD_access_strip(args->operation[0]);
      strcpy(dest, args->operation[0]);
      if (CMETHOD_method_find(tclass.class_current, args->operation[0], &method_dec)) {
         dest[0] = '\0';

         tclass.remove_statement = TRUE;

         if (strcmp(method_dec.name, EXCEPTION_NAME) == 0) {
            for (i = 0; i  < method_dec.arg_count; i++) {
               tclass.class_current->exception_count++;
               tclass.class_current->exception =
                  realloc(tclass.class_current->exception,
                          tclass.class_current->exception_count * sizeof(TClassException));
               exception = &tclass.class_current->exception[tclass.class_current->exception_count - 1];
               sprintf(exception->name, "%s%s",
                       method_dec.arg_type[i], method_dec.arg_name[i]);
            }
         }
         else {
            method = CLASS_method_find(tclass.class_current, method_dec.name);
            if (method) {
               strcpy(method->vmethod_overload, method->vmethod_name);
               method->implemented = TRUE;
            }
            else {
               tclass.class_current->method_count++;
               tclass.class_current->method =
                  realloc(tclass.class_current->method,
                          tclass.class_current->method_count * sizeof(TClassMethod));
               method = &tclass.class_current->method[tclass.class_current->method_count - 1];
               CMETHOD_method_find(tclass.class_current, args->operation[0], method);
               method->lineno = args->lineno;
               sprintf(method->proto, "METHOD_virtual__%s_%s",
                       tclass.class_current->name, method->name);
               CLASS_method_name_set(tclass.class_current, method);
               strcpy(method->vmethod_overload, method->vmethod_name);
            }
         }
      }
      return NULL;
   case BT_Separator:
      if (tclass.remove_statement) {
         dest[0] = '\0';
         tclass.remove_statement = FALSE;
      }
      return NULL;
   default:
      break;
   }
   return NULL;
}/*CLASS_template*/

TTemplate tp_Class = {
   "class",
   CLASS_template,
   TRUE,
};

char *PARAMETER_template(struct TEMPLATE *this,
                         EBlockType type, TTemplateArgs *args,
                         int dest_length, char *dest,
                         int extra_length, char *extra) {
   TClassParameter *parameter;
   char *start, *end;
   char ptype[MAX_ARG];
   bool is_array = FALSE;
   
   if (args->process_mode == PM_Documentation) {
      switch (type) {
      case BT_Template:
         if (args->op_count && strcmp(args->operation[0], "ARRAY") == 0) {
            sprintf(dest, "ARRAY<%s>", args->argv[0]);
         }
         else {
            sprintf(dest, "%s", args->argv[0]);
         }
         break;
      default:
         *dest = '\0';
         break;
      }
      return NULL;
   }

   /* add parameter to class if inside class */
   if (CLASS_in_class()) {
      switch (type) {
      case BT_BlockBegin:
         tclass.attribute_depth++;
         if (!tclass.in_attribute) {
            tclass.in_attribute = TRUE;
            sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "#line %d\n", args->lineno);
            sprintf(tclass.class_current->parameter_current->method, "METHOD_attribute_%s_%s",
            tclass.class_current->name, tclass.class_current->parameter_current->data);
            tclass.class_current->parameter_current->lineno = args->lineno;

            /*>>>array types*/
            sprintf(ptype, "%s", tclass.class_current->parameter_current->data_type);

            if (args->current_file) {
               sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "void %s(%s *this, %s *data, int attribute_element) {\n",
               tclass.class_current->parameter_current->method,
               tclass.class_current->name,
               ptype);
            }
            else {
               sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "extern void %s(%s *this, %s *data, int attribute_element);",
                       tclass.class_current->parameter_current->method,
                       tclass.class_current->name,
                       ptype);
            }
         }
         else {
            if (args->current_file) {
               sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "%s", dest);
            }
         }
         dest[0] = '\0';
         return NULL;
      case BT_BlockEnd:
         tclass.attribute_depth--;
         if (tclass.attribute_depth == 0) {
            tclass.in_attribute = FALSE;
            if (args->current_file) {
               sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "}");
            }
            sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "\n");
         }
         else {
            if (args->current_file) {
               sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "%s", dest);
            }
         }
         dest[0] = '\0';
         return NULL;
      case BT_BlockBeginPre:
      case BT_BlockBeginPost:
      case BT_BlockEndPre:
      case BT_BlockEndPost:
         return NULL;
      default:
         break;
     }
     if (tclass.attribute_depth) {
        /* processing an attribute method */
        if (args->current_file) {
            sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "\n#line %d\n", args->lineno);        
            sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "%s", dest);
//            if (dest[0] == ';') {
//               /*>>>kludge, should pass through seperators */
//               sprintf(tclass.attribute_method + strlen(tclass.attribute_method), "\n");
//            }
        }
        dest[0] = '\0';
        return NULL;
     }

      /* else attribute in class processing */
      tclass.class_current->parameter_count++;
      tclass.class_current->parameter =
         realloc(tclass.class_current->parameter,
                 tclass.class_current->parameter_count * sizeof(TClassParameter));
      parameter = &tclass.class_current->parameter[tclass.class_current->parameter_count - 1];
      memset(parameter, 0, sizeof(TClassParameter));
      tclass.class_current->parameter_current = parameter;

      start = args->argv[0];
      end   = args->argv[0] + strlen(args->argv[0]);
      CLASS_type_find(&start, &end);
      strcpy(parameter->data, start);      
      strcpy(parameter->data_type, args->argv[0]);
      PREPROCESS_name_convert(parameter->data_type);
      start--;
      while (isspace(*start)) {
         start--;
      }
      parameter->data_type[start - args->argv[0] + 1] = '\0';
      if (args->argc == 1 || strlen(args->argv[1]) == 0 || this == &tp_Menu) {
         sprintf(parameter->name, "\"%s\"", parameter->data);
      }
      else {
         strcpy(parameter->name, args->argv[1]);
      }
      sprintf(dest, "%s %s", parameter->data_type, parameter->data);
      if (strncmp(parameter->data_type, "ARRAY<", 6) == 0) {
         memmove(parameter->data_type, parameter->data_type + 6, strlen(parameter->data_type) - 5);
         parameter->data_type[strlen(parameter->data_type) - 1] = '\0';
         is_array = TRUE;
      }

      if (args->argc >= 3) {
         if (this == &tp_Menu) {
            sprintf(parameter->delim, "%s", args->argv[2]);
         }
         else {
            sprintf(parameter->options, "%s", args->argv[2]);
         }
      }

      if (this == &tp_Attribute) {
         if (args->op_count != 0) {
            return "bad attribute options";
         }
         parameter->type = PT_Attribute;

         if (is_array || (args->op_count && strcmp(args->operation[0], "ARRAY") == 0)) {
            parameter->type = PT_AttributeArray;
            sprintf(dest, "type_ARRAY_%s %s", parameter->data_type, parameter->data);
         }
      }
      else if (this == &tp_Element) {
         if (args->op_count == 0) {
            parameter->type = PT_Element;
         }
         else if (strcmp(args->operation[0], "OBJECT") == 0) {
            parameter->type = PT_ElementObject;
         }
         else if (strcmp(args->operation[0], "LIST") == 0) {
            parameter->type = PT_ElementObjectList;
            sprintf(dest, "type_LIST_%s %s", parameter->data_type, parameter->data);
         }
         else {
            return "bad element type";
         }
      }
      if (this == &tp_Menu) {
         parameter->type = PT_Menu;
         sprintf(parameter->path, "%s", args->argv[1]);
      }
      else if (this == &tp_Data) {
         parameter->type = PT_Data;
      }

      parameter->enum_count = TEXT_enum_count(parameter->data_type);
      if (parameter->enum_count) {
         parameter->type = PT_AttributeEnum;
      }

      return NULL;
   }

   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         if (args->op_count == 1) {
            strcpy(ptype, args->argv[0]);
            PREPROCESS_name_convert(ptype);
            if (strcmp(args->operation[0], "typedef") == 0) {
               if (args->current_file) {
                  sprintf(dest, "TATTRIBUTETYPE_CONVERT "ATTRIBUTETYPECONVERT_NAME"_%s; ", ptype);
                  sprintf(dest + strlen(dest),
                         "TAttributeType "ATTRIBUTETYPEARRAY_NAME"_%s={sizeof(%s), "ATTRIBUTETYPECONVERT_NAME"_%s};",
                         ptype, args->argv[0], ptype);
                  sprintf(dest + strlen(dest),
                         "TAttributeType "ATTRIBUTETYPE_NAME"_%s={sizeof(%s), "ATTRIBUTETYPECONVERT_NAME"_%s,&"ATTRIBUTETYPEARRAY_NAME"_%s}",
                         ptype, args->argv[0], ptype, ptype);
                  }
               else {
                  sprintf(dest, "extern TATTRIBUTETYPE_CONVERT "ATTRIBUTETYPECONVERT_NAME"_%s; "
                          "extern TAttributeType "ATTRIBUTETYPE_NAME"_%s",
                          ptype, ptype);
                  sprintf(dest + strlen(dest),
                          ";extern TAttributeType "ATTRIBUTETYPEARRAY_NAME"_%s",
                          ptype, ptype);
               }
               return NULL;
            }
            if (strcmp(args->operation[0], "type") == 0) {
               sprintf(dest, ATTRIBUTETYPE_NAME"_%s", ptype);
               return NULL;
            }
            if (strcmp(args->operation[0], "typearray") == 0) {
               sprintf(dest, ATTRIBUTETYPEARRAY_NAME"_%s", ptype);
               return NULL;
            }
            else if (strcmp(args->operation[0], "convert") == 0) {
               sprintf(dest, ATTRIBUTETYPECONVERT_NAME"_%s", ptype);
               return NULL;
            }
         }
         else {
            sprintf(dest, "&"PARAMETER_NAME"_%s_%s", args->argv[0], args->argv[1]);
            return NULL;
         }
         return "bad method";
      default:
         break;
      }
   default:
      break;
   }
   return "error";
}/*PARAMETER_template*/

TTemplate tp_Attribute = {
   ATTRIBUTE_TEMPLATE_NAME,
   PARAMETER_template,
   TRUE,
};

TTemplate tp_Element = {
   ELEMENT_TEMPLATE_NAME,
   PARAMETER_template,
   TRUE,
};

TTemplate tp_Menu = {
   MENU_TEMPLATE_NAME,
   PARAMETER_template,
   TRUE,
};

TTemplate tp_Data = {
   DATA_TEMPLATE_NAME,
   PARAMETER_template,
   TRUE,
};


char *ALIAS_template(struct TEMPLATE *this,
                     EBlockType type, TTemplateArgs *args,
                     int dest_length, char *dest,
                     int extra_length, char *extra) {
   if (args->process_mode != PM_Code) {
      return NULL;
   }
                     
   /* add attribute to class if inside class */
   if (CLASS_in_class()) {
      strcpy(tclass.class_current->alias, args->argv[0]);
      if (args->argc >= 2) {
         strcpy(tclass.class_current->displayname, args->argv[1]);
      }
      *dest = '\0';
      tclass.remove_statement = TRUE;
      return NULL;
   }

   return "error";
}/*ALIAS_template*/

TTemplate tp_Alias = {
   ALIAS_TEMPLATE_NAME,
   ALIAS_template,
   TRUE,
};

char *OPTION_template(struct TEMPLATE *this,
                      EBlockType type, TTemplateArgs *args,
                      int dest_length, char *dest,
                      int extra_length, char *extra) {
   if (args->process_mode != PM_Code) {
      return NULL;
   }
                      
   /* add attribute to class if inside class */
   if (CLASS_in_class()) {
      tclass.class_current->option = 1;
      *dest = '\0';
      tclass.remove_statement = TRUE;
      return NULL;
   }

   return "error";
}/*OPTION_template*/

TTemplate tp_Option = {
   OPTION_TEMPLATE_NAME,
   OPTION_template,
   TRUE,
};

char *OBJECT_template(struct TEMPLATE *this,
                      EBlockType type, TTemplateArgs *args,
                      int dest_length, char *dest,
                      int extra_length, char *extra) {
   if (args->process_mode != PM_Code) {
      return NULL;
   }
                      
   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         sprintf(dest, "%s %s; "
                       "TObjectDef obj_def_%s_%s = {{%uUL, IT_ObjectDef},"
                                                   "(CObject *)&%s, "
                                                   "&class_%s, %s}",
                 args->argv[0], args->argv[1],
                 args->argv[0], args->argv[1], SID_INIT,
                 args->argv[1],
                 args->argv[0],
                 args->argc > 2 ? args->argv[2] : "0");
         return NULL;
      case TT_Single:
         return NULL;
      default:
         break;
      }
   default:
      break;
   }
   return "error";
}/*OBJECT_template*/

TTemplate tp_Object = {
   "OBJECT",
   OBJECT_template,
};

char *BASE_template(struct TEMPLATE *this,
                    EBlockType type, TTemplateArgs *args,
                    int dest_length, char *dest,
                    int extra_length, char *extra) {
   int i;

   if (args->process_mode != PM_Code) {
      return NULL;
   }

   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Object:
         sprintf(dest, "_virtual_%s_%s((void *)this",
                 tclass.class_current->base, args->operation[0]);

         for (i = 0; i < args->argc; i++) {
            sprintf(dest + strlen(dest), ", %s", args->argv[i]);
         }

         sprintf(dest + strlen(dest), ")");
         return NULL;
      default:
         break;
      }
   default:
      break;
   }
   return "error";
}/*BASE_template*/

TTemplate tp_Base = {
   "base",
   BASE_template,
};

char *NEW_template(struct TEMPLATE *this,
                   EBlockType type, TTemplateArgs *args,
                   int dest_length, char *dest,
                   int extra_length, char *extra) {
   int i;
   char *object, *obj_class;
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }

   /*>>>need to be able to return 'do nothing' option*/
   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Single:
         return NULL;
      default:
         if (tclass.in_class && !tclass.in_attribute)
            return NULL;

         if (!args->object)
            return "bad arguments";

         object    = args->object[0] == '\0' ? "0" : args->object;
         obj_class = args->operation[0];

         if (strcmp(obj_class, "class") == 0) {
            sprintf(dest, "CObject__construct((CObject *) %s, %s)",
                    object, args->argv[0]);
            return NULL;
         }

#if 0
         sprintf(dest, "(%s *) FRAMEWORK_last_object_set(CObject__construct((CObject *) %s, &class_%s)); "
                       "%s_%s(FRAMEWORK_last_object()",
                 obj_class, object, obj_class,
                 obj_class, obj_class);
#else
         sprintf(dest, "CONSTRUCT_%s(%s", obj_class, object);
//         sprintf(dest, "CONSTRUCT_%s((CObject *) %s);"
//                       "%s_%s(FRAMEWORK_last_object()",
//                 obj_class, object,
//                 obj_class, obj_class);
#endif                 
         if (args->argc) {
            for (i = 0; i < args->argc; i++) {
               sprintf(dest + strlen(dest), ", %s", args->argv[i]);
            }
         }
         sprintf(dest + strlen(dest), ")");
         return NULL;
      }
   default:
      break;
   }

   return "error";
}/*NEW_template*/

TTemplate tp_New = {
   "new",
   NEW_template,
};

char *DELETE_template(struct TEMPLATE *this,
                      EBlockType type, TTemplateArgs *args,
                      int dest_length, char *dest,
                      int extra_length, char *extra) {
   if (args->process_mode != PM_Code) {
      return NULL;
   }

   switch (type) {
   case BT_Template:
      if (tclass.in_class && !tclass.in_attribute)
         return NULL;

//>>>   if (!args->argc != 1)
//         return "bad arguments";

      sprintf(dest, "CObject__xdestroy((CObject *) %s)",
              args->argv[0]);
      return NULL;
   default:
      break;
   }

   return "error";
}/*DELETE_template*/

TTemplate tp_Delete = {
   "delete",
   DELETE_template,
};
