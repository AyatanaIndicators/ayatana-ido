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

#include <gtk/gtk.h>
#include "idoscalemenuitem.h"

static void     ido_scale_menu_item_set_property           (GObject               *object,
                                                            guint                  prop_id,
                                                            const GValue          *value,
                                                            GParamSpec            *pspec);
static void     ido_scale_menu_item_get_property           (GObject               *object,
                                                            guint                  prop_id,
                                                            GValue                *value,
                                                            GParamSpec            *pspec);
static gboolean ido_scale_menu_item_button_press_event     (GtkWidget             *menuitem,
                                                            GdkEventButton        *event);
static gboolean ido_scale_menu_item_button_release_event   (GtkWidget             *menuitem,
                                                            GdkEventButton        *event);
static gboolean ido_scale_menu_item_motion_notify_event    (GtkWidget             *menuitem,
                                                            GdkEventMotion        *event);
static void     ido_scale_menu_item_primary_image_notify   (GtkImage              *image,
                                                            GParamSpec            *pspec,
                                                            IdoScaleMenuItem      *item);
static void     ido_scale_menu_item_secondary_image_notify (GtkImage              *image,
                                                            GParamSpec            *pspec,
                                                            IdoScaleMenuItem      *item);
static void     ido_scale_menu_item_notify                 (IdoScaleMenuItem      *item,
                                                            GParamSpec            *pspec,
                                                            gpointer               user_data);

struct _IdoScaleMenuItemPrivate {
  GtkWidget     *scale;
  GtkAdjustment *adjustment;
  GtkWidget     *primary_image;
  GtkWidget     *secondary_image;
  GtkWidget     *hbox;
  GtkAllocation  child_allocation;
  gdouble        left_padding;
  gdouble        right_padding;
  gboolean       reverse_scroll;
  gboolean       grabbed;
};

enum {
  SLIDER_GRABBED,
  SLIDER_RELEASED,
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_ADJUSTMENT,
  PROP_REVERSE_SCROLL_EVENTS
};

static guint signals[LAST_SIGNAL] = { 0 };

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
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (menuitem);
  GtkWidget *scale = priv->scale;

  if (priv->reverse_scroll)
    {
      switch (event->direction)
        {
        case GDK_SCROLL_UP:
          event->direction = GDK_SCROLL_DOWN;
          break;

        case GDK_SCROLL_DOWN:
          event->direction = GDK_SCROLL_UP;
          break;

        default:
          break;
        }
    }

  gtk_widget_event (scale,
                    ((GdkEvent *)(void*)(event)));

  return TRUE;
}

static void
ido_scale_menu_item_size_allocate (GtkWidget     *widget,
                                   GtkAllocation *allocation)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (widget);
  GtkRequisition primary_image_req;
  GtkRequisition secondary_image_req;
  gint horizontal_padding;
  gint primary_padding, secondary_padding;

  GTK_WIDGET_CLASS (ido_scale_menu_item_parent_class)->size_allocate (widget, allocation);

  gtk_widget_get_child_requisition (priv->primary_image, &primary_image_req);
  gtk_widget_get_child_requisition (priv->secondary_image, &secondary_image_req);

  gtk_widget_style_get (widget,
                        "horizontal-padding", &horizontal_padding,
                        NULL);

  primary_padding = gtk_widget_get_visible (priv->primary_image) ? primary_image_req.width : 0;
  secondary_padding = gtk_widget_get_visible (priv->secondary_image) ? secondary_image_req.width : 0;

  priv->left_padding = gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR ? primary_padding : secondary_padding;

  if (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR)
    {
      priv->left_padding = primary_padding;
      priv->right_padding = secondary_padding;
    }
  else
    {
      priv->left_padding = secondary_padding;
      priv->right_padding = primary_padding;
    }

  priv->child_allocation.x = GTK_CONTAINER (widget)->border_width + widget->style->xthickness;
  priv->child_allocation.y = GTK_CONTAINER (widget)->border_width + widget->style->ythickness;

  priv->child_allocation.x += horizontal_padding;
  priv->child_allocation.x += GTK_MENU_ITEM (widget)->toggle_size;

  priv->child_allocation.width = MAX (1, (gint)allocation->width - priv->child_allocation.x * 2);
  priv->child_allocation.width -= (primary_padding + secondary_padding);
  priv->child_allocation.height = MAX (1, (gint)allocation->height - priv->child_allocation.y * 2);

  gtk_widget_set_size_request (priv->scale, priv->child_allocation.width, -1);
}

