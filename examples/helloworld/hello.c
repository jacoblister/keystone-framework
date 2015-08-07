//#include "framework.c"

#if 1
int
#endif

class CHelloWorldApp : CApplication {
 public:
   void main(ARRAY<CString> *args);
#if 1
   int test;
#endif
   ATTRIBUTE<int value> {
#if 1
      this->value = val;
#else
      this->value = *data;
#endif      
   }
};

OBJECT<CHelloWorldApp, helloworldapp>;

int CHelloWorldApp::main(ARRAY<CString> *args) {
   CGDialogMessage message;    
    
   new(&message).CGDialogMessage("Hello Keystone!", "Hello World!", NULL);
   CGWindowDialog(&message).execute();
   delete(&message);

   return 0;
}/*CHelloWorldApp::main*/