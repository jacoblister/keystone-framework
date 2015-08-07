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

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

/* GTK Outstanding issues -
Disable keyboard navigation completely, use framework navigation
Keyboard interface for drop list (as for Win32)
Coloured text for textbox fields
Numeric textbox fields
Static bitmap backbuffer has problems
All Windows have menu bars
*/

#include <gtk/gtk.h>

#include "native.h"

#define GTK_OBJ_KEY "fwobject"

/*>>>quick hack to get radio buttons working, will need to rework for radio groups*/
static GtkRadioButton *radio_last;

void CGXULElement::utov(CGCanvas *canvas, TPoint *result) {
   result[0].x = this->x;
   result[0].y = this->y;
   result[1].x = this->x + this->width;
   result[1].y = this->y + this->height;
   CGCanvas(canvas).transform_set_gobject(CGObject(this), FALSE);
   CGCoordSpace(&canvas->coord_space).point_array_utov(2, result);
}/*CGXULElement::utov*/

void CGXULElement::show(bool show) {
   CGLayout *layout;
   CGWindow *window;

   layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   if (!layout && CObjClass_is_derived(&class(CGLayout), CObject(this).obj_class())) {
      layout = CGLayout(this);
   }

   if (!layout)
      return;

   if (CGXULElement(layout) != this && !CGObject(layout)->native_object)
       return;

   /* don't allocate elements unless there is a parent */
   window = CGWindow(CObject(this).parent_find(&class(CGWindow)));
   if (!window)
      return;

   if (show) {
      if (!CGObject(this)->native_object) {
         CGXULElement(this).NATIVE_allocate(layout);
         if (CGObject(this)->native_object && (long)CGObject(this)->native_object  != -1) {
            if (CGObject(this)->visibility) {
               gtk_widget_show(GTK_WIDGET(CGObject(this)->native_object));
            }
            else {
               gtk_widget_hide(GTK_WIDGET(CGObject(this)->native_object));
            }
         }
      }
   }
   else {
      if (CGObject(this)->native_object) {
         CGXULElement(this).NATIVE_release(layout);
      }
   }
}/*CGXULElement::show*/

void CGXULElement::NATIVE_draw(CGCanvas *canvas, TRect *extent, EGObjectDrawMode mode) {
}/*CGXULElement::NATIVE_draw*/

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
         if (visibility) {
            gtk_widget_show(GTK_WIDGET(CGObject(this)->native_object));
         }
         else {
            gtk_widget_hide(GTK_WIDGET(CGObject(this)->native_object));
         }
         gtk_widget_set_sensitive(GTK_WIDGET(CGObject(this)->native_object),
                                  !this->disabled);
      }

      if (CObjClass_is_derived(&class(CGWindow), CObject(this).obj_class())) {
          return;
      }

      if (layout &&
         (!CObjClass_is_derived(&class(CGContainer), CObject(CObject(this).parent()).obj_class()) ||
         (CObjClass_is_derived(&class(CGContainer), CObject(CObject(this).parent()).obj_class()) && CGContainer(CObject(this).parent())->allow_child_repos) ||
          CObjClass_is_derived(&class(CGPacker), CObject(CObject(this).parent()).obj_class()))) {

         CGXULElement(this).utov(CGCanvas(layout), position);
         gtk_widget_set_uposition(GTK_WIDGET(CGObject(this)->native_object),
                                  (int)position[0].x, (int)position[0].y);
         gtk_widget_set_usize(GTK_WIDGET(CGObject(this)->native_object),
                              (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
         CGXULElement(this).extent_set(CGCanvas(layout));
      }
   }
}/*CGXULElement::notify_attribute_update*/

bool CGXULElement::repeat_event(CEvent *event) {
   return TRUE;
}/*CGXULElement::event*/

Local gint CGXULElement_GTK_motion_notify_event(GtkWidget *widget,
                                   GdkEventMotion *event_motion,
                                   gpointer data) {
   /* Fix wrongly positioned motion events to main canvas */
   event_motion->x += CGXULElement(data)->x;
   event_motion->y += CGXULElement(data)->y;
   return FALSE;
}/*CGXULElement_GTK_motion_notify_event*/

Local gint CGXULElement_GTK_signal_key_press(GtkWidget *widget, GdkEventKey *event,
                                             CGXULElement *this) {
   return FALSE;
}/*CGXULElement_GTK_signal_key_press*/

Local gint CGXULElement_GTK_signal_focus_in_event(GtkWidget *text_edit,
                                                  GdkEventFocus *event,
                                                  CGTextBox *this) {
   CGCanvas *canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
   CGLayout(canvas).key_gselection_set(CGObject(this));
   return FALSE;
}/*CGXULElement_GTK_signal_focus_in_event*/

void CGXULElement::NATIVE_allocate(CGLayout *layout) {
   gtk_widget_set_sensitive(GTK_WIDGET(CGObject(this)->native_object),
                            !this->disabled);
   gtk_signal_connect(GTK_OBJECT(CGObject(this)->native_object), "key_press_event",
                      (GtkSignalFunc) CGXULElement_GTK_signal_key_press, this);
   gtk_signal_connect(GTK_OBJECT(CGObject(this)->native_object), "focus_in_event",
                      (GtkSignalFunc) CGXULElement_GTK_signal_focus_in_event, this);
}/*CGXULElement::NATIVE_allocate*/

void CGXULElement::NATIVE_release(CGLayout *layout) {
   gtk_widget_destroy(GTK_WIDGET(CGObject(this)->native_object));
   CGObject(this)->native_object = NULL;

   if (CObject(this).obj_class() == &class(CGRadioButton)) {
      radio_last = NULL;
   }
}/*CGXULElement::NATIVE_release*/

void CGXULElement::NATIVE_focus_in(void) {
   if (CObject(this).obj_class() == &class(CGMenuList)) {
      gtk_widget_grab_focus(GTK_COMBO(CGObject(this)->native_object)->entry);
   }
   else {
      gtk_widget_grab_focus(GTK_WIDGET(CGObject(this)->native_object));
   }
}/*CGXULElement::NATIVE_focus_in*/

void CGXULElement::NATIVE_focus_out(void) {
    /*>>>nothing for now*/
}/*CGXULElement::NATIVE_focus_out*/

void CGButton::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);

   if (!CGObject(this)->native_object) {
      return;
   }

   gtk_button_set_label(GTK_BUTTON(CGObject(this)->native_object), CString(&this->label).string());
}/*CGButton::notify_attribute_update*/

