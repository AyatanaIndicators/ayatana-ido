/**
 * Copyright 2013 Canonical Ltd.
 *
 * Authors:
 *   Charles Kerr <charles.kerr@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License version 3, as published 
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along 
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

#include <string.h> /* strstr() */

#include <gtk/gtk.h>

#include "idoactionhelper.h"
#include "idoappointmentmenuitem.h"

enum
{
  PROP_0,
  PROP_COLOR,
  PROP_SUMMARY,
  PROP_TIME,
  PROP_FORMAT,
  PROP_LAST
};

static GParamSpec *properties[PROP_LAST];

struct _IdoAppointmentMenuItemPrivate
{
  char * summary;
  char * format;
  char * color_string;
  GDateTime * date_time;

  GtkWidget * color_image;
  GtkWidget * summary_label;
  GtkWidget * timestamp_label;
};

typedef IdoAppointmentMenuItemPrivate priv_t;

G_DEFINE_TYPE (IdoAppointmentMenuItem,
               ido_appointment_menu_item,
               GTK_TYPE_MENU_ITEM);

/***
****  GObject Virtual Functions
***/

static void
my_get_property (GObject     * o,
                 guint         property_id,
                 GValue      * v,
                 GParamSpec  * pspec)
{
  IdoAppointmentMenuItem * self = IDO_APPOINTMENT_MENU_ITEM (o);
  priv_t * p = self->priv;

  switch (property_id)
    {
      case PROP_COLOR:
        g_value_set_string (v, p->color_string);
        break;

      case PROP_SUMMARY:
        g_value_set_string (v, p->summary);
        break;

      case PROP_FORMAT:
        g_value_set_string (v, p->format);
        break;

      case PROP_TIME:
        g_value_set_uint64 (v, g_date_time_to_unix (p->date_time));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (o, property_id, pspec);
        break;
    }
}

static void
my_set_property (GObject       * o,
                 guint           property_id,
                 const GValue  * v,
                 GParamSpec    * pspec)
{
  IdoAppointmentMenuItem * self = IDO_APPOINTMENT_MENU_ITEM (o);

  switch (property_id)
    {
      case PROP_COLOR:
        ido_appointment_menu_item_set_color (self, g_value_get_string (v));
        break;

      case PROP_SUMMARY:
        ido_appointment_menu_item_set_summary (self, g_value_get_string (v));
        break;

      case PROP_FORMAT:
        ido_appointment_menu_item_set_format (self, g_value_get_string (v));
        break;

      case PROP_TIME:
        ido_appointment_menu_item_set_time (self, g_value_get_int64 (v));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (o, property_id, pspec);
        break;
    }
}

static void
my_dispose (GObject * object)
{
  IdoAppointmentMenuItem * self = IDO_APPOINTMENT_MENU_ITEM (object);
  priv_t * p = self->priv;

  g_clear_pointer (&p->date_time, g_date_time_unref);

  G_OBJECT_CLASS (ido_appointment_menu_item_parent_class)->dispose (object);
}

static void
my_finalize (GObject * object)
{
  IdoAppointmentMenuItem * self = IDO_APPOINTMENT_MENU_ITEM (object);
  priv_t * p = self->priv;

  g_free (p->color_string);
  g_free (p->summary);
  g_free (p->format);

  G_OBJECT_CLASS (ido_appointment_menu_item_parent_class)->finalize (object);
}

/***
****  Instantiation
***/

static void
ido_appointment_menu_item_class_init (IdoAppointmentMenuItemClass *klass)
{
  GParamFlags prop_flags;
  GObjectClass * gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (IdoAppointmentMenuItemPrivate));

  gobject_class->get_property = my_get_property;
  gobject_class->set_property = my_set_property;
  gobject_class->dispose = my_dispose;
  gobject_class->finalize = my_finalize;

  prop_flags = G_PARAM_CONSTRUCT
             | G_PARAM_READWRITE
             | G_PARAM_STATIC_STRINGS;

  properties[PROP_COLOR] = g_param_spec_string (
    "color",
    "Color",
    "Color coding for the appointment's type",
    "White",
    prop_flags);

  properties[PROP_SUMMARY] = g_param_spec_string (
    "summary",
    "Summary",
    "Brief description of the appointment",
    "",
    prop_flags);

  properties[PROP_TIME] = g_param_spec_int64 (
    "time",
    "Time",
    "unix time_t specifying when the appointment begins",
    0, G_MAXINT64, 0,
    prop_flags);

  properties[PROP_FORMAT] = g_param_spec_string (
    "format",
    "strftime format",
    "strftime-style format string for the timestamp",
    "%F %T",
    prop_flags);

  g_object_class_install_properties (gobject_class, PROP_LAST, properties);
}

static void
ido_appointment_menu_item_init (IdoAppointmentMenuItem *self)
{
  priv_t * p;
  GtkBox * box;
  GtkWidget * w;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            IDO_APPOINTMENT_MENU_ITEM_TYPE,
                                            IdoAppointmentMenuItemPrivate);

  p = self->priv;

  p->color_image = gtk_image_new ();
  p->summary_label = gtk_label_new (NULL);
  p->timestamp_label = gtk_label_new (NULL);
  w = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);

  gtk_misc_set_alignment (GTK_MISC(p->timestamp_label), 1.0, 0.5);
  box = GTK_BOX (w);
  gtk_box_pack_start (box, p->color_image, FALSE, FALSE, 2);
  gtk_box_pack_start (box, p->summary_label, FALSE, FALSE, 2);
  gtk_box_pack_end (box, p->timestamp_label, FALSE, FALSE, 5);

  gtk_widget_show_all (w);
  gtk_container_add (GTK_CONTAINER (self), w);
}

