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
#include "../gobject.c"
#include "../gcanvas.c"
#include "../gxulelement.c"
#include "../gwindow.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <windows.h>
#include <commctrl.h>
#if KEYSTONE_WIN32_GDIP
#include "gdiplusc.h"
#endif
#include "native.h"

#if KEYSTONE_WIN32_GDIP
extern ULONG_PTR gdiplusToken;

static inline void win32_gdiplus_init(void) {
   GdiplusStartupInput gdiplusStartupInput;
   if (!gdiplusToken) {
      memset(&gdiplusStartupInput, 0, sizeof(gdiplusStartupInput));
      gdiplusStartupInput.GdiplusVersion = 1;
      GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
   }
}
#endif

void CGBitmap::NATIVE_allocate(void) {
   int depth;
   HDC memDC = CreateCompatibleDC(NULL);
   static BITMAPINFOHEADER *bmih;

   depth = GetDeviceCaps(memDC, BITSPIXEL);
   DeleteDC(memDC);

   if (!this->native_object) {
      this->native_object = (HBITMAP)
      CreateBitmap(this->width, this->height, 1, depth, NULL);
   }
}/*CGBitmap::NATIVE_allocate*/

void CGBitmap::NATIVE_release(void) {
#if KEYSTONE_WIN32_GDIP
   if (this->image) {
      GdipDisposeImage((GpBitmap *)this->native_object);
      if (this->native_data) {
         free(this->native_data);
      }
      return;
   }
#endif
   if (this->native_object) {
      DeleteObject((HGDIOBJ)this->native_object);
      this->native_object = NULL;
   }
}/*CGBitmap::NATIVE_release*/

