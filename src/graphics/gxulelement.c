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
#include "../cstring.c"
#include "../datetime.c"
#include "../objpersistent.c"
#include "../objdataserver.c"
#include "gobject.c"
//#include "gcanvas.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGLayout;
class CGTree;
class CGWindow;
class CGWindowChild;

class CGXULElement : CGObject {
 private:
   void new(void);
   void delete(void);

   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void show(bool show);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual bool keyboard_input(void);
   virtual void cpoint_get(ARRAY<TPoint> *point);
   virtual void cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new);
   virtual void create_point_set(TPoint *point);
   virtual bool transform_assimilate(TGTransform *transform);
   virtual void extent_set(CGCanvas *canvas);
   virtual void binding_resolve(void);
   virtual CGObject *render_primative(bool for_print);
   virtual void NATIVE_draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
   void* NATIVE_get_background(void);

   /*>>>probably better to set focus on layout/window, not element*/
   bool NATIVE_focus_current(void);
   void NATIVE_focus_in(void);
   void NATIVE_focus_out(void);
   bool updating;
   
   /*>>>just for Win32, implment for all for now */
   bool repeat_event(CEvent *event);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
   void utov(CGCanvas *canvas, TPoint *result);
   bool allocated_position(TRect *result);
 public:
   ATTRIBUTE<TCoord x>;
   ATTRIBUTE<TCoord y>;
   ATTRIBUTE<TCoord width>;
   ATTRIBUTE<TCoord height>;

   ATTRIBUTE<int flex> {
      if (data)
         this->flex = *data;
      else
         this->flex = 1;
   };
   ATTRIBUTE<TCoord min_width,  "min-width">;
   ATTRIBUTE<TCoord min_height, "min-height">;
   ATTRIBUTE<TCoord max_width,  "max-width">;
   ATTRIBUTE<TCoord max_height, "max-height">;
   ATTRIBUTE<bool disabled, , PO_INHERIT>;
   ATTRIBUTE<bool isdefault, "default">;
   ATTRIBUTE<CString tooltiptext>;
   ATTRIBUTE<CXPath binding>;

   void CGXULElement(int x, int y, int width, int height);
};

class CGSpacer : CGXULElement {
 public:
   ALIAS<"xul:spacer">;

   void CGSpacer(int x, int y, int width, int height);
};

class CGContainer : CGXULElement {
 private:
   bool allow_child_repos;              /*>>>kludge, for scrolledarea*/

   void new(void);
   void delete(void);

   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);

   virtual void show(bool show);
 public:
   void CGContainer(void);
};

class CGPacker : CGContainer {
 private:
   void delete(void); 
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode);
 public:
   void CGPacker(void);
};

ENUM:typedef<EGBoxOrientType> {
   {horizontal},
   {vertical},
};

ENUM:typedef<EGBoxPackType> {
   {start},
   {center},
   {end}
};

ENUM:typedef<EGBoxAlignType> {
   {start},
   {center},
   {end},
   {baseline},
   {stretch}
};

class CGBox : CGPacker {
 private:
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);

   CObject *all_child_next(CObject *child);
 public:
   ALIAS<"xul:box">;

   ATTRIBUTE<EGBoxOrientType orient>;
   ATTRIBUTE<EGBoxPackType pack>;
   ATTRIBUTE<EGBoxAlignType align>;

   void CGBox(void);
};

class CGVBox : CGBox {
 private:
   void new(void);
 public:
   ALIAS<"xul:vbox">;

    void CGVBox(void);
};

class CGHBox : CGBox {
 private:
   void new(void);
 public:
   ALIAS<"xul:hbox">;

   void CGHBox(void);
};

class CGStack : CGPacker {
 private:
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
 public:
   ALIAS<"xul:stack">;

    void CGStack(void);
};

class CGDeck : CGPacker {
 public:
   ALIAS<"xul:deck">;

    void CGDeck(void);
};

class CGGridCols : CObjPersistent {
 public:
   ALIAS<"xul:columns">;

   void CGGridCols(void);
};

class CGGridCol: CObjPersistent {
 public:
   ALIAS<"xul:column">;

   void CGGridCol(void);
};

class CGGridRows : CObjPersistent {
 public:
   ALIAS<"xul:rows">;

   void CGGridRows(void);
};

class CGGridRow: CObjPersistent {
 public:
   ALIAS<"xul:row">;

   void CGGridRow(void);
};

class CGGrid : CGPacker {
// private:
//   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
//   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
 public:
   ALIAS<"xul:grid">;

   ELEMENT:OBJECT<CGGridCols grid_cols, "xul:columns">;
   ELEMENT:OBJECT<CGGridRows grid_rows, "xul:rows">;

   void CGGrid(void);
};

//class CGXULImage : CGXULElement {
// private:
//   CGBitmap bitmap;
//};

ENUM:typedef<EGXULDirType> {
   {ltr},
   {rtl},
};

class CGButton : CGXULElement {
 private:
   void new(void);
   void delete(void);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:button">;

   ATTRIBUTE<EGXULDirType dir> {
       if (data) {
           this->dir = *data;
       }
       else {
           this->dir = EGXULDirType.ltr;
       }
   };
   ATTRIBUTE<CString label>;
   ATTRIBUTE<CString src>;

   void CGButton(int x, int y, int width, int height, const char *label);
};

class CGColourPicker : CGButton {
 private:
   void new(void);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:colorpicker">;

   ATTRIBUTE<TGColour colour, "color">;
   ATTRIBUTE<bool native_picker, "nativePicker"> {
      if (data) {
         this->native_picker = *data;
      }
      else {
         this->native_picker = FALSE;
      }
   };

   void CGColourPicker(int x, int y, int width, int height, TGColour colour);
};

class CGDatePickerButton : CGButton {
 private:
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:datepickerbutton">;

   ATTRIBUTE<TUNIXTime time>;

   void CGDatePickerButton(int x, int y, int width, int height, TUNIXTime time);
};


class CGToggleButton : CGButton {
 private:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:togglebutton">;
   ATTRIBUTE<bool toggled>;
   void CGToggleButton(int x, int y, int width, int height, char *label, bool toggled);
};

class CGCheckBox : CGButton {
 private:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:checkbox">;
   ATTRIBUTE<bool checked>;
   void CGCheckBox(int x, int y, int width, int height, char *label, bool checked);
};

class CGRadioButton : CGButton {
 private:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:radio">;
   ATTRIBUTE<bool selected>;
   void CGRadioButton(int x, int y, int width, int height, char *label, bool selected);
};

class CGSlider : CGXULElement {
 private:
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:slider">;

   ATTRIBUTE<int minimum>;
   ATTRIBUTE<int maximum>;
   ATTRIBUTE<int position>;

