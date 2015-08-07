#include "framework.c"

class CArrayExample : CApplication {
 public:
   void main(ARRAY<CString> *args);
};

OBJECT<CArrayExample, array_example>;

int CArrayExample::main(ARRAY<CString> *args) {
   VECTOR<char, 10> vec_static;
   
   printf("array capacity = %d\n", VECTOR(&vec_static).capacity());
}