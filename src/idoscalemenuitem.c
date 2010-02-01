/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors:
 *    Cody Russell <crussell@canonical.com>
 */

#include <gtk/gtk.h>
#include "idoscalemenuitem.h"

static GObject* ido_scale_menu_item_constructor          (GType                  type,
                                                          guint                  n_construct_properties,
                                                          GObjectConstructParam *construct_params);
static void     ido_scale_menu_item_set_property         (GObject               *object,
                                                          guint                  prop_id,
                                                          const GValue          *value,
                                                          GParamSpec            *pspec);
static void     ido_scale_menu_item_get_property         (GObject               *object,
                                                          guint                  prop_id,
                                                          GValue                *value,
                                                          GParamSpec            *pspec);
static void     ido_scale_menu_item_forall               (GtkContainer          *container,
                                                          gboolean               include_internals,
                                                          GtkCallback            callback,
                                                          gpointer               data);
static gboolean ido_scale_menu_item_button_press_event   (GtkWidget             *menuitem,
                                                          GdkEventButton        *event);
static gboolean ido_scale_menu_item_button_release_event (GtkWidget             *menuitem,
                                                          GdkEventButton        *event);
static gboolean ido_scale_menu_item_motion_notify_event  (GtkWidget             *menuitem,
                                                          GdkEventMotion        *event);
static gboolean ido_scale_menu_item_expose               (GtkWidget             *widget,
                                                          GdkEventExpose        *event);

struct _IdoScaleMenuItemPrivate {
  GtkWidget     *scale;
  GtkAdjustment *adjustment;
  GtkWidget     *offscreen;
};

enum {
  PROP_0,
  PROP_ADJUSTMENT
};

G_DEFINE_TYPE (IdoScaleMenuItem, ido_scale_menu_item, GTK_TYPE_MENU_ITEM)

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), IDO_TYPE_SCALE_MENU_ITEM, IdoScaleMenuItemPrivate))

static void
ido_scale_menu_item_state_changed (GtkWidget *widget,
                                   GtkStateType previous_state)
{
  gtk_widget_set_state (widget, GTK_STATE_NORMAL);
}

static gboolean
ido_scale_menu_item_scroll_event (GtkWidget      *menuitem,
                                  GdkEventScroll *event)
{
  GtkWidget *scale = GET_PRIVATE (IDO_SCALE_MENU_ITEM (menuitem))->scale;

  gtk_widget_event (scale, (GdkEvent *) event);

  return TRUE;
}

static void
ido_scale_menu_item_class_init (IdoScaleMenuItemClass *item_class)
{
  GObjectClass      *gobject_class = G_OBJECT_CLASS (item_class);
  GtkObjectClass    *object_class = GTK_OBJECT_CLASS (item_class);
  GtkWidgetClass    *widget_class = GTK_WIDGET_CLASS (item_class);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (item_class);

  container_class->forall = ido_scale_menu_item_forall;

  widget_class->button_press_event = ido_scale_menu_item_button_press_event;
  widget_class->button_release_event = ido_scale_menu_item_button_release_event;
  widget_class->motion_notify_event = ido_scale_menu_item_motion_notify_event;
  widget_class->scroll_event = ido_scale_menu_item_scroll_event;
  widget_class->state_changed = ido_scale_menu_item_state_changed;
  widget_class->expose_event = ido_scale_menu_item_expose;

  gobject_class->constructor  = ido_scale_menu_item_constructor;
  gobject_class->set_property = ido_scale_menu_item_set_property;
  gobject_class->get_property = ido_scale_menu_item_get_property;

  g_object_class_install_property (gobject_class,
                                   PROP_ADJUSTMENT,
                                   g_param_spec_object ("adjustment",
                                                        "Adjustment",
                                                        "The adjustment containing the scale value",
                                                        GTK_TYPE_ADJUSTMENT,
                                                        G_PARAM_READWRITE));

  g_type_class_add_private (object_class, sizeof (IdoScaleMenuItemPrivate));
}

static void
ido_scale_menu_item_init (IdoScaleMenuItem *self)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (self);

  priv->scale = NULL;
  priv->adjustment = NULL;
}

