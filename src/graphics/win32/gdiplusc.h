/* Alternative header file for a subset of  GDIPLUS, straight C only */

#ifndef _GDIPLUSC_H
#define _GDIPLUSC_H

#define ULONG_PTR ULONG

#define WINGDIPAPI __stdcall
#define GDIPCONST const

#define    PixelFormatIndexed      0x00010000 // Indexes into a palette
#define    PixelFormatGDI          0x00020000 // Is a GDI-supported format
#define    PixelFormatAlpha        0x00040000 // Has an alpha component
#define    PixelFormatPAlpha       0x00080000 // Pre-multiplied alpha
#define    PixelFormatExtended     0x00100000 // Extended color 16 bits/channel
#define    PixelFormatCanonical    0x00200000 

#define    PixelFormatUndefined       0
#define    PixelFormatDontCare        0

#define    PixelFormat1bppIndexed     (1 | ( 1 << 8) | PixelFormatIndexed | PixelFormatGDI)
#define    PixelFormat4bppIndexed     (2 | ( 4 << 8) | PixelFormatIndexed | PixelFormatGDI)
#define    PixelFormat8bppIndexed     (3 | ( 8 << 8) | PixelFormatIndexed | PixelFormatGDI)
#define    PixelFormat16bppGrayScale  (4 | (16 << 8) | PixelFormatExtended)
#define    PixelFormat16bppRGB555     (5 | (16 << 8) | PixelFormatGDI)
#define    PixelFormat16bppRGB565     (6 | (16 << 8) | PixelFormatGDI)
#define    PixelFormat16bppARGB1555   (7 | (16 << 8) | PixelFormatAlpha | PixelFormatGDI)
#define    PixelFormat24bppRGB        (8 | (24 << 8) | PixelFormatGDI)
#define    PixelFormat32bppRGB        (9 | (32 << 8) | PixelFormatGDI)
#define    PixelFormat32bppARGB       (10 | (32 << 8) | PixelFormatAlpha | PixelFormatGDI | PixelFormatCanonical)
#define    PixelFormat32bppPARGB      (11 | (32 << 8) | PixelFormatAlpha | PixelFormatPAlpha | PixelFormatGDI)
#define    PixelFormat48bppRGB        (12 | (48 << 8) | PixelFormatExtended)
#define    PixelFormat64bppARGB       (13 | (64 << 8) | PixelFormatAlpha  | PixelFormatCanonical | PixelFormatExtended)
#define    PixelFormat64bppPARGB      (14 | (64 << 8) | PixelFormatAlpha  | PixelFormatPAlpha | PixelFormatExtended)
#define    PixelFormatMax             15


/* types */
typedef long GPENUM;
typedef long GPOBJECT;
typedef float REAL;
typedef RECT RectF;
typedef RECT Rect;

typedef struct {
    REAL X, Y;
} Point;

/* Startup/Shutdown */
typedef struct 
{
    UINT32 GdiplusVersion;             // Must be 1
    void *DebugEventCallback; // Ignored on free builds
    BOOL SuppressBackgroundThread;     // FALSE unless you're prepared to call 
                                       // the hook/unhook functions properly
    BOOL SuppressExternalCodecs;       // FALSE unless you want GDI+ only to use
}                                       // its internal image codecs.
GdiplusStartupInput;

typedef struct 
{
    // The following 2 fields are NULL if SuppressBackgroundThread is FALSE.
    // Otherwise, they are functions which must be called appropriately to
    // replace the background thread.
    //
    // These should be called on the application's main message loop - i.e.
    // a message loop which is active for the lifetime of GDI+.
    // "NotificationHook" should be called before starting the loop,
    // and "NotificationUnhook" should be called after the loop ends.
    
    void *NotificationHook;
    void *NotificationUnhook;
} GdiplusStartupOutput;

void* WINAPI GdiplusStartup(ULONG_PTR *token, const GdiplusStartupInput *input, GdiplusStartupOutput *output);
void WINAPI GdiplusShutdown(ULONG_PTR token);

/* Extras */
#define Color(a,r,g,b) (ARGB)((a << 24) | (r << 16) | (g << 8) | b)

//---------------------------------------------------------------------------
// Enums
//---------------------------------------------------------------------------

typedef GPENUM WarpMode;
typedef unsigned long ARGB;
typedef GPENUM LinearGradientMode;
typedef GPENUM CustomLineCapType;
typedef GPENUM EncoderParameters;
typedef GPENUM ImageType;
typedef GPENUM PixelFormat;
typedef GPENUM GetThumbnailImageAbort;
typedef GPENUM RotateFlipType;
typedef GPENUM ColorPalette;
typedef GPENUM PropertyItem;
typedef GPENUM IDirectDrawSurface7;
//typedef GPENUM BitmapData;
typedef struct {
    UINT Width;
    UINT Height;
    INT Stride;
    PixelFormat PixelFormat;
    VOID* Scan0;
    UINT_PTR Reserved;
} BitmapData;