void CGToggleButton::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);

   if (CGObject(this)->native_object) {
      CGXULElement(this)->updating = TRUE;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CGObject(this)->native_object),
                                   this->toggled);
      CGXULElement(this)->updating = FALSE;
   }
}/*CGToggleButton::notify_attribute_update*/

void CGCheckBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);

   if (CGObject(this)->native_object) {
      CGXULElement(this)->updating = TRUE;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CGObject(this)->native_object),
                                   this->checked);
      CGXULElement(this)->updating = FALSE;
   }
}/*CGCheckBox::notify_attribute_update*/

void CGRadioButton::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);

   if (CGObject(this)->native_object) {
      CGXULElement(this)->updating = TRUE;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CGObject(this)->native_object),
                                   /*this->selected*/TRUE);
      CGXULElement(this)->updating = FALSE;
   }
}/*CGRadioButton::notify_attribute_update*/

Local gint CGButton_GTK_signal_clicked(GtkWidget *button, CGButton *this) {
   CString assignment;
   TObjectPtr object;

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

   return FALSE;
}/*CGButton_GTK_signal_clicked*/

Local gint CGColourPicker_GTK_signal_expose(GtkWidget *button, GdkEventExpose *event,
                                            CGButton *this) {
   CGCanvas *canvas;
   TGColour colour;
   GdkGC *gc;
/*>>>show none, disabled and inherit as for Win32 */

   canvas = CGCanvas(CObject(this).parent_find(&class(CGCanvas)));
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

   colour = GCOL_CONVERT_RGB(canvas->palette, canvas->paletteB, CGColourPicker(this)->colour);

   gc = gdk_gc_new(GTK_WIDGET(button)->window);
   gdk_rgb_gc_set_foreground(gc, colour);
   gdk_draw_rectangle(GTK_WIDGET(button)->window, gc,
                        TRUE,
                        button->allocation.x + 6, button->allocation.y + 6,
                        button->allocation.width - 12, button->allocation.height - 12);
   gdk_gc_destroy(gc);

   return TRUE;
};

Local gint CGColourPicker_GTK_signal_clicked(GtkWidget *button, CGButton *this) {
   CGDialogChooseColour choose_colour;
   CGLayout *layout = NULL;
   CGWindow *window = NULL;

   if (CXPath(&CGXULElement(this)->binding).assignment()) {
      return CGButton_GTK_signal_clicked(button, this);
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

   return FALSE;
}/*CGColourPicker_GTK_signal_clicked*/

Local gint CGButton_GTK_signal_toggled(GtkWidget *button, CGButton *this) {
   bool toggled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));

   if (CGXULElement(this)->updating)
      return FALSE;

   if (CGXULElement(this)->binding.object.object) {
      switch (CGXULElement(this)->binding.object.attr.index) {
      case ATTR_INDEX_DEFAULT:
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute, toggled);
         break;
      case ATTR_INDEX_VALUE:
         default:
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_text_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                    CGXULElement(this)->binding.object.attr.element,
                                                                                                    CGXULElement(this)->binding.object.attr.index,
                                                                                                    toggled ? "TRUE" : "FALSE");
         break;
         }
      /*>>>cheating, out of order */
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
      if (CObject(this).obj_class() == &class(CGToggleButton)) {
         CGToggleButton(this)->toggled = toggled;
      }
      if (CObject(this).obj_class() == &class(CGCheckBox)) {
         CGCheckBox(this)->checked = toggled;
      }
      else if (CObject(this).obj_class() == &class(CGRadioButton)) {
         CGRadioButton(this)->selected = toggled;
      }
   }
   else {
      if (CObject(this).obj_class() == &class(CGToggleButton)) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGToggleButton,toggled>);
         CGToggleButton(this)->toggled = toggled;
         CObjPersistent(this).attribute_update_end();
      }
      if (CObject(this).obj_class() == &class(CGCheckBox)) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGCheckBox,checked>);
         CGCheckBox(this)->checked = toggled;
         CObjPersistent(this).attribute_update_end();
      }
      else if (CObject(this).obj_class() == &class(CGRadioButton)) {
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGRadioButton,selected>);
         CGRadioButton(this)->selected = toggled;
         CObjPersistent(this).attribute_update_end();
      }
   }

   return FALSE;
}/*CGButton_GTK_signal_toggled*/

