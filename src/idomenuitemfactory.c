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

#include "idousermenuitem.h"
#include "idoscalemenuitem.h"

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

static GtkMenuItem *
ido_menu_item_factory_create_menu_item (UbuntuMenuItemFactory *factory,
                                        const gchar           *type,
                                        GMenuItem             *menuitem,
                                        GActionGroup          *actions)
{
  GtkMenuItem *item = NULL;

  if (g_str_equal (type, "indicator.user-menu-item"))
    item = ido_user_menu_item_new_from_model (menuitem, actions);

  else if (g_str_equal (type, "com.canonical.unity.slider"))
    item = ido_scale_menu_item_new_from_model (menuitem, actions);

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
