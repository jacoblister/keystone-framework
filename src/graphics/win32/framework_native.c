/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

//#include "../main.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <winsock2.h>
#include <windows.h>
#include <commctrl.h>

#include "gdiplusc.h"
#include "../main.c"
#include "native.h"

void CALLBACK CFramework__Win32_timeout(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
//   printf("main timeout %d\n", CFramework(&framework).thread_this());
   CThread(CFramework(&framework).thread_this()).sleep(0);
}/*CFramework__LINUX_timeout*/

WNDPROC Win32_ATL_wndproc;
WNDPROC Win32_ATL_wndproc_subclassed;

bool CFramework__Win32_translate_key_event(MSG *msg, CEventKey *result) {
   EEventKeyType key_type;
   int modifier;
   WPARAM wParam = 0;

   switch (msg->message) {
   case WM_KEYUP:
   case WM_SYSKEYUP:
   case WM_KEYDOWN:
   case WM_SYSKEYDOWN:
      key_type = (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) ? EEventKeyType.down : EEventKeyType.up;

      modifier = 0;
      modifier |= (GetKeyState(VK_CONTROL) & 0x8000) ? (1 << EEventModifier.ctrlKey) : 0;
      modifier |= (GetKeyState(VK_SHIFT) & 0x8000) ? (1 << EEventModifier.shiftKey) : 0;
      modifier |= (GetKeyState(VK_MENU) & 0x8000) ? (1 << EEventModifier.altKey) : 0;

      switch (msg->wParam) {
      case VK_LEFT:
         new(result).CEventKey(key_type, EEventKey.Left, 0, modifier);
         goto sendevent;
      case VK_RIGHT:
         new(result).CEventKey(key_type, EEventKey.Right, 0, modifier);
         goto sendevent;
      case VK_UP:
         new(result).CEventKey(key_type, EEventKey.Up, 0, modifier);
         goto sendevent;
      case VK_DOWN:
         new(result).CEventKey(key_type, EEventKey.Down, 0, modifier);
         goto sendevent;
      case VK_NEXT:
         new(result).CEventKey(key_type, EEventKey.PgDn, 0, modifier);
         goto sendevent;
      case VK_PRIOR:
         new(result).CEventKey(key_type, EEventKey.PgUp, 0, modifier);
         goto sendevent;
      case VK_HOME:
         new(result).CEventKey(key_type, EEventKey.Home, 0, modifier);
         goto sendevent;
      case VK_END:
         new(result).CEventKey(key_type, EEventKey.End, 0, modifier);
         goto sendevent;
      case VK_INSERT:
         new(result).CEventKey(key_type, EEventKey.Insert, 0, modifier);
         goto sendevent;
      case VK_DELETE:
         new(result).CEventKey(key_type, EEventKey.Delete, 0, modifier);
         goto sendevent;
      case VK_SPACE:
         new(result).CEventKey(key_type, EEventKey.Space, 0, modifier);
         goto sendevent;
      /*>>pageup/dn?*/
      case VK_F1:
      case VK_F2:
      case VK_F3:
      case VK_F4:
      case VK_F5:
      case VK_F6:
      case VK_F7:
      case VK_F8:
      case VK_F9:
      case VK_F10:
      case VK_F11:
      case VK_F12:
      case VK_F13:
      case VK_F14:
      case VK_F15:
      case VK_F16:
      case VK_F17:
      case VK_F18:
      case VK_F19:
      case VK_F20:
      case VK_F21:
      case VK_F22:
      case VK_F23:
      case VK_F24:
         new(result).CEventKey(key_type, EEventKey.Function, (char)(msg->wParam - VK_F1 + 1), modifier);
         goto sendevent;
      case VK_TAB: 
         new(result).CEventKey(key_type, EEventKey.Tab, 0, modifier);
         goto sendevent;
      case VK_BACK: 
         new(result).CEventKey(key_type, EEventKey.Backspace, 0, modifier);
         goto sendevent;
      case VK_RETURN: 
         new(result).CEventKey(key_type, EEventKey.Enter, 0, modifier);
         goto sendevent;
      case VK_ESCAPE: 
         new(result).CEventKey(key_type, EEventKey.Escape, 0, modifier);
         goto sendevent;
//      sendevent:
//         CEventKey_NATIVE_DATA(&event)->message = msg->message;
//         CEventKey_NATIVE_DATA(&event)->wParam = msg->wParam;      
//         CEventKey_NATIVE_DATA(&event)->lParam = msg->lParam;            
//         printf("keyboard event\n");
         break;
      }
      break;
   case WM_SYSCHAR:
   case WM_CHAR:
      modifier = 0;
      modifier |= (GetKeyState(VK_CONTROL) & 0x8000) ? (1 << EEventModifier.ctrlKey) : 0;
      modifier |= (GetKeyState(VK_SHIFT) & 0x8000) ? (1 << EEventModifier.shiftKey) : 0;
      modifier |= (GetKeyState(VK_MENU) & 0x8000) ? (1 << EEventModifier.altKey) : 0;
      switch (msg->wParam) {
      case 0x1B:                       /* Escape */
      case 0x0A: case 0x0D:            /* Enter */      
      case 0x08:                       /* Backspace */
      case 0x09:                       /* Tab */
         if (!GetKeyState(VK_CONTROL) & 0x8000)
            return FALSE;
      default:
         wParam = msg->wParam;
         if (GetKeyState(VK_CONTROL) & 0x8000) {
             /* correct ASCII characters with CTRL >>>can do much better */
             wParam += 64;
         }
         new(result).CEventKey(EEventKeyType.down, EEventKey.Ascii, (char)wParam, modifier);
         break;
      }
      goto sendevent;
   sendevent:
      CEventKey_NATIVE_DATA(result)->message = msg->message;
      CEventKey_NATIVE_DATA(result)->wParam = wParam;      
      CEventKey_NATIVE_DATA(result)->lParam = msg->lParam;
      return TRUE;
   }/*switch*/
   return FALSE;
}/*CFramework__Win32_translate_key*/