void CGButton::NATIVE_allocate(CGLayout *layout) {
   GtkWidget *widget;
   TPoint position[2];

   CGXULElement(this).utov(CGCanvas(layout), position);

   if (CObject(this).obj_class() == &class(CGToggleButton)) {
      widget = gtk_toggle_button_new_with_label(CString(&this->label).string());
      gtk_signal_connect(GTK_OBJECT(widget), "toggled",
                         (GtkSignalFunc) CGButton_GTK_signal_toggled, this);
      gtk_widget_set_usize(widget,
                           (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   }
   else if (CObject(this).obj_class() == &class(CGCheckBox)) {
      widget = gtk_check_button_new_with_label(CString(&this->label).string());
      gtk_signal_connect(GTK_OBJECT(widget), "toggled",
                         (GtkSignalFunc) CGButton_GTK_signal_toggled, this);
      gtk_widget_set_usize(widget,
                           (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   }
   else if (CObject(this).obj_class() == &class(CGRadioButton)) {
      widget = gtk_radio_button_new_with_label_from_widget(radio_last, CString(&this->label).string());
      radio_last = GTK_RADIO_BUTTON(widget);
      gtk_signal_connect(GTK_OBJECT(widget), "toggled",
                         (GtkSignalFunc) CGButton_GTK_signal_toggled, this);
      gtk_widget_set_usize(widget,
                           (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   }
   else {
      widget = gtk_button_new_with_label(CString(&this->label).string());
      if (CObject(this).obj_class() == &class(CGColourPicker)) {
         gtk_signal_connect_after(GTK_OBJECT(widget), "expose_event",
                                  (GtkSignalFunc) CGColourPicker_GTK_signal_expose, this);
         gtk_signal_connect(GTK_OBJECT(widget), "clicked",
                            (GtkSignalFunc) CGColourPicker_GTK_signal_clicked, this);
      }
      else {
         gtk_signal_connect(GTK_OBJECT(widget), "clicked",
                            (GtkSignalFunc) CGButton_GTK_signal_clicked, this);
      }
      gtk_widget_set_usize(widget,
                           (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   }
   gtk_widget_show(widget);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);
//   gtk_signal_connect(GTK_OBJECT(widget), "key_press_event",
//                      (GtkSignalFunc) CGXULElement_GTK_signal_key_press, this);

   CGObject(this)->native_object = (void *)widget;

   class:base.NATIVE_allocate(layout);
}/*CGButton::NATIVE_allocate*/

void CGColourPicker::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
}/*CGColourPicker::notify_attribute_update*/

void CGSlider::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   GtkAdjustment *adjust;
   GtkWidget *widget = (GtkWidget *)CGObject(this)->native_object;

   if (widget) {
      adjust = gtk_range_get_adjustment(GTK_RANGE(widget));
      adjust->value = this->maximum - this->position;
      gtk_range_set_adjustment(GTK_RANGE(widget), adjust);
   }

   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
}/*CGSlider::notify_attribute_update*/

Local void CGSlider_GTK_signal_value_changed(GtkAdjustment *adjustment,
                                             CGSlider *this) {
   if (CGXULElement(this)->binding.object.object) {
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                  CGXULElement(this)->binding.object.attr.element,
                                                                                                  CGXULElement(this)->binding.object.attr.index,
                                                                                                  CGSlider(this)->maximum - (int)adjustment->value);
      /*>>>cheating, out of order */
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
   }
   else {
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGSlider,position>);
      CGSlider(this)->position = CGSlider(this)->maximum - (int)adjustment->value;
      CObjPersistent(this).attribute_update_end();
   }
}/*CGSlider_GTK_signal_value_changed*/

void CGSlider::NATIVE_allocate(CGLayout *layout) {
   GtkAdjustment *adjust;
   GtkWidget *widget;
   TPoint position[2];

   adjust =
      GTK_ADJUSTMENT(gtk_adjustment_new(this->maximum - this->position,
                                        this->minimum,
                                        this->maximum,
                                        1, 1, 0));
   CGXULElement(this).utov(CGCanvas(layout), position);

   widget = gtk_vscale_new(adjust);
   gtk_scale_set_draw_value(GTK_SCALE(widget), FALSE);
   gtk_widget_set_usize(widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   gtk_signal_connect(GTK_OBJECT(adjust), "value_changed",
                      GTK_SIGNAL_FUNC(CGSlider_GTK_signal_value_changed), this);
   gtk_widget_show(widget);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);

   CGObject(this)->native_object = (void *)widget;

   class:base.NATIVE_allocate(layout);
}/*CGSlider::NATIVE_allocate*/

void CGTextLabel::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
    /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);

   if (!changing && CGObject(this)->native_object) {
      if (CObject(this).obj_class() == &class(CGStatusBarPanel)) {
         gtk_statusbar_pop(GTK_STATUSBAR(CGObject(this)->native_object), 0);
         gtk_statusbar_push(GTK_STATUSBAR(CGObject(this)->native_object), 0, CString(&this->value).string());
      }
      else {
         gtk_label_set_text(GTK_LABEL(CGObject(this)->native_object), CString(&this->value).string());
      }
   }
}/*CGTextLabel::notify_attribute_update*/

void CGTextLabel::NATIVE_allocate(CGLayout *layout) {
   GtkWidget *widget;
   TPoint position[2];

   if (CObject(this).obj_class() == &class(CGStatusBarPanel)) {
      widget = gtk_statusbar_new();
      gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(widget), FALSE);
      gtk_statusbar_push(GTK_STATUSBAR(widget), 0, CString(&this->value).string());
   }
   else {
      widget = gtk_label_new(CString(&this->value).string());
   /*>>>?set justify not working*/
//   gtk_label_set_justify(GTK_LABEL(widget), GTK_JUSTIFY_LEFT);
      gtk_misc_set_alignment(GTK_MISC(widget), 0.0, 0.5);
   }

   CGXULElement(this).utov(CGCanvas(layout), position);

   gtk_widget_set_usize(widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);
   gtk_widget_show(widget);

   CGObject(this)->native_object = (void *)widget;

   class:base.NATIVE_allocate(layout);
}/*CGTextLabel:NATIVE_allocate*/

void CGDatePicker::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
   if (!changing) {
   }
}/*CGDatePicker::notify_attribute_update*/

void CGDatePicker::NATIVE_allocate(CGLayout *layout) {
   GtkWidget *widget;
   TPoint position[2];

   CGXULElement(this).utov(CGCanvas(layout), position);

   widget = gtk_button_new_with_label("date picker");
   gtk_widget_set_usize(widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   gtk_widget_show(widget);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);
//   gtk_signal_connect(GTK_OBJECT(widget), "key_press_event",
//                      (GtkSignalFunc) CGXULElement_GTK_signal_key_press, this);

   CGObject(this)->native_object = (void *)widget;

   class:base.NATIVE_allocate(layout);
}/*CGDatePicker::NATIVE_allocate*/

static GtkStyle *gtk_text_box_style[2];

void CGTextBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);

   if (CGObject(this)->native_object && !changing) {
      gtk_entry_set_text(GTK_ENTRY(CGObject(this)->native_object), CString(&this->value).string());
      gtk_widget_set_style(GTK_WIDGET(CGObject(this)->native_object),
                           CGXULElement(this)->isdefault ?
                           gtk_text_box_style[1] : gtk_text_box_style[0]);
      this->modified = FALSE;
   }
}/*CGTextBox::notify_attribute_update*/


Local gint CGTextBox_GTK_signal_focus_in_event(GtkWidget *text_edit,
                                               GdkEventFocus *event,
                                               CGTextBox *this) {
   this->modified = FALSE;

   return FALSE;
}/*CGTextBox_GTK_signal_focus_in_event*/

Local gint CGTextBox_GTK_signal_changed(GtkWidget *text_edit,
                                        CGTextBox *this) {
   this->modified = TRUE;

   return FALSE;
}/*CGTextBox_GTK_signal_changed*/