   void CGSlider(int x, int y, int width, int height);
};

class CGTextLabel : CGXULElement {
 private:
   void new(void);
   void delete(void);
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:text">;

   ATTRIBUTE<CString value>;
   ATTRIBUTE<TGColour colour, "color">;
   ATTRIBUTE<TGColour background_colour, "background-color">;
   ATTRIBUTE<int font_size, "font-size">;
   ATTRIBUTE<EGXULDirType dir> {
       if (data) {
           this->dir = *data;
       }
       else {
           this->dir = EGXULDirType.ltr;
       }
   };

   void CGTextLabel(int x, int y, int width, int height, const char *text);
};

ENUM:typedef<EGProgressMode> {
   {determined},
   {undetermined},
};

class CGProgressMeter : CGXULElement {
 private:
   virtual void binding_resolve(void); 
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing); 
   virtual void NATIVE_allocate(CGLayout *layout);
 public:
   ALIAS<"xul:progressmeter">;
   ATTRIBUTE<EGProgressMode mode> {
      this->mode = data ? *data : EGProgressMode.determined;
   };
   ATTRIBUTE<int value>;   
};

class CGDatePicker : CGXULElement {
 private:
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:datepicker">;

   ATTRIBUTE<TUNIXTime time>;

   void CGDatePicker(int x, int y, int width, int height, TUNIXTime time);
};


/*>>>not quite ideal, xul status bar panels can have images too, and label attribute is 'label' not 'value' */
class CGStatusBarPanel : CGTextLabel {
 public:
   ALIAS<"xul:statusbarpanel">;
   void CGStatusBarPanel(int x, int y, int width, int height, const char *text);
};

#if !OS_PalmOS

ENUM:typedef<EGTextBoxType> {
   {normal},
   {nodefault},   
   {autocomplete},
   {password},
   {numeric},
   {numericSpace}, /* Same as numeric, but allows spaces */
   {timed}
};

class CGTextBox : CGXULElement {
 private:
   bool modified;

   void new(void);
   void delete(void);
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:textbox">;

   ATTRIBUTE<TGColour colour, "color"> {
       if (data) {
           this->colour = *data;
       }
       else {
           this->colour = CGXULElement(this)->isdefault ? (GCOL_TYPE_NAMED | EGColourNamed.darkgrey) : GCOL_BLACK;
       }
   };
   ATTRIBUTE<TGColour background_colour, "background-color"> {
       if (data) {
           this->background_colour = *data;
       }
       else {
           this->background_colour = GCOL_NONE;//CGXULElement(this)->isdefault ? GCOL_RGB(255, 245, 231) : GCOL_NONE;
       }
   };
   ATTRIBUTE<CString value>;
   ATTRIBUTE<EGTextBoxType type> {
       if (data) {
           this->type = *data;
       }
       else {
           this->type = EGTextBoxType.normal;
       }
   };
   ATTRIBUTE<EGXULDirType dir> {
       if (data) {
           this->dir = *data;
       }
       else {
           this->dir = EGXULDirType.ltr;
       }
   };
   ATTRIBUTE<int maxlength> {
       if (data) {
           this->maxlength = *data;
       }
       else {
           this->maxlength = -1;
       }
   };
   ATTRIBUTE<bool multiline> {
       if (data) {
           this->multiline = *data;
       }
       else {
           this->multiline = FALSE;
       }
   };

   void CGTextBox(int x, int y, int width, int height, char *text);
};

ENUM:typedef<EGScrollBarUpdateType> {
   {normal},
   {nodrag},
};

class CGScrollBar : CGXULElement {
 private:
   void new(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void binding_resolve(void);
 public:
   ALIAS<"xul:scrollbar">;
   ATTRIBUTE<EGBoxOrientType orient>;
   ATTRIBUTE<int curpos>;
   ATTRIBUTE<int maxpos>;
   ATTRIBUTE<int range>;
   ATTRIBUTE<EGScrollBarUpdateType update_type, "updateType"> {
       if (data) {
           this->update_type = *data;
       }
       else {
           this->update_type = EGScrollBarUpdateType.normal;
       }
   };

   void CGScrollBar(int x, int y, int width, int height);
};

class CGScrolledArea : CGContainer {
 private:
   void new(void);
   void delete(void);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);

   CGScrollBar scroll_horiz;
   CGScrollBar scroll_vert;
 public:
   void CGScrolledArea(int x, int y, int width, int height);

   ALIAS<"xul:scrolledarea">;
//>>>   ELEMENT:OBJECT<CGScrollBar hscroll>;
//   ELEMENT:OBJECT<CGScrollBar vscroll>;
};

/*>>>possibly inherit from CObjPersistent*/
ENUM:typedef<EGListItemType> {
   {normal},
   {radio},
   {checkbox},
};

class CGListItem : CGObject {
 private:
   void new(void);
   void delete(void);

   void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   static inline void value_int_default(void);
 public:
   ALIAS<"xul:listitem">;

   ATTRIBUTE<int value_int, "valueInt"> {
      if (data) {
         this->value_int = *data;
      }
      else {
         CGListItem(this).value_int_default();
//       if (CObject(this).parent()) {
//          this->value_int = CObject(CObject(this).parent()).child_index(CObject(this));
//       }
      }
   };
   ATTRIBUTE<CString value, "value"> {
      if (data) {
         CString(&this->value).set_string(data);
      }
      else {
         CString(&this->value).set_string(&this->label);
      }
      CGListItem(this).value_int_default();
//    this->value_int = atoi(CString(&this->value).string());
   };
   ATTRIBUTE<EGListItemType type> {
      if (data) {
         this->type = *data;
      }
      else {
         this->type = EGListItemType.normal;
      }
   };
   ATTRIBUTE<bool disabled>;
   ATTRIBUTE<CString label>;
   ATTRIBUTE<char accesskey> {
      if (data) {
         this->accesskey = *data;
      }
      else {
         this->accesskey = 0;
      }
   };
   ATTRIBUTE<int id>;
   ATTRIBUTE<bool checked>;
   ATTRIBUTE<bool isdefault> {
      if (data) {
         this->isdefault = *data;
      }
      else {
         this->isdefault = FALSE;
      }
   };
   ATTRIBUTE<TGColour colour, "color"> {
      if (data) {
         this->colour = *data;
      }
      else {
         this->colour = this->isdefault ? (GCOL_TYPE_NAMED | EGColourNamed.darkgrey) : GCOL_BLACK;
      }
   };

   void CGListItem(const char *label, int id, bool disabled, bool checked);
};

