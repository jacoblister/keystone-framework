/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "../../framework_base.c"
#include "../../object.c"
#include "../../event.c"
#include "../gobject.c"
#include "../gcanvas.c"
#include "../gwindow.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <windows.h>
#include <direct.h>
#include "native.h"

void CGWindow_WIN32_resize_canvas(HWND hwnd) {
   RECT client, wsize;
   void *udata = (void *)GetWindowLong(hwnd, GWL_USERDATA);
   CGWindow *this = udata;
   CGXULElement *child = CGXULElement(CObject(this).child_first());

   if (!CObjPersistent(child).attribute_default_get(ATTRIBUTE<CGXULElement,min_width>)) {
      GetClientRect(hwnd, &client);
      AdjustWindowRect(&client, WS_OVERLAPPEDWINDOW, CObject(&this->menu).child_count() && CGObject(&this->menu)->visibility);

      GetWindowRect(hwnd, &wsize);
      wsize.left   -= client.left;
      wsize.top    -= client.top;
      wsize.right  = wsize.left + (int)CGXULElement(child)->min_width;
      wsize.bottom = wsize.top  + (int)CGXULElement(child)->min_height;

      AdjustWindowRect(&wsize, WS_OVERLAPPEDWINDOW, CObject(&this->menu).child_count() && CGObject(&this->menu)->visibility);

      MoveWindow(hwnd,
                 wsize.left, wsize.top,
                 wsize.right - wsize.left, wsize.bottom - wsize.top,
                 TRUE);
   }
   else {
       SendMessage((HWND)CGObject(child)->native_object, WM_SIZE, 0, 0);
   }
}/*CGWindow_WIN32_resize_canvas*/

void CGMenu::NATIVE_allocate(void) {
   CGObject(this)->native_object = (void *)CreateMenu();

   CGMenu(this).NATIVE_item_allocate(NULL);
}/*CGMenu::NATIVE_allocate*/

void CGMenu::NATIVE_release(void) {
}/*CGMenu::NATIVE_release*/

void CGMenu::NATIVE_item_allocate(CGMenuItem *parent) {
   CGMenuItem *item;
   CObject *menu = parent ? CObject(parent) : CObject(this);
   HMENU *hMenu = (HMENU *)&CGObject(menu)->native_object;
   CString label;
   char *cp;

   if (parent == NULL) {
      while (DeleteMenu(CGObject(this)->native_object, 0, MF_BYPOSITION)) {}
   }

   new(&label).CString(NULL);
   item = CGMenuItem(CObject(menu).child_first());
   while (item) {
      if (CGObject(item)->visibility == EGVisibility.visible) {
         if (CString(&item->acceltext).length()) {
            CString(&label).printf("%s\t%s", CString(&CGListItem(item)->label).string(),
                                    CString(&item->acceltext).string());
         }
         else {
            CString(&label).printf("%s", CString(&CGListItem(item)->label).string());
         }
         cp = CString(&label).strchr('_');
         if (cp) {
             *cp = '&';
         }

         if (CObject(item).child_count()) {
            CGObject(item)->native_object = (void *)CreateMenu();
            CGMenu(this).NATIVE_item_allocate(CGMenuItem(item));
            if (CObject(item).obj_class() == &class(CGMenuSeparator)) {
               AppendMenu(*hMenu, MF_SEPARATOR,
                          (UINT_PTR)CGObject(item)->native_object, NULL);
            }
            else {
               AppendMenu(*hMenu, MF_BYPOSITION | MF_POPUP |
                          (CGListItem(item)->disabled ? MF_GRAYED : 0) |
                          (CGListItem(item)->checked ? MF_CHECKED : 0),
                          (UINT_PTR)CGObject(item)->native_object, /*CString(&CGListItem(item)->label).string()*/CString(&label).string());
            }
         }
         else {
            if (CObject(item).obj_class() == &class(CGMenuSeparator)) {
               AppendMenu(*hMenu, MF_SEPARATOR, (UINT_PTR)item, NULL);
            }
            else {
               AppendMenu(*hMenu, MF_BYPOSITION |
                          (CGListItem(item)->disabled ? MF_GRAYED : 0) |
                          (CGListItem(item)->checked ? MF_CHECKED : 0),
                          (UINT_PTR)item, /*CString(&CGListItem(item)->label).string()*/CString(&label).string());
            }
         }
      }
      item = CGMenuItem(CObject(menu).child_next(CObject(item)));
   }
   delete(&label);
}/*CGMenu::NATIVE_item_allocate*/

