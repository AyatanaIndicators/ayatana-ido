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

#include <gdk/gdkkeysyms.h>
#include "idocalendarmenuitem.h"

static void     ido_calendar_menu_item_select            (GtkItem        *item);
static void     ido_calendar_menu_item_deselect          (GtkItem        *item);
static gboolean ido_calendar_menu_item_button_release    (GtkWidget      *widget,
                                                          GdkEventButton *event);
static gboolean ido_calendar_menu_item_button_press      (GtkWidget      *widget,
                                                          GdkEventButton *event);
static gboolean ido_calendar_menu_item_key_press         (GtkWidget      *widget,
                                                          GdkEventKey    *event,
                                                          gpointer        data);
static void     ido_calendar_menu_item_send_focus_change (GtkWidget      *widget,
                                                          gboolean        in);
static void     calendar_realized_cb                     (GtkWidget        *widget,
                                                          IdoCalendarMenuItem *item);
static void     calendar_move_focus_cb                   (GtkWidget        *widget,
                                                          GtkDirectionType  direction,
                                                          IdoCalendarMenuItem *item);
static void     calendar_month_changed_cb                (GtkWidget *widget, 
                                                          gpointer user_data);                             

struct _IdoCalendarMenuItemPrivate
{
  GtkWidget       *box;
  GtkWidget       *calendar;
  gboolean         selected;
};

G_DEFINE_TYPE (IdoCalendarMenuItem, ido_calendar_menu_item, GTK_TYPE_MENU_ITEM)

#define IDO_CALENDAR_MENU_ITEM_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), IDO_TYPE_CALENDAR_MENU_ITEM, IdoCalendarMenuItemPrivate))

static void
ido_calendar_menu_item_class_init (IdoCalendarMenuItemClass *klass)
{
  GObjectClass     *gobject_class;
  GtkWidgetClass   *widget_class;
  GtkMenuItemClass *menu_item_class;
  GtkItemClass     *item_class;

  gobject_class = G_OBJECT_CLASS (klass);
  widget_class = GTK_WIDGET_CLASS (klass);
  menu_item_class = GTK_MENU_ITEM_CLASS (klass);
  item_class = GTK_ITEM_CLASS (klass);

  widget_class->button_release_event = ido_calendar_menu_item_button_release;
  widget_class->button_press_event = ido_calendar_menu_item_button_press;

  item_class->select = ido_calendar_menu_item_select;
  item_class->deselect = ido_calendar_menu_item_deselect;

  menu_item_class->hide_on_activate = TRUE;

  g_type_class_add_private (gobject_class, sizeof (IdoCalendarMenuItemPrivate));
  
  g_signal_new("month-changed", G_TYPE_FROM_CLASS(klass),
                                G_SIGNAL_RUN_LAST, 0, NULL, NULL,
                                g_cclosure_marshal_VOID__VOID,
                                G_TYPE_NONE, 0);
}

static void
ido_calendar_menu_item_init (IdoCalendarMenuItem *item)
{
  IdoCalendarMenuItemPrivate *priv;
  GtkBorder border;

  border.left = 4;
  border.right = 4;
  border.top = 2;
  border.bottom = 2;

  priv = item->priv = IDO_CALENDAR_MENU_ITEM_GET_PRIVATE (item);

  /* Will be disposed automatically */
  priv->calendar = g_object_new (gtk_calendar_get_type (),
                                 NULL);

  g_signal_connect (priv->calendar,
                    "realize",
                    G_CALLBACK (calendar_realized_cb),
                    item);
  g_signal_connect (priv->calendar,
                    "move-focus",
                    G_CALLBACK (calendar_move_focus_cb),
                    item);

  priv->box = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (priv->box), priv->calendar, FALSE, FALSE, 0);

  gtk_container_add (GTK_CONTAINER (item), priv->box);

  gtk_widget_show_all (priv->box);
}

static void
ido_calendar_menu_item_send_focus_change (GtkWidget *widget,
                                          gboolean   in)
{
  GdkEvent *event = gdk_event_new (GDK_FOCUS_CHANGE);

  g_object_ref (widget);

  if (in)
    gtk_widget_grab_focus (widget);

  event->focus_change.type = GDK_FOCUS_CHANGE;
  event->focus_change.window = g_object_ref (widget->window);
  event->focus_change.in = in;

  gtk_widget_event (widget, event);

  g_object_notify (G_OBJECT (widget), "has-focus");

  g_object_unref (widget);
  gdk_event_free (event);
}

static gboolean
ido_calendar_menu_item_key_press (GtkWidget   *widget,
                                  GdkEventKey *event,
                                  gpointer     data)
{
  IdoCalendarMenuItem *menuitem = (IdoCalendarMenuItem *)data;

  if (menuitem->priv->selected)
    {
      GtkWidget *calendar = menuitem->priv->calendar;

      gtk_widget_event (calendar,
                        ((GdkEvent *)(void*)(event)));

      if (calendar->window != NULL)
        {
          gdk_window_raise (calendar->window);
        }

      if (!gtk_widget_has_focus (calendar))
        {
          gtk_widget_grab_focus (calendar);
        }

      return event->keyval != GDK_Return;
    }

  return FALSE;
}