static inline void CGListItem::value_int_default(void) {
   if (!CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGListItem,value_int>))
      return;
   if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGListItem,value>)) {
      if (CObject(this).parent()) {
         this->value_int = CObject(CObject(this).parent()).child_index(CObject(this));
      }
      else {
         this->value_int = atoi(CString(&this->value).string());
      }
   }
}/*CGListItem::value_int_default*/


class CGMenuItem : CGListItem {
 private:
   void new(void);
   void delete(void);

   CObjPersistent *menu_object;
   const TAttribute *menu_attribute;
 public:
   void CGMenuItem(const char *label, const char *acceltext, int id, bool disabled, bool checked);
   static inline void attribute_set(CObjPersistent *object, const TAttribute *attribute);

   ALIAS<"xul:menuitem">;
   ATTRIBUTE<TKeyboardKey accelerator>;
   ATTRIBUTE<CString acceltext>;
};

class CGMenuSeparator : CGMenuItem {
 public:
   void CGMenuSeparator(void);

   ALIAS<"xul:menuseparator">;
};

static inline void CGMenuItem::attribute_set(CObjPersistent *object, const TAttribute *attribute) {
   this->menu_object = object;
   this->menu_attribute = attribute;
};

class CGListBox : CGXULElement {
 private:
   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
   virtual void NATIVE_allocate(CGLayout *layout);

//#if OS_Linux
   void *items;
//#endif
 public:
   ALIAS<"xul:listbox">;
   ATTRIBUTE<int item>;

   void CGListBox(int x, int y, int width, int height);
};

class CGListMenuPopup : CObjPersistent {
 public:
   ALIAS<"xul:menupopup">;

   void CGListMenuPopup(void);
};

class CGMenuList : CGXULElement {
 private:
   ATTRIBUTE<int item>;

   void new(void);
   void delete(void);

   void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   void binding_resolve(void);
   void NATIVE_list_set(int enum_count, const char **enum_name);
   virtual void NATIVE_allocate(CGLayout *layout);
//#if OS_Linux
   void *strings;
//#endif
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:menulist">;

   ELEMENT:OBJECT<CGListMenuPopup menupopup, "xul:menupopup">;
   ATTRIBUTE<TGColour colour, "color"> {
       if (data) {
           this->colour = *data;
       }
       else {
           this->colour = CGXULElement(this)->isdefault ? (GCOL_TYPE_NAMED | EGColourNamed.darkgrey) : GCOL_BLACK;
       }
   };
   ATTRIBUTE<TGColour background_colour, "background-color"> {
       if (data) {
           this->background_colour = *data;
       }
       else {
           this->background_colour = GCOL_WHITE;//CGXULElement(this)->isdefault ? GCOL_RGB(255, 245, 231) : GCOL_NONE;
       }
   };

   void CGMenuList(int x, int y, int width, int height);
};

class CObjClientGTree : CObjClient {
 private:
   CGTree *gtree;
   virtual void notify_all_update(void);
   virtual void notify_object_child_add(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_child_remove(CObjPersistent *object, CObjPersistent *child);
   virtual void notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void notify_selection_update(CObjPersistent *object, bool changing);
   void *NATIVE_item_object_find(CObjPersistent *object, void *root);
 public:
   void CObjClientGTree(CObjServer *obj_server, CObject *host,
                        CObjPersistent *object_root, CGTree *gtree);
};

class CGTreeCols : CObjPersistent {
 public:
   ALIAS<"xul:treecols">;

   void CGTreeCols(void);
};

class CGTreeCol : CObjPersistent {
 private:
   int position;

   void new(void);
   void delete(void);
 public:
   ALIAS<"xul:treecol">;
   ATTRIBUTE<CString label>;

   void CGTreeCol(const char *label);
};

class CGTree : CGXULElement {
 private:
   void new(void);
   void delete(void);

   CObjClientGTree client;
   TAttribute *attribute_selected;

   virtual void NATIVE_allocate(CGObjCanvas *canvas);

   void NATIVE_XULElement_fill(void *parent, CObjPersistent *object, int index);
#if OS_Win32
   void *hwnd_header;
   void *hwnd_tree;
#endif
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
//#if OS_Linux
   void NATIVE_XULElement_update_object(void *node, CObjPersistent *object);
   void *tree_widget;
//#endif
   bool selection_resolving;
 public:
   ALIAS<"xul:tree">;

   ATTRIBUTE<bool attribute_hide, "attribute-hide">;
   ELEMENT:OBJECT<CGTreeCols tree_cols, "xul:treecols">;

   void CGTree(CObjServer *obj_server, CObjPersistent *object_root, int x, int y, int width, int height);
   void column_add(const char *label);
};

class CGTabBox : CGContainer {
  private:
   virtual void NATIVE_allocate(CGLayout *layout);
#if OS_Win32
   void *wndproc;
#endif
   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
  public:
   ALIAS<"xul:tabbox">;
   ATTRIBUTE<int selectedIndex>;

   void CGTabBox(int x, int y, int width, int height);
};

ENUM:typedef<EGSplitterType> {
   {Vertical},
   {Horizontal},
};

class CGSplitter : CGContainer {
 private:
   int old_position;
   int drag_mode;

   virtual void notify_object_linkage(EObjectLinkage linkage, CObject *object);
   virtual void notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing);
   virtual void NATIVE_allocate(CGLayout *layout);
   virtual void NATIVE_release(CGLayout *layout);
#if OS_Win32
#endif
   virtual long WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam);
 public:
   ALIAS<"xul:splitter">;

   void CGSplitter(int x, int y, int width, int height);

   ATTRIBUTE<EGSplitterType type>;
   ATTRIBUTE<int position>;
};

#endif

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CGXULElement::new(void) {
   new(&this->binding).CXPath(NULL, NULL);
   new(&this->tooltiptext).CString(NULL);

   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,binding>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,disabled>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,isdefault>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,flex>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,tooltiptext>, TRUE);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>,  TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,max_width>,  TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,max_height>, TRUE);
}/*CGXULElement::new*/

void CGXULElement::CGXULElement(int x, int y, int width, int height) {
   CGXULElement(this)->x      = x;
   CGXULElement(this)->y      = y;
   CGXULElement(this)->width  = width;
   CGXULElement(this)->height = height;

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>,  TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,max_width>,  TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,max_height>, TRUE);

   if (width == 0 && height == 0) {
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,x>, TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,y>, TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,width>,  TRUE);
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGXULElement,height>, TRUE);
   }
}/*CGXULElement::CGXULElement*/

void CGXULElement::delete(void) {
   CGXULElement(this).show(FALSE);

   delete(&this->tooltiptext);
   delete(&this->binding);

   class:base.delete();
}/*CGXULElement::delete*/

