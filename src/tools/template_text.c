/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

#include "environ.h"
#include "../base/text.h"
#include <ctype.h>

#include "keycc.h"
#include "template.h"

#define MAX_ARG             100
#define ENUM_ITEM_MAX       500
#define TXENUM_MAX          1000

#define ENUM_PREFIX          "_enum__"
#define ENUMNAME_PREFIX       "_enumname__"
#define ENUM_COUNT           "COUNT"

typedef struct {
   char id[MAX_ARG];
   char name[MAX_ARG];
   char description[MAX_ARG];
} TTxEnumItem;

typedef struct {
   char name[MAX_ARG];
   int item_count;
   TTxEnumItem item[ENUM_ITEM_MAX];
   TTemplate template;
} TTxEnum;

typedef struct {
   int dummy;
   int txenum_count;
   TTxEnum txenum[TXENUM_MAX];
   TTxEnum *txenum_current;
   int depth;
} TText;

TText text;

Local TTxEnum *TEXT_enum_find(char *name) {
   int i;
   TTxEnum *ep = text.txenum;

   for (i = 0; i < text.txenum_count; i++) {
      if (strcmp(ep->name, name) == 0)
         return ep;
      ep++;
  }

   return NULL;
}/*TEXT_enum_find*/

Global int TEXT_enum_count(char *name) {
   TTxEnum *ep = TEXT_enum_find(name);

   if (ep) {
      return ep->item_count;
   }

   return 0;
}/*TEXT_enum_count*/

char *ENAME_template(struct TEMPLATE *this,
                     EBlockType type, TTemplateArgs *args,
                     int dest_length, char *dest,
                     int extra_length, char *extra) {
   TTxEnum *ep = TEXT_enum_find(this->name);
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }

   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Single:
         return NULL;
      case TT_Object:
         if (strlen(args->object)) {
            if (strcmp(args->operation[0], "name") == 0) {
               sprintf(dest, ENUMNAME_PREFIX"%s[%s]", this->name, args->object);
               return NULL;
               }
            }
          else {
             if (strcmp(args->operation[0], ENUM_COUNT) == 0) {
                sprintf(dest, "%d", ep->item_count);
             }
             else {
                sprintf(dest, ENUM_PREFIX"%s_%s", this->name, args->operation[0]);
             }
             return NULL;
          }
          break;
      default:
          break;
      }
      return "error";
   default:
      break;
   }

   return NULL;
}/*ENAME_template*/

