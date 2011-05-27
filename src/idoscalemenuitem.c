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
#include "idorange.h"
#include "idoscalemenuitem.h"
#include "idotypebuiltins.h"

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
static void     update_packing                             (IdoScaleMenuItem      *self,
                                                            IdoScaleMenuItemStyle  style,
                                                            IdoScaleMenuItemStyle  old_style);

struct _IdoScaleMenuItemPrivate {
  GtkWidget            *scale;
  GtkAdjustment        *adjustment;
  GtkWidget            *primary_image;
  GtkWidget            *secondary_image;
  GtkWidget            *primary_label;
  GtkWidget            *secondary_label;
  GtkWidget            *hbox;
  GtkAllocation         child_allocation;
  gdouble               left_padding;
  gdouble               right_padding;
  gboolean              reverse_scroll;
  gboolean              grabbed;
  IdoScaleMenuItemStyle style;
  IdoRangeStyle         range_style;
  gint                  toggle_size;
};

enum {
  SLIDER_GRABBED,
  SLIDER_RELEASED,
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_ADJUSTMENT,
  PROP_REVERSE_SCROLL_EVENTS,
  PROP_STYLE,
  PROP_RANGE_STYLE
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
  GtkRequisition primary_req;
  GtkRequisition secondary_req;
  gint horizontal_padding;
  gint primary_padding, secondary_padding;

  GTK_WIDGET_CLASS (ido_scale_menu_item_parent_class)->size_allocate (widget, allocation);

  switch (priv->style)
    {
    case IDO_SCALE_MENU_ITEM_STYLE_IMAGE:
      gtk_widget_get_child_requisition (priv->primary_image, &primary_req);
      gtk_widget_get_child_requisition (priv->secondary_image, &secondary_req);

      primary_padding = gtk_widget_get_visible (priv->primary_image) ? primary_req.width : 0;
      secondary_padding = gtk_widget_get_visible (priv->secondary_image) ? secondary_req.width : 0;
      break;

    case IDO_SCALE_MENU_ITEM_STYLE_LABEL:
      gtk_widget_get_child_requisition (priv->primary_label, &primary_req);
      gtk_widget_get_child_requisition (priv->secondary_label, &secondary_req);

      primary_padding = gtk_widget_get_visible (priv->primary_label) ? primary_req.width : 0;
      secondary_padding = gtk_widget_get_visible (priv->secondary_label) ? secondary_req.width : 0;
      break;

    default:
      primary_req.width = primary_req.height = 0;
      secondary_req.width = secondary_req.height = 0;
      primary_padding = 0;
      secondary_padding = 0;
      break;
    }

  gtk_widget_style_get (widget,
                        "horizontal-padding", &horizontal_padding,
                        NULL);

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
  priv->child_allocation.x += priv->toggle_size;

  priv->child_allocation.width = MAX (1, (gint)allocation->width - priv->child_allocation.x * 2);
  priv->child_allocation.width -= (primary_padding + secondary_padding);
  priv->child_allocation.height = MAX (1, (gint)allocation->height - priv->child_allocation.y * 2);

  gtk_widget_set_size_request (priv->scale, priv->child_allocation.width, -1);
}

static void
ido_scale_menu_item_toggle_size_allocate (IdoScaleMenuItem *item,
                                          gint              toggle_size,
                                          gpointer          user_data)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (item);

  priv->toggle_size = toggle_size;
}

static void
ido_scale_menu_item_constructed (GObject *object)
{
  IdoScaleMenuItem *self = IDO_SCALE_MENU_ITEM (object);
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (self);
  GtkWidget *adj = gtk_adjustment_new (0.0, 0.0, 100.0, 1.0, 10.0, 0.0);
  IdoRangeStyle range_style;
  GtkWidget *hbox;

  priv->adjustment  = NULL;

  g_object_get (self,
                "range-style", &range_style,
                NULL);

  priv->scale = ido_range_new (adj, range_style);
  g_object_ref (priv->scale);
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

  priv->primary_label = gtk_label_new ("");
  priv->secondary_label = gtk_label_new ("");

  priv->hbox = hbox;

  update_packing (self, priv->style, priv->style);

  g_signal_connect (self, "toggle-size-allocate",
                    G_CALLBACK (ido_scale_menu_item_toggle_size_allocate),
                    NULL);

  g_signal_connect (self, "notify",
                    G_CALLBACK (ido_scale_menu_item_notify),
                    NULL);

  gtk_container_add (GTK_CONTAINER (self), hbox);
}