void CGMenu::NATIVE_item_update(CGMenuItem *item) {
   CGMenuItem *child;
   CObject *parent = CObject(item).parent();
   int index = 0;// = CObject(parent).child_index(CObject(item));
   CGWindow *window = CGWindow(CObject(this).parent());
   UINT menustate, value;
   bool change = FALSE;
   
   child = CGMenuItem(CObject(parent).child_first());
   while (child) {
      if (CGObject(child)->visibility) {
         if (child == item) break;
         index++;
      }
      child = CGMenuItem(CObject(parent).child_next(CObject(child)));
   }

//index = CObject(parent).child_index(CObject(item));

   if (!CGObject(parent)->native_object)
       return;

   menustate = GetMenuState((HMENU)CGObject(parent)->native_object, index, MF_BYPOSITION);
   value = CGListItem(item)->disabled ? MF_GRAYED : MF_ENABLED;
   if (value != (menustate & MF_GRAYED)) {
      EnableMenuItem((HMENU)CGObject(parent)->native_object, index, MF_BYPOSITION | value);
      change = TRUE;
   }
   value = CGListItem(item)->checked ? MF_CHECKED : MF_UNCHECKED;
   if (value != (menustate & MF_CHECKED)) {
      change = TRUE;
      CheckMenuItem((HMENU)CGObject(parent)->native_object, index, MF_BYPOSITION | value);
   }

   if (window && change) {
      DrawMenuBar((HWND)CGObject(window)->native_object);
   }
}/*CGMenu::NATIVE_item_update*/

CGMenuItem *CGMenuPopup::execute(CGCanvas *canvas) {
   HWND hwnd = GCanvas_NATIVE_DATA(canvas)->hwnd;
   POINT point;
   CGObject(this)->native_object = (void *)CreatePopupMenu();


   CGMenu(this).NATIVE_item_allocate(NULL);

   point.x = (LONG)canvas->pointer_position.x;
   point.y = (LONG)canvas->pointer_position.y;
   ClientToScreen(hwnd, &point);

   return (CGMenuItem *)
      TrackPopupMenu((HMENU)CGObject(this)->native_object,
                     TPM_RIGHTBUTTON | TPM_RETURNCMD,
                     point.x, point.y, 0, hwnd, NULL);
}/*CGMenuPopup::execute*/

Global LRESULT CALLBACK CGWindow_WIN32_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
   CGWindow *this;
   CGXULElement *xul_element, *child;
   void *udata = (void *)GetWindowLong(hwnd, GWL_USERDATA);
   RECT client, wsize;
   int w_width, w_height;
   CEventKey event;
   WINDOWPLACEMENT wp;
   MSG msg;

   if (message == WM_CREATE) {
      this = CGWindow(((LPCREATESTRUCT) lParam)->lpCreateParams);
      SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   }
   else {
      this = udata ? udata : NULL;
   }
   
   msg.hwnd = hwnd;
   msg.message = message;
   msg.wParam = wParam;
   msg.lParam = lParam;
   if (CFramework__Win32_translate_key_event(&msg, &event)) {
      if (CGObject(this).event(CEvent(&event)) || event.key == EEventKey.Tab) {
         /* Window has handled event, no default processing */
         return 0;
      }
   }

