#include <windows.h>
// link in AltAxWinInit() & containment code statically
// this require a instance (_Module) of CComModule

//#include <atlbase.h>
//extern CComModule _Module;
//#include <atlcom.h>
//#include <atlhost.h>
//CComModule _Module;

#include <stdio.h>
extern "C" {
/*>>>fix this*/
//bool activex_linkme;
//void _virtual_CGActiveXContainer_dummy(void *self) {
//   printf("actx new\n");
//};
   void activex_linkme(void) {
   }
}

typedef int (CALLBACK * ATLAXWININIT_PROC)(void);

class CActiveXInit {
 public:
  HINSTANCE hDll;
  ATLAXWININIT_PROC m_pfnAtlAxWinInit;
  ATLAXWININIT_PROC m_pfnAtlAxWinTerm;

   CActiveXInit(void) {
//      _Module.Init(NULL, GetModuleHandle(NULL));
      //Initialize ATL control containment code.
      hDll = LoadLibrary("atl");
      m_pfnAtlAxWinInit = (ATLAXWININIT_PROC)GetProcAddress(hDll, TEXT("AtlAxWinInit"));
      m_pfnAtlAxWinInit();

//      AtlAxWinInit();
//      init = GetProcAddress(LoadLibrary("atl"),"AtlAxWinInit"); _asm call init;
//      CoInitialize(NULL);
   };
   ~CActiveXInit(void) {
      hDll = LoadLibrary("atl");
      m_pfnAtlAxWinTerm = (ATLAXWININIT_PROC)GetProcAddress(hDll, TEXT("AtlAxWinTerm"));
      if (m_pfnAtlAxWinTerm) {
         m_pfnAtlAxWinTerm();
      }
//      AtlAxWinTerm();
//      _Module.Term();
   };
};

CActiveXInit activexinit;