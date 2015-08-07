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
#include "../gxulelement.c"
#include "../gobject.c"
#include "../glayout.c"
#include "../gproperties.c"
#include "../gwindow.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <windows.h>
#include <commctrl.h>

#include "native.h"

#define ATTR_DEFAULT_COLOUR RGB(0xA0, 0xA0, 0xA0)
#define MENULIST_HEIGHT 300

typedef BOOL(WINAPI *PITDTE)(HWND);
#if 0
/* Win XP theme API >>>may need to use in future */
typedef BOOL(WINAPI *PITDTE)(HWND);
typedef HRESULT(WINAPI *PDTPB)(HWND, HDC, const RECT *);
   PITDTE pIsThemeDialogTextureEnabled;
   PDTPB pDrawThemeParentBackground;
   HMODULE hDLL = GetModuleHandle(TEXT("uxtheme"));
   pIsThemeDialogTextureEnabled = (PITDTE) GetProcAddress(hDLL, "IsThemeDialogTextureEnabled");
   pDrawThemeParentBackground = (PDTPB) GetProcAddress(hDLL, "DrawThemeParentBackground");

   if (pDrawThemeParentBackground != NULL) {
       pDrawThemeParentBackground((HWND)lParam, (HDC)wParam, NULL);
       /*>>>minor kludge, transparnt if child of tab for XP visual styles */
   }
#endif

extern WNDPROC Win32_ATL_wndproc;
extern WNDPROC Win32_ATL_wndproc_subclassed;

Local LRESULT CALLBACK CGXULElement_WIN32_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
   CGXULElement *this;
   CGCanvas *canvas = NULL;
   CGWindow *window;
   TGColour colour;
   HWND parent;
   RECT rect;
   HDC hdc;
   HBRUSH hBrush;
   HPEN hPen;
   WNDPROC wndproc = (WNDPROC)GetClassLong(hwnd, GCL_WNDPROC);
   LRESULT result;
   bool filter, filter_framework;
   TRACKMOUSEEVENT trackmouseevent;

   this = (void *)GetWindowLong(hwnd, GWL_USERDATA);
   if (!this) {
      return CallWindowProc(wndproc, hwnd, message, wParam, lParam);
   }

   switch (message) {
   case WM_RBUTTONUP:
      if (CObject(this).obj_class() == &class(CGColourPicker)) {
         /*foward message for common handling */
         CGXULElement(this).WIN32_XULELEMENT_MSG(message, 0, 0);
         break;
      }
      goto mouse_event;
   case WM_MOUSEMOVE:
      CLEAR(&trackmouseevent);
      trackmouseevent.cbSize = sizeof(TRACKMOUSEEVENT);
      trackmouseevent.dwFlags = TME_LEAVE;
      trackmouseevent.hwndTrack = hwnd;
      trackmouseevent.dwHoverTime = HOVER_DEFAULT;
      _TrackMouseEvent(&trackmouseevent);
      /* fallthrough */
   case WM_LBUTTONDOWN:
   case WM_LBUTTONUP:
      if (CObject(this).obj_class() == &class(CGDatePickerButton)
           && message == WM_LBUTTONUP) {
         /*foward message for common handling */
         CGXULElement(this).WIN32_XULELEMENT_MSG(WM_COMMAND, 0, 0);
         return 0;
      }
   case WM_LBUTTONDBLCLK:
   case WM_RBUTTONDOWN:
//   case WM_RBUTTONUP:
   case WM_MOUSELEAVE:
//   case 0x020A: //WM_MOUSEWHEEL:   
   mouse_event:
      if (CObject(this).obj_class() == &class(CGRadioButton)) {
          CGXULElement(this)->updating = FALSE;
      }

      /* don't repeat events inside ActiveX container */
      parent = GetParent(hwnd);
      if (parent) {
          if (Win32_ATL_wndproc_subclassed && ((WNDPROC)GetWindowLong(parent, GWL_WNDPROC) == Win32_ATL_wndproc_subclassed)) {
             break;
         }
      }
      /* repeat mouse events to parent canvas */
      canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
      SendMessage((HWND)CGObject(canvas)->native_object, message, (0xFFFF)/*wParam*/,
                  MAKELPARAM(LOWORD(lParam) + this->x, HIWORD(lParam) + this->y));
      break;
   case WM_CHAR:
   case WM_SYSCHAR:
   case WM_KEYDOWN:
   case WM_SYSKEYDOWN:
   case WM_KEYUP:
   case WM_SYSKEYUP:
      if (CObject(this).obj_class() == &class(CGRadioButton)) {
          CGXULElement(this)->updating = FALSE;
      }
      filter = FALSE;
      result = 0;
      filter_framework = FALSE;

      /* Filter select keypresses from Win32 default handling */
      if (GetKeyState(VK_MENU) & 0x8000) {
         filter = TRUE;
      }
      if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) {
         switch (wParam) {
         case VK_BACK:
            filter_framework = TRUE;
            break;
         case VK_HOME:
         case VK_END:
         case VK_NEXT:
         case VK_PRIOR:
         case VK_DOWN:
         case VK_UP:
            if (CObject(this).obj_class() == &class(CGSlider) ||
               CObject(this).obj_class() == &class(CGTree) ||
               CObject(this).obj_class() == &class(CGTextBox) && CGTextBox(this)->multiline) {
               filter_framework = TRUE;
            }
            else {
               filter = TRUE;
            }
            break;
         case VK_LEFT:
         case VK_RIGHT:
            if (CObject(this).obj_class() == &class(CGSlider) ||
               CObject(this).obj_class() == &class(CGTree)) {
               filter = TRUE;
            }
            if (CObject(this).obj_class() == &class(CGTextBox)) {
               filter_framework = TRUE;
            }
            break;
         case VK_RETURN:
            if (CObjClass_is_derived(&class(CGTextBox), CObject(this).obj_class()) &&
                CGTextBox(this)->multiline) {
               filter_framework = TRUE;
            }
            if (CObjClass_is_derived(&class(CGButton), CObject(this).obj_class())) {
               /* Make 'Enter' press button as well */
               SendMessage((HWND)CGObject(this)->native_object, BM_CLICK, 0, 0);
            }
            break;
         }
      }
      if (message == WM_CHAR) {
         if (GetKeyState(VK_CONTROL) & 0x8000) {
            switch (wParam) {
            case 'C' - (65-1):
            case 'V' - (65-1):
            case 'X' - (65-1):
               filter = FALSE;
               break;
            default:
               filter = TRUE;
               break;
            }
         }
         switch (wParam) {
         case 0x0D:                     /* CR */
            if (CObject(this).obj_class() == &class(CGTextBox) && !CGTextBox(this)->multiline) {
//               SendMessage((HWND)CGObject(this)->native_object, WM_CHAR, 0x09, 0);
               canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
               SendMessage((HWND)CGObject(canvas)->native_object, WM_COMMAND, MAKEWPARAM(EN_KILLFOCUS, EN_KILLFOCUS), (LPARAM)CGObject(this)->native_object);
            }
            if (CObject(this).obj_class() == &class(CGTextBox) && CGTextBox(this)->multiline) {
               /* pass enter through to multiline text entry boxes */
               filter_framework = TRUE;
               break;
            }
         case 0x08:                     /* Backspace */
            filter_framework = TRUE;
            break;
         case 0x09:                     /* Tab */
         case 0x1B:                     /* ESC */
            filter = TRUE;
         }
      }

      if (CObject(this).obj_class() == &class(CGTextBox)) {
         if (CGTextBox(this)->type == EGTextBoxType.numeric && !(GetKeyState(VK_CONTROL) & 0x8000)) {
            if (message == WM_CHAR && !(isdigit(wParam) || wParam == '.' || wParam == '-' || wParam == 0x08)) {
               filter = TRUE;
            }
         }
         if (CGTextBox(this)->type == EGTextBoxType.numericSpace && !(GetKeyState(VK_CONTROL) & 0x8000)) {
            if (message == WM_CHAR && !(isdigit(wParam) || wParam == '.' || wParam == '-' || wParam == 0x08 || wParam == ' ')) {
               filter = TRUE;
            }
         }

         if (message == WM_CHAR && wParam == ('A' - (65-1)) && (GetKeyState(VK_CONTROL) & 0x8000)) {
            SendMessage((HWND)CGObject(this)->native_object, EM_SETSEL, 0, -1);
         }
#if 0
         /*>>> overtype mode, not working yet */
         else if (message == WM_CHAR && !filter &&
                  !(GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000)) {
            SendMessage((HWND)CGObject(this)->native_object, WM_KEYDOWN, VK_DELETE, 0);
         }
#endif
      }

      window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
      if (!filter) {
         result = CallWindowProc(wndproc, hwnd, message, wParam, lParam);
      }

      if (!filter_framework) {
         /* pass keyboard input through to parent window */
         if (window) {
            SendMessage((HWND)CGObject(window)->native_object, message, wParam, lParam);
         }
      }
      return result;
   case WM_SETFOCUS:
      if (CObject(this).obj_class() == &class(CGRadioButton)) {
          CGXULElement(this)->updating = TRUE;
      }
      canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
      CGLayout(canvas).key_gselection_set(CGObject(this));
//   case WM_ERASEBKGND:
//      break;
   case WM_KILLFOCUS:
   case WM_PAINT:
   case BM_SETSTATE:
      if (CObject(this).obj_class() == &class(CGColourPicker)) {
         if (CGXULElement(this)->binding.object.object) {
            if (CObjClass_is_derived(&class(CGCanvas), CObject(CGXULElement(this)->binding.object.object).obj_class())) {
                canvas = CGCanvas(CGXULElement(this)->binding.object.object);
            }
            else {
                canvas = CGCanvas(CObject(CGXULElement(this)->binding.object.object).parent_find(&class(CGCanvas)));
            }
         }
         if (!canvas) {
            canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
         }
         CallWindowProc(wndproc, hwnd, message, wParam, lParam);
         hdc = GetDC(hwnd);
         GetClientRect(hwnd, &rect);
         /*>>>any better info avaliable on button geometry?*/
         rect.left += 6;
         rect.top += 6;
         rect.right -= 6;
         rect.bottom -= 6;
         if (CGColourPicker(this)->colour == GCOL_NONE) {
            if (this->disabled) {
               colour = GCOL_CONVERT_RGB(canvas->palette, canvas->paletteB, (GCOL_TYPE_NAMED | EGColourNamed.grey));
            }
            else {
               colour = GCOL_CONVERT_RGB(canvas->palette, canvas->paletteB, (GCOL_TYPE_NAMED | EGColourNamed.palevioletred));
            }
            hPen = CreatePen(PS_SOLID, 0, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
            SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
            SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, rect.left, rect.top, rect.right + 1, rect.bottom + 1);
            MoveToEx(hdc, rect.left, rect.top, NULL);
            LineTo(hdc, rect.right, rect.bottom);
            MoveToEx(hdc, rect.right, rect.top, NULL);
            LineTo(hdc, rect.left, rect.bottom);
            DeleteObject(SelectObject(hdc, GetStockObject(NULL_PEN)));
         }
         else {
            colour = GCOL_CONVERT_RGB(canvas->palette, canvas->paletteB, CGColourPicker(this)->colour);
            if (this->disabled) {
               SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
               hBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
            }
            else if (this->isdefault) {
               SetBkColor(hdc, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
               hBrush = CreateHatchBrush(HS_DIAGCROSS, GetSysColor(COLOR_BTNFACE));
            }
            else {
               hBrush = CreateSolidBrush(RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
            }
            SelectObject(hdc, hBrush);
            FillRect(hdc, &rect, hBrush);
            DeleteObject(SelectObject(hdc, GetStockObject(NULL_BRUSH)));
         }
         ReleaseDC(hwnd, hdc);
         return 0;
      }
#if 0
      /*>>>experimental overwrite  of buttons, drop lists, not working, so leave for now */
      else if (CObject(this).obj_class() == &class(CGButton) || CObject(this).obj_class() == &class(CGMenuList)) {
         CallWindowProc(wndproc, hwnd, message, wParam, lParam);
         hdc = GetDC(hwnd);
         SetBkMode(hdc, TRANSPARENT);
         SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
         GetTextExtentPoint32(hdc, "colour", 6, &size);
         TextOut(hdc,
                (this->width / 2) - size.cx + 4,
                (this->height / 2) - size.cy + 4,
                "colour", 6);
         return 0;
      }
#endif
      break;
   }

   return CallWindowProc(wndproc, hwnd, message, wParam, lParam);
};

void CGXULElement::utov(CGCanvas *canvas, TPoint *result) {
   result[0].x = this->x;
   result[0].y = this->y;
   result[1].x = this->x + this->width;
   result[1].y = this->y + this->height;

   if (this->min_width < 0) {
      /*no transform on marked XUL element, >>>should be a seperate flag for this */
   }
   else {
      CGCanvas(canvas).transform_set_gobject(CGObject(this), TRUE);
      CGCoordSpace(&canvas->coord_space).point_array_utov(2, result);
   }
}/*CGXULElement::utov*/

bool CGXULElement::allocated_position(TRect *result) {
   RECT position;

   if (CGObject(this)->native_object && GetWindowRect((HWND)CGObject(this)->native_object, &position)) {
      result->point[0].x = position.left;
      result->point[0].y = position.top;
      result->point[1].x = position.right;
      result->point[1].y = position.bottom;
      return TRUE;
   }

   return FALSE;
}/*CGXULElement::allocated_position*/

