/*
 * A GtkCheckMenuItem that uses a GtkSwitch to show its 'active' property
 *
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Charles Kerr <charles.kerr@canonical.com>
 */

#include "config.h"

#include "idoswitchmenuitem.h"

static gboolean ido_switch_menu_button_release_event (GtkWidget      * widget,
                                                      GdkEventButton * event);


struct _IdoSwitchMenuItemPrivate
{
  GtkWidget * box;
  GtkWidget * content_area;
  GtkWidget * switch_w;
};

/***
****  Life Cycle
***/

G_DEFINE_TYPE (IdoSwitchMenuItem, ido_switch_menu_item, GTK_TYPE_CHECK_MENU_ITEM)

static void
ido_switch_menu_item_class_init (IdoSwitchMenuItemClass *klass)
{
  GObjectClass * gobject_class;
  GtkWidgetClass * widget_class;
  GtkCheckMenuItemClass * check_class;

  gobject_class = G_OBJECT_CLASS (klass);
  g_type_class_add_private (gobject_class, sizeof (IdoSwitchMenuItemPrivate));

  widget_class = GTK_WIDGET_CLASS (klass);
  widget_class->button_release_event = ido_switch_menu_button_release_event;

  check_class = GTK_CHECK_MENU_ITEM_CLASS (klass);
  check_class->draw_indicator = NULL;
}

static void
ido_switch_menu_item_init (IdoSwitchMenuItem *item)
{
  IdoSwitchMenuItemPrivate *priv;

  priv = item->priv = G_TYPE_INSTANCE_GET_PRIVATE (item, IDO_TYPE_SWITCH_MENU_ITEM, IdoSwitchMenuItemPrivate);
  priv->box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  priv->content_area = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  priv->switch_w = gtk_switch_new ();

  gtk_box_pack_start (GTK_BOX (priv->box), priv->content_area, TRUE, TRUE, 0);
  gtk_box_pack_end (GTK_BOX (priv->box), priv->switch_w, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (item), priv->box);
  gtk_widget_show_all (priv->box);

  g_object_bind_property (item, "active",
                          priv->switch_w, "active",
                          G_BINDING_SYNC_CREATE);
}

/***
**** Don't popdown the menu immediately after clicking on a switch...
**** wait a moment so the user can see the GtkSwitch be toggled.
***/

static gboolean
popdown_later_cb (gpointer widget)
{
  GtkWidget * parent = gtk_widget_get_parent (widget);
  if (GTK_IS_MENU (parent))
    {
      gtk_menu_shell_deactivate (GTK_MENU_SHELL(parent));
    }
  g_object_unref (widget);
  return FALSE; /* only call this cb once */
}

static gboolean
ido_switch_menu_button_release_event (GtkWidget * widget, GdkEventButton * event)
{
  gtk_menu_item_activate (GTK_MENU_ITEM(widget));
  g_timeout_add (500, popdown_later_cb, g_object_ref(widget));
  return TRUE; /* stop the event so that it doesn't trigger popdown() */
}

/***
**** Public API
***/

GtkWidget *
ido_switch_menu_item_new (void)
{
  return g_object_new (IDO_TYPE_SWITCH_MENU_ITEM, NULL);
}

GtkContainer *
ido_switch_menu_item_get_content_area (IdoSwitchMenuItem * item)
{
  g_return_val_if_fail (IDO_IS_SWITCH_MENU_ITEM(item), NULL);

  return GTK_CONTAINER (item->priv->content_area);
}