typedef GPENUM ColorAdjustType;
typedef GPENUM ColorMatrix;
typedef GPENUM ColorMatrixFlags;
typedef GPENUM ColorChannelFlags;
typedef GPENUM ColorMap;
typedef GPENUM WrapMode;
typedef GPENUM CompositingMode;
typedef GPENUM CompositingQuality;
typedef enum {
    SmoothingModeInvalid     = -1,
    SmoothingModeDefault     = 0,
    SmoothingModeHighSpeed   = 1,
    SmoothingModeHighQuality = 2,
    SmoothingModeNone = 3,
    SmoothingModeAntiAlias = 4
} SmoothingMode;
typedef enum {
    LineCapFlat             = 0,
    LineCapSquare           = 1,
    LineCapRound            = 2,
    LineCapTriangle         = 3,
    LineCapNoAnchor = 0x10,
    LineCapSquareAnchor = 0x11,
    LineCapRoundAnchor = 0x12,
    LineCapDiamondAnchor = 0x13,
    LineCapArrowAnchor = 0x14,
    LineCapCustom = 0xff
} LineCap;
typedef enum {
    LineJoinMiter        = 0,
    LineJoinBevel        = 1,
    LineJoinRound        = 2,
    LineJoinMiterClipped = 3
} LineJoin;
typedef enum {
    FillModeAlternate,        // 0
    FillModeWinding           // 1
} FillMode;
typedef enum {
    MatrixOrderPrepend    = 0,
    MatrixOrderAppend     = 1
} MatrixOrder;

typedef enum {
    CombineModeReplace,     // 0
    CombineModeIntersect,   // 1
    CombineModeUnion,       // 2
    CombineModeXor,         // 3
    CombineModeExclude,     // 4
    CombineModeComplement   // 5 (Exclude From)
} CombineMode;

typedef enum {
    UnitWorld,      // 0 -- World coordinate (non-physical unit)
    UnitDisplay,    // 1 -- Variable -- for PageTransform only
    UnitPixel,      // 2 -- Each unit is one device pixel.
    UnitPoint,      // 3 -- Each unit is a printer's point, or 1/72 inch.
    UnitInch,       // 4 -- Each unit is 1 inch.
    UnitDocument,   // 5 -- Each unit is 1/300 inch.
    UnitMillimeter  // 6 -- Each unit is 1 millimeter.
} Unit;

typedef Unit GpUnit;
typedef GPENUM PixelOffsetMode;
typedef GPENUM TextRenderingHint;
typedef GPENUM InterpolationMode;
typedef GPENUM DrawImageAbort;
typedef GPENUM GraphicsState;
typedef GPENUM GraphicsContainer;
typedef GPENUM WmfPlaceableFileHeader;
typedef GPENUM MetafileHeader;
typedef GPENUM EmfType;
typedef GPENUM MetafileFrameUnit;
typedef GPENUM ImageCodecInfo;
//typedef short UINT16;
//typedef GPENUM Unit;
typedef Point PointF;
typedef GPENUM StringAlignment;
typedef GPENUM StringTrimming;
typedef GPENUM StringDigitSubstitute;
typedef GPENUM CharacterRange;

//---------------------------------------------------------------------------
// classes
//---------------------------------------------------------------------------
typedef GPOBJECT GpGraphics;

typedef GPOBJECT GpBrush;
typedef GPOBJECT GpTexture;
typedef GPOBJECT GpSolidFill;
typedef GPOBJECT GpLineGradient;
typedef GPOBJECT GpPathGradient;
typedef GPOBJECT GpHatch;

typedef GPOBJECT GpPen;
typedef GPOBJECT GpCustomLineCap;
typedef GPOBJECT GpAdjustableArrowCap;

typedef GPOBJECT GpImage;
typedef GPOBJECT GpBitmap;
typedef GPOBJECT GpMetafile;
typedef GPOBJECT GpImageAttributes;

typedef GPOBJECT GpPath;
typedef GPOBJECT GpRegion;
typedef GPOBJECT GpPathIterator;

typedef GPOBJECT GpFontFamily;
typedef GPOBJECT GpFont;
typedef GPOBJECT GpStringFormat;
typedef GPOBJECT GpFontCollection;
typedef GPOBJECT GpInstalledFontCollection;
typedef GPOBJECT GpPrivateFontCollection;

typedef GPOBJECT GpCachedBitmap;