void CGXULElement::show(bool show) {
   CGLayout *layout;
   CGWindow *window;

   layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   
   if (!layout && CObjClass_is_derived(&class(CGLayout), CObject(this).obj_class())) {
      layout = CGLayout(this);
   }

   if (!layout)
      return;

#if 0   
   /* for now - only show XUL elements while parent layout is base states */
   if (CObject_exists((CObject *)&layout->fsm) &&
       !CFsm(&layout->fsm).in_state((STATE)&CGLayout::state_animate)) {
      show = FALSE;
   }
#endif   
   /* don't allocate elements unless there is a parent */

   window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
   if (!window)
      return;

   if (show) {
      if (!CGObject(this)->native_object) {
         CGXULElement(this).NATIVE_allocate(layout);
         ShowWindow((HWND)CGObject(this)->native_object, CGObject(this)->visibility ? SW_SHOWNA : SW_HIDE);
      }
   }
   else {
      if (CGObject(this)->native_object) {
         CGXULElement(this).NATIVE_release(layout);
      }
   }
}/*CGXULElement::show*/

void CGXULElement::NATIVE_draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
//   InvalidateRect((HWND)CGObject(this)->native_object, NULL, 0);
}/*CGXULElement::NATIVE_draw*/

void CGXULElement::NATIVE_release(CGLayout *layout) {
   SetWindowLong((HWND)CGObject(this)->native_object, GWL_USERDATA, 0);
   DestroyWindow((HWND)CGObject(this)->native_object);

   CGObject(this)->native_object = NULL;
}/*CGXULElement::NATIVE_release*/

bool CGXULElement::NATIVE_focus_current(void) {
   return (HWND)CGObject(this)->native_object == GetFocus();
}/*CGXULElement::NATIVE_focus_current*/

void CGXULElement::NATIVE_focus_in(void) {
   SetFocus((HWND)CGObject(this)->native_object);
   if (CObjClass_is_derived(&class(CGTextBox), CObject(this).obj_class()) && !CGTextBox(this)->multiline) {
      SendMessage((HWND)CGObject(this)->native_object, EM_SETSEL, 0, -1);
   }
}/*CGXULElement::NATIVE_focus_in*/

void CGXULElement::NATIVE_focus_out(void) {
   SetFocus(NULL);
}/*CGXULElement::NATIVE_focus_out*/

void CGXULElement::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CGLayout *layout;
   TPoint position[2];
   bool visibility;
   CObject *parent;
   
   visibility = EGVisibility.visible;
   parent = CObject(this);
   while (parent) {
      if (CObjClass_is_derived(&class(CGObject), CObject(parent).obj_class()) && CGObject(parent)->visibility == EGVisibility.hidden) {
         visibility = EGVisibility.hidden;
      }
      parent = CObject(parent).parent();
   }

   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);

   if (CGObject(this)->native_object && !changing) {
      layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));

      if (!CObjClass_is_derived(&class(CGCanvas), CObject(this).obj_class()) &&
          !CObjClass_is_derived(&class(CGWindow), CObject(this).obj_class())) {
         ShowWindow((HWND)CGObject(this)->native_object, visibility ? SW_SHOWNA : SW_HIDE);
         EnableWindow((HWND)CGObject(this)->native_object, !this->disabled);
      }

      if (CObjClass_is_derived(&class(CGWindow), CObject(this).obj_class())) {
          return;
      }

      if (layout &&
         !CObjClass_is_derived(&class(CGContainer), CObject(CObject(this).parent()).obj_class()) ||
         (CObjClass_is_derived(&class(CGContainer), CObject(CObject(this).parent()).obj_class()) && CGContainer(CObject(this).parent())->allow_child_repos) ||
          CObjClass_is_derived(&class(CGPacker), CObject(CObject(this).parent()).obj_class())) {

         CGXULElement(this).utov(CGCanvas(layout), position);

         if (CObject(this).obj_class() == &class(CGMenuList)) {
            position[1].y = position[0].y + MENULIST_HEIGHT;
         }

         MoveWindow((HWND)CGObject(this)->native_object,
                    (int)position[0].x, (int)position[0].y,
                    (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                    TRUE);
         CGXULElement(this).extent_set(CGCanvas(layout));
//CGCanvas(layout).queue_draw(NULL);
      }
   }
}/*CGXULElement::notify_attribute_update*/

void CGXULElement::NATIVE_allocate(CGLayout *layout) {
   LOGFONT lf;
   static LOGFONT last_lf;
   static HFONT hFont, gui_font;

   if (CGObject(this)->native_object) {
#if 1
      if (strlen(framework.xul_font_family) &&
          (CObject(this).obj_class() == &class(CGTextBox) /*|| CObject(this).obj_class() == &class(CGMenuList)*/)) {
         CLEAR(&lf);
         strcpy(lf.lfFaceName, framework.xul_font_family);
         lf.lfHeight = -framework.xul_font_size;
         lf.lfWeight = framework.xul_font_weight ? FW_BOLD : 0;
         lf.lfItalic = framework.xul_font_style == EGFontStyle.italic;

         if (memcmp(&lf, &last_lf, sizeof(LOGFONT)) != 0) {
            if (gui_font) {
               DeleteObject(gui_font);
            }
            gui_font = CreateFontIndirect(&lf);
            memcpy(&last_lf, &lf, sizeof(LOGFONT));
            }
         hFont = gui_font;
      }
      else {
         hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
      }
      SendMessage((HWND)CGObject(this)->native_object, WM_SETFONT, (WPARAM)hFont, 0);
#else
      SendMessage((HWND)CGObject(this)->native_object, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
#endif
      SetWindowLong((HWND)CGObject(this)->native_object, GWL_USERDATA, (long)this);
   }
}/*CGXULElement::NATIVE_allocate*/

bool CGXULElement::repeat_event(CEvent *event) {
   /*>>>consider using general event handler only and source flag on the event */
   SendMessage((HWND)CGObject(this)->native_object, CEventKey_NATIVE_DATA(event)->message, 
               CEventKey_NATIVE_DATA(event)->wParam, CEventKey_NATIVE_DATA(event)->lParam);
   return TRUE;
}/*CGXULElement::event*/

long CGXULElement::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
//   printf("XUL Element message %d %d %d\n", uMsg, wParam, lParam);
   return 0;
}

void CGButton::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   const char *label;
   class:base.notify_attribute_update(attribute, changing);

   if (!changing) {
      label = CString(&this->label).length() ? CString(&this->label).string() : " ";
      SendMessage((HWND)CGObject(this)->native_object, WM_SETTEXT, 0, (LPARAM)label);
   }
}/*CGButton::notify_attribute_update*/

void CGButton::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   DWORD style = WS_CHILD | (CGXULElement(this)->disabled ? WS_DISABLED : 0) |
                 (this->dir == EGXULDirType.rtl ? BS_LEFTTEXT : 0);
   const char *label;

   CGXULElement(this).utov(CGCanvas(layout), position);

   if (CObject(this).obj_class() == &class(CGCheckBox)) {
      style |= BS_AUTOCHECKBOX;
   }
   else if (CObject(this).obj_class() == &class(CGRadioButton)) {
      style |= BS_RADIOBUTTON;
   }
   else if (CObject(this).obj_class() == &class(CGToggleButton)) {
      style |= BS_AUTOCHECKBOX | BS_PUSHLIKE;
   }
   else {
      style |= BS_DEFPUSHBUTTON;
   }

   label = CString(&this->label).string();
   if (CObject(this).obj_class() == &class(CGColourPicker)) {
      label = " ";
   }
   
   hwnd =
      CreateWindow("BUTTON", label, style,
                   (int)position[0].x, (int)position[0].y,
                   (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                   parent, NULL, GetModuleHandle(NULL), (LPVOID)this);
   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);

   if (CObject(this).obj_class() == &class(CGToggleButton)) {
      SendMessage(hwnd, BM_SETCHECK, CGToggleButton(this)->toggled ? BST_CHECKED : BST_UNCHECKED, 0);
   }
   else if (CObject(this).obj_class() == &class(CGCheckBox)) {
      SendMessage(hwnd, BM_SETCHECK, CGCheckBox(this)->checked ? BST_CHECKED : BST_UNCHECKED, 0);
   }
   else if (CObject(this).obj_class() == &class(CGRadioButton)) {
      SendMessage(hwnd, BM_SETCHECK, CGRadioButton(this)->selected ? BST_CHECKED : BST_UNCHECKED, 0);
   }

   CGObject(this)->native_object = (void *)hwnd;
   class:base.NATIVE_allocate(layout);
}/*CGButton::NATIVE_allocate*/

long CGButton::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   TObjectPtr object;
   CString assignment;
   CObject *child;
   CEventPointer event;
   CGLayout *layout = (CGLayout *)CObject(this).parent_find(&class(CGLayout));

   switch (uMsg) {
   case WM_COMMAND:
      if (CGXULElement(this)->binding.object.object) {
         if (CGXULElement(this)->binding.operation == EPathOperation.op_delete) {
            delete(CGXULElement(this)->binding.object.object);
         }
         else {
            new(&assignment).CString(NULL);
            object = CGXULElement(this)->binding.object;

            CXPath(&CGXULElement(this)->binding).assignment_get_string(&assignment);
            if (CString(&assignment).empty()) {
                CString(&assignment).set("TRUE");
            }

            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, FALSE);
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                         CGXULElement(this)->binding.object.attr.element,
                                                                                                         CGXULElement(this)->binding.object.attr.index,
                                                                                                         &assignment);
            /*>>>cheating, out of order */
            CObjPersistent(object.object).attribute_update(object.attr.attribute);
            CObjPersistent(object.object).attribute_update_end();
            delete(&assignment);
         }
      }
      else {
         child = CObject(this).child_first();
         while (child) {
            if (CObjClass_is_derived(&class(CGAnimateInput), CObject(child).obj_class())) {
               CGLayout(layout).key_gselection_set(CGObject(this));            
               new(&event).CEventPointer(EEventPointer.LeftDown, 0, 0, 0);
               CGAnimation(child).event(CEvent(&event));
               delete(&event);
               new(&event).CEventPointer(EEventPointer.LeftUp, 0, 0, 0);
               CGAnimation(child).event(CEvent(&event));
               delete(&event);
            }
            child = CObject(this).child_next(child);               
         }
      }
   }
//   class:base(this).WIN32_XULELEMENT_MSG(uMsg, wParam, lParam);

   return 0;
}/*CGButton::WIN32_XULELEMENT_MSG*/

void CGToggleButton::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);

   if (!changing) {
      SendMessage((HWND)CGObject(this)->native_object, BM_SETCHECK,
                                 this->toggled ? BST_CHECKED : BST_UNCHECKED, 0);
   }
}/*CGToggleButton::notify_attribute_update*/

void CGCheckBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);

   if (!changing) {
      SendMessage((HWND)CGObject(this)->native_object, BM_SETCHECK,
                                 this->checked ? BST_CHECKED : BST_UNCHECKED, 0);
   }
}/*CGCheckBox::notify_attribute_update*/

long CGToggleButton::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   bool toggled = SendMessage((HWND)CGObject(this)->native_object, BM_GETSTATE, 0, 0) & BST_CHECKED;

   switch (uMsg) {
   case WM_CTLCOLORSTATIC:   
      return (HBRUSH)CGXULElement(this).NATIVE_get_background();
   case WM_COMMAND:
      if (CGXULElement(this)->binding.object.object) {
         switch (CGXULElement(this)->binding.object.attr.index) {
         case ATTR_INDEX_DEFAULT:
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, toggled);
            break;
         case ATTR_INDEX_VALUE:
         default:
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, FALSE);
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_text_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                       CGXULElement(this)->binding.object.attr.element,
                                                                                                       CGXULElement(this)->binding.object.attr.index,
                                                                                                       toggled ? "TRUE" : "FALSE");
            break;
            }
         /*>>>cheating, out of order */
         CGXULElement(this).binding_resolve();
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
      }
      else {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGToggleButton,toggled>);
         this->toggled = toggled;
         CObjPersistent(this).attribute_update_end();
      }
      break;
   }

   return 0;
}/*CGToggleButton::WIN32_XULELEMENT_MSG*/

