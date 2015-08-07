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
#include "template.h"
#include "combine.h"
#include "configparser.h"

#define TEMP_FILE_NAME_IN  "__tempin__.c"
#define TEMP_FILE_NAME_OUT "__tempout__.c"
#define BUF_SIZE (MAX_LINE * 100)
/*>>>need a better scheme for buffer allocation and pasing to templates*/

#define TEMPLATE_MAX 100
#define BLOCK_DEPTH_MAX 50

#define STDOUT 1

/*misc*/
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

Global void PREPROCESS_name_convert(char *name) {
   while (*name) {
      switch (*name) {
      case ' ':
         *name = '_';
          break;
      case '*':
         *name = 'p';
         break;
      default:
         break;
      }
   name++;
   }
}/*PREPROCESS_name_convert*/

Global bool ismchar(char c) {
   return isalnum(c) || c == '~' || c == '_';
}

Global char *strmatch(char *str) {
   int depth = 0;
   char c_begin, c_end;
   bool inquote = FALSE;
   bool incomment = FALSE;

   switch (*str) {
   case '(':
      c_begin = '(';
      c_end   = ')';
      break;
   case '<':
      c_begin = '<';
      c_end = '>';
      break;
   default:
      ASSERT("strmatch:subfunction not implemented");
   }

   while (*str) {
     /*>>>still not quite there*/
     if (*str == '\"' || *str == '\'') {
        inquote = !inquote;
     }
     if (!inquote && str[0] == '*' && str[1] == '/') {
        incomment = TRUE;
     }
     if (!inquote && str[0] == '/' && str[1] == '*') {
        incomment = FALSE;
     }

     if (!inquote && !incomment) {
         if (*str == c_begin) {
            depth++;
         }
         else if (*str == c_end) {
            depth--;
            if (depth <= 0) {
               return str;
            }
         }
      }
      str++;
   }

   return NULL;
}/*strmatch*/

typedef struct {
   int count;
   TTemplate **template;
} TTemplateList;

void TPLIST_add(TTemplateList *this, TTemplate *template) {
   this->count++;
   this->template = realloc(this->template, this->count * sizeof(TTemplate *));
   this->template[this->count - 1] = template;
}/*TPLIST_add*/

int TPLIST_count(TTemplateList *this) {
   return this->count;
}/*TPLIST_count*/

TTemplate *TPLIST_template_n(TTemplateList *this, int n) {
   return this->template[n];
}/*TPLIST_template_n*/

TTemplate *TPLIST_find(TTemplateList *this, char *name) {
   int i;

   for (i = 0; i < this->count; i++) {
      if (strcmp(name, this->template[i]->name) == 0)
         return this->template[i];
   }

   return NULL;
}/*TPLIST_find*/

typedef struct {
   int allocated;
   int used;
   char *buf;
} TFileBuffer;

void FBUF_new(TFileBuffer *this) {
   memset(this, 0, sizeof(this));
   this->allocated = 10000000; //100000;
   this->buf = malloc(this->allocated);
}/*FBUF_new*/

static inline char *FBUF_buf(TFileBuffer *this) {
   return this->buf;
}/*FBUF_buf*/

static inline bool FBUF_empty(TFileBuffer *this) {
   return strlen(this->buf) == 0;
}/*FBUF_empty*/

static void FBUF_clear(TFileBuffer *this) {
   *this->buf = '\0';
   this->used = 0;
}/*FBUF_clear*/

static void FBUF_append(TFileBuffer *this, char *data) {
   int size;

   if (!data)
      return;

   size = strlen(data);

   if (this->used + size > this->allocated) {
      this->allocated += 100000;
      this->buf = realloc(this->buf, this->allocated);
      if (!this->buf) {
         printf("keycc: memory allocation error\n");
         exit(1);
      }
   }

   strcpy(this->buf + this->used, data);

   this->used += size;
}/*FBUF_append*/

static void FBUF_file_write(TFileBuffer *this, FILE *file) {
   int count, written = 0, remain = this->used;

   while (remain) {
      count = fwrite(this->buf + written, 1, remain, file);
      remain -= count;
      written += count;
   }
}/*FBUF_file_write*/

static void FBUF_delete(TFileBuffer *this) {
   free(this->buf);
}/*FBUF_delete*/

