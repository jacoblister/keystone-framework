/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

#include "environ.h"

#include "keycc.h"
#include "template.h"

typedef struct {
   int depth;
} TException;

TException exception;

char *TRY_template(struct TEMPLATE *this,
                  EBlockType type, TTemplateArgs *args,
                  int dest_length, char *dest,
                  int extra_length, char *extra) {
   /*>>>parameter checking*/                  
   if (args->process_mode != PM_Code) {
      return NULL;
   }

   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Single:
#if 0
         strcpy(dest, "{TExBlock block; EX_block_add(&block); if (EX_setjmp(&block.context) == EX_Code) ");
#else
//         strcpy(dest, "{TExBlock block; EX_block_add(&block); if (_setjmp(block.context) == EX_Code) ");
         strcpy(dest, "{TExBlock block; EX_block_add(&block); _setjmp(block.context); if (block.state == EX_Code) ");
#endif
         return NULL;
      default:
         return "error";
      }
   default:
      break;
   }
   return NULL;
}/*TRY_template*/

char *CATCH_template(struct TEMPLATE *this,
                     EBlockType type, TTemplateArgs *args,
                     int dest_length, char *dest,
                     int extra_length, char *extra) {
   /*>>>parameter checking*/
   int i;

   if (args->process_mode != PM_Code) {
      return NULL;
   }

   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         sprintf(dest, "EX_exception(&block); "
                       "if (block.state == EX_Handling && EX_exception_test(&block");
         dest += strlen(dest);

         for (i = 0; i < args->argc; i++) {
            sprintf(dest, ", %s", args->argv[i]);
            dest += strlen(dest);
         }
         sprintf(dest + strlen(dest), ")) ");
         return NULL;
      default:
         return "error";
         }
      break;
   case BT_BlockBegin:
      exception.depth++;
      return NULL;
   case BT_BlockEnd:
      exception.depth--;
      return NULL;
   case BT_BlockEndPre:
      if (exception.depth == 1) {
         strcpy(dest, "EX_handled();");
      }
      return NULL;
   default:
      break;
   }

   return NULL;
}/*CATCH_template*/

char *FINALLY_template(struct TEMPLATE *this,
                       EBlockType type, TTemplateArgs *args,
                       int dest_length, char *dest,
                       int extra_length, char *extra) {
   /*>>>parameter checking*/

   if (args->process_mode != PM_Code) {
      return NULL;
   }
   
   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Single:
#if 0
         strcpy(dest, "if (EX_setjmp(&block.context) == EX_Finally) ");
#else
         strcpy(dest, "if (_setjmp(block.context) == EX_Finally) ");
#endif
         return NULL;
      default:
         return "error";
      }
      break;
   case BT_BlockEndPre:
      strcpy(dest, "} EX_block_remove(&block);");
      return NULL;
   default:
      break;
   }

   return NULL;
}/*FINALLY_template*/

char *THROW_template(struct TEMPLATE *this,
                     EBlockType type, TTemplateArgs *args,
                     int dest_length, char *dest,
                     int extra_length, char *extra) {
   /*>>>parameter checking*/
   int i;
   
   if (args->process_mode != PM_Code) {
      return NULL;
   }

   switch (type) {
   case BT_Template:
      sprintf(dest, "EX_throw(");
      dest += strlen(dest);

      for (i = 0; i < args->argc; i++) {
         if (i != 0)
            sprintf(dest, ",");
         dest += strlen(dest);
         sprintf(dest, "%s", args->argv[i]);
         dest += strlen(dest);
      }
      sprintf(dest, ")");
      return NULL;
   default:
      break;
   }

   return "error";
}/*THROW_template*/

char *EXCEPTION_template(struct TEMPLATE *this,
                         EBlockType type, TTemplateArgs *args,
                         int dest_length, char *dest,
                         int extra_length, char *extra) {
   /* no template translation if inside a class definition */
   if (args->process_mode != PM_Code) {
      return NULL;
   }
   
   if (CLASS_in_class())
      return NULL;

   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Single:
         return NULL;
      default:
         if (args->argc > 2)
            return "bad arguments";

         if (args->argc >= 1) {
            sprintf(dest, "&class(%s), ", args->argv[0]);
         }
         else {
            sprintf(dest, "0, ");
         }

         if (args->argc == 2) {
            sprintf(dest + strlen(dest), "%s_exception_%s",
                    args->argv[0], args->argv[1]);
         }
         else {
            sprintf(dest + strlen(dest), "-1");
         }
         return NULL;
      }
   default:
      break;
   }
   return "error";
}/*EXCEPTION_template*/

TTemplate tp_Try = {
   "try",
   TRY_template,
};

TTemplate tp_Catch = {
   "catch",
   CATCH_template,
};

TTemplate tp_Finally = {
   "finally",
   FINALLY_template,
};

TTemplate tp_Throw = {
   "throw",
   THROW_template,
};

TTemplate tp_Exception = {
   EXCEPTION_NAME,
   EXCEPTION_template,
};