void *CGXULElement::NATIVE_get_background(void) {
   static HBRUSH hBrush;
   TGColour colour;

   if (hBrush) {
      DeleteObject(hBrush);
   }
   colour = GCOL_CONVERT_RGB(NULL, NULL, GCOL_TYPE_NAMED | EGColourNamed.lightsteelblue);
   hBrush = CreateSolidBrush(RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
   return (void *)hBrush;   
#if 0
   static HBRUSH hBrush;
   TGColour colour;
   HDC hdc, copydc;
   HBITMAP hBitmap;
   CGLayout *layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   TPoint position[2];
   RECT rect;

//   MoveWindow((HWND)CGObject(this)->native_object, 0,0,0,0, FALSE);
   rect.left   = (int)this->x;
   rect.top    = (int)this->y;
   rect.right  = (int)this->width;
   rect.bottom = (int)this->height;
   InvalidateRect((HWND)CGObject(layout)->native_object, &rect, TRUE);
   UpdateWindow((HWND)CGObject(layout)->native_object);
//   if (hBrush) {
//      DeleteObject(hBrush);
//   }      
//   CGXULElement(this).utov(CGCanvas(layout), position);
//   hdc = GetDC((HWND)CGObject(layout)->native_object);
//   copydc = CreateCompatibleDC(hdc);
//   hBitmap = CreateCompatibleBitmap(hdc,this->width,this->height);
//   SelectObject(copydc, hBitmap);
//   BitBlt(copydc,0,0,this->width,this->height,hdc,180,0,SRCCOPY);
//   hBrush = CreatePatternBrush(hBitmap);
   hBrush = GetStockObject(NULL_BRUSH);
//   MoveWindow((HWND)CGObject(this)->native_object, this->x,this->y,this->width,this->height, FALSE);
//   colour = GCOL_CONVERT_RGB(NULL, NULL, GCOL_TYPE_NAMED | EGColourNamed.lime);
//   hBrush = CreateSolidBrush(RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
   return (void *)hBrush;
#endif   
}/*CGXULElement::NATIVE_get_background*/

long CGCheckBox::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   bool checked = SendMessage((HWND)CGObject(this)->native_object, BM_GETSTATE, 0, 0) & BST_CHECKED;
   LPNMCUSTOMDRAW hdr;

   switch (uMsg) {
   case WM_CTLCOLORBTN:
      break;
   case WM_CTLCOLORSTATIC:
      return (HBRUSH)CGXULElement(this).NATIVE_get_background();
   case WM_COMMAND:
      if (CGXULElement(this)->binding.object.object) {
         switch (CGXULElement(this)->binding.object.attr.index) {
         case ATTR_INDEX_DEFAULT:
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, checked);
            break;
         case ATTR_INDEX_VALUE:
         default:
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, FALSE);
            if (CGXULElement(this)->binding.object.attr.attribute->type == PT_AttributeEnum ||
                CGXULElement(this)->binding.object.attr.attribute->data_type == &ATTRIBUTE:type<int>) {
               CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                          CGXULElement(this)->binding.object.attr.element,
                                                                                                          CGXULElement(this)->binding.object.attr.index,
                                                                                                          checked);
            }
            else {
               CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_text_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                          CGXULElement(this)->binding.object.attr.element,
                                                                                                          CGXULElement(this)->binding.object.attr.index,
                                                                                                          checked ? "TRUE" : "FALSE");
            }
            break;
         }
         /*>>>cheating, out of order */
         CGXULElement(this).binding_resolve();
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
      }
      else {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGCheckBox,checked>);
         this->checked = checked;
         CObjPersistent(this).attribute_update_end();
      }
      break;
   }

   return 0;
}/*CGCheckBox::WIN32_XULELEMENT_MSG*/

void CGRadioButton::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);

   if (!changing) {
      SendMessage((HWND)CGObject(this)->native_object, BM_SETCHECK,
                                 this->selected ? BST_CHECKED : BST_UNCHECKED, 0);
   }
}/*CGRadioButton::notify_attribute_update*/

long CGRadioButton::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   bool selected = SendMessage((HWND)CGObject(this)->native_object, BM_GETSTATE, 0, 0) & BST_CHECKED;

   if (CGXULElement(this)->updating)
      return 0;

   switch (uMsg) {
   case WM_CTLCOLORSTATIC:
      return (HBRUSH)CGXULElement(this).NATIVE_get_background();
   case WM_COMMAND:
      selected = !selected;
      if (CGXULElement(this)->binding.object.object) {
         switch (CGXULElement(this)->binding.object.attr.index) {
         case ATTR_INDEX_DEFAULT:
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, selected);
            break;
         case ATTR_INDEX_VALUE:
         default:
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_text_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                       CGXULElement(this)->binding.object.attr.element,
                                                                                                       CGXULElement(this)->binding.object.attr.index,
                                                                                                       selected ? "TRUE" : "FALSE");
            break;
            }
         /*>>>cheating, out of order */
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
//         this->selected = selected;
      }
      else {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRadioButton,selected>);
         this->selected = selected;
         CObjPersistent(this).attribute_update_end();
      }
      break;
   }

   return 0;
}/*CGRadioButton::WIN32_XULELEMENT_MSG*/

long CGColourPicker::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
    CGDialogChooseColour choose_colour;
    CGLayout *layout = NULL;
    CGWindow *window = NULL;

    switch (uMsg) {
    case WM_RBUTTONUP:
//    case WM_RBUTTONDOWN:
    case WM_COMMAND:
      if (CXPath(&CGXULElement(this)->binding).assignment()) {
         return class:base.WIN32_XULELEMENT_MSG(uMsg, wParam, lParam);
      }
      if (CGXULElement(this)->binding.object.object) {
         if (CObjClass_is_derived(&class(CGLayout), CObject(CGXULElement(this)->binding.object.object).obj_class())) {
            layout = CGLayout(CGXULElement(this)->binding.object.object);
         }
         else {
            layout = CGLayout(CObject(CGXULElement(this)->binding.object.object).parent_find(&class(CGLayout)));
         }
         if (!layout) {
            layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
         }
         window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
      }

      if (uMsg == WM_COMMAND && !this->native_picker) {
         new(&choose_colour).CGDialogChooseColour("Choose Colour",
                                                   &CGXULElement(this)->binding.object,
                                                   layout ? CGCanvas(layout)->palette : NULL,
                                                   layout ? CGCanvas(layout)->paletteB : NULL,
                                                   window, FALSE);
         CGWindowDialog(&choose_colour).button_add("OK", 0);
         CGWindowDialog(&choose_colour).execute_wait();
         CGWindowDialog(&choose_colour).wait(TRUE);
         CGWindowDialog(&choose_colour).close();
         delete(&choose_colour);
      }
      else {
         new(&choose_colour).CGDialogChooseColour("Choose Colour",
                                                   &CGXULElement(this)->binding.object,
                                                   layout ? CGCanvas(layout)->palette : NULL,
                                                   layout ? CGCanvas(layout)->paletteB : NULL,
                                                   window, TRUE);
         CGDialogChooseColour(&choose_colour).execute();
         delete(&choose_colour);
      }
      break;
   }

   return 0;
}/*CGColourPicker::WIN32_XULELEMENT_MSG*/

void CGColourPicker::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   if (!changing && CGObject(this)->native_object) {
      InvalidateRect((HWND)CGObject(this)->native_object, NULL, TRUE);
   }
}/*CGColourPicker::notify_attribute_update*/

long CGDatePickerButton::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
    CGDialogChooseDate choose_date;
    CGLayout *layout = NULL;
    CGWindow *window = NULL;
//    TPoint point; 
   RECT rect;

    switch (uMsg) {
    case WM_RBUTTONUP:
//    case WM_RBUTTONDOWN:
    case WM_COMMAND:
      if (CXPath(&CGXULElement(this)->binding).assignment()) {
         return class:base.WIN32_XULELEMENT_MSG(uMsg, wParam, lParam);
      }
      if (CGXULElement(this)->binding.object.object) {
         if (CObjClass_is_derived(&class(CGLayout), CObject(CGXULElement(this)->binding.object.object).obj_class())) {
            layout = CGLayout(CGXULElement(this)->binding.object.object);
         }
         else {
            layout = CGLayout(CObject(CGXULElement(this)->binding.object.object).parent_find(&class(CGLayout)));
         }
         if (!layout) {
            layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
         }
         window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
      }

      new(&choose_date).CGDialogChooseDate("Start Date", FALSE,
                                           &CGXULElement(this)->binding.object,
                                           window);
      CGWindowDialog(&choose_date).button_add("OK", 1);
      CGWindowDialog(&choose_date).button_add("Cancel", 0);      
      GetWindowRect((HWND)CGObject(this)->native_object, &rect);
//      point.x = CGXULElement(this)->x;
//      point.y = CGXULElement(this)->y + CGXULElement(this)->height;      
//      CGCanvas(layout).point_ctos(&point);
      CGWindow(&choose_date).position_set(rect.left, rect.bottom + 1);
      CGWindowDialog(&choose_date).execute_wait();
      CGWindowDialog(&choose_date).wait(TRUE);
      CGWindowDialog(&choose_date).close();
      delete(&choose_date);
      
      break;
   }

   return 0;
}/*CGDatePickerButton::WIN32_XULELEMENT_MSG*/

void CGSlider::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   SendMessage((HWND)CGObject(this)->native_object, TBM_SETPOS,
               (WPARAM)TRUE, (LPARAM)this->maximum - this->position);
}/*CGSlider::notify_attribute_update*/

void CGSlider::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   long style = CGXULElement(this)->disabled ? WS_DISABLED : 0;

   CGXULElement(this).utov(CGCanvas(layout), position);

   hwnd =
      CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD | TBS_NOTICKS | TBS_VERT | TBS_BOTH | TBS_FIXEDLENGTH | style,
                   (int)position[0].x, (int)position[0].y,
                   (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                   parent, NULL, GetModuleHandle(NULL), (LPVOID)this);
   SendMessage(hwnd, TBM_SETRANGE, (WPARAM)FALSE, (LPARAM)MAKELONG(this->minimum, this->maximum));
   SendMessage(hwnd, TBM_SETPOS, (WPARAM)TRUE, this->maximum - this->position);

    /*>>>not working, can't get back original WNDPROC for slider class*/
   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);

   CGObject(this)->native_object = (void *)hwnd;
   class:base.NATIVE_allocate(layout);
}/*CGSlider::NATIVE_allocate*/


long CGSlider::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   int position;

   switch (uMsg) {
   case WM_CTLCOLORSTATIC:
     break;
   case WM_HSCROLL:
   case WM_VSCROLL:
      position = SendMessage((HWND)CGObject(this)->native_object, TBM_GETPOS, 0, 0);
      if (CGXULElement(this)->binding.object.object) {
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                   CGXULElement(this)->binding.object.attr.element,
                                                                                                   CGXULElement(this)->binding.object.attr.index,
                                                                                                   CGSlider(this)->maximum - position);
         /*>>>cheating, out of order */
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
      }
      else {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGSlider,position>);
         CGSlider(this)->position = CGSlider(this)->maximum - position;
         CObjPersistent(this).attribute_update_end();
      }
   }

   return 0;
}/*CGSlider::WIN32_XULELEMENT_MSG*/

void CGTextLabel::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   if (!changing) {
      SendMessage((HWND)CGObject(this)->native_object, WM_SETTEXT, 0, (LPARAM)CString(&this->value).string());
   }
}/*CGTextLabel::notify_attribute_update*/

long CGTextLabel::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   TGColour colour;
   static HBRUSH hBrush;
   static HFONT hFont;
   LOGFONT lf;
   CGLayout *parent = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   /*>>>kludge, statically allocate placeholder for brush and font, no explicit free for these on exit */

   switch (uMsg) {
   case WM_CTLCOLORSTATIC:
      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGTextLabel,colour>) ||
          CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGTextLabel,background_colour>) ||
          CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGTextLabel,font_size>)) {
#if 1
//          if (parent && CObjClass_is_derived(&class(CGLayoutTab), CObject(parent).obj_class())) {
         if (parent && CGCanvas(parent)->colour_background == GCOL_NONE) {
            /*>>>minor kludge, transparnt if child of tab for XP visual styles */
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (long)GetStockObject(NULL_BRUSH);
         }
#endif
      break;
      }

      colour = GCOL_CONVERT_RGB(NULL, NULL, this->colour);
      SetTextColor((HDC)wParam, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
      colour = GCOL_CONVERT_RGB(NULL, NULL, this->background_colour);
      SetBkMode((HDC)wParam, TRANSPARENT);
      SetBkColor((HDC)wParam, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
#if 1
      if (hFont)
         DeleteObject(hFont);
      CLEAR(&lf);
      strcpy(lf.lfFaceName, "Arial");
      lf.lfHeight = -(this->font_size);
      lf.lfWeight = FW_BOLD;
      hFont = CreateFontIndirect(&lf);
      SendMessage((HWND)CGObject(this)->native_object, WM_SETFONT, (WPARAM)hFont, 0);

      if (hBrush) {
         DeleteObject(hBrush);
      }
      hBrush = CreateSolidBrush(RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
#else
      if (!hFont) {
         CLEAR(&lf);
         strcpy(lf.lfFaceName, "Arial");
         lf.lfHeight = -(this->font_size);
         lf.lfWeight = FW_BOLD;
         hFont = CreateFontIndirect(&lf);
         SendMessage((HWND)CGObject(this)->native_object, WM_SETFONT, (WPARAM)hFont, 0);
      }

      if (!hBrush) {
         hBrush = CreateSolidBrush(RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
      }
#endif
      return (long)hBrush;
   }

   return 0;//DefWindowProc((HWND)CGObject(this)->native_object, uMsg, wParam, lParam);
}/*CGTextLabel::WIN32_XULELEMENT_MSG*/

void CGTextLabel::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   long style = (CGXULElement(this)->disabled ? WS_DISABLED : 0);
   int ex_style = (this->dir == EGXULDirType.rtl ? WS_EX_RIGHT : 0);                

   if (CObject(this).obj_class() == &class(CGStatusBarPanel)) {
       style |= SS_SUNKEN;
   }

   CGXULElement(this).utov(CGCanvas(layout), position);

   hwnd =
      CreateWindowEx(ex_style, "STATIC", CString(&this->value).string(), WS_CHILD | style,
                     (int)position[0].x, (int)position[0].y,
                     (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                     parent, NULL, GetModuleHandle(NULL), (LPVOID)this);
   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);

   CGObject(this)->native_object = (void *)hwnd;
   class:base.NATIVE_allocate(layout);
}/*CGTextLabel::NATIVE_allocate*/

void CGDatePicker::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   struct tm *timeinfo;
   SYSTEMTIME systime;

   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   if (!changing) {
      timeinfo = gmtime((time_t *)&this->time);
      CLEAR(&systime);
      systime.wYear = (WORD)timeinfo->tm_year + 1900;
      systime.wMonth = (WORD)timeinfo->tm_mon + 1;
      systime.wDay = (WORD)timeinfo->tm_mday;
      MonthCal_SetCurSel((HWND)CGObject(this)->native_object, &systime);
   }
}/*CGDatePicker::notify_attribute_update*/

extern time_t mktime_gmt(struct tm *ptm);