typedef struct PREPROCESS {
   ECCompiler ccompiler;
   char input_filename[MAX_LINE];
   char cur_filename[MAX_LINE];
   bool current_file;
   int  cur_line;
   TTemplateList template;
   TTemplate *template_current;
   TFileBuffer buf_output;
   TFileBuffer buf_extra;
   bool pending_flush_extra;
   int block_depth;
   TTemplate *template_block[BLOCK_DEPTH_MAX];
   TCombine combine;
   bool inquote;
   bool inblockcomment;
   bool inlinecomment;
   struct {
      char buffer[BUF_SIZE];
//      int block_depth;
   } statement;

   EProcessMode process_mode;
   bool opt_noinclude;
} TPreProcess;

static void PREPROCESS_new(TPreProcess *this) {
   memset(this, 0, sizeof(*this));

   COMBINE_new(&this->combine);
}/*PREPROCESS_new*/

static void PREPROCESS_delete(TPreProcess *this) {
   COMBINE_delete(&this->combine);
}/*PREPROCESS_delete*/

Global ECCompiler PREPROCESS_CCompiler(TPreProcess *this) {
   return this->ccompiler;
}/*PREPROCESS_CCompiler*/

Global char *PREPROCESS_current_filename(TPreProcess *this) {
   return this->cur_filename;
}/*PREPROCESS_current_filename*/

Local int PREPROCESS_nl_count(TPreProcess *this, char *str) {
   int nl_count = 0;
   while (*str) {
      if (*str == '\n') {
         nl_count++;
      }
      str++;
   }

   return nl_count;
}/*PREPROCESS_nl_count*/

Local void PREPROCESS_nl_append(TPreProcess *this, char *str, int nl_count) {
   int nl_count_current = PREPROCESS_nl_count(this, str);
   while (nl_count_current < nl_count) {
      sprintf(str + strlen(str), "\n");
      nl_count_current++;
   }
}/*PREPROCESS_nl_append*/;

int PREPROCESS_cobjclass_offset(TPreProcess *this) {
   switch (this->ccompiler) {
   case CC_m68k_palmos_gcc:
      return 10; /*>>> - sizeof(int) */
   default:
      return sizeof(void *) + sizeof(void *);
   }
}/*PREPROCESS_cobjclass_offset*/

int PREPROCESS_cobjclass_vtbl_offset(TPreProcess *this) {
   switch (this->ccompiler) {
   case CC_m68k_palmos_gcc:
      return 10;
   default:
      return sizeof(int) + sizeof(int) + sizeof(void *);
   }
}/*PREPROECSS_cobjclass_vtbl_offset*/

void PREPROCESS_lineinfo_print(TPreProcess *this) {
   printf("keycc: file %s, line %d\n", this->cur_filename, this->cur_line);
}/*PREPROCESS_lineinfo_print*/

void PREPROCESS_template_register(TPreProcess *this, TTemplate *template) {
   TPLIST_add(&this->template, template);
}/*PREPROCESS_template_register*/

static inline PREPROCESS_block_check_comment(TPreProcess *this, char *cp, bool *inquote, bool *incomment) {
   if (*cp == '\"') {
      (*inquote) = !(*inquote);
   }
   if (!(*inquote) && cp[0] == '*' && cp[1] == '/') {
      (*incomment) = TRUE;
   }
   if (!(*inquote) && cp[0] == '/' && cp[1] == '*') {
     (*incomment) = FALSE;
   }
}