bool CFramework__Win32_EchoMessage(MSG *msg) {
   HWND parent;
   CEventKey event;
   CObject *object;
   
   if (CFramework__Win32_translate_key_event(msg, &event)) {
      object = (CObject *)GetWindowLong(msg->hwnd, GWL_USERDATA);
      object = CObject_exists(object) ? object : NULL;
      if (CApplication(framework.application).notify_event(object, CEvent(&event))) {
         return TRUE;
      }
   }
     
   /* Foward keyboard input to ATL container controls >>> bit kludgy*/
   switch (msg->message) {
   case 0x020A: //WM_MOUSEWHEEL:
   case WM_MOUSEMOVE:
   case WM_LBUTTONDOWN:
   case WM_LBUTTONUP:
   case WM_LBUTTONDBLCLK:
   case WM_RBUTTONDOWN:
   case WM_RBUTTONUP:
   case WM_RBUTTONDBLCLK:
//    case WM_MOUSELEAVE:

   case WM_KEYUP:
   case WM_SYSKEYUP:
   case WM_KEYDOWN:
   case WM_SYSKEYDOWN:
   case WM_SYSCHAR:
   case WM_CHAR:
      parent = GetParent(msg->hwnd);
      if (parent) {
         if (Win32_ATL_wndproc_subclassed && ((WNDPROC)GetWindowLong(parent, GWL_WNDPROC) == Win32_ATL_wndproc_subclassed)) {
            return SendMessage(parent, msg->message, msg->wParam, msg->lParam);
         }
      }
   }
   return FALSE;
}
ULONG_PTR gdiplusToken = 0;