long CGDatePicker::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   LPNMSELCHANGE hdr;
   struct tm mk_timeinfo;
   TUNIXTime utime;
   time_t result;
   time_t offset;

   switch (uMsg) {
   case WM_NOTIFY:
      hdr = (LPNMSELCHANGE)lParam;
      if (hdr->nmhdr.code == MCN_SELCHANGE) {
//         CLEAR(&mk_timeinfo);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get(CGXULElement(this)->binding.object.attr.attribute,
                                                                                 CGXULElement(this)->binding.object.attr.element,
                                                                                 CGXULElement(this)->binding.object.attr.index,
                                                                                 &ATTRIBUTE:type<TUNIXTime>, (void *)&utime);
         result = utime;

         offset = 0;
         memcpy(&mk_timeinfo, gmtime(&offset), sizeof(struct tm));
         mk_timeinfo.tm_mday += 1;
         offset = (int)(60 * 60 * 24) - (int)mktime(&mk_timeinfo);

         memcpy(&mk_timeinfo, gmtime(&result), sizeof(struct tm));
         mk_timeinfo.tm_year = hdr->stSelStart.wYear - 1900;
         mk_timeinfo.tm_mon  = hdr->stSelStart.wMonth - 1;
         mk_timeinfo.tm_mday = hdr->stSelStart.wDay;
         result = mktime(&mk_timeinfo) + offset;

         if (CGXULElement(this)->binding.object.object) {
            utime = (TUNIXTime)result;
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set(CGXULElement(this)->binding.object.attr.attribute,
                                                                                    CGXULElement(this)->binding.object.attr.element,
                                                                                    CGXULElement(this)->binding.object.attr.index,
                                                                                    &ATTRIBUTE:type<TUNIXTime>, (void *)&utime);
            /*>>>cheating, out of order */
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
         }
         else {
            CObjPersistent(this).attribute_update(ATTRIBUTE<CGDatePicker,time>);
            this->time = (TUNIXTime)time;
            CObjPersistent(this).attribute_update_end();
         }
      }
      break;
   }
   return 0;
}/*CGDatePicker::WIN32_XULELEMENT_MSG*/

void CGDatePicker::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   long style = CGXULElement(this)->disabled ? WS_DISABLED : 0;

   CGXULElement(this).utov(CGCanvas(layout), position);

   hwnd =
      CreateWindowEx(0, "SysMonthCal32", "pick", WS_CHILD | style,
                     (int)position[0].x, (int)position[0].y,
                     (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                     parent, NULL, GetModuleHandle(NULL), (LPVOID)this);
   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);

   CGObject(this)->native_object = (void *)hwnd;
   class:base.NATIVE_allocate(layout);
}/*CGDatePicker::NATIVE_allocate*/


//#define PBM_SETPOS (WM_USER + 2)
#ifndef PBM_SETMARQUEE
#define PBM_SETMARQUEE (WM_USER + 10)
#define PBS_MARQUEE 0x08
#endif

void CGProgressMeter::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   long style;
   HWND hwnd;
   
   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   hwnd = (HWND)CGObject(this)->native_object;
   if (CGObject(this)->native_object && !changing) {
      style = GetWindowLong(hwnd, GWL_STYLE);
      style &= ~PBS_MARQUEE;
      if (this->mode == EGProgressMode.undetermined) {      
         style |= PBS_MARQUEE;
      }
      SetWindowLong(hwnd, GWL_STYLE, style);   
      SendMessage(hwnd, PBM_SETMARQUEE, TRUE, 100);   
      SendMessage(hwnd, PBM_SETPOS, this->value, 0);   
   }
}/*CGProgressMeter::notify_attribute_update*/

void CGProgressMeter::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   long style = (CGXULElement(this)->disabled ? WS_DISABLED : 0);
   int ex_style = WS_EX_STATICEDGE;

   CGXULElement(this).utov(CGCanvas(layout), position);

   hwnd =
      CreateWindowEx(ex_style, PROGRESS_CLASS, NULL, WS_CHILD | style,
                     (int)position[0].x, (int)position[0].y,
                     (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                     parent, NULL, GetModuleHandle(NULL), (LPVOID)this);

   style = GetWindowLong(hwnd, GWL_STYLE);
   style &= ~PBS_MARQUEE;
   if (this->mode == EGProgressMode.undetermined) {      
      style |= PBS_MARQUEE;
   }
   SetWindowLong(hwnd, GWL_STYLE, style);   
   SendMessage(hwnd, PBM_SETMARQUEE, TRUE, 100);   
   SendMessage(hwnd, PBM_SETPOS, this->value, 0);   
//   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
//   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);

   CGObject(this)->native_object = (void *)hwnd;
   class:base.NATIVE_allocate(layout);
}/*CGProgressMeter::NATIVE_allocate*/

void CGTextBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   if (CGObject(this)->native_object && !changing) {
      CGXULElement(this)->updating = TRUE;
      CString(&this->value).encoding_set(EStringEncoding.UTF16);      
      SetWindowTextW((HWND)CGObject(this)->native_object, CString(&this->value).wstring());
      CGXULElement(this)->updating = FALSE;
//      SendMessage((HWND)CGObject(this)->native_object, WM_SETTEXT, 0, (LPARAM)CString(&this->value).string());
//      /* Setting text causes an edit change notification, cancel this */
//      this->modified = FALSE;
   }
}/*CGTextBox::notify_attribute_update*/

void CGTextBox::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   long style = (CGXULElement(this)->disabled ? WS_DISABLED : 0);
   int ex_style = WS_EX_STATICEDGE | (this->dir == EGXULDirType.rtl ? WS_EX_RIGHT : 0);

   switch (this->type) {
   case EGTextBoxType.password:
      style |= ES_PASSWORD;
      break;
   case EGTextBoxType.numeric:
//      style |= ES_NUMBER;
      break;
   }
   if (this->multiline) {
      style |= (ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | WS_BORDER);
   }
   else {
      style |= ES_AUTOHSCROLL | WS_BORDER;// | ES_MULTILINE;
   }

   CGXULElement(this).utov(CGCanvas(layout), position);

   hwnd =
      CreateWindowEx(ex_style, "EDIT", NULL, WS_CHILD | style,
                     (int)position[0].x, (int)position[0].y,
                     (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                     parent, NULL, GetModuleHandle(NULL), (LPVOID)this);
   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);
   SetWindowTextW(hwnd, CString(&this->value).wstring());
//   SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)CString(&this->value).string());

   /* clear modified flag set by setting initial text */
   this->modified = FALSE;

   if (this->maxlength != -1) {
      SendMessage(hwnd, EM_SETLIMITTEXT, this->maxlength, 0);
   }

   CGObject(this)->native_object = (void *)hwnd;
   class:base.NATIVE_allocate(layout);

#if 0
   if (this->multiline) {
      LOGFONT lf;
      CLEAR(&lf);
      strcpy(lf.lfFaceName, "Lucida Console");
      lf.lfHeight = -24;
      lf.lfWeight = FW_BOLD;

      SendMessage((HWND)CGObject(this)->native_object, WM_SETFONT, (WPARAM)CreateFontIndirect(&lf), 0);
   }
#endif
}/*CGTextBox::NATIVE_allocate*/

void CGTextBox::NATIVE_release(CGLayout *layout) {
   SendMessage((HWND)CGObject(layout)->native_object, WM_COMMAND, MAKEWPARAM(EN_KILLFOCUS, EN_KILLFOCUS), (LPARAM)CGObject(this)->native_object);

   class:base.NATIVE_release(layout);
}/*CGTextBox::NATIVE_release*/

long CGTextBox::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   char *buffer;
   unsigned short *wcp;
   TGColour colour;
   long result = 0;
   int length;
   static HFONT hFont;
   static HBRUSH hBrush;
   static LOGFONT lf;

   switch (uMsg) {
   case WM_CTLCOLOREDIT:
      colour = GCOL_CONVERT_RGB(NULL, NULL, this->colour);
      SetTextColor((HDC)wParam, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
      if (this->background_colour != GCOL_NONE) {
         colour = GCOL_CONVERT_RGB(NULL, NULL, this->background_colour);
         SetBkColor((HDC)wParam, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
         if (hBrush) {
            DeleteObject(hBrush);
         }
         hBrush = CreateSolidBrush(RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));

         return (long)hBrush;
      }
      return -1;
   case WM_COMMAND:
      switch (HIWORD(wParam)) {
      case EN_SETFOCUS:
         this->modified = FALSE;
         break;
      case EN_CHANGE:
         if (CGXULElement(this)->updating) {
            break;
         }
         if ((HWND)CGObject(this)->native_object == GetFocus()) {
            this->modified = TRUE;
#if 1
            buffer = CFramework(&framework).scratch_buffer_allocate();
            SendMessage((HWND)CGObject(this)->native_object, WM_GETTEXT, 10000, (LPARAM)buffer);
            length = strlen(buffer);
            CFramework(&framework).scratch_buffer_release(buffer);
            if (length == this->maxlength) {
               /* Push 'tab' to element to exit field */
               SendMessage((HWND)CGObject(this)->native_object, WM_CHAR, 0x09, 0);
            }
#endif
         }
         break;
      case EN_KILLFOCUS:
         if (this->modified) {
            buffer = CFramework(&framework).scratch_buffer_allocate();
            /*>>>should use scratch buffer size, if this can be known */
            SendMessage((HWND)CGObject(this)->native_object, WM_GETTEXT, 10000, (LPARAM)buffer);
            length = strlen(buffer);
            GetWindowTextW((HWND)CGObject(this)->native_object, (unsigned short *)buffer, 10000);
            if (CGXULElement(this)->binding.object.object) {
               wcp = (unsigned short *)buffer;

               while (*wcp && *wcp == ' ') {
                  wcp++;
               }

               if (*wcp || this->type == EGTextBoxType.nodefault || this->type == EGTextBoxType.password) {
                  CString(&this->value).encoding_set(EStringEncoding.UTF16);
                  ARRAY(&this->value.data).data_set((length + 1) * 2, buffer);

                  CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                  FALSE);
                  CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                               CGXULElement(this)->binding.object.attr.element,
                                                                                                               CGXULElement(this)->binding.object.attr.index,
                                                                                                               &this->value);
               }
               else {
                  /* Set attribute to default */
                  CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                  TRUE);
               }
               /*>>>cheating, out of order */
               CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
               CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();

               CString(&this->value).encoding_set(EStringEncoding.UTF16);
               ARRAY(&this->value.data).data_set((length + 1) * 2, buffer);
            }
            else {
               CObjPersistent(this).attribute_update(ATTRIBUTE<CGTextBox,value>);
               CString(&this->value).encoding_set(EStringEncoding.UTF16);
               ARRAY(&this->value.data).data_set((length + 1) * 2, buffer);
               CObjPersistent(this).attribute_update_end();
            }
            this->modified = FALSE;
            CFramework(&framework).scratch_buffer_release(buffer);
         }
         break;
      }
      break;
   default:
//      result = DefWindowProc((HWND)CGObject(this)->native_object, uMsg, wParam, lParam);
      break;
   }

   return result;
}/*CGTextBox::WIN32_XULELEMENT_MSG*/

void CGListBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   if (!changing) {
      SendMessage((HWND)CGObject(this)->native_object, LB_SETCURSEL, this->item, 0);
   }
}/*CGListBox::notify_attribute_update*/

void CGListBox::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   long style = CGXULElement(this)->disabled ? WS_DISABLED : 0;
   CGListItem *item;

   CGXULElement(this).utov(CGCanvas(layout), position);

   hwnd =
      CreateWindow("LISTBOX", NULL, WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | LBS_NOTIFY | style,
                   (int)position[0].x, (int)position[0].y, (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                   parent, NULL, GetModuleHandle(NULL), (LPVOID)this);
   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);

   CGObject(this)->native_object = (void *)hwnd;

   item = CGListItem(CObject(this).child_first());
   while (item) {
      SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)CString(&CGListItem(item)->label).string());
      item = CGListItem(CObject(this).child_next(CObject(item)));
   }

   SendMessage(hwnd, LB_SETCURSEL, this->item, 0);

   class:base.NATIVE_allocate(layout);
}/*CGListBox::NATIVE_allocate*/

long CGListBox::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   HWND hwnd = (HWND) CGObject(this)->native_object;
   int index;
   CGListItem *item;
   static bool open = FALSE;

   switch (uMsg) {
   case WM_COMMAND:
      switch (HIWORD(wParam)) {
      case LBN_SELCHANGE:
         index = SendMessage(hwnd, LB_GETCURSEL, 0, 0);
         if (CGXULElement(this)->binding.object.object) {
            item = CGListItem(CObject(this).child_n(index));
            if (item && CGListItem(item)->isdefault) {
               CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, TRUE);
            }
            else {
               CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, FALSE);
               if (CGXULElement(this)->binding.object.attr.attribute->type == PT_AttributeEnum ||
                   CGXULElement(this)->binding.object.attr.attribute->data_type == &ATTRIBUTE:type<int>) {
                  CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                     CGXULElement(this)->binding.object.attr.element,
                                                                                                     CGXULElement(this)->binding.object.attr.index,
                                                                                                     index);
               }
               else {
                  CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_text_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                      CGXULElement(this)->binding.object.attr.element,
                                                                                                      CGXULElement(this)->binding.object.attr.index,
                                                                                                      CString(&CGListItem(item)->label).string());
               }
            }
            /*>>>cheating, out of order */
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
         }
         else {
            CObjPersistent(this).attribute_update(ATTRIBUTE<CGListBox,item>);
            this->item = index;
            CObjPersistent(this).attribute_update_end();
         }
         break;
      }
      break;
   }
   return 0;
}/*CGListBox::WIN32_XULELEMENT_MSG*/