Local gint CGTextBox_GTK_signal_focus_out_event(GtkWidget *text_edit,
                                                GdkEventFocus *event,
                                                CGTextBox *this) {
   const char *cp;

   if (this->modified) {
      if (CGXULElement(this)->binding.object.object) {
         cp = gtk_entry_get_text(GTK_ENTRY(text_edit));
         while (*cp && *cp == ' ') {
            cp++;
         }

         if (*cp) {
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute,
                                                                                     FALSE);
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_text_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                CGXULElement(this)->binding.object.attr.element,
                                                                                                CGXULElement(this)->binding.object.attr.index,
                                                                                                cp);
         }
         else {
            /* Set attribute to default */
            CObjPersistent(CGXULElement(this)->binding.object.object).attribute_default_set(CGXULElement(this)->binding.object.attr.attribute,
                                                                                     TRUE);
         }
         /*>>>cheating, out of order */
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
         CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
      }
      else {
         CString(&this->value).set(gtk_entry_get_text(GTK_ENTRY(text_edit)));
         CObjPersistent(this).attribute_update(ATTRIBUTE<CGTextBox,value>);
         CObjPersistent(this).attribute_update_end();
      }
      this->modified = FALSE;
   }

   return FALSE;
}/*CGTextBox_GTK_signal_focus_out_event*/

void CGTextBox::NATIVE_allocate(CGLayout *layout) {
   GtkWidget *widget;
   TPoint position[2];

   widget = gtk_entry_new();

   CGXULElement(this).utov(CGCanvas(layout), position);

   gtk_widget_set_usize(widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   gtk_signal_connect(GTK_OBJECT(widget), "focus_in_event",
                      (GtkSignalFunc) CGTextBox_GTK_signal_focus_in_event, this);
   gtk_signal_connect(GTK_OBJECT(widget), "changed",
                      (GtkSignalFunc) CGTextBox_GTK_signal_changed, this);
   gtk_signal_connect(GTK_OBJECT(widget), "focus_out_event",
                      (GtkSignalFunc) CGTextBox_GTK_signal_focus_out_event, this);
   gtk_signal_connect(GTK_OBJECT(widget),
                      "motion_notify_event",
                      (GtkSignalFunc) CGXULElement_GTK_motion_notify_event, this);

   gtk_entry_set_text(GTK_ENTRY(widget), CString(&this->value).string());
   if (this->type == EGTextBoxType.password) {
      gtk_entry_set_visibility(GTK_ENTRY(widget), FALSE);
   }

   if (this->maxlength != -1) {
      gtk_entry_set_max_length(GTK_ENTRY(widget), this->maxlength);
   }
   gtk_widget_show(widget);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);

   CGObject(this)->native_object = (void *)widget;

   if (!gtk_text_box_style[0]) {
      gtk_text_box_style[0] = gtk_widget_get_style(widget);
      gtk_text_box_style[1] = gtk_style_copy(gtk_text_box_style[0]);

      gtk_text_box_style[1]->text[GTK_STATE_NORMAL].red   = 47050;
      gtk_text_box_style[1]->text[GTK_STATE_NORMAL].green = 47050;
      gtk_text_box_style[1]->text[GTK_STATE_NORMAL].blue  = 47050;
   }

   class:base.NATIVE_allocate(layout);
}/*CGTextBox::NATIVE_allocate*/

void CGTextBox::NATIVE_release(CGLayout *layout) {
   /*>>>force set of current contents */

   class:base.NATIVE_release(layout);
}/*CGTextBox::NATIVE_release*/

Local void CGScrollBar_GTK_signal_value_changed(GtkAdjustment *adjustment,
                                                CGScrollBar *this) {
   if (CGXULElement(this)->binding.object.object) {
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_set_int_element_index(CGXULElement(this)->binding.object.attr.attribute,
                                                                                                  CGXULElement(this)->binding.object.attr.element,
                                                                                                  CGXULElement(this)->binding.object.attr.index,
                                                                                                  (int)adjustment->value);
      /*>>>cheating, out of order */
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
   }
   else {
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGScrollBar,curpos>);
      CGScrollBar(this)->curpos = (int)adjustment->value;
      CObjPersistent(this).attribute_update_end();
   }
}/*CGScrollBar_GTK_signal_value_changed*/

void CGScrollBar::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   GtkAdjustment *adjust;
   GtkWidget *widget = (GtkWidget *)CGObject(this)->native_object;

   if (!changing && widget) {
      adjust = gtk_range_get_adjustment(GTK_RANGE(widget));
      adjust->lower = 0;
      adjust->upper = this->range;
      adjust->page_size = this->range - this->maxpos;
      adjust->value = this->curpos;
      gtk_range_set_adjustment(GTK_RANGE(widget), adjust);
   }

   class:base.notify_attribute_update(attribute, changing);
}/*CGScrollBar::notify_attribute_update*/

void CGScrollBar::NATIVE_allocate(CGLayout *layout) {
   GtkAdjustment *adjust;
   GtkWidget *widget;
   TPoint position[2];

   adjust =
      GTK_ADJUSTMENT(gtk_adjustment_new(this->curpos,
                                        0, this->range,
                                        1, 1, this->range - this->maxpos));
   CGXULElement(this).utov(CGCanvas(layout), position);

   if (this->orient == EGBoxOrientType.vertical) {
      widget = gtk_vscrollbar_new(adjust);
   }
   else {
      widget = gtk_hscrollbar_new(adjust);
   }

   gtk_widget_set_usize(widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   gtk_signal_connect(GTK_OBJECT(adjust), "value_changed",
                      GTK_SIGNAL_FUNC(CGScrollBar_GTK_signal_value_changed), this);
   gtk_widget_show(widget);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);

   CGObject(this)->native_object = (void *)widget;

   class:base.NATIVE_allocate(layout);
}/*CGScrollBar::NATIVE_allocate*/

void CGScrolledArea::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   CGScrolledArea(this).notify_attribute_update(NULL, FALSE);
}/*CGScrolledArea::notify_object_linkage*/

void CGScrolledArea::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   TPoint position[2];
   CGLayout *child_layout, *layout;

   layout = CGLayout(CObject(this).parent_find(&class(CGLayout)));
   child_layout = CGLayout(CObject(this).child_first());

   if (layout && child_layout && CGObject(child_layout)->native_object) {
      CGXULElement(this).utov(CGCanvas(layout), position);
      gtk_widget_set_uposition(GTK_WIDGET(CGObject(child_layout)->native_object),
                               (int)position[0].x, (int)position[0].y);
      gtk_widget_set_usize(GTK_WIDGET(CGObject(child_layout)->native_object),
                           (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   }

   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
}/*CGScrolledArea::notify_attribute_update*/

void CGListBox::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
#if 0
   GtkCombo *combo;
   const char *value;

   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);

   if (!changing && CGObject(this)->native_object) {
      CGXULElement(this)->updating = TRUE;
      combo = GTK_COMBO(CGObject(this)->native_object);
      value = g_list_nth_data(this->strings, this->item);
      gtk_entry_set_text(GTK_ENTRY(combo->entry), value);

      CGXULElement(this)->updating = FALSE;
   }