static gboolean
ido_calendar_menu_item_button_press (GtkWidget      *widget,
                                     GdkEventButton *event)
{
  GtkWidget *calendar = IDO_CALENDAR_MENU_ITEM (widget)->priv->calendar;

  if (event->button == 1)
    {
      if (calendar->window != NULL)
        {
          gdk_window_raise (calendar->window);
        }

      if (!gtk_widget_has_focus (calendar))
        {
          gtk_widget_grab_focus (calendar);
        }

      gtk_widget_event (calendar,
                        ((GdkEvent *)(void*)(event)));

      return TRUE;
    }

  return FALSE;
}

static gboolean
ido_calendar_menu_item_button_release (GtkWidget      *widget,
                                       GdkEventButton *event)
{
  GtkWidget *calendar = IDO_CALENDAR_MENU_ITEM (widget)->priv->calendar;

  gtk_widget_event (calendar,
                    ((GdkEvent *)(void*)(event)));

  return TRUE;
}

static void
ido_calendar_menu_item_select (GtkItem *item)
{
  IDO_CALENDAR_MENU_ITEM (item)->priv->selected = TRUE;

  ido_calendar_menu_item_send_focus_change (GTK_WIDGET (IDO_CALENDAR_MENU_ITEM (item)->priv->calendar), TRUE);
}

static void
ido_calendar_menu_item_deselect (GtkItem *item)
{
  IDO_CALENDAR_MENU_ITEM (item)->priv->selected = FALSE;

  ido_calendar_menu_item_send_focus_change (GTK_WIDGET (IDO_CALENDAR_MENU_ITEM (item)->priv->calendar), FALSE);
}


static void
calendar_realized_cb (GtkWidget        *widget,
                      IdoCalendarMenuItem *item)
{
  if (widget->window != NULL)
    {
      gdk_window_raise (widget->window);
    }

  g_signal_connect (GTK_WIDGET (item)->parent,
                    "key-press-event",
                    G_CALLBACK (ido_calendar_menu_item_key_press),
                    item);
  
  g_signal_connect (item->priv->calendar,
                    "month-changed",
                    G_CALLBACK (calendar_month_changed_cb),
                    item);

  ido_calendar_menu_item_send_focus_change (widget, TRUE);
}

static void
calendar_move_focus_cb (GtkWidget        *widget,
                        GtkDirectionType  direction,
                        IdoCalendarMenuItem *item)
{
  ido_calendar_menu_item_send_focus_change (GTK_WIDGET (IDO_CALENDAR_MENU_ITEM (item)->priv->calendar), FALSE);

  g_signal_emit_by_name (item,
                         "move-focus",
                         GTK_DIR_TAB_FORWARD);
}

static void
calendar_month_changed_cb (GtkWidget        *widget, 
                           gpointer          user_data)
{
  IdoCalendarMenuItem *item = (IdoCalendarMenuItem *)user_data;
  g_signal_emit_by_name (item, "month-changed", NULL);
}

/* Public API */
GtkWidget *
ido_calendar_menu_item_new (void)
{
  return g_object_new (IDO_TYPE_CALENDAR_MENU_ITEM, NULL);
}

gboolean
ido_calendar_menu_item_mark_day	(IdoCalendarMenuItem *menuitem, guint day)
{
  g_return_val_if_fail(IDO_IS_CALENDAR_MENU_ITEM(menuitem), FALSE);
  
  return gtk_calendar_mark_day(GTK_CALENDAR (menuitem->priv->calendar), day);
}

gboolean
ido_calendar_menu_item_unmark_day (IdoCalendarMenuItem *menuitem, guint day)
{
  g_return_val_if_fail(IDO_IS_CALENDAR_MENU_ITEM(menuitem), FALSE);
  
  return gtk_calendar_unmark_day(GTK_CALENDAR (menuitem->priv->calendar), day);
}

void
ido_calendar_menu_item_clear_marks (IdoCalendarMenuItem *menuitem)
{
  g_return_if_fail(IDO_IS_CALENDAR_MENU_ITEM(menuitem));
  
  gtk_calendar_clear_marks(GTK_CALENDAR (menuitem->priv->calendar));
}

void
ido_calendar_menu_item_set_display_options (IdoCalendarMenuItem *menuitem, GtkCalendarDisplayOptions flags)
{
  g_return_if_fail(IDO_IS_CALENDAR_MENU_ITEM(menuitem));
  
  gtk_calendar_set_display_options (GTK_CALENDAR (menuitem->priv->calendar), flags);
}

GtkCalendarDisplayOptions
ido_calendar_menu_item_get_display_options (IdoCalendarMenuItem *menuitem)
{
  g_return_val_if_fail(IDO_IS_CALENDAR_MENU_ITEM(menuitem), 0);
  
  return gtk_calendar_get_display_options (GTK_CALENDAR (menuitem->priv->calendar));
}

void 
ido_calendar_menu_item_get_date (IdoCalendarMenuItem *menuitem,
                                 guint *year,
                                 guint *month,
                                 guint *day) {
	
  g_return_if_fail(IDO_IS_CALENDAR_MENU_ITEM(menuitem));
  gtk_calendar_get_date (GTK_CALENDAR (menuitem->priv->calendar), year, month, day);
}