void CGBitmap::load_file(CGCanvas *canvas, const char *filename) {
   unsigned int i;
   HBITMAP hBitmap;
   HDC hdc, hdcB;
   BITMAP bm_info;
   WCHAR *wfilename;

#if KEYSTONE_WIN32_GDIP
   GpBitmap *bitmap;
   UINT width, height;
   GpRect rect;
   BitmapData bmdata;

   if (canvas->render_mode_canvas == EGCanvasRender.full) {
      this->image = TRUE;
      wfilename = CFramework(&framework).scratch_buffer_allocate();
//      mbsrtowcs(wfilename, &filename, 10000, NULL);
      /*>>>hack, manually convert string to wide char */
      memset(wfilename, 0, (strlen(filename) + 1) * 2);
      for (i = 0; i < strlen(filename); i++) {
         wfilename[i] = filename[i];
      }

      win32_gdiplus_init();
      GdipCreateBitmapFromFile(wfilename, (GpBitmap **)&bitmap);

      CFramework(&framework).scratch_buffer_release(wfilename);

      if (!bitmap) {
         return;
      }

      GdipGetImageWidth((GpImage *)bitmap, &width);
      GdipGetImageHeight((GpImage *)bitmap, &height);
      this->width = width;
      this->height = height;

      /*>>>kludge, copy image back out into memory to free file handle */
      rect.X = 0;
      rect.Y = 0;
      rect.Width = width;
      rect.Height = height;
      GdipBitmapLockBits(bitmap, (GpRect *)&rect, 1, PixelFormat32bppARGB, &bmdata);
      this->native_data = realloc(this->native_data, bmdata.Stride * bmdata.Height);
      memcpy(this->native_data, bmdata.Scan0, bmdata.Stride * bmdata.Height);
      GdipBitmapUnlockBits(bitmap, &bmdata);
      GdipCreateBitmapFromScan0(bmdata.Width, bmdata.Height, bmdata.Stride, bmdata.PixelFormat, this->native_data/*bmdata.Scan0*/, (GpBitmap **)&this->native_object);
      GdipBitmapUnlockBits(bitmap, &bmdata);

      GdipDisposeImage(bitmap);
//      this->native_object = bitmap;
      return;
   }
#endif

   /* load BMP image, klunky for now*/
   hBitmap = (HBITMAP)LoadImage(0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
   GetObject(hBitmap, sizeof(BITMAP), &bm_info);
   this->width = bm_info.bmWidth;
   this->height = bm_info.bmHeight;

   CGBitmap(this).NATIVE_allocate();

   hdc = CreateCompatibleDC(NULL);
   hdcB = CreateCompatibleDC(NULL);
   SetBkMode(hdc, TRANSPARENT);
   SelectObject(hdc, (HBITMAP)this->native_object);
   SetBkMode(hdcB, TRANSPARENT);
   SelectObject(hdcB, (HBITMAP)hBitmap);

   BitBlt(hdc, 0, 0, this->width, this->height,
          hdcB, 0, 0, SRCCOPY);
   DeleteDC(hdc);
   DeleteDC(hdcB);
}/*CGBitmap::load_file*/

void CGBitmap::datalines_write(int line_start, int lines, void *data) {
   /*>>>for now, keep dc reference in bitmap*/
   BITMAPINFO info;
   HDC hdc;

   hdc = CreateCompatibleDC(NULL);

   info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   info.bmiHeader.biWidth = this->width;
   info.bmiHeader.biHeight = this->height;
   info.bmiHeader.biPlanes = 1;
   info.bmiHeader.biBitCount = (WORD)GetDeviceCaps(hdc, BITSPIXEL);
   info.bmiHeader.biCompression = BI_RGB;

   SetBkMode(hdc, TRANSPARENT);
   SelectObject(hdc, (HBITMAP)this->native_object);
   SetDIBitsToDevice(hdc, 0, line_start, this->width, lines, 0, 0, 0, lines, data, &info, DIB_RGB_COLORS);
   DeleteDC(hdc);
}/*CGBitmap::datalines_write*/

#define SCROLL_JUMP 20
Local LRESULT CALLBACK CGCanvas_WIN32_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
   CGCanvas *this;
   CGWindow *window;
   CGXULElement *xul_element;
   void *udata = (void *)GetWindowLong(hwnd, GWL_USERDATA);
   RECT w_ud_rect, window_rect;
   TRect ud_rect;
   int scroll_type, scroll_pos;
   int modifier;
   bool full_update;
   EEventPointer event_type;
   CEventPointer event;
   COLORREF bg_colour;
   SCROLLINFO si;
   TRACKMOUSEEVENT trackmouseevent;
   DWORD result;

   if (message == WM_CREATE) {
      this = CGCanvas(((LPCREATESTRUCT) lParam)->lpCreateParams);
   }
   else {
      this = udata ? udata : NULL;
   }

//   CLEAR(&ui_message);
//printf("canvas message %d (%d %d)\n", message, wParam, lParam);
   switch (message) {
   case WM_CREATE:
      SetWindowLong(hwnd, GWL_USERDATA, (long)this);
      CLEAR(&this->native_data);
      GCanvas_NATIVE_DATA(this)->hwnd = hwnd;
      break;
   case WM_NCPAINT:
      break;
#if 0
   case WM_ERASEBKGND:
      /*process erasebkgnd message, experimental, can produce more correct results*/
//      if (!this->native_erase_background)
//         return 0;
      GetClientRect(hwnd, &w_ud_rect);
      ud_rect.point[0].x = w_ud_rect.left;
      ud_rect.point[0].y = w_ud_rect.top;
      ud_rect.point[1].x = w_ud_rect.right;
      ud_rect.point[1].y = w_ud_rect.bottom;

      GCanvas_NATIVE_DATA(this)->hdc = (HDC)wParam;
      CGCanvas(this).draw_rectangle(this->native_erase_background_colour,
                                    TRUE, w_ud_rect.left,w_ud_rect.top,w_ud_rect.right,w_ud_rect.bottom);
      DeleteObject(GCanvas_NATIVE_DATA(this)->hPen);
      DeleteObject(GCanvas_NATIVE_DATA(this)->hBrush);
      DeleteObject(GCanvas_NATIVE_DATA(this)->hFont);
      GCanvas_NATIVE_DATA(this)->hdc = 0;
      return 0;
#endif
   case WM_PRINTCLIENT:
      break;
   case WM_ERASEBKGND:
//      GetUpdateRect(hwnd, &w_ud_rect, TRUE);
//      printf("canvas bg erase %d %d %d %d\n", w_ud_rect.left, w_ud_rect.top, w_ud_rect.right, w_ud_rect.bottom);
//      break;
//      printf("canvas erase background\n");
      break;
//      return 1;
  case WM_PAINT:
      if (this->native_redraw_show) {
         this->colour_background = GCOL_RGB(rand() % 256, rand() % 256, rand() % 256);
      }
      if (this->colour_background == GCOL_DIALOG) {
         bg_colour = GetSysColor(COLOR_BTNFACE);
         this->native_erase_background_colour = GCOL_RGB(GetRValue(bg_colour), GetGValue(bg_colour), GetBValue(bg_colour));
//this->native_erase_background_colour = GCOL_NONE;
      }
      else {
         this->native_erase_background_colour = this->colour_background;
      }

      GetUpdateRect(hwnd, &w_ud_rect, TRUE);
      ud_rect.point[0].x = w_ud_rect.left;
      ud_rect.point[0].y = w_ud_rect.top;
      ud_rect.point[1].x = w_ud_rect.right;
      ud_rect.point[1].y = w_ud_rect.bottom;
      full_update = (w_ud_rect.left == 0 && w_ud_rect.top == 0 &&
                     w_ud_rect.right == this->allocated_width && w_ud_rect.bottom == this->allocated_height);

      CGCanvas(this).NATIVE_enter(TRUE);

      /* set clipping */
      SelectClipRgn(GCanvas_NATIVE_DATA(this)->hdc, NULL);
      IntersectClipRect(GCanvas_NATIVE_DATA(this)->hdc,
                        w_ud_rect.left, w_ud_rect.top, w_ud_rect.right, w_ud_rect.bottom);

      CGCanvas(this).redraw(full_update ? NULL : &ud_rect, EGObjectDrawMode.Draw);
      CGCanvas(this).NATIVE_enter(FALSE);
      break;
   case WM_CTLCOLORBTN:
   case WM_CTLCOLORLISTBOX:
   case WM_CTLCOLOREDIT:
   case WM_CTLCOLORSTATIC:
   case WM_CTLCOLORSCROLLBAR:
      udata = (void *)GetWindowLong((void *)lParam, GWL_USERDATA);
      if (udata) {
         xul_element = CGXULElement(udata);
         if (CObject(xul_element).obj_class() != &class(CGTree) /* &&
            CObject(xul_element).obj_class() != &class(CGSlider)*/) {
            goto foward;
         }
      }
      break;
   case WM_MEASUREITEM:
      goto foward;
   case WM_DRAWITEM:
      goto foward;
      break;
   case WM_COMMAND:
      if (lParam == 0) {
         break;
      }
      /*fallthrough*/
   case WM_VSCROLL:
   case WM_HSCROLL:
   foward:
      /*>>>problem in template expanstion when parameter contains ',' character*/
      if (message == WM_MEASUREITEM) {
         udata = (void *)((LPMEASUREITEMSTRUCT) lParam)->CtlID;
      }
      else if (message == WM_DRAWITEM) {
         udata = (void *)GetWindowLong(((LPDRAWITEMSTRUCT) lParam)->hwndItem, GWL_USERDATA);
      }
      else {
         udata = (void *)GetWindowLong((void *)lParam, GWL_USERDATA);
      }
      if (udata) {
         xul_element = CGXULElement(udata);
         result = CGXULElement(xul_element).WIN32_XULELEMENT_MSG(message, wParam, lParam);
         if (result) {
            return result;
         }
         break;
      }
      else {
         scroll_type = message == WM_HSCROLL ? SB_HORZ : SB_VERT;

         /* Handle horizontal scroll bar messages */
         si.cbSize = sizeof(SCROLLINFO);
         si.fMask = SIF_ALL;
         GetScrollInfo(hwnd, scroll_type, &si);

         scroll_pos = si.nPos;

         switch (LOWORD(wParam)) {
         case SB_LEFT:      /*SB_TOP*/
            si.nPos = si.nMin;
            break;
         case SB_RIGHT:     /*SB_BOTTOM*/
            si.nPos = si.nMax;
            break;
         case SB_LINELEFT:  /*SB_LINEUP*/
            si.nPos -= SCROLL_JUMP;
            break;
         case SB_LINERIGHT: /*SB_LINEDOWN*/
            si.nPos += SCROLL_JUMP;
            break;
         case SB_PAGELEFT:  /*SB_PAGEUP*/
            si.nPos -= si.nPage;
            break;
         case SB_PAGERIGHT: /*SB_PAGEDOWN*/
            si.nPos += si.nPage;
            break;
         case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
         default:
            break;
         }
         /* Set the position and then retrieve it.  Due to adjusments by
            Windows it may not be the same as the value set */
         si.fMask = SIF_POS;
         SetScrollInfo(hwnd, scroll_type, &si, TRUE);
         GetScrollInfo(hwnd, scroll_type, &si);
         /* If the position has changed, scroll the window and update it */
         if (si.nPos != scroll_pos) {
            InvalidateRect(hwnd, NULL, TRUE);
//>>>            ScrollWindow(hwnd, iHorizPos - si.nPos, 0, NULL, NULL);

            scroll_pos = si.nPos;
            GetScrollInfo(hwnd, scroll_type, &si);

            /* Update canvas view origin */
            switch (message) {
            case WM_HSCROLL:
               this->view_origin.x = scroll_pos;
               break;
            case WM_VSCROLL:
               this->view_origin.y = scroll_pos;
               break;
            }

            CGCanvas(this).view_update(FALSE);

            UpdateWindow(hwnd);
            }
         }
      break;
   case WM_NOTIFY:
      /*>>>problem in template expanstion when parameter contains ',' character*/
      udata = (void *)GetWindowLong(((LPNMHDR)lParam)->hwndFrom, GWL_USERDATA);
      xul_element = CGXULElement(udata);
      if (xul_element) {
         result = CGXULElement(xul_element).WIN32_XULELEMENT_MSG(message, wParam, lParam);
         if (result) {
            return result;
         }
      }
      break;
   case WM_SIZING:
//      if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,width>) &&
//          !CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,height>)) {
//         /* disallow user window size changes */
//         GetWindowRect(hwnd, (LPRECT)lParam);
//      }
      break;
   case WM_SHOWWINDOW:
      if (!this)
         break;
   case WM_SIZE:
      GetClientRect(hwnd, &window_rect);
      this->allocated_width  = window_rect.right  - window_rect.left;
      this->allocated_height = window_rect.bottom - window_rect.top;

      CGCanvas(this).NATIVE_size_allocate();
      break;
   case WM_SETCURSOR:
      if (GCanvas_NATIVE_DATA(this)->hcursor) {
         SetCursor(GCanvas_NATIVE_DATA(this)->hcursor);
         return TRUE;
      }
      break;
#if 0
   /*>>>to consider?*/
   case WM_SETFOCUS:
      if (CObjClass_is_derived(&class(CGLayout), CObject(this).obj_class())) {
         CGLayout(this).key_gselection_set(CGObject(this));
      }
      break;
#endif
   case WM_KEYDOWN:
   case WM_SYSKEYDOWN:
   case WM_KEYUP:
   case WM_SYSKEYUP:
   case WM_SYSCHAR:
   case WM_CHAR:
      /* Echo through to parent window */
      window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
      SendMessage((HWND)CGObject(window)->native_object, message, wParam, lParam);
      return TRUE;

   case 0x0127: //WM_CHANGEUISTATE
      wParam &= 0xFFFEFFFF;
      break;
   case 0x0128: //WM_UPDATEUISTATE
   case 0x0129: //WM_QUERYUISTATE
      break;
   case 0x020A: //WM_MOUSEWHEEL:
      event_type = (short)HIWORD(wParam) > 0 ? EEventPointer.ScrollUp : EEventPointer.ScrollDown;
      goto mouse_event;
   case WM_MOUSEMOVE:
      if (wParam != 0xFFFF) {
         /* 0xFFFF means repeated event from a child of this canvas, don't track */
         CLEAR(&trackmouseevent);
         trackmouseevent.cbSize = sizeof(TRACKMOUSEEVENT);
         trackmouseevent.dwFlags = TME_LEAVE;
         trackmouseevent.hwndTrack = hwnd;
         trackmouseevent.dwHoverTime = HOVER_DEFAULT;
         _TrackMouseEvent(&trackmouseevent);
      }

//      this->pointer_position.x = (short)LOWORD(lParam) + this->viewBox.point[0].x;
//      this->pointer_position.y = (short)HIWORD(lParam) + this->viewBox.point[0].y;
//printf("mouse position %d, %g, %g\n", this, this->pointer_position.x, this->pointer_position.y);
      event_type = EEventPointer.Move;
      goto mouse_event;
   case WM_LBUTTONDOWN:
      event_type = EEventPointer.LeftDown;
      goto mouse_event;
   case WM_LBUTTONUP:
      event_type = EEventPointer.LeftUp;
      goto mouse_event;
   case WM_LBUTTONDBLCLK:
      event_type = EEventPointer.LeftDClick;
      goto mouse_event;
   case WM_RBUTTONDOWN:
      event_type = EEventPointer.RightDown;
      goto mouse_event;
   case WM_RBUTTONUP:
      event_type = EEventPointer.RightUp;
      goto mouse_event;
   case WM_MOUSELEAVE:
      event_type = EEventPointer.Leave;
      goto mouse_event;
   mouse_event:
      this->pointer_position.x = (short)LOWORD(lParam) + this->viewBox.point[0].x;
      this->pointer_position.y = (short)HIWORD(lParam) + this->viewBox.point[0].y;

      modifier = 0;
      modifier |= wParam & MK_CONTROL ? (1 << EEventModifier.ctrlKey) : 0;
      modifier |= wParam & MK_SHIFT ? (1 << EEventModifier.shiftKey) : 0;
      modifier |= wParam & MK_ALT ? (1 << EEventModifier.altKey) : 0;

      CGCanvas(this).NATIVE_enter(TRUE);
      new(&event).CEventPointer(event_type,
                                (short)LOWORD(lParam) + (int)this->viewBox.point[0].x,
                                (short)HIWORD(lParam) + (int)this->viewBox.point[0].y,
                                modifier);
      CGObject(this).event(CEvent(&event));
      delete(&event);
      CGCanvas(this).NATIVE_enter(FALSE);
      return 0;
   }
//   return 0;
   return DefWindowProc(hwnd, message, wParam, lParam);
}/*CGCanvas_WIN32_wnd_proc*/