#endif
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);
}/*CGListBox:notify_attribute_update*/

void CGListBox::NATIVE_allocate(CGLayout *layout) {
   GtkWidget *widget;
   TPoint position[2];
   CGListItem *item;
   GtkListItem *gtk_item;

   widget = gtk_list_new();

   CGXULElement(this).utov(CGCanvas(layout), position);

   gtk_widget_set_usize(widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   gtk_widget_show(widget);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);

   CGObject(this)->native_object = (void *)widget;

   this->items = NULL; /*>>>cheap, will it leak?*/
   item = CGListItem(CObject(this).child_first());
   while (item) {
      gtk_item = GTK_LIST_ITEM(gtk_list_item_new_with_label(CString(&item->label).string()));
      gtk_widget_show(GTK_WIDGET(gtk_item));
      this->items = g_list_append(this->items, gtk_item);

      item = CGListItem(CObject(this).child_next(CObject(item)));
   }
   gtk_list_append_items(GTK_LIST(widget), this->items);

   class:base.NATIVE_allocate(layout);
}/*CGListBox::NATIVE_allocate*/

void CGMenuList::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   GtkCombo *combo;
   const char *value;

   /*>>>quick hack*/
   class:base.notify_attribute_update(attribute, changing);
   _virtual_CObjPersistent_notify_attribute_update(CObjPersistent(this), attribute, changing);

   if (!changing && CGObject(this)->native_object) {
      CGXULElement(this)->updating = TRUE;
      combo = GTK_COMBO(CGObject(this)->native_object);
      value = g_list_nth_data(this->strings, this->item);
      gtk_entry_set_text(GTK_ENTRY(combo->entry), value);

      CGXULElement(this)->updating = FALSE;
   }
}/*CGMenuList:notify_attribute_update*/

void CGMenuList::NATIVE_list_set(int enum_count, const char **enum_name) {
   int i;
   this->strings = NULL;

   CGXULElement(this)->updating = TRUE;

   for (i = 0; i < enum_count; i++) {
      this->strings = g_list_append(this->strings, (char *)enum_name[i]);
   }
   gtk_combo_set_popdown_strings(GTK_COMBO(CGObject(this)->native_object),
                                 this->strings);

   CGXULElement(this)->updating = FALSE;
}/*CGMenuList::NATIVE_list_set*/

