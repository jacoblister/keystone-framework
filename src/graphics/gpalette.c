/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "../framework_base.c"
#include "../objpersistent.c"
#include "../cstring.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

ENUM:typedef<EGColourNamed> {
   {aliceblue}, {antiquewhite}, {aqua}, {aquamarine}, {azure}, {beige}, {bisque}, {black},
   {blanchedalmond}, {blue}, {blueviolet}, {brown}, {burlywood}, {cadetblue}, {chartresue}, {chocolate},
   {coral}, {cornflowerblue}, {cornsilk}, {crimson}, {cyan}, {darkblue}, {darkcyan}, {darkgoldenrod},
   {darkgray}, {darkgreen}, {darkgrey}, {darkkhaki}, {darkmagenta}, {darkolivegreen}, {darkorange}, {darkorchid},
   {darkred}, {darksalmon}, {darkseagreen}, {darksaltblue}, {darkslategray}, {darkslategrey}, {darkturquoise}, {darkviolet},
   {deeppink}, {deepskyblue}, {dimgray}, {dimgrey}, {dodgerblue}, {firebrick}, {floralwhite}, {forestgreen},
   {fuchsia}, {gainsboro}, {ghostwhite}, {gold}, {goldenrod}, {gray}, {grey}, {green},
   {greenyellow}, {honeydew}, {hotpink}, {indianred}, {indigo}, {ivory}, {khaki}, {lavender},
   {lavenderblush}, {lawngreen}, {lemonchiffon}, {lightblue}, {lightcoral}, {lightcyan}, {lightgoldenrodyellow}, {lightgray},
   {lightgreen}, {lightgrey},
   {lightpink}, {lightsalmon}, {lightseagreen}, {lightskyblue}, {lightslategray}, {lightslategrey}, {lightsteelblue}, {lightyellow},
   {lime}, {limegreen}, {linen}, {magenta}, {maroon}, {mediumaquamarine}, {mediumblue}, {mediumorchid},
   {mediumpurple}, {mediumseagreen}, {mediumslateblue}, {mediumspringgreen}, {mediumturquoise}, {mediumvioletred}, {midnightblue}, {mintcream},
   {mistyrose}, {moccasin}, {navyjowhite}, {navy}, {oldlace}, {olive}, {olivedrab}, {orange},
   {orangered}, {orchid}, {palegoldenrod}, {palegreen}, {paleturquoise}, {palevioletred}, {papayawhip}, {peachpuff},
   {peru}, {pink}, {plum}, {powderblue}, {purple}, {red}, {rosybrown}, {royalblue},
   {saddlebrown}, {salmon}, {sandybrown}, {seagreen}, {seashell}, {sienna}, {silver}, {skyblue},
   {slateblue}, {slategray}, {slategrey}, {snow}, {springgreen}, {steelblue}, {tan}, {teal},
   {thistle}, {tomato}, {turquoise}, {violet}, {wheat}, {white}, {whitesmoke}, {yellow},
   {yellowgreen}
};

typedef ulong TGColour;
ATTRIBUTE:typedef<TGColour>;
ARRAY:typedef<TGColour>;