void CGCanvas::NATIVE_enter(bool enter) {
   if (enter) {
      if (!this->enter_count) {
         if (CObject(this).obj_class() == &class(CGCanvasBitmap)) {
            GCanvas_NATIVE_DATA(this)->hdc = CreateCompatibleDC(NULL);
            SelectObject(GCanvas_NATIVE_DATA(this)->hdc, (HBITMAP)CGCanvasBitmap(this)->bitmap.native_object);
         }
         else if (CObject(this).obj_class() == &class(CGCanvasPrint)) {
      }
      else {
            GCanvas_NATIVE_DATA(this)->hdc = GetDC(GCanvas_NATIVE_DATA(this)->hwnd);
         }
         SetBkMode(GCanvas_NATIVE_DATA(this)->hdc, TRANSPARENT);
#if KEYSTONE_WIN32_GDIP
         if (this->render_mode_canvas == EGCanvasRender.full) {
            win32_gdiplus_init();
            GdipCreateFromHDC(GCanvas_NATIVE_DATA(this)->hdc, &GCanvas_NATIVE_DATA(this)->gdipGraphics);
            GdipSetSmoothingMode(GCanvas_NATIVE_DATA(this)->gdipGraphics, SmoothingModeAntiAlias);
            GdipCreatePen1(Color(255, 0, 0, 0), 0, 0, &GCanvas_NATIVE_DATA(this)->gdipPen);
            GdipCreateSolidFill(Color(255, 0, 0, 0), &GCanvas_NATIVE_DATA(this)->gdipBrushSolid);
            GdipCreateSolidFill(Color(255, 255, 0, 0), &GCanvas_NATIVE_DATA(this)->gdipBrushLinGr);
            if (CObject(this).obj_class() == &class(CGCanvasPrint)) {
               GdipSetPageUnit(GCanvas_NATIVE_DATA(this)->gdipGraphics, UnitPixel);
            }
         }
#endif
      }
      this->enter_count++;
   }
   else {
      this->enter_count--;
      if (!this->enter_count) {
#if KEYSTONE_WIN32_GDIP
         if (this->render_mode_canvas == EGCanvasRender.full) {
            GdipDeletePen(GCanvas_NATIVE_DATA(this)->gdipPen);
            GdipDeleteBrush(GCanvas_NATIVE_DATA(this)->gdipBrushSolid);
            GdipDeleteBrush(GCanvas_NATIVE_DATA(this)->gdipBrushLinGr);

            GdipDeleteGraphics(GCanvas_NATIVE_DATA(this)->gdipGraphics);
         }
#endif
         DeleteObject(GCanvas_NATIVE_DATA(this)->hPen);
         DeleteObject(GCanvas_NATIVE_DATA(this)->hBrush);
         DeleteObject(GCanvas_NATIVE_DATA(this)->hFont);

         if (CObject(this).obj_class() == &class(CGCanvasBitmap)) {
            DeleteDC(GCanvas_NATIVE_DATA(this)->hdc);
         }
         else if (CObject(this).obj_class() == &class(CGCanvasPrint)) {
            return;
         }
         else {
            ReleaseDC(GCanvas_NATIVE_DATA(this)->hwnd, GCanvas_NATIVE_DATA(this)->hdc);
         }
         GCanvas_NATIVE_DATA(this)->hdc = 0;
      }
   }
}/*CGCanvas::NATIVE_enter*/

void CGCanvas::NATIVE_allocate(CGLayout *layout) {
   static TCHAR szClassName[] = TEXT("GRCanvas");
   WNDCLASS cls;
   RECT wsize = {0, 0, 640, 480};
   HWND *hwnd = (HWND *)&CGObject(this)->native_object;
   DWORD style, style_ex;
   CGObject *parent;

   cls.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
   cls.lpfnWndProc   = CGCanvas_WIN32_wnd_proc;
   cls.cbClsExtra    = 0;
   cls.cbWndExtra    = sizeof(long);
   cls.hInstance     = GetModuleHandle(NULL);
   cls.hIcon         = NULL;
   cls.hbrBackground = NULL;//GetStockObject(LTGRAY_BRUSH);
   cls.lpszMenuName  = NULL;
   cls.lpszClassName = szClassName;
   cls.hCursor       = LoadCursor(NULL, IDC_ARROW);

   style = WS_CHILD;// | WS_CLIPCHILDREN;
   style_ex = 0;
   if (CObject(CObject(this).parent()).obj_class() == &class(CGSplitter)) {
      style_ex = WS_EX_CLIENTEDGE;
   }

   parent = CGObject(CObject(this).parent());
   while (parent) {
      if (CObjClass_is_derived(&class(CGCanvas), CObject(parent).obj_class())   ||
          CObjClass_is_derived(&class(CGWindow), CObject(parent).obj_class())   ||
          CObjClass_is_derived(&class(CGTabBox), CObject(parent).obj_class())   ||
          CObjClass_is_derived(&class(CGSplitter), CObject(parent).obj_class())
         ) {
         break;
      }
      parent = CGObject(CObject(parent).parent());
   }

   RegisterClass(&cls);

   wsize.right  = (int)CGXULElement(this)->width  - 1;
   wsize.bottom = (int)CGXULElement(this)->height - 1;

   /* Create the main window here */
   AdjustWindowRectEx(&wsize, style, FALSE, style_ex);

   *hwnd =
      CreateWindowEx(style_ex, szClassName, NULL, style,
                     (int)CGXULElement(this)->x, (int)CGXULElement(this)->y, wsize.right - wsize.left + 1, wsize.bottom - wsize.top + 1,
                     (HWND)CGObject(parent)->native_object, 0, GetModuleHandle(NULL), (LPVOID)this);

//   SetWindowLong(*hwnd, GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN);

   class:base.NATIVE_allocate(layout);
}/*CGCanvas::NATIVE_allocate*/

void CGCanvas::NATIVE_release(CGLayout *layout) {
   /* release any GDI objects selected into the device context */
   DeleteObject(SelectObject(GCanvas_NATIVE_DATA(this)->hdc, GetStockObject(NULL_BRUSH)));
   DeleteObject(SelectObject(GCanvas_NATIVE_DATA(this)->hdc, GetStockObject(BLACK_PEN)));
   DeleteObject(SelectObject(GCanvas_NATIVE_DATA(this)->hdc, GetStockObject(SYSTEM_FONT)));

   /* delete the device context */
   DeleteDC(GCanvas_NATIVE_DATA(this)->hdc);

   GCanvas_NATIVE_DATA(this)->hdc = NULL;

   if (CObject(this).obj_class() != &class(CGCanvasBitmap)) {
      /*>>>kludge, shouldn't need 'SetParent', canvas is sometimes not hidden from screen on delete */
      SetParent((HWND)CGObject(this)->native_object, NULL);
      class:base.NATIVE_release(layout);
   }
}/*CGCanvas::NATIVE_release*/

void CGCanvas::NATIVE_show(bool show, bool end) {
    SetWindowLong((HWND)CGObject(this)->native_object, GWL_STYLE, WS_CHILD | (end ? WS_CLIPCHILDREN : 0));
    if (show && end) {
        ShowWindow((HWND)CGObject(this)->native_object, TRUE);
    }
}/*CGCanvas::NATIVE_show*/

void CGCanvas::NATIVE_size_allocate(void) {
   /*>>>resets scroll bars at present.  Find a better way off keeping scroll position of size change */

   SCROLLINFO si;
   HWND hwnd = (HWND)CGObject(this)->native_object;

   if (!hwnd)
      return;

   if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,width>) &&
      CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,height>)) {
      CGXULElement(this)->width  = this->allocated_width;//window_rect.right  - window_rect.left;
      CGXULElement(this)->height = this->allocated_height;//window_rect.bottom - window_rect.top;
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_update_end();
   }

   CGCanvas(this).view_update(TRUE);

   if (this->allocated_width < this->scroll_width && this->scrollMode != EGCanvasScroll.none) {
      /* Reset scroll bar parameters */
      si.cbSize = sizeof(SCROLLINFO);
      si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
      si.nPos   = (int)this->view_origin.x;
      si.nMin   = 0;

      /* horizontal */
      si.nMax  = this->scroll_width - 1;
      si.nPage = this->allocated_width;
      SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

      ShowScrollBar(hwnd, SB_HORZ, TRUE);
      }
   else {
      ShowScrollBar(hwnd, SB_HORZ, FALSE);
   }

   if (this->allocated_height < this->scroll_height && this->scrollMode != EGCanvasScroll.none) {
      /* Reset scroll bar parameters */
      si.cbSize = sizeof(SCROLLINFO);
      si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
      si.nPos   = (int)this->view_origin.y;
      si.nMin   = 0;

      /* vertical */
      si.nMax  = this->scroll_height - 1;
      si.nPage = this->allocated_height;
      SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

      ShowScrollBar(hwnd, SB_VERT, TRUE);
      }
   else {
      ShowScrollBar(hwnd, SB_VERT, FALSE);
   }

//   /* update canvas origin */
//    this->view_origin.x = 0;
//    this->view_origin.y = 0;

   /*>>>shouldn't need this*/
   InvalidateRect(hwnd, NULL, TRUE);
}/*CGCanvas::NATIVE_size_allocate*/

void CGCanvas::point_ctos(TPoint *point) {
   POINT wPoint;
   wPoint.x = (LONG)point->x;
   wPoint.y = (LONG)point->y;
   ClientToScreen((HWND)CGObject(this)->native_object, &wPoint);
   point->x = wPoint.x;
   point->y = wPoint.y;
}/*CGCanvas::point_ctos*/

void CGCanvas::point_stoc(TPoint *point) {
   POINT wPoint; 
   wPoint.x = (LONG)point->x;
   wPoint.y = (LONG)point->y;
   ScreenToClient((HWND)CGObject(this)->native_object, &wPoint);
   point->x = wPoint.x;
   point->y = wPoint.y;
}/*CGCanvas::point_stoc*/

void CGCanvas::pointer_position_get(TPoint *position) {
   POINT wPoint;

   GetCursorPos(&wPoint);
   ScreenToClient((HWND)CGObject(this)->native_object, &wPoint);
   position->x = wPoint.x;
   position->y = wPoint.y;
}/*CGCanvas::pointer_position_get*/

void CGCanvas::pointer_position_set(const TPoint *position) {
   POINT wPoint;
   wPoint.x = (LONG)position->x;
   wPoint.y = (LONG)position->y;
   ClientToScreen((HWND)CGObject(this)->native_object, &wPoint);
   SetCursorPos(wPoint.x, wPoint.y);
}/*CGCanvas::pointer_position_set*/