void CGScrollBar::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   SCROLLINFO si;

   class:base.notify_attribute_update(attribute, changing);
   if (!changing) {
      si.cbSize = sizeof(SCROLLINFO);
      si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
      si.nPos   = this->curpos;
      si.nMin   = 0;
      si.nMax  = this->range;
      si.nPage = this->range - this->maxpos + 1;
      SetScrollInfo((HWND)CGObject(this)->native_object, SB_CTL, &si, TRUE);
   }
}/*CGScrollBar::notify_attribute_update*/

long CGScrollBar::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   SCROLLINFO si;
   int scroll_pos;

   switch (uMsg) {
   case WM_VSCROLL:
   case WM_HSCROLL:
      /* Handle scroll bar messages */
      si.cbSize = sizeof(SCROLLINFO);
      si.fMask = SIF_ALL;
      GetScrollInfo((HWND)CGObject(this)->native_object, SB_CTL, &si);

      scroll_pos = si.nPos;

      switch (LOWORD(wParam)) {
      case SB_LEFT:      /*SB_TOP*/
         si.nPos = si.nMin;
         break;
      case SB_RIGHT:     /*SB_BOTTOM*/
         si.nPos = si.nMax;
         break;
      case SB_LINELEFT:  /*SB_LINEUP*/
         si.nPos -= 10;
         break;
      case SB_LINERIGHT: /*SB_LINEDOWN*/
         si.nPos += 10;
         break;
      case SB_PAGELEFT:  /*SB_PAGEUP*/
         si.nPos -= si.nPage;
         break;
      case SB_PAGERIGHT: /*SB_PAGEDOWN*/
         si.nPos += si.nPage;
         break;
      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
         si.nPos = si.nTrackPos;
         break;
      default:
         break;
      }

      /* Set the position and then retrieve it.  Due to adjusments by
         Windows it may not be the same as the value set */
      si.fMask = SIF_POS;
      SetScrollInfo((HWND)CGObject(this)->native_object, SB_CTL, &si, TRUE);
      GetScrollInfo((HWND)CGObject(this)->native_object, SB_CTL, &si);

      switch (LOWORD(wParam)) {
      case SB_THUMBTRACK:
      case SB_THUMBPOSITION:
          if (this->update_type == EGScrollBarUpdateType.nodrag)
              return 0;
          break;
      case SB_ENDSCROLL:
          if (this->update_type == EGScrollBarUpdateType.normal)
              return 0;
      default:
         break;
      }

      if (CGXULElement(this)->binding.object.object) {
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                     CGXULElement(this)->binding.object.attr.element,
                                                                                                     CGXULElement(this)->binding.object.attr.index,
                                                                                                     si.nPos);
         /*>>>cheating, out of order */
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
      }
      else {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
         CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGScrollBar,curpos>, si.nPos);
         CObjPersistent(this).attribute_update_end(void);
      }
   }
   return 0;
}

void CGScrollBar::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   long style = (CGXULElement(this)->disabled ? WS_DISABLED : 0) |
                 (this->orient == EGBoxOrientType.vertical ? SBS_VERT : SBS_HORZ);
   SCROLLINFO si;

   CGXULElement(this).utov(CGCanvas(layout), position);

   hwnd =
      CreateWindowEx(/*WS_EX_CLIENTEDGE*/0, "SCROLLBAR", NULL, WS_CHILD | style,
                     (int)position[0].x, (int)position[0].y,
                     (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                     parent, NULL, GetModuleHandle(NULL), (LPVOID)this);
   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);

   si.cbSize = sizeof(SCROLLINFO);
   si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
   si.nPos   = this->curpos;
   si.nMin   = 0;
   si.nMax  = this->range;
   si.nPage = this->range - this->maxpos + 1;
   SetScrollInfo(hwnd, SB_CTL, &si, TRUE);

   CGObject(this)->native_object = (void *)hwnd;

   class:base.NATIVE_allocate(layout);
}/*CGScrollBar::NATIVE_allocate*/

void CGScrolledArea::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGScrolledArea(this).notify_attribute_update(NULL, FALSE);
}/*CGScrolledArea::notify_object_linkage*/

void CGScrolledArea::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   TPoint position[2];
   CObject *child;
   CGLayout *child_layout, *layout;

   if (!changing) {
      layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
      child = CObject(this).child_first();
      child_layout = NULL;

      while (child) {
         if (CObjClass_is_derived(&class(CGLayout), CObject(child).obj_class())) {
            child_layout = CGLayout(child);
         }
         child = CObject(this).child_next(child);
      }

      if (layout && child_layout && CGObject(child_layout)->native_object) {
         CGXULElement(this).utov(CGCanvas(layout), position);
         MoveWindow((HWND)CGObject(child_layout)->native_object,
                    (int)position[0].x, (int)position[0].y,
                    (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                    TRUE);
         /*>>>this should be already send by the above MoveWindow() why not?*/
//         SendMessage((HWND)CGObject(child_layout)->native_object, WM_SIZE, 0, 0);
      }
   }

   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
}/*CGScrolledArea::notify_attribute_update*/

Local LRESULT CALLBACK CGMenuList_WIN32_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
   CGMenuList *this;
   CGCanvas *canvas;
   CGWindow *window;
   HWND parent;
   WNDPROC wndproc = (WNDPROC)GetClassLong(hwnd, GCL_WNDPROC);
   LRESULT result;
   bool filter, filter_framework;
   bool list_down;
   CGListItem *item;
   int i;

   this = (void *)GetWindowLong(hwnd, GWL_USERDATA);
   if (!this) {
      return CallWindowProc(wndproc, hwnd, message, wParam, lParam);
   }

   switch (message) {
   case WM_MOUSEMOVE:
   case WM_LBUTTONDOWN:
   case WM_LBUTTONUP:
   case WM_LBUTTONDBLCLK:
   case WM_RBUTTONDOWN:
   case WM_RBUTTONUP:
//   case 0x020A: //WM_MOUSEWHEEL:   
      /* don't repeat events inside ActiveX container */
      parent = GetParent(hwnd);
      if (parent) {
         if (Win32_ATL_wndproc_subclassed && ((WNDPROC)GetWindowLong(parent, GWL_WNDPROC) == Win32_ATL_wndproc_subclassed)) {
             break;
         }
      }
      /* repeat mouse events to parent canvas */
      canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
      SendMessage((HWND)CGObject(canvas)->native_object, message, wParam,
                  MAKELPARAM(LOWORD(lParam) + CGXULElement(this)->x, HIWORD(lParam) + CGXULElement(this)->y));
      break;
   case WM_CHAR:
   case WM_SYSCHAR:
   case WM_KEYDOWN:
   case WM_SYSKEYDOWN:
   case WM_KEYUP:
   case WM_SYSKEYUP:
      filter = FALSE;
      result = 0;
      filter_framework = FALSE;

      canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
      list_down = SendMessage(hwnd, CB_GETDROPPEDSTATE, 0, 0);
      if (message == WM_CHAR/* && wParam == 0x0D*/) {
         filter = TRUE;
//         if (wParam != 0x09 && wParam != 0x1B) {
         if (wParam == 0x0D) {
            filter_framework = TRUE;
         }
//         if (wParam == 0x1B && list_down) {
//            filter_framework = TRUE;
//         }
         if (isalpha(wParam) || isspace(wParam)) {
             item = CGListItem(CObject(&this->menupopup).child_first());
             i = 0;
             while (item) {
                 if (toupper(wParam) == toupper(item->accesskey)) {
                    SendMessage((HWND)CGObject(this)->native_object, CB_SETCURSEL, i, 0);
                    SendMessage((HWND)CGObject(canvas)->native_object, WM_COMMAND, (CBN_SELCHANGE << 16), (LPARAM)hwnd);
                    break;
                 }
                 item = CGListItem(CObject(&this->menupopup).child_next(CObject(item)));
                 i++;
             }
         }
      }
      else if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) {
         switch (wParam) {
         case VK_ESCAPE:
            if (list_down) {
               filter_framework = TRUE;
            }
            else {
               filter = TRUE;
            }
            break;
         case VK_RETURN:
            if (!list_down) {
               SendMessage(hwnd, CB_SHOWDROPDOWN, TRUE, 0);
               filter = TRUE;
               filter_framework = TRUE;
            }
            else {
                SendMessage(hwnd, CB_SHOWDROPDOWN, FALSE, 0);
                SendMessage((HWND)CGObject(canvas)->native_object, WM_COMMAND, (CBN_SELCHANGE << 16), (LPARAM)hwnd);
                filter = TRUE;
                filter_framework = TRUE;
            }
            break;
         case VK_UP:
         case VK_DOWN:
         case VK_LEFT:
         case VK_RIGHT:
            if (list_down) {
               if (wParam == VK_UP) {
                  wParam = VK_LEFT;
               }
               if (wParam == VK_DOWN) {
                  wParam = VK_RIGHT;
               }
               filter_framework = TRUE;
            }
            else {
               if (wParam == VK_UP || wParam == VK_DOWN ||
                   wParam == VK_LEFT || wParam == VK_RIGHT) {
                  filter = TRUE;
               }
               break;
            }
            break;
         }
      }

      if (!filter) {
         result = CallWindowProc(wndproc, hwnd, message, wParam, lParam);
      }

      if (!filter_framework) {
         /* pass keyboard input through to parent window */
         window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
         SendMessage((HWND)CGObject(window)->native_object, message, wParam, lParam);
      }
      return result;
   case WM_SETFOCUS:
      canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
      CGLayout(canvas).key_gselection_set(CGObject(this));
      break;
   }
   return CallWindowProc(wndproc, hwnd, message, wParam, lParam);
};

void CGMenuList::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   if (!changing) {
      SendMessage((HWND)CGObject(this)->native_object, CB_SETCURSEL, this->item, 0);
   }
}/*CGMenuList::notify_attribute_update*/

void CGMenuList::NATIVE_list_set(int enum_count, const char **enum_name) {
   int i;
   HWND hwnd = (HWND) CGObject(this)->native_object;

   for (i = 0; i < enum_count; i++) {
      SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)enum_name[i]);
   }
}/*CGMenuList::NATIVE_list_set*/

void CGMenuList::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   long style = CGXULElement(this)->disabled ? WS_DISABLED : 0;
   CGMenuItem *item;

   CGXULElement(this).utov(CGCanvas(layout), position);
   
   /* Only use user draw menu if coloured items exist */
   item = CGMenuItem(CObject(&this->menupopup).child_first());
   while (item) {
      if (CGListItem(item)->colour != GCOL_BLACK) {
         break;
      }
      item = CGMenuItem(CObject(&this->menupopup).child_next(CObject(item)));
   }
   if (item) {
      style |= CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;   
   }
   else {
      style |= CBS_DROPDOWNLIST | CBS_HASSTRINGS;
   }

   hwnd =
      CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | style,
                   (int)position[0].x, (int)position[0].y, (int)position[1].x - (int)position[0].x, MENULIST_HEIGHT,
                   parent, (HMENU)this, GetModuleHandle(NULL), (LPVOID)this);
   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGMenuList_WIN32_wnd_proc);

   CGObject(this)->native_object = (void *)hwnd;

   if ((CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) &&
       CGXULElement(this)->binding.object.attr.attribute->type == PT_AttributeEnum &&
       !CObject(&this->menupopup).child_count()) {
      CGMenuList(this).NATIVE_list_set(CGXULElement(this)->binding.object.attr.attribute->enum_count,
                                       CGXULElement(this)->binding.object.attr.attribute->enum_name);
   }
   else {
      item = CGMenuItem(CObject(&this->menupopup).child_first());
      while (item) {
         SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)CString(&CGListItem(item)->label).string());
         item = CGMenuItem(CObject(&this->menupopup).child_next(CObject(item)));
      }
   }
   SendMessage(hwnd, CB_SETCURSEL, this->item, 0);

   class:base.NATIVE_allocate(layout);
   
   /*>>>should be setting item hight based on current font */
   SendMessage(hwnd, CB_SETITEMHEIGHT, 0, 13);
   SendMessage(hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
//   SendMessage(hwnd, CB_SETITEMHEIGHT, 0, 13);
}/*CGMenuList::NATIVE_allocate*/

