// Note: based on
// RTFEX.H
//
// Copyright (c) 1998,99 On Time
// http://www.on-time.com

#ifndef I_EXCEPTION
#define I_EXCEPTION

#include "environ.h"
#include "contain.h"

#include <setjmp.h>

static inline int EX_setjmp(jmp_buf *env) {
   return setjmp(*env);
}

#define EXCEPTION_CODE_ANY -1

struct tag_CObject;
struct tag_CObjClass;

typedef struct {
   struct tag_CObject *obj;
   struct tag_CObjClass *obj_class;
   int exception_code;
   const char *message;
} TException;

/* data needed for each exception block */

typedef enum {
   EX_Code,
   EX_Handling,
   EX_Finally,
} ExBlockState;

typedef struct {
   TListItem base;
   jmp_buf context;
   jmp_buf continue_context;
   struct tag_CObject *obj;
   struct tag_CObjClass *obj_class;
   int exception_code;
   const char *message;
   bool handled;
   ExBlockState state;
} TExBlock;

void EX_new(void);
void EX_delete(void);
void EX_block_add(TExBlock *block);
bool EX_block_remove(TExBlock *block);
void EX_throw(struct tag_CObject *obj,
              struct tag_CObjClass *obj_class, int exception_code,
              const char *message);
void EX_continue(void);
void EX_reexec(void);
void EX_handled(void);
TException *EX_exception(TExBlock *block);
bool EX_exception_test(TExBlock *block,
                       struct tag_CObject *obj, struct tag_CObjClass *obj_class,
                       int exception_code);

#endif/*I_EXCEPTION*/