//   CLEAR(&ui_message);
   switch (message) {
   case WM_CREATE:
      SetWindowLong(hwnd, GWL_USERDATA, (long)this);
      break;
   case WM_CLOSE:
      if (this->disable_close)
         return 0;

      if (CGWindow(this).notify_request_close()) {
         delete(this);
         SetWindowLong(hwnd, GWL_USERDATA, 0);
      }
      return 0;
   case WM_DESTROY:
      return 0;
//>>>paint area around windows canvas area with null colour*/
// case WM_PAINT:
//      BeginPaint(hwnd, &ps);
//(HBRUSH) GetStockObject(LTGRAY_BRUSH); /*>>>for now*/
//   Rectangle(ps.hdc,
//   EndPaint(hwnd, &ps);
//      return 0;
   case 0x020A: //WM_MOUSEWHEEL:
      /* send through to layout in focus */
      if (this->input_focus) {
         SendMessage((HWND)CGObject(this->input_focus)->native_object, message, wParam, lParam);
      }
      break;
   case WM_SIZING:
      child = CGXULElement(CObject(this).child_first());
      wsize.left   = 0;
      wsize.top    = 0;
      wsize.right  = (int)CGXULElement(child)->min_width;
      wsize.bottom = (int)CGXULElement(child)->min_height;
      AdjustWindowRect(&wsize, WS_OVERLAPPEDWINDOW, CObject(&this->menu).child_count() && CGObject(&this->menu)->visibility);

      w_width  = ((LPRECT)lParam)->right  - ((LPRECT)lParam)->left;
      w_height = ((LPRECT)lParam)->bottom - ((LPRECT)lParam)->top;

      if (!CObjPersistent(child).attribute_default_get(ATTRIBUTE<CGXULElement,min_width>)) {
         if (w_width < (wsize.right - wsize.left)) {
            switch (wParam) {
            case WMSZ_BOTTOMLEFT:
            case WMSZ_LEFT:
            case WMSZ_TOPLEFT:
                 ((LPRECT)lParam)->left = ((LPRECT)lParam)->right - (wsize.right - wsize.left);
               break;
            case WMSZ_BOTTOMRIGHT:
            case WMSZ_RIGHT:
            case WMSZ_TOPRIGHT:
                 ((LPRECT)lParam)->right = ((LPRECT)lParam)->left + (wsize.right - wsize.left);
               break;
            }
         }
      }
      if (!CObjPersistent(child).attribute_default_get(ATTRIBUTE<CGXULElement,min_height>)) {
         if (w_height < (wsize.bottom - wsize.top)) {
            switch (wParam) {
            case WMSZ_BOTTOM:
            case WMSZ_BOTTOMLEFT:
            case WMSZ_BOTTOMRIGHT:
                ((LPRECT)lParam)->bottom = ((LPRECT)lParam)->top + (wsize.bottom - wsize.top);
               break;
            case WMSZ_TOP:
            case WMSZ_TOPLEFT:
            case WMSZ_TOPRIGHT:
                ((LPRECT)lParam)->top = ((LPRECT)lParam)->bottom - (wsize.bottom - wsize.top);
               break;
            }
         }
      }

      /* Again for max size */
      w_width  = ((LPRECT)lParam)->right  - ((LPRECT)lParam)->left;
      w_height = ((LPRECT)lParam)->bottom - ((LPRECT)lParam)->top;

      if (!CObjPersistent(child).attribute_default_get(ATTRIBUTE<CGXULElement,max_width>)) {
         if (w_width > (wsize.right - wsize.left)) {
            switch (wParam) {
            case WMSZ_BOTTOMLEFT:
            case WMSZ_LEFT:
            case WMSZ_TOPLEFT:
                 ((LPRECT)lParam)->left = ((LPRECT)lParam)->right - (wsize.right - wsize.left);
               break;
            case WMSZ_BOTTOMRIGHT:
            case WMSZ_RIGHT:
            case WMSZ_TOPRIGHT:
                 ((LPRECT)lParam)->right = ((LPRECT)lParam)->left + (wsize.right - wsize.left);
               break;
            }
         }
      }
      if (!CObjPersistent(child).attribute_default_get(ATTRIBUTE<CGXULElement,max_height>)) {
         if (w_height > (wsize.bottom - wsize.top)) {
            switch (wParam) {
            case WMSZ_BOTTOM:
            case WMSZ_BOTTOMLEFT:
            case WMSZ_BOTTOMRIGHT:
                ((LPRECT)lParam)->bottom = ((LPRECT)lParam)->top + (wsize.bottom - wsize.top);
               break;
            case WMSZ_TOP:
            case WMSZ_TOPLEFT:
            case WMSZ_TOPRIGHT:
                ((LPRECT)lParam)->top = ((LPRECT)lParam)->bottom - (wsize.bottom - wsize.top);
               break;
            }
         }
      }
      break;
    case WM_KILLFOCUS:
//>>>cheating, but killfocus gets sent when a child control is selected */
//       CFramework(&framework).window_active_set(NULL);
       break;
    case WM_SETFOCUS:
       CFramework(&framework).window_active_set(this);
      if (!this->input_focus) {
//          WARN("Window:no focus layout");
      }
      else {
         CGLayout(this->input_focus).key_gselection_refresh();
      }
      break;
    case WM_SHOWWINDOW:
      return 0;
    case WM_SIZE:
    case WM_MOVE:
      GetWindowRect(hwnd, &client);
      GetWindowPlacement(hwnd, &wp);

      CObjPersistent(this).attribute_update(ATTRIBUTE<CGWindow,maximized>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGWindow,placement>);

      if (!this->fullscreen && !this->maximized && wp.showCmd != SW_MAXIMIZE &&
         wp.showCmd != SW_MINIMIZE && wp.showCmd != SW_SHOWMINIMIZED) {
         this->placement.point[0].x = client.left;
         this->placement.point[0].y = client.top;
         this->placement.point[1].x = client.right;
         this->placement.point[1].y = client.bottom;
      }
      this->maximized = (wParam == SIZE_MAXIMIZED);
      CObjPersistent(this).attribute_update_end();

      if (message == WM_SIZE) {
         child = CGXULElement(CObject(this).child_first());
         GetClientRect(hwnd, &client);
         if (child) {
//        CGCanvas(child).NATIVE_size_allocate(0, 0, client.right - client.left, client.bottom - client.top);
            MoveWindow((HWND)CGObject(child)->native_object,
                       client.left, client.top,
                       client.right - client.left, client.bottom - client.top,
                       TRUE);
          }
          else {
              WARN("window:size no canvas\n");
          }
//          return 0;
      }
      break;
   case WM_SYSCOMMAND:
      if (wParam == SC_KEYMENU && lParam == 0) {
         /* For now, ignore this message completely, causes window menu to be opened for keyboard access */
         return 0;
      }
      switch (wParam) {
      case SC_MAXIMIZE:
         if (this->fullscreen) {
            SetWindowLong((HWND)CGObject(this)->native_object, GWL_STYLE, WS_POPUP | WS_VISIBLE);
         }
         break;
      case SC_RESTORE:
         break;
      }
      break;
   case WM_COMMAND:
      if (lParam == 0) {
         CGMenu(&this->menu).menu_item_select((CGMenuItem*)wParam);
         break;
      }
      /*>>>problem in template expanstion when parameter contains ',' character*/
      udata = (void *)GetWindowLong((void *)lParam, GWL_USERDATA);
      if (udata) {
         xul_element = CGXULElement(udata);
         return CGXULElement(xul_element).WIN32_XULELEMENT_MSG(message, wParam, lParam);
      }
      else {
      }
      break;
   }
   return DefWindowProc(hwnd, message, wParam, lParam);
}/*CGWindow_WIN32_wnd_proc*/