static void
ido_scale_menu_item_set_property (GObject         *object,
                                  guint            prop_id,
                                  const GValue    *value,
                                  GParamSpec      *pspec)
{
  IdoScaleMenuItem *menu_item = IDO_SCALE_MENU_ITEM (object);
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (menu_item);

  switch (prop_id)
    {
    case PROP_ADJUSTMENT:
      gtk_range_set_adjustment (GTK_RANGE (priv->scale), g_value_get_object (value));

      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
ido_scale_menu_item_get_property (GObject         *object,
                                  guint            prop_id,
                                  GValue          *value,
                                  GParamSpec      *pspec)
{
  IdoScaleMenuItem *menu_item = IDO_SCALE_MENU_ITEM (object);
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (menu_item);
  GtkAdjustment *adjustment = gtk_range_get_adjustment (GTK_RANGE (priv->scale));

  switch (prop_id)
    {
    case PROP_ADJUSTMENT:
      g_value_set_object (value, adjustment);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static gboolean
offscreen_damage (GtkWidget      *widget,
                  GdkEventExpose *event,
                  GtkWidget      *menuitem)
{
  gtk_widget_queue_draw (menuitem);

  return TRUE;
}

static gboolean
ido_scale_menu_item_expose (GtkWidget      *widget,
                            GdkEventExpose *event)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (widget);

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      GdkPixbuf *pixbuf = gtk_offscreen_window_get_pixbuf (GTK_OFFSCREEN_WINDOW (priv->offscreen));

      gdk_draw_pixbuf (widget->window,
                       widget->style->black_gc,
                       pixbuf,
                       0, 0,
                       widget->allocation.x, widget->allocation.y,
                       gdk_pixbuf_get_width (pixbuf),
                       gdk_pixbuf_get_height (pixbuf),
                       GDK_RGB_DITHER_NORMAL,
                       0, 0);

      g_object_unref (pixbuf);
    }

  return TRUE;
}

static GObject*
ido_scale_menu_item_constructor (GType                  type,
                                 guint                  n_construct_properties,
                                 GObjectConstructParam *construct_params)
{
  GObject *object;
  IdoScaleMenuItemPrivate *priv;

  object = G_OBJECT_CLASS (ido_scale_menu_item_parent_class)->constructor (type,
                                                                           n_construct_properties,
                                                                           construct_params);

  priv = GET_PRIVATE (object);

  priv->offscreen = gtk_offscreen_window_new ();

  priv->scale = gtk_hscale_new_with_range (0.0, 100.0, 1.0);
  gtk_scale_set_draw_value (GTK_SCALE (priv->scale), FALSE);
  gtk_widget_set_size_request (priv->scale, 95, 21);
  gtk_container_add (GTK_CONTAINER (priv->offscreen), priv->scale);
  gtk_widget_show (priv->scale);
  gtk_widget_show (priv->offscreen);

  gtk_widget_queue_draw (priv->offscreen);

  g_signal_connect (priv->offscreen,
                    "damage-event",
                    G_CALLBACK (offscreen_damage),
                    object);

  return object;
}

static void
ido_scale_menu_item_forall (GtkContainer *container,
                            gboolean      include_internals,
                            GtkCallback   callback,
                            gpointer      data)
{
  IdoScaleMenuItem *item = IDO_SCALE_MENU_ITEM (container);
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (item);

  GTK_CONTAINER_CLASS (ido_scale_menu_item_parent_class)->forall (container,
                                                                  include_internals,
                                                                  callback,
                                                                  data);

  if (include_internals && priv->scale)
    (* callback) (priv->scale, data);
}

static gboolean
ido_scale_menu_item_button_press_event (GtkWidget *menuitem,
                                        GdkEventButton *event)
{
  GtkWidget *scale = GET_PRIVATE (IDO_SCALE_MENU_ITEM (menuitem))->scale;

  gtk_widget_event (scale, (GdkEvent *)event);

  return TRUE;
}

static gboolean
ido_scale_menu_item_button_release_event (GtkWidget *menuitem,
                                          GdkEventButton *event)
{
  GtkWidget *scale = GET_PRIVATE (IDO_SCALE_MENU_ITEM (menuitem))->scale;
  GdkWindow *tmp = event->window;

  event->window = GTK_RANGE (scale)->event_window;

  gtk_widget_event (scale, (GdkEvent *) event);

  event->window = tmp;

  return TRUE;
}

static gboolean
ido_scale_menu_item_motion_notify_event (GtkWidget *menuitem,
                                         GdkEventMotion *event)
{
  GtkWidget *scale = GET_PRIVATE (IDO_SCALE_MENU_ITEM (menuitem))->scale;
  gtk_widget_event (scale, (GdkEvent *) event);

  return TRUE;
}


/**
 * ido_scale_menu_item_new:
 * @label: the text of the new menu item.
 * @adjustment: A #GtkAdjustment describing the slider value.
 * @returns: a new #IdoScaleMenuItem.
 *
 * Creates a new #IdoScaleMenuItem with an empty label.
 **/
GtkWidget*
ido_scale_menu_item_new (const gchar   *label,
                         GtkAdjustment *adjustment)
{
  return g_object_new (IDO_TYPE_SCALE_MENU_ITEM,
                       "adjustment", adjustment,
                       NULL);
}

/**
 * ido_scale_menu_item_new_with_label:
 * @label: the text of the menu item.
 * @min: The minimum value of the slider.
 * @max: The maximum value of the slider.
 * @step: The step increment of the slider.
 * @returns: a new #IdoScaleMenuItem.
 *
 * Creates a new #IdoScaleMenuItem containing a label.
 **/
GtkWidget*
ido_scale_menu_item_new_with_range (const gchar *label,
                                    gdouble      min,
                                    gdouble      max,
                                    gdouble      step)
{
  GtkObject *adjustment = gtk_adjustment_new (min, min, max, step, 10 * step, 0);

  return g_object_new (IDO_TYPE_SCALE_MENU_ITEM,
                       "label", label,
                       "adjustment", adjustment,
		       NULL);
}

GtkWidget*
ido_scale_menu_item_get_scale (IdoScaleMenuItem *menuitem)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_val_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem), NULL);

  priv = GET_PRIVATE (menuitem);

  return priv->scale;
}

#define __IDO_SCALE_MENU_ITEM_C__