static void
ido_scale_menu_item_class_init (IdoScaleMenuItemClass *item_class)
{
  GObjectClass      *gobject_class = G_OBJECT_CLASS (item_class);
  GtkWidgetClass    *object_class = GTK_OBJECT_CLASS (item_class);
  GtkWidgetClass    *widget_class = GTK_WIDGET_CLASS (item_class);

  widget_class->button_press_event   = ido_scale_menu_item_button_press_event;
  widget_class->button_release_event = ido_scale_menu_item_button_release_event;
  widget_class->motion_notify_event  = ido_scale_menu_item_motion_notify_event;
  widget_class->scroll_event         = ido_scale_menu_item_scroll_event;
  widget_class->state_changed        = ido_scale_menu_item_state_changed;
  widget_class->size_allocate        = ido_scale_menu_item_size_allocate;

  gobject_class->constructed  = ido_scale_menu_item_constructed;
  gobject_class->set_property = ido_scale_menu_item_set_property;
  gobject_class->get_property = ido_scale_menu_item_get_property;

  g_object_class_install_property (gobject_class,
                                   PROP_STYLE,
                                   g_param_spec_enum ("accessory-style",
                                                      "Style of primary/secondary widgets",
                                                      "The style of the primary/secondary widgets",
                                                      IDO_TYPE_SCALE_MENU_ITEM_STYLE,
                                                      IDO_SCALE_MENU_ITEM_STYLE_NONE,
                                                      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_RANGE_STYLE,
                                   g_param_spec_enum ("range-style",
                                                      "Range style",
                                                      "Style of the range",
                                                      IDO_TYPE_RANGE_STYLE,
                                                      IDO_RANGE_STYLE_DEFAULT,
                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

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
update_packing (IdoScaleMenuItem *self, IdoScaleMenuItemStyle style, IdoScaleMenuItemStyle old_style)
{
  IdoScaleMenuItemPrivate *priv = GET_PRIVATE (self);
  GtkContainer *container = GTK_CONTAINER (priv->hbox);

  if (style != old_style)
    {
      switch (old_style)
        {
        case IDO_SCALE_MENU_ITEM_STYLE_NONE:
          gtk_container_remove (container, priv->scale);
          break;

        case IDO_SCALE_MENU_ITEM_STYLE_IMAGE:
          gtk_container_remove (container, priv->primary_image);
          gtk_container_remove (container, priv->secondary_image);
          gtk_container_remove (container, priv->scale);
          break;

        case IDO_SCALE_MENU_ITEM_STYLE_LABEL:
          gtk_container_remove (container, priv->primary_label);
          gtk_container_remove (container, priv->secondary_label);
          gtk_container_remove (container, priv->scale);
          break;

        default:
          gtk_container_remove (container, priv->scale);
          break;
        }
    }

  switch (style)
    {
    case IDO_SCALE_MENU_ITEM_STYLE_NONE:
      gtk_box_pack_start (GTK_BOX (priv->hbox), priv->scale, FALSE, FALSE, 0);
      break;

    case IDO_SCALE_MENU_ITEM_STYLE_IMAGE:
      gtk_box_pack_start (GTK_BOX (priv->hbox), priv->primary_image, FALSE, FALSE, 0);
      gtk_box_pack_start (GTK_BOX (priv->hbox), priv->scale, FALSE, FALSE, 0);
      gtk_box_pack_start (GTK_BOX (priv->hbox), priv->secondary_image, FALSE, FALSE, 0);
      break;

    case IDO_SCALE_MENU_ITEM_STYLE_LABEL:
      gtk_box_pack_start (GTK_BOX (priv->hbox), priv->primary_label, FALSE, FALSE, 0);
      gtk_box_pack_start (GTK_BOX (priv->hbox), priv->scale, FALSE, FALSE, 0);
      gtk_box_pack_start (GTK_BOX (priv->hbox), priv->secondary_label, FALSE, FALSE, 0);
      break;

    default:
      gtk_box_pack_start (GTK_BOX (priv->hbox), priv->scale, FALSE, FALSE, 0);
      break;
    }

  gtk_widget_show_all (priv->hbox);
}

static void
ido_scale_menu_item_init (IdoScaleMenuItem *self)
{
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

    case PROP_STYLE:
      ido_scale_menu_item_set_style (menu_item, g_value_get_enum (value));
      break;

    case PROP_RANGE_STYLE:
      priv->range_style = g_value_get_enum (value);
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
  GtkAdjustment *adjustment;

  switch (prop_id)
    {
    case PROP_ADJUSTMENT:
      adjustment = gtk_range_get_adjustment (GTK_RANGE (priv->scale));
      g_value_set_object (value, adjustment);
      break;

    case PROP_REVERSE_SCROLL_EVENTS:
      g_value_set_boolean (value, priv->reverse_scroll);
      break;

    case PROP_RANGE_STYLE:
      g_value_set_enum (value, priv->range_style);
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

  ubuntu_gtk_widget_set_has_grab (scale, TRUE);

  gtk_widget_event (scale,
                    ((GdkEvent *)(void*)(event)));

  ubuntu_gtk_widget_set_has_grab (scale, FALSE);

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
 * @size: The size style of the range.
 * @adjustment: A #GtkAdjustment describing the slider value.
 * @returns: a new #IdoScaleMenuItem.
 *
 * Creates a new #IdoScaleMenuItem with an empty label.
 **/
GtkWidget*
ido_scale_menu_item_new (const gchar   *label,
                         IdoRangeStyle  range_style,
                         GtkAdjustment *adjustment)
{
  return g_object_new (IDO_TYPE_SCALE_MENU_ITEM,
                       "adjustment",  adjustment,
                       "range-style", range_style,
                       NULL);
}

/**
 * ido_scale_menu_item_new_with_label:
 * @label: the text of the menu item.
 * @size: The size style of the range.
 * @min: The minimum value of the slider.
 * @max: The maximum value of the slider.
 * @step: The step increment of the slider.
 * @returns: a new #IdoScaleMenuItem.
 *
 * Creates a new #IdoScaleMenuItem containing a label.
 **/
GtkWidget*
ido_scale_menu_item_new_with_range (const gchar  *label,
                                    IdoRangeStyle range_style,
                                    gdouble       value,
                                    gdouble       min,
                                    gdouble       max,
                                    gdouble       step)
{
  GtkWidget *adjustment = gtk_adjustment_new (value, min, max, step, 10 * step, 0);

  return g_object_new (IDO_TYPE_SCALE_MENU_ITEM,
                       "label",       label,
                       "range-style", range_style,
                       "adjustment",  adjustment,
		       NULL);
}

/**
 * ido_scale_menu_item_get_scale:
 * @menuitem: The #IdoScaleMenuItem
 * @returns: A pointer to the scale widget.
 *
 * Retrieves the scale widget.
 **/
GtkWidget*
ido_scale_menu_item_get_scale (IdoScaleMenuItem *menuitem)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_val_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem), NULL);

  priv = GET_PRIVATE (menuitem);

  return priv->scale;
}

/**
 * ido_scale_menu_item_get_style:
 * @menuitem: The #IdoScaleMenuItem
 * @returns: A #IdoScaleMenuItemStyle enum describing the style.
 *
 * Retrieves the type of widgets being used for the primary and
 * secondary widget slots.  This could be images, labels, or nothing.
 **/
IdoScaleMenuItemStyle
ido_scale_menu_item_get_style (IdoScaleMenuItem *menuitem)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_val_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem), IDO_SCALE_MENU_ITEM_STYLE_NONE);

  priv = GET_PRIVATE (menuitem);

  return priv->style;
}