void CGWindow::topmost(bool topmost, bool stick) {
   if (stick) {
//>>>   SetWindowPos((HWND)CGObject(this)->native_object, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
      this->topmost = topmost;
   }
   else {
      BringWindowToTop((HWND)CGObject(this)->native_object);
   }
}/*CGWindow::topmost*/

void CGWindow::disable_close(bool disable) {
   HMENU hMenu = GetSystemMenu((HWND)CGObject(this)->native_object, FALSE);
   EnableMenuItem(hMenu, SC_CLOSE, disable ? MF_GRAYED : MF_ENABLED);
   this->disable_close = disable;
}/*CGWindow::disable_close*/

void CGWindow::disable_maximize(bool disable) {
   LONG winstyle;
   winstyle = GetWindowLongW((HWND)CGObject(this)->native_object, GWL_STYLE);
   winstyle &= (0xFFFFFFFF ^ WS_MAXIMIZEBOX); 
   SetWindowLongW((HWND)CGObject(this)->native_object, GWL_STYLE, winstyle);
}/*CGWindow::disable_maximize*/

void CGWindow::disable_minimize(bool disable) {
   LONG winstyle;
   winstyle = GetWindowLongW((HWND)CGObject(this)->native_object, GWL_STYLE);
   winstyle &= (0xFFFFFFFF ^ WS_MINIMIZEBOX); 
   SetWindowLongW((HWND)CGObject(this)->native_object, GWL_STYLE, winstyle);
}/*CGWindow::disable_minimize*/