Local gint CGMenuList_GTK_signal_select_child(GtkWidget *list,
                                              GtkWidget *child,
                                              CGMenuList *this) {
   int index = gtk_list_child_position(GTK_LIST(list), child);
   CGMenuItem *item;

   if (CGXULElement(this)->updating)
      return FALSE;

   if (CGXULElement(this)->binding.object.object) {
      item = CGMenuItem(CObject(&this->menupopup).child_n(index));
      if (!CObjPersistent(item).attribute_default_get(ATTRIBUTE<CGListItem,value_int>) ||
          CGXULElement(this)->binding.object.attr.attribute->data_type == &ATTRIBUTE:type<int> || !item) {
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
      /*>>>cheating, out of order */
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update(CGXULElement(this)->binding.object.attr.attribute);
      CObjPersistent(CGXULElement(this)->binding.object.object).attribute_update_end();
   }
   else {
      CObjPersistent(this).attribute_update(ATTRIBUTE<CGMenuList,item>);
      this->item = index;
      CObjPersistent(this).attribute_update_end();
   }

   return FALSE;
}/*CGMenuList_GTK_signal_select_child*/

void CGMenuList::NATIVE_allocate(CGLayout *layout) {
   GtkWidget *widget;
   TPoint position[2];
   CGMenuItem *item;

   widget = gtk_combo_new();

   CGXULElement(this).utov(CGCanvas(layout), position);

   gtk_widget_set_usize(widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   gtk_signal_connect(GTK_OBJECT(GTK_COMBO(widget)->list), "select_child",
                      (GtkSignalFunc) CGMenuList_GTK_signal_select_child, this);
   gtk_signal_connect(GTK_OBJECT(GTK_COMBO(widget)->entry),
                      "motion_notify_event",
                      (GtkSignalFunc) CGXULElement_GTK_motion_notify_event, this);
   gtk_editable_set_editable(GTK_EDITABLE(GTK_COMBO(widget)->entry), FALSE);
   gtk_widget_show(widget);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);

   CGObject(this)->native_object = (void *)widget;

   if ((CGXULElement(this)->binding.object.object && CGXULElement(this)->binding.object.attr.attribute) &&
       CGXULElement(this)->binding.object.attr.attribute->type == PT_AttributeEnum &&
       !CObject(&this->menupopup).child_count()) {
      CGMenuList(this).NATIVE_list_set(CGXULElement(this)->binding.object.attr.attribute->enum_count,
                                       CGXULElement(this)->binding.object.attr.attribute->enum_name);
   }
   else {
      CGXULElement(this)->updating = TRUE;
      this->strings = NULL;

      item = CGMenuItem(CObject(&this->menupopup).child_first());
      while (item) {
         this->strings = g_list_append(this->strings, (char *)CString(&CGListItem(item)->label).string());
         item = CGMenuItem(CObject(&this->menupopup).child_next(CObject(item)));
      }

      gtk_combo_set_popdown_strings(GTK_COMBO(CGObject(this)->native_object),
                                    this->strings);

      CGXULElement(this)->updating = FALSE;
   }

   class:base.NATIVE_allocate(layout);
}/*CGMenuList::NATIVE_allocate*/

static GtkStyle *gtk_ctree_style;

void CGTree::NATIVE_XULElement_update_object(void *node, CObjPersistent *object) {
   int i;
   CGTreeCol *tree_col;
   const TAttribute *attribute;
   CString value;
   bool is_default;

   new(&value).CString(NULL);
   i = 0;
   tree_col = CGTreeCol(CObject(&this->tree_cols).child_first());
   while (tree_col) {
      if (i >= 2) {
         attribute = CObjPersistent(object).attribute_find(CString(&tree_col->label).string());
         if (attribute) {
            is_default = CObjPersistent(object).attribute_default_get(attribute);
            gtk_ctree_node_set_cell_style(GTK_CTREE(this->tree_widget),
                                          node, i,
                                          is_default ? gtk_ctree_style : NULL);

            CObjPersistent(object).attribute_get_string(attribute, &value);

            gtk_ctree_node_set_text(GTK_CTREE(this->tree_widget),
                                    node, i, CString(&value).string());

         }
      }
      tree_col = CGTreeCol(CObject(&this->tree_cols).child_next(CObject(tree_col)));
      i++;
   }

   delete(&value);
}/*CGTree::NATIVE_XULElement_update*/

void CGTree::NATIVE_XULElement_fill(void *parent, CObjPersistent *object, int index) {
   GtkCTreeNode *node_parent = parent;
   GtkCTreeNode *node, *attr_root = NULL, *attr_node;
   CObjPersistent *child;
   gchar *item[40];
   ARRAY<const TAttribute *> attribute_list;
   const TAttribute *attribute;
   int i;
   CString value;
   char temp[40];
   GdkColor col;

   /*>>>move elsewhere in this module (eg init phase)*/
   gdk_color_parse("#B0B0B0", &col);
   if (!gtk_ctree_style) {
      gtk_ctree_style = gtk_style_new();

      gtk_ctree_style->fg[GTK_STATE_NORMAL].red   = 47050;
      gtk_ctree_style->fg[GTK_STATE_NORMAL].green = 47050;
      gtk_ctree_style->fg[GTK_STATE_NORMAL].blue  = 47050;
   }

   CLEAR(&item);

   new(&value).CString(NULL);
   ARRAY(&attribute_list).new();
   CObjPersistent(object).attribute_list(&attribute_list, TRUE, TRUE, FALSE);

   node = gtk_ctree_insert_node(GTK_CTREE(this->tree_widget),
                                node_parent, NULL, NULL, 8,
                                NULL, NULL, NULL, NULL, FALSE, FALSE);

   gtk_ctree_node_set_text(GTK_CTREE(this->tree_widget),
                           node, 0, (char *)CObjClass_alias(CObject(object).obj_class()));

   CGTree(this).NATIVE_XULElement_update_object((void *)node, object);

   gtk_ctree_node_set_row_data(GTK_CTREE(this->tree_widget),
                               node, (gpointer)object);

   if (!this->attribute_hide) {
      item[0] = "@";
      item[1] = NULL;
      item[2] = NULL;
      attr_root = gtk_ctree_insert_node(GTK_CTREE(this->tree_widget),
                                        node, NULL, item, 8,
                                        NULL, NULL, NULL, NULL, FALSE, FALSE);
   }

   for (i = 0; i < ARRAY(&attribute_list).count(); i++) {
      attribute = ARRAY(&attribute_list).data()[i];
      if (attribute->type == PT_ElementObject) {
         CGTree(this).NATIVE_XULElement_fill(node, CObjPersistent(&((byte *)object)[attribute->offset]), -1);
      }
   }

   if (!this->attribute_hide) {
      for (i = 0; i < ARRAY(&attribute_list).count(); i++) {
         attribute = ARRAY(&attribute_list).data()[i];
         if (attribute->type == PT_ElementObject) {
            continue;
         }

         sprintf(temp, "@%s", attribute->name);
         CObjPersistent(object).attribute_get_string(attribute, &value);
         item[0] = temp;
         item[1] = CString(&value).string();
         item[2] = NULL;
         attr_node = gtk_ctree_insert_node(GTK_CTREE(this->tree_widget),
                                           attr_root, NULL, item, 8,
                                           NULL, NULL, NULL, NULL, FALSE, FALSE);
         gtk_ctree_node_set_row_data(GTK_CTREE(this->tree_widget),
                                     attr_node, (gpointer)attribute);
         if (CObjPersistent(object).attribute_default_get(attribute)) {
            gtk_ctree_node_set_cell_style(GTK_CTREE(this->tree_widget),
                                          attr_node, 1, gtk_ctree_style);
         }
      }
   }

   ARRAY(&attribute_list).delete();
   delete(&value);

   child = CObjPersistent(CObject(object).child_first());
   while (child) {
      CGTree(this).NATIVE_XULElement_fill(node, child, -1);
      child = CObjPersistent(CObject(object).child_next(CObject(child)));
   }
}/*CGTree::NATIVE_XULElement_fill*/

void CObjClientGTree::notify_selection_update(CObjPersistent *object, bool changing) {
   int count;
   CObjPersistent *selected_object;
   GtkCTreeNode *root_node;
   GtkCTreeNode *select_node;
   GdkColor col;
   CSelection *selection;

   /*>>>hack*/
   gdk_color_parse(changing ? "#ffffff" : "#B0B0B0", &col);

   root_node = gtk_ctree_node_nth(GTK_CTREE(this->gtree->tree_widget), 0);

   if (this->gtree->tree_widget) {
      selection = CObjServer(CObjClient(this)->server).selection_get();
      count = ARRAY(CSelection(selection).selection()).count();
      if (count) {
         selected_object = CObjPersistent(ARRAY(CSelection(selection).selection()).data()[0].object);
         select_node = gtk_ctree_find_by_row_data(GTK_CTREE(this->gtree->tree_widget),
                                                  root_node, selected_object);

         this->gtree->selection_resolving = TRUE;
         gtk_ctree_select(GTK_CTREE(this->gtree->tree_widget),
                          select_node);
         this->gtree->selection_resolving = FALSE;

#if 0
//         gtk_ctree_collapse_recursive(GTK_CTREE(CGObject(this->gtree)->native_object),
//                                      root_node);
         select_node = gtk_ctree_find_by_row_data(GTK_CTREE(CGObject(this->gtree)->native_object),
                                                  root_node, selected_object);
         gtk_ctree_node_set_background(GTK_CTREE(CGObject(this->gtree)->native_object),
                                       select_node, &col);
         while (selected_object) {
            selected_object = CObjPersistent(CObject(selected_object).parent());
            select_node = gtk_ctree_find_by_row_data(GTK_CTREE(CGObject(this->gtree)->native_object),
                                                     root_node, selected_object);
//            gtk_ctree_expand(GTK_CTREE(CGObject(this->gtree)->native_object),
//                             select_node);
         }
#endif
      }
   }
}/*CObjClientTree::notify_selection_update*/

void CObjClientGTree::notify_all_update(void) {
   CObjPersistent *object_root;
   GtkCTreeNode *root;
   int count;

   if (!this->gtree->tree_widget) {
      return;
   }

   count = ARRAY(&CObjClient(this)->object_root).count();
   if (!count) {
      return;
   }

   object_root = ARRAY(&CObjClient(this)->object_root).data()[0];
   root = gtk_ctree_node_nth(GTK_CTREE(this->gtree->tree_widget), 0);

   if (!root) {
      ASSERT("CObjClientGTree::notify_all_update - object not found");
   }

   gtk_ctree_remove_node(GTK_CTREE(this->gtree->tree_widget),
                         root);

   gtk_widget_hide(GTK_WIDGET(this->gtree->tree_widget));
   CGTree(this->gtree).NATIVE_XULElement_fill(NULL, object_root, -1);
   gtk_widget_show(GTK_WIDGET(this->gtree->tree_widget));
}/*CObjClientGTree::notify_all_update*/

void CObjClientGTree::notify_object_child_add(CObjPersistent *object, CObjPersistent *child) {
   GtkCTreeNode *root_node;
   GtkCTreeNode *child_node;

   if (CObject(child) == CObject(object).child_last()) {
      if (this->gtree->tree_widget) {
         /*>>>search for root node*/
         root_node = gtk_ctree_node_nth(GTK_CTREE(this->gtree->tree_widget), 0);
         child_node = gtk_ctree_find_by_row_data(GTK_CTREE(this->gtree->tree_widget),
                                                 root_node, object);
         CGTree(this->gtree).NATIVE_XULElement_fill(child_node, child, -1);
      }
   }
   else {
       CObjClientGTree(this).notify_all_update();
   }
}/*CObjClientGTree::notify_object_child_add*/

void CObjClientGTree::notify_object_child_remove(CObjPersistent *object, CObjPersistent *child) {
   GtkCTreeNode *root_node;
   GtkCTreeNode *child_node;

   if (this->gtree->tree_widget) {
      root_node = gtk_ctree_node_nth(GTK_CTREE(this->gtree->tree_widget), 0);
      child_node = gtk_ctree_find_by_row_data(GTK_CTREE(this->gtree->tree_widget),
                                              root_node, child);
      gtk_ctree_remove_node(GTK_CTREE(this->gtree->tree_widget),
                            child_node);
   }
}/*CObjClientGTree::notify_object_child_remove*/

void CObjClientGTree::notify_object_attribute_update(CObjPersistent *object, ARRAY<const TAttribute *> *attribute, bool changing) {
   ARRAY<const TAttribute *> attr;
   const TAttribute *object_attribute;
   int i;
   GtkCTreeNode *root_node, *node, *attr_node;
   CString value;
   bool is_default;

   root_node = gtk_ctree_node_nth(GTK_CTREE(this->gtree->tree_widget), 0);
   node = gtk_ctree_find_by_row_data(GTK_CTREE(this->gtree->tree_widget),
                                     root_node, (gpointer)object);

   CGTree(this->gtree).NATIVE_XULElement_update_object((void *)node, object);

   new(&value).CString(NULL);
   ARRAY(&attr).new();
   CObjPersistent(object).attribute_list(&attr, TRUE, FALSE, FALSE);
   for (i = 0; i < ARRAY(&attr).count(); i++) {
      object_attribute = ARRAY(&attr).data()[i];

      attr_node =
         gtk_ctree_find_by_row_data(GTK_CTREE(this->gtree->tree_widget),
                                    node, (gpointer)ARRAY(&attr).data()[i]);

      if (attr_node) {
         is_default = CObjPersistent(object).attribute_default_get(object_attribute);
         CObjPersistent(object).attribute_get_string(ARRAY(&attr).data()[i], &value);
         gtk_ctree_node_set_text(GTK_CTREE(this->gtree->tree_widget),
                                 attr_node, 1, CString(&value).string());
         gtk_ctree_node_set_cell_style(GTK_CTREE(this->gtree->tree_widget),
                                       attr_node, 1, is_default ? gtk_ctree_style : NULL);

      }
   }
   ARRAY(&attr).delete();
   delete(&value);
}/*CObjClientGTree::notify_object_attribute_update*/

Local void CGTree_GTK_signal_tree_select_row(GtkWidget *ctree, GtkCTreeNode *row,
                                             gint column) {
   CGTree *this = (CGTree *)gtk_object_get_data(GTK_OBJECT(ctree), GTK_OBJ_KEY);/*>>>ick*/
   void *row_data = gtk_ctree_node_get_row_data(GTK_CTREE(ctree), row);   /*>>>methods with , !)*/
//   CObjPersistent *object_root =
//      ARRAY(&CObjClient(&this->client)->object_root).data()[0];
   CSelection *selection;

   if (this->selection_resolving)
      return;

   if (row_data &&
       (((CObject *)row_data)->obj_sid == SID_OBJ)) {
      selection = CObjServer(CObjClient(&this->client)->server).selection_get();
      if (selection) {
         CObjServer(CObjClient(&this->client)->server).root_selection_update();
         CSelection(selection).clear();
         CSelection(selection).add(CObjPersistent(row_data), NULL);
         CObjServer(CObjClient(&this->client)->server).root_selection_update_end();
      }
   }
}/*CGTree_GTK_signal_tree_select_row*/

void CGTree::NATIVE_allocate(CGLayout *layout) {
   GtkWidget *scroll_widget, *tree_widget;
   CObjPersistent *object_root =
      ARRAY(&CObjClient(&this->client)->object_root).data()[0];
   TPoint position[2];
   CGTreeCol *tree_col;
   int i;

   tree_widget = gtk_ctree_new(CObject(&this->tree_cols).child_count(), 0);

   i = 0;
   tree_col = CGTreeCol(CObject(&this->tree_cols).child_first());
   while (tree_col) {
      gtk_clist_set_column_title(GTK_CLIST(tree_widget), i, CString(&tree_col->label).string());
      gtk_clist_set_column_width(GTK_CLIST(tree_widget), i, i ? 60 : 200);
      tree_col = CGTreeCol(CObject(&this->tree_cols).child_next(CObject(tree_col)));
      i++;
   }

   gtk_clist_column_titles_show(GTK_CLIST(tree_widget));

   gtk_container_set_border_width(GTK_CONTAINER(tree_widget), 0);
   gtk_ctree_set_show_stub(GTK_CTREE(tree_widget), FALSE);
//   gtk_widget_set_usize(tree_widget,
//                        C_GOBJ(this)->extent.right  - C_GOBJ(this)->extent.left,
//                        C_GOBJ(this)->extent.bottom - C_GOBJ(this)->extent.top);

#if 0
   gtk_signal_connect(GTK_OBJECT(tree_widget), "key_press_event",
                      (GtkSignalFunc) GOTREE_key_press_event, this);
#endif
   gtk_signal_connect(GTK_OBJECT(tree_widget), "tree_select_row",
                      (GtkSignalFunc) CGTree_GTK_signal_tree_select_row, this);
   gtk_widget_show(tree_widget);

   CGXULElement(this).utov(CGCanvas(layout), position);

   scroll_widget = gtk_scrolled_window_new(NULL, NULL);
   gtk_container_set_border_width(GTK_CONTAINER(scroll_widget), 0);
   gtk_widget_set_usize(scroll_widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);
   gtk_widget_show(scroll_widget);
   gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_widget),
                                         tree_widget);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_widget),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 scroll_widget, (int)position[0].x, (int)position[0].y);

   /*>>>a bit of a hack*/