char *ENUM_template(struct TEMPLATE *this,
                    EBlockType type, TTemplateArgs *args,
                    int dest_length, char *dest,
                    int extra_length, char *extra) {
   char *cp;
   TTxEnumItem *item;
   int i;
   TTxEnum *ep;
   
//   if (args->process_mode != PM_Code) {
//      sprintf(dest, "<enum/>");      
//      return NULL;
//   }

   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         if (!args->op_count) {
            return NULL;         
         }
         
         if (strcmp(args->operation[0], "typedef") == 0) {
            if (TEXT_enum_find(args->argv[0])) {
               return NULL; //>>>"duplicate definition";
            }
            text.txenum_count++;
            text.txenum_current = &text.txenum[text.txenum_count - 1];
            memset(text.txenum_current, 0, sizeof(TTxEnum));
            strcpy(text.txenum_current->name, args->argv[0]);
            text.depth = 0;
            sprintf(dest, "typedef enum");

            /* Register new enum as template */
            memset(&text.txenum_current->template, 0, sizeof(text.txenum_current->template));
            strcpy(text.txenum_current->template.name, text.txenum_current->name);
            text.txenum_current->template.template_fn = ENAME_template;
            PREPROCESS_template_register(&preprocess, &text.txenum_current->template);
            return NULL;
         }
         if (args->process_mode == PM_Code) {
            if (strcmp(args->operation[0], "type") == 0) {
               sprintf(dest, ENUM_PREFIX"%s", args->argv[0]);
               return NULL;
            }
            if (strcmp(args->operation[0], "decode") == 0) {
               ep = TEXT_enum_find(args->argv[0]);
               if (!ep) {
                  return "referenced undefined ENUM";
               }
               sprintf(dest, "ENUM_decode(%d, "ENUMNAME_PREFIX"%s, %s)",
                       ep->item_count, ep->name, args->argv[1]);
               return NULL;
            }
            return "bad method";
         }
         return NULL;
      default:
         break;
      }
      return "error";
   case BT_BlockBegin:
      text.depth++;
      if (text.depth > 1) {
         *dest = '\0';
      }
      return NULL;
   case BT_BlockEndPre:
      return NULL;
   case BT_BlockEndPost:
      if (text.depth == 0) {
         sprintf(dest, " %s", text.txenum_current->name);
         if (args->process_mode == PM_Code) {
            if (args->current_file) {
               sprintf(extra, "const char *"ENUMNAME_PREFIX"%s[] = {\n",
                       text.txenum_current->name);
               for (i = 0; i < text.txenum_current->item_count; i++) {
                  item = &text.txenum_current->item[i];
                  if (strlen(item->name)) {
                     sprintf(extra + strlen(extra), "%s", item->name);
                  }
                  else {
                     sprintf(extra + strlen(extra), "\"%s\"", item->id);
                  }
                  sprintf(extra + strlen(extra), ",\n");
               }
               sprintf(extra + strlen(extra), "};\n\n");

               sprintf(extra + strlen(extra), "TEnum "ENUM_PREFIX"%s = {%d, "ENUMNAME_PREFIX"%s};\n",
                       text.txenum_current->name,
                       text.txenum_current->item_count, text.txenum_current->name);
            }
            else {
               sprintf(extra, "extern const char *"ENUMNAME_PREFIX"%s[];\n",
                       text.txenum_current->name);
            }
         }
      }
      return NULL;
   case BT_BlockEnd:
      if (text.depth > 1) {
         *dest = '\0';
      }
      text.depth--;
      return NULL;
   case BT_Statement:
      if (text.depth == 2) {
         item = &text.txenum_current->item[text.txenum_current->item_count];
         text.txenum_current->item_count++;

         cp = strtok(args->operation[0], ",");
         if (cp) {
            /*id*/
            strcpy(item->id, cp);
         }
         cp = strtok(NULL, ",");
         if (cp) {
            /*name*/
            strcpy(item->name, cp);
         }
         cp = strtok(NULL, ",");
         if (cp) {
            /*description*/
            strcpy(item->description, cp);
         }
         switch (args->process_mode) {
         case PM_Code:
            sprintf(dest, ENUM_PREFIX"%s_%s", text.txenum_current->name, item->id);
            break;
         case PM_Documentation:
            if (strlen(item->name)) {
               sprintf(dest, "%s /*!<(<i>aka %s</i>) - */", item->id, item->name);
            }
            else { 
               sprintf(dest, "%s", item->id);
            }
            break;
         }
      }
      return NULL;
   case BT_Separator:
      return NULL;
   default:
      break;
   }
   return NULL;
}/*ENUM_template*/

char *TEXT_template(struct TEMPLATE *this,
                    EBlockType type, TTemplateArgs *args,
                    int dest_length, char *dest,
                    int extra_length, char *extra) {
   char *cp;
   TTxEnumItem *item;
   int i;
   TTxEnum *ep;
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }

   switch (type) {
   case BT_Template:
      if (args->argc) {
         sprintf(dest, "L%s", args->argv[0]);
      }
      break;
   default:
      break;
   }
   return NULL;
}/*TEXT_template*/

TTemplate tp_Enum = {
   "ENUM",
   ENUM_template,
   TRUE,
};

TTemplate tp_Text = {
   "TEXT",
   TEXT_template,
   TRUE,
};

