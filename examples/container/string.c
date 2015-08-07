/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CStringTest : CApplication {
 public:
   ALIAS<"string">;     
 
   void main(ARRAY<CString> *args);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

OBJECT<CStringTest, stringtest>;

VECTOR:typedef<char>;
VECTOR<char> vec_dynamic;
//VECTOR<char>;
//VECTOR<char, 10> vec_static;
VECTOR<char, 10> vec_static_init = ['a', 'b', 'c', 'd'];

//void test(VECTOR<char> input, VECTOR<char> result);

int CStringTest::main(ARRAY<CString> *args) {
   int i;
   CString value;
   ARRAY<char> chararray;
   VECTOR<char, 10> vec_stack_init = ['1', '2', '3', '4'];
   
   ARRAY(&chararray).new();
   ARRAY(&chararray).data_set(6, "Array");
   new(&value).CString("Example");

   printf("string=%s array=%s\n", CString(&value).string(), ARRAY(&chararray).data());
   printf("capacity=%d, count=%d\n", VECTOR(&vec_static_init).capacity(), VECTOR(&vec_static_init).count());
   for (i = 0; i < VECTOR(&vec_static_init).capacity(); i++) {
      printf("index %d = %c\n", i, VECTOR(&vec_static_init).item(i));
   }
   
   delete(&value);
   ARRAY(&chararray).delete();
   
   return 0;
#if 0
//   TString st;
//   TString *stx = (TString *)"\000\000\000\000\000""Test String";
//   TString *st2 = STRING<"Test String">;
//   STRING<40> temp;
//   STRING<*> *dyntemp = NULL;
   
//   STRING(&temp).new();
//   STRING(dyntemp).new();
//   STRING(dyntemp).delete();
   
//   printf("string test (%s)\n", STRING(st2).cstr());
//   return 0;
#endif   
}/*CStateTest::main*/


/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/