void CGCanvas::pointer_cursor_set(EGPointerCursor pointer_cursor) {
   if (this->pointer_cursor != pointer_cursor) {
      switch (pointer_cursor) {
      case EGPointerCursor.default:
         GCanvas_NATIVE_DATA(this)->hcursor = NULL;//LoadCursor(NULL, IDC_ARROW);//NULL;
         break;
      case EGPointerCursor.pointer:
         /* hand cursor, may only work for Win98 onwards*/
         GCanvas_NATIVE_DATA(this)->hcursor = LoadCursor(NULL, MAKEINTRESOURCE(32649));
         break;
      case EGPointerCursor.crosshair:
         GCanvas_NATIVE_DATA(this)->hcursor = LoadCursor(NULL, IDC_CROSS);
         break;
      case EGPointerCursor.wait:
         GCanvas_NATIVE_DATA(this)->hcursor = LoadCursor(NULL, IDC_WAIT);
         break;
      case EGPointerCursor.move:
         GCanvas_NATIVE_DATA(this)->hcursor = LoadCursor(NULL, IDC_SIZEALL);
         break;
      case EGPointerCursor.resize_n:
      case EGPointerCursor.resize_s:
         GCanvas_NATIVE_DATA(this)->hcursor = LoadCursor(NULL, IDC_SIZENS);
         break;
      case EGPointerCursor.resize_e:
      case EGPointerCursor.resize_w:
         GCanvas_NATIVE_DATA(this)->hcursor = LoadCursor(NULL, IDC_SIZEWE);
         break;
      case EGPointerCursor.resize_ne:
      case EGPointerCursor.resize_sw:
         GCanvas_NATIVE_DATA(this)->hcursor = LoadCursor(NULL, IDC_SIZENESW);
         break;
      case EGPointerCursor.resize_nw:
      case EGPointerCursor.resize_se:
         GCanvas_NATIVE_DATA(this)->hcursor = LoadCursor(NULL, IDC_SIZENWSE);
         break;
      }
      SetCursor(GCanvas_NATIVE_DATA(this)->hcursor);

   this->pointer_cursor = pointer_cursor;
   }
}/*CGCanvas::pointer_cursor_set*/

void CGCanvas::pointer_capture(bool capture) {
   if (capture) {
      SetCapture((HWND)CGObject(this)->native_object);
   }
   else {
      ReleaseCapture();
   }
}/*CGCanvas::pointer_capture*/

void CGCanvas::xor_write_set(bool xor_write) {
   /* For Win32, switch back to GDI basic for xor writing */
   if (xor_write) {
      CGCanvas(this).render_set(EGCanvasRender.basic);
   }
   SetROP2(GCanvas_NATIVE_DATA(this)->hdc, xor_write ? R2_XORPEN : R2_COPYPEN);
   this->xor_write = xor_write;

   if (!xor_write) {
      CGCanvas(this).render_restore();
   }
}/*CGCanvas::xor_write_set*/

void CGCanvas::NATIVE_queue_draw(const TRect *extent) {
   RECT rect;
   if ((HWND)CGObject(this)->native_object) {
      if (extent) {
         rect.left   = (int)extent->point[0].x;
         rect.top    = (int)extent->point[0].y;
         rect.right  = (int)extent->point[1].x + 1;
         rect.bottom = (int)extent->point[1].y + 1;
#if KEYSTONE_WIN32_GDIP
         /* If using antialias (GDIP) extend redraw extent by 1 pixel
            >>>perhaps better done elsewhere */
         rect.left--;
         rect.top--;
         rect.right++;
         rect.bottom++;
#endif
         InvalidateRect((HWND)CGObject(this)->native_object, &rect, TRUE);
         if (this->native_redraw_immediate) {
            UpdateWindow((HWND)CGObject(this)->native_object);
         }
      }
      else {
         InvalidateRect((HWND)CGObject(this)->native_object, NULL, TRUE);
      }
   }
}/*CGCanvas::NATIVE_queue_draw*/

void CGCanvas::NATIVE_line_set(TGColour colour, bool fill) {
   /*>>>inefficient, should check if new colour is the same as the last */
   LOGBRUSH logbrush;
   DWORD style;
   TGColour rgb_colour;
#if KEYSTONE_WIN32_GDIP
   /*>>>kludge, this should be held in canvas (this->storke_dasharray)*/
   int i;
   static REAL dasharray[100];
   LineCap linecap;
   LineJoin linejoin;
#endif

   rgb_colour = GCOL_CONVERT_RGB(this->palette, this->paletteB, colour);

#if KEYSTONE_WIN32_GDIP
   if (this->render_mode == EGCanvasRender.full) {
      GdipSetPenColor(GCanvas_NATIVE_DATA(this)->gdipPen, Color(colour == GCOL_NONE ? 0 : (int)(this->opacity * 255),
                      GCOL_RGB_RED(rgb_colour), GCOL_RGB_GREEN(rgb_colour), GCOL_RGB_BLUE(rgb_colour)));
      GdipSetPenWidth(GCanvas_NATIVE_DATA(this)->gdipPen, (REAL)(fill ? 0 : this->stroke_width));

      switch (this->stroke_linecap) {
      case EGStrokeLineCap.none:      
      case EGStrokeLineCap.butt:
         linecap = LineCapFlat;
         break;
      case EGStrokeLineCap.round:
         linecap = LineCapRound;
         break;
      case EGStrokeLineCap.square:
         linecap = LineCapSquare;
         break;
      default:
         linecap = LineCapFlat;
         ASSERT("CGCanvas::NATIVE_line_set - bad cap style");
      }

      switch (this->stroke_linejoin) {
      case EGStrokeLineJoin.none:
      case EGStrokeLineJoin.bevel:
         linejoin = LineJoinBevel;
         break;
      case EGStrokeLineJoin.miter:
         linejoin = LineJoinMiter;
         GdipSetPenMiterLimit(GCanvas_NATIVE_DATA(this)->gdipPen, (REAL)this->stroke_miterlimit);
         break;
      case EGStrokeLineJoin.round:
         linejoin = LineJoinRound;
         break;
      default:
         linejoin = LineJoinBevel;
         ASSERT("CGCanvas::NATIVE_line_set - bad join style");
      }

      GdipSetPenStartCap(GCanvas_NATIVE_DATA(this)->gdipPen, linecap);
      GdipSetPenEndCap(GCanvas_NATIVE_DATA(this)->gdipPen, linecap);
      GdipSetPenLineJoin(GCanvas_NATIVE_DATA(this)->gdipPen, linejoin);

      if (ARRAY(&this->stroke_dasharray).count()) {
         for (i = 0; i < ARRAY(&this->stroke_dasharray).count() && i < 100; i++) {
             dasharray[i] = (REAL)(ARRAY(&this->stroke_dasharray).data()[i] / (this->stroke_width ? this->stroke_width : 1));
          }
          GdipSetPenDashArray(GCanvas_NATIVE_DATA(this)->gdipPen, dasharray, ARRAY(&this->stroke_dasharray).count());
          GdipSetPenDashOffset(GCanvas_NATIVE_DATA(this)->gdipPen, (REAL)(this->stroke_dashoffset / (this->stroke_width ? this->stroke_width : 1)));
      }
      else {
         GdipSetPenDashStyle(GCanvas_NATIVE_DATA(this)->gdipPen, 0);
      }
      return;
   }
#endif

   if (colour == GCOL_NONE) {
      GCanvas_NATIVE_DATA(this)->hPen = CreatePen(PS_NULL, 0, 0);
   }
   else {
      CLEAR(&logbrush);
      logbrush.lbStyle = BS_SOLID;
      logbrush.lbColor = RGB(GCOL_RGB_RED(rgb_colour), GCOL_RGB_GREEN(rgb_colour), GCOL_RGB_BLUE(rgb_colour));
      logbrush.lbHatch = 0;

      style = PS_GEOMETRIC;
      switch (this->stroke_linecap) {
      case EGStrokeLineCap.none:            
      case EGStrokeLineCap.butt:
         style |= PS_ENDCAP_FLAT;
         break;
      case EGStrokeLineCap.round:
         style |= PS_ENDCAP_ROUND;
         break;
      case EGStrokeLineCap.square:
         style |= PS_ENDCAP_SQUARE;
         break;
      default:
        ASSERT("CGCanvas::NATIVE_line_set - bad cap style");
      }

      switch (this->stroke_linejoin) {
      case EGStrokeLineJoin.none:            
      case EGStrokeLineJoin.bevel:
         style |= PS_JOIN_BEVEL;
         break;
      case EGStrokeLineJoin.miter:
         SetMiterLimit(GCanvas_NATIVE_DATA(this)->hdc, (REAL)this->stroke_miterlimit, NULL);
         style |= PS_JOIN_MITER;
         break;
      case EGStrokeLineJoin.round:
         style |= PS_JOIN_ROUND;
         break;
      default:
        ASSERT("CGCanvas::NATIVE_line_set - bad cap style");
      }

      if (ARRAY(&this->stroke_dasharray).count()) {
         GCanvas_NATIVE_DATA(this)->hPen = ExtCreatePen(style | PS_USERSTYLE | PS_GEOMETRIC,
                             (int)(fill ? 0 : (int)this->stroke_width), &logbrush,
                             ARRAY(&this->stroke_dasharray).count(),
                             (DWORD *)ARRAY(&this->stroke_dasharray).data());
      }
      else {
         GCanvas_NATIVE_DATA(this)->hPen = ExtCreatePen(style | PS_SOLID | PS_GEOMETRIC,
                             fill ? 0 : (int)this->stroke_width, &logbrush, 0, NULL);
      }
   }

   DeleteObject(SelectObject(GCanvas_NATIVE_DATA(this)->hdc, GCanvas_NATIVE_DATA(this)->hPen));
}/*CGCanvas::NATIVE_line_set*/