static TTemplate *PREPROCESS_block_template_find(TPreProcess *this, char *input,
                                                 char **t_start, char **t_end, TTemplateArgs *args) {
   /*search for template in block*/
   TTemplate *template = NULL;
   char *cp , *op_ptr;
   int i, j;
   bool inquote   = FALSE;
   bool incomment = FALSE;
   bool inbracket = FALSE;
   bool has_extra = FALSE;

   cp = input;

   *t_start = NULL;
   CLEAR(args);

   while (*cp && !*t_start) {
      PREPROCESS_block_check_comment(this, cp, &inquote, &incomment);

      if (!inquote && !incomment) {
         for (i = 0; i < TPLIST_count(&this->template) && !*t_start; i++) {
            template = TPLIST_template_n(&this->template, i);
            *t_start = strncmp(cp, template->name, strlen(template->name)) == 0 ? cp : NULL;
            if (*t_start && *t_start > input && (ismchar(*(*t_start - 1)) || *(*t_start - 1) == '_'))
               *t_start = NULL;
            *t_end = *t_start + strlen(template->name);
            if (*t_start && *t_end <= input + strlen(input) && (ismchar(**t_end) || **t_end == '_'))
               *t_start = NULL;
         }
      }

      while (*cp && isalnum(*cp)) {
         cp++;
      }
      while (*cp && !isalnum(*cp)) {
         PREPROCESS_block_check_comment(this, cp, &inquote, &incomment);
         cp++;
      }
   }
   cp = input;

   if (*t_start) {
      args->type = TT_Definition;

      op_ptr = args->buf_operation;
      cp = *t_start + strlen(template->name);
      *t_end = cp - 1;

      if (*cp == ' ') {
         if (!template->complex) {
            args->type = TT_Single;
            return template;
         }

         *t_end = cp + 1;
         while (ismchar(**t_end))
            (*t_end)++;
         strncpy(op_ptr, cp + 1, *t_end - cp - 1);
         op_ptr[*t_end - cp - 1] = '\0';

         args->operation[args->op_count] = op_ptr;
         op_ptr += strlen(op_ptr) + 1;
         args->op_count++;
         cp = *t_end;
         (*t_end)--;
      }
      while (*cp == ' ') {
         cp++;
      }
      if (*cp == ':') {
         while (*cp == ':') {
            cp++;
         }
         while (*cp == ' ') {
             cp++;
         }

         *t_end = cp;
         while (ismchar(**t_end))
            (*t_end)++;

         strncpy(op_ptr, cp, *t_end - cp);
         op_ptr[*t_end - cp] = '\0';

         args->operation[args->op_count] = op_ptr;
         op_ptr += strlen(op_ptr + 1);
         args->op_count++;

         cp = *t_end;
         (*t_end)--;
      }

      if (*cp == '.') {
         cp++;
         *t_end = cp;
         while (ismchar(**t_end)) {
            (*t_end)++;
         }

         args->buf_object[0] = '\0';
         args->object = args->buf_object;

         strncpy(args->buf_operation, cp, *t_end - cp);
         args->buf_operation[*t_end - cp] = '\0';
         args->op_count     = 1;
         args->operation[0] = args->buf_operation;

         args->type = TT_Object;
         cp = *t_end;
         (*t_end)--;
      }

      if (*cp && strchr("<(", *cp)) {
         *t_end = strmatch(cp);
         if (!(*t_end)) {
            PREPROCESS_lineinfo_print(this);
            ASSERT("Incomplete bracketing\n");
         }

         if (*(*t_end + 1) == '.') {
            strncpy(args->buf_object, cp + 1, *t_end - cp - 1);
            args->buf_object[*t_end - cp - 1] = '\0';

            (*t_end) += 2;
            cp = *t_end;
            while(ismchar(**t_end)) {
               (*t_end)++;
            }
            if (**t_end == '(') {
               strncpy(args->buf_operation, cp, *t_end - cp);
               args->buf_operation[*t_end - cp] = '\0';

               cp = strchr(cp + 1, '(');
               *t_end = strmatch(cp);
               if (!(*t_end)) {
                  PREPROCESS_lineinfo_print(this);
                  ASSERT("Incomplete bracketing\n");
               }
            }

            args->object       = args->buf_object;
            args->op_count     = 1;
            args->operation[0] = args->buf_operation;
            args->type = TT_Object;
         }

         strncpy(args->buf_args, cp + 1, *t_end - cp - 1);
         args->buf_args[*t_end - cp - 1] = '\0';

         j = 0;
         args->argv[0] = strtok(args->buf_args, ",");
         while (args->argv[j]) {
            while (*args->argv[j] == ' ') {
               args->argv[j]++;
            }
            j++;
            args->argv[j] = strtok(NULL, ",");
         }
         args->argc = j;
      }
      else if (args->argc == 0 && args->op_count == 0) {
         args->type = TT_Single;
      }

      if (template->extra_arg && (*(*t_end + 1)) != 0 && args->type == TT_Definition) {
         (*t_end)++;

         cp = *t_end;
         while ((*(*t_end + 1)) != 0) {
            if (!inbracket && ((*(*t_end + 1)) == ',' || (*(*t_end + 1)) == ')')) {
               break;
            }
            if (!isspace(**t_end)) {
               has_extra = TRUE;
            }
            if (**t_end == '[') {
               inbracket = TRUE;
            }
            if (**t_end == ']') {
               inbracket = FALSE;
            }
            (*t_end)++;
         }
        if (!has_extra) {
           args->argc_extra = 0;
        }
        else {
            memcpy(args->buf_args_extra, cp, *t_end - cp + 1);
            j = 0;
            args->argv_extra[0] = strtok(args->buf_args_extra, "=");
            while (args->argv_extra[j]) {
               /* strip leading and trailing whitespace */
               while (isspace(*args->argv_extra[j])) {
                  args->argv_extra[j]++;
               }
               while (isspace(args->argv_extra[j][strlen(args->argv_extra[j]) - 1])) {
                  args->argv_extra[j][strlen(args->argv_extra[j]) - 1] = '\0';
               }
               j++;
               args->argv_extra[j] = strtok(NULL, "=");
            }
            args->argc_extra = j;
         }
      }

      return template;
   }

   return NULL;
}/*PREPROCESS_block_template_find*/

