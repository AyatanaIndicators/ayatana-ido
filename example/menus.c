#include <gtk/gtk.h>

#include "idoscalemenuitem.h"
#include "idoentrymenuitem.h"

int
main (int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *menu;
  GtkWidget *menuitem;
  GtkWidget *root;
  GtkWidget *menubar;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Menus");
  gtk_widget_set_size_request (window, 300, 200);
  g_signal_connect (window, "delete-event",
                    G_CALLBACK (gtk_main_quit), NULL);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  menubar = gtk_menu_bar_new ();
  gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);

  menu = gtk_menu_new ();

  root = gtk_menu_item_new_with_label ("File");
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (root), menu);

  menuitem = gtk_menu_item_new_with_label ("New");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

  menuitem = gtk_menu_item_new_with_label ("Open");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

  menuitem = ido_scale_menu_item_new_with_range ("Volume", 0, 100, 1);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

  menuitem = ido_entry_menu_item_new ();
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

  /* Add the menubar */
  gtk_menu_shell_append (GTK_MENU_SHELL (menubar), root);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}