void CGCanvas::NATIVE_fill_set(TGColour colour, bool fill) {
   /*>>>inefficient, should check if new colour is the same as the last */
   LOGBRUSH logbrush;
   TGColour rgb_colour;
   int i;
   CObject *child;
   CGLinearGradient *lin_gr;

   rgb_colour = GCOL_CONVERT_RGB(this->palette, this->paletteB, colour);

#if KEYSTONE_WIN32_GDIP
   if (this->render_mode == EGCanvasRender.full) {
      if (GCOL_TYPE(colour) == GCOL_TYPE_GRADIENT) {
         /*lookup gradient >>>kludgy*/
         child = CObject(&CGLayout(this)->defs).child_first();
         i = 0;
         while (child) {
            if (CObjClass_is_derived(&class(CGLinearGradient), CObject(child).obj_class())) {
               if (i == GCOL_GRADIENT_INDEX(colour)) {
                  break;
               }
               i++;
            }
            child = CObject(&CGLayout(this)->defs).child_next(CObject(child));
         }
         if (child) {
             lin_gr = CGLinearGradient(child);
             child = CObject(lin_gr).child_first();
             rgb_colour = GCOL_CONVERT_RGB(this->palette, this->paletteB, CGGradientStop(child)->stop_colour);
             GdipSetSolidFillColor(GCanvas_NATIVE_DATA(this)->gdipBrushLinGr,
                                   Color(colour == GCOL_NONE ? 0 : (int)(this->opacity * 255),
                                   GCOL_RGB_RED(rgb_colour), GCOL_RGB_GREEN(rgb_colour), GCOL_RGB_BLUE(rgb_colour)));
         }

         GCanvas_NATIVE_DATA(this)->gdipBrush = GCanvas_NATIVE_DATA(this)->gdipBrushLinGr;
      }
      else {
         GdipSetSolidFillColor(GCanvas_NATIVE_DATA(this)->gdipBrushSolid,
                               Color(colour == GCOL_NONE ? 0 : (int)(this->opacity * 255),
                               GCOL_RGB_RED(rgb_colour), GCOL_RGB_GREEN(rgb_colour), GCOL_RGB_BLUE(rgb_colour)));
         GCanvas_NATIVE_DATA(this)->gdipBrush = GCanvas_NATIVE_DATA(this)->gdipBrushSolid;
      }
      return;
   }
#endif

   CLEAR(&logbrush);
   logbrush.lbStyle = fill ? BS_SOLID : BS_HOLLOW;
   logbrush.lbColor =
      GetNearestColor(GCanvas_NATIVE_DATA(this)->hdc,
                      RGB(GCOL_RGB_RED(rgb_colour), GCOL_RGB_GREEN(rgb_colour), GCOL_RGB_BLUE(rgb_colour)));

   GCanvas_NATIVE_DATA(this)->hBrush = CreateBrushIndirect(&logbrush);
   DeleteObject(SelectObject(GCanvas_NATIVE_DATA(this)->hdc, GCanvas_NATIVE_DATA(this)->hBrush));
}/*CGCanvas::NATIVE_fill_set*/

void CGCanvas::NATIVE_stroke_style_set(double stroke_width, ARRAY<int> *stroke_dasharray, int stroke_dashoffset,
                                       int stroke_miterlimit,
                                       EGStrokeLineCap stroke_linecap, EGStrokeLineJoin stroke_linejoin) {
   if (stroke_dasharray) {
      ARRAY(&this->stroke_dasharray).copy(stroke_dasharray);
   }
   else {
      ARRAY(&this->stroke_dasharray).used_set(0);
   }
   this->stroke_width      = stroke_width;
   this->stroke_dashoffset = stroke_dashoffset;
   this->stroke_miterlimit = stroke_miterlimit;
   this->stroke_linecap    = stroke_linecap;
   this->stroke_linejoin   = stroke_linejoin;
}/*CGCanvas::NATIVE_stroke_style_set*/

void CGCanvas::NATIVE_clip_set_rect(TRect *rect) {
#if KEYSTONE_WIN32_GDIP
   GdipSetClipRect(GCanvas_NATIVE_DATA(this)->gdipGraphics, (REAL)rect->point[0].x, (REAL)rect->point[0].y,
                   (REAL)(rect->point[1].x - rect->point[0].x), (REAL)(rect->point[1].y - rect->point[0].y), CombineModeReplace);
//   GdipSetClipRect(GCanvas_NATIVE_DATA(this)->gdipGraphics, 0, 0, 700, 1400, CombineModeReplace);
#endif
}/*CGCanvas::NATIVE_clip_set_rect*/

void CGCanvas::NATIVE_clip_reset(void) {
#if KEYSTONE_WIN32_GDIP
   GdipResetClip(GCanvas_NATIVE_DATA(this)->gdipGraphics);
#endif
}/*CGCanvas::NATIVE_clip_reset*/

void CGCanvas::NATIVE_draw_point(TGColour colour, TCoord x, TCoord y) {
   colour = GCOL_CONVERT_RGB(this->palette, this->paletteB, colour);

   SetPixel(GCanvas_NATIVE_DATA(this)->hdc, (int)x, (int)y, RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));
}/*CGCanvas::NATIVE_draw_point*/

void CGCanvas::NATIVE_draw_line(TGColour colour, TCoord x, TCoord y, TCoord x2, TCoord y2) {
   bool oneaxis;
   CGCanvas(this).NATIVE_line_set(colour, FALSE);

#if KEYSTONE_WIN32_GDIP
   if (this->render_mode == EGCanvasRender.full) {
      oneaxis = (x == x2 || y == y2);
      if (oneaxis) {
         GdipSetSmoothingMode(GCanvas_NATIVE_DATA(this)->gdipGraphics, SmoothingModeNone);
         GdipSetPenWidth(GCanvas_NATIVE_DATA(this)->gdipPen, (REAL)(int)(this->stroke_width + 0.5));
      }
      GdipDrawLine(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipPen, (REAL)x, (REAL)y, (REAL)x2, (REAL)y2);
      if (oneaxis) {
         GdipSetSmoothingMode(GCanvas_NATIVE_DATA(this)->gdipGraphics, SmoothingModeAntiAlias);
      }
      return;
   }
#endif

    //   CGCanvas(this).NATIVE_draw_point(colour, x2, y2);
   MoveToEx(GCanvas_NATIVE_DATA(this)->hdc, (int)x, (int)y, NULL);
   LineTo(GCanvas_NATIVE_DATA(this)->hdc, (int)x2, (int)y2);
}/*CGCanvas::NATIVE_draw_line*/

void CGCanvas::NATIVE_draw_rectangle(TGColour colour, bool fill, TCoord x, TCoord y, TCoord x2, TCoord y2) {
   POINT point[5];
#if KEYSTONE_WIN32_GDIP
   Point gp_point[5];
#endif

   CGCanvas(this).NATIVE_fill_set(colour, fill);
   CGCanvas(this).NATIVE_line_set(colour, fill);

#if KEYSTONE_WIN32_GDIP
   if (this->render_mode == EGCanvasRender.full) {
      gp_point[0].X = (REAL)x;  gp_point[0].Y = (REAL)y;
      gp_point[1].X = (REAL)x2; gp_point[1].Y = (REAL)y;
      gp_point[2].X = (REAL)x2; gp_point[2].Y = (REAL)y2;
      gp_point[3].X = (REAL)x;  gp_point[3].Y = (REAL)y2;
      gp_point[4].X = (REAL)x;  gp_point[4].Y = (REAL)y;

      if (fill) {
         GdipFillPolygon2(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipBrush,
                          gp_point, COUNTOF(gp_point));
      }
      else {
         GdipDrawPolygon(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipPen,
                         gp_point, COUNTOF(gp_point));
      }
      return;
   }
#endif
   point[0].x = (int)x;  point[0].y = (int)y;
   point[1].x = (int)x2; point[1].y = (int)y;
   point[2].x = (int)x2; point[2].y = (int)y2;
   point[3].x = (int)x;  point[3].y = (int)y2;
   point[4].x = (int)x;  point[4].y = (int)y;

   Polygon(GCanvas_NATIVE_DATA(this)->hdc, point, COUNTOF(point));
}/*CGCanvas::NATIVE_draw_rectangle*/

void CGCanvas::NATIVE_draw_ellipse(TGColour colour, bool fill, TCoord x, TCoord y, TCoord rx, TCoord ry) {
   double stroke_width = this->stroke_width;

   CGCanvas(this).NATIVE_fill_set(colour, fill);

   if (fill) {
      this->stroke_width = 1;
   }
   CGCanvas(this).NATIVE_line_set(colour, fill);
   if (fill) {
      this->stroke_width = stroke_width;
   }

#if KEYSTONE_WIN32_GDIP
   if (this->render_mode == EGCanvasRender.full) {
      if (fill) {
         GdipFillEllipse(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipBrush, 
                         (REAL)(x - rx), (REAL)(y - ry), (REAL)(rx * 2), (REAL)(ry * 2));
      }
      else {
         GdipDrawEllipse(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipPen, 
                         (REAL)(x - rx), (REAL)(y - ry), (REAL)(rx * 2), (REAL)(ry * 2));
      }
      return;
   }
#endif
   Ellipse(GCanvas_NATIVE_DATA(this)->hdc, (int)(x - rx), (int)(y - ry), (int)(x + rx + 1), (int)(y + ry + 1));
}/*CGCanvas::NATIVE_draw_ellipse*/

