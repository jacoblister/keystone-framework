/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/
#include "environ.h"

#include "keycc.h"
#include "template.h"

typedef enum {
   MM_None,
   MM_Import,
   MM_Interface,
   MM_Implementation,
   MM_End,
} EFileModuleMode;

typedef struct {
   char name[MAX_LINE];
   EFileModuleMode mode;
} TFileModule;

typedef struct {
   char *cur_filename;
   bool current_file;
   bool hide;
   int file_module_count;
   TFileModule *file_module;
} TModule;

TModule module;

Local TFileModule *MODULE_file_module_find(char *file_module_name) {
   TFileModule *file_module = module.file_module;
   int i;

   for (i = 0; i < module.file_module_count; i++, file_module++) {
      if (strcmp(file_module->name, file_module_name) == 0) {
         return file_module;
      }
   }

   return NULL;
}/*MODULE_file_module_find*/

Local TFileModule *MODULE_file_module_add(char *file_module_name) {
   TFileModule *file_module;

   module.file_module_count++;
   module.file_module = realloc(module.file_module, sizeof(TFileModule) * module.file_module_count);

   file_module = &module.file_module[module.file_module_count - 1];
   CLEAR(file_module);
   strcpy(file_module->name, file_module_name);
   file_module->mode = MM_None;

   return file_module;
}/*MODULE_file_module_add*/

Local void MODULE_mode_change(EFileModuleMode mode) {
   switch (mode) {
   case MM_None:
   case MM_Import:
   case MM_Interface:
      module.hide = FALSE;
      break;
   case MM_Implementation:
      module.hide = !module.current_file;
      break;
   case MM_End:
      module.hide = TRUE;
      break;
   }
}/*MODULE_mode_change*/

Global void MODULE_current_file(char *cur_filename, bool current_file) {
   TFileModule *file_module;
   module.cur_filename = cur_filename;
   module.current_file = current_file;

   file_module = MODULE_file_module_find(module.cur_filename);
   if (file_module) {
      MODULE_mode_change(file_module->mode);
   }
}/*MODULE_current_file*/

Global char *MODULE_template(struct TEMPLATE *this,
                             EBlockType type, TTemplateArgs *args,
                             int dest_length, char *dest,
                             int extra_length, char *extra) {
   TFileModule *file_module;
   
   if (args->process_mode != PM_Code) {
      *dest = '\0';
      return NULL;
   }

   switch (type) {
   case BT_Template:
      switch (args->type) {
      case TT_Definition:
         *dest = '\0';

         file_module = MODULE_file_module_find(module.cur_filename);
         if (!file_module) {
            file_module = MODULE_file_module_add(module.cur_filename);
         }

         if (file_module->mode != MM_End) {
            if (strcmp(args->operation[0], "IMPORT") == 0) {
               file_module->mode = MM_Import;
               module.hide = FALSE;
            }
            else if (strcmp(args->operation[0], "INTERFACE") == 0) {
               file_module->mode = MM_Interface;
               module.hide = FALSE;
            }
            else if (strcmp(args->operation[0], "IMPLEMENTATION") == 0) {
               file_module->mode = MM_Implementation;
               module.hide = !args->current_file;
            }
            else if (strcmp(args->operation[0], "END") == 0) {
               file_module->mode = MM_End;
            }
            else {
               return "bad operation";
            }
            MODULE_mode_change(file_module->mode);
         }
         break;
      default:
         return "bad form";
      }
      break;
   case BT_Statement:
      if (module.hide) {
         *dest = '\0';
         *extra = '\0';
         }
      break;
   case BT_Separator:
      *dest = '\0';
      break;
   default:
      return "error";
   }

   return NULL;
}/*MODULE_template*/

TTemplate tp_Module = {
   "MODULE",
   MODULE_template,
};