static char *PREPROCESS_block_translate(TPreProcess *this, char *input, char *extra) {
   char *start, *end, *cp;
   int diff;
   TTemplateArgs args;
   TTemplate *template;
   static char buf_output[BUF_SIZE];
   char *t_error;

   template = PREPROCESS_block_template_find(this, input, &start, &end, &args);

   this->template_current = template;

   cp = template ? end : NULL;
   while (template) {
      /* call template, do translation */
      strncpy(buf_output, start, end - start + 1);
      buf_output[end - start + 1] = '\0';
      args.lineno = this->cur_line;
      args.current_file = this->current_file;
      args.process_mode = this->process_mode;
      t_error =
         (*template->template_fn)(template,
                                  BT_Template, &args,
                                  BUF_SIZE, buf_output,
                                  BUF_SIZE, extra);
      if (t_error) {
         return t_error;
      }
      extra += strlen(extra);

      diff = (end - start - strlen(buf_output));
      memmove(end - diff, end + 1, strlen(end));
      strncpy(start, buf_output, strlen(buf_output));

      /* set cp to end of translated template */
      cp -= diff;

      input = start + 1;
      template = PREPROCESS_block_template_find(this, input, &start, &end, &args);
   }

   while (cp && *cp) {
      if (!isspace(*cp)) {
         this->template_current = NULL;
      }
      cp++;
   }

   return NULL;
}/*PREPROCESS_block_translate*/

static void PREPROCESS_block(TPreProcess *this, EBlockType type, char *input, char *output, char *extra) {
   char *t_error = NULL;
   TTemplateArgs args;
   TTemplate *template = this->template_block[this->block_depth];
   int nl_count;

   /*>>>kludge!*/
   if (this->template_current == &tp_Module) {
      this->template_current = NULL;
   }

   switch (type) {
   case BT_Statement:
      t_error = PREPROCESS_block_translate(this, input, extra);

      if (!t_error) {
         nl_count = PREPROCESS_nl_count(this, input);
         strcpy(output, input);
         if (this->block_depth && template) {
            CLEAR(&args);
            args.operation[0] = input;
            args.lineno = this->cur_line;
            args.current_file = this->current_file;
            args.process_mode = this->process_mode;
            t_error =
               (*template->template_fn)(template, type, &args,
                                        BUF_SIZE, output,
                                        BUF_SIZE, extra);

         PREPROCESS_nl_append(this, output, nl_count);
         }
      }
      break;
   case BT_BlockBeginPre:
      this->block_depth++;

      this->template_block[this->block_depth] = this->template_current ?
         this->template_current : this->template_block[this->block_depth - 1];
      template = this->template_current;
      /*fallthrough*/
   case BT_BlockBeginPost:
   case BT_BlockEndPre:
   case BT_BlockEndPost:
   case BT_BlockBegin:
   case BT_BlockEnd:
   case BT_Separator:
      if (input) {
         strcpy(output, input);
      }
      else {
         *output = '\0';
      }
      if (this->block_depth && template) {
         CLEAR(&args);
         args.lineno = this->cur_line;
         args.current_file = this->current_file;
         args.process_mode = this->process_mode;
         t_error =
            (*template->template_fn)(template, type, &args,
                                     BUF_SIZE, output,
                                     BUF_SIZE, extra);
      }

      if (type == BT_BlockEndPost) {
         if (this->block_depth) {
            this->block_depth--;
         }

         this->template_current = this->template_block[this->block_depth];
      }
      break;
   case BT_Comment:
      if (input) {
         strcpy(output, input);
      }
      break;
   default:
      break;
   }

   if (t_error) {
      PREPROCESS_lineinfo_print(this);
      printf("keycc: template error:%s:%s\n",
             this->template_current->name, t_error);
//   {
//     int a = 1, b = 0, c = a / b;
//   }
      exit(1);
   }
}/*PREPROCESS_block*/