static void
ido_scale_menu_item_class_init (IdoScaleMenuItemClass *item_class)
{
  GObjectClass      *gobject_class = G_OBJECT_CLASS (item_class);
  GtkObjectClass    *object_class = GTK_OBJECT_CLASS (item_class);
  GtkWidgetClass    *widget_class = GTK_WIDGET_CLASS (item_class);

  widget_class->button_press_event = ido_scale_menu_item_button_press_event;
  widget_class->button_release_event = ido_scale_menu_item_button_release_event;
  widget_class->motion_notify_event = ido_scale_menu_item_motion_notify_event;
  widget_class->scroll_event = ido_scale_menu_item_scroll_event;
  widget_class->state_changed = ido_scale_menu_item_state_changed;
  widget_class->size_allocate = ido_scale_menu_item_size_allocate;

  gobject_class->set_property = ido_scale_menu_item_set_property;
  gobject_class->get_property = ido_scale_menu_item_get_property;

  g_object_class_install_property (gobject_class,
                                   PROP_ADJUSTMENT,
                                   g_param_spec_object ("adjustment",
                                                        "Adjustment",
                                                        "The adjustment containing the scale value",
                                                        GTK_TYPE_ADJUSTMENT,
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_REVERSE_SCROLL_EVENTS,
                                   g_param_spec_boolean ("reverse-scroll-events",
                                                         "Reverse scroll events",
                                                         "Reverses how up/down scroll events are interpreted",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

  signals[SLIDER_GRABBED] = g_signal_new ("slider-grabbed",
                                          G_OBJECT_CLASS_TYPE (gobject_class),
                                          G_SIGNAL_RUN_FIRST,
                                          0,
                                          NULL, NULL,
                                          g_cclosure_marshal_VOID__VOID,
                                          G_TYPE_NONE, 0);

  signals[SLIDER_RELEASED] = g_signal_new ("slider-released",
                                           G_OBJECT_CLASS_TYPE (gobject_class),
                                           G_SIGNAL_RUN_FIRST,
                                           0,
                                           NULL, NULL,
                                           g_cclosure_marshal_VOID__VOID,
                                           G_TYPE_NONE, 0);

  g_type_class_add_private (object_class, sizeof (IdoScaleMenuItemPrivate));
}

static void
ido_scale_menu_item_init (IdoScaleMenuItem *self)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (self);
  GtkWidget *hbox;

  priv->adjustment = NULL;

  priv->scale = gtk_hscale_new_with_range (0.0, 100.0, 1.0);
  gtk_scale_set_draw_value (GTK_SCALE (priv->scale), FALSE);

  hbox = gtk_hbox_new (FALSE, 0);

  priv->primary_image = gtk_image_new ();
  g_signal_connect (priv->primary_image, "notify",
                    G_CALLBACK (ido_scale_menu_item_primary_image_notify),
                    self);

  priv->secondary_image = gtk_image_new ();
  g_signal_connect (priv->secondary_image, "notify",
                    G_CALLBACK (ido_scale_menu_item_secondary_image_notify),
                    self);

  gtk_box_pack_start (GTK_BOX (hbox), priv->primary_image, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), priv->scale, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), priv->secondary_image, FALSE, FALSE, 0);

  priv->hbox = hbox;

  gtk_widget_show_all (priv->hbox);

  g_signal_connect (self, "notify",
                    G_CALLBACK (ido_scale_menu_item_notify),
                    NULL);

  gtk_container_add (GTK_CONTAINER (self), hbox);
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

    case PROP_REVERSE_SCROLL_EVENTS:
      priv->reverse_scroll = g_value_get_boolean (value);
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

    case PROP_REVERSE_SCROLL_EVENTS:
      g_value_set_boolean (value, priv->reverse_scroll);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
translate_event_coordinates (GtkWidget *widget,
                             gdouble    in_x,
                             gdouble   *out_x)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (widget);

  *out_x = in_x - priv->child_allocation.x - priv->left_padding;
}

static gboolean
ido_scale_menu_item_button_press_event (GtkWidget      *menuitem,
                                        GdkEventButton *event)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (menuitem);
  GtkWidget *scale = priv->scale;
  GtkWidget *parent;
  gdouble x;

  // can we block emissions of "grab-notify" on parent??
  parent = gtk_widget_get_parent (GTK_WIDGET (menuitem));

  translate_event_coordinates (menuitem, event->x, &x);
  event->x = x;

  translate_event_coordinates (menuitem, event->x_root, &x);
  event->x_root = x;

  GTK_OBJECT_FLAGS (scale) |= GTK_HAS_GRAB;

  gtk_widget_event (scale,
                    ((GdkEvent *)(void*)(event)));

  GTK_OBJECT_FLAGS (scale) &= ~(GTK_HAS_GRAB);

  if (!priv->grabbed)
    {
      priv->grabbed = TRUE;
      g_signal_emit (menuitem, signals[SLIDER_GRABBED], 0);
    }

  return TRUE;
}