#define GCOL_TYPE_RGB              0x00000000
#define GCOL_TYPE_INDEX            0x10000000
#define GCOL_TYPE_INDEXB           0x20000000
#define GCOL_TYPE_NAMED            0x30000000
#define GCOL_TYPE_NONE             0x40000000
#define GCOL_TYPE_DIALOG           0x50000000
#define GCOL_TYPE_GRADIENT         0x60000000
#define GCOL_TYPE_MASK             0xF0000000
#define GCOL_TYPE(colour)          ((colour) & GCOL_TYPE_MASK)
#define GCOL_RGB(red, green, blue) (GCOL_TYPE_RGB   | ((ulong)(((red) & 0xFFL) << 16) | ((ulong)((green) & 0xFFL) << 8) | (ulong)((blue) & 0xFFL)))
#define GCOL_RGB_RED(colour)       ((colour >> 16) & 0xFFL)
#define GCOL_RGB_GREEN(colour)     ((colour >> 8) & 0xFFL)
#define GCOL_RGB_BLUE(colour)      ((colour) & 0xFFL)
#define GCOL_INDEX(index)          (GCOL_TYPE_INDEX | (index & ~GCOL_TYPE_MASK))
#define GCOL_INDEX_INDEX(colour)   (int) ((colour) & ~GCOL_TYPE_MASK)
#define GCOL_INDEXB(index)         (GCOL_TYPE_INDEXB | (index & ~GCOL_TYPE_MASK))
#define GCOL_INDEXB_INDEX(colour)  (int) ((colour) & ~GCOL_TYPE_MASK)
#define GCOL_GRADIENT(index)       (GCOL_TYPE_GRADIENT | (index & ~GCOL_TYPE_MASK))
#define GCOL_GRADIENT_INDEX(colour) (int) ((colour) & ~GCOL_TYPE_MASK)
//#define GCOL_NAMED(name)           (GCOL_TYPE_NAMED | (EGColourNamed.##name & ~GCOL_TYPE_MASK))
#define GCOL_NAMED_VALUE(index)    (GCOL_TYPE_NAMED | (index & ~GCOL_TYPE_MASK))
#define GCOL_NAMED_INDEX(colour)   (int) ((colour) & ~GCOL_TYPE_MASK)
#define GCOL_NAMED_NAME(colour)    (EGColourNamed) ((colour) & ~GCOL_TYPE_MASK)
#define GCOL_NONE                  GCOL_TYPE_NONE
#define GCOL_DIALOG                GCOL_TYPE_DIALOG
#define GCOL_UNDEFINED             0xFFFFFFFF
//#define GCOL_BLACK                 (GCOL_TYPE_NAMED + 1)
//#define GCOL_WHITE                 (GCOL_TYPE_NAMED + (sizeof(GColourNamed)/4) - 4)

#define GCOL_BLACK                 GCOL_RGB(0x00, 0x00, 0x00)
#define GCOL_WHITE                 GCOL_RGB(0xFF, 0xFF, 0xFF)

class CGPalette : CObjPersistent {
 private:
   void delete(void); 
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
 public:
   ALIAS<"svg:palette">;
   ATTRIBUTE<ARRAY<TGColour> colour, "color"> {
      if (!data) {
         /* Use Win16 default palette, good as any */
         ARRAY(&this->colour).used_set(64);
         memset(ARRAY(&this->colour).data(), 0, sizeof(TGColour) * 64);
         ARRAY(&this->colour).data()[0] = GCOL_RGB(0,0,0);
         ARRAY(&this->colour).data()[1] = GCOL_RGB(0,0,255);
         ARRAY(&this->colour).data()[2] = GCOL_RGB(0,255,255);
         ARRAY(&this->colour).data()[3] = GCOL_RGB(0,255,0);
         ARRAY(&this->colour).data()[4] = GCOL_RGB(192,0,192);
         ARRAY(&this->colour).data()[5] = GCOL_RGB(255,0,0);
         ARRAY(&this->colour).data()[6] = GCOL_RGB(255,255,0);
         ARRAY(&this->colour).data()[7] = GCOL_RGB(255,255,255);
         ARRAY(&this->colour).data()[8] = GCOL_RGB(0,0,128);
         ARRAY(&this->colour).data()[9] = GCOL_RGB(0,128,128);
         ARRAY(&this->colour).data()[10] = GCOL_RGB(0,128,0);
         ARRAY(&this->colour).data()[11] = GCOL_RGB(128,0,128);
         ARRAY(&this->colour).data()[12] = GCOL_RGB(128,0,0);
         ARRAY(&this->colour).data()[13] = GCOL_RGB(192,192,0);
         ARRAY(&this->colour).data()[14] = GCOL_RGB(192,192,192);
         ARRAY(&this->colour).data()[15] = GCOL_RGB(128,128,128);
      }
      else {
         ARRAY(&this->colour).data()[attribute_element] = *data;
      }
   };

