/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

#include "environ.h"

#include "keycc.h"
#include "template.h"

typedef char TTypeName[MAX_LINE];

typedef struct {
   int registered_type_count;
   TTypeName *registered_type;
} TContainer;

TContainer container;

Local void CONTAIN_type_register(char *type_name) {
   container.registered_type_count++;
   container.registered_type = realloc(container.registered_type,
                                       MAX_LINE * container.registered_type_count);
   strcpy(container.registered_type[container.registered_type_count - 1],
          type_name);
}/*CONTAIN_type_register*/

Local bool CONTAIN_type_registered(char *type_name) {
   int i;

   for (i = 0; i < container.registered_type_count; i++) {
      if (strcmp(container.registered_type[i], type_name) == 0) {
      return TRUE;
      }
   }

   return FALSE;
}/*CONTAIN_type_registered*/

typedef struct {
   int dummy;
} TList;

TList list;

char *LIST_template(struct TEMPLATE *this,
                    EBlockType type, TTemplateArgs *args,
                    int dest_length, char *dest,
                    int extra_length, char *extra) {
   char type_name[MAX_LINE];

   /*>>>parameter checking*/
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }
   
   *dest = '\0';
   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         sprintf(type_name, "type_LIST_%s", args->argv[0]);
                 PREPROCESS_name_convert(type_name);

         switch (args->op_count) {
         case 0:
            sprintf(dest, "%s", type_name);
            break;
         case 1:
            if (strcmp(args->operation[0], "typedef") == 0) {
               if (!CONTAIN_type_registered(type_name)) {
                     sprintf(dest, "typedef struct {"
                                      "union {"
                                         "TList list;"
                                         "struct {"
                                            "%s *first, *last;"
                                         "} tlist;"
                                      "} u;"
                                   "%s *result;"
                                   "} %s",
                             args->argv[0], args->argv[0], type_name);
               }
               CONTAIN_type_register(type_name);
            }
            else {
               return "bad definition operation";
               }
            break;
         default:
            return "bad definition argument count";
         }
         return NULL;
      case TT_Object:
         *dest = '\0';
         if (strcmp(args->operation[0], "new") == 0) {
            sprintf(dest, "LIST_new(&((%s)->u.list), %s)",
                    args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "delete") == 0) {
            sprintf(dest, "LIST_delete(&(%s)->u.list)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "add") == 0) {
            sprintf(dest, "LIST_item_add(&((%s)->u.list), &(%s)->base, 0, 0)",
                    args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "add_front") == 0) {
            sprintf(dest, "LIST_item_add(&((%s)->u.list), &(%s)->base, 0, 1)",
                    args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "insert") == 0) {
            sprintf(dest, "LIST_item_add(&((%s)->u.list), &(%s)->base, %s, 0)",
                    args->object, args->argv[0], args->argv[1]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "insert_front") == 0) {
            sprintf(dest, "LIST_item_add(&((%s)->u.list), &(%s)->base, %s, 1)",
                    args->object, args->argv[0], args->argv[1]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "remove") == 0) {
            sprintf(dest, "LIST_item_remove(&((%s)->u.list), &(%s)->base)",
                    args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "find") == 0) {
            sprintf(dest, "((%s)->result = (void *)LIST_item_find(&((%s)->u.list), %s))",
                    args->object, args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "find_next") == 0) {
            sprintf(dest, "((%s)->result = (void *)LIST_item_find_next(&((%s)->u.list), (TListItem *)((%s)->result = %s), %s))",
                    args->object, args->object, args->object, args->argv[0], args->argv[1]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "first") == 0) {
            sprintf(dest, "((%s)->u.tlist.first)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "n") == 0) {
            sprintf(dest, "((%s)->result = (void *)LIST_n_item(&((%s)->u.list), %s))",
                    args->object, args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "item_n") == 0) {
            sprintf(dest, "LIST_item_n(&((%s)->u.list), %s)",
                    args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "exists") == 0) {
            sprintf(dest, "LIST_item_exists(&((%s)->u.list), %s)",
                    args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "last") == 0) {
            sprintf(dest, "((%s)->u.tlist.last)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "next") == 0) {
            sprintf(dest, "((%s)->result = (void *)(((TListItem *)%s)->next))",
                    args->object, args->argv[0]);
//            sprintf(dest, "((%s)->result = (void *)((TListItem *)((%s)->result = %s))->next)",
//                    args->object, args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "previous") == 0) {
            sprintf(dest, "((%s)->result = (void *)(((TListItem *)%s)->previous))",
                    args->object, args->argv[0]);
//            sprintf(dest, "((%s)->result = (void *)((TListItem *)((%s)->result = %s))->previous)",
//                    args->object, args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "count") == 0) {
            sprintf(dest, "((%s)->u.list.count)", args->object);
            return NULL;
         }
         return "bad method";
      default:
         return "bad operation";
      }
   default:
      break;
   }
   return NULL;
}/*LIST_template*/