typedef void *GpStatus;
typedef FillMode GpFillMode;
typedef GPOBJECT GpWrapMode;
typedef GPOBJECT GpCoordinateSpace;
typedef Point GpPointF;
typedef GPOBJECT GpPoint;
typedef GPOBJECT GpRectF;
//typedef GPOBJECT GpRect;
typedef struct {
    INT X;
    INT Y;
    INT Width;
    INT Height;
} GpRect;
typedef GPOBJECT GpSizeF;
typedef GPOBJECT GpHatchStyle;
typedef GPOBJECT GpDashStyle;
typedef LineCap GpLineCap;
typedef GPOBJECT GpDashCap;

typedef GPOBJECT GpPenAlignment;

typedef LineJoin GpLineJoin;
typedef GPOBJECT GpPenType;

typedef GPOBJECT GpMatrix;
typedef GPOBJECT GpBrushType;
typedef MatrixOrder GpMatrixOrder;
typedef GPOBJECT GpFlushIntention;
typedef GPOBJECT GpPathData;

//---------------------------------------------------------------------------
// methods (subset)
//---------------------------------------------------------------------------

GpStatus WINGDIPAPI
GdipCreateFromHDC(HDC hdc, GpGraphics **graphics);

GpStatus WINGDIPAPI
GdipDeleteGraphics(GpGraphics *graphics);

GpStatus WINGDIPAPI 
GdipGetPageUnit(GpGraphics *graphics, GpUnit *unit);

GpStatus WINGDIPAPI 
GdipSetPageUnit(GpGraphics *graphics, GpUnit unit);

GpStatus WINGDIPAPI 
GdipSetPageScale(GpGraphics *graphics, REAL scale);

GpStatus WINGDIPAPI
GdipCreateMatrix(GpMatrix **matrix);

GpStatus WINGDIPAPI
GdipDeleteMatrix(GpMatrix *matrix);

GpStatus WINGDIPAPI
GdipTranslateMatrix(GpMatrix *matrix, REAL offsetX, REAL offsetY,
                    GpMatrixOrder order);

GpStatus WINGDIPAPI
GdipScaleMatrix(GpMatrix *matrix, REAL scaleX, REAL scaleY,
                GpMatrixOrder order);

GpStatus WINGDIPAPI
GdipRotateMatrix(GpMatrix *matrix, REAL angle, GpMatrixOrder order);

GpStatus WINGDIPAPI
GdipShearMatrix(GpMatrix *matrix, REAL shearX, REAL shearY,
                GpMatrixOrder order);

GpStatus WINGDIPAPI
GdipInvertMatrix(GpMatrix *matrix);

GpStatus WINGDIPAPI
GdipCreatePen1(ARGB color, REAL width, GpUnit unit, GpPen **pen);

GpStatus WINGDIPAPI
GdipSetPenColor(GpPen *pen, ARGB argb);

GpStatus WINGDIPAPI
GdipSetPenWidth(GpPen *pen, REAL width);

GpStatus WINGDIPAPI
GdipSetPenStartCap(GpPen *pen, GpLineCap startCap);

GpStatus WINGDIPAPI
GdipSetPenEndCap(GpPen *pen, GpLineCap endCap);

GpStatus WINGDIPAPI
GdipSetPenLineJoin(GpPen *pen, GpLineJoin lineJoin);

GpStatus WINGDIPAPI
GdipSetPenMiterLimit(GpPen *pen, REAL miterLimit);

GpStatus WINGDIPAPI
GdipSetPenDashStyle(GpPen *pen, GpDashStyle dashstyle);

GpStatus WINGDIPAPI
GdipSetPenDashOffset(GpPen *pen, REAL offset);

GpStatus WINGDIPAPI
GdipSetPenDashArray(GpPen *pen, GDIPCONST REAL *dash, INT count);

GpStatus WINGDIPAPI
GdipDeletePen(GpPen *pen);

GpStatus WINGDIPAPI
GdipCreateSolidFill(ARGB color, GpSolidFill **brush);

GpStatus WINGDIPAPI
GdipSetSolidFillColor(GpSolidFill *brush, ARGB color);

GpStatus WINGDIPAPI
GdipSetClipRect(GpGraphics *graphics, REAL x, REAL y,
                         REAL width, REAL height, CombineMode combineMode);

GpStatus WINGDIPAPI
GdipResetClip(GpGraphics *graphics);

GpStatus WINGDIPAPI
GdipIsClipEmpty(GpGraphics *graphics, BOOL *result);

GpStatus WINGDIPAPI
GdipDrawLine(GpGraphics *graphics, GpPen *pen, REAL x1, REAL y1,
                      REAL x2, REAL y2);
                      
GpStatus WINGDIPAPI
GdipDrawEllipse(GpGraphics *graphics, GpPen *pen, REAL x, REAL y,
                         REAL width, REAL height);
                         
