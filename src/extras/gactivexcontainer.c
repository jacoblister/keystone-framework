/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGActiveXContainer : CGXULElement {
 private:
   void *wndproc;  
   void new(void);
   void delete(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
 private:
   bool passthrough_input_events;
   virtual void show(bool show);
   virtual void activex_allocate(void *hwnd);
   virtual void activex_release(void);
 public:
   ALIAS<"xul:activex">;
   ATTRIBUTE<CString activeXClass>;

   void CGActiveXContainer(int x, int y, int width, int height, const char *ActiveX_classname);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

int framework_activex_size;
extern void activex_linkme(void);

#include <windows.h>

#ifndef WM_FORWARDMSG
#define WM_FORWARDMSG 0x037F // MFC/ATL ?
#endif

void CGActiveXContainer::new(void) {
   class:base.new();

   new(&this->activeXClass).CString(NULL);
//   this->passthrough_input_events = TRUE;

   framework_activex_size = sizeof(CGActiveXContainer);
   activex_linkme();
}/*CGActiveXContainer::new*/

void CGActiveXContainer::CGActiveXContainer(int x, int y, int width, int height, const char *ActiveX_classname) {
}/*CGActiveXContainer::CGActiveXContainer*/

void CGActiveXContainer::delete(void) {
   class:base.delete();

   delete(&this->activeXClass);
}/*CGActiveXContainer::delete*/

extern WNDPROC Win32_ATL_wndproc;
extern WNDPROC Win32_ATL_wndproc_subclassed;

Local LRESULT CALLBACK CGActiveXContainer_WIN32_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
   CGActiveXContainer *this = (void *)GetWindowLong(hwnd, GWL_USERDATA);
   CGWindow *window; 
   CGCanvas *canvas;
   bool filter = TRUE;
   POINT point;
   
   switch (message) {
   case 0x020A: //WM_MOUSEWHEEL:
   case WM_MOUSEMOVE:
   case WM_LBUTTONDOWN:
   case WM_LBUTTONUP:
   case WM_LBUTTONDBLCLK:
   case WM_RBUTTONDOWN:
   case WM_RBUTTONUP:
   case WM_RBUTTONDBLCLK:
      if (this->passthrough_input_events) {
         point.x = LOWORD(lParam);
         point.y = HIWORD(lParam);
         ClientToScreen(hwnd, &point);
         canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));   
         ScreenToClient((HWND)CGObject(canvas)->native_object, &point);
         SendMessage((HWND)CGObject(canvas)->native_object, message, wParam, 
                     MAKELPARAM(point.x, point.y));
         return 1;
      }
      return 0;
   case WM_SYSCHAR:
   case WM_CHAR:
      /* echo selected keyboard input through to parent window */       
      window = CGWindow(CObject(this).parent_find(&class(CGWindow)));       
   
      if (GetKeyState(VK_CONTROL) & 0x8000 ||
          GetKeyState(VK_MENU) & 0x8000) {
          filter = FALSE;
      }
      switch (wParam) {
      case 0x08:                     /* Backspace */          
      case 0x09:                     /* Tab */
      case 0x1B:                     /* ESC */
         filter = FALSE;
      }

      if (window && !filter) {
         SendMessage((HWND)CGObject(window)->native_object, message, wParam, lParam);
         return 0;
      }
    case WM_KEYUP:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:       
       return 0;
    }
    
    if (this && CObject_exists((CObject *)this)) {
       return CallWindowProc((WNDPROC)this->wndproc, hwnd, message, wParam, lParam);
    }
    else if (Win32_ATL_wndproc) {
       return CallWindowProc((WNDPROC)Win32_ATL_wndproc, hwnd, message, wParam, lParam);
    }
    return 0;
}

void CGActiveXContainer::show(bool show) {
   class:base.show(show); 
}/*CGActiveXContainer::show*/

void CGActiveXContainer::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   DWORD style = WS_VISIBLE | WS_CHILD | (CGXULElement(this)->disabled ? WS_DISABLED : 0);

   if (!parent) 
      return;
   
   CGXULElement(this).utov(CGCanvas(layout), position);
   
   hwnd =
      CreateWindow("AtlAxWin"/*"AtlAxWin80"*/, CString(&this->activeXClass).string(), style,
                   (int)position[0].x, (int)position[0].y,
                   (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                   parent, NULL, GetModuleHandle(NULL), NULL);
   SetWindowLong(hwnd, GWL_USERDATA, (LONG)this);    
   this->wndproc = (void *)SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGActiveXContainer_WIN32_wnd_proc);    
   Win32_ATL_wndproc = (WNDPROC)this->wndproc;
   Win32_ATL_wndproc_subclassed = (WNDPROC)CGActiveXContainer_WIN32_wnd_proc;

   CGObject(this)->native_object = (void *)hwnd;
   class:base.NATIVE_allocate(layout);
   
   CGActiveXContainer(this).activex_allocate((void *)hwnd);
}/*CGActiveXContainer::NATIVE_allocate*/

void CGActiveXContainer::NATIVE_release(CGLayout *layout) {
   CGWindow *window = CGWindow(CObject(this).parent_find(&class(CGWindow))); 
    
   CGActiveXContainer(this).activex_release(void);
    
   SetWindowLong((HWND)CGObject(this)->native_object, GWL_WNDPROC, (LONG)this->wndproc);
    
   class:base.NATIVE_release(layout);
       
   /*>>>kludge, make sure there is no stray focus on active X component */   
   if (window) {
      CGXULElement(window).NATIVE_focus_in();   
   }
}/*CGActiveXContainer::NATIVE_release*/

void CGActiveXContainer::activex_allocate(void *hwnd) {}
void CGActiveXContainer::activex_release(void) {}

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