static void PREPROCESS_statement_extract(TPreProcess *this, char *end, char *output, bool incend) {
   if (incend) {
      strncpy(output, this->statement.buffer, end - this->statement.buffer + 1);
      output[end - this->statement.buffer + 1] = '\0';
   }
   else {
      strncpy(output, this->statement.buffer, end - this->statement.buffer);
      output[end - this->statement.buffer] = '\0';
   }
   memmove(this->statement.buffer, end + 1, this->statement.buffer + strlen(this->statement.buffer) - end);
}/*PREPROCESS_statement_extract*/;

static bool PREPROCESS_line(TPreProcess *this, char *input, char *output,
                            char *extra) {
   TTemplateArgs args;
   static char statement[BUF_SIZE];
   char *cp, c;
   bool flush_extra = FALSE;
   bool inquote = FALSE;
   bool instatement = FALSE;

   /* module processing */
   if (input) {
      if (strncmp(input, tp_Module.name, strlen(tp_Module.name)) == 0) {
         PREPROCESS_block(this, BT_Statement, input, output, extra);
      }
      CLEAR(&args);
      args.current_file = this->current_file;
      (tp_Module.template_fn)(&tp_Module, BT_Statement, &args,
                              BUF_SIZE, input,
                              BUF_SIZE, extra);
      if (*input == '\0' && *extra == '\0') {
         *output = '\0';
         return FALSE;
      }
   }

   *output = '\0';
   *extra  = '\0';

   if (!input) {
      strcpy(this->statement.buffer + strlen(this->statement.buffer), "\4");
      }
   else {
      strcpy(this->statement.buffer + strlen(this->statement.buffer), input);
      }

   cp = this->statement.buffer;
   while (*cp) {
#if 1
     if (!instatement && cp[0] == '/' && cp[1] == '/') {
        while (*cp) {
           cp++;
        }
        cp--;

        PREPROCESS_statement_extract(this, cp, statement, TRUE);
        PREPROCESS_block(this, BT_Comment, statement, output, extra);
        output += strlen(output);
        continue;
     }

     if (!instatement && !this->inblockcomment) {
        if (cp[0] == '/' && cp[1] == '*') {
           this->inblockcomment = TRUE;
        }
     }
     if (this->inblockcomment) {
        if (cp[0] == '*' && cp[1] == '/') {
           cp++;
           PREPROCESS_statement_extract(this, cp, statement, TRUE);
           instatement = FALSE;
           PREPROCESS_block(this, BT_Comment, statement, output, extra);
           output += strlen(output);
           this->inblockcomment = FALSE;
           cp++;
        }
        else {
           cp++;
           continue;
        }
     }

     if (!isspace(*cp)) {
        instatement = TRUE;
     }
#endif
     if (*cp && strchr(";{}\"\4", *cp)) {
        c = *cp;

        if (c == '\"' || c == '\'') {
           /*>>>fairly incomplete quote handling*/
           inquote = !inquote;
           cp++;
           continue;
        }

        if (inquote || this->inblockcomment) {
           cp++;
           continue;
        }

        PREPROCESS_statement_extract(this, cp, statement, FALSE);
        instatement = FALSE;
        PREPROCESS_block(this, BT_Statement, statement, output, extra);
        output += strlen(output);

        /* pre */
        switch (c) {
        case '{':
           PREPROCESS_block(this, BT_BlockBeginPre, NULL, output, extra);
           break;
        case '}':
           PREPROCESS_block(this, BT_BlockEndPre, NULL, output, extra);
           break;
        }
        output += strlen(output);

        /* op */
        statement[0] = c;
        statement[1] = '\0';
        switch (c) {
        case '{':
           PREPROCESS_block(this, BT_BlockBegin, statement, output, extra);
           break;
        case '}':
           PREPROCESS_block(this, BT_BlockEnd, statement, output, extra);
           break;
        case ';':
           PREPROCESS_block(this, BT_Separator, statement, output, extra);
           if (this->pending_flush_extra) {
              flush_extra = TRUE;
              this->pending_flush_extra = FALSE;
           }
           break;
        }
        output += strlen(output);

        /* post */
        switch (c) {
        case '{':
           PREPROCESS_block(this, BT_BlockBeginPost, NULL, output, extra);
           this->pending_flush_extra = FALSE;
           break;
        case '}':
           PREPROCESS_block(this, BT_BlockEndPost, NULL, output, extra);
           if (this->block_depth == 0) {
              this->pending_flush_extra = TRUE;
           }
           break;
        }
        output += strlen(output);
        cp = this->statement.buffer;
        continue;
     }
     cp++;
   }

   while (isspace(*this->statement.buffer)) {
      *output = *this->statement.buffer;
      output++;
      *output = '\0';
      memmove(this->statement.buffer, this->statement.buffer + 1,
              strlen(this->statement.buffer));
   }

   return flush_extra;
}/*PREPROCESS_line*/