//   CGObject(this)->native_object = (void *)tree_widget;
   CGObject(this)->native_object = (void *)scroll_widget;
   this->tree_widget = (void *)tree_widget;

   CGTree(this).NATIVE_XULElement_fill(NULL, object_root, -1);

   gtk_object_set_data(GTK_OBJECT(tree_widget), GTK_OBJ_KEY, (void *)this);

   class:base.NATIVE_allocate(layout);
}/*CGTree::NATIVE_allocate*/

Local void CGTabBox_GTK_signal_switch_page(GtkNotebook *notebook,
                                           GtkNotebookPage *page,
                                           guint page_num, CGTabBox *this) {
   CGLayoutTab *layout = CGLayoutTab(CObject(this).child_n(page_num));

   CObjPersistent(this).attribute_set_int(ATTRIBUTE<CGTabBox,selectedIndex>, page_num);

   if (layout) {
      CObjPersistent(layout).attribute_update(NULL);
      CObjPersistent(layout).attribute_update_end();
   }
}

void CGTabBox::NATIVE_allocate(CGLayout *layout) {
   GtkWidget *widget, *tab_widget;
   TPoint position[2];
   GtkWidget *label;
   CObjPersistent *tab_child;

   widget = gtk_notebook_new();

   CGXULElement(this).utov(CGCanvas(layout), position);

   gtk_widget_set_usize(widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);

   tab_child = CObjPersistent(CObject(this).child_first());
   while (tab_child) {
      CGCanvas(tab_child)->colour_background = GCOL_NONE;
      CGObject(tab_child).show(TRUE);

      label = gtk_label_new(CString(&CGLayoutTab(tab_child)->label).string());
      gtk_widget_show(label);
      tab_widget =
         GTK_WIDGET(CGObject(tab_child)->native_object);
      gtk_notebook_append_page(GTK_NOTEBOOK(widget), tab_widget, label);

      tab_child = CObjPersistent(CObject(this).child_next(CObject(tab_child)));
   }
   gtk_notebook_set_page(GTK_NOTEBOOK(widget), this->selectedIndex);
   gtk_signal_connect(GTK_OBJECT(widget), "switch_page",
                      (GtkSignalFunc) CGTabBox_GTK_signal_switch_page, this);

   gtk_widget_show(widget);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);

   CGObject(this)->native_object = (void *)widget;

   class:base.NATIVE_allocate(layout);
}/*CGTabBox::NATIVE_allocate*/

