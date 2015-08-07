#include "environ.h"

#if 0
/* Single */
ARRAY

/* 'Definition' template examples */
ARRAY(int)               [template = "ARRAY",      op_count = 0, operation = NULL,    argc = 1, argv = "int"]
LIST(TPoint)             [template = "LIST",       op_count = 0, operation = NULL,    argc = 1, argv = "TPoint"]
CString(test)            [template = "CString", object = "test",   operation = NULL, argc = 0]
MODULE:INTERFACE         [template = "MODULE",     op_count = 1, operation = "INTERFACE", argc = 0]
CString::print(void)     [template = "CString",    op_count = 1, operation = "print", argc = 1, argv = "void"]
THIS_BEGIN               [template = "THIS_BEGIN", op_count = 0, operation = NULL,    argc = 0]
class CString : CObject("String", "text") {
   [template = "class", op_count = 2, operation = "CString", "CObject", argc = 2, argv = "String", "text"]

/* 'Object' template examples */
ARRAY(darray).set("text", 5)   [template = "ARRAY",   object = "darray", operation = "set", argc = 1, argv = ""text"", "5"]
ARRAY(darray).data[3]          [template = "ARRAY",   object = "darray", operation = "data", argc = 0]
CString(test).print            [template = "CString", object = "test",   operation = "print", argc = 0]


#endif

bool ismchar(char c);
char *strmatch(char *str);

typedef enum {
  CC_gcc,
  CC_cl,
  CC_m68k_palmos_gcc,
} ECCompiler;

#define MAX_LINE 2000

typedef enum {
   BT_Statement,
   BT_BlockBeginPre,
   BT_BlockBegin,
   BT_BlockBeginPost,
   BT_BlockEndPre,
   BT_BlockEnd,
   BT_BlockEndPost,
   BT_Separator,
   BT_Template,
   BT_Comment,   
} EBlockType;

typedef enum {
  TT_Single,
  TT_Definition,
  TT_Object,
} ETemplateType;

typedef enum {
  PM_Code,
  PM_Documentation,
} EProcessMode;

typedef struct {
   ETemplateType type;
   char *template;
   int op_count;
   char *operation[10];
   char *object;
   int argc;
   char *argv[40];
   int argc_extra;
   char *argv_extra[40];
   EProcessMode process_mode;
   int lineno;
   bool current_file;
   char buf_object[MAX_LINE];
   char buf_operation[MAX_LINE];
   char buf_args[MAX_LINE];
   char buf_args_extra[MAX_LINE];
} TTemplateArgs;

struct TEMPLATE;

typedef char *(*TEMPLATE_FN)(struct TEMPLATE *this,
                             EBlockType type, TTemplateArgs *args,
                             int dest_length, char *dest,
                             int extra_length, char *extra);

typedef struct TEMPLATE {
   char name[80];
   TEMPLATE_FN template_fn;
   bool complex;
   bool extra_arg;
} TTemplate;

struct PREPROCESS;
extern struct PREPROCESS preprocess;

void PREPROCESS_name_convert(char *name);
ECCompiler PREPROCESS_CCompiler(struct PREPROCESS *this);
char *PREPROCESS_current_filename(struct PREPROCESS *this);
int PREPROCESS_cobjclass_offset(struct PREPROCESS *this);
int PREPROCESS_cobjclass_vtbl_offset(struct PREPROCESS *this);
void PREPROCESS_template_register(struct PREPROCESS *this, TTemplate *template);

