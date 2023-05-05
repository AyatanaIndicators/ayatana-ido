/*
 * Copyright 2023 Robert Tari
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Robert Tari <robert@tari.in>
 */

#ifndef __IDO_LEVEL_MENU_ITEM_H__
#define __IDO_LEVEL_MENU_ITEM_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define IDO_TYPE_LEVEL_MENU_ITEM (ido_level_menu_item_get_type ())
#define IDO_LEVEL_MENU_ITEM(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), IDO_TYPE_LEVEL_MENU_ITEM, IdoLevelMenuItem))
#define IDO_IS_LEVEL_MENU_ITEM(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IDO_TYPE_LEVEL_MENU_ITEM))

typedef struct _IdoLevelMenuItem IdoLevelMenuItem;
typedef struct _IdoLevelMenuItemClass IdoLevelMenuItemClass;

struct _IdoLevelMenuItemClass
{
    GtkMenuItemClass parent_class;
};

struct _IdoLevelMenuItem
{
    GtkMenuItem parent;
};

GType ido_level_menu_item_get_type (void) G_GNUC_CONST;
GtkWidget* ido_level_menu_item_new ();
GtkMenuItem* ido_level_menu_item_new_from_model (GMenuItem *pMenuItem, GActionGroup *pActionGroup);
void idoLevelMenuItemSetIcon (IdoLevelMenuItem *self, GIcon *pIcon);
void idoLevelMenuItemSetText (IdoLevelMenuItem *self, const char *sText);
void idoLevelMenuItemSetLevel (IdoLevelMenuItem *self, guint16 nLevel);

G_END_DECLS

#endif