GpStatus WINGDIPAPI
GdipFillEllipse(GpGraphics *graphics, GpBrush *brush, REAL x, REAL y,
                REAL width, REAL height);
                
GpStatus WINGDIPAPI
GdipFillRectangle(GpGraphics *graphics, GpBrush *brush, REAL x, REAL y,
                  REAL width, REAL height);
                
GpStatus WINGDIPAPI
GdipDrawPolygon(GpGraphics *graphics, GpPen *pen, GDIPCONST GpPointF *points,
                         INT count);
                         
GpStatus WINGDIPAPI
GdipFillPolygon2(GpGraphics *graphics, GpBrush *brush,
                 GDIPCONST GpPointF *points, INT count);
                 
GpStatus WINGDIPAPI
GdipDrawPath(GpGraphics *graphics, GpPen *pen, GpPath *path);

GpStatus WINGDIPAPI
GdipFillPath(GpGraphics *graphics, GpBrush *brush, GpPath *path);

GpStatus WINGDIPAPI
GdipCreatePath(GpFillMode brushMode, GpPath **path);

GpStatus WINGDIPAPI
GdipAddPathLine(GpPath *path, REAL x1, REAL y1, REAL x2, REAL y2);

GpStatus WINGDIPAPI
GdipAddPathLine2(GpPath *path, GDIPCONST GpPointF *points, INT count);

GpStatus WINGDIPAPI
GdipAddPathBezier(GpPath *path, REAL x1, REAL y1, REAL x2, REAL y2,
                           REAL x3, REAL y3, REAL x4, REAL y4);

GpStatus WINGDIPAPI
GdipAddPathArc(GpPath *path, REAL x, REAL y, REAL width, REAL height,
                        REAL startAngle, REAL sweepAngle);

GpStatus WINGDIPAPI
GdipAddPathPath(GpPath *path, GDIPCONST GpPath* addingPath, BOOL connect);

GpStatus WINGDIPAPI
GdipAddPathRectangle(GpPath *path, REAL x, REAL y, REAL width, REAL height);

GpStatus WINGDIPAPI
GdipTransformPath(GpPath* path, GpMatrix* matrix);

GpStatus WINGDIPAPI
GdipStartPathFigure(GpPath *path);

GpStatus WINGDIPAPI
GdipClosePathFigure(GpPath *path);

GpStatus WINGDIPAPI
GdipClosePathFigures(GpPath *path);

GpStatus WINGDIPAPI
GdipDeletePath(GpPath* path);

                      
GpStatus WINGDIPAPI
GdipCreateLineBrush(GDIPCONST GpPointF* point1,
                    GDIPCONST GpPointF* point2,
                    ARGB color1, ARGB color2,
                    GpWrapMode wrapMode,
                    GpLineGradient **lineGradient);

GpStatus WINGDIPAPI
GdipDeleteBrush(GpBrush *brush);

GpStatus WINGDIPAPI
GdipCreateBitmapFromFile(GDIPCONST WCHAR* filename, GpBitmap **bitmap);

GpStatus WINGDIPAPI
GdipCloneBitmapAreaI(INT x,
                     INT y,
                     INT width,
                     INT height,
                     PixelFormat format,
                     GpBitmap *srcBitmap,
                     GpBitmap **dstBitmap);

GpStatus WINGDIPAPI
GdipBitmapLockBits(GpBitmap* bitmap,
                   GDIPCONST GpRect* rect,
                   UINT flags,
                   PixelFormat format,
                   BitmapData* lockedBitmapData);

GpStatus WINGDIPAPI
GdipBitmapUnlockBits(GpBitmap* bitmap,
                     BitmapData* lockedBitmapData);

GpStatus WINGDIPAPI
GdipCreateBitmapFromScan0(INT width,
                          INT height,
                          INT stride,
                          PixelFormat format,
                          BYTE* scan0,
                          GpBitmap** bitmap);

GpStatus WINGDIPAPI
GdipDrawImage(GpGraphics *graphics, GpImage *image, REAL x, REAL y);

GpStatus WINGDIPAPI
GdipDrawImageRect(GpGraphics *graphics, GpImage *image, REAL x, REAL y,
                           REAL width, REAL height);
                          
GpStatus WINGDIPAPI
GdipGetImageWidth(GpImage *image, UINT *width);

GpStatus WINGDIPAPI
GdipGetImageHeight(GpImage *image, UINT *height);

GpStatus WINGDIPAPI
GdipDisposeImage(GpImage *image);
               
GpStatus WINGDIPAPI
GdipSetSmoothingMode(GpGraphics *graphics, SmoothingMode smoothingMode);

#endif  // !_GDIPLUSC.H