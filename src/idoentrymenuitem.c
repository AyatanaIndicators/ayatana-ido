/*
 * Copyright 2010 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of either or both of the following licenses:
 *
 * 1) the GNU Lesser General Public License version 3, as published by the
 * Free Software Foundation; and/or
 * 2) the GNU Lesser General Public License version 2.1, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 and version 2.1 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authors:
 *    Cody Russell <crussell@canonical.com>
 */

#include "idoentrymenuitem.h"

static void     ido_entry_menu_item_select            (GtkItem        *item);
static void     ido_entry_menu_item_deselect          (GtkItem        *item);
static gboolean ido_entry_menu_item_button_release    (GtkWidget      *widget,
                                                       GdkEventButton *event);
static gboolean ido_entry_menu_item_key_press         (GtkWidget      *widget,
                                                       GdkEventKey    *event,
                                                       gpointer        data);
static gboolean ido_entry_menu_item_button_press      (GtkWidget      *widget,
                                                       GdkEventButton *event);
static void     ido_entry_menu_item_send_focus_change (GtkWidget *widget,
                                                       gboolean   in);
static void     entry_realized_cb                     (GtkWidget        *widget,
                                                       IdoEntryMenuItem *item);
static void     entry_move_focus_cb                   (GtkWidget        *widget,
                                                       GtkDirectionType  direction,
                                                       IdoEntryMenuItem *item);

struct _IdoEntryMenuItemPrivate
{
  GtkWidget       *box;
  GtkWidget       *entry;
  gboolean         selected;
};

G_DEFINE_TYPE (IdoEntryMenuItem, ido_entry_menu_item, GTK_TYPE_MENU_ITEM)

#define IDO_ENTRY_MENU_ITEM_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), IDO_TYPE_ENTRY_MENU_ITEM, IdoEntryMenuItemPrivate))

static void
ido_entry_menu_item_class_init (IdoEntryMenuItemClass *klass)
{
  GObjectClass     *gobject_class;
  GtkWidgetClass   *widget_class;
  GtkMenuItemClass *menu_item_class;
  GtkItemClass     *item_class;

  gobject_class = G_OBJECT_CLASS (klass);
  widget_class = GTK_WIDGET_CLASS (klass);
  menu_item_class = GTK_MENU_ITEM_CLASS (klass);
  item_class = GTK_ITEM_CLASS (klass);

  widget_class->button_release_event = ido_entry_menu_item_button_release;
  widget_class->button_press_event = ido_entry_menu_item_button_press;

  item_class->select = ido_entry_menu_item_select;
  item_class->deselect = ido_entry_menu_item_deselect;

  menu_item_class->hide_on_activate = FALSE;

  g_type_class_add_private (gobject_class, sizeof (IdoEntryMenuItemPrivate));
}

static void
ido_entry_menu_item_init (IdoEntryMenuItem *item)
{
  IdoEntryMenuItemPrivate *priv;
  GtkBorder border;

  border.left = 4;
  border.right = 4;
  border.top = 2;
  border.bottom = 2;

  priv = item->priv = IDO_ENTRY_MENU_ITEM_GET_PRIVATE (item);

  priv->entry = g_object_new (gtk_entry_get_type (),
                              "inner-border", &border,
                              NULL);

  g_signal_connect (priv->entry,
                    "realize",
                    G_CALLBACK (entry_realized_cb),
                    item);
  g_signal_connect (priv->entry,
                    "move-focus",
                    G_CALLBACK (entry_move_focus_cb),
                    item);

  priv->box = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (priv->box), priv->entry, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (item), priv->box);

  gtk_widget_show_all (priv->box);
}

static gboolean
ido_entry_menu_item_button_release (GtkWidget      *widget,
                                    GdkEventButton *event)
{
  return TRUE;
}

static gboolean
ido_entry_menu_item_key_press (GtkWidget     *widget,
                               GdkEventKey   *event,
                               gpointer       data)
{
  IdoEntryMenuItem *menuitem = (IdoEntryMenuItem *)data;

  if (menuitem->priv->selected)
    {
      gtk_widget_event (menuitem->priv->entry,
                        (GdkEvent *)event);
    }

  return FALSE;
}

static void
ido_entry_menu_item_send_focus_change (GtkWidget *widget,
                   gboolean   in)
{
  GdkEvent *event = gdk_event_new (GDK_FOCUS_CHANGE);

  g_object_ref (widget);

  if (in)
    {
      GTK_WIDGET_SET_FLAGS (widget, GTK_HAS_FOCUS);
    }
  else
    {
      GTK_WIDGET_UNSET_FLAGS (widget, GTK_HAS_FOCUS);
    }

  event->focus_change.type = GDK_FOCUS_CHANGE;
  event->focus_change.window = g_object_ref (widget->window);
  event->focus_change.in = in;

  gtk_widget_event (widget, event);

  g_object_notify (G_OBJECT (widget), "has-focus");

  g_object_unref (widget);
  gdk_event_free (event);
}

static gboolean
ido_entry_menu_item_button_press (GtkWidget      *widget,
                                  GdkEventButton *event)
{
  GtkWidget *entry;

  entry = IDO_ENTRY_MENU_ITEM (widget)->priv->entry;

  if (entry->window != NULL)
    {
      gdk_window_raise (entry->window);
    }

  if (!GTK_WIDGET_HAS_FOCUS (entry))
    {
      gtk_widget_grab_focus (entry);
    }

  return FALSE;
}

static void
ido_entry_menu_item_select (GtkItem *item)
{
  IDO_ENTRY_MENU_ITEM (item)->priv->selected = TRUE;

  ido_entry_menu_item_send_focus_change (GTK_WIDGET (IDO_ENTRY_MENU_ITEM (item)->priv->entry), TRUE);
}

static void
ido_entry_menu_item_deselect (GtkItem *item)
{
  IDO_ENTRY_MENU_ITEM (item)->priv->selected = FALSE;

  ido_entry_menu_item_send_focus_change (GTK_WIDGET (IDO_ENTRY_MENU_ITEM (item)->priv->entry), FALSE);
}


static void
entry_realized_cb (GtkWidget        *widget,
                   IdoEntryMenuItem *item)
{
  if (widget->window != NULL)
    {
      gdk_window_raise (widget->window);
    }

  g_signal_connect (GTK_WIDGET (item)->parent,
                    "key-press-event",
                    G_CALLBACK (ido_entry_menu_item_key_press),
                    item);

  ido_entry_menu_item_send_focus_change (widget, TRUE);
}

static void
entry_move_focus_cb (GtkWidget        *widget,
                     GtkDirectionType  direction,
                     IdoEntryMenuItem *item)
{
  ido_entry_menu_item_send_focus_change (GTK_WIDGET (IDO_ENTRY_MENU_ITEM (item)->priv->entry), FALSE);

  g_signal_emit_by_name (item,
                         "move-focus",
                         GTK_DIR_TAB_FORWARD);
}

/* Public API */
GtkWidget *
ido_entry_menu_item_new (void)
{
  return g_object_new (IDO_TYPE_ENTRY_MENU_ITEM, NULL);
}

GtkWidget *
ido_entry_menu_item_get_entry (IdoEntryMenuItem *item)
{
  g_return_val_if_fail (IDO_IS_ENTRY_MENU_ITEM (item), NULL);

  return item->priv->entry;
}
