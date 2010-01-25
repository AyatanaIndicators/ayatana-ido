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

static void     ido_scale_menu_item_size_request         (GtkWidget             *widget,
                                                          GtkRequisition        *requisition);
static void     ido_scale_menu_item_size_allocate        (GtkWidget             *widget,
                                                          GtkAllocation         *allocation);
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
static void     ido_scale_menu_item_construct_child      (IdoScaleMenuItem      *item);
static gboolean ido_scale_menu_item_button_press_event   (GtkWidget             *menuitem,
                                                          GdkEventButton        *event);
static gboolean ido_scale_menu_item_button_release_event (GtkWidget             *menuitem,
                                                          GdkEventButton        *event);
static gboolean ido_scale_menu_item_motion_notify_event  (GtkWidget             *menuitem,
                                                          GdkEventMotion        *event);

struct _IdoScaleMenuItemPrivate {
  GtkWidget     *scale;
  GtkAdjustment *adjustment;
};

enum {
  PROP_0,
  PROP_MIN,
  PROP_MAX,
  PROP_STEP,
  PROP_VALUE,
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

  widget_class->size_request = ido_scale_menu_item_size_request;
  widget_class->size_allocate = ido_scale_menu_item_size_allocate;

  widget_class->button_press_event = ido_scale_menu_item_button_press_event;
  widget_class->button_release_event = ido_scale_menu_item_button_release_event;
  widget_class->motion_notify_event = ido_scale_menu_item_motion_notify_event;
  widget_class->scroll_event = ido_scale_menu_item_scroll_event;
  widget_class->state_changed = ido_scale_menu_item_state_changed;

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
      if (!priv->scale)
        {
          priv->adjustment = g_value_get_object (value);
          priv->scale = gtk_hscale_new (priv->adjustment);
          gtk_scale_set_draw_value (GTK_SCALE (priv->scale), FALSE);
        }

      ido_scale_menu_item_construct_child (menu_item);

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

  if (priv->scale != NULL)
    {
      ido_scale_menu_item_construct_child (IDO_SCALE_MENU_ITEM (object));
    }

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

static void
ido_scale_menu_item_size_request (GtkWidget      *widget,
                                  GtkRequisition *requisition)
{
  IdoScaleMenuItem *item;
  IdoScaleMenuItemPrivate *priv;
  GtkWidget *parent;
  GtkPackDirection dir;
  gint child_width = 0;
  gint child_height = 0;

  parent = gtk_widget_get_parent (widget);
  item = IDO_SCALE_MENU_ITEM (widget);
  priv = GET_PRIVATE (item);

  if (GTK_IS_MENU_BAR (parent))
    dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    dir = GTK_PACK_DIRECTION_LTR;

  if (priv->scale && GTK_WIDGET_VISIBLE (priv->scale))
    {
      GtkRequisition child_requisition;

      gtk_widget_size_request (priv->scale,
                               &child_requisition);

      child_width = child_requisition.width;
      child_height = child_requisition.height;
    }

  GTK_WIDGET_CLASS (ido_scale_menu_item_parent_class)->size_request (widget, requisition);

  if (dir == GTK_PACK_DIRECTION_LTR || dir == GTK_PACK_DIRECTION_RTL)
    requisition->height = MAX (requisition->height, child_height);
  else
    requisition->width = MAX (requisition->width, child_width);
}

static void
ido_scale_menu_item_size_allocate (GtkWidget     *widget,
                                   GtkAllocation *allocation)
{
  IdoScaleMenuItem *item;
  IdoScaleMenuItemPrivate *priv;
  GtkWidget *parent;
  GtkPackDirection dir;

  item = IDO_SCALE_MENU_ITEM (widget);
  priv = GET_PRIVATE (item);
  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    dir = GTK_PACK_DIRECTION_LTR;

  GTK_WIDGET_CLASS (ido_scale_menu_item_parent_class)->size_allocate (widget, allocation);

  if (priv->scale && GTK_WIDGET_VISIBLE (priv->scale))
    {
      GtkRequisition child_requisition;
      GtkAllocation  child_allocation;
      gint x, y, offset;

      gtk_widget_get_child_requisition (priv->scale,
                                        &child_requisition);

      if (dir == GTK_PACK_DIRECTION_LTR || dir == GTK_PACK_DIRECTION_RTL)
        {
          offset = GTK_CONTAINER (item)->border_width + widget->style->xthickness;

          if ((gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR) ==
              (dir == GTK_PACK_DIRECTION_LTR))
            {
              x = offset + (GTK_MENU_ITEM (item)->toggle_size - child_requisition.width) / 2;
            }
          else
            {
              // TODO rtl
            }
        }
      else
        {
          offset = GTK_CONTAINER (item)->border_width + widget->style->ythickness;

          // TODO vertical
        }

      y = widget->allocation.height / 2 - child_requisition.height / 2;

      child_allocation.width  = allocation->width;
      child_allocation.height = child_requisition.height;
      child_allocation.x    = widget->allocation.x + MAX (x, 0);
      child_allocation.y    = widget->allocation.y + MAX (y, 0);

      gtk_widget_size_allocate (priv->scale, &child_allocation);
    }
}

static gboolean
ido_scale_menu_item_button_press_event (GtkWidget *menuitem,
                                        GdkEventButton *event)
{
  GtkWidget *scale = GET_PRIVATE (IDO_SCALE_MENU_ITEM (menuitem))->scale;

  event->x = 0;
  event->y = 0;

  //gtk_grab_add (scale);
  //gtk_grab_remove (menuitem->parent);

  gtk_widget_event (scale, (GdkEvent *)event);

  //g_signal_stop_emission_by_name (menuitem->parent, "hide");

  return FALSE;
}

static gboolean
ido_scale_menu_item_button_release_event (GtkWidget *menuitem,
                                          GdkEventButton *event)
{
  GtkWidget *scale = GET_PRIVATE (IDO_SCALE_MENU_ITEM (menuitem))->scale;

  //gtk_grab_remove (scale);

  gtk_widget_event (scale, (GdkEvent *) event);

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

static void
ido_scale_menu_item_construct_child (IdoScaleMenuItem *item)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (item);
  GtkWidget *scale = NULL;

  if (priv->scale)
    {
      scale = g_object_ref (priv->scale);
      if (scale->parent)
        {
          gtk_container_remove (GTK_CONTAINER (scale->parent), scale);
        }
    }

  priv->scale = NULL;

  priv->scale = gtk_hscale_new (priv->adjustment);
  gtk_widget_set_can_focus (priv->scale, TRUE);
  gtk_scale_set_draw_value (GTK_SCALE (priv->scale), FALSE);

  if (GTK_BIN (item)->child)
    gtk_container_remove (GTK_CONTAINER (item),
                          GTK_BIN (item)->child);

  if (scale)
    {
      priv->scale = scale;
      g_object_set (priv->scale,
                    "visible", TRUE,
                    "no-show-all", TRUE,
                    NULL);

      gtk_container_add (GTK_CONTAINER (item), scale);
      gtk_widget_show_all (scale);

      g_object_unref (scale);

      return;
    }
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
