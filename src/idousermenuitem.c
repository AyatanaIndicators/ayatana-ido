/*
Copyright 2011 Canonical Ltd.

Authors:
    Conor Curran <conor.curran@canonical.com>
    Mirco Müller <mirco.mueller@canonical.com>
    Charles Kerr <charles.kerr@canonical.com>

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranties of
MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
 #include "config.h"
#endif

#include <gtk/gtk.h>

#include "idousermenuitem.h"

#define FALLBACK_ICON_NAME "avatar-default"

enum
{
  PROP_0,
  PROP_LABEL,
  PROP_ICON,
  PROP_IS_LOGGED_IN,
  PROP_IS_CURRENT_USER,
  PROP_LAST
};

static GParamSpec *properties[PROP_LAST];

struct _IdoUserMenuItemPrivate
{
  GtkWidget* user_image;
  GtkWidget* user_name;
  GtkWidget* container;
  GtkWidget* tick_icon;
  gboolean is_logged_in;
  gboolean is_current_user;
  gchar * label;
  gchar * icon_name;
};

G_DEFINE_TYPE (IdoUserMenuItem, ido_user_menu_item, GTK_TYPE_MENU_ITEM);

/* Prototypes */
static gboolean ido_user_menu_item_primitive_draw_cb_gtk_3 (GtkWidget * image,
                                                            cairo_t   * cr,
                                                            gpointer    gself);


static void
my_get_property (GObject     * o,
                 guint         property_id,
                 GValue      * value,
                 GParamSpec  * pspec)
{
  IdoUserMenuItem * self = IDO_USER_MENU_ITEM (o);

  switch (property_id)
    {
      case PROP_LABEL:
        g_value_set_string (value, self->priv->label);
        break;

      case PROP_ICON:
        g_value_set_string (value, self->priv->icon_name);
        break;

      case PROP_IS_LOGGED_IN:
        g_value_set_boolean (value, self->priv->is_logged_in);
        break;

      case PROP_IS_CURRENT_USER:
        g_value_set_boolean (value, self->priv->is_current_user);
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
  IdoUserMenuItem * self = IDO_USER_MENU_ITEM (o);

  switch (property_id)
    {
      case PROP_LABEL:
        ido_user_menu_item_set_label (self, g_value_get_string (value));
        break;

      case PROP_ICON:
        ido_user_menu_item_set_icon (self, g_value_get_string (value));
        break;

      case PROP_IS_LOGGED_IN:
        ido_user_menu_item_set_logged_in (self, g_value_get_boolean (value));
        break;

      case PROP_IS_CURRENT_USER:
        self->priv->is_current_user = g_value_get_boolean (value);
        gtk_widget_queue_draw (GTK_WIDGET(self));
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (o, property_id, pspec);
        break;
    }
}

static void
my_dispose (GObject *object)
{
  G_OBJECT_CLASS (ido_user_menu_item_parent_class)->dispose (object);
}

static void
my_finalize (GObject *object)
{
  IdoUserMenuItem * self = IDO_USER_MENU_ITEM (object);

  g_free (self->priv->label);
  g_free (self->priv->icon_name);

  G_OBJECT_CLASS (ido_user_menu_item_parent_class)->finalize (object);
}

static void
ido_user_menu_item_class_init (IdoUserMenuItemClass *klass)
{
  GParamFlags prop_flags;
  GObjectClass * gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (IdoUserMenuItemPrivate));

  gobject_class->get_property = my_get_property;
  gobject_class->set_property = my_set_property;
  gobject_class->dispose = my_dispose;
  gobject_class->finalize = my_finalize;

  prop_flags = G_PARAM_CONSTRUCT
             | G_PARAM_READWRITE
             | G_PARAM_STATIC_STRINGS;

  properties[PROP_LABEL] = g_param_spec_string ("label",
                                                "The user's name",
                                                "The name to display",
                                                "J. Random User",
                                                prop_flags);

  properties[PROP_ICON] = g_param_spec_string ("icon",
                                               "The icon's name",
                                               "The icon to display",
                                               NULL,
                                               prop_flags);

  properties[PROP_IS_LOGGED_IN] = g_param_spec_boolean ("is-logged-in",
                                                        "is logged in",
                                                        "is user logged in?",
                                                        FALSE,
                                                        prop_flags);

  properties[PROP_IS_CURRENT_USER] = g_param_spec_boolean ("is-current-user",
                                                           "is current user",
                                                           "is user current?",
                                                           FALSE,
                                                           prop_flags);

  g_object_class_install_properties (gobject_class, PROP_LAST, properties);
}

