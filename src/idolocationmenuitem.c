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
#include "idolocationmenuitem.h"

enum
{
  PROP_0,
  PROP_NAME,
  PROP_TIMEZONE,
  PROP_FORMAT,
  PROP_LAST
};

static GParamSpec *properties[PROP_LAST];

struct _IdoLocationMenuItemPrivate
{
  char * name;
  char * timezone;
  char * format;

  guint timestamp_timer;

  GtkWidget * name_label;
  GtkWidget * timestamp_label;
};

typedef IdoLocationMenuItemPrivate priv_t;

G_DEFINE_TYPE (IdoLocationMenuItem, ido_location_menu_item, GTK_TYPE_MENU_ITEM);

/***
****  Timestamp Label
***/

static void
update_timestamp_label (IdoLocationMenuItem * self)
{
  priv_t * p = self->priv;

  if (p->format && *p->format)
    {
      GTimeZone * tz;
      GDateTime * now;
      char * str;

      tz = g_time_zone_new (p->timezone);
      if (tz == NULL)
        tz = g_time_zone_new_local ();
      now = g_date_time_new_now (tz);
      str = g_date_time_format (now, p->format);

      gtk_label_set_text (GTK_LABEL(p->timestamp_label), str);

      g_free (str);
      g_date_time_unref (now);
      g_time_zone_unref (tz);
    }
  else
    {
      gtk_label_set_text (GTK_LABEL(p->timestamp_label), "");
    }
}

static void
stop_timestamp_timer (IdoLocationMenuItem * self)
{
  priv_t * p = self->priv;

  if (p->timestamp_timer != 0)
    {
      g_source_remove (p->timestamp_timer);
      p->timestamp_timer = 0;
    }
}

static void start_timestamp_timer (IdoLocationMenuItem * self);

static gboolean
on_timestamp_timer (gpointer gself)
{
  IdoLocationMenuItem * self = IDO_LOCATION_MENU_ITEM (gself);

  update_timestamp_label (self);

  start_timestamp_timer (self);
  return G_SOURCE_REMOVE;
}

static guint
calculate_seconds_until_next_minute (void)
{
  guint seconds;
  GTimeSpan diff;
  GDateTime * now;
  GDateTime * next;
  GDateTime * start_of_next;

  now = g_date_time_new_now_local ();
  next = g_date_time_add_minutes (now, 1);
  start_of_next = g_date_time_new_local (g_date_time_get_year (next),
                                         g_date_time_get_month (next),
                                         g_date_time_get_day_of_month (next),
                                         g_date_time_get_hour (next),
                                         g_date_time_get_minute (next),
                                         1);

  diff = g_date_time_difference (start_of_next, now);
  seconds = (diff + (G_TIME_SPAN_SECOND - 1)) / G_TIME_SPAN_SECOND;

  /* cleanup */
  g_date_time_unref (start_of_next);
  g_date_time_unref (next);
  g_date_time_unref (now);

  return seconds;
}

static void
start_timestamp_timer (IdoLocationMenuItem * self)
{
  int interval_sec;
  gboolean timestamp_shows_seconds;
  priv_t * p = self->priv;
  const char * const fmt = p->format;

  stop_timestamp_timer (self);

  timestamp_shows_seconds = fmt && (strstr(fmt,"%s") ||
                                    strstr(fmt,"%S") ||
                                    strstr(fmt,"%T"));

  if (timestamp_shows_seconds)
    interval_sec = 1;
  else
    interval_sec = calculate_seconds_until_next_minute();

  p->timestamp_timer = g_timeout_add_seconds (interval_sec,
                                              on_timestamp_timer,
                                              self);
}

/***
****  GObject Virtual Functions
***/

static void
my_get_property (GObject     * o,
                 guint         property_id,
                 GValue      * value,
                 GParamSpec  * pspec)
{
  IdoLocationMenuItem * self = IDO_LOCATION_MENU_ITEM (o);
  priv_t * p = self->priv;

  switch (property_id)
    {
      case PROP_NAME:
        g_value_set_string (value, p->name);
        break;

      case PROP_TIMEZONE:
        g_value_set_string (value, p->timezone);
        break;

      case PROP_FORMAT:
        g_value_set_string (value, p->format);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (o, property_id, pspec);
        break;
    }
}

static void
my_set_property (GObject       * o,
                 guint           property_id,
                 const GValue  * value,
                 GParamSpec    * pspec)
{
  IdoLocationMenuItem * self = IDO_LOCATION_MENU_ITEM (o);

  switch (property_id)
    {
      case PROP_NAME:
        ido_location_menu_item_set_name (self, g_value_get_string (value));
        break;

      case PROP_TIMEZONE:
        ido_location_menu_item_set_timezone (self, g_value_get_string (value));
        break;

      case PROP_FORMAT:
        ido_location_menu_item_set_format (self, g_value_get_string (value));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (o, property_id, pspec);
        break;
    }
}

static void
my_dispose (GObject * object)
{
  stop_timestamp_timer (IDO_LOCATION_MENU_ITEM (object));

  G_OBJECT_CLASS (ido_location_menu_item_parent_class)->dispose (object);
}

static void
my_finalize (GObject * object)
{
  IdoLocationMenuItem * self = IDO_LOCATION_MENU_ITEM (object);
  priv_t * p = self->priv;

  g_free (p->format);
  g_free (p->name);
  g_free (p->timezone);

  G_OBJECT_CLASS (ido_location_menu_item_parent_class)->finalize (object);
}

