/*
Copyright 2011 Canonical Ltd.

Authors:
    Conor Curran <conor.curran@canonical.com>

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
#ifndef __IDO_USER_MENU_ITEM_H__
#define __IDO_USER_MENU_ITEM_H__

#include <gtk/gtk.h>
#include <libdbusmenu-gtk/menuitem.h>

G_BEGIN_DECLS

#define IDO_USER_MENU_ITEM_TYPE            (ido_user_menu_item_get_type ())
#define IDO_USER_MENU_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), IDO_USER_MENU_ITEM_TYPE, IdoUserMenuItem))
#define IDO_USER_MENU_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), IDO_USER_MENU_ITEM_TYPE, IdoUserMenuItemClass))
#define IS_IDO_USER_MENU_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IDO_USER_MENU_ITEM_TYPE))
#define IS_IDO_USER_MENU_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), IDO_USER_MENU_ITEM_TYPE))
#define IDO_USER_MENU_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), IDO_USER_MENU_ITEM_TYPE, IdoUserMenuItemClass))

typedef struct _IdoUserMenuItem        IdoUserMenuItem;
typedef struct _IdoUserMenuItemClass   IdoUserMenuItemClass;
typedef struct _IdoUserMenuItemPrivate IdoUserMenuItemPrivate;

struct _IdoUserMenuItemClass
{
  GtkMenuItemClass parent_class;
};

struct _IdoUserMenuItem
{
  /*< private >*/
  GtkMenuItem parent;
  IdoUserMenuItemPrivate * priv;
};

GType ido_user_menu_item_get_type (void) G_GNUC_CONST;
GtkWidget* ido_user_menu_item_new(DbusmenuMenuitem *twin_item);

G_END_DECLS

#endif
