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
//#include "../goobjcanvas.h"
#include "../gwindow.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

Local gint CGMenuItem__activate(GtkWidget *widget, CGMenuItem *this) {
   CObject *menu = CObject(this);

   while (CObject(menu).obj_class() == &class(CGMenuItem)) {
      menu = CObject(menu).parent();
   }

   CGMenu(menu)->selected = this;

   return 0;
}/*CGMenuItem__activate*/

Local gint CGMenu__menu_selection_done(GtkWidget *widget,
                                       CGMenu *this) {
   if (CGMenu(this)->selected) {
      CGMenu(this).menu_item_select(CGMenu(this)->selected);
      CGMenu(this)->selected = NULL;
   }
   return 0;
}/*CGMenuPopup__menu_selection_done*/

Local gint CGMenu__menu_toggled(GtkWidget *widget,
                                CGListItem *this) {
   gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widget), CGListItem(this)->checked);
   return TRUE;
}/*CGMenuPopup__menu_toggled*/

void CGMenu::NATIVE_allocate(void) {
   GtkWidget **widget = (GtkWidget **)&CGObject(this)->native_object;

   *widget = gtk_menu_bar_new();
   if (CGObject(this)->visibility) {
      gtk_widget_show(*widget);
   }
   else {
      gtk_widget_hide(*widget);
   }

   CGMenu(this).NATIVE_item_allocate(NULL);

   gtk_signal_connect(GTK_OBJECT(*widget), "selection_done",
                      GTK_SIGNAL_FUNC(CGMenu__menu_selection_done), this);
}/*CGMenu::NATIVE_allocate*/

void CGMenu::NATIVE_release(void) {
   gtk_widget_destroy(GTK_WIDGET(CGObject(this)->native_object));
}/*CGMenu::NATIVE_release*/

void CGMenu::NATIVE_item_allocate(CGMenuItem *parent) {
   CGMenuItem *item;
   GtkWidget *item_widget;
   CObject *menu = parent ? CObject(parent) : CObject(this);
   GtkWidget *menu_widget = (GtkWidget *)CGObject(menu)->native_object;
   CString label;

   if (!menu_widget) {
      return;
   }

   new(&label).CString(NULL);
   item = CGMenuItem(CObject(menu).child_first());
   while (item) {
      if (CGObject(item)->visibility == EGVisibility.visible) {
         if (CString(&item->acceltext).length()) {
            /*>>>tab character just gives a single tab in GTK.  Will need check all menu items for maximum length in future */
            CString(&label).printf("%s\t%s", CString(&CGListItem(item)->label).string(),
                                    CString(&item->acceltext).string());
         }
         else {
            CString(&label).printf("%s", CString(&CGListItem(item)->label).string());
         }
         if (CObject(item).obj_class() == &class(CGMenuSeparator)) {
            item_widget = gtk_separator_menu_item_new();
         }
         else {
            if ((parent || CObject(this).obj_class() == &class(CGMenuPopup))) {
               if (CGListItem(item)->type == EGListItemType.checkbox) {
                  item_widget = gtk_check_menu_item_new_with_mnemonic(CString(&label).string());
                  gtk_signal_connect(GTK_OBJECT(item_widget), "toggled",
                                     GTK_SIGNAL_FUNC(CGMenu__menu_toggled), item);
                  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item_widget), CGListItem(item)->checked);
               }
               else {
                   item_widget = gtk_menu_item_new_with_mnemonic(CString(&label).string());
               }
            }
           else {
              item_widget = gtk_menu_item_new_with_mnemonic(CString(&label).string());
           }
           gtk_widget_set_sensitive(item_widget, !CGListItem(item)->disabled);
         }
         CGObject(item)->native_object = (void *)item_widget;
         gtk_widget_show(item_widget);
         if (parent || CObject(this).obj_class() == &class(CGMenuPopup)) {
            gtk_menu_append(GTK_MENU(menu_widget), item_widget);
            gtk_signal_connect(GTK_OBJECT(item_widget), "activate",
                               GTK_SIGNAL_FUNC(CGMenuItem__activate), item);

         }
         else {
            gtk_menu_bar_append(GTK_MENU_BAR(menu_widget), item_widget);
         }

         if (CObject(item).child_count()) {
            CGObject(item)->native_object = (void *)gtk_menu_new();
            CGMenu(this).NATIVE_item_allocate(CGMenuItem(item));
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_widget),
                                      GTK_WIDGET(CGObject(item)->native_object));
         }
      }
      item = CGMenuItem(CObject(menu).child_next(CObject(item)));
   }
   delete(&label);
}/*CGMenu::NATIVE_item_allocate*/

