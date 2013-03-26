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

#include <libindicator/indicator-image-helper.h>

#include "shared-names.h"
#include "user-widget.h"


typedef struct _UserWidgetPrivate UserWidgetPrivate;

struct _UserWidgetPrivate
{
  DbusmenuMenuitem* twin_item;
  GtkWidget* user_image;
  GtkWidget* user_name;
  GtkWidget* container;
  GtkWidget* tick_icon;
  gboolean logged_in;
  gboolean sessions_active;
};

#define USER_WIDGET_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), USER_WIDGET_TYPE, UserWidgetPrivate))

/* Prototypes */
static void user_widget_class_init    (UserWidgetClass *klass);
static void user_widget_init          (UserWidget *self);
static void user_widget_dispose       (GObject *object);
static void user_widget_finalize      (GObject *object);

static void user_widget_set_twin_item (UserWidget* self,
                                       DbusmenuMenuitem* twin_item);

static gboolean user_widget_primitive_draw_cb_gtk_3 (GtkWidget *image,
                                                         cairo_t* cr,
                                                         gpointer user_data);

G_DEFINE_TYPE (UserWidget, user_widget, GTK_TYPE_MENU_ITEM);

static void
user_widget_class_init (UserWidgetClass *klass)
{
  GObjectClass * gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (UserWidgetPrivate));

  gobject_class->dispose = user_widget_dispose;
  gobject_class->finalize = user_widget_finalize;
}

static void
user_widget_init (UserWidget *self)
{
  self->priv = USER_WIDGET_GET_PRIVATE(self);

  UserWidgetPrivate * priv = self->priv;

  priv->user_image = NULL;
  priv->user_name  = NULL;
  priv->logged_in = FALSE;
  priv->sessions_active = FALSE;
  priv->container = NULL;
  priv->tick_icon = NULL;

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
                          G_CALLBACK(user_widget_primitive_draw_cb_gtk_3),
                          GTK_WIDGET(self));
}

static void
user_widget_dispose (GObject *object)
{
  G_OBJECT_CLASS (user_widget_parent_class)->dispose (object);
}

// TODO tidy up image and name
static void
user_widget_finalize (GObject *object)
{
  G_OBJECT_CLASS (user_widget_parent_class)->finalize (object);
}


/*****************************************************************/

// TODO handle drawing of green check mark
static gboolean
user_widget_primitive_draw_cb_gtk_3 (GtkWidget *widget,
                                     cairo_t* cr,
                                     gpointer user_data)
{
  g_return_val_if_fail(IS_USER_WIDGET(user_data), FALSE);
  UserWidget* meta = USER_WIDGET(user_data);
  UserWidgetPrivate * priv = USER_WIDGET_GET_PRIVATE(meta);

  // Draw dot only when user is the current user.
  if (dbusmenu_menuitem_property_get_bool (priv->twin_item, USER_ITEM_PROP_IS_CURRENT_USER))
    {
      gdouble x, y;
      GtkStyle * style = gtk_widget_get_style (widget);

      GtkAllocation allocation;
      gtk_widget_get_allocation (widget, &allocation);
      x = allocation.x + 13;
      y = allocation.height / 2;

      cairo_arc (cr, x, y, 3.0, 0.0, 2 * G_PI);

      cairo_set_source_rgb (cr, style->fg[gtk_widget_get_state(widget)].red/65535.0,
                                style->fg[gtk_widget_get_state(widget)].green/65535.0,
                                style->fg[gtk_widget_get_state(widget)].blue/65535.0);
      cairo_fill (cr);
    }

  return FALSE;
}

/***
****
***/

static void
update_icon (UserWidget * self, DbusmenuMenuitem * mi)
{
  gboolean updated = FALSE;
  GtkImage * image = GTK_IMAGE(self->priv->user_image);

  /* first try the menuitem's icon property */
  const gchar * icon_name = dbusmenu_menuitem_property_get (mi, USER_ITEM_PROP_ICON);
  if (icon_name != NULL)
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
                                    USER_ITEM_ICON_DEFAULT,
                                    GTK_ICON_SIZE_MENU);
    }
}

static void
update_logged_in (UserWidget * self, DbusmenuMenuitem * mi)
{
  const gboolean b = dbusmenu_menuitem_property_get_bool (mi, USER_ITEM_PROP_LOGGED_IN);

  g_debug ("User \"%s\" %s active sessions",
           dbusmenu_menuitem_property_get (mi, USER_ITEM_PROP_NAME),
           b ? "has" : "doesn't have");

  gtk_widget_set_visible (self->priv->tick_icon, b);
}

static void
update_name (UserWidget * self, DbusmenuMenuitem * mi)
{
  gtk_label_set_label (GTK_LABEL(self->priv->user_name),
                       dbusmenu_menuitem_property_get (mi, USER_ITEM_PROP_NAME));
}

static void
user_widget_property_update (DbusmenuMenuitem  * mi,
                             const gchar       * property,
                             GVariant          * value,
                             UserWidget        * self)
{
  g_return_if_fail (IS_USER_WIDGET (self));

  if (!g_strcmp0 (property, USER_ITEM_PROP_LOGGED_IN))
    {
      update_logged_in (self, mi);
    }
  else if (!g_strcmp0 (property, USER_ITEM_PROP_ICON))
    {
      update_icon (self, mi);
    }
  else if (!g_strcmp0 (property, USER_ITEM_PROP_NAME))
    {
      update_name (self, mi);
    }
  else
    {
      g_debug ("%s FIXME: unhandled property change %s", G_STRFUNC, property);
    }
}

static void
user_widget_set_twin_item (UserWidget * self, DbusmenuMenuitem * mi)
{
  self->priv->twin_item = mi;

  update_icon      (self, mi);
  update_name      (self, mi);
  update_logged_in (self, mi);

  g_signal_connect (G_OBJECT(mi), "property-changed",
                    G_CALLBACK(user_widget_property_update), self);
}

 /**
  * user_widget_new:
  * @item: the #DbusmenuMenuitem this widget will render.
  *
  * Returns: (transfer full): a new #UserWidget.
  **/
GtkWidget*
user_widget_new (DbusmenuMenuitem *item)
{
  GtkWidget* widget =  g_object_new(USER_WIDGET_TYPE, NULL);
  user_widget_set_twin_item ( USER_WIDGET(widget), item );
  return widget;
}
