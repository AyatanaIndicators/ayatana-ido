/*
 * Copyright 2013 Canonical Ltd.
 * Copyright 2021-2023 Robert Tari
 *
 * Authors:
 *   Charles Kerr <charles.kerr@canonical.com>
 *   Ted Gould <ted@canonical.com>
 *   Robert Tari <robert@tari.in>
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

#include <gtk/gtk.h>

#include "idoactionhelper.h"
#include "idotimestampmenuitem.h"

/* create a menu-sized pixbuf filled with specified color */
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

/**
 * ido_appointment_menu_item_new_from_model:
 * @menu_item: the corresponding menuitem
 * @actions: action group to tell when this GtkMenuItem is activated
 *
 * Creates a new IdoTimeStampMenuItem with properties initialized
 * appropriately for a org.ayatana.indicator.alarm
 *
 * If the menuitem's 'action' attribute is set, trigger that action
 * in @actions when this IdoAppointmentMenuItem is activated.
 */
GtkMenuItem *
ido_appointment_menu_item_new_from_model (GMenuItem    * menu_item,
                                          GActionGroup * actions)
{
  guint i;
  guint n;
  gint64 i64;
  gchar * str;
  IdoBasicMenuItem * ido_menu_item;
  const gchar * names[4] = {0};
  GValue * values;
  const guint n_max = 4;

  /* create the ido_menu_item */

  n = 0;
  values = g_new0(GValue, n_max);

  if (g_menu_item_get_attribute (menu_item, G_MENU_ATTRIBUTE_LABEL, "s", &str))
    {
      names[n] = "text";
      g_value_init (&values[n], G_TYPE_STRING);
      g_value_take_string (&values[n], str);
      n++;
    }

  if (g_menu_item_get_attribute (menu_item, "x-ayatana-color", "s", &str))
    {
      names[n] = "pixbuf";
      g_value_init (&values[n], G_TYPE_OBJECT);
      g_value_take_object (&values[n], create_color_icon_pixbuf (str));
      g_free(str);
      n++;
    }

  if (g_menu_item_get_attribute (menu_item, "x-ayatana-time-format", "s", &str))
    {
      names[n] = "format";
      g_value_init (&values[n], G_TYPE_STRING);
      g_value_take_string (&values[n], str);
      n++;
    }

  if (g_menu_item_get_attribute (menu_item, "x-ayatana-time", "x", &i64))
    {
      names[n] = "date-time";
      g_value_init (&values[n], G_TYPE_DATE_TIME);
      g_value_take_boxed (&values[n], g_date_time_new_from_unix_local (i64));
      n++;
    }

  g_assert (n <= G_N_ELEMENTS (names));
  g_assert (n <= n_max);
  ido_menu_item = IDO_BASIC_MENU_ITEM(g_object_new_with_properties (IDO_TYPE_TIME_STAMP_MENU_ITEM, n, names, values));

  for (i=0; i<n; i++)
    g_value_unset (&values[i]);

  g_free (values);

  /* add an ActionHelper */

  if (g_menu_item_get_attribute (menu_item, "action", "s", &str))
    {
      GVariant * target;
      IdoActionHelper * helper;

      target = g_menu_item_get_attribute_value (menu_item, "target",
                                                G_VARIANT_TYPE_ANY);
      helper = ido_action_helper_new (GTK_WIDGET(ido_menu_item), actions,
                                      str, target);
      g_signal_connect_swapped (ido_menu_item, "activate",
                                G_CALLBACK (ido_action_helper_activate), helper);
      g_signal_connect_swapped (ido_menu_item, "destroy",
                                G_CALLBACK (g_object_unref), helper);

      g_clear_pointer (&target, g_variant_unref);
      g_free (str);
    }

  return GTK_MENU_ITEM (ido_menu_item);
}