void CGXULElement::cpoint_get(ARRAY<TPoint> *point) {
   ARRAY(point).used_set(2);

   ARRAY(point).data()[0].x = this->x;
   ARRAY(point).data()[0].y = this->y;
   ARRAY(point).data()[1].x = this->x + this->width;
   ARRAY(point).data()[1].y = this->y + this->height;
}/*CGXULElement::cpoint_get*/

void CGXULElement::cpoint_set(ARRAY<TPoint> *point, int index, TPoint *point_new) {
   switch (index) {
   case -1:
      this->x = ARRAY(point).data()[0].x;
      this->y = ARRAY(point).data()[0].y;
      this->width  = ARRAY(point).data()[1].x - ARRAY(point).data()[0].x;
      this->height = ARRAY(point).data()[1].y - ARRAY(point).data()[0].y;
      return;
   case 0:
      ARRAY(point).data()[index] = *point_new;
      this->x = ARRAY(point).data()[0].x;
      this->y = ARRAY(point).data()[0].y;
      this->width  = ARRAY(point).data()[1].x - ARRAY(point).data()[0].x;
      this->height = ARRAY(point).data()[1].y - ARRAY(point).data()[0].y;
      break;
   case 1:
      ARRAY(point).data()[index] = *point_new;
      this->width  = ARRAY(point).data()[1].x - ARRAY(point).data()[0].x;
      this->height = ARRAY(point).data()[1].y - ARRAY(point).data()[0].y;
      break;
   }
}/*CGXULElement::cpoint_set*/

void CGXULElement::create_point_set(TPoint *point) {
   this->x      = point[0].x;
   this->y      = point[0].y;
   this->width  = point[1].x - point[0].x;
   this->height = point[1].y - point[0].y;
}/*CGXULElement::create_point_set*/

bool CGXULElement::transform_assimilate(TGTransform *transform) {
   switch (transform->type) {
   case EGTransform.scale:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      this->x = this->x * transform->t.scale.sx;
      this->y = this->y * transform->t.scale.sy;
      this->width = this->width * transform->t.scale.sx;
      this->height = this->height * transform->t.scale.sy;
      CObjPersistent(this).attribute_update_end();
      return TRUE;
   case EGTransform.translate:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,y>);
      this->x += (int) transform->t.translate.tx;
      this->y += (int) transform->t.translate.ty;
      CObjPersistent(this).attribute_update_end();
      return TRUE;
   default:
      break;
   }
   return FALSE;
}/*CGXULElement::transform_assimilate*/

void CGXULElement::binding_resolve(void) {}
CGObject *CGXULElement::render_primative(bool for_print) { return NULL; }

//void CGXULElement::NATIVE_allocate(CGLayout *layout) {}

void CGContainer::new(void) {
   class:base.new();
}/*CGContainer::new*/

void CGContainer::CGContainer(void) {
   CGXULElement(this).CGXULElement(0, 0, 0, 0);
}/*CGContainer::CGContainer*/

void CGContainer::delete(void) {
   class:base.delete();
}/*CGContainer::delete*/

void CGContainer::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   /*>>>quick hack*/
   _virtual_CObjPersistent_notify_object_linkage(CObjPersistent(this), linkage, object);

   switch (linkage) {
   case EObjectLinkage.ParentSet:
      CGObject(this).show(TRUE);
      break;
   case EObjectLinkage.ParentReleasePre:      
      CGObject(this).show(FALSE);   
      break;
   case EObjectLinkage.ParentRelease:
//      CGObject(this).show(FALSE);
      break;
   default:
      break;
   }
}/*CGContainer::notify_object_linkage*/

void CGContainer::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);
}/*CGContainer::notify_attribute_update*/

void CGContainer::show(bool show) {
   CObject *object = CObject(this).child_first();

   if (show && !CObjClass_is_derived(&class(CGPacker), CObject(this).obj_class())) {
      class:base.show(show);
   }

   if (!show || !CObjClass_is_derived(&class(CGTabBox), CObject(this).obj_class())) {
      /* show all children of container object */
      while (object) {
         if (CObjClass_is_derived(&class(CGObject), CObject(object).obj_class())) {
            CGObject(object).show(show);
         }
         object = CObject(this).child_next(object);
      }
   }

   if (!show && !CObjClass_is_derived(&class(CGPacker), CObject(this).obj_class())) {
      class:base.show(show);
   }
}/*CGContainer::show*/

void CGContainer::NATIVE_allocate(CGLayout *layout) {
   class:base.NATIVE_allocate(layout);
}/*CGContainer::NATIVE_allocate*/

void CGContainer::NATIVE_release(CGLayout *layout) {
   class:base.NATIVE_release(layout);
}/*CGContainer::NATIVE_release*/

void CGPacker::CGPacker(void) {
   CGContainer(this).CGContainer();
}/*CGPacker::CGPacker*/

void CGPacker::delete(void) {
}/*CGPacker::delete*/

void CGPacker::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);
}/*CGPacker::notify_attribute_update*/

void CGPacker::draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
   /*prevent empty XUL element rendering */
}/*CGPacker::draw*/

void CGBox::CGBox(void) {
   CGPacker(this).CGPacker();
}/*CGBox::CGBox*/

void CGBox::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   /*>>>quick hack*/
   _virtual_CGContainer_notify_object_linkage(CGContainer(this), linkage, object);

   switch (linkage) {
   case EObjectLinkage.ChildAdd:
   case EObjectLinkage.ChildRemove:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_update_end();
      break;
   default:
      break;
   }
}/*CGBox::notify_object_linkage*/

void CGBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObject *object;
   int i;
   TCoord size, pos = 0, a, flex;
   TCoord element_size, container_size, flex_total;

   class:base.notify_attribute_update(attribute, changing);

   if (!CObject(this).child_count() || changing)
      return;

   flex_total = pos = a = 0;
   pos = this->orient == EGBoxOrientType.horizontal ? CGXULElement(this)->x : CGXULElement(this)->y;

   container_size = this->orient == EGBoxOrientType.horizontal ?
                    CGXULElement(this)->width : CGXULElement(this)->height;
   object = CGBox(this).all_child_next(NULL);
   while (object) {
      if (CObjPersistent(object).attribute_default_get(this->orient == EGBoxOrientType.horizontal ?
                                                       ATTRIBUTE<CGXULElement,width> : ATTRIBUTE<CGXULElement,height>)) {
         flex_total += CGXULElement(object)->flex ? CGXULElement(object)->flex : 1;
      }
      else {
         element_size = this->orient == EGBoxOrientType.horizontal ?
                        CGXULElement(object)->width : CGXULElement(object)->height;
         container_size -= element_size;
      }
      object = CGBox(this).all_child_next(object);
   }

   object = CGBox(this).all_child_next(NULL);
   while (object) {
      element_size = 0;
      size = this->orient == EGBoxOrientType.horizontal ? CGXULElement(this)->width : CGXULElement(this)->height;

      if (!CObjPersistent(object).attribute_default_get(this->orient == EGBoxOrientType.horizontal ?
                                                        ATTRIBUTE<CGXULElement,width> : ATTRIBUTE<CGXULElement,height>)) {
         element_size = this->orient == EGBoxOrientType.horizontal ?
                        CGXULElement(object)->width : CGXULElement(object)->height;
      }
      else {
         element_size = 0;
         flex = CGXULElement(object)->flex ? CGXULElement(object)->flex : 1;
         for (i = 0; i < flex; i++) {
            while (a < container_size) {
               a += flex_total;
               element_size++;
            }
            a -= container_size;
         }
      }

      switch (this->orient) {
      case EGBoxOrientType.horizontal:
         CGXULElement(object)->x = pos;
         CGXULElement(object)->y = CGXULElement(this)->y;
         CGXULElement(object)->width = element_size;
         if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,height>)) {
            CGXULElement(object)->height = CGXULElement(this)->height;
         }
         break;
      case EGBoxOrientType.vertical:
         CGXULElement(object)->x = CGXULElement(this)->x;
         CGXULElement(object)->y = pos;
         if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,width>)) {
            CGXULElement(object)->width = CGXULElement(this)->width;
         }
         CGXULElement(object)->height = element_size;
         break;
      }
      CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,x>);
      CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,y>);
      CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(object).attribute_update_end();

      pos += element_size;

      object = CGBox(this).all_child_next(object);
   }
}/*CGBox::notify_attribute_update*/

CObject *CGBox::all_child_next(CObject *child) {
   CObject *object;

   object = child;
   do {
      if (!object) {
         object = CObject(this).child_first();
      }
      else {
         if (!CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
            /* descent into children */
            object = CObject(object).child_first();
         }
         else {
            object = NULL;
         }
         if (!object) {
            object = CObject(CObject(child).parent()).child_next(CObject(child));
         }
         while (!object) {
            object = CObject(child).parent();
            child = object;
            if (object == CObject(this)) {
               return NULL;
            }
            object = CObject(CObject(object).parent()).child_next(CObject(object));
         }
      }

      if (object == NULL)
         return NULL;

      child = object;
   } while (!CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class()));
//   } while (!CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class()) ||
//             CGObject(object)->visibility != EGVisibility.visible);

   return object;
}/*CGBox::all_child_next*/

void CGVBox::CGVBox(void) {
   CGBox(this).CGBox();
}/*CGVBox::CGVBox*/

void CGVBox::new(void) {
   _virtual_CGXULElement_new((CGXULElement *)this);

   CGBox(this)->orient = EGBoxOrientType.vertical;
}/*CGVBox::notify_attribute_update*/

void CGHBox::CGHBox(void) {
   CGBox(this).CGBox();
}/*CGHBox::CGHBox*/

void CGHBox::new(void) {
   _virtual_CGXULElement_new((CGXULElement *)this);

   CGBox(this)->orient = EGBoxOrientType.horizontal;
}/*CGHBox::new*/

void CGStack::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   /*>>>quick hack*/
   _virtual_CGContainer_notify_object_linkage(CGContainer(this), linkage, object);

   switch (linkage) {
   case EObjectLinkage.ChildAdd:
   case EObjectLinkage.ChildRemove:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,width>);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,height>);
      CObjPersistent(this).attribute_update_end();
      break;
   default:
      break;
   }
}/*CGStack::notify_object_linkage*/

void CGStack::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObject *object;

   class:base.notify_attribute_update(attribute, changing);

   if (!CObject(this).child_count() || changing)
      return;

   object = CObject(this).child_first();
   while (object) {
       if (CObjClass_is_derived(&class(CGXULElement), CObject(object).obj_class())) {
          if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,x>)) {
             CGXULElement(object)->x = CGXULElement(this)->x;
          }
          if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,y>)) {
             CGXULElement(object)->y = CGXULElement(this)->y;
          }
          if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,width>)) {
             CGXULElement(object)->width = CGXULElement(this)->width;
          }
          if (CObjPersistent(object).attribute_default_get(ATTRIBUTE<CGXULElement,height>)) {
             CGXULElement(object)->height = CGXULElement(this)->height;
          }
          CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,x>);
          CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,y>);
          CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,width>);
          CObjPersistent(object).attribute_update(ATTRIBUTE<CGXULElement,height>);
          CObjPersistent(object).attribute_update_end();
       }
       object = CObject(this).child_next(CObject(object));
   }
}/*CGStack::notify_attribute_update*/

void CGButton::new(void) {
   class:base.new();

   new(&this->label).CString(NULL);
   new(&this->src).CString(NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGButton,dir>, TRUE);
}/*CGButton::new*/

void CGButton::CGButton(int x, int y, int width, int height, const char *label) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   CString(&this->label).set(label);
}/*CGButton::CGButton*/

void CGButton::delete(void) {
   delete(&this->src);
   delete(&this->label);

   class:base.delete();
}/*CGButton::delete*/

void CGToggleButton::CGToggleButton(int x, int y, int width, int height, char *label, bool toggled) {
   CGButton(this).CGButton(x, y, width, height, label);
   this->toggled = toggled;
}/*CGToggleButton::CGToggleButton*/

void CGCheckBox::CGCheckBox(int x, int y, int width, int height, char *label, bool checked) {
   CGButton(this).CGButton(x, y, width, height, label);
   this->checked = checked;
}/*CGCheckBox::CGCheckBox*/

void CGToggleButton::binding_resolve(void) {
   bool value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      switch (CGXULElement(this)->binding.object.attr.index) {
      case ATTR_INDEX_VALUE:
      default:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                           CGXULElement(this)->binding.object.attr.element,
                                                                                                           CGXULElement(this)->binding.object.attr.index);
         break;
      case ATTR_INDEX_DEFAULT:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);
         break;
      }

      if (this->toggled != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGToggleButton,toggled>);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGToggleButton,toggled>, value ? "TRUE" : "FALSE");
         CObjPersistent(this).attribute_update_end();
      }
   }
}/*CGToggleButton::binding_resolve*/

void CGCheckBox::binding_resolve(void) {
   bool value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      switch (CGXULElement(this)->binding.object.attr.index) {
      case ATTR_INDEX_VALUE:
      default:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                           CGXULElement(this)->binding.object.attr.element,
                                                                                                           CGXULElement(this)->binding.object.attr.index);
         break;
      case ATTR_INDEX_DEFAULT:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);
         break;
      }

      if (this->checked != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGCheckBox,checked>);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGCheckBox,checked>, value ? "TRUE" : "FALSE");
         CObjPersistent(this).attribute_update_end();
      }
   }
}/*CGCheckBox::binding_resolve*/