static void PREPROCESS_lineinfo_write(TPreProcess *this, char *dest,
                                      int line_no, char *file_name) {
   switch (this->ccompiler) {
   case CC_gcc:
   case CC_m68k_palmos_gcc:
      sprintf(dest, "# %d \"%s\"\n", line_no, file_name);
      break;
   case CC_cl:
      sprintf(dest, "#line %d \"%s\"\n", line_no, file_name);
      break;
   }
}/*PREPROCESS_lineinfo_write*/

static int PREPROCESS_begin(TPreProcess *this, int argc, char **argv) {
   int i;
   int arg_infile = 0, arg_outfile = 0;
   static char buffer[BUF_SIZE], buffer_out[BUF_SIZE], buffer_extra[BUF_SIZE];
   static char tempdir[BUF_SIZE];
   FILE *in_file;
   FILE *out_file;
   char *cp;
   bool flush_extra, line_lineinfo;

#if OS_Linux
   strcpy(tempdir, "/tmp");
#else
   strcpy(tempdir, getenv("TEMP"));
#endif
   strcpy(tempdir, ".");

   FBUF_new(&this->buf_output);
   FBUF_new(&this->buf_extra);

   this->process_mode = PM_Code;

   /* extract input and output file arguments */
   for (i = 0; i < argc; i++) {
     if (strcmp(argv[i], "--documentation") == 0) {
        this->process_mode = PM_Documentation;
     }
     if (strcmp(argv[i], "--noinclude") == 0) {
        this->opt_noinclude = TRUE;
     }
     if ((argv[i][0] == '-' && argv[i][1] == 'o')) {
         memcpy(&argv[i][0], &argv[i][2], strlen(&argv[i][2]) + 1);
         arg_outfile = i;
      }
      else if (argv[i][0] == '/' && argv[i][1] == 'F' && argv[i][2] == 'o') {
         memcpy(&argv[i][0], &argv[i][3], strlen(&argv[i][3]) + 1);
         arg_outfile = i;
      }
      else if (!strchr("-/`", argv[i][0])) {
         arg_infile = i;
      }
   }

   if (!arg_infile) {
      printf("keycc: no input file specfied\n");
      exit(1);
   }

   realpath(argv[arg_infile], this->input_filename);

   COMBINE_process(&preprocess.combine, this->ccompiler, this->opt_noinclude, this->process_mode == PM_Code, this->input_filename, TEMP_FILE_NAME_IN);

   /* preprocess */
   sprintf(buffer, "%s/%s", tempdir, TEMP_FILE_NAME_IN);
   if (!(in_file = fopen(buffer, "r"))) {
      printf("keycc: error opening first pass input\n");
      exit(1);
   }

   if (arg_outfile) {
      sprintf(buffer, "%s/%s", tempdir, TEMP_FILE_NAME_OUT);
      if (!(out_file = fopen(buffer, "w"))) {
         printf("keycc: error opening output for compiler\n");
         exit(1);
      }
   }
   else {
      out_file = fdopen(STDOUT, "w");
   }

   while (fgets(buffer, BUF_SIZE, in_file)) {
      flush_extra = FALSE;
      *buffer_extra = '\0';
      sscanf(buffer + 1, "%s", buffer_out);

      cp = buffer;
      if (*cp == '#') {
         strcpy(buffer_out, buffer);
         line_lineinfo = FALSE;

         switch (this->ccompiler) {
         case CC_gcc:
         case CC_m68k_palmos_gcc:
            line_lineinfo = !isalpha(*(cp + 1));
            break;
         case CC_cl:
            line_lineinfo = strncmp(cp + 1, "line", 4) == 0;
            break;
         }

         if (line_lineinfo) {
            PREPROCESS_line(this, NULL, buffer_out, buffer_extra);
            FBUF_append(&this->buf_output, buffer_out);
            FBUF_append(&this->buf_extra, buffer_extra);

            while (*cp && !isdigit(*cp)) {
               cp++;
             }
            this->cur_line = atoi(cp);
            while (isdigit(*cp)) {
               cp++;
            }

            cp = strchr(buffer, '\"');
            if (!cp) {
               PREPROCESS_lineinfo_print(this);
               printf("keycc: badly formatted compiler preprocessor line\n");
               exit(1);
            }

            realpath(cp + 1, this->cur_filename);
            cp = strchr(this->cur_filename, '\"');
            if (!cp) {
               PREPROCESS_lineinfo_print(this);
               printf("keycc: badly formatted compiler preprocessor line\n");
               exit(1);
            }
            *cp = '\0';

            this->current_file = strcmp(this->input_filename, this->cur_filename) == 0;
            MODULE_current_file(this->cur_filename, this->current_file);

            PREPROCESS_lineinfo_write(this, buffer_out, this->cur_line, this->cur_filename);
         }
         else {
            /* other preprocesssor directive */
            if (this->process_mode != PM_Code) {
               flush_extra = PREPROCESS_line(this, buffer, buffer_out, buffer_extra);
            }
            this->cur_line++;
         }
      }
      else {
         flush_extra = PREPROCESS_line(this, buffer, buffer_out, buffer_extra);
//printf(">%s", buffer_out);
         this->cur_line++;
      }
      FBUF_append(&this->buf_output, buffer_out);
      FBUF_append(&this->buf_extra, buffer_extra);
      if (flush_extra && !FBUF_empty(&this->buf_extra)) {
//         sprintf(buffer, "__lc_generated/%s__", this->cur_filename);
//         PREPROCESS_lineinfo_write(this, buffer_out, 1, buffer);
         PREPROCESS_lineinfo_write(this, buffer_out, 1, this->cur_filename);
         FBUF_append(&this->buf_output, buffer_out);
         FBUF_append(&this->buf_output, FBUF_buf(&this->buf_extra));
         FBUF_clear(&this->buf_extra);
         PREPROCESS_lineinfo_write(this, buffer_out,
                                   this->cur_line, this->cur_filename);
         FBUF_append(&this->buf_output, buffer_out);
         flush_extra = FALSE;
      }
   }

   /* termination */
   PREPROCESS_line(this, NULL, buffer_out, buffer_extra);
   FBUF_append(&this->buf_output, buffer_out);
   FBUF_append(&this->buf_extra, buffer_extra);

   FBUF_file_write(&this->buf_extra, out_file);
   FBUF_file_write(&this->buf_output, out_file);

   fclose(out_file);
   fclose(in_file);

   /* back to compiler */
   if (arg_outfile) {
      sprintf(buffer, "%s -c %s ", CONFIG_key_get("CC"), CONFIG_key_get("DEBUG"));
      sprintf(buffer + strlen(buffer), "-c -I%s/src/base %s ",
              CONFIG_key_get(ENV_DIRECTORY), CONFIG_key_get("TARGET"));
      for (i = 1; i < argc; i++) {
         if (i == arg_infile) {
            sprintf(buffer + strlen(buffer), "%s/%s ",
                    tempdir, TEMP_FILE_NAME_OUT);
         }
         else if (i == arg_outfile) {
            sprintf(buffer + strlen(buffer), "%s%s ",
                    CONFIG_key_get("OBJOUT"), argv[i]);
         }
         else {
            sprintf(buffer + strlen(buffer), "%s ", argv[i]);
         }
      }

      if (system(buffer)) {
         printf("keycc: error running\n");
         exit(1);
      }
   }

   FBUF_delete(&this->buf_extra);
   FBUF_delete(&this->buf_output);

   /* delete temporary files */
   remove(TEMP_FILE_NAME_IN);
   remove(TEMP_FILE_NAME_OUT);

   return 1;
}