void CGTabBox::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   class:base.notify_object_linkage(linkage, object);
}/*CGTabBox::notify_object_linkage*/

void CGSplitter::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
   class:base.notify_attribute_update(attribute, changing);
}/*CGSplitter::notify_attribute_update*/

void CGSplitter::NATIVE_allocate(CGLayout *layout) {
   GtkWidget *widget = NULL, *split_widget;
   TPoint position[2];
   CObjPersistent *split_child;
   int i = 0;

   switch (this->type) {
   case EGSplitterType.Vertical:
      widget = gtk_hpaned_new();
      break;
   case EGSplitterType.Horizontal:
      widget = gtk_vpaned_new();
      break;
   }

   CGXULElement(this).utov(CGCanvas(layout), position);

   gtk_widget_set_usize(widget,
                        (int)position[1].x - (int)position[0].x, (int)position[1].y - (int)position[0].y);

   split_child = CObjPersistent(CObject(this).child_first());
   while (split_child) {
      CGObject(split_child).show(TRUE);

      split_widget =
         GTK_WIDGET(CGObject(split_child)->native_object);
      switch (i) {
      case 0:
         gtk_paned_add1(GTK_PANED(widget), split_widget);
         break;
      case 1:
         gtk_paned_add2(GTK_PANED(widget), split_widget);
         break;
      }

      split_child = CObjPersistent(CObject(this).child_next(CObject(split_child)));
      i++;
   }

   gtk_widget_show(widget);

   gtk_fixed_put(GTK_FIXED(CGCanvas_NATIVE_DATA(layout)->fixed),
                 widget, (int)position[0].x, (int)position[0].y);

   /*>>>? appears to have no effect*/
   gtk_paned_set_position(GTK_PANED(widget), this->position);

   CGObject(this)->native_object = (void *)widget;

   class:base.NATIVE_allocate(layout);
}/*CGSplitter::NATIVE_allocate*/

void CGSplitter::NATIVE_release(CGLayout *layout) {
   _virtual_CGXULElement_NATIVE_release(CGXULElement(this), layout);
}/*CGSplitter::NATIVE_release*/

/* Dummy methods */

bool CGXULElement::allocated_position(TRect *result) {
   return FALSE;
}

bool CGXULElement::NATIVE_focus_current(void) {
   return TRUE;
}

void CGProgressMeter::NATIVE_allocate(CGLayout *layout) {
}
void CGProgressMeter::notify_attribute_update(ARRAY<const TAttribute *> *attribute, bool changing) {
}

long CGXULElement::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGButton::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGToggleButton::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGCheckBox::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGRadioButton::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGColourPicker::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGSlider::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGListBox::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGMenuList::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGTree::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGTabBox::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGSplitter::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGTextBox::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGScrollBar::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGTextLabel::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGDatePicker::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}
long CGDatePickerButton::WIN32_XULELEMENT_MSG(unsigned int uMsg, unsigned long wParam, unsigned long lParam) { return 0;}

/*==========================================================================*/
MODULE::END;/*==============================================================*/
/*==========================================================================*/