void CGRadioButton::CGRadioButton(int x, int y, int width, int height, char *label, bool selected) {
   CGButton(this).CGButton(x, y, width, height, label);
   this->selected = selected;
}/*CGRadioButton::CGRadioButton*/

void CGRadioButton::binding_resolve(void) {
   bool value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      switch (CGXULElement(this)->binding.object.attr.index) {
      case ATTR_INDEX_VALUE:
      default:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                           CGXULElement(this)->binding.object.attr.element,
                                                                                                           CGXULElement(this)->binding.object.attr.index);
         break;
      case ATTR_INDEX_DEFAULT:
         value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);
         break;
      }

      if (this->selected != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRadioButton,selected>);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGRadioButton,selected>, value ? "TRUE" : "FALSE");
         CObjPersistent(this).attribute_update_end();
      }
   }
}/*CGRadioButton::binding_resolve*/

void CGColourPicker::new(void) {
   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGColourPicker,native_picker>, TRUE);
}/*CGColourPicker::new*/

void CGColourPicker::CGColourPicker(int x, int y, int width, int height, TGColour colour) {
   CGButton(this).CGButton(x, y, width, height, NULL);
   this->colour = colour;
}/*CGColourPicker::CGColourPicker*/

void CGColourPicker::binding_resolve(void) {
   TGColour value;
   bool is_default;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute &&
       !CXPath(&CGXULElement(this)->binding).assignment()) {
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get(CGXULElement(this)->binding.object.attr.attribute,
                                                                              CGXULElement(this)->binding.object.attr.element,
                                                                              CGXULElement(this)->binding.object.attr.index,
                                                                              &ATTRIBUTE:type<TGColour>, (void *)&value);
      is_default = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);

      if (this->colour != value || CGXULElement(this)->isdefault != is_default) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGColourPicker,colour>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,isdefault>);
         CObjPersistent(this).attribute_set(ATTRIBUTE<CGColourPicker,colour>, -1, ATTR_INDEX_VALUE, &ATTRIBUTE:type<TGColour>, &value);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,isdefault>, is_default ? "TRUE" : "FALSE");
         CObjPersistent(this).attribute_update_end();
      }
   }
}/*CGColourPicker::binding_resolve*/

void CGDatePickerButton::CGDatePickerButton(int x, int y, int width, int height, TUNIXTime time) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   this->time = time;
}/*CGDatePickerButton::CGDatePickerButton*/

void CGDatePickerButton::binding_resolve(void) {
   TUNIXTime time = 0;
   CString label;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      new(&label).CString(NULL);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get(CGXULElement(this)->binding.object.attr.attribute,
                                                                              CGXULElement(this)->binding.object.attr.element,
                                                                              CGXULElement(this)->binding.object.attr.index,
                                                                              &ATTRIBUTE:type<TUNIXTime>, (void *)&time);
      if (this->time != time) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGDatePickerButton,time>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGButton,label>);      
         CObjPersistent(this).attribute_set(ATTRIBUTE<CGDatePickerButton,time>, -1, -1, &ATTRIBUTE:type<TUNIXTime>, (void *)&time);
         CObjPersistent(this).attribute_set(ATTRIBUTE<CGButton,label>, -1, -1, &ATTRIBUTE:type<TUNIXTime>, (void *)&time);      
         CObjPersistent(this).attribute_update_end();
      }
      delete(&label);
   }
}/*CGDatePickerButton::binding_resolve*/

void CGSlider::CGSlider(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);

   this->minimum = 0;
   this->maximum = 100;
}/*CGSlider::CGSlider*/

void CGSlider::binding_resolve(void) {
   int value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      value =
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                            CGXULElement(this)->binding.object.attr.element,
                                                                                            CGXULElement(this)->binding.object.attr.index);

      if (this->position != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGSlider,position>);
         CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGSlider,position>, value);
         CObjPersistent(this).attribute_update_end();
      }
   }
}/*CGSlider::binding_resolve*/

void CGTextLabel::new(void) {
   new(&this->value).CString(NULL);

   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextLabel,colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextLabel,background_colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextLabel,font_size>, TRUE);
}/*CGTextLabel::new*/

void CGTextLabel::CGTextLabel(int x, int y, int width, int height, const char *text) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   CString(&this->value).set(text);
}/*CGTextLabel::CGTextLabel*/

void CGTextLabel::delete(void) {
   class:base.delete();

   delete(&this->value);
}/*CGTextLabel::delete*/

void CGTextLabel::binding_resolve(void) {
   CString value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      new(&value).CString(NULL);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                            CGXULElement(this)->binding.object.attr.element,
                                                                                            CGXULElement(this)->binding.object.attr.index,
                                                                                            &value);
      if (CString(&this->value).strcmp(CString(&value).string()) != 0) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGTextLabel,value>);
         CObjPersistent(this).attribute_set_string(ATTRIBUTE<CGTextLabel,value>, &value);
         CObjPersistent(this).attribute_update_end();
      }

      delete(&value);
   }
}/*CGTextLabel::binding_resolve*/

void CGProgressMeter::binding_resolve(void) {
   int value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      value =
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                   CGXULElement(this)->binding.object.attr.element,
                                                                                                   CGXULElement(this)->binding.object.attr.index);

      if (this->value != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGProgressMeter,value>);
         CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGProgressMeter,value>, value);
         CObjPersistent(this).attribute_update_end();
      }
   }
}/*CGProgressMeter::binding_resolve*/

void CGDatePicker::CGDatePicker(int x, int y, int width, int height, TUNIXTime time) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   this->time = time;
}/*CGDatePicker::CGDatePicker*/

void CGDatePicker::binding_resolve(void) {
   TUNIXTime time = 0;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get(CGXULElement(this)->binding.object.attr.attribute,
                                                                              CGXULElement(this)->binding.object.attr.element,
                                                                              CGXULElement(this)->binding.object.attr.index,
                                                                              &ATTRIBUTE:type<TUNIXTime>, (void *)&time);
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGDatePicker,time>);
      CObjPersistent(this).attribute_set(ATTRIBUTE<CGDatePicker,time>, -1, -1, &ATTRIBUTE:type<TUNIXTime>, (void *)&time);
      CObjPersistent(this).attribute_update_end();
   }
}/*CGDatePicker::binding_resolve*/

void CGStatusBarPanel::CGStatusBarPanel(int x, int y, int width, int height, const char *text) {
    CGTextLabel(this).CGTextLabel(x, y, width, height, text);
}/*CGStatusBarPanel::CGStatusBarPanel*/

#if !OS_PalmOS

