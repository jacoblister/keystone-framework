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
#include "../file.c"
#include "../objpersistent.c"
#include "../objdataserver.c"
#include "gobject.c"
#include "gpalette.c"
#include "glayout.c" /*>>>implied*/
#include "gprimitive.c"
#include "gwindow.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGLicenseFill : CObject {
 public:
   void CGLicenseFill(void);
   void fill_layout(CGLayout *layout);
};

class CGPropertiesFill : CObject {
 public:
   void CGPropertiesFill(void);
   void fill_layout(CGLayout *layout, CObjServer *obj_server, CObjPersistent *object);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CGLicenseFill::CGLicenseFill(void) {
}/*CGLicenseFill::CGLicenseFill*/

void CGLicenseFill::fill_layout(CGLayout *layout) {
   CGVBox *vbox;
   CGTextLabel *label;
   char text[KEYSTONE_LICENSE_LENGTH];
   char *token;

   CObjPersistent(layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_width>,  FALSE);
   CObjPersistent(layout).attribute_default_set(ATTRIBUTE<CGXULElement,min_height>, FALSE);
   CObjPersistent(layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_width>,  300);
   CObjPersistent(layout).attribute_set_int(ATTRIBUTE<CGXULElement,min_height>, 150);
   CObjPersistent(layout).attribute_default_set(ATTRIBUTE<CGCanvas,colour_background>, TRUE);
//   CGCanvas(layout).colour_background_set(GCOL_NONE);

   vbox = new.CGVBox();
   CObject(layout).child_add(CObject(vbox));

   CFramework(&framework).license_info(text);
   token = strtok(text, "\n");
   while (token) {
      label = new.CGTextLabel(0, 0, 350, 15, token);
      CObject(vbox).child_add(CObject(label));

      token = strtok(NULL, "\n");
   }

   label = new.CGTextLabel(0, 0, 350, 15, "");
   CObject(vbox).child_add(CObject(label));
   label = new.CGTextLabel(0, 0, 350, 15, "Aggregate Components:");
   CObject(vbox).child_add(CObject(label));

   CFramework(&framework).license_components(text);
   token = strtok(text, "\n");
   while (token) {
      label = new.CGTextLabel(0, 0, 350, 15, token);
      CObject(vbox).child_add(CObject(label));

      token = strtok(NULL, "\n");
   }
}/*CGLicenseFill::CGLicenseFill*/

void CGPropertiesFill::CGPropertiesFill(void) {
}/*CGPropertiesFill::CGPropertiesFill*/

void CGPropertiesFill::fill_layout(CGLayout *layout, CObjServer *obj_server, CObjPersistent *object) {
   const TAttribute *attribute;
   CGTextLabel *label;
   CGTextBox *text_box;
   CGVBox *vbox;
   CGHBox *hbox;
   int i = 0;
   ARRAY<const TAttribute *> attr_list;

   CGCanvas(layout).colour_background_set(GCOL_NONE);

   ARRAY(&attr_list).new();
   CObjPersistent(object).attribute_list(&attr_list, TRUE, FALSE, FALSE);

   vbox = new.CGVBox();
   CObject(layout).child_add(CObject(vbox));

   for (i = 0; i < ARRAY(&attr_list).count(); i++) {
      attribute = ARRAY(&attr_list).data()[i];

      hbox = new.CGHBox();
      CGXULElement(hbox)->height = 22;
      CObjPersistent(hbox).attribute_default_set(ATTRIBUTE<CGXULElement,height>, FALSE);
      CObject(vbox).child_add(CObject(hbox));

      label = new.CGTextLabel(0, 0, 120, 20, attribute->name);
      CObject(hbox).child_add(CObject(label));

      text_box = new.CGTextBox(0, 0, 0, 22, "???");
      CObjPersistent(text_box).attribute_default_set(ATTRIBUTE<CGXULElement,width>, TRUE);
      CGXULElement(text_box)->flex = 1;
      CObject(hbox).child_add(CObject(text_box));
      CXPath(&CGXULElement(text_box)->binding).set(object, attribute, ATTR_INDEX_VALUE);
   }

   ARRAY(&attr_list).delete();

   CGLayout(layout).animate(); //>>>do this in layout::show*/
}/*CGPropertiesFill::fill_layout*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/