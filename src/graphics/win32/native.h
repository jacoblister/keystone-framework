/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

#include "../../win32/native.h"

#include <windows.h>
#if KEYSTONE_WIN32_GDIP
#include "gdiplusc.h"
#endif

typedef struct {
   HWND hwnd;
   HDC hdc;
   HCURSOR hcursor;
   HBRUSH hBrush;
   HPEN hPen;
   HFONT hFont;
#if KEYSTONE_WIN32_GDIP
    /* GDI+ */
   GpGraphics *gdipGraphics;
   GpBrush *gdipBrush;    
   GpBrush *gdipBrushSolid;
   GpBrush *gdipBrushLinGr;
   GpBrush *gdipBrushRadGr;    
   GpPen *gdipPen;
   GpFont *gdipFont;
#endif    
   /* GDI Printing */
   HANDLE hGDIPrinter;
   HGLOBAL hDevMode;
} TGCanvasNative;

LRESULT CALLBACK CGWindow_WIN32_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
bool CFramework__Win32_translate_key_event(MSG *msg, CEventKey *result);

//#define GCanvas_NATIVE_DATA(canvas) ((TGCanvasNative *)(&CGCanvas(canvas)->native_data))
#define GCanvas_NATIVE_DATA(canvas) ((TGCanvasNative *)(&((CGCanvas *)canvas)->native_data))