void CGWindow::modal(bool modal) {
   CGWindow *app_window;
   int i;

   framework.modal_window = NULL;

   if (this->modal == modal)
      return;

   this->modal = modal;

//   CGWindow(this).disable_close(this->modal);

   if (modal) {
       framework.modal_window = this;
   }
   else {
      for (i = 0; i < ARRAY(&framework.window).count(); i++) {
         app_window = ARRAY(&framework.window).data()[i];
         if (app_window->modal) {
            framework.modal_window = app_window;
         }
      }
   }

   for (i = 0; i < ARRAY(&framework.window).count(); i++) {
      app_window = ARRAY(&framework.window).data()[i];
      if (framework.modal_window) {
         EnableWindow((HWND)CGObject(app_window)->native_object, framework.modal_window == app_window);
      }
      else {
         EnableWindow((HWND)CGObject(app_window)->native_object, TRUE);
      }
  }
}/*CGWindow::modal*/

void CGWindow::NATIVE_maximize(bool maximize) {
   SendMessage((HWND)CGObject(this)->native_object, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
}/*CGWindow::maximize*/

void CGWindow::NATIVE_fullscreen(bool fullscreen) {
   if (fullscreen) {
      CGWindow(this).NATIVE_maximize(TRUE);
   }
   else {
      SetWindowLong((HWND)CGObject(this)->native_object, GWL_STYLE, WS_VISIBLE | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW);
      MoveWindow((HWND)CGObject(this)->native_object,
                 (int)this->placement.point[0].x, (int)this->placement.point[0].y,
                 (int)(this->placement.point[1].x - this->placement.point[0].x),
                 (int)(this->placement.point[1].y - this->placement.point[0].y),
                 TRUE);
      CGWindow(this).NATIVE_maximize(TRUE);
   }
}/*CGWindow::fullscreen*/

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x80000
#define LWA_COLORKEY 1
#define LWA_ALPHA 2
#endif

typedef DWORD (WINAPI *PSLWA)(HWND, DWORD, BYTE, DWORD);

void CGWindow::NATIVE_opacity(double opacity) {
   PSLWA pSetLayeredWindowAttributes;
   HMODULE hDLL = GetModuleHandle(TEXT("user32"));
   pSetLayeredWindowAttributes = (PSLWA) GetProcAddress(hDLL,"SetLayeredWindowAttributes");

   SetWindowLong((HWND)CGObject(this)->native_object, GWL_EXSTYLE, GetWindowLong((HWND)CGObject(this)->native_object,GWL_EXSTYLE ) | WS_EX_LAYERED);
   if (pSetLayeredWindowAttributes != NULL) {
       pSetLayeredWindowAttributes ((HWND)CGObject(this)->native_object, RGB(255,255,255), (BYTE)(opacity * 255), /*LWA_COLORKEY|*/LWA_ALPHA);
   }
}/*CGWindow::NATIVE_opacity*/

void CGWindow::size_set(int width, int height) {
   RECT wsize;
   HWND hwnd = (HWND)CGObject(this)->native_object;

   GetWindowRect(hwnd, &wsize);
   wsize.right  = wsize.left + width;
   wsize.bottom = wsize.top + height;

   MoveWindow(hwnd,
              wsize.left, wsize.top,
              wsize.right - wsize.left, wsize.bottom - wsize.top,
              TRUE);
}/*CGWindow::size_set*/

void CGWindow::position_set(int x, int y) {
   RECT wsize;
   HWND hwnd = (HWND)CGObject(this)->native_object;

   GetWindowRect(hwnd, &wsize);

   MoveWindow(hwnd, x, y, wsize.right - wsize.left, wsize.bottom - wsize.top,
              TRUE);
}/*CGWindow::position_set*/

void CGWindow::position_size_set(int x, int y, int width, int height) {
   HWND hwnd = (HWND)CGObject(this)->native_object;

   MoveWindow(hwnd, x, y, width, height, TRUE);
}/*CGWindow::position_size_set*/

void CGWindow::NATIVE_new(void) {
   static TCHAR szClassName[] = TEXT("GRWindow");
   WNDCLASS cls;
   RECT wsize = {0, 0, 200 - 1, 200 - 1};
   HWND parent = NULL, *hwnd = (HWND *)&CGObject(this)->native_object;
   DWORD style, style_ex;
   int x_pos, y_pos;
   HICON icon;
   CGXULElement *child = CGXULElement(CObject(this).child_first());
   TRect mon_area;

   cls.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
   cls.lpfnWndProc   = CGWindow_WIN32_wnd_proc;
   cls.cbClsExtra    = 0;
   cls.cbWndExtra    = sizeof(long);
   cls.hInstance     = GetModuleHandle(NULL);
   cls.hIcon         = NULL;
   cls.hbrBackground = NULL;
   cls.lpszMenuName  = NULL;
   cls.lpszClassName = szClassName;
   cls.hCursor       = LoadCursor(NULL, IDC_ARROW);

   style = WS_OVERLAPPEDWINDOW;//>>> | WS_HSCROLL | WS_VSCROLL;
   style_ex = 0;

   RegisterClass(&cls);

   if (!CObjPersistent(child).attribute_default_get(ATTRIBUTE<CGXULElement,min_width>)) {
      wsize.right  = (int)CGXULElement(child)->min_width  - 1;
      wsize.bottom = (int)CGXULElement(child)->min_height - 1;
      style = WS_OVERLAPPEDWINDOW;
   }

   if (this->nodecoration) {
       style = WS_POPUP;
   }

   /* Create the main window here */
   AdjustWindowRect(&wsize, style, FALSE);

#if 1
   if (this->parent) {
      CGWindow(this->parent).NATIVE_monitor_area(&mon_area);
   }
   else {
      CGWindow(this).NATIVE_monitor_area(&mon_area);
   }
   x_pos = (int)((mon_area.point[0].x + mon_area.point[1].x) / 2) - ((wsize.right  - wsize.left + 1) / 2);
   y_pos = (int)((mon_area.point[0].y + mon_area.point[1].y) / 2) - ((wsize.bottom - wsize.top + 1) / 2);
#else
   x_pos = (GetSystemMetrics(SM_CXSCREEN) - (wsize.right  - wsize.left + 1)) / 2;
   y_pos = (GetSystemMetrics(SM_CYSCREEN) - (wsize.bottom - wsize.top  + 1)) / 2;
#endif

   if (this->parent) {
       parent = (HWND)CGObject(this->parent)->native_object;
   }
   *hwnd =
      CreateWindowEx(style_ex, szClassName, NULL, style,
                     x_pos, y_pos, wsize.right - wsize.left + 1, wsize.bottom - wsize.top + 1,
                     parent, 0, GetModuleHandle(NULL), (LPVOID)this);

   /* Set Icon (For now, set from window attribute) */
   icon = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(10100), IMAGE_ICON, 32, 32, 0);
   this->native_data[0] = (void *)icon;
   SendMessage(*hwnd, WM_SETICON, ICON_BIG, (WPARAM)icon);
   icon = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(10100), IMAGE_ICON, 16, 16, 0);
   this->native_data[1] = (void *)icon;
   SendMessage(*hwnd, WM_SETICON, ICON_SMALL, (WPARAM)icon);
   CGMenu(&this->menu).NATIVE_allocate();
}/*CGWindow::NATIVE_new*/