static gboolean
ido_scale_menu_item_button_release_event (GtkWidget *menuitem,
                                          GdkEventButton *event)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (menuitem);
  GtkWidget *scale = priv->scale;
  GdkWindow *tmp = event->window;
  gdouble x;

  if (event->x > priv->child_allocation.x &&
      event->x < priv->child_allocation.x + priv->left_padding)
    {
      GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (priv->scale));

      if (gtk_widget_get_direction (menuitem) == GTK_TEXT_DIR_LTR)
        {
          gtk_adjustment_set_value (adj, gtk_adjustment_get_lower (adj));
        }
      else
        {
          gtk_adjustment_set_value (adj, gtk_adjustment_get_upper (adj));
        }

      if (priv->grabbed)
        {
          priv->grabbed = FALSE;
          g_signal_emit (menuitem, signals[SLIDER_RELEASED], 0);
        }

      return TRUE;
    }

  if (event->x < priv->child_allocation.x + priv->child_allocation.width + priv->right_padding + priv->left_padding &&
      event->x > priv->child_allocation.x + priv->child_allocation.width + priv->left_padding)
    {
      GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (priv->scale));

      if (gtk_widget_get_direction (menuitem) == GTK_TEXT_DIR_LTR)
        {
          gtk_adjustment_set_value (adj, gtk_adjustment_get_upper (adj));
        }
      else
        {
          gtk_adjustment_set_value (adj, gtk_adjustment_get_lower (adj));
        }

      if (priv->grabbed)
        {
          priv->grabbed = FALSE;
          g_signal_emit (menuitem, signals[SLIDER_RELEASED], 0);
        }

      return TRUE;
    }

  event->window = GTK_RANGE (scale)->event_window;

  translate_event_coordinates (menuitem, event->x, &x);
  event->x = x;

  translate_event_coordinates (menuitem, event->x_root, &x);
  event->x_root= x;

  gtk_widget_event (scale,
                    ((GdkEvent *)(void*)(event)));

  event->window = tmp;

  if (priv->grabbed)
    {
      priv->grabbed = FALSE;
      g_signal_emit (menuitem, signals[SLIDER_RELEASED], 0);
    }

  return TRUE;
}

static gboolean
ido_scale_menu_item_motion_notify_event (GtkWidget      *menuitem,
                                         GdkEventMotion *event)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (menuitem);
  GtkWidget *scale = priv->scale;
  gdouble x;

  translate_event_coordinates (menuitem, event->x, &x);
  event->x = x;

  translate_event_coordinates (menuitem, event->x_root, &x);
  event->x_root= x;

  gtk_widget_event (scale,
                    ((GdkEvent *)(void*)(event)));

  return TRUE;
}

static void
menu_hidden (GtkWidget        *menu,
             IdoScaleMenuItem *scale)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (scale);

  if (priv->grabbed)
    {
      priv->grabbed = FALSE;
      g_signal_emit (scale, signals[SLIDER_RELEASED], 0);
    }
}

static void
ido_scale_menu_item_notify (IdoScaleMenuItem *item,
                            GParamSpec       *pspec,
                            gpointer          user_data)
{
  if (g_strcmp0 (pspec->name, "parent"))
    {
      GtkWidget *parent = gtk_widget_get_parent (GTK_WIDGET (item));

      if (parent)
        {
          g_signal_connect (parent, "hide",
                            G_CALLBACK (menu_hidden),
                            item);
        }
    }
}

static void
ido_scale_menu_item_primary_image_notify (GtkImage         *image,
                                          GParamSpec       *pspec,
                                          IdoScaleMenuItem *item)
{
  if (gtk_image_get_storage_type (image) == GTK_IMAGE_EMPTY)
    gtk_widget_hide (GTK_WIDGET (image));
  else
    gtk_widget_show (GTK_WIDGET (image));
}

static void
ido_scale_menu_item_secondary_image_notify (GtkImage         *image,
                                            GParamSpec       *pspec,
                                            IdoScaleMenuItem *item)
{
  if (gtk_image_get_storage_type (image) == GTK_IMAGE_EMPTY)
    gtk_widget_hide (GTK_WIDGET (image));
  else
    gtk_widget_show (GTK_WIDGET (image));
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
                                    gdouble      value,
                                    gdouble      min,
                                    gdouble      max,
                                    gdouble      step)
{
  GtkObject *adjustment = gtk_adjustment_new (value, min, max, step, 10 * step, 0);

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

GtkWidget *
ido_scale_menu_item_get_primary_image (IdoScaleMenuItem *menuitem)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_val_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem), NULL);

  priv = GET_PRIVATE (menuitem);

  return priv->primary_image;
}

GtkWidget *
ido_scale_menu_item_get_secondary_image (IdoScaleMenuItem *menuitem)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_val_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem), NULL);

  priv = GET_PRIVATE (menuitem);

  return priv->secondary_image;
}


#define __IDO_SCALE_MENU_ITEM_C__