static void
ido_user_menu_item_init (IdoUserMenuItem *self)
{
  IdoUserMenuItemPrivate * priv;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            IDO_USER_MENU_ITEM_TYPE,
                                            IdoUserMenuItemPrivate);

  priv = self->priv;

  // Create the UI elements.
  priv->user_image = gtk_image_new ();
  gtk_misc_set_alignment(GTK_MISC(priv->user_image), 0.0, 0.0);

  priv->user_name = gtk_label_new (NULL);

  priv->container = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);

  priv->tick_icon = gtk_image_new_from_icon_name ("account-logged-in",
                                                   GTK_ICON_SIZE_MENU);
  gtk_misc_set_alignment(GTK_MISC(priv->tick_icon), 1.0, 0.5);

  // Pack it together
  gtk_box_pack_start (GTK_BOX (priv->container),
                      priv->user_image,
                      FALSE,
                      FALSE,
                      0);
  gtk_box_pack_start (GTK_BOX (priv->container),
                      priv->user_name,
                      FALSE,
                      FALSE,
                      3);
  gtk_box_pack_end (GTK_BOX(priv->container),
                    priv->tick_icon,
                    FALSE,
                    FALSE, 5);

  gtk_widget_show_all (priv->container);
  gtk_container_add (GTK_CONTAINER (self), priv->container);
  gtk_widget_show_all (priv->tick_icon);
  gtk_widget_set_no_show_all (priv->tick_icon, TRUE);
  gtk_widget_hide (priv->tick_icon);


  // Fetch the drawing context.
  g_signal_connect_after (GTK_WIDGET(self), "draw",
                          G_CALLBACK(ido_user_menu_item_primitive_draw_cb_gtk_3),
                          GTK_WIDGET(self));
}


/*****************************************************************/

static gboolean
ido_user_menu_item_primitive_draw_cb_gtk_3 (GtkWidget * widget,
                                            cairo_t   * cr,
                                            gpointer    user_data)
{
  IdoUserMenuItemPrivate * priv;

  g_return_val_if_fail(IS_IDO_USER_MENU_ITEM(user_data), FALSE);

  priv = IDO_USER_MENU_ITEM(user_data)->priv;

  /* Draw dot only when user is the current user. */
  if (priv->is_current_user)
    {
      GtkStyleContext * style_context;
      GtkStateFlags state_flags;
      GdkRGBA color;
      gdouble x, y;

      /* get the foreground color */
      style_context = gtk_widget_get_style_context (widget);
      state_flags = gtk_widget_get_state_flags (widget);
      gtk_style_context_get_color (style_context, state_flags, &color);

      GtkAllocation allocation;
      gtk_widget_get_allocation (widget, &allocation);
      x = allocation.x + 13;
      y = allocation.height / 2;

      cairo_arc (cr, x, y, 3.0, 0.0, 2 * G_PI);

      gdk_cairo_set_source_rgba (cr, &color);

      cairo_fill (cr);
    }

  return FALSE;
}

/***
****  PUBLIC API
***/

void
ido_user_menu_item_set_icon (IdoUserMenuItem * self, const char * icon_name)
{
  gboolean updated = FALSE;
  IdoUserMenuItemPrivate * p = self->priv;
  GtkImage * image = GTK_IMAGE (p->user_image);

  /* make a private copy of the icon name */
  g_free (p->icon_name);
  self->priv->icon_name = g_strdup (icon_name);

  /* now try to use it */
  if (icon_name && *icon_name)
    {
      int width = 18; /* arbitrary default values */
      int height = 18;
      GError * err = NULL;
      GdkPixbuf * pixbuf = NULL;

      /* load the image */
      gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, &height);
      pixbuf = gdk_pixbuf_new_from_file_at_size (icon_name, width, height, &err);
      if (err == NULL)
        {
          gtk_image_set_from_pixbuf (image, pixbuf);
          g_object_unref (pixbuf);
          updated = TRUE;
        }
      else
        {
          g_warning ("Couldn't load the image \"%s\": %s", icon_name, err->message);
          g_clear_error (&err);
        }
    }

  /* as a fallback, use the default user icon */
  if (!updated)
    {
      gtk_image_set_from_icon_name (image,
                                    FALLBACK_ICON_NAME,
                                    GTK_ICON_SIZE_MENU);
    }
}

void
ido_user_menu_item_set_logged_in (IdoUserMenuItem * self, gboolean is_logged_in)
{
  gtk_widget_set_visible (self->priv->tick_icon, is_logged_in);
}

void
ido_user_menu_item_set_current_user (IdoUserMenuItem * self, gboolean is_current_user)
{
  self->priv->is_current_user = is_current_user;
  gtk_widget_queue_draw (GTK_WIDGET (self));
}

void
ido_user_menu_item_set_label (IdoUserMenuItem * self, const char * label)
{
  gtk_label_set_label (GTK_LABEL(self->priv->user_name), label);
}

GtkWidget*
ido_user_menu_item_new (void)
{
  return GTK_WIDGET (g_object_new (IDO_USER_MENU_ITEM_TYPE, NULL));
}