void CGWindow::NATIVE_delete(void) {
   DestroyIcon(this->native_data[0]);
   DestroyIcon(this->native_data[1]);

   if (this->modal) {
      CGWindow(this).modal(FALSE);
   }
   DestroyWindow((HWND)CGObject(this)->native_object);

//   /*>>>cheap! this shouldn't be needed, and causes flicker.  Why isn't the parent window automatically selected on destroy?*/
   if (CGObject(this)->visibility == EGVisibility.visible && this->parent) {
      BringWindowToTop((HWND)CGObject(this->parent)->native_object);
   }
}/*CGWindow::NATIVE_delete*/

void CGWindow::NATIVE_show(bool show) {
   if (show) {
      CGWindow_WIN32_resize_canvas((HWND)CGObject(this)->native_object);
      ShowWindow((HWND)CGObject(this)->native_object, this->nodecoration ? SW_SHOWNA : SW_SHOW);
      UpdateWindow((HWND)CGObject(this)->native_object);

      if (CGObject(&this->menu)->visibility) {
         SetMenu(CGObject(this)->native_object, (HMENU)CGObject(&this->menu)->native_object);
      }
      else {
         SetMenu(CGObject(this)->native_object, NULL);
      }
   }
   else {
      ShowWindow((HWND)CGObject(this)->native_object, SW_HIDE);
   }
}/*CGWindow::NATIVE_show*/