void
ido_scale_menu_item_set_style (IdoScaleMenuItem      *menuitem,
                               IdoScaleMenuItemStyle  style)
{
  IdoScaleMenuItemPrivate *priv;
  IdoScaleMenuItemStyle    old_style;

  g_return_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem));

  priv = GET_PRIVATE (menuitem);

  old_style = priv->style;
  priv->style = style;

  update_packing (menuitem, style, old_style);
}

/**
 * ido_scale_menu_item_get_primary_image:
 * @menuitem: The #IdoScaleMenuItem
 * @returns: A #GtkWidget pointer for the primary image.
 *
 * Retrieves a pointer to the image widget used in the primary slot.
 * Whether this is visible depends upon the return value from
 * ido_scale_menu_item_get_style().
 **/
GtkWidget *
ido_scale_menu_item_get_primary_image (IdoScaleMenuItem *menuitem)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_val_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem), NULL);

  priv = GET_PRIVATE (menuitem);

  return priv->primary_image;
}

/**
 * ido_scale_menu_item_get_secondary_image:
 * @menuitem: The #IdoScaleMenuItem
 * @returns: A #GtkWidget pointer for the secondary image.
 *
 * Retrieves a pointer to the image widget used in the secondary slot.
 * Whether this is visible depends upon the return value from
 * ido_scale_menu_item_get_style().
 **/