/***
****
***/

static GdkPixbuf *
create_color_icon_pixbuf (const char * color_spec)
{
  static int width = -1;
  static int height = -1;
  GdkPixbuf * pixbuf = NULL;

  if (width == -1)
    {
      gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, &height);
      width = CLAMP (width, 10, 30);
      height = CLAMP (height, 10, 30);
    }

  if (color_spec && *color_spec)
    {
      cairo_surface_t * surface;
      cairo_t * cr;
      GdkRGBA rgba;

      surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
      cr = cairo_create (surface);

      if (gdk_rgba_parse (&rgba, color_spec))
        gdk_cairo_set_source_rgba (cr, &rgba);

      cairo_paint (cr);
      cairo_set_source_rgba (cr, 0, 0, 0, 0.5);
      cairo_set_line_width (cr, 1);
      cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
      cairo_stroke (cr);

      pixbuf = gdk_pixbuf_get_from_surface (surface, 0, 0, width, height);

      cairo_destroy (cr);
      cairo_surface_destroy (surface);
    }

  return pixbuf;
}

static void
update_timestamp_label (IdoAppointmentMenuItem * self)
{
  char * str;
  priv_t * p = self->priv;

  str = g_date_time_format (p->date_time, p->format);
  gtk_label_set_text (GTK_LABEL(p->timestamp_label), str);
  g_free (str);
}

/***
****  Public API
***/

GtkWidget *
ido_appointment_menu_item_new (void)
{
  return GTK_WIDGET (g_object_new (IDO_APPOINTMENT_MENU_ITEM_TYPE, NULL));
}

void
ido_appointment_menu_item_set_color (IdoAppointmentMenuItem * self,
                                     const char             * color_string)
{
  priv_t * p;
  GdkPixbuf * pixbuf;

  g_return_if_fail (IDO_IS_APPOINTMENT_MENU_ITEM (self));
  p = self->priv;

  g_free (p->color_string);
  p->color_string = g_strdup (color_string);
  pixbuf = create_color_icon_pixbuf (p->color_string);
  gtk_image_set_from_pixbuf (GTK_IMAGE(p->color_image), pixbuf);
  g_object_unref (G_OBJECT(pixbuf));
}

void
ido_appointment_menu_item_set_summary (IdoAppointmentMenuItem * self,
                                       const char             * summary)
{
  priv_t * p;

  g_return_if_fail (IDO_IS_APPOINTMENT_MENU_ITEM (self));
  p = self->priv;

  g_free (p->summary);
  p->summary = g_strdup (summary);
  gtk_label_set_text (GTK_LABEL(p->summary_label), p->summary);
}

void
ido_appointment_menu_item_set_time (IdoAppointmentMenuItem * self,
                                    time_t                   time)
{
  priv_t * p;

  g_return_if_fail (IDO_IS_APPOINTMENT_MENU_ITEM (self));
  p = self->priv;

  g_clear_pointer (&p->date_time, g_date_time_unref);
  p->date_time = g_date_time_new_from_unix_local (time);
  update_timestamp_label (self);
}

/**
 * @strftime_fmt: the format string used to build the appointment's time string
 */
void
ido_appointment_menu_item_set_format (IdoAppointmentMenuItem * self,
                                      const char             * strftime_fmt)
{
  priv_t * p;

  g_return_if_fail (IDO_IS_APPOINTMENT_MENU_ITEM (self));
  p = self->priv;

  g_free (p->format);
  p->format = g_strdup (strftime_fmt);
  update_timestamp_label (self);
}

GtkMenuItem *
ido_appointment_menu_item_new_from_model (GMenuItem    * menu_item,
                                          GActionGroup * actions)
{
  gint64 i64;
  gchar * str;
  IdoAppointmentMenuItem * ido_appointment;

  ido_appointment = IDO_APPOINTMENT_MENU_ITEM (ido_appointment_menu_item_new());

  if (g_menu_item_get_attribute (menu_item, "label", "s", &str))
    {
      ido_appointment_menu_item_set_summary (ido_appointment, str);
      g_free (str);
    }

  if (g_menu_item_get_attribute (menu_item, "x-canonical-color", "s", &str))
    {
      ido_appointment_menu_item_set_color (ido_appointment, str);
      g_free (str);
    }

  if (g_menu_item_get_attribute (menu_item, "x-canonical-time", "x", &i64))
    {
      ido_appointment_menu_item_set_time (ido_appointment, (time_t)i64);
    }

  if (g_menu_item_get_attribute (menu_item, "x-canonical-time-format", "s", &str))
    {
      ido_appointment_menu_item_set_format (ido_appointment, str);
      g_free (str);
    }

  if (g_menu_item_get_attribute (menu_item, "action", "s", &str))
    {
      GVariant * target;
      IdoActionHelper * helper;

      target = g_menu_item_get_attribute_value (menu_item, "target",
                                                G_VARIANT_TYPE_ANY);
      helper = ido_action_helper_new (GTK_WIDGET(ido_appointment), actions,
                                      str, target);
      g_signal_connect_swapped (ido_appointment, "activate",
                                G_CALLBACK (ido_action_helper_activate), helper);
      g_signal_connect_swapped (ido_appointment, "destroy",
                                G_CALLBACK (g_object_unref), helper);

      g_clear_pointer (&target, g_variant_unref);
      g_free (str);
    }

  return GTK_MENU_ITEM (ido_appointment);
}