void CGCanvas::NATIVE_draw_polygon(TGColour colour, bool fill,
                                   int point_count, TPoint *point_data, bool final_connect) {
#if KEYSTONE_WIN32_GDIP
   Point *gp_point;
   GpPath *path;
#endif
   POINT *point;
   int i;

   CGCanvas(this).NATIVE_fill_set(colour, fill);
   CGCanvas(this).NATIVE_line_set(colour, fill);

#if KEYSTONE_WIN32_GDIP
   if (this->render_mode == EGCanvasRender.full) {
      gp_point = CFramework(&framework).scratch_buffer_allocate();
      /* convert coords >>>not, data already in framework static_buffer! */
      /*>>>dosn't draw polylines at present, will probably need to use a path for this */
      for (i = 0; i < point_count; i++) {
         gp_point[i].X = (REAL)point_data[i].x;
         gp_point[i].Y = (REAL)point_data[i].y;
      }

      GdipCreatePath(0, &path);
      GdipAddPathLine2(path, gp_point, point_count);

      if (final_connect) {
         GdipClosePathFigure(path);
      }

      if (fill) {
         GdipFillPath(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipBrush, path);
//         GdipFillPolygon2(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipBrush,
//                          gp_point, point_count);
      }
      else {
          GdipDrawPath(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipPen, path);
//         GdipDrawPolygon(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipPen,
//                         gp_point, point_count);
      }

      GdipDeletePath(path);

      CFramework(&framework).scratch_buffer_release(gp_point);
      return;
   }
#endif
   point = CFramework(&framework).scratch_buffer_allocate();
   /* convert coords >>>not, data already in framework static_buffer! */
   for (i = 0; i < point_count; i++) {
      point[i].x = (int)(point_data[i].x + 0.5);
      point[i].y = (int)(point_data[i].y + 0.5);
   }
   if (!fill && !final_connect) {
      Polyline(GCanvas_NATIVE_DATA(this)->hdc, point, point_count);
   }
   else {
      Polygon(GCanvas_NATIVE_DATA(this)->hdc, point, point_count);
   }

   CFramework(&framework).scratch_buffer_release(point);
}/*CGCanvas::NATIVE_draw_polygon*/

void CGCanvas::NATIVE_draw_path(TGColour colour, bool fill,
                                 int instruction_count, TGPathInstruction *instruction) {
#if KEYSTONE_WIN32_GDIP
   GpPath *path = NULL;
   GpPath *arc_path;
   GpMatrix *arc_transform;
#endif
   POINT *p_point = NULL;
   EGPathInstruction ins_op, ins_last_op = EGPathInstruction.NoOp;
   TGPathInstruction *ip;
   int i, j;
   TPoint point, point_first, point_last, point_control_last = {0,0}, point_t[4];
   bool closepath = FALSE;
   double cx, cy, rx, ry, th0, thd;
   TCoord rotation;

   CGCanvas(this).NATIVE_fill_set(colour, fill);
   CGCanvas(this).NATIVE_line_set(colour, fill);

   j = 0;
   if (this->render_mode == EGCanvasRender.full) {
#if KEYSTONE_WIN32_GDIP
      GdipCreatePath(FillModeAlternate, &path);
#endif
   }
   else {
      p_point = CFramework(&framework).scratch_buffer_allocate();
   }
   point.x = 0;
   point.y = 0;
   point_last.x = 0;
   point_last.y = 0;
   for (i = 0; i < instruction_count; i++) {
      ip = &instruction[i];
      ins_op = instruction[i].instruction == EGPathInstruction.Repeat ? ins_last_op : instruction[i].instruction;
      ins_last_op = ins_op;
      switch (ins_op) {
      case EGPathInstruction.NoOp:
         break;
      case EGPathInstruction.MoveToAbs:
         point.x = ip->point[0].x;
         point.y = ip->point[0].y;
         goto path;
      case EGPathInstruction.MoveToRel:
         point.x += ip->point[0].x;
         point.y += ip->point[0].y;
         goto path;
      case EGPathInstruction.ClosePath:
      case EGPathInstruction.ClosePathAlias:
         closepath = TRUE;
//         point = point_first;
#if KEYSTONE_WIN32_GDIP
         if (this->render_mode == EGCanvasRender.full) {
            GdipClosePathFigure(path);
         }
#endif
         break;
      case EGPathInstruction.LineToAbs:
         point.x = ip->point[0].x;
         point.y = ip->point[0].y;
         goto point;
      case EGPathInstruction.LineToRel:
         point.x += ip->point[0].x;
         point.y += ip->point[0].y;
         goto point;
      case EGPathInstruction.HLineToAbs:
         point.x = ip->point[0].x;
         goto point;
      case EGPathInstruction.HLineToRel:
         point.x += ip->point[0].x;
         goto point;
      case EGPathInstruction.VLineToAbs:
         point.y = ip->point[0].y;
         goto point;
      case EGPathInstruction.VLineToRel:
         point.y += ip->point[0].y;
         goto point;
      path:
//         if (0) {};
         point_first = point;
#if KEYSTONE_WIN32_GDIP
         if (this->render_mode == EGCanvasRender.full) {
            if (j) {
               GdipStartPathFigure(path);
            }
            closepath = FALSE;
            j = 0;
         }
#endif
      point:
         if (this->render_mode == EGCanvasRender.full) {
#if KEYSTONE_WIN32_GDIP
            if (j > 0) {
               point_t[0] = point_last;
               point_t[1] = point;
               CGCoordSpace(&this->coord_space).point_array_utov(2, point_t);

               GdipAddPathLine(path, (REAL)point_t[0].x, (REAL)point_t[0].y, (REAL)point_t[1].x, (REAL)point_t[1].y);
            }
#endif
         }
         else {
            point_t[0] = point;
            CGCoordSpace(&this->coord_space).point_utov(&point_t[0]);

            p_point[j].x = (int)point_t[0].x;
            p_point[j].y = (int)point_t[0].y;
         }

         j++;
         break;
      case EGPathInstruction.CurveToAbs:
      case EGPathInstruction.QuadToAbs:
         point_t[0] = point_last;
         point_t[1] = ip->point[0];
         point_t[2] = ip->point[ins_op == EGPathInstruction.QuadToAbs ? 0 : 1];
         point_t[3] = ip->point[ins_op == EGPathInstruction.QuadToAbs ? 1 : 2];
         goto curve;
      case EGPathInstruction.CurveToRel:
      case EGPathInstruction.QuadToRel:
         point_t[0] = point_last;
         point_t[1].x = point_last.x + ip->point[0].x;
         point_t[1].y = point_last.y + ip->point[0].y;
         point_t[2].x = point_last.x + ip->point[ins_op == EGPathInstruction.QuadToRel ? 0 : 1].x;
         point_t[2].y = point_last.y + ip->point[ins_op == EGPathInstruction.QuadToRel ? 0 : 1].y;
         point_t[3].x = point_last.x + ip->point[ins_op == EGPathInstruction.QuadToRel ? 1 : 2].x;
         point_t[3].y = point_last.y + ip->point[ins_op == EGPathInstruction.QuadToRel ? 1 : 2].y;
         goto curve;
      case EGPathInstruction.CurveToShortAbs:
      case EGPathInstruction.QuadToShortAbs:
         point_t[0] = point_last;
         point_t[1].x = point_last.x + (point_last.x - point_control_last.x);
         point_t[1].y = point_last.y + (point_last.y - point_control_last.y);
         point_t[2] = ip->point[0];
         point_t[3] = ip->point[ins_op == EGPathInstruction.QuadToShortAbs ? 0 : 1];
         goto curve;
      case EGPathInstruction.CurveToShortRel:
      case EGPathInstruction.QuadToShortRel:
         point_t[0] = point_last;
         point_t[1].x = point_last.x + (point_last.x - point_control_last.x);
         point_t[1].y = point_last.y + (point_last.y - point_control_last.y);
         point_t[2].x = point_last.x + ip->point[0].x;
         point_t[2].y = point_last.y + ip->point[0].y;
         point_t[3].x = point_last.x + ip->point[ins_op == EGPathInstruction.QuadToShortRel ? 0 : 1].x;
         point_t[3].y = point_last.y + ip->point[ins_op == EGPathInstruction.QuadToShortRel ? 0 : 1].y;
         goto curve;
      curve:
         switch (ins_op) {
         case EGPathInstruction.QuadToAbs:
         case EGPathInstruction.QuadToRel:
         case EGPathInstruction.QuadToShortAbs:
         case EGPathInstruction.QuadToShortRel:
            point_control_last = point_t[1];
            /* Convert quadratic to cubic */
            point_t[2] = point_t[1];
            point_t[1].x = (point_t[0].x + point_t[1].x * 2) / 3;
            point_t[1].y = (point_t[0].y + point_t[1].y * 2) / 3;
            point_t[2].x = (point_t[3].x + point_t[2].x * 2) / 3;
            point_t[2].y = (point_t[3].y + point_t[2].y * 2) / 3;
            break;
         default:
            point_control_last = point_t[2];
            break;
         }
         point = point_t[3];
         CGCoordSpace(&this->coord_space).point_array_utov(4, point_t);
         if (this->render_mode == EGCanvasRender.full) {
#if KEYSTONE_WIN32_GDIP
            GdipAddPathBezier(path, (REAL)point_t[0].x, (REAL)point_t[0].y, (REAL)point_t[1].x, (REAL)point_t[1].y,
                                    (REAL)point_t[2].x, (REAL)point_t[2].y, (REAL)point_t[3].x, (REAL)point_t[3].y);
#endif
         }
         break;
      case EGPathInstruction.ArcToAbs:
      case EGPathInstruction.ArcToRel:
         point_t[0] = point_last;
         if (ins_op == EGPathInstruction.ArcToRel) {
            point.x = point_last.x + ip->point[2].x;
            point.y = point_last.y + ip->point[2].y;
         }
         else {
            point = ip->point[2];
         }
         point_t[2] = point;

         CGCoordSpace(&this->coord_space).point_array_utov(3, point_t);
         point_t[1].x = ip->point[0].x * CGCoordSpace(&this->coord_space).scaling_x();
         point_t[1].y = ip->point[0].y * CGCoordSpace(&this->coord_space).scaling_y();
         rotation = ip->point[1].x + CGCoordSpace(&this->coord_space).rotation();

         CGCoordSpace(&this->coord_space).arc_position(point_t[0].x, point_t[0].y,
                                                       point_t[1].x, point_t[1].y,
                                                       rotation,
                                                       (((int)ip->point[1].y) & 1) ? 1 : 0, 
                                                       (((int)ip->point[1].y) & 2) ? 1 : 0,
                                                       point_t[2].x, point_t[2].y,
                                                       &cx, &cy, &rx, &ry, &th0, &thd);
#if KEYSTONE_WIN32_GDIP                                                          
         if (this->render_mode == EGCanvasRender.full) {
            GdipCreateMatrix(&arc_transform);
            GdipCreatePath(FillModeAlternate, &arc_path);
            GdipAddPathArc(arc_path,
                            -1.0, -1.0, 2.0, 2.0,
                            (REAL)th0, (REAL)thd);
            GdipTranslateMatrix(arc_transform, (REAL)cx, (REAL)cy, MatrixOrderPrepend);
            GdipRotateMatrix(arc_transform, (REAL)rotation, MatrixOrderPrepend);
            GdipScaleMatrix(arc_transform, (REAL)rx, (REAL)ry, MatrixOrderPrepend);
            GdipTransformPath(arc_path, arc_transform);
            GdipAddPathPath(path, arc_path, TRUE);
            GdipDeleteMatrix(arc_transform);
            break;
         }
#endif
         CGCanvas(this).NATIVE_draw_ellipse(colour, fill, cx, cy, rx, ry);
         break;
      default:
         break;
      }
      point_last = point;
   }
#if KEYSTONE_WIN32_GDIP
   if (this->render_mode == EGCanvasRender.full) {
      if (fill) {
         GdipFillPath(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipBrush, path);
      }
      else {
         GdipDrawPath(GCanvas_NATIVE_DATA(this)->gdipGraphics, GCanvas_NATIVE_DATA(this)->gdipPen, path);
      }

      GdipDeletePath(path);
      return;
   }
#endif
   if (closepath || fill) {
      Polygon(GCanvas_NATIVE_DATA(this)->hdc, p_point, j);
   }
   else {
      Polyline(GCanvas_NATIVE_DATA(this)->hdc, p_point, j);
   }

   CFramework(&framework).scratch_buffer_release(p_point);
}/*CGCanvas::NATIVE_draw_path*/