GtkWidget *
ido_scale_menu_item_get_secondary_image (IdoScaleMenuItem *menuitem)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_val_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem), NULL);

  priv = GET_PRIVATE (menuitem);

  return priv->secondary_image;
}

/**
 * ido_scale_menu_item_get_primary_label:
 * @menuitem: The #IdoScaleMenuItem
 * @returns: A const gchar* string of the label text.
 *
 * Retrieves a string of the text for the primary label widget.
 * Whether this is visible depends upon the return value from
 * ido_scale_menu_item_get_style().
 **/
G_CONST_RETURN gchar*
ido_scale_menu_item_get_primary_label (IdoScaleMenuItem *menuitem)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_val_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem), NULL);

  priv = GET_PRIVATE (menuitem);

  return gtk_label_get_text (GTK_LABEL (priv->primary_label));
}

/**
 * ido_scale_menu_item_get_primary_label:
 * @menuitem: The #IdoScaleMenuItem
 * @returns: A const gchar* string of the label text.
 *
 * Retrieves a string of the text for the primary label widget.
 * Whether this is visible depends upon the return value from
 * ido_scale_menu_item_get_style().
 **/
G_CONST_RETURN gchar*
ido_scale_menu_item_get_secondary_label (IdoScaleMenuItem *menuitem)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_val_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem), NULL);

  priv = GET_PRIVATE (menuitem);

  return gtk_label_get_text (GTK_LABEL (priv->secondary_label));
}

/**
 * ido_scale_menu_item_set_primary_label:
 * @menuitem: The #IdoScaleMenuItem
 * @label: A string containing the label text
 *
 * Sets the text for the label widget in the primary slot.  This
 * widget will only be visibile if the return value of
 * ido_scale_menu_item_get_style() is set to %IDO_SCALE_MENU_ITEM_STYLE_LABEL.
 **/
void
ido_scale_menu_item_set_primary_label (IdoScaleMenuItem *menuitem,
                                       const gchar      *label)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem));

  priv = GET_PRIVATE (menuitem);

  if (priv->primary_label)
    {
      gtk_label_set_text (GTK_LABEL (priv->primary_label), label);
    }
}

/**
 * ido_scale_menu_item_set_primary_label:
 * @menuitem: The #IdoScaleMenuItem
 * @label: A string containing the label text
 *
 * Sets the text for the label widget in the primary slot.  This
 * widget will only be visibile if the return value of
 * ido_scale_menu_item_get_style() is set to %IDO_SCALE_MENU_ITEM_STYLE_LABEL.
 **/
void
ido_scale_menu_item_set_secondary_label (IdoScaleMenuItem *menuitem,
                                         const gchar      *label)
{
  IdoScaleMenuItemPrivate *priv;

  g_return_if_fail (IDO_IS_SCALE_MENU_ITEM (menuitem));

  priv = GET_PRIVATE (menuitem);

  if (priv->secondary_label)
    {
      gtk_label_set_text (GTK_LABEL (priv->secondary_label), label);
    }
}


#define __IDO_SCALE_MENU_ITEM_C__