   void CGPalette(ARRAY<TGColour> *colour);

//   void count_set(int count);
//   void colour_set(int index, TGColour colour);
//   TGColour colour_rgb(TGColour colour);
};

extern bool GCOL_PALETTE_FIND(CObjPersistent *object, CGPalette **palette, CGPalette **paletteB, bool *translate);
TGColour GCOL_CONVERT_RGB(CGPalette *palette, CGPalette *paletteB, TGColour colour);
TGColour GCOL_CONVERT_RGB_OBJECT(CObjPersistent *object, TGColour colour);

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#define GCOL_STRING_NONE "none"
#define GCOL_STRING_RGB "rgb"
#define GCOL_STRING_INDEX "index"
#define GCOL_STRING_INDEXB "indexB"
#define GCOL_STRING_GRADIENT "gradient"

TGColour GColourNamed[] = {
   0xF0F8FF, 0xFAEBD7, 0x00FFFF, 0x7FFFD4, 0xF0FFFF, 0xF4F4DC, 0xFFE4C4, 0x000000,
   0xFFEBCD, 0x0000FF, 0x822BE2, 0xA52A2A, 0xDEB887, 0x5F9EA0, 0x7FFF00, 0xD2691E,
   0xFF7F50, 0x6495ED, 0xFFF8DC, 0xDC143C, 0x00FFFF, 0x00008B, 0x008B8B, 0xB8860B,
   0xA9A9A9, 0x006400, 0xA9A9A9, 0xBDD76B, 0x8B008B, 0x556B2F, 0xFF8C00, 0x9932CC,
   0x8B0000, 0xE9967A, 0x8FBCFF, 0x483D8B, 0x2F4F4F, 0x2F4F4F, 0x00CED1, 0x9400D3,
   0xFF1493, 0x00BFFF, 0x696969, 0x696969, 0x1E90FF, 0xB22222, 0xFFFAF4, 0x228B22,
   0xFF00FF, 0xDCDCDC, 0xF8F8FF, 0xFFD700, 0xDAA520, 0x808080, 0x808080, 0x008000,
   0xADFF2F, 0xF0FFF0, 0xFF69B4, 0xCD5C5C, 0x4B0082, 0xFFFFF0, 0xF0E68C, 0xE6E6FA,
   0xFFF0F5, 0x7CFC00, 0xFFFACD, 0xADD8E6, 0xF08080, 0xE0FFFF, 0xFAFAD2, 0xD3D3D3,
   0x90EE90, 0xD3D3D3,
   0xFFB6C1, 0xFFA07A, 0x20B2AA, 0x87CEFA, 0x778899, 0x778899, 0xB0C4DE, 0xFFFFE0,
   0x00FF00, 0x32CD32, 0xFAF0E6, 0xFF00FF, 0x800000, 0x66CDAA, 0x0000CD, 0xBA55D3,
   0x9370DB, 0x3CB371, 0x7B68EE, 0x00FA9A, 0x48D1CC, 0xC71585, 0x191970, 0xF5FFFA,
   0xFFE4E1, 0xFFE4B5, 0xFFDEAD, 0x000080, 0xFDF5E6, 0x808000, 0x6B8E23, 0xFFA500,
   0xFF4500, 0xDA70D6, 0xEEE8AA, 0x98FB98, 0xAFEEEE, 0xDB7093, 0xFFEFD5, 0xFFDAB9,
   0xCD853F, 0xFFC0CB, 0xDDA0DD, 0xB0E0E6, 0x800080, 0xFF0000, 0xBC8F8F, 0x4169E1,
   0x8B4513, 0xFA8072, 0xF4A460, 0x2E8B57, 0xFFF5EE, 0xA0522D, 0xC0C0C0, 0x87CEEB,
   0x6A5ACD, 0x708090, 0x708090, 0xFFFAFA, 0x00FF7F, 0x4682B4, 0xD2B48C, 0x008080,
   0xD8BFD8, 0xFF6347, 0x40E0D0, 0xEE82EE, 0xF5DEB3, 0xFFFFFF, 0xF5F5F5, 0xFFFF00,
   0x9ACD32
};