TPreProcess preprocess;

int main(int argc, char **argv) {
   char config_name[100];
   char temp[100];

   if (argc < 2) {
      printf("Keystone Application Framework C preprocessor\n");
      printf("Copyright (C) Jacob Lister, Abbey Systems 2008.  All rights reserved.\n\n");
      printf("usage: keycc <infile> [-o<outfile>] [--documentation --noimplementation --noinclude --nocomp] [compileroption...]\n");
      printf("   <infile>                             - input source file name\n");
      printf("   -o<outfile>                          - output module name (write to STDOUT if not specified)\n");
      printf("   --documentation                      - generate faux C++ for DOxygen\n");
      printf("   --noinclude                          - do not process include files\n");
      printf("   --noimplementation                   - strip implementation section from output\n");
      printf("   --nocomp                             - do not compile, preprocess only\n");
      printf("   compileroption...                    - other options sent to compiler\n");
      exit(1);
      }
   PREPROCESS_new(&preprocess);

   CONFIG_new();
   sprintf(config_name, "%s", CONFIG_make_filename(argv[0], CONFIG_FILE_NAME));
   if (!CONFIG_read_file(config_name)) {
      printf("cannot open config file '%s'\n", config_name);
      exit(1);
   };
   
   if (!CONFIG_key_get(ENV_DIRECTORY)) {
      printf("'%s' environment variable not set\n", ENV_DIRECTORY);
      exit(1);
   }
   if (!CONFIG_key_get(ENV_TARGET)) {
      printf("'%s' environment variable not set\n", ENV_TARGET);
      exit(1);
   }
   COMBINE_include_add(&preprocess.combine, CONFIG_key_get(ENV_DIRECTORY));
   sprintf(temp, "%s/src/base", CONFIG_key_get(ENV_DIRECTORY));
   COMBINE_include_add(&preprocess.combine, temp);
   
   if (!CONFIG_key_get("CC")) {
      printf("CC (C Compiler) not set in config");
      exit(1);
   }
   if (!CONFIG_key_get("TARGET")) {
      printf("TARGET not set in config");
      exit(1);
   }
   if (!CONFIG_key_get("OBJOUT")) {
      printf("OBJOUT not set in config");
      exit(1);
   }
   if (!CONFIG_key_get("DEBUG")) {
      printf("DEBUG not set in config");
      exit(1);
   }

   if (strcmp(CONFIG_key_get("CC"), "gcc") == 0) {
      preprocess.ccompiler = CC_gcc;
   }
   else if (strcmp(CONFIG_key_get("CC"), "cl") == 0) {
      preprocess.ccompiler = CC_cl;
   }
   else if (strcmp(CONFIG_key_get("CC"), "m68k-palmos-gcc") == 0) {
      preprocess.ccompiler = CC_m68k_palmos_gcc;
   }
   else {
      printf("supported compilers - gcc, cl\n");
      exit(1);
   }

   PREPROCESS_template_register(&preprocess, &tp_Module);

   PREPROCESS_template_register(&preprocess, &tp_List);
   PREPROCESS_template_register(&preprocess, &tp_Array);
   PREPROCESS_template_register(&preprocess, &tp_Stack);
   PREPROCESS_template_register(&preprocess, &tp_Bitfield);
   PREPROCESS_template_register(&preprocess, &tp_Vector);

   PREPROCESS_template_register(&preprocess, &tp_Class);
   PREPROCESS_template_register(&preprocess, &tp_Attribute);
   PREPROCESS_template_register(&preprocess, &tp_Element);
   PREPROCESS_template_register(&preprocess, &tp_Menu);
   PREPROCESS_template_register(&preprocess, &tp_Data);
   PREPROCESS_template_register(&preprocess, &tp_Alias);
   PREPROCESS_template_register(&preprocess, &tp_Option);
   PREPROCESS_template_register(&preprocess, &tp_Object);
//   PREPROCESS_template_register(&preprocess, &tp_Base);
   PREPROCESS_template_register(&preprocess, &tp_New);
   PREPROCESS_template_register(&preprocess, &tp_Delete);

   PREPROCESS_template_register(&preprocess, &tp_Enum);
//   PREPROCESS_template_register(&preprocess, &tp_Text);

   PREPROCESS_template_register(&preprocess, &tp_Try);
   PREPROCESS_template_register(&preprocess, &tp_Catch);
   PREPROCESS_template_register(&preprocess, &tp_Finally);
   PREPROCESS_template_register(&preprocess, &tp_Throw);
   PREPROCESS_template_register(&preprocess, &tp_Exception);

   PREPROCESS_begin(&preprocess, argc, argv);

   CONFIG_delete();
   PREPROCESS_delete(&preprocess);


   return 0;
}