void CGMenu::NATIVE_item_update(CGMenuItem *item) {
   CObject *parent = CObject(item).parent();
   GtkWidget *widget;

   if (!CGObject(item)->native_object)
      return;

   widget = GTK_WIDGET(CGObject(item)->native_object);

   if (parent != CObject(this) || CObject(this).obj_class() == &class(CGMenuPopup)) {
      if (CGListItem(item)->type == EGListItemType.checkbox) {
         gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widget), CGListItem(item)->checked);
      }
   }
   gtk_widget_set_sensitive(widget, !CGListItem(item)->disabled);
}/*CGMenu::NATIVE_item_update*/

Local gint CGMenuPopup__menu_selection_done(GtkWidget *widget,
                                            CGMenuPopup *this) {
   this->selection_done = TRUE;
   return 0;
}/*CGMenuPopup__menu_selection_done*/

CGMenuItem *CGMenuPopup::execute(CGCanvas *canvas) {
   GtkWidget **widget = (GtkWidget **)&CGObject(this)->native_object;

   *widget = gtk_menu_new();
   gtk_signal_connect(GTK_OBJECT(*widget), "selection_done",
                      GTK_SIGNAL_FUNC(CGMenuPopup__menu_selection_done), this);
   CGMenu(this).NATIVE_item_allocate(NULL);

   gtk_menu_popup(GTK_MENU(CGObject(this)->native_object),
                  NULL, NULL, NULL, NULL, 3, 0);

   while (!this->selection_done) {
      gtk_main_iteration();
   }

   return CGMenu(this)->selected;
}/*CGMenuPopup::execute*/

Local bool CGWindow__gtk_signal_delete_event(GtkWidget *widget,
                                             GdkEventAny *event,
                                             gpointer data) {
   CGWindow *this = CGWindow(data);

   if (CGWindow(this).notify_request_close()) {
      if (!this->modal) {
         delete(CObject(data));
      }
   }

   return TRUE;
}/*CGWindow__gtk_signal_delete_event*/

Local gint CGWindow__gtk_signal_focus_in_event(GtkWidget *widget,
                                               GdkEventFocus *event,
                                               gpointer data) {
   CGWindow *window;
   int i;

   CFramework(&framework).window_active_set(CGWindow(data));

   for (i = 0; i < ARRAY(&framework.window).count(); i++) {
      window = ARRAY(&framework.window).data()[i];
      if (CGObject(window)->native_object != widget &&
          window->topmost) {
         gdk_window_raise(GTK_WIDGET(CGObject(window)->native_object)->window);
      }
      else {
      }
   }

   return FALSE;
}/*CGWindow__gtk_signal_focus_in_event*/

Local gint CGWindow__gtk_signal_focus_out_event(GtkWidget *widget,
                                               GdkEventFocus *event,
                                               gpointer data) {
   CFramework(&framework).window_active_set(NULL);

   return FALSE;
}/*CGWindow__gtk_signal_focus_out_event*/