long CGMenuList::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   HWND hwnd = (HWND) CGObject(this)->native_object;
   int index;
   CGMenuItem *item;
   static bool open = FALSE;
   static HBRUSH hBrush;
   TGColour colour, bgcolour;
   LPDRAWITEMSTRUCT draw_item;
   RECT rc;
   HBRUSH hbrush, holdbrush;
   char item_text[256];

   switch (uMsg) {
   case WM_MEASUREITEM:
      ((LPMEASUREITEMSTRUCT) lParam)->itemHeight = (UINT)CGXULElement(this)->height;
      break;
   case WM_DRAWITEM:
      draw_item = (LPDRAWITEMSTRUCT) lParam;
      rc = draw_item->rcItem;
     
      SetBkMode(draw_item->hDC,TRANSPARENT);

      if (draw_item->itemID == -1) return 0; 
      item = CGMenuItem(CObject(&this->menupopup).child_n(draw_item->itemID));
      if (item) {
         colour = GCOL_CONVERT_RGB(NULL, NULL, CGListItem(item)->colour);
         strcpy(item_text, CString(&CGListItem(item)->value).string());
      }
      else {
         colour = GCOL_BLACK;
         strcpy(item_text, (char *)draw_item->itemData ? (char *)draw_item->itemData : "empty");
      }
      bgcolour = GCOL_CONVERT_RGB(NULL, NULL, this->background_colour);      
        
//      if ((draw_item->itemAction & ODA_DRAWENTIRE) && (draw_item->itemState & ODS_COMBOBOXEDIT)) {
//         InflateRect(&rc, -1, -1);
//      }
//      else {
         rc.right++;
         rc.bottom++;
//      }
      hbrush = CreateSolidBrush((draw_item->itemState & ODS_SELECTED) ? 
                                GetSysColor(COLOR_HIGHLIGHT) : 
                                RGB(GCOL_RGB_RED(bgcolour), GCOL_RGB_GREEN(bgcolour), GCOL_RGB_BLUE(bgcolour)));
      holdbrush = SelectObject(draw_item->hDC, hbrush);
      SelectObject(draw_item->hDC, GetStockObject(NULL_PEN));
      Rectangle(draw_item->hDC, rc.left, rc.top, rc.right, rc.bottom);
      SelectObject(draw_item->hDC, holdbrush);
      DeleteObject(hbrush);
      SetTextColor(draw_item->hDC, (draw_item->itemState & ODS_SELECTED) ? GetSysColor(COLOR_HIGHLIGHTTEXT) : 
                   RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
      TextOut(draw_item->hDC, draw_item->rcItem.left + 2, draw_item->rcItem.top, 
              item_text, strlen(item_text));
      break;
   case WM_CTLCOLOREDIT:
   case WM_CTLCOLORLISTBOX:
      colour = GCOL_CONVERT_RGB(NULL, NULL, this->colour);
      SetTextColor((HDC)wParam, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
      if (this->background_colour != GCOL_NONE) {
         colour = GCOL_CONVERT_RGB(NULL, NULL, this->background_colour);
         SetBkColor((HDC)wParam, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
         if (hBrush)
            DeleteObject(hBrush);
         hBrush = CreateSolidBrush(RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
         return (long)hBrush;
      }
      break;
   case WM_COMMAND:
      switch (HIWORD(wParam)) {
      case CBN_SELCHANGE:
          if (SendMessage(hwnd, CB_GETDROPPEDSTATE, 0, 0)) {
             break;
          }
         index = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
         if (CGXULElement(this)->binding.object.object) {
            item = CGMenuItem(CObject(&this->menupopup).child_n(index));
            if (item && CGListItem(item)->isdefault) {
               CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, TRUE);
            }
            else {
               CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, FALSE);
               if (CGXULElement(this)->binding.object.attr.attribute->data_type == &ATTRIBUTE:type<int> || !item ||
                   !CObjPersistent(item).attribute_default_get(ATTRIBUTE<CGListItem,value_int>)) {
                  CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                            CGXULElement(this)->binding.object.attr.element,
                                                                                                            CGXULElement(this)->binding.object.attr.index,
                                                                                                            item ? CGListItem(item)->value_int : index);
               }
               else {
                  CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_text_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                             CGXULElement(this)->binding.object.attr.element,
                                                                                                             CGXULElement(this)->binding.object.attr.index,
                                                                                                             CString(&CGListItem(item)->value).string());
               }
            }
            /*>>>cheating, out of order */
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
         }
         else {
            CObjPersistent(this).attribute_update(ATTRIBUTE<CGMenuList,item>);
            this->item = index;
            CObjPersistent(this).attribute_update_end();
         }
         break;
      }
      break;
   }

   return 0;
}/*CGMenuList::WIN32_XULELEMENT_MSG*/

void *CObjClientGTree::NATIVE_item_object_find(CObjPersistent *object, void *root) {
   HTREEITEM root_item = (HTREEITEM)root, child_item;
   HTREEITEM result;
   TVITEM tv_item;

   if (!root_item) {
      root_item = TreeView_GetRoot(this->gtree->hwnd_tree);
   }

   child_item = TreeView_GetChild(this->gtree->hwnd_tree, root_item);
   while (child_item) {
      result = CObjClientGTree(this).NATIVE_item_object_find(object, (void *)child_item);
      if (result) {
         return result;
      }
      child_item = TreeView_GetNextSibling(this->gtree->hwnd_tree, child_item);
   }

   CLEAR(&tv_item);
   tv_item.mask  = TVIF_HANDLE | TVIF_PARAM;
   tv_item.hItem = root_item;

   TreeView_GetItem(this->gtree->hwnd_tree, &tv_item);
   if ((void *)tv_item.lParam == (void *)object) {
      return (void *)root_item;
   }

   return NULL;
}/*CObjClientGTree::NATIVE_item_object_find*/

void CObjClientGTree::notify_selection_update(CObjPersistent *object, bool changing) {
   int count, i;
   CObjPersistent *selected_object;
   HTREEITEM hSelected = NULL;
   TVITEM tv_item;
   CSelection *selection;

//   if (changing)
//      return;

   if (this->gtree->hwnd_tree) {
      selection = CObjServer(CObjClient(this)->server).selection_get();
      count = ARRAY(CSelection(selection).selection()).count();
      for (i = 0; i < count; i++) {
         selected_object = CObjPersistent(ARRAY(CSelection(selection).selection()).data()[i].object);
         hSelected = (HTREEITEM) CObjClientGTree(this).NATIVE_item_object_find(selected_object, NULL);

         CLEAR(&tv_item);
         tv_item.mask      = TVIF_STATE | TVIF_HANDLE;
         tv_item.hItem     = hSelected;
         tv_item.stateMask = TVIS_BOLD;
         tv_item.state     = changing ? 0 : TVIS_BOLD;
         TreeView_SetItem((HWND)this->gtree->hwnd_tree, &tv_item);
         if (!changing && i == 0) {
            CGXULElement(this->gtree)->updating = TRUE;
            TreeView_SelectItem((HWND)this->gtree->hwnd_tree, hSelected);
            CGXULElement(this->gtree)->updating = FALSE;
         }
      }
   }
}/*CObjClientTree::notify_selection_update*/

void CObjClientGTree::notify_all_update(void) {
   CObjPersistent *object_root;
   HTREEITEM hRoot;
   int count;
   CSelection *selection;

   count = ARRAY(&CObjClient(this)->object_root).count();
   if (!count) {
      return;
   }

   object_root = ARRAY(&CObjClient(this)->object_root).data()[0];
   hRoot = (HTREEITEM) CObjClientGTree(this).NATIVE_item_object_find(object_root, NULL);

   if (!hRoot) {
      ASSERT("CObjClientGTree::notify_all_update - object not found");
   }

   if (this->gtree->hwnd_tree) {
      TreeView_DeleteItem(this->gtree->hwnd_tree, hRoot);
   }

   CGTree(this->gtree).NATIVE_XULElement_fill(NULL, object_root, -1);

   selection = CObjServer(CObjClient(this)->server).selection_get();
   if (selection) {
      CObjClientGTree(this).notify_selection_update(object_root, FALSE);
   }
}/*CObjClientGTree::notify_all_update*/

void CObjClientGTree::notify_object_child_add(CObjPersistent *object, CObjPersistent *child) {
   HTREEITEM hRoot = (HTREEITEM) CObjClientGTree(this).NATIVE_item_object_find(object, NULL);
   int index = -1;

   if (CObject(child) == CObject(object).child_last()) {
      index = -1;
   }
//   else if (CObject(child) == CObject(object).child_first()) {
//      index = 0;
//   }
   else {
//      WARN("CObjClientGTree::notify_object_child_add - unsupported position add");
       CObjClientGTree(this).notify_all_update();
       return;
   }

   if (!hRoot) {
//      WARN("CObjClientGTree::notify_object_child_add - parent not found");
      return;
   }

   if (this->gtree->hwnd_tree) {
      CGTree(this->gtree).NATIVE_XULElement_fill(hRoot, child, index);
   }
}/*CObjClientGTree::notify_object_child_add*/

void CObjClientGTree::notify_object_child_remove(CObjPersistent *object, CObjPersistent *child) {
   HTREEITEM hRoot = (HTREEITEM) CObjClientGTree(this).NATIVE_item_object_find(child, NULL);

   if (!hRoot) {
//      WARN("CObjClientGTree::notify_object_child_remove - object not found");
      return;
   }

   if (this->gtree->hwnd_tree) {
      TreeView_DeleteItem(this->gtree->hwnd_tree, hRoot);
   }
}/*CObjClientGTree::notify_object_child_remove*/

void CObjClientGTree::notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing) {
   /*>>>update only the affected areas of the element, updating every time might be overkill*/
   if (!changing) {
      InvalidateRect((HWND)this->gtree->hwnd_tree, NULL, TRUE);
   }
}/*CObjClientGTree::notify_object_attribute_update*/

void CGTree::NATIVE_XULElement_fill(void *parent, CObjPersistent *object, int index) {
   HTREEITEM hParent = (HTREEITEM) parent;
   TVINSERTSTRUCT tv_insert;
   TVITEM tv_item;
   HTREEITEM hItem, hItem_attribute;
   CObjPersistent *child;
   ARRAY<const TAttribute *> attribute_list;
   const TAttribute *attribute;
   int i;

   CLEAR(&tv_insert);
   tv_insert.hParent      = hParent;
   /*>>>not quite right, should insert after static children and attribute nodes */
   tv_insert.hInsertAfter = index == -1 ? TVI_LAST : TVI_FIRST;
   tv_insert.item.mask    = TVIF_TEXT | TVIF_PARAM;
//   tv_insert.item.pszText = (char *)CObjClass_alias_short(CObject(object).obj_class());
   tv_insert.item.pszText = (char *)CObjClass_displayname(CObject(object).obj_class());
   tv_insert.item.lParam  = (LPARAM)object;
   hItem = TreeView_InsertItem((HWND)this->hwnd_tree, &tv_insert);

   ARRAY(&attribute_list).new();
   CObjPersistent(object).attribute_list(&attribute_list, TRUE, TRUE, FALSE);

   if (!this->attribute_hide) {
      /* attributes */
      CLEAR(&tv_insert);
      tv_insert.hParent      = hItem;
      tv_insert.item.mask    = TVIF_TEXT;
      tv_insert.item.pszText = "@";
      hItem_attribute = TreeView_InsertItem((HWND)this->hwnd_tree, &tv_insert);

      for (i = 0; i < ARRAY(&attribute_list).count(); i++) {
         attribute = ARRAY(&attribute_list).data()[i];
         if (attribute->type == PT_ElementObject) {
            continue;
         }
         CLEAR(&tv_item);
         tv_item.mask   = TVIF_HANDLE;
         tv_item.lParam = (LPARAM)attribute;
         CLEAR(&tv_insert);
         tv_insert.hParent      = hItem_attribute;
         tv_insert.item.mask    = TVIF_TEXT | TVIF_PARAM;
         tv_insert.item.pszText = LPSTR_TEXTCALLBACK;
         tv_insert.item.lParam  = (LPARAM)attribute;
         tv_item.hItem = TreeView_InsertItem((HWND)this->hwnd_tree, &tv_insert);

         TreeView_SetItem((HWND)this->hwnd_tree, &tv_item);
      }
   }

   for (i = 0; i < ARRAY(&attribute_list).count(); i++) {
      attribute = ARRAY(&attribute_list).data()[i];
      if (attribute->type == PT_ElementObject) {
         CGTree(this).NATIVE_XULElement_fill(hItem, CObjPersistent(&((byte *)object)[attribute->offset]), -1);
      }
   }

   ARRAY(&attribute_list).delete();

   child = CObjPersistent(CObject(object).child_first());
   while (child) {
      CGTree(this).NATIVE_XULElement_fill(hItem, child, -1);
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }
}/*CGTree::NATIVE_XULElement_fill*/

Local LRESULT CALLBACK CGTree_WIN32_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
   CGTree *this;
   void *udata = (void *)GetWindowLong(hwnd, GWL_USERDATA);
   int width = LOWORD(lParam), height = HIWORD(lParam);

   if (message == WM_CREATE) {
      this = CGTree(((LPCREATESTRUCT) lParam)->lpCreateParams);
   }
   else {
      this = udata ? udata : NULL;
   }

   switch (message) {
   case WM_CREATE:
      SetWindowLong(hwnd, GWL_USERDATA, (long)this);
      break;
   case WM_NOTIFY:
      if (this) {
         return CGXULElement(this).WIN32_XULELEMENT_MSG(message, wParam, lParam);
      }
      return 0;
   case WM_SIZE:
      MoveWindow((HWND)this->hwnd_header,
                 0, 0, width, 20,
                 TRUE);
      MoveWindow((HWND)this->hwnd_tree,
                 0, 19, width, height - 19,
                 TRUE);
      break;
   }

   return DefWindowProc(hwnd, message, wParam, lParam);
}/*CGTree_WIN32_wnd_proc*/