void CFramework::NATIVE_new(void) {
   WNDCLASSEX wndClass;
//   GdiplusStartupInput gdiplusStartupInput;
   INITCOMMONCONTROLSEX icex;

//   WSADATA info;
//   if (WSAStartup(MAKEWORD(2,0), &info) != 0) {
//      printf("Cannot initialize WinSock!\n");
//      exit(0);
//   }

   /* consider initilizing in gcontrol_native*/
//   InitCommonControls();
   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC = ICC_DATE_CLASSES | ICC_USEREX_CLASSES;
   InitCommonControlsEx(&icex);

#if KEYSTONE_WIN32_GDIP
//   // Initialize GDI+.
//   memset(&gdiplusStartupInput, 0, sizeof(gdiplusStartupInput));
//   gdiplusStartupInput.GdiplusVersion = 1;
//   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif

   ARRAY(&this->tls_index).new();
   ARRAY(&this->tls_index).used_set(1);
   ARRAY(&this->tls_index).data()[ETLSIndex.This] = TlsAlloc();

//   SetTimer(NULL, 0, 20, (TIMERPROC)CFramework__Win32_timeout);

   GetClassInfoEx(GetModuleHandle(NULL), "AtlAxWin", &wndClass);
   Win32_ATL_wndproc = wndClass.lpfnWndProc;
}/*CFramework::NATIVE_new*/

void CFramework::NATIVE_delete(void) {
#if KEYSTONE_WIN32_GDIP
   if (gdiplusToken) {
      GdiplusShutdown(gdiplusToken);
   }
#endif

   ARRAY(&this->tls_index).delete();
}/*CFramework::NATIVE_delete*/

void CFramework::NATIVE_main_iteration(void) {
   MSG msg;

   while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      if (!CFramework__Win32_EchoMessage(&msg)) {
         DispatchMessage(&msg);
      }
   }
}/*CFramework::NATIVE_main_iteration*/

void CFramework::NATIVE_main(void) {
   MSG msg;

   while (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      if (!CFramework__Win32_EchoMessage(&msg)) {
         DispatchMessage(&msg);
      }
   }
}/*CFramework::NATIVE_main*/

void CFramework::NATIVE_modal_wait(CObject *object) {
   MSG msg;

   int modal_count_entry = this->modal_count + 1;

   this->modal_count = modal_count_entry;

   while (this->modal_count >= modal_count_entry) {
      GetMessage(&msg, NULL, 0, 0);
      TranslateMessage(&msg);
      if (!CFramework__Win32_EchoMessage(&msg)) {
         DispatchMessage(&msg);
      }
   }
}/*CFramework::NATIVE_modal_wait*/

void CFramework::NATIVE_modal_exit(CObject *object) {
   this->modal_count--;
}/*CFramework::NATIVE_modal_exit*/

void CFramework::NATIVE_kill(void) {
   PostQuitMessage(0);
}/*CFramework::NATIVE_kill*/

bool CFramework::display_device_resolution(int *res_x, int *res_y) {
    HDC hdc = CreateCompatibleDC(NULL);

    *res_x = GetDeviceCaps(hdc, HORZRES);
    *res_y = GetDeviceCaps(hdc, VERTRES);

    DeleteDC(hdc);

    return TRUE;
}/*CFramework::display_device_resolution*/

void CFramework::beep(void) {
   MessageBeep(MB_OK);
}/*CFramework::beep*/

void CFramework::debug_message(char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   va_start(args, format);

   vsprintf(output, format, args);
   OutputDebugString(output);

   CFramework(&framework).scratch_buffer_release(output);
}/*CFramework::debug_message*/

void CApplication::NATIVE_set_home_path(char *argv0) {
   char *p = strrchr(argv0, '\\');
   char exename[256], homepath[256];

   strcpy(exename, p ? p + 1 : argv0);
   if (!strrchr(exename, '.'))
      strcat(exename, ".exe");
   /* Store start-up directory */
   memset(homepath, 0, sizeof(homepath));
   if (p == NULL) {
      // Handle situation where full path to executable is not supplied in argp[0]
      GetCurrentDirectory(sizeof(homepath), homepath);
   }
   else {
      strncpy(homepath, argv0, p - argv0);
   }
   CString(&this->home_path).set(homepath);
}/*CApplication::NATIVE_set_home_path*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