char *ARRAY_template(struct TEMPLATE *this,
                     EBlockType type, TTemplateArgs *args,
                     int dest_length, char *dest,
                     int extra_length, char *extra) {
   char type_name[MAX_LINE];

   /*>>>parameter checking*/
   /*>>>prevent unused methods depending on type (STACK, ARRAY)*/
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }
   
   *dest = '\0';
   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         sprintf(type_name, "type_ARRAY_%s", args->argv[0]);
         PREPROCESS_name_convert(type_name);

         switch (args->op_count) {
         case 0:
            sprintf(dest, "%s", type_name);
            break;
         case 1:
            if (strcmp(args->operation[0], "typedef") == 0) {
               if (!CONTAIN_type_registered(type_name)) {
                  sprintf(dest, "typedef union {"
                                   "TArray array;"
                                   "struct {"
                                      "ulong obj_sid;"
                                      "int element_size;"
                                      "int allocated, used;"
                                      "%s *data;"                
                                   "} t_array;"
                                "} %s",
                          args->argv[0], type_name);
               }
               CONTAIN_type_register(type_name);
            }
            else {
               return "bad definition operation";
            }
            break;
         default:
            return "bad definition argument count";
         }
         return NULL;
      case TT_Object:
         *dest = '\0';
         if (strcmp(args->operation[0], "new") == 0) {
            sprintf(dest, "ARRAY_new(&(%s)->array, sizeof(*(%s)->t_array.data))",
                    args->object, args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "delete") == 0) {
            sprintf(dest, "ARRAY_delete(&(%s)->array)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "copy") == 0) {
            sprintf(dest, "ARRAY_copy(&(%s)->array, &(%s)->array)", args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "element_size") == 0) {
            sprintf(dest, "((%s)->array.element_size)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "data") == 0) {
            sprintf(dest, "((%s)->t_array.data)",
                    args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "item_last") == 0) {
            sprintf(dest, "&((%s)->t_array.data)[(%s)->t_array.used - 1]",
                    args->object, args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "allocated") == 0) {
            sprintf(dest, "((%s)->array.allocated)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "count") == 0) {
            sprintf(dest, "((%s)->array.used)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "used_set") == 0) {
            sprintf(dest, "ARRAY_used_set(&(%s)->array, (%s))",
                    args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "data_set") == 0) {
            /*>>>implement 'insert', 'remove', change back to memcpy*/
            sprintf(dest, "{ ARRAY_used_set(&(%s)->array, (%s));"
                          "memmove((%s)->t_array.data,"
                          "(%s), (%s) * (%s)->array.element_size); }",
                    args->object, args->argv[0],
                    args->object,
                    args->argv[1], args->argv[0], args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "item_add") == 0 ||
                  strcmp(args->operation[0], "push") == 0) {
            /*>>>still not quite there*/
            sprintf(dest, "{ ARRAY_used_set(&(%s)->array, (%s)->array.used + 1);"
                          " ((%s)->t_array.data)"
                          "[(%s)->array.used - 1] = (%s); }",
                    args->object, args->object,
                    args->object, 
                    args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "item_add_front") == 0) {
            sprintf(dest, "{ ARRAY_used_set(&(%s)->array, (%s)->array.used + 1);"
                           "memmove(&(%s)->t_array.data[1], &(%s)->t_array.data[0], ((%s)->array.used - 1) * (%s)->array.element_size); "
                           " ((%s)->t_array.data)[0] = (%s); }",
                     args->object, args->object,
                     args->object, args->object, args->object, args->object,
                     args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "item_remove") == 0) {
            sprintf(dest, "{ ARRAY_item_remove(&(%s)->array, &(%s)); }",
                    args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "top") == 0) {
            sprintf(dest, "((%s)->t_array.data)"
                          "[(%s)->array.used - 1];",
                    args->object, 
                    args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "pop") == 0) {
            sprintf(dest, "((%s)->t_array.data)"
                          "[(%s)->array.used - 1];"
                          "ARRAY_used_set(&(%s)->array, (%s)->array.used - 1);",
                    args->object, 
                    args->object,
                    args->object, args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "item_add_empty") == 0) {
            /*>>>still not quite there*/
            sprintf(dest, "{ ARRAY_used_set(&(%s)->array, (%s)->array.used + 1);}",
                    args->object, args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "sort_type") == 0) {
            sprintf(dest, "{ ARRAY_sort_type(&(%s)->array, (%s), (%s)); }",
                    args->object, args->argv[0], args->argv[1]);
            return NULL;
         }
         return "bad method";
      default:
         return "bad operation";
      }
   default:
      break;
   }
   return NULL;
}/*ARRAY_template*/

char *STRING_template(struct TEMPLATE *this,
                      EBlockType type, TTemplateArgs *args,
                      int dest_length, char *dest,
                      int extra_length, char *extra) {
   char *sbegin, *send;
   int length;
   
   /*>>>parameter checking*/
   /*>>>prevent unused methods depending on type (STACK, ARRAY)*/
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }
   
   *dest = '\0';
   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         sbegin = strchr(args->argv[0], '\"');
         if (sbegin) {
            send = strchr(sbegin + 1, '\"');
         }
         if (sbegin && send) {
            /* Static string constant header, note, assume little endian */
            sprintf(dest, "(TString *)\042\\%03o\\%03o\\%03o\\%03o\\%03o\042", 
                     (unsigned int)(send - sbegin - 1 + 1) & 0xFF, (unsigned int)((send - sbegin - 1 + 1) >> 8) & 0xFF,
                     (unsigned int)(send - sbegin - 1 + 1) & 0xFF, (unsigned int)((send - sbegin - 1 + 1) >> 8) & 0xFF,
                     0);
            /* Static string constant copy.  >>> Handle Unicode translation */
            dest[strlen(dest) + send - sbegin + 1] = '\0';
            strncpy(dest + strlen(dest), sbegin, send - sbegin + 1);
         }
         else if (isdigit(args->argv[0][0])) {
            sprintf(dest, "struct {short blen; short ulen; char flags; char data[%d];}", atoi(args->argv[0]));   
         }
         else if (args->argv[0][0] == '*') {
            sprintf(dest, "TString");
         }
         else {
            sprintf(dest, "TString");
         }
         break;
      case TT_Object:
         *dest = '\0';
         if (strcmp(args->operation[0], "new") == 0) {
            sprintf(dest, "STRING_new(%s)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "delete") == 0) {
            sprintf(dest, "STRING_delete(%s)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "cstr") == 0) {
            /* >>>only works for static ASCII strings for now */
            sprintf(dest, "(const char *)(%s->data)", args->object);
            return NULL;
         }
         return "bad method";
      default:
         return "bad operation";
      }
   default:
      break;
   }
   return NULL;
}/*STRING_template*/