Local gint CGWindow__gtk_signal_key_press_event(GtkWidget *widget,
                                                GdkEventKey *event,
                                                gpointer data) {
   CGWindow *this = CGWindow(data);
   CEventKey keyevent;
   bool capture = FALSE;
   int modifier;
   
   modifier  = 0;
   modifier |= event->state & 1 ? (1 << EEventModifier.shiftKey) : 0;

   if (event->keyval <= 0xFF) {
      /*remap space bar */
      if (event->keyval == ' ') {
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Space, 0, modifier);
      }
      else {
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Ascii, (char)event->keyval, modifier);
      }
      CGObject(this).event(CEvent(&keyevent));
   }
   else {
      switch (event->keyval) {
      case GDK_BackSpace:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Backspace, 0, modifier);
         goto sendevent;
      case GDK_Escape:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Escape, 0, modifier);
         goto sendevent;
      case GDK_Tab:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Tab, 0, modifier);
         capture = TRUE;
         goto sendevent;
      case GDK_Return:
      case GDK_KP_Enter:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Enter, 0, modifier);
         goto sendevent;
      case GDK_Left:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Left, 0, modifier);
         capture = TRUE;
         goto sendevent;
      case GDK_Right:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Right, 0, modifier);
         capture = TRUE;
         goto sendevent;
      case GDK_Up:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Up, 0, modifier);
         capture = TRUE;
         goto sendevent;
      case GDK_Down:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Down, 0, modifier);
         capture = TRUE;
         goto sendevent;
      case GDK_Home:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Home, 0, modifier);
         capture = TRUE;
         goto sendevent;
      case GDK_End:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.End, 0, modifier);
         capture = TRUE;
         goto sendevent;
      case GDK_Insert:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Insert, 0, modifier);
         goto sendevent;
      case GDK_Delete:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Delete, 0, modifier);
         goto sendevent;
      case GDK_F1:
      case GDK_F2:
      case GDK_F3:
      case GDK_F4:
      case GDK_F5:
      case GDK_F6:
      case GDK_F7:
      case GDK_F8:
      case GDK_F9:
      case GDK_F10:
      case GDK_F11:
      case GDK_F12:
      case GDK_F13:
      case GDK_F14:
      case GDK_F15:
      case GDK_F16:
      case GDK_F17:
      case GDK_F18:
      case GDK_F19:
      case GDK_F20:
      case GDK_F21:
      case GDK_F22:
      case GDK_F23:
      case GDK_F24:
         new(&keyevent).CEventKey(EEventKeyType.down, EEventKey.Function, (char)(event->keyval - GDK_F1 + 1), modifier);
         goto sendevent;
      sendevent:
         CGObject(this).event(CEvent(&keyevent));
//         CGObject(this->input_focus).event(CEvent(&keyevent));
         break;
      }
   }
//   printf("key press event\n");

   return capture;
}/*CGWindow__gtk_signal_key_press_event*/

void CGWindow::modal(bool modal) {
   CGWindow *app_window;
   int i;

   if (this->modal == modal)
      return;

   this->topmost = modal;

   for (i = 0; i < ARRAY(&framework.window).count(); i++) {
      app_window = ARRAY(&framework.window).data()[i];
      if (app_window != this) {
         if (app_window->modal) {
            app_window->topmost = !modal;
         }
      }
   }

   gtk_window_set_modal(GTK_WINDOW(CGObject(this)->native_object), modal);
   this->modal = modal;
}/*CGWindow::modal*/

void CGWindow::topmost(bool topmost, bool stick) {
//   this->topmost = topmost;
}/*CGWindow::topmost*/

void CGWindow::disable_close(bool disable) {
//   do nothing for now*/
}/*CGWindow::disable_close*/

void CGWindow::disable_maximize(bool disable) {
//   do nothing for now*/
}/*CGWindow::disable_maximize*/

void CGWindow::disable_minimize(bool disable) {
//   do nothing for now*/
}/*CGWindow::disable_minimize*/

void CGWindow::NATIVE_maximize(bool maximize) {
   gtk_window_maximize(GTK_WINDOW(CGObject(this)->native_object));
}/*CGWindow::NATIVE_maximize*/

void CGWindow::NATIVE_fullscreen(bool fullscreen) {
   if (fullscreen) {
//      gtk_window_fullscreen(GTK_WINDOW(CGObject(this)->native_object));
   }
   else {
//      gtk_window_unfullscreen(GTK_WINDOW(CGObject(this)->native_object));
   }
}/*CGWindow::NATIVE_fullscreen*/