bool ATTRIBUTE:convert<TGColour>(CObjPersistent *object,
                                 const TAttributeType *dest_type, const TAttributeType *src_type,
                                 int dest_index, int src_index,
                                 void *dest, const void *src) {
   TGColour *colour, rgb_colour;
   CString *string;
   int *value;
   int red, green, blue;
   bool channel_float;
   double channel_value;
   int i;
   const char *cp;
   CGPalette *palette, *paletteB;
   bool translate;

   if (dest_type == &ATTRIBUTE:type<TGColour> && src_type == NULL) {
      memset(dest, 0, sizeof(TGColour));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TGColour> && src_type == &ATTRIBUTE:type<TGColour>) {
      *((TGColour *)dest) = *((TGColour *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TGColour>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TGColour> && src_type == &ATTRIBUTE:type<CString>) {
      string = (CString *)src;
      colour = (TGColour *)dest;

      if (CString(string).length() == 0) {
         /*set empty string, no action*/
         return TRUE;
      }

      if (dest_index == -1) {
//         *colour = 0;
         if (CString(string).string()[0] == '#') {
            /*>>>CString::scanf() possible?*/
            if (CString(string).length() == 4) {
               sscanf(CString(string).string(), "#%1x%1x%1x", &red, &green, &blue);
               red = red * 255 / 15;
               green = green * 255 / 15;
               blue = blue * 255 / 15;
            }
            else {
               sscanf(CString(string).string(), "#%02x%02x%02x", &red, &green, &blue);
            }
            *colour = GCOL_RGB(red, green, blue);
            return TRUE;
         }

         /* 'none' colour */
         if (CString(string).strcmp(GCOL_STRING_NONE) == 0) {
            *colour = GCOL_NONE;
            return TRUE;
         }

         /* 'rgb' colour */
         if (CString(string).strncmp(GCOL_STRING_RGB, strlen(GCOL_STRING_RGB)) == 0) {
            *colour = GCOL_RGB(0xFF, 0, 0);
            cp = CString(string).strchr('(');
            channel_float = strchr(cp, '%') != NULL;
            red = 0;
            green = 0;
            blue = 0;
            if (cp) {
               cp++;
               if (channel_float) {
                  channel_value = atof(cp) * 255 / 100;
               }
               else {
                  channel_value = atof(cp);
               }
               red = (int)channel_value;
            }
            cp = strchr(cp, ',');
            if (cp) {
               cp++;
               if (channel_float) {
                  channel_value = atof(cp) * 255 / 100;
               }
               else {
                  channel_value = atof(cp);
               }
               green = (int)channel_value;
            }
            cp = strchr(cp, ',');
            if (cp) {
               cp++;
               if (channel_float) {
                  channel_value = atof(cp) * 255 / 100;
               }
               else {
                  channel_value = atof(cp);
               }
               blue = (int)channel_value;
            }
            *colour = GCOL_RGB(red, green, blue);
            return TRUE;
         }

         /* 'indexB' indexed colour */
         if (CString(string).strncmp(GCOL_STRING_INDEXB, strlen(GCOL_STRING_INDEXB)) == 0) {
            cp = CString(string).strchr('(');
            if (!cp) {
               cp = CString(string).strchr('[');
            }
            if (cp) {
               *colour = GCOL_TYPE_INDEXB | atoi(cp + 1);
//               if (CObject(object).obj_class() != &class(CGPalette) &&
//                  GCOL_PALETTE_FIND(object, &palette, &paletteB, &translate) && translate) {
//                  *colour = GCOL_CONVERT_RGB(palette, paletteB, *colour);
//               }
               return TRUE;
            }
            return FALSE;
         }
         /* 'index' indexed colour */
         if (CString(string).strncmp(GCOL_STRING_INDEX, strlen(GCOL_STRING_INDEX)) == 0) {
            cp = CString(string).strchr('(');
            if (!cp) {
               cp = CString(string).strchr('[');
            }
            if (cp) {
               *colour = GCOL_TYPE_INDEX | atoi(cp + 1);
               if (CObject(object).obj_class() != &class(CGPalette) &&
                  GCOL_PALETTE_FIND(object, &palette, &paletteB, &translate) && translate) {
                  *colour = GCOL_CONVERT_RGB(palette, paletteB, *colour);
               }
               return TRUE;
            }
            return FALSE;
         }
         /* 'gradient' index */
         if (CString(string).strncmp(GCOL_STRING_GRADIENT, strlen(GCOL_STRING_GRADIENT)) == 0) {
            cp = CString(string).strchr('(');
            if (!cp) {
               cp = CString(string).strchr('[');
            }
            if (cp) {
               *colour = GCOL_TYPE_GRADIENT | atoi(cp + 1);
               return TRUE;
            }
            return FALSE;
         }

         /* search for named colours */
         for (i = 0; i < EGColourNamed.COUNT(); i++) {
            if (CString(string).strcmp(EGColourNamed(i).name()) == 0) {
               *colour = GCOL_TYPE_NAMED | i;
               return TRUE;
            }
         }
      }
      else {
         i = atoi(CString(string).string());
         if (i < 0) {
            i = 0;
         }
         if (i > 0xFF) {
            i = 0xFF;
         }
         switch (GCOL_TYPE(*colour)) {
         case GCOL_TYPE_RGB:
            rgb_colour = *colour;
            break;
         case GCOL_TYPE_GRADIENT:
            rgb_colour = 0;
            break;
         case GCOL_TYPE_INDEX:
         case GCOL_TYPE_INDEXB:
            rgb_colour = GCOL_CONVERT_RGB_OBJECT(object, *colour);
            break;
         case GCOL_TYPE_NAMED:
            rgb_colour = GColourNamed[GCOL_NAMED_INDEX(*colour)];
            break;
         case GCOL_TYPE_NONE:
            rgb_colour = 0;
            break;
         default:
            return FALSE;
         }

         switch(dest_index) {
         case 0:
            *colour = GCOL_RGB(i,
                               GCOL_RGB_GREEN(rgb_colour),
                               GCOL_RGB_BLUE(rgb_colour));
            break;
         case 1:
            *colour = GCOL_RGB(GCOL_RGB_RED(rgb_colour),
                               i,
                               GCOL_RGB_BLUE(rgb_colour));
            break;
         case 2:
            *colour = GCOL_RGB(GCOL_RGB_RED(rgb_colour),
                               GCOL_RGB_GREEN(rgb_colour),
                               i);
            break;
         default:
            return FALSE;
         }
         return TRUE;
      }
      return FALSE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TGColour>) {
      colour = (TGColour *)src;
      string = (CString *)dest;

      if (src_index == -1) {
         switch (GCOL_TYPE(*colour)) {
         case GCOL_TYPE_RGB:
            CString(string).printf("#%02x%02x%02x",
                                   GCOL_RGB_RED(*colour), GCOL_RGB_GREEN(*colour), GCOL_RGB_BLUE(*colour));
            break;
         case GCOL_TYPE_INDEX:
            CString(string).printf(GCOL_STRING_INDEX"(%d)", GCOL_INDEX_INDEX(*colour));
            return TRUE;
         case GCOL_TYPE_INDEXB:
            CString(string).printf(GCOL_STRING_INDEXB"(%d)", GCOL_INDEXB_INDEX(*colour));
            return TRUE;
         case GCOL_TYPE_GRADIENT:
            CString(string).printf(GCOL_STRING_GRADIENT"(%d)", GCOL_GRADIENT_INDEX(*colour));
            return TRUE;
         default:
            CString(string).printf("?");
            return FALSE;
         case GCOL_TYPE_NAMED:
            /*>>>kludge, use GCOL_NAMED_NAME*/
            CString(string).printf("%s", EGColourNamed((*colour) & ~GCOL_TYPE_MASK).name());
            break;
         case GCOL_TYPE_NONE:
            CString(string).printf(GCOL_STRING_NONE);
            break;
         }
         return TRUE;
      }
      else {
         switch (GCOL_TYPE(*colour)) {
         case GCOL_TYPE_RGB:
            rgb_colour = *colour;
            break;
         case GCOL_TYPE_INDEX:
         case GCOL_TYPE_INDEXB:
            rgb_colour = GCOL_CONVERT_RGB_OBJECT(object, *colour);
            break;
         case GCOL_TYPE_NAMED:
            rgb_colour = GColourNamed[GCOL_NAMED_INDEX(*colour)];
            break;
         case GCOL_TYPE_NONE:
            rgb_colour = 0;
            break;
         default:
            return FALSE;
         }

         switch (src_index) {
         case 0:
            CString(string).printf("%d", GCOL_RGB_RED(rgb_colour));
            break;
         case 1:
            CString(string).printf("%d", GCOL_RGB_GREEN(rgb_colour));
            break;
         case 2:
            CString(string).printf("%d", GCOL_RGB_BLUE(rgb_colour));
            break;
         default:
            return FALSE;
         }
         return TRUE;
      }
   }

   if (dest_type == &ATTRIBUTE:type<TGColour> && src_type == &ATTRIBUTE:type<int>) {
      value = (int *)src;
      colour = (TGColour *)dest;

      switch (GCOL_TYPE(*colour)) {
      case GCOL_TYPE_RGB:
         rgb_colour = *colour;
         break;
      case GCOL_TYPE_INDEX:
      case GCOL_TYPE_INDEXB:
         rgb_colour = GCOL_CONVERT_RGB_OBJECT(object, *colour);
         break;
      case GCOL_TYPE_NAMED:
         rgb_colour = GColourNamed[GCOL_NAMED_INDEX(*colour)];
         break;
      case GCOL_TYPE_NONE:
         rgb_colour = 0;
         break;
      default:
         return FALSE;
      }

      switch(dest_index) {
      case -1:
         *colour = GCOL_RGB(*value, *value, *value);
         break;
      case 0:
         *colour = GCOL_RGB(*value,
                            GCOL_RGB_GREEN(rgb_colour),
                            GCOL_RGB_BLUE(rgb_colour));
         break;
      case 1:
         *colour = GCOL_RGB(GCOL_RGB_RED(rgb_colour),
                            *value,
                            GCOL_RGB_BLUE(rgb_colour));
         break;
      case 2:
         *colour = GCOL_RGB(GCOL_RGB_RED(rgb_colour),
                            GCOL_RGB_GREEN(rgb_colour),
                            *value);
         break;
      default:
         return FALSE;
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<TGColour>) {
      colour = (TGColour *)src;
      value = (int *)dest;

      switch (GCOL_TYPE(*colour)) {
      case GCOL_TYPE_RGB:
         rgb_colour = *colour;
         break;
      case GCOL_TYPE_INDEX:
      case GCOL_TYPE_INDEXB:
         rgb_colour = GCOL_CONVERT_RGB_OBJECT(object, *colour);
         break;
      case GCOL_TYPE_NAMED:
         rgb_colour = GColourNamed[GCOL_NAMED_INDEX(*colour)];
         break;
      case GCOL_TYPE_NONE:
         rgb_colour = 0;
         break;
      default:
         return FALSE;
      }

      switch(src_index) {
      case -1:
         *value = (GCOL_RGB_RED(rgb_colour) + GCOL_RGB_GREEN(rgb_colour) + GCOL_RGB_BLUE(rgb_colour)) / 3;
         break;
      case 0:
         *value = GCOL_RGB_RED(rgb_colour);
         break;
      case 1:
         *value = GCOL_RGB_GREEN(rgb_colour);
         break;
      case 2:
         *value = GCOL_RGB_BLUE(rgb_colour);
         break;
      default:
         return FALSE;
      }
      return TRUE;
   }

   return FALSE;
}

void CGPalette::CGPalette(ARRAY<TGColour> *palette) {
   ARRAY(&this->colour).new();
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGPalette,colour>, TRUE);

//   /*>>>hack, fixed palette size*/
//   ARRAY(&this->colour).used_set(32);
//   memset(ARRAY(&this->colour).data(), 0, 32 * sizeof(TGColour));
}/*CGPalette::CGPalette*/

void CGPalette::delete(void) {
   ARRAY(&this->colour).delete();
}/*CGPalette::delete*/

TGColour GCOL_CONVERT_RGB_OBJECT(CObjPersistent *object, TGColour colour) {
   ARRAY<TGColour> *palette = NULL, *paletteB = NULL;
   const TAttribute *attribute, *attributeB;

   attribute = CObjPersistent(object).attribute_find("palette");
   if (attribute) {
      palette = (ARRAY<TGColour> *)(((byte *)object) + attribute->offset);
   }
   attributeB = CObjPersistent(object).attribute_find("paletteB");
   if (attributeB) {
      paletteB = (ARRAY<TGColour> *)(((byte *)object) + attributeB->offset);
   }

   switch (GCOL_TYPE(colour)) {
   case GCOL_TYPE_INDEX:
      if (!palette) {
         return GCOL_BLACK;
      }
      if (GCOL_INDEX_INDEX(colour) >= ARRAY(palette).count()) {
         return GCOL_BLACK;
      }
      if (GCOL_TYPE(ARRAY(palette).data()[GCOL_INDEX_INDEX(colour)]) == GCOL_TYPE_RGB) {
         return ARRAY(palette).data()[GCOL_INDEX_INDEX(colour)];
      }
      return GCOL_BLACK;
   case GCOL_TYPE_INDEXB:
      if (!paletteB) {
         return GCOL_BLACK;
      }
      if (GCOL_INDEXB_INDEX(colour) >= ARRAY(paletteB).count()) {
         return GCOL_BLACK;
      }
      if (GCOL_TYPE(ARRAY(paletteB).data()[GCOL_INDEX_INDEX(colour)]) == GCOL_TYPE_RGB) {
         return ARRAY(paletteB).data()[GCOL_INDEX_INDEX(colour)];
      }
      return GCOL_BLACK;
   case GCOL_TYPE_NAMED:
      return GColourNamed[GCOL_NAMED_NAME(colour)];
   case GCOL_TYPE_DIALOG:
   default:
      return colour;
   }
}

TGColour GCOL_CONVERT_RGB(CGPalette *palette, CGPalette *paletteB, TGColour colour) {
   switch (GCOL_TYPE(colour)) {
   case GCOL_TYPE_INDEX:
      if (!palette) {
         return GCOL_BLACK;
      }
      if (GCOL_INDEX_INDEX(colour) >= ARRAY(&palette->colour).count()) {
         return GCOL_BLACK;
      }
      if (GCOL_TYPE(ARRAY(&palette->colour).data()[GCOL_INDEX_INDEX(colour)]) != GCOL_TYPE_INDEX) {
          return GCOL_CONVERT_RGB(palette, paletteB, ARRAY(&palette->colour).data()[GCOL_INDEX_INDEX(colour)]);
      }
      return GCOL_BLACK;
   case GCOL_TYPE_INDEXB:
      if (!paletteB) {
         return GCOL_BLACK;
      }
      if (GCOL_INDEX_INDEX(colour) >= ARRAY(&paletteB->colour).count()) {
         return GCOL_BLACK;
      }
      if (GCOL_TYPE(ARRAY(&paletteB->colour).data()[GCOL_INDEX_INDEX(colour)]) != GCOL_TYPE_INDEX) {
          return GCOL_CONVERT_RGB(palette, paletteB, ARRAY(&paletteB->colour).data()[GCOL_INDEX_INDEX(colour)]);
      }
      return GCOL_BLACK;
   case GCOL_TYPE_NAMED:
      return GColourNamed[GCOL_NAMED_NAME(colour)];
   case GCOL_TYPE_DIALOG:
   default:
      return colour;
   }
}

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/