char *BITFIELD_template(struct TEMPLATE *this,
                        EBlockType type, TTemplateArgs *args,
                        int dest_length, char *dest,
                        int extra_length, char *extra) {
   /*>>>parameter checking*/
   /*>>>prevent unused methods depending on type (STACK, ARRAY)*/
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }
   
   *dest = '\0';
   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         sprintf(dest, "TBitfield");
         break;
      case TT_Object:
         *dest = '\0';
         if (strcmp(args->operation[0], "new") == 0) {
            sprintf(dest, "BITFIELD_new(%s)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "delete") == 0) {
            sprintf(dest, "BITFIELD_delete(%s)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "count") == 0) {
            sprintf(dest, "BITFIELD_count(%s)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "count_set") == 0) {
            sprintf(dest, "BITFIELD_count_set(%s, %s)", args->object, args->argv[0]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "set") == 0) {
            sprintf(dest, "BITFIELD_set(%s, %s, %s)", args->object, args->argv[0], args->argv[1]);
            return NULL;
         }
         else if (strcmp(args->operation[0], "get") == 0) {
            sprintf(dest, "BITFIELD_get(%s, %s)", args->object, args->argv[0]);
            return NULL;
         }
         return "bad method";
      default:
         return "bad operation";
      }
   default:
      break;
   }
   return NULL;
}/*BITFIELD_template*/

char *VECTOR_template(struct TEMPLATE *this,
                      EBlockType type, TTemplateArgs *args,
                      int dest_length, char *dest,
                      int extra_length, char *extra) {
   char type_name[MAX_LINE], *cp, c;
   int i;
   int init_element_count = 0;
   bool init_in_bracket = FALSE;
   bool init_in_quote = FALSE;

   /*>>>parameter checking*/
   /*>>>prevent unused methods depending on type (STACK, ARRAY)*/
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }
   
   *dest = '\0';
   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         sprintf(type_name, "type_VECTOR_%s", args->argv[0]);
         PREPROCESS_name_convert(type_name);

         switch (args->op_count) {
         case 0:
            switch (args->argc) {
            case 0:
               return "bad declaration";
            case 1:
            case 2:
               switch (args->argc_extra) {
               case 0:
               case 1:
                  /* declared */
                  sprintf(dest, "%s %s", type_name, args->argv_extra[0]);
                  break;
               case 2:
                  /* initalized */
                  
                  /* Parse initialzer (very incomplete!, borrow args buffer for derived output, not ideal)*/
                  cp = args->argv_extra[1];
                  while (*cp) {
                     switch (*cp) {
                     case '[':
                        init_in_bracket = TRUE;
                        init_element_count = 1;
                        *cp = '{';
                        break;
                     case ']':
                        init_in_bracket = FALSE;
                        *cp = '}';
                        break;
                     case ',':                        
                        if (init_in_bracket) {
                           init_element_count++;
                        }
                        break;
                     default:
                        break;
                     }
                     cp++;
                  }
                  
                  sprintf(dest, "struct %s {"
                                "%s *array_data;"
                                "int element_size;"
                                "int capacity, count;"
                                "ulong obj_sid;"
                                "%s data[%s];"
                                "} %s = {&%s.data[0], sizeof(%s.data[0]), %s, %d, SID_OBJ_ARRAY_STATIC, %s}", 
                                args->argv_extra[0], 
                                args->argv[0], 
                                args->argv[0], args->argv[1],
                                args->argv_extra[0], args->argv_extra[0], args->argv_extra[0], args->argv[1], init_element_count, args->argv_extra[1]);
                  break;
               default:                 
                  return "bad declaration";               
               }
               break;
            default:
               break;
            }
            break;
         case 1:
            if (strcmp(args->operation[0], "typedef") == 0) {
               if (!CONTAIN_type_registered(type_name)) {
                  sprintf(dest, "typedef struct {"
                                   "%s *array_data;"
                                   "int element_size;"
                                   "int capacity, used;"
                                   "ulong obj_sid;"
                                "} %s",
                          args->argv[0], type_name);
               }
               CONTAIN_type_register(type_name);
            }
            else {
               return "bad definition operation";
            }
            break;
         default:
            return "bad definition argument count";
         }
         return NULL;
      case TT_Object:
         *dest = '\0';
//         if (strcmp(args->operation[0], "new") == 0) {
//            sprintf(dest, "VECTOR_new(&%s, sizeof(*(%s)->t_array.data))",
//                    args->object, args->object);
//            return NULL;
//         }
         if (strcmp(args->operation[0], "capacity") == 0) {
            sprintf(dest, "((%s)->capacity)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "count") == 0) {
            sprintf(dest, "((%s)->count)", args->object);
            return NULL;
         }
         else if (strcmp(args->operation[0], "item") == 0) {
//            sprintf(dest, "((%s)->array_data[%s])", args->object, args->argv[0]);
            sprintf(dest, "(VECTOR_assert((TVector *)%s, %s, %s, \"VECTOR_item:bad index\") ? "
                          "((%s)->array_data[%s]) : ((%s)->array_data[%s]))",
                          args->object, args->argv[0], args->argv[0],
                          args->object, args->argv[0], args->object, args->argv[0]);
            return NULL;
         }
         return "bad method";
      default:
         return "bad operation";
      }
   default:
      break;
   }
   return NULL;
}/*VECTOR_template*/

TTemplate tp_List = {
   "LIST",
   LIST_template,
};

TTemplate tp_Array = {
   "ARRAY",
   ARRAY_template,
};

TTemplate tp_String = {
   "STRING",
   STRING_template,
};

TTemplate tp_Stack = {
   "STACK",
   ARRAY_template,
};

TTemplate tp_Bitfield = {
   "BITFIELD",
   BITFIELD_template,
};

TTemplate tp_Vector = {
   "VECTOR",
   VECTOR_template,
   FALSE, 
   TRUE
};
