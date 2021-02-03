/*
 * Copyright 2013 Canonical Ltd.
 *
 * Authors:
 *   Charles Kerr <charles.kerr@canonical.com>
 *   Ted Gould <ted@canonical.com>
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

#include <string.h> /* strstr() */

#include <gtk/gtk.h>

#include "idoactionhelper.h"
#include "idotimestampmenuitem.h"

enum
{
  PROP_0,
  PROP_FORMAT,
  PROP_DATE_TIME,
  PROP_LAST
};

static GParamSpec *properties[PROP_LAST];

typedef struct {
  char * format;
  GDateTime * date_time;
} IdoTimeStampMenuItemPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (IdoTimeStampMenuItem,
                            ido_time_stamp_menu_item,
                            IDO_TYPE_BASIC_MENU_ITEM);

/***
****  GObject Virtual Functions
***/

static void
my_get_property (GObject     * o,
                 guint         property_id,
                 GValue      * v,
                 GParamSpec  * pspec)
{
  IdoTimeStampMenuItem * self = IDO_TIME_STAMP_MENU_ITEM (o);
  IdoTimeStampMenuItemPrivate * priv = ido_time_stamp_menu_item_get_instance_private(self);

  switch (property_id)
    {
      case PROP_FORMAT:
        g_value_set_string (v, priv->format);
        break;

      case PROP_DATE_TIME:
        g_value_set_boxed (v, priv->date_time);
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
  IdoTimeStampMenuItem * self = IDO_TIME_STAMP_MENU_ITEM (o);

  switch (property_id)
    {
      case PROP_FORMAT:
        ido_time_stamp_menu_item_set_format (self, g_value_get_string (v));
        break;

      case PROP_DATE_TIME:
        ido_time_stamp_menu_item_set_date_time (self, g_value_get_boxed (v));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (o, property_id, pspec);
        break;
    }
}

static void
my_dispose (GObject * object)
{
  IdoTimeStampMenuItem * self = IDO_TIME_STAMP_MENU_ITEM (object);
  IdoTimeStampMenuItemPrivate * priv = ido_time_stamp_menu_item_get_instance_private(self);

  g_clear_pointer (&priv->date_time, g_date_time_unref);

  G_OBJECT_CLASS (ido_time_stamp_menu_item_parent_class)->dispose (object);
}

static void
my_finalize (GObject * object)
{
  IdoTimeStampMenuItem * self = IDO_TIME_STAMP_MENU_ITEM (object);
  IdoTimeStampMenuItemPrivate * priv = ido_time_stamp_menu_item_get_instance_private(self);

  g_free (priv->format);

  G_OBJECT_CLASS (ido_time_stamp_menu_item_parent_class)->finalize (object);
}

/***
****  Instantiation
***/

static void
ido_time_stamp_menu_item_class_init (IdoTimeStampMenuItemClass *klass)
{
  GParamFlags prop_flags;
  GObjectClass * gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = my_get_property;
  gobject_class->set_property = my_set_property;
  gobject_class->dispose = my_dispose;
  gobject_class->finalize = my_finalize;

  prop_flags = G_PARAM_CONSTRUCT
             | G_PARAM_READWRITE
             | G_PARAM_STATIC_STRINGS;

  properties[PROP_FORMAT] = g_param_spec_string (
    "format",
    "strftime format",
    "strftime-style format string for the timestamp",
    "%F %T",
    prop_flags);

  properties[PROP_DATE_TIME] = g_param_spec_boxed (
    "date-time",
    "Date-Time",
    "GDateTime specifying the time to render",
    G_TYPE_DATE_TIME,
    prop_flags);

  g_object_class_install_properties (gobject_class, PROP_LAST, properties);
}

static void
ido_time_stamp_menu_item_init (IdoTimeStampMenuItem *self)
{
  /* no-op */
}

static void
update_timestamp_label (IdoTimeStampMenuItem * self)
{
  char * str;

  IdoTimeStampMenuItemPrivate * priv = ido_time_stamp_menu_item_get_instance_private(self);

  if (priv->date_time && priv->format)
    str = g_date_time_format (priv->date_time, priv->format);
  else
    str = NULL;

  ido_basic_menu_item_set_secondary_text (IDO_BASIC_MENU_ITEM (self), str);
  g_free (str);
}

/***
****  Public API
***/

/* create  a new IdoTimeStampMenuItem */
GtkWidget *
ido_time_stamp_menu_item_new (void)
{
  return GTK_WIDGET (g_object_new (IDO_TYPE_TIME_STAMP_MENU_ITEM, NULL));
}

/**
 * ido_time_stamp_menu_item_set_time:
 * @time: the time to be rendered in the appointment's timestamp label.
 *
 * Set the time that will be displayed in the menuitem's
 * right-justified timestamp label
 */
void
ido_time_stamp_menu_item_set_date_time (IdoTimeStampMenuItem * self,
                                        GDateTime            * date_time)
{
  IdoTimeStampMenuItemPrivate * priv;

  g_return_if_fail (IDO_IS_TIME_STAMP_MENU_ITEM (self));

  priv = ido_time_stamp_menu_item_get_instance_private(self);

  g_clear_pointer (&priv->date_time, g_date_time_unref);
  if (date_time != NULL)
    priv->date_time = g_date_time_ref (date_time);
  update_timestamp_label (self);
}

/**
 * ido_time_stamp_menu_item_set_format:
 * @format: the format string used when showing the appointment's time
 *
 * Set the format string for rendering the appointment's time
 * in its right-justified secondary label.
 *
 * See strfrtime(3) for more information on the format string.
 */
void
ido_time_stamp_menu_item_set_format (IdoTimeStampMenuItem * self,
                                     const char           * strftime_fmt)
{
  IdoTimeStampMenuItemPrivate * priv;

  g_return_if_fail (IDO_IS_TIME_STAMP_MENU_ITEM (self));

  priv = ido_time_stamp_menu_item_get_instance_private(self);

  g_free (priv->format);
  priv->format = g_strdup (strftime_fmt);
  update_timestamp_label (self);
}

const gchar *
ido_time_stamp_menu_item_get_format (IdoTimeStampMenuItem * self)
{
  IdoTimeStampMenuItemPrivate * priv;

  g_return_val_if_fail (IDO_IS_TIME_STAMP_MENU_ITEM (self), NULL);

  priv = ido_time_stamp_menu_item_get_instance_private(self);

  return priv->format;
}