void CGTextBox::new(void) {
   class:base.new();

   new(&this->value).CString(NULL);
   CString(&this->value).encoding_set(EStringEncoding.UTF16);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,type>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,background_colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,dir>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,maxlength>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,multiline>, TRUE);
}/*CGTextBox::new*/

void CGTextBox::CGTextBox(int x, int y, int width, int height, char *text) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   CString(&this->value).set(text);
   CString(&this->value).encoding_set(EStringEncoding.UTF16);
}/*CGTextBox::CGTextBox*/

void CGTextBox::delete(void) {
   class:base.delete();

   delete(&this->value);
}/*CGTextBox::delete*/

void CGTextBox::binding_resolve(void) {
   CString value;
   bool is_default;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      new(&value).CString(NULL);
      CString(&value).encoding_set(EStringEncoding.UTF16);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                   CGXULElement(this)->binding.object.attr.element,
                                                                                                   CGXULElement(this)->binding.object.attr.index,
                                                                                                   &value);
      CString(&value).encoding_set(EStringEncoding.UTF16);
      is_default = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);

       /* resolve binding only if value has changed */
      if ((is_default != CGXULElement(this)->isdefault ||
          !CString(&this->value).match(&value)) //CString(&this->value).strcmp(CString(&value).string()) != 0)
          && !this->modified) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGTextBox,value>);
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,isdefault>);
         CObjPersistent(this).attribute_set_string(ATTRIBUTE<CGTextBox,value>, &value);
         CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,isdefault>, is_default ? "TRUE" : "FALSE");

         if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGTextBox,colour>)) {
             CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,colour>, TRUE);
         }
         if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGTextBox,background_colour>)) {
             CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGTextBox,background_colour>, TRUE);
         }

         CObjPersistent(this).attribute_update_end();
      }

      delete(&value);
   }
}/*CGTextBox::binding_resolve*/

void CGScrollBar::new(void) {
   class:base.new();
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGScrollBar,update_type>,TRUE);
}/*CGScrollBar::new*/

void CGScrollBar::CGScrollBar(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}/*CGScrollBar::CGScrollBar*/

void CGScrollBar::binding_resolve(void) {
   int value;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                          CGXULElement(this)->binding.object.attr.element,
                                                                                                          CGXULElement(this)->binding.object.attr.index);
      if (this->curpos != value) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
         CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGScrollBar,curpos>, value);
         CObjPersistent(this).attribute_update_end();
      }
   }
}/*CGScrollBar::binding_resolve*/

void CGScrolledArea::new(void) {
    class:base.new();
}/*CGScrolledArea::new*/

void CGScrolledArea::CGScrolledArea(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}/*CGScrolledArea::CGScrolledArea*/

void CGScrolledArea::delete(void) {
    class:base.delete();
}/*CGScrolledArea::delete*/

void CGScrolledArea::NATIVE_allocate(CGLayout *layout) {
   CGObject(this)->native_object = (void *)-1;
}/*CGScrolledArea::NATIVE_allocate*/

void CGScrolledArea::NATIVE_release(CGLayout *layout) {
   CGObject(this)->native_object = NULL;
}/*CGScrolledArea::NATIVE_release*/

void CGListItem::new(void) {
   class:base.new();

   new(&this->label).CString(NULL);
   new(&this->value).CString(NULL);

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,value>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,value_int>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,disabled>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,id>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,accesskey>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,checked>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,isdefault>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGListItem,type>, TRUE);
}/*CGListItem::new*/

void CGListItem::CGListItem(const char *label, int id, bool disabled, bool checked) {
   CString(&this->label).set(label);
   this->id = id;
   this->disabled = disabled;
   this->checked = checked;
}/*CGListItem::CGListItem*/

void CGListItem::delete(void) {
   delete(&this->value);
   delete(&this->label);

   class:base.delete();
}/*CGListItem::delete*/

void CGMenuItem::new(void) {
   new(&this->acceltext).CString(NULL);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuItem,acceltext>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuItem,accelerator>, TRUE);

   class:base.new();
}/*CGMenuItem::new*/

void CGMenuItem::CGMenuItem(const char *label, const char *acceltext, int id, bool disabled, bool checked) {
   CGListItem(this).CGListItem(label, id, disabled, checked);
   if (acceltext) {
      CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuItem,acceltext>, FALSE);
      CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGMenuItem,acceltext>, acceltext);
   }
}/*CGMenuItem::CGMenuItem*/

void CGMenuItem::delete(void) {
   class:base.delete();

   delete(&this->acceltext);
}/*CGMenuItem::delete*/

void CGMenuSeparator::CGMenuSeparator(void) {
}/*CGMenuSeparator::CGMenuSeparator*/

void CGListMenuPopup::CGListMenuPopup(void) {
}/*CGListMenuPopup::CGListMenuPopup*/

void CGMenuList::new(void) {
   class:base.new();

   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuList,colour>, TRUE);
   CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuList,background_colour>, TRUE);

   new(&this->menupopup).CGListMenuPopup();
   CObject(&this->menupopup).parent_set(CObject(this));
}/*CGMenuList::new*/

void CGMenuList::CGMenuList(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}/*CGMenuList::CGMenuList*/

void CGMenuList::delete(void) {
   delete(&this->menupopup);

   class:base.delete();
}/*CGMenuList::delete*/

void CGMenuList::binding_resolve(void) {
   int i, index = -2, int_value;
   bool is_default;
   CString value;
   CGMenuItem *item;
   int int_binding = FALSE;
   
   item = CGMenuItem(CObject(&this->menupopup).child_first());
   while (item) {
      if (!CObjPersistent(item).attribute_default_get(ATTRIBUTE<CGListItem,value_int>)) {
         int_binding = TRUE;
      }
      item = CGMenuItem(CObject(&this->menupopup).child_next(CObject(item)));
   }
 
   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      if (int_binding || !CObject(&this->menupopup).child_count() ||
          CGXULElement(this)->binding.object.attr.attribute->data_type == &ATTRIBUTE:type<int>) {
         int_value = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                    CGXULElement(this)->binding.object.attr.element,
                                                                                                    CGXULElement(this)->binding.object.attr.index);
         if (!CObject(&this->menupopup).child_count()) {
            index = int_value;
         }
         else {
            i = 0;
            item = CGMenuItem(CObject(&this->menupopup).child_first());
            while (item) {
               if (CGListItem(item)->value_int == int_value) {
                  index = i;
                  break;
               }
               i++;
               item = CGMenuItem(CObject(&this->menupopup).child_next(CObject(item)));
            }
         }
      }
      else {
         new(&value).CString(NULL);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                               CGXULElement(this)->binding.object.attr.element,
                                                                                               CGXULElement(this)->binding.object.attr.index,
                                                                                               &value);
         i = 0;
         item = CGMenuItem(CObject(&this->menupopup).child_first());
         while (item) {
            if (CString(&CGListItem(item)->value).strcmp(CString(&value).string()) == 0) {
               index = i;
               break;
            }
            i++;
            item = CGMenuItem(CObject(&this->menupopup).child_next(CObject(item)));
         }
         delete(&value);
      }
      is_default = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);

      if (index != -2) {
         if (is_default != CGXULElement(this)->isdefault ||
             index != this->item) {
            CObjPersistent(this).attribute_update(ATTRIBUTE<CGMenuList,item>);
            CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,isdefault>);
            CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGMenuList,item>, ATTR_ELEMENT_VALUE, ATTR_INDEX_VALUE, index);
            CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,isdefault>, is_default ? "TRUE" : "FALSE");
            CObjPersistent(this).attribute_update_end();
         }
      }

      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGMenuList,colour>)) {
         CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuList,colour>, TRUE);
      }
      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGMenuList,background_colour>)) {
         CObjPersistent(this).attribute_default_set(ATTRIBUTE<CGMenuList,background_colour>, TRUE);
      }
