/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "keycc.h"
#include "combine.h"

void COMBINE_new(TCombine *this) {
   memset(this, 0, sizeof(*this));
}

void COMBINE_delete(TCombine *this) {
}

void COMBINE_include_clear(TCombine *this) {
}/*COMBINE_include_clear*/

void COMBINE_include_add(TCombine *this, const char *include_path) {
  strcpy(this->search_path[this->path_count], include_path);
  this->path_count++;
}/*COMBINE_include_add*/

#if COMP_MSVC
static __inline char *realpath(char *src, char *dest) {
   char *cp;
   _fullpath(dest, src, _MAX_PATH);

   /*>>>kludge*/
   if (dest[1] == ':') {
      dest[0] = toupper(dest[0]);
   }

   cp = dest;
   while (*cp) {
      if (*cp == '\\') {
         *cp = '/';
      }
      cp++;
   }

   return dest;
}
#endif

void COMBINE_used_add(TCombine *this, const char *name) {
  char temp[MAX_LINE];
    
  if (this->used_count >= COMBINE_MAX_USED) {
     ASSERT("COMBINE_used_add: too many used files");
  }
  
  realpath((char *)name, temp);      

  strcpy(this->filename_used[this->used_count], temp);
  this->used_count++;
}/*COMBINE_used_add*/

bool COMBINE_used(TCombine *this, const char *name) {
   int i;
   char temp[MAX_LINE];

   realpath((char *)name, temp);

   for (i = 0; i < this->used_count; i++) {
      if (strcmp(this->filename_used[i], temp) == 0) {
         return TRUE;
      }
   }

   return FALSE;
}/*COMBINE_used*/

bool COMBINE_file_find(TCombine *this, char *name) {
   char result[MAX_LINE];
   char temp[MAX_LINE];
   FILE *file;
   int i;
   char *cp;

   file = fopen(name, "r");
   if (file) {
      fclose(file);
      return TRUE;
   }

   sprintf(result, "%s", this->include_file[this->include_count - 1].filename);
   cp = strrchr(result, '/');
   if (!cp) {
      cp = strrchr(result, '\\');
   }
   if (cp) {
      cp++;
      *cp = '\0';
      sprintf(cp + strlen(cp), "%s", name);
   }
   else {
      sprintf(result, "%s", name);
   }
   strcpy(temp, result);
   realpath(temp, result);

   file = fopen(result, "r");
   if (file) {
      fclose(file);
      strcpy(name, result);
      return TRUE;
   }

   for (i = 0; i < this->path_count; i++) {
      sprintf(result, "%s/%s", this->search_path[i], name);
      file = fopen(result, "r");
      if (file) {
         fclose(file);
         strcpy(name, result);
         return TRUE;
      }
   }

   return FALSE;
}/*COMBINE_file_find*/

bool COMBINE_file_open(TCombine *this, const char *name) {
   int i;

#if 0
   /*check open stack for re-including*/
   for (i = 0; i < this->include_count; i++) {
      if (strcmp(this->include_file[i].filename, name) == 0) {
         return FALSE;
      }
   }
#else
   if (COMBINE_used(this, name)) {
      return FALSE;
   }
   COMBINE_used_add(this, name);
#endif

   memset(&this->include_file[this->include_count], 0, sizeof(TCombineInclude));
   strcpy(this->include_file[this->include_count].filename, name);
   this->include_file[this->include_count].file = fopen(this->include_file[this->include_count].filename, "r");
   this->include_count++;

  if (this->include_count >= COMBINE_MAX_INCLUDE) {
     ASSERT("COMBINE_used_add: too many included files");
  }

   return TRUE;
}/*COMBINE_file_open*/

bool COMBINE_file_close(TCombine *this) {
   this->include_count--;
   fclose(this->include_file[this->include_count].file);

   return this->include_count != 0;
}/*COMBINE_file_close*/

bool COMBINE_process_line(TCombine *this) {
   TCombineInclude *inc = &this->include_file[this->include_count - 1];
   char buffer[MAX_LINE];
   char *cp, *cbegin, *cend;
   char include_file_name[MAX_LINE];
   bool include_file;

   if (fgets(buffer, MAX_LINE, inc->file)) {
      inc->parent_line++;

      if (this->stripcomment) {
         /* strip comments */
         cp = buffer;
         while (cp) {
            if (cp[0] == '/' && cp[1] == '/') {
               cp[0] = '\n';
               cp[1] = '\0';
               break;
            }
            cp++;
            cp = strchr(cp, '/');
         }

         cp = buffer;
         while (*cp) {
            if (cp[0] == '/' && cp[1] == '*') {
               this->incomment = TRUE;
               cp[0] = ' ';
               cp[1] = ' ';
               cp += 2;
            }

            if (cp[0] == '*' && cp[1] == '/') {
               this->incomment = FALSE;
               cp[0] = ' ';
               cp[1] = ' ';
            }

            if (this->incomment && *cp != '\n') {
               *cp = ' ';
            }

            cp++;
         }
      }

      /* strip leading whitespace */
      cp = buffer;
      while (*cp == ' ') {
         cp++;
      }
      if (*cp == '\n') {
         buffer[0] = '\n';
         buffer[1] = '\0';
      }

      include_file = FALSE;
      if ((strncmp(buffer, "#include", 8) == 0) && !strchr(buffer, '<')) {
         cp = &buffer[10];
         cbegin = cp;
         cp = strrchr(cp, '.');

         if (cp) {
            cend = cp;
            cend++;
            while (*cend && !isalnum(*cend)) {
               cend++;
            }
            memset(include_file_name, 0, MAX_LINE);
            memcpy(include_file_name, cbegin, cend - cbegin + 1);
         }

         if (cp && cp[1] == 'c' && !this->noinclude) {
            if (!COMBINE_file_find(this, include_file_name)) {
               printf("COMBINE_process_name: include file '%s' not found\n", include_file_name);
               exit(1);
            }
            if (COMBINE_file_open(this, include_file_name)) {
               fprintf(this->outfile, "#%s 1 \"%s\"\n", this->lineprefix, include_file_name);
            }
            else {
               fprintf(this->outfile, "\n");
            }
            include_file = TRUE;
         }
         else if (cp) {
            COMBINE_file_find(this, include_file_name);
            sprintf(buffer, "#include \"%s\"\n", include_file_name);
         }
      }
      if (!include_file) {
         fputs(buffer, this->outfile);
      }

      return TRUE;
   }
   else {
      if (COMBINE_file_close(this)) {
         inc = &this->include_file[this->include_count - 1];
         fprintf(this->outfile, "\n#%s %d \"%s\"\n", this->lineprefix, inc->parent_line + 1, inc->filename);
         return TRUE;
      }
   }

   return FALSE;
}/*COMBINE_process_line*/

void COMBINE_process(TCombine *this, ECCompiler ccompiler, bool noinclude, bool stripcomment, const char *infile, const char *outfile) {
   this->stripcomment = stripcomment;
   this->ccompiler = ccompiler;
   this->noinclude = noinclude;
   switch (this->ccompiler) {
   case CC_cl:
      this->lineprefix ="line";
      break;
   default:
      this->lineprefix = "";
      break;
   }

   this->outfile = fopen(outfile, "w");

   if (!noinclude) {
      fprintf(this->outfile, "#%s 1 \"%s\"\n", this->lineprefix, infile);
   }

   COMBINE_file_open(this, infile);

   while (COMBINE_process_line(this)) {}

   fclose(this->outfile);
}/*COMBINE_process*/
