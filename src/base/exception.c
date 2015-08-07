/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*>>>note, single thread only*/

#include "environ.h"
#include "contain.h"
#include "exception.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

LIST:typedef<TExBlock>;

typedef struct {
   LIST<TExBlock> block;
} TExcept;

TExcept except;

#ifdef COMP_M68K_PALMOS_GCC
static inline void EX_longjmp(jmp_buf *env, int val) {
   longjmp(env, val);
}
#else
static inline void EX_longjmp(jmp_buf *env, int val) {
   longjmp(*env, val);
}
#endif

Global void EX_new(void) {
   LIST(&except.block).new(0);
}/*EX_new*/

Global void EX_delete(void) {
   LIST(&except.block).delete();
}/*EX_delete*/

Global void EX_throw(struct tag_CObject *obj,
                     struct tag_CObjClass *obj_class, int exception_code,
                     const char *message) {
   TExBlock *block_next, *block = LIST(&except.block).last();

   while (block) {
      block->handled = FALSE;
      switch(block->state) {
      case EX_Code:
         /* execute the handler */
         block->obj            = obj;
         block->obj_class      = obj_class;
         block->exception_code = exception_code;
         block->message        = message;
         block->state          = EX_Handling;

         if (_setjmp(block->continue_context) == 0) {
            EX_longjmp(&block->context, block->state);
            break;
         }
         printf("exception handled, continue\n");
         return;
      case EX_Handling:
         /* rethrow, do finally first */
         block->obj            = obj;
         block->obj_class      = obj_class;
         block->exception_code = exception_code;
         block->message        = message;
         block->state          = EX_Finally;
         EX_longjmp(&block->context, block->state);
         break;
      case EX_Finally:
         /* throw within finally; propagate */
         block_next = LIST(&except.block).previous(block);
         LIST(&except.block).remove(block);
         block = block_next;
         break;
      default:
         ASSERT("EXCEPTION:Corrupted exception handler chain");
         break;
      }
   }

   printf("EXIT:EXCEPTION:Unhandled exception:%s\n", message);
   ABORT();
}/*EX_throw*/

Global void EX_reexec(void) {
   TExBlock *block = LIST(&except.block).last();
   block->exception_code = 0;
   block->state = EX_Code;
   block->handled = TRUE;
   EX_longjmp(&block->context, block->state);
}/*EX_reexec*/

Global void EX_continue(void) {
   TExBlock *block = LIST(&except.block).last();

   ASSERT("EX_continue:not working\n");

   EX_longjmp(&block->continue_context, 1);
}/*EX_continue*/

Global void EX_handled(void) {
   TExBlock *block = LIST(&except.block).last();

#if DEBUG
   if (!block)
      ASSERT("EXCEPTION:no exception handler in frame in EX_handled");
   switch (block->state) {
      case EX_Handling:
         break;
      case EX_Code:
      case EX_Finally:
         ASSERT("EXCEPTION:exception handled outside handler");
      default:
         ASSERT("EXCEPTION:Corrupted exception handler chain");
   }
#endif

   block->exception_code = 0;
   block->state = EX_Code;
   block->handled = TRUE;
}/*EX_handled*/

Global void EX_block_add(TExBlock *block) {
   CLEAR(block);
   block->handled = TRUE;
   LIST(&except.block).add(block, 0);
}/*EX_block_add*/

Global bool EX_block_remove(TExBlock *block) {
#ifdef DEBUG
   if (LIST(&except.block).last() != block)
      ASSERT("EXCEPTION:Corrupted exception handler chain");
#endif

   switch (block->state) {
   case EX_Handling:                 /* exception is now handled */
   case EX_Code:                     /* no exception */
      block->state = EX_Finally;
      EX_longjmp(&block->context, block->state);
   case EX_Finally:                  /* we are done */
      LIST(&except.block).remove(block);
      if (block->handled)
         return FALSE;               /* continue after try */
      else {
         EX_throw(NULL, NULL, block->exception_code, block->message);
      }
      break;
   default:
      ASSERT("EXCEPTION:Corrupted exception handler chain");
   }
   return FALSE;
}/*EX_block_remove*/

Global TException *EX_exception(TExBlock *block) {
   static TException exception;

   CLEAR(&exception);
   exception.obj            = block->obj;
   exception.obj_class      = block->obj_class;
   exception.exception_code = block->exception_code;
   exception.message        = block->message;

   return &exception;
}/*EX_exception_object*/

Global bool EX_exception_test(TExBlock *block,
                              struct tag_CObject *obj, struct tag_CObjClass *obj_class,
                              int exception_code) {
   if (obj && block->obj != obj)
      return FALSE;

   if (obj_class && obj_class != block->obj_class)
      return FALSE;

   if (exception_code != EXCEPTION_CODE_ANY &&
       exception_code != block->exception_code)
       return FALSE;

   return TRUE;
}/*EX_exception_test*/