void CGWindow::NATIVE_title_set(const char *title) {
   SetWindowText((HWND)CGObject(this)->native_object, title);
}/*CGWindow::NATIVE_title_set*/

void CGWindow::NATIVE_extent_set(TRect *extent) {
   SetWindowPos((HWND)CGObject(this)->native_object, NULL, (int)extent->point[0].y, (int)extent->point[0].x,
                (int)extent->point[1].x - (int)extent->point[0].x, (int)extent->point[1].y - (int)extent->point[0].y,
                SWP_NOMOVE | SWP_NOZORDER);
}/*CGWindow::NATIVE_extent_set*/

void CGWindow::NATIVE_show_help(CString *help_topic) {
   CString cmd;

   new(&cmd).CString(NULL);
   CString(&cmd).printf("hh mk:@MSITStore:%s", CString(help_topic).string());
   CThread(CFramework(&framework).thread_this()).spawn(CString(&cmd).string());
   delete(&cmd);
}/*CGWindow::NATIVE_show_help*/


#ifndef MONITOR_DEFAULTTONULL
#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002
#define MONITORINFOF_PRIMARY        0x00000001
typedef struct tagMONITORINFO
{
   DWORD   cbSize;
    RECT    rcMonitor;
    RECT    rcWork;
    DWORD   dwFlags;
} MONITORINFO, *LPMONITORINFO;
#endif
typedef BOOL (WINAPI *PGMI)(HANDLE, LPMONITORINFO);
typedef HANDLE (WINAPI *PMFW)(HWND, DWORD);
void CGWindow::NATIVE_monitor_area(TRect *result) {
   HANDLE monitor;
   MONITORINFO info;
   PGMI pGetMonitorInfo;
   PMFW pMonitorFromWindow;
   HMODULE hDLL = GetModuleHandle(TEXT("user32"));
   pGetMonitorInfo = (PGMI) GetProcAddress(hDLL,"GetMonitorInfoA");
   pMonitorFromWindow = (PMFW) GetProcAddress(hDLL,"MonitorFromWindow");

   monitor = pMonitorFromWindow((HWND)CGObject(this)->native_object, MONITOR_DEFAULTTONEAREST);
   info.cbSize = sizeof(MONITORINFO);
   pGetMonitorInfo(monitor, &info);

   result->point[0].x = info.rcWork.left;
   result->point[0].y = info.rcWork.top;
   result->point[1].x = info.rcWork.right;
   result->point[1].y = info.rcWork.bottom;
}/*CGWindow::NATIVE_monitor_area*/