void CGCanvas::NATIVE_font_set(CGFontFamily *font_family, int font_size,
                               EGFontStyle font_style, EGFontWeight font_weight,
                               EGTextDecoration text_decoration) {
   LOGFONT lf;
   bool OEM = this->oem_font;

   CString(&this->font_family).set(CString(font_family).string());;
   this->text_decoration = text_decoration;

   CLEAR(&lf);
   if (!CString(font_family).length()) {
      DeleteObject(SelectObject(GCanvas_NATIVE_DATA(this)->hdc, GetStockObject(SYSTEM_FONT)));
      return;
   }
   /* translate a few common names */
   if (strcmp(CString(font_family).string(), "Times Roman") == 0) {
      strcpy(lf.lfFaceName, "Times New Roman");
   }
   else if (strcmp(CString(font_family).string(), "Lucida") == 0) {
      strcpy(lf.lfFaceName, "Lucida Console");
   }
   else {
     strcpy(lf.lfFaceName, CString(font_family).string());
   }

   lf.lfHeight = -font_size;
   switch (font_weight) {
   case EGFontWeight.normal:
      lf.lfWeight = FW_NORMAL;
      break;
   case EGFontWeight.bold:
      lf.lfWeight = FW_BOLD;
      break;
   case EGFontWeight.bolder:
      lf.lfWeight = FW_ULTRABOLD;
      break;
   case EGFontWeight.lighter:
      lf.lfWeight = FW_LIGHT;
      break;
   }
   lf.lfItalic = font_style == EGFontStyle.italic;
   lf.lfUnderline = text_decoration == EGTextDecoration.underline;
   if (OEM) {
      lf.lfCharSet = OEM_CHARSET;
   }

   GCanvas_NATIVE_DATA(this)->hFont = CreateFontIndirect(&lf);
   DeleteObject(SelectObject(GCanvas_NATIVE_DATA(this)->hdc, GCanvas_NATIVE_DATA(this)->hFont));
}/*CGCanvas::NATIVE_font_set*/

void CGCanvas::NATIVE_text_align_set(EGTextAnchor anchor, EGTextBaseline baseline) {
   UINT fMode;

   this->text_anchor = anchor;
   this->text_baseline = baseline;

   switch (baseline) {
   case EGTextBaseline.alphabetic:
   default:
      fMode = TA_BASELINE;
      break;
   case EGTextBaseline.text_before_edge:
      fMode = TA_TOP;
      break;
   case EGTextBaseline.text_after_edge:
   case EGTextBaseline.central:
   case EGTextBaseline.middle:
      fMode = TA_BOTTOM;
      break;
   }

   switch (anchor) {
   case EGTextAnchor.start:
   default:
      fMode |= TA_LEFT;
      break;
   case EGTextAnchor.middle:
      fMode |= TA_CENTER;
      break;
   case EGTextAnchor.end:
      fMode |= TA_RIGHT;
      break;
   }
   SetTextAlign(GCanvas_NATIVE_DATA(this)->hdc, fMode);
}/*CGCanvas::NATIVE_text_align_set*/

static inline int text_newline_length(const char *text) {
   const char *cp_end;

   cp_end = strchr(text, 0x0D);
   if (!cp_end) {
      cp_end = strchr(text, 0x0A);
   }
   if (cp_end) {
      return cp_end - text;
   }
   else {
      return strlen(text);
   }
};

void CGCanvas::NATIVE_text_extent_get(TCoord x, TCoord y, const char *text, TRect *result) {
   SIZE size;
   TEXTMETRIC tm;
   TCoord descent;

   GetTextExtentPoint32(GCanvas_NATIVE_DATA(this)->hdc, text, text_newline_length(text), &size);
   GetTextMetrics(GCanvas_NATIVE_DATA(this)->hdc, &tm);

   result->point[1].x = (TCoord)size.cx / CGCoordSpace(&this->coord_space).scaling_x();
   result->point[1].y = (TCoord)size.cy / CGCoordSpace(&this->coord_space).scaling_y();
   descent = (TCoord)tm.tmDescent / CGCoordSpace(&this->coord_space).scaling_y();

   switch (this->text_anchor) {
   case EGTextAnchor.start:
   default:
      break;
   case EGTextAnchor.middle:
      x -= result->point[1].x / 2;
      break;
   case EGTextAnchor.end:
      x -= result->point[1].x;
      break;
   }

   if (CString(&this->font_family).length()) {
      switch (this->text_baseline) {
      case EGTextBaseline.text_before_edge:
         break;
      case EGTextBaseline.text_after_edge:
      default:
         y -= result->point[1].y;
         break;
      case EGTextBaseline.central:
      case EGTextBaseline.middle:
         y -= result->point[1].y / 2;
         break;
      }
   }

  result->point[0].x = x;
  result->point[0].y = y + descent;
  result->point[1].x += x;
  result->point[1].y += y + descent;
}/*CGCanvas::NATIVE_text_extent_get*/

void CGCanvas::NATIVE_text_extent_get_string(TCoord x, TCoord y, CString *text, TRect *result) {
   CString tempstr;

   new(&tempstr).CString(NULL);
   CString(&tempstr).set_string(text);
   CGCanvas(this).NATIVE_text_extent_get(x, y, CString(&tempstr).string(), result);
   delete(&tempstr);
}/*CGCanvas::NATIVE_text_extent_get_string*/

void CGCanvas::NATIVE_draw_text(TGColour colour, TCoord x, TCoord y, const char *text) {
   SIZE size;
//   colour = CGPalette(&this->palette).colour_rgb(colour);
   colour = GCOL_CONVERT_RGB(this->palette, this->paletteB, colour);

   if (!CString(&this->font_family).length()) {
      SetTextAlign(GCanvas_NATIVE_DATA(this)->hdc, TA_TOP);
   }

   SetTextColor(GCanvas_NATIVE_DATA(this)->hdc,
                RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));

   if (this->text_baseline == EGTextBaseline.central ||
       this->text_baseline == EGTextBaseline.middle) {
      GetTextExtentPoint32(GCanvas_NATIVE_DATA(this)->hdc, text, text_newline_length(text), &size);
      y += size.cy >> 1;
   }

   TextOut(GCanvas_NATIVE_DATA(this)->hdc, (int)x, (int)y, text, text_newline_length(text));
}/*CGCanvas::NATIVE_draw_text*/

void CGCanvas::NATIVE_draw_text_string(TGColour colour, TCoord x, TCoord y, CString *text) {
   SIZE size;
   CString tempstr;
//   colour = CGPalette(&this->palette).colour_rgb(colour);
   colour = GCOL_CONVERT_RGB(this->palette, this->paletteB, colour);

   new(&tempstr).CString(NULL);
   CString(&tempstr).set_string(text);

   if (!CString(&this->font_family).length()) {
      SetTextAlign(GCanvas_NATIVE_DATA(this)->hdc, TA_TOP);
   }

   SetTextColor(GCanvas_NATIVE_DATA(this)->hdc,
                RGB(GCOL_RGB_RED(colour), GCOL_RGB_GREEN(colour), GCOL_RGB_BLUE(colour)));

   if (this->text_baseline == EGTextBaseline.central ||
       this->text_baseline == EGTextBaseline.middle) {
      GetTextExtentPoint32W(GCanvas_NATIVE_DATA(this)->hdc, CString(text).wstring(), text_newline_length(CString(&tempstr).string()), &size);
      y += size.cy >> 1;
   }

   if (this->oem_font) {
      TextOut(GCanvas_NATIVE_DATA(this)->hdc, (int)x, (int)y, CString(text).string(), text_newline_length(CString(&tempstr).string()));
   }
   else {
      TextOutW(GCanvas_NATIVE_DATA(this)->hdc, (int)x, (int)y, CString(text).wstring(), text_newline_length(CString(&tempstr).string()));
   }

   delete(&tempstr);
}/*CGCanvas::NATIVE_draw_text_string*/