void CGTree::NATIVE_allocate(CGLayout *layout) {
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   CObjPersistent *object_root =
      ARRAY(&CObjClient(&this->client)->object_root).data()[0];
   HDITEM header_item;
   static TCHAR szClassName[] = TEXT("GRTreeContainer");
   WNDCLASS cls;
   DWORD style, style_ex;
   CGTreeCol *tree_col;
   int position;

   cls.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
   cls.lpfnWndProc   = CGTree_WIN32_wnd_proc;
   cls.cbClsExtra    = 0;
   cls.cbWndExtra    = sizeof(long);
   cls.hInstance     = GetModuleHandle(NULL);
   cls.hIcon         = NULL;
   cls.hbrBackground = NULL;
   cls.lpszMenuName  = NULL;
   cls.lpszClassName = szClassName;
   cls.hCursor       = LoadCursor(NULL, IDC_ARROW);

   style = WS_CHILD;
   style_ex = 0;

   RegisterClass(&cls);

   hwnd = CreateWindow(szClassName, "Tree Container", WS_CHILD,
                       (int)CGXULElement(this)->x, (int)CGXULElement(this)->y, (int)CGXULElement(this)->width, (int)CGXULElement(this)->height,
                       parent, NULL, GetModuleHandle(NULL), (LPVOID)this);
   /*>>>tree view is a special case, might want to rethink*/
   SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);
   SetWindowLong(hwnd, GWL_USERDATA, (long)this);

   this->hwnd_header =
      CreateWindow(WC_HEADER, "Header", WS_VISIBLE | WS_CHILD | WS_BORDER,
                   (int)CGXULElement(this)->x, (int)CGXULElement(this)->y, (int)CGXULElement(this)->width, 20,
                   hwnd, NULL, GetModuleHandle(NULL), (LPVOID)this);

   CLEAR(&header_item);
   header_item.mask       = HDI_TEXT | HDI_WIDTH;
   header_item.fmt        = HDF_STRING | HDF_CENTER;

   tree_col = CGTreeCol(CObject(&this->tree_cols).child_last());
   position = (CObject(&this->tree_cols).child_count() - 1) * 100 + 200;
   while (tree_col) {
      position -= 100;
      tree_col->position     = position;
      header_item.cxy        = position >= 200 ? 100 : 200;
      header_item.pszText    = CString(&tree_col->label).string();
      header_item.cchTextMax = 5;
      Header_InsertItem(this->hwnd_header, 0, &header_item);

      tree_col = CGTreeCol(CObject(&this->tree_cols).child_previous(CObject(tree_col)));
   }

   this->hwnd_tree =
      CreateWindow(WC_TREEVIEW, "Tree View", WS_VISIBLE | WS_CHILD | WS_BORDER |
                   TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
                   (int)CGXULElement(this)->x, (int)CGXULElement(this)->y + 19,
                   (int)CGXULElement(this)->width, (int)CGXULElement(this)->height - 20,
                   hwnd, NULL, GetModuleHandle(NULL), (LPVOID)this);
   SetWindowLong(this->hwnd_tree, GWL_USERDATA, (long)this);
   SetWindowLong(this->hwnd_tree, GWL_WNDPROC, (LONG)CGXULElement_WIN32_wnd_proc);

   CGObject(this)->native_object = (void *)hwnd;
   class:base.NATIVE_allocate(layout);

   CGTree(this).NATIVE_XULElement_fill(NULL, object_root, -1);
}/*CGTree::NATIVE_allocate*/

long CGTree::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   union {
      unsigned long lParam;
      NMHDR *hdr;
      NMTREEVIEW *nmtv;
      NMTVDISPINFO *nmtvdi;
      NMTVCUSTOMDRAW *nmtvcd;
   } param;
   CString value;
   TVITEM tv_item, tv_item_parent;
   HDITEM hd_item;
   bool is_default;
   COLORREF text_colour_old;
   int i, position;
   CGTreeCol *tree_col;
   const TAttribute *attribute;
   CSelection *selection;

   param.lParam = lParam;

   if (uMsg == WM_NOTIFY) {
      switch (param.hdr->code) {
      case HDN_ITEMCHANGED:
         CLEAR(&hd_item);
         hd_item.mask = HDI_WIDTH;
         i = position = 0;

         tree_col = CGTreeCol(CObject(&this->tree_cols).child_first());
         while (tree_col) {
            tree_col->position = position;

            Header_GetItem(this->hwnd_header, i, &hd_item);
            position += hd_item.cxy;

            i++;
            tree_col = CGTreeCol(CObject(&this->tree_cols).child_next(CObject(tree_col)));
         }

         InvalidateRect((HWND)this->hwnd_tree, NULL, TRUE);
         break;
      case NM_CUSTOMDRAW:
         if (param.hdr->hwndFrom != this->hwnd_tree)
            return 0;

         switch (param.nmtvcd->nmcd.dwDrawStage) {
         case CDDS_ITEMPREPAINT:
            CLEAR(&tv_item);
            tv_item.mask  = TVIF_HANDLE | TVIF_PARAM;
            tv_item.hItem = (HTREEITEM) param.nmtvcd->nmcd.dwItemSpec;
            TreeView_GetItem(this->hwnd_tree, &tv_item);

            CLEAR(&tv_item_parent);
            tv_item_parent.mask  = TVIF_HANDLE | TVIF_PARAM;
            tv_item_parent.hItem = TreeView_GetParent((HWND)this->hwnd_tree, param.nmtvcd->nmcd.dwItemSpec);
            TreeView_GetItem(this->hwnd_tree, &tv_item_parent);

            if (!tv_item_parent.hItem || (tv_item.lParam && tv_item_parent.lParam)) {
               /* Object Item */
               text_colour_old = GetTextColor(param.nmtvcd->nmcd.hdc);

               new(&value).CString(NULL);
               tree_col = CGTreeCol(CObject(&this->tree_cols).child_n(2));
               while (tree_col) {
                  attribute = CObjPersistent((void *)tv_item.lParam).attribute_find(CString(&tree_col->label).string());
                  if (attribute) {
                     is_default =
                        CObjPersistent((void *)tv_item.lParam).attribute_default_get(attribute);
                     SetTextColor(param.nmtvcd->nmcd.hdc, is_default ? ATTR_DEFAULT_COLOUR : text_colour_old);
                     CString(&value).encoding_set(EStringEncoding.UTF16);
                     CObjPersistent((void *)tv_item.lParam).attribute_get_string(attribute, &value);
#if 1
                     if (value.encoding) {
                        TextOutW(param.nmtvcd->nmcd.hdc, param.nmtvcd->nmcd.rc.left + tree_col->position,
                                 param.nmtvcd->nmcd.rc.top,
                                 CString(&value).wstring(), CString(&value).length());
                     }
                     else {
                        TextOut(param.nmtvcd->nmcd.hdc, param.nmtvcd->nmcd.rc.left + tree_col->position,
                                param.nmtvcd->nmcd.rc.top,
                                CString(&value).string(), CString(&value).length());
                     }
#endif
                  }
                  tree_col = CGTreeCol(CObject(&this->tree_cols).child_next(CObject(tree_col)));
               }

               SetTextColor(param.nmtvcd->nmcd.hdc, text_colour_old);

               delete(&value);
               return 0;
            }

            CLEAR(&tv_item);
            tv_item.mask  = TVIF_HANDLE | TVIF_PARAM;
            tv_item.hItem = TreeView_GetParent((HWND)this->hwnd_tree, param.nmtvcd->nmcd.dwItemSpec);
            tv_item.hItem = TreeView_GetParent((HWND)this->hwnd_tree, tv_item.hItem);
            TreeView_GetItem(this->hwnd_tree, &tv_item);

            if (!tv_item.lParam || !param.nmtvcd->nmcd.lItemlParam)
               return 0;

            if ( ((TAttribute *)param.nmtvcd->nmcd.lItemlParam)->type != PT_AttributeEnum &&
                (((TAttribute *)param.nmtvcd->nmcd.lItemlParam)->data_type == (void *)0 ||
                 ((TAttribute *)param.nmtvcd->nmcd.lItemlParam)->data_type == (void *)-1))
               return 0;

            tree_col = CGTreeCol(CObject(&this->tree_cols).child_n(1));

            new(&value).CString(NULL);
            CString(&value).encoding_set(EStringEncoding.UTF16);
            CObjPersistent((void *)tv_item.lParam).attribute_get_string((TAttribute *)param.nmtvcd->nmcd.lItemlParam,
                                                                        &value);
            text_colour_old = GetTextColor(param.nmtvcd->nmcd.hdc);

            is_default =
               CObjPersistent((void *)tv_item.lParam).attribute_default_get((TAttribute *)param.nmtvcd->nmcd.lItemlParam);
            if (is_default) {
               SetTextColor(param.nmtvcd->nmcd.hdc, ATTR_DEFAULT_COLOUR);
            }
#if 1
            if (value.encoding) {
               TextOutW(param.nmtvcd->nmcd.hdc, param.nmtvcd->nmcd.rc.left + tree_col->position,
                       param.nmtvcd->nmcd.rc.top,
                       CString(&value).wstring(), CString(&value).length());
            }
            else {
               TextOut(param.nmtvcd->nmcd.hdc, param.nmtvcd->nmcd.rc.left + tree_col->position,
                       param.nmtvcd->nmcd.rc.top,
                       CString(&value).string(), CString(&value).length());
            }
#endif
            SetTextColor(param.nmtvcd->nmcd.hdc, text_colour_old);
            delete(&value);
            break;
         case CDDS_PREPAINT:
            return CDRF_NOTIFYITEMDRAW;
         }
         return 0;
      case TVN_GETDISPINFO:
         CLEAR(&tv_item);
         tv_item.mask  = TVIF_HANDLE | TVIF_PARAM;
         tv_item.hItem = TreeView_GetParent((HWND)this->hwnd_tree, param.nmtvdi->item.hItem);
         tv_item.hItem = TreeView_GetParent((HWND)this->hwnd_tree, tv_item.hItem);
         TreeView_GetItem(this->hwnd_tree, &tv_item);

         if (!tv_item.lParam)
            return 0;

         new(&value).CString(NULL);
         CObjPersistent((void *)tv_item.lParam).attribute_get_string((TAttribute *)param.nmtvdi->item.lParam,
                                                                     &value);
         switch (((TAttribute *)param.nmtvdi->item.lParam)->type) {
         case PT_Attribute:
         case PT_AttributeEnum:
         case PT_AttributeArray:
            sprintf(param.nmtvdi->item.pszText, "@");
            break;
         default:
            break;
         }

         sprintf(param.nmtvdi->item.pszText + strlen(param.nmtvdi->item.pszText),
                 "%s", ((TAttribute *)param.nmtvdi->item.lParam)->name);
         delete(&value);
         break;
      case TVN_DELETEITEM:
         /*>>>kludge, prevent unwanted selection update after object deletion */
         this->selection_resolving = TRUE;
         break;
      case TVN_SELCHANGED:
         if (CGXULElement(this)->updating) {
            break;
         }
         if (this->selection_resolving == TRUE) {
            this->selection_resolving = FALSE;
         }
         else if (param.nmtv->itemNew.lParam &&
                   (((CObject *)param.nmtv->itemNew.lParam)->obj_sid == SID_OBJ)) {
              /*>>>use selection update method in CObjServer*/
             selection = CObjServer(CObjClient(&this->client)->server).selection_get();
             if (selection) {
               CObjServer(CObjClient(&this->client)->server).root_selection_update();
               CSelection(selection).clear();
               CSelection(selection).add(CObjPersistent((void *)param.nmtv->itemNew.lParam), NULL);
               CObjServer(CObjClient(&this->client)->server).root_selection_update_end();
            }
         }
         break;
      }
   }

   return 0;
}/*CGTree::WIN32_XULELEMENT_MSG*/

long CGTabBox::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   union {
      unsigned long lParam;
      NMHDR *hdr;
   } param;
   int index;
   TCITEM tie;
   CObject *child;

   param.lParam = lParam;

   if (uMsg == WM_NOTIFY) {
      switch (param.hdr->code) {
      case TCN_SELCHANGING:
         CLEAR(&tie);
         tie.mask = TCIF_PARAM;
         index = TabCtrl_GetCurSel((HWND)CGObject(this)->native_object);
         TabCtrl_GetItem((HWND)CGObject(this)->native_object, index, &tie);

         ShowWindow((HWND)tie.lParam, SW_HIDE);
         break;
      case TCN_SELCHANGE:
         CLEAR(&tie);
         tie.mask = TCIF_PARAM;
         index = TabCtrl_GetCurSel((HWND)CGObject(this)->native_object);

         CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGTabBox,selectedIndex>, index);
         TabCtrl_GetItem((HWND)CGObject(this)->native_object, index, &tie);
         ShowWindow((HWND)tie.lParam, SW_SHOWNA);
#if 1
         child = (CObject *)GetWindowLong((void *)tie.lParam, GWL_USERDATA);
         CGLayout(CGObject(child)).key_gselection_set(CGObject(child));
#endif
         break;
      }
   }

   return 0;
}/*CGTabBox::WIN32_XULELEMENT_MSG*/

LRESULT CALLBACK CGTabBox__WIN32_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
   CGTabBox *this = (CGTabBox *)GetWindowLong(hwnd, GWL_USERDATA);
   CObjPersistent *tab_child;
   CGWindow *window;
   CGCanvas *canvas;

   switch (message) {
   case WM_SETFOCUS:
      canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
      CGLayout(canvas).key_gselection_set(CGObject(this));
      break;
   case WM_CHAR:
   case WM_SYSCHAR:
   case WM_KEYDOWN:
   case WM_SYSKEYDOWN:
   case WM_KEYUP:
   case WM_SYSKEYUP:
      /* pass keyboard input through to parent canvas */
      window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
      SendMessage((HWND)CGObject(window)->native_object, message, wParam, lParam);

      /* Filter select keypresses from Win32 default handling*/
      if (GetKeyState(VK_MENU) & 0x8000) {
         return 0;
      }
      if (message == WM_CHAR) {
         switch (wParam) {
         case 0x09:                     /* Tab */
         case 0x0D:                     /* CR */
         case 0x1B:                     /* ESC */
            return 0;
         }
      }
      break;
   case WM_SIZE:
      tab_child = CObjPersistent(CObject(this).child_first());
      while (tab_child) {
         MoveWindow((HWND)CGObject(tab_child)->native_object, 1, 21,
                    LOWORD(lParam) - 2, HIWORD(lParam) - 22, TRUE);
         tab_child = CObjPersistent(CObject(this).child_next(CObject(tab_child)));
      }
      break;
   }

   if (this) {
      return CallWindowProc(this->wndproc, hwnd, message, wParam, lParam);
   }
   else {
      return 0;
   }
}/*CGTabBox__WIN32_WndProc*/