//Local gint CGWindow_GTK_signal_key_press(GtkWidget *widget, GdkEventKey *event,
//                                         CGXULElement *this) {
//   int modifier;
//
//   switch (event->keyval) {
//   case GDK_Tab:
//      printf("window tab key %d\n", event->state);
//      return TRUE;
//   }
//   return FALSE;
//}/*CGXULElement_GTK_signal_key_press*/


void CGWindow::size_set(int width, int height) {
   gtk_window_resize(GTK_WINDOW(CGObject(this)->native_object), width, height);
}/*CGWindow::size_set*/

void CGWindow::position_set(int x, int y) {
   gtk_window_move(GTK_WINDOW(CGObject(this)->native_object), x, y);
}/*CGWindow::position_set*/

void CGWindow::position_size_set(int x, int y, int width, int height) {
   gtk_window_move(GTK_WINDOW(CGObject(this)->native_object), x, y);
   gtk_window_resize(GTK_WINDOW(CGObject(this)->native_object), width, height);
}/*CGWindow::position_size_set*/

void CGWindow::NATIVE_new(void) {
   GtkWidget **widget = (GtkWidget **)&(CGObject(this)->native_object);

   *widget = gtk_window_new(this->nodecoration ? GTK_WINDOW_POPUP : GTK_WINDOW_TOPLEVEL);
//   gtk_signal_connect(GTK_OBJECT(*widget), "key_press_event",
//                      (GtkSignalFunc) CGWindow_GTK_signal_key_press, this);

   gtk_window_set_position(GTK_WINDOW(*widget), GTK_WIN_POS_CENTER);
}/*CGWindow::NATIVE_new*/

void CGWindow::NATIVE_delete(void) {
   if (CGObject(this)->native_object) {
      gtk_widget_destroy(GTK_WIDGET(CGObject(this)->native_object));
   }
}/*CGWindow::NATIVE_delete*/

void CGWindow::NATIVE_show(bool show) {
   GtkWidget **widget = (GtkWidget **)&(CGObject(this)->native_object);
   CGXULElement *child = CGXULElement(CObject(this).child_first());

   if (show) {
      this->vbox = gtk_vbox_new(FALSE, 1);
      gtk_container_add(GTK_CONTAINER(*widget), this->vbox);
      CGMenu(&this->menu).NATIVE_allocate();
      if (CObject(&this->menu).child_count()) {
         gtk_box_pack_start(GTK_BOX(this->vbox),
                            GTK_WIDGET(CGObject(&this->menu)->native_object),
                            FALSE, FALSE, 0);
      }
      gtk_box_pack_start(GTK_BOX(this->vbox),
                         GTK_WIDGET(CGObject(child)->native_object),
                         TRUE, TRUE, 0);
      gtk_signal_connect(GTK_OBJECT(*widget), "delete_event",
                         GTK_SIGNAL_FUNC(CGWindow__gtk_signal_delete_event),
                         this);
      gtk_widget_show(this->vbox);

      gtk_signal_connect(GTK_OBJECT(*widget), "focus_in_event",
                         GTK_SIGNAL_FUNC(CGWindow__gtk_signal_focus_in_event),
                         this);
      gtk_signal_connect(GTK_OBJECT(*widget), "focus_out_event",
                         GTK_SIGNAL_FUNC(CGWindow__gtk_signal_focus_out_event),
                         this);
      gtk_signal_connect(GTK_OBJECT(*widget), "key_press_event",
                         GTK_SIGNAL_FUNC(CGWindow__gtk_signal_key_press_event),
                         this);

      gtk_widget_set_usize(GTK_WIDGET(CGObject(child)->native_object),
                           (int)CGXULElement(child)->min_width,
                           (int)CGXULElement(child)->min_height);
      gtk_window_set_position(GTK_WINDOW(CGObject(this)->native_object),
                              GTK_WIN_POS_CENTER);

      gtk_widget_show(GTK_WIDGET(CGObject(this)->native_object));
      if (this->nodecoration) {
//         gtk_window_set_decorated(GTK_WINDOW(CGObject(this)->native_object), FALSE);
      }
//      GTK_WINDOW(CGObject(this)->native_object)->allow_shrink = TRUE;
//      GTK_WINDOW(CGObject(this)->native_object)->allow_grow = TRUE;
   }
   else {
      gtk_widget_hide(GTK_WIDGET(CGObject(this)->native_object));
   }

//>>>   gtk_widget_set_usize(GTK_WIDGET(CGObject(this)->native_object),
//                        320, 200);
}/*CGWindow::NATIVE_show*/