/***
****  Instantiation
***/

static void
ido_location_menu_item_class_init (IdoLocationMenuItemClass *klass)
{
  GParamFlags prop_flags;
  GObjectClass * gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (IdoLocationMenuItemPrivate));

  gobject_class->get_property = my_get_property;
  gobject_class->set_property = my_set_property;
  gobject_class->dispose = my_dispose;
  gobject_class->finalize = my_finalize;

  prop_flags = G_PARAM_CONSTRUCT
             | G_PARAM_READWRITE
             | G_PARAM_STATIC_STRINGS;

  properties[PROP_NAME] = g_param_spec_string (
    "name",
    "The location's name",
    "The name to display; eg, 'Oklahoma City'",
    "Location",
    prop_flags);

  properties[PROP_TIMEZONE] = g_param_spec_string (
    "timezone",
    "timezone identifier",
    "string used to identify a timezone; eg, 'America/Chicago'",
    NULL,
    prop_flags);

  properties[PROP_FORMAT] = g_param_spec_string (
    "format",
    "strftime format",
    "strftime-style format string for the timestamp",
    "%T",
    prop_flags);

  g_object_class_install_properties (gobject_class, PROP_LAST, properties);
}

static void
ido_location_menu_item_init (IdoLocationMenuItem *self)
{
  priv_t * p;
  GtkBox * box;
  GtkWidget * w;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            IDO_LOCATION_MENU_ITEM_TYPE,
                                            IdoLocationMenuItemPrivate);

  p = self->priv;

  p->name_label = gtk_label_new (NULL);
  p->timestamp_label = gtk_label_new (NULL);

  w = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 3);
  gtk_misc_set_alignment (GTK_MISC(p->timestamp_label), 1.0, 0.5);
  box = GTK_BOX (w);
  gtk_box_pack_start (box, p->name_label, FALSE, FALSE, 3);
  gtk_box_pack_end (box, p->timestamp_label, FALSE, FALSE, 5);

  gtk_widget_show_all (w);
  gtk_container_add (GTK_CONTAINER (self), w);
}


/***
****  Public API
***/

GtkWidget *
ido_location_menu_item_new (void)
{
  return GTK_WIDGET (g_object_new (IDO_LOCATION_MENU_ITEM_TYPE, NULL));
}

/**
 * @name: human-readable name, such as a city (eg: "Oklahoma City")
 */
void
ido_location_menu_item_set_name (IdoLocationMenuItem * self,
                                 const char          * name)
{
  priv_t * p;

  g_return_if_fail (IDO_IS_LOCATION_MENU_ITEM (self));
  p = self->priv;

  g_free (p->name);
  p->name = g_strdup (name);
  gtk_label_set_text (GTK_LABEL(p->name_label), p->name);
}

/**
 * @timezone: timezone identifier (eg: "America/Chicago")
 */
void
ido_location_menu_item_set_timezone (IdoLocationMenuItem   * self,
                                     const char            * timezone)
{
  priv_t * p;

  g_return_if_fail (IDO_IS_LOCATION_MENU_ITEM (self));
  p = self->priv;

  g_free (p->timezone);
  p->timezone = g_strdup (timezone);
  update_timestamp_label (self);
}

/**
 * @strftime_fmt: the format string used to build the location's time string
 */
void
ido_location_menu_item_set_format (IdoLocationMenuItem   * self,
                                   const char            * strftime_fmt)
{
  priv_t * p;

  g_return_if_fail (IDO_IS_LOCATION_MENU_ITEM (self));
  p = self->priv;

  g_free (p->format);
  p->format = g_strdup (strftime_fmt);
  update_timestamp_label (self);
  start_timestamp_timer (self);
}

GtkMenuItem *
ido_location_menu_item_new_from_model (GMenuItem    * menu_item,
                                       GActionGroup * actions)
{
  gchar * str;
  IdoLocationMenuItem * ido_location;

  ido_location = IDO_LOCATION_MENU_ITEM (ido_location_menu_item_new ());

  if (g_menu_item_get_attribute (menu_item, "label", "s", &str))
    {
      ido_location_menu_item_set_name (ido_location, str);
      g_free (str);
    }

  if (g_menu_item_get_attribute (menu_item, "x-canonical-timezone", "s", &str))
    {
      ido_location_menu_item_set_timezone (ido_location, str);
      g_free (str);
    }

  if (g_menu_item_get_attribute (menu_item, "x-canonical-time-format", "s", &str))
    {
      ido_location_menu_item_set_format (ido_location, str);
      g_free (str);
    }

  if (g_menu_item_get_attribute (menu_item, "action", "s", &str))
    {
      GVariant * target;
      IdoActionHelper * helper;

      target = g_menu_item_get_attribute_value (menu_item, "target",
                                                G_VARIANT_TYPE_ANY);
      helper = ido_action_helper_new (GTK_WIDGET(ido_location), actions,
                                      str, target);
      g_signal_connect_swapped (ido_location, "activate",
                                G_CALLBACK (ido_action_helper_activate), helper);
      g_signal_connect_swapped (ido_location, "destroy",
                                G_CALLBACK (g_object_unref), helper);

      if (target)
        g_variant_unref (target);
      g_free (str);
    }

  return GTK_MENU_ITEM (ido_location);
}