bool CGDialogFileSelect::execute(CString *filename) {
   OPENFILENAME ofn;       // common dialog box structure
   char cwd[MAX_PATH];
   char szFile[260];       // buffer for file name
   char szFilter[260], *cp;       // buffer for file filter
   bool select_result;
   CGMenuItem *item;
   char sf_name[MAX_PATH], sf_ext[_MAX_EXT];
   bool fullpath;
   unsigned int i;

   memset(szFilter, 0, sizeof(szFilter));
   cp = szFilter;
   item = CGMenuItem(CObject(&this->filter).child_first());
   while (item) {
      sprintf(cp, "%s", CString(&CGListItem(item)->label).string());
      cp += CString(&CGListItem(item)->label).length();
      *cp = '\0';
      cp++;
      sprintf(cp, "%s", CString(&item->acceltext).string());
      cp += CString(&item->acceltext).length();
      *cp = '\0';
      cp++;
      item = CGMenuItem(CObject(&this->filter).child_next(CObject(item)));
   }
   *cp = '\0';
   cp++;

   // Initialize OPENFILENAME
   ZeroMemory(&szFile, sizeof(szFile));
   ZeroMemory(&ofn, sizeof(OPENFILENAME));
   sprintf(szFile, CString(filename).string());
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = this->parent ? (HWND)CGObject(this->parent)->native_object : NULL;
   ofn.lpstrFile = szFile;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = szFilter;//"All\0*.*\0Text\0*.TXT\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = (LPSTR)_getcwd(cwd, MAX_PATH);//NULL;
   ofn.lpstrTitle = this->title;
   ofn.Flags = OFN_PATHMUSTEXIST;// | OFN_FILEMUSTEXIST;

   // Display the Open dialog box.

   CString(filename).clear();

   switch (this->mode) {
   case EDialogFileSelectMode.open:
      select_result = GetOpenFileName(&ofn);
      break;
   case EDialogFileSelectMode.save:
      ofn.Flags |= OFN_NOCHANGEDIR;
      select_result = GetSaveFileName(&ofn);
      break;
   default:
      select_result = FALSE;
      break;
   }

   fullpath = TRUE;
   cp = strrchr(szFile, '\\');
   if (cp) {
      memset(szFilter, 0, sizeof(szFilter));
      strncpy(szFilter, szFile, cp - szFile);
      
      if (strlen(szFilter) == strlen(cwd)) {
         for (i = 0; i < strlen(szFilter); i++) {
            if (toupper(szFilter[i]) != toupper(cwd[i])) {
               break;
            }
         }
         if (i == strlen(szFilter)) {
            fullpath = FALSE;
         }
      }
   }

   /* If extension not specified, use selected filter list item */
   _splitpath(szFile, NULL, NULL, sf_name, sf_ext);
   item = CGMenuItem(CObject(&this->filter).child_n(ofn.nFilterIndex - 1));
   if (!strlen(sf_ext) && item) {
      sprintf(sf_ext, &CString(&item->acceltext).string()[1]);
      sprintf(szFile + strlen(szFile), "%s", sf_ext);
   }

   CGWindow(this).modal(FALSE);

   if (select_result == TRUE) {
      if (!fullpath) {
          sprintf(szFile, "%s%s", sf_name, sf_ext);
      }
      CString(filename).set(szFile);
      return TRUE;
   }

   return FALSE;
}/*CGDialogFileSelect::execute*/

int CGDialogChooseColour::execute(void) {
   CHOOSECOLOR cc;
   COLORREF crCustColors[16];
   bool result;
   TGColour colour;

   if (this->native_dialog) {
      CObjPersistent(this->object->object).attribute_get(this->object->attr.attribute,
                                                         this->object->attr.element,
                                                         this->object->attr.index,
                                                         &ATTRIBUTE:type<TGColour>, (void *)&colour);
      colour = GCOL_CONVERT_RGB(this->palette, this->paletteB, colour);

      cc.lStructSize = sizeof(CHOOSECOLOR);
      cc.hwndOwner = CGWindow(this)->parent ? ((HWND)CGObject(CGWindow(this)->parent)->native_object) : NULL;
      cc.hInstance = NULL;
      cc.rgbResult = RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour));
      cc.lpCustColors = crCustColors;
      cc.Flags = CC_RGBINIT | CC_FULLOPEN;
      cc.lCustData = 0;
      cc.lpfnHook = NULL;
      cc.lpTemplateName = NULL;
      memset(crCustColors, 0xFF, sizeof(crCustColors));

      result = ChooseColor(&cc);

      if (result) {
         colour = GCOL_RGB(GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
         CObjPersistent(this->object->object).attribute_update(this->object->attr.attribute);
         CObjPersistent(this->object->object).attribute_set(this->object->attr.attribute,
                                                            this->object->attr.element,
                                                            this->object->attr.index,
                                                            &ATTRIBUTE:type<TGColour>, (void *)&colour);
         CObjPersistent(this->object->object).attribute_update_end();
      }
   }

   return 0;
}/*CGWindowDialog::execute*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