void CGTabBox::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   DWORD style = WS_CHILD | (CGXULElement(this)->disabled ? WS_DISABLED : 0);
   TCITEM tie;
   CObjPersistent *tab_child;
   int index;

   CGXULElement(this).utov(CGCanvas(layout), position);

   hwnd =
      CreateWindow(WC_TABCONTROL, "tab view", style,
                   (int)position[0].x, (int)position[0].y,
                   (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                   parent, NULL, GetModuleHandle(NULL), (LPVOID)this);
   CGObject(this)->native_object = (void *)hwnd;
   class:base.NATIVE_allocate(layout);

   SetWindowLong(hwnd, GWL_USERDATA, (long)this);
   this->wndproc = (void *)SetWindowLong(hwnd, GWL_WNDPROC, (LONG)CGTabBox__WIN32_WndProc);

   index = CObject(this).child_count();
   tab_child = CObjPersistent(CObject(this).child_last());
   while (tab_child) {
      index--;
//      CGCanvas(tab_child)->colour_background = GCOL_NONE;
      CGObject(tab_child).show(TRUE);
      MoveWindow((HWND)CGObject(tab_child)->native_object, 1, 21,
                  (int)position[1].x - (int)position[0].x - 2, (int)position[1].y - (int)position[0].y - 22, TRUE);

      tie.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
      tie.iImage = -1;
      tie.pszText = CString(&CGLayoutTab(tab_child)->label).string();
      tie.lParam = (LONG)CGObject(tab_child)->native_object;
      TabCtrl_InsertItem(hwnd, 0, &tie);

      if (index != this->selectedIndex) {
         ShowWindow((HWND)CGObject(tab_child)->native_object, SW_HIDE);
      }
      else {
#if 0
         CGLayout(CGObject(tab_child)).key_gselection_set(CGObject(tab_child));
#endif
      }
      tab_child = CObjPersistent(CObject(this).child_previous(CObject(tab_child)));
   }

   TabCtrl_SetCurFocus(hwnd, this->selectedIndex);

//   class:base.NATIVE_allocate(layout);
}/*CGTabBox::NATIVE_allocate*/

void CGTabBox::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CObject *child;
   int i;

   class:base.notify_object_linkage(linkage, object);

   switch (linkage) {
   case EObjectLinkage.ChildRemovePre:
      i = 0;
      child = CObject(this).child_first();
      while (child) {
         if (child == object)
            break;
         i++;
         child = CObject(this).child_next(child);
      }
      if (child != NULL) {
         TabCtrl_DeleteItem((HWND)CGObject(this)->native_object, i);
         this->selectedIndex = 0;
         TabCtrl_SetCurFocus((HWND)CGObject(this)->native_object, this->selectedIndex);
      }
      break;
   }
}/*CGTabBox::notify_object_linkage*/

long CGSplitter::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) {
   return 0;
}/*CGSplitter::WIN32_XULELEMENT_MSG*/

Local void DrawXorBar(HDC hdc, int x1, int y1, int width, int height) {
   static WORD _dotPatternBmp[8] = {
      0x00aa, 0x0055, 0x00aa, 0x0055,
      0x00aa, 0x0055, 0x00aa, 0x0055
   };

   HBITMAP hbm;
   HBRUSH  hbr, hbrushOld;

   hbm = CreateBitmap(8, 8, 1, 1, _dotPatternBmp);
   hbr = CreatePatternBrush(hbm);

   SetBrushOrgEx(hdc, x1, y1, 0);
   hbrushOld = (HBRUSH)SelectObject(hdc, hbr);

   PatBlt(hdc, x1, y1, width, height, PATINVERT);

   SelectObject(hdc, hbrushOld);

   DeleteObject(hbr);
   DeleteObject(hbm);
}/*DrawXorBar*/

LRESULT CGSplitter_WIN32_mouse_move(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)   {
   HDC hdc;
   RECT rect;
   POINT pt;
   CGSplitter *this = (CGSplitter *)GetWindowLong(hwnd, GWL_USERDATA);

   if (this->drag_mode == FALSE)
      return 0;

   pt.x = (short)LOWORD(lParam);  // horizontal position of cursor
   pt.y = (short)HIWORD(lParam);

   GetWindowRect(hwnd, &rect);

   ClientToScreen(hwnd, &pt);
   pt.x -= rect.left;
   pt.y -= rect.top;

   OffsetRect(&rect, -rect.left, -rect.top);

   if(pt.x < 0) pt.x = 0;
   if(pt.x > rect.right - 4) {
      pt.x = rect.right - 4;
   }
   if(pt.y < 0) pt.y = 0;
   if(pt.y > rect.bottom - 4) {
      pt.y = rect.bottom - 4;
   }

   switch (this->type) {
   case EGSplitterType.Vertical:
      if (pt.x != this->old_position && wParam & MK_LBUTTON) {
         hdc = GetWindowDC(hwnd);
         DrawXorBar(hdc, this->old_position - 2, 1, 4, rect.bottom - 2);
         DrawXorBar(hdc, pt.x - 2, 1, 4, rect.bottom - 2);

         ReleaseDC(hwnd, hdc);

         this->old_position = pt.x;
      }
      break;
   case EGSplitterType.Horizontal:
      if (pt.y != this->old_position && wParam & MK_LBUTTON) {
         hdc = GetWindowDC(hwnd);
         DrawXorBar(hdc, 1, this->old_position - 2, rect.right - 2, 4);
         DrawXorBar(hdc, 1, pt.y - 2, rect.right - 2, 4);

         ReleaseDC(hwnd, hdc);

         this->old_position = pt.y;
      }
      break;
   }
   return 0;
}/*CGSplitter_WIN32_mouse_move*/

LRESULT CGSplitter_WIN32_l_button_down(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
   POINT pt;
   HDC hdc;
   RECT rect;
   CGSplitter *this = (CGSplitter *)GetWindowLong(hwnd, GWL_USERDATA);

   pt.x = (short)LOWORD(lParam);  // horizontal position of cursor
   pt.y = (short)HIWORD(lParam);

   GetWindowRect(hwnd, &rect);

   //convert the mouse coordinates relative to the top-left of
   //the window
   ClientToScreen(hwnd, &pt);
   pt.x -= rect.left;
   pt.y -= rect.top;

   //same for the window coordinates - make them relative to 0,0
   OffsetRect(&rect, -rect.left, -rect.top);

   if(pt.x < 0) pt.x = 0;
   if(pt.x > rect.right - 4) {
      pt.x = rect.right - 4;
   }
   if(pt.y < 0) pt.y = 0;
   if(pt.y > rect.bottom - 4) {
      pt.y = rect.bottom - 4;
   }

   /* Don't enter drag mode if selection not on bar */
   switch (this->type) {
   case EGSplitterType.Vertical:
      if (abs(pt.x - this->position) > 2)
         return 0;
      break;
   case EGSplitterType.Horizontal:
      if (abs(pt.y - this->position) > 2)
         return 0;
      break;
   }

   this->drag_mode = TRUE;

   SetCapture(hwnd);

   hdc = GetWindowDC(hwnd);

   switch (this->type) {
   case EGSplitterType.Vertical:
      this->old_position = pt.x;

      DrawXorBar(hdc, pt.x - 2, 1, 4, rect.bottom - 2);
      break;
   case EGSplitterType.Horizontal:
      this->old_position = pt.y;

      DrawXorBar(hdc, 1, pt.y - 2, rect.right - 2, 4);
      break;
   }

   ReleaseDC(hwnd, hdc);

   return 0;
}/*CGSplitter_WIN32_l_button_down*/

LRESULT CGSplitter_WIN32_l_button_up(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
   HDC hdc;
   RECT rect;
   POINT pt;
   CGSplitter *this = (CGSplitter *)GetWindowLong(hwnd, GWL_USERDATA);

   pt.x = (short)LOWORD(lParam);        /* horizontal position of cursor */
   pt.y = (short)HIWORD(lParam);

   if (this->drag_mode == FALSE)
      return 0;

   GetWindowRect(hwnd, &rect);

   ClientToScreen(hwnd, &pt);
   pt.x -= rect.left;
   pt.y -= rect.top;

   OffsetRect(&rect, -rect.left, -rect.top);

   if (pt.x < 0) pt.x = 0;
   if (pt.x > rect.right - 4) {
      pt.x = rect.right - 4;
   }
   if (pt.y < 0) pt.y = 0;
   if (pt.y > rect.bottom - 4) {
      pt.y = rect.bottom - 4;
   }

   hdc = GetWindowDC(hwnd);

   switch (this->type) {
   case EGSplitterType.Vertical:
      this->old_position = pt.x;

      DrawXorBar(hdc, this->old_position - 2, 1, 4, rect.bottom - 2);
      break;
   case EGSplitterType.Horizontal:
      this->old_position = pt.y;

      DrawXorBar(hdc, 1, this->old_position - 2, rect.right - 2, 4);
      break;
   }

   ReleaseDC(hwnd, hdc);

   this->drag_mode = FALSE;

   /* convert the splitter position back to screen coords. */
   GetWindowRect(hwnd, &rect);
   pt.x += rect.left;
   pt.y += rect.top;

   /* now convert into CLIENT coordinates */
   ScreenToClient(hwnd, &pt);
   GetClientRect(hwnd, &rect);
   switch (this->type) {
   case EGSplitterType.Vertical:
      this->position = pt.x;
      break;
   case EGSplitterType.Horizontal:
      this->position = pt.y;
      break;
   }

   ReleaseCapture();

//   CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
//   CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);

   CObjPersistent(this).attribute_update(ATTRIBUTE<CGSplitter,position>);
   CObjPersistent(this).attribute_update_end();

   return 0;
}/*CGSplitter_WIN32_l_button_up*/

Local LRESULT CALLBACK CGSplitter_WIN32_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
   switch (message) {
   case WM_LBUTTONDOWN:
      return CGSplitter_WIN32_l_button_down(hwnd, message, wParam, lParam);
   case WM_LBUTTONUP:
      return CGSplitter_WIN32_l_button_up(hwnd, message, wParam, lParam);
   case WM_MOUSEMOVE:
      return CGSplitter_WIN32_mouse_move(hwnd, message, wParam, lParam);
   default:
      break;
   }

   return DefWindowProc(hwnd, message, wParam, lParam);
}/*CGSplitter_WIN32_wnd_proc*/

void CGSplitter::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObjPersistent *splitter_child;
   int i = 0, position = 0, size = 0;

   class:base.notify_attribute_update(attribute, changing);

   if (CGObject(this)->native_object) {
      splitter_child = CObjPersistent(CObject(this).child_last());
      while (splitter_child) {
         switch (this->type) {
         case EGSplitterType.Vertical:
            switch (i) {
            case 1:
               position = 3;
               size = this->position - 2;
               break;
            case 0:
               position = this->position + 3;
               size = (int)(CGXULElement(this)->width - this->position - 5);
               break;
            }
            MoveWindow((HWND)CGObject(splitter_child)->native_object, position, 3,
                       size, (int)CGXULElement(this)->height - 6, TRUE);
            break;
         case EGSplitterType.Horizontal:
            switch (i) {
            case 1:
               position = 3;
               size = this->position - 2;
               break;
            case 0:
               position = this->position + 3;
               size = (int)(CGXULElement(this)->height - this->position - 5);
               break;
            }
            MoveWindow((HWND)CGObject(splitter_child)->native_object, 3, position,
                       (int)CGXULElement(this)->width - 6, size, TRUE);
            break;
         }
         splitter_child = CObjPersistent(CObject(this).child_previous(CObject(splitter_child)));
         i++;
      }
   }
}/*CGSplitter::notify_attribute_update*/

void CGSplitter::NATIVE_allocate(CGLayout *layout) {
   TPoint position[2];
   HWND parent = (HWND)CGObject(layout)->native_object;
   HWND hwnd;
   DWORD style = WS_CHILD | (CGXULElement(this)->disabled ? WS_DISABLED : 0);
   CObjPersistent *splitter_child;
   WNDCLASSEX wc;
   int i = 0;

   CGXULElement(this).utov(CGCanvas(layout), position);

   wc.cbSize            = sizeof(wc);
   wc.style             = 0;
   wc.lpfnWndProc       = CGSplitter_WIN32_wnd_proc;
   wc.cbClsExtra        = 0;
   wc.cbWndExtra        = 0;
   wc.hInstance         = GetModuleHandle(NULL);
   wc.hIcon             = NULL;
   wc.hCursor           = LoadCursor (NULL, this->type == EGSplitterType.Horizontal ? IDC_SIZENS : IDC_SIZEWE);
   wc.hbrBackground     = (HBRUSH)(COLOR_3DFACE+1);
   wc.lpszMenuName      = 0;
   wc.lpszClassName     = this->type == EGSplitterType.Horizontal ? "SplitterH" : "SplitterV";
   wc.hIconSm           = LoadIcon (NULL, IDI_APPLICATION);

   RegisterClassEx(&wc);

   hwnd =
      CreateWindowEx(0, wc.lpszClassName, "", style,
                     (int)position[0].x, (int)position[0].y,
                     (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y,
                     parent, NULL, GetModuleHandle(NULL), (LPVOID)this);

   CGObject(this)->native_object = (void *)hwnd;

   splitter_child = CObjPersistent(CObject(this).child_first());
   while (splitter_child) {
      CGObject(splitter_child).show(TRUE);
//      this->child[i] = CGLayout(splitter_child);

      splitter_child = CObjPersistent(CObject(this).child_next(CObject(splitter_child)));
      i++;
   }

   class:base.NATIVE_allocate(layout);

   CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
   CObjPersistent(this).attribute_update_end();
}/*CGSplitter::NATIVE_allocate*/

void CGSplitter::NATIVE_release(CGLayout *layout) {
   _virtual_CGXULElement_NATIVE_release(CGXULElement(this), layout);
}/*CGSplitter::NATIVE_release*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