void CGCanvas::NATIVE_draw_bitmap(CGBitmap *bitmap, TCoord x, TCoord y) {
   HDC memDC;

#if KEYSTONE_WIN32_GDIP
   if (bitmap->image && this->render_mode == EGCanvasRender.full) {
      GdipDrawImage(GCanvas_NATIVE_DATA(this)->gdipGraphics, (GpBitmap *)bitmap->native_object, (REAL)x, (REAL)y);
      return;
   }
#endif
   memDC = CreateCompatibleDC(NULL);

   SelectObject(memDC, (HBITMAP)bitmap->native_object);
   BitBlt(GCanvas_NATIVE_DATA(this)->hdc, (int)x, (int)y, bitmap->width, bitmap->height,
          memDC, 0, 0, SRCCOPY);
   DeleteDC(memDC);
}/*CGCanvas::NATIVE_draw_bitmap*/

void CGCanvas::NATIVE_draw_bitmap_scaled(CGBitmap *bitmap, TCoord x, TCoord y, TCoord width, TCoord height) {
   HDC memDC;

#if KEYSTONE_WIN32_GDIP
   if (bitmap->image && this->render_mode == EGCanvasRender.full) {
      GdipDrawImageRect(GCanvas_NATIVE_DATA(this)->gdipGraphics, (GpBitmap *)bitmap->native_object, (REAL)x, (REAL)y, (REAL)width, (REAL)height);
      return;
   }
#endif

   memDC = CreateCompatibleDC(NULL);

   SelectObject(memDC, (HBITMAP)bitmap->native_object);
   StretchBlt(GCanvas_NATIVE_DATA(this)->hdc, (int)x, (int)y, (int)width, (int)height,
              memDC, 0, 0, bitmap->width, bitmap->height, SRCCOPY);
   DeleteDC(memDC);
}/*CGCanvas::NATIVE_draw_bitmap_scaled*/

void CGCanvasBitmap::NATIVE_allocate(CGLayout *layout) {
//   GCanvas_NATIVE_DATA(this)->hdc = CreateCompatibleDC(NULL);
//   SetBkMode(GCanvas_NATIVE_DATA(this)->hdc, TRANSPARENT);
//   SelectObject(GCanvas_NATIVE_DATA(this)->hdc, (HBITMAP)this->bitmap.native_object);
}/*CGCanvasBitmap::NATIVE_allocate*/

void CGCanvasBitmap::NATIVE_release(CGLayout *layout) {
   class:base.NATIVE_release(layout);
}/*CGCanvasBitmap::NATIVE_release*/

bool CGCanvasPrint::NATIVE_output_clipboard(void) {
   CString(&this->outfile).set(NULL);
   this->output_mode = EGCanvasPrintOutput.clipboard;

   return TRUE;
}/*CGCanvasPrint::NATIVE_output_clipboard*/

bool CGCanvasPrint::NATIVE_output_file(const char *filename) {
   CString(&this->outfile).set(filename);
   this->output_mode = EGCanvasPrintOutput.file;

   return TRUE;
}/*CGCanvasPrint::NATIVE_output_file*/

bool CGCanvasPrint::NATIVE_output_choose_printer(void) {
   /* return the printer name and output file name specified by the user */
   PRINTDLG pd;
   DEVNAMES *dnames;

   this->output_mode = EGCanvasPrintOutput.printer;

   memset(&pd, 0, sizeof(PRINTDLG));
   pd.lStructSize = sizeof(PRINTDLG);
   pd.hwndOwner = this->parent_window ? (HWND) CGObject(this->parent_window)->native_object : NULL;
   pd.Flags = PD_ALLPAGES | PD_COLLATE | PD_NOSELECTION;
   pd.nCopies = 1;

   if (!PrintDlg(&pd)) {
      return FALSE;                     /* Canceled */
   }

   dnames = GlobalLock(pd.hDevNames);
   CString(&this->printer).set((char *)dnames + dnames->wDeviceOffset);
   CString(&this->outfile).set((char *)dnames + dnames->wOutputOffset);
   GlobalUnlock(pd.hDevNames);

   GCanvas_NATIVE_DATA(this)->hDevMode = pd.hDevMode;

   return TRUE;
}/*CGCanvasPrint::NATIVE_output_choose_printer*/

typedef BOOL (WINAPI *PGDP)(LPTSTR, LPDWORD);

void CGCanvasPrint::NATIVE_allocate(CGLayout *layout) {
// int devmode_size;
   static DOCINFO di = {sizeof(DOCINFO), TEXT("SVG Print Job")};
   DEVMODE *devmode;
   RECT rect;
   int width, height;
   char pname[256];
   DWORD psize;
   PGDP pGetDefaultPrinter;
   HMODULE hDLL;

   switch(this->output_mode) {
   case EGCanvasPrintOutput.printer:
      if (CString(&this->printer).length()) {
         if (!OpenPrinter(CString(&this->printer).string(), &GCanvas_NATIVE_DATA(this)->hGDIPrinter, NULL)) {
            return;
         }
         devmode = GlobalLock(GCanvas_NATIVE_DATA(this)->hDevMode);
         strcpy(pname, CString(&this->printer).string());
      }
      else {
         psize = sizeof(pname);
#if 0
         ASSERT("CGCanvasPrint::Default Printer Selection Not Implemented!");
#else 
         hDLL = LoadLibrary(TEXT("winspool.drv"));   
         pGetDefaultPrinter = (PGDP) GetProcAddress(hDLL,"GetDefaultPrinterA");

         if (pGetDefaultPrinter != NULL) {
            pGetDefaultPrinter(pname, &psize);
         }
#endif         
         devmode = CFramework(&framework).scratch_buffer_allocate();
         if (!OpenPrinter(pname, &GCanvas_NATIVE_DATA(this)->hGDIPrinter, NULL)) {
            return;
         }
         DocumentProperties(NULL, GCanvas_NATIVE_DATA(this)->hGDIPrinter, pname, devmode, NULL, DM_OUT_BUFFER);
      }

      /* Set print orientation >>>not ideal, should do this when choosing printer */
      switch (this->orientation) {
      case EGCanvasPrintOrientation.none:
         break;
      case EGCanvasPrintOrientation.portrait:
         devmode->dmOrientation = DMORIENT_PORTRAIT;
         break;
      case EGCanvasPrintOrientation.landscape:
         devmode->dmOrientation = DMORIENT_LANDSCAPE;
         break;
      }
      GCanvas_NATIVE_DATA(this)->hdc = CreateDC(NULL, pname, NULL, devmode);

      if (CString(&this->printer).length()) {
         GlobalUnlock(GCanvas_NATIVE_DATA(this)->hDevMode);
      }
      else {
         CFramework(&framework).scratch_buffer_release(devmode);
      }

      if (!GCanvas_NATIVE_DATA(this)->hdc) {
         return;
      }

      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>, FALSE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGXULElement,width>, GetDeviceCaps(GCanvas_NATIVE_DATA(this)->hdc, HORZRES));
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGXULElement,height>, GetDeviceCaps(GCanvas_NATIVE_DATA(this)->hdc, VERTRES));

      if (StartDoc(GCanvas_NATIVE_DATA(this)->hdc, &di) <= 0) {
         return;
      }
      if (StartPage(GCanvas_NATIVE_DATA(this)->hdc) <= 0) {
         return;
      }
      break;
   case EGCanvasPrintOutput.clipboard:
       CGCanvas(this->src_canvas).NATIVE_enter(TRUE);

       width = GetDeviceCaps(GCanvas_NATIVE_DATA(this->src_canvas)->hdc, HORZRES);
       height = GetDeviceCaps(GCanvas_NATIVE_DATA(this->src_canvas)->hdc, VERTRES);

      rect.left   = (int)CGCanvas(this)->viewBox.point[0].x;
      rect.top    = (int)CGCanvas(this)->viewBox.point[0].y;
      rect.right  = (int)((CGCanvas(this)->viewBox.point[1].x * 32) / width * CGCanvas(this)->viewBox.point[1].x);
      rect.bottom = (int)(CGCanvas(this)->viewBox.point[1].y * 32);

      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>, FALSE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGXULElement,width>, (int)(CGCanvas(this)->viewBox.point[1].x - CGCanvas(this)->viewBox.point[0].x));
      CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGXULElement,height>, (int)(CGCanvas(this)->viewBox.point[1].y - CGCanvas(this)->viewBox.point[0].y));

      GCanvas_NATIVE_DATA(this)->hdc = CreateEnhMetaFile(GCanvas_NATIVE_DATA(this->src_canvas)->hdc, NULL, NULL/*&rect*/, "SVG Export\0Picture\0");
      break;
   case EGCanvasPrintOutput.file:
      return;
   }
   CGCanvas(this).NATIVE_enter(TRUE);
}/*CGCanvasPrint::NATIVE_allocate*/

void CGCanvasPrint::NATIVE_release(CGLayout *layout) {
   HENHMETAFILE hEnhMetaFile;

   CGCanvas(this).NATIVE_enter(FALSE);

   switch (this->output_mode) {
   case EGCanvasPrintOutput.printer:
      EndPage(GCanvas_NATIVE_DATA(this)->hdc);
      EndDoc(GCanvas_NATIVE_DATA(this)->hdc);
      DeleteDC(GCanvas_NATIVE_DATA(this)->hdc);
      break;
   case EGCanvasPrintOutput.clipboard:
      hEnhMetaFile = CloseEnhMetaFile(GCanvas_NATIVE_DATA(this)->hdc);
      OpenClipboard(this->parent_window ? (HWND) CGObject(this->parent_window)->native_object : NULL);
      EmptyClipboard();
      SetClipboardData(CF_ENHMETAFILE, hEnhMetaFile);
      CloseClipboard();

      CGCanvas(this->src_canvas).NATIVE_enter(FALSE);
      break;
   case EGCanvasPrintOutput.file:
      return;
   }

//free(GCanvas_NATIVE_DATA(this)->devmode);
//GCanvas_NATIVE_DATA(this)->devmode = NULL;
}/*CGCanvasPrint::NATIVE_release*/

/*==========================================================================*/
MODULE::END;/*==============================================================*/
/*==========================================================================*/