void CGWindow::NATIVE_title_set(const char *title) {
   gtk_window_set_title(GTK_WINDOW(CGObject(this)->native_object), CString(&this->title).string());
}/*CGWindow::NATIVE_title_set*/

void CGWindow::NATIVE_extent_set(TRect *extent) {
   gtk_widget_set_usize(GTK_WIDGET(CGObject(this)->native_object),
                        (int)extent->point[1].x - (int)extent->point[0].x,
                        (int)extent->point[1].y - (int)extent->point[1].y);
}/*CGWindow::NATIVE_extent_set*/

void CGWindow::NATIVE_show_help(CString *help_topic) {
}/*CGWindow::NATIVE_show_help*/

#if 0
void store_filename(GtkFileSelection *selector, gpointer user_data) {
   selected_filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION(file_selector));
}

void create_file_selection(void) {

   /* Create the selector */

   file_selector = gtk_file_selection_new("Please select a file for editing.");

   gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->ok_button),
                        "clicked", GTK_SIGNAL_FUNC (store_filename), NULL);

   /* Ensure that the dialog box is destroyed when the user clicks a button. */

   gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->ok_button),
                              "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy),
                              (gpointer) file_selector);

   gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->cancel_button),
                              "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy),
                              (gpointer) file_selector);

   /* Display that dialog */

   gtk_widget_show (file_selector);
}
#endif

void CGDialogFileSelect__click_ok(GtkWidget *widget, gpointer user_data) {
   CGDialogFileSelect *this = CGDialogFileSelect(user_data);

   this->exec_result = 1;

   CFramework(&framework).NATIVE_modal_exit(CObject(this));
}/*CGDialogFileSelect__click_ok*/

void CGDialogFileSelect__click_cancel(GtkWidget *widget, gpointer user_data) {
   CGDialogFileSelect *this = CGDialogFileSelect(user_data);

   this->exec_result = 0;

   CFramework(&framework).NATIVE_modal_exit(CObject(this));
}/*CGDialogFileSelect__click_cancel*/

bool CGDialogFileSelect::execute(CString *filename) {
   GtkWidget *file_sel;

   file_sel = gtk_file_selection_new(this->title);
   gtk_window_set_position(GTK_WINDOW(file_sel),
                           GTK_WIN_POS_CENTER);

   gtk_signal_connect(GTK_OBJECT (GTK_FILE_SELECTION(file_sel)->ok_button),
                      "clicked", GTK_SIGNAL_FUNC(CGDialogFileSelect__click_ok),
                      this);
   gtk_signal_connect(GTK_OBJECT (GTK_FILE_SELECTION(file_sel)->cancel_button),
                      "clicked", GTK_SIGNAL_FUNC(CGDialogFileSelect__click_cancel),
                      this);
   gtk_widget_show(file_sel);

   CFramework(&framework).NATIVE_modal_wait(CObject(this));
   CFramework(&framework).NATIVE_modal_exit(CObject(this));

//   gtk_file_selection_complete(GTK_FILE_SELECTION(file_sel), "*");

   CString(filename).set(gtk_file_selection_get_filename(GTK_FILE_SELECTION(file_sel)));

   gtk_widget_destroy(file_sel);

   return this->exec_result;
}/*CGDialogFileSelect::execute*/

/* Dummy methods */
void CGWindow::NATIVE_opacity(double opacity) {}
void CGWindow::NATIVE_monitor_area(TRect *result) {
   result->point[0].x = 0;
   result->point[0].y = 0;
   result->point[1].x = 1024;
   result->point[1].y = 768;
}/*CGWindow::NATIVE_monitor_area*/


/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