#if 0
      /*>>>find method of displaying default status for Win32*/
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,disabled>);
      CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,disabled>, is_default ? "TRUE" : "FALSE");
      CObjPersistent(this).attribute_update_end();
#endif
   }
}/*CGMenuList::binding_resolve*/

void CGListBox::CGListBox(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}/*CGListBox::CGListBox*/

void CGListBox::binding_resolve(void) {
   int i, index = -1;
   bool is_default;
   CString value;
   CGListItem *item;

   if (CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) {
      if (CGXULElement(this)->binding.object.attr.attribute->type == PT_AttributeEnum ||
          CGXULElement(this)->binding.object.attr.attribute->data_type == &ATTRIBUTE:type<int>) {
         index = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                           CGXULElement(this)->binding.object.attr.element,
                                                                                                           CGXULElement(this)->binding.object.attr.index);
      }
      else {
         new(&value).CString(NULL);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_get_string_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                               CGXULElement(this)->binding.object.attr.element,
                                                                                               CGXULElement(this)->binding.object.attr.index,
                                                                                               &value);
         i = 0;
         item = CGListItem(CObject(this).child_first());
         while (item) {
            if (CString(&CGListItem(item)->label).strcmp(CString(&value).string()) == 0) {
               index = i;
               break;
            }
            i++;
            item = CGListItem(CObject(this).child_next(CObject(item)));
         }
         delete(&value);
      }
      is_default = CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_get(CGXULElement(this)->binding.object.attr.attribute);

      if (index != -1) {
         if (is_default != CGXULElement(this)->isdefault ||
             index != this->item) {
            CObjPersistent(this).attribute_update(ATTRIBUTE<CGListBox,item>);
            CObjPersistent(this).attribute_set_int_element_index(ATTRIBUTE<CGListBox,item>, ATTR_ELEMENT_VALUE, ATTR_INDEX_VALUE, index);
            CObjPersistent(this).attribute_update_end();
         }
      }
#if 0
      /*>>>find method of displaying default status for Win32*/
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGXULElement,disabled>);
      CObjPersistent(this).attribute_set_text(ATTRIBUTE<CGXULElement,disabled>, is_default ? "TRUE" : "FALSE");
      CObjPersistent(this).attribute_update_end();
#endif
   }
}/*CGMenuList::binding_resolve*/

void CObjClientGTree::CObjClientGTree(CObjServer *obj_server, CObject *host,
                                      CObjPersistent *object_root, CGTree *gtree) {
   this->gtree = gtree;
   CObjClient(this).CObjClient(obj_server, host);
   CObjClient(this).object_root_add(object_root);
}/*CObjClientGTree::CObjClientGTree*/

void CGTreeCols::CGTreeCols(void) {
   CGTreeCol *tree_col;

   tree_col = new.CGTreeCol("Attribute");
   CObject(this).child_add(CObject(tree_col));
   tree_col = new.CGTreeCol("Value");
   CObject(this).child_add(CObject(tree_col));
}/*CGTreeCols::CGTreeCols*/

void CGTreeCol::new(void) {
   new(&this->label).CString(NULL);
}/*CGTreeCol::new*/

void CGTreeCol::CGTreeCol(const char *label) {
   CString(&this->label).set(label);
}/*CGTreeCol::CGTreeCol*/

void CGTreeCol::delete(void) {
   delete(&this->label);
}/*CGTreeCol::delete*/

void CGTree::new(void) {
   new(&this->tree_cols).CGTreeCols();

   class:base.new();
}/*CGTree::new*/

void CGTree::CGTree(CObjServer *obj_server, CObjPersistent *object_root, int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
   new(&this->client).CObjClientGTree(obj_server, CObject(this), object_root, this);
}/*CGTree::CGTree*/

void CGTree::delete(void) {
   delete(&this->client);

   delete(&this->tree_cols);

   class:base.delete();
}/*CGTree::delete*/

void CGTree::column_add(const char *label) {
   CGTreeCol *tree_col = new.CGTreeCol(label);
   CObject(&this->tree_cols).child_add(CObject(tree_col));
}/*CGTree::column_add*/

void CGTabBox::CGTabBox(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);
}/*CGTabBox::CGTabBox*/

void CGTabBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   CObject *child;
   TCoord min_width = 0, min_height = 0;
   /*>>>not the right place for this */

   if (!changing) {
      child = CObject(this).child_tree_first();
      while (child) {
         if (CObjClass_is_derived(&class(CGXULElement), CObject(child).obj_class())) {
            min_width = CGXULElement(child)->min_width > min_width ? CGXULElement(child)->min_width : min_width;
            min_height = CGXULElement(child)->min_height > min_height ? CGXULElement(child)->min_height : min_height;
         }
         child = CObject(this).child_tree_next(child);
      }

      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,min_width>)) {
         CGXULElement(this)->min_width  = min_width;
      }
      if (CObjPersistent(this).attribute_default_get(ATTRIBUTE<CGXULElement,min_height>)) {
         CGXULElement(this)->min_height = min_height;
      }
   }

   class:base.notify_attribute_update(attribute, changing);
}/*CGTabBox::notify_attribute_update*/

void CGSplitter::CGSplitter(int x, int y, int width, int height) {
   CGXULElement(this).CGXULElement(x, y, width, height);

   this->position = 220;
}/*CGSplitter::CGSplitter*/

void CGSplitter::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   switch (linkage) {
   case EObjectLinkage.ChildAdd:
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGSplitter,position>);
      CObjPersistent(this).attribute_update_end();
      break;
   default:
      break;
   }
}/*CGSplitter::notify_object_linkage*/

#endif

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/