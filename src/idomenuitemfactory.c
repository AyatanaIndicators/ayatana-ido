/*
 * Copyright 2013 Canonical Ltd.
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
 *
 * Authors:
 *     Lars Uebernickel <lars.uebernickel@canonical.com>
 */

#include <gtk/gtk.h>
#include <gtk/ubuntu-private.h>

#include "idoactionhelper.h"
#include "idousermenuitem.h"

#define IDO_TYPE_MENU_ITEM_FACTORY         (ido_menu_item_factory_get_type ())
#define IDO_MENU_ITEM_FACTORY(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), IDO_TYPE_MENU_ITEM_FACTORY, IdoMenuItemFactory))
#define IDO_IS_MENU_ITEM_FACTORY(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), IDO_TYPE_MENU_ITEM_FACTORY))

typedef GObject      IdoMenuItemFactory;
typedef GObjectClass IdoMenuItemFactoryClass;

GType       ido_menu_item_factory_get_type (void);
static void ido_menu_item_factory_interface_init (UbuntuMenuItemFactoryInterface *iface);

G_DEFINE_TYPE_WITH_CODE (IdoMenuItemFactory, ido_menu_item_factory, G_TYPE_OBJECT,
  G_IMPLEMENT_INTERFACE (UBUNTU_TYPE_MENU_ITEM_FACTORY, ido_menu_item_factory_interface_init)
  g_io_extension_point_implement (UBUNTU_MENU_ITEM_FACTORY_EXTENSION_POINT_NAME,
                                  g_define_type_id, "ido", 0);)

/**
 * user_menu_item_state_changed:
 *
 * Updates an IdoUserMenuItem from @state. The state contains a
 * dictionary with keys 'active-user' (for the user that the current
 * session belongs too) and 'logged-in-users' (a list of all currently
 * logged in users).
 */
static void
user_menu_item_state_changed (IdoActionHelper *helper,
                              GVariant        *state,
                              gpointer         user_data)
{
  IdoUserMenuItem *item;
  GVariant *target;
  GVariant *v;

  item = IDO_USER_MENU_ITEM (ido_action_helper_get_widget (helper));

  ido_user_menu_item_set_current_user (item, FALSE);
  ido_user_menu_item_set_logged_in (item, FALSE);

  target = ido_action_helper_get_action_target (helper);
  g_return_if_fail (g_variant_is_of_type (target, G_VARIANT_TYPE_STRING));

  if ((v = g_variant_lookup_value (state, "active-user", G_VARIANT_TYPE_STRING)))
    {
      if (g_variant_equal (v, target))
        ido_user_menu_item_set_current_user (item, TRUE);

      g_variant_unref (v);
    }

  if ((v = g_variant_lookup_value (state, "logged-in-users", G_VARIANT_TYPE_STRING_ARRAY)))
    {
      GVariantIter it;
      GVariant *user;

      g_variant_iter_init (&it, v);
      while ((user = g_variant_iter_next_value (&it)))
        {
          if (g_variant_equal (user, target))
            ido_user_menu_item_set_logged_in (item, TRUE);
          g_variant_unref (user);
        }

      g_variant_unref (v);
    }
}

/**
 * create_user_menu_item:
 *
 * Creates an IdoUserMenuItem. If @menuitem contains an action, the
 * widget is bound to that action in @actions.
 */
static GtkMenuItem *
create_user_menu_item (GMenuItem    *menuitem,
                       GActionGroup *actions)
{
  IdoUserMenuItem *item;
  gchar *label;
  gchar *action;

  item = IDO_USER_MENU_ITEM (ido_user_menu_item_new ());

  if (g_menu_item_get_attribute (menuitem, "label", "s", &label))
    {
      ido_user_menu_item_set_label (item, label);
      g_free (label);
    }

  if (g_menu_item_get_attribute (menuitem, "action", "s", &action))
    {
      IdoActionHelper *helper;
      GVariant *target;

      target = g_menu_item_get_attribute_value (menuitem, "target", G_VARIANT_TYPE_ANY);

      helper = ido_action_helper_new (GTK_WIDGET (item), actions, action, target);
      g_signal_connect (helper, "action-state-changed",
                        G_CALLBACK (user_menu_item_state_changed), NULL);

      g_signal_connect_object (item, "activate",
                               G_CALLBACK (ido_action_helper_activate),
                               helper, G_CONNECT_SWAPPED);
      g_signal_connect_swapped (item, "destroy", G_CALLBACK (g_object_unref), helper);

      if (target)
        g_variant_unref (target);
      g_free (action);
    }

  return GTK_MENU_ITEM (item);
}

static GtkMenuItem *
ido_menu_item_factory_create_menu_item (UbuntuMenuItemFactory *factory,
                                        const gchar           *type,
                                        GMenuItem             *menuitem,
                                        GActionGroup          *actions)
{
  GtkMenuItem *item = NULL;

  if (g_str_equal (type, "indicator.user-menu-item"))
    item = create_user_menu_item (menuitem, actions);

  return item;
}

static void
ido_menu_item_factory_class_init (IdoMenuItemFactoryClass *class)
{
}

static void
ido_menu_item_factory_interface_init (UbuntuMenuItemFactoryInterface *iface)
{
  iface->create_menu_item = ido_menu_item_factory_create_menu_item;
}

static void
ido_menu_item_factory_init (IdoMenuItemFactory *factory)
{
}
