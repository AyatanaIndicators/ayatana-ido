/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors:
 *    Cody Russell <crussell@canonical.com>
 */

#ifndef __IDO_SCALE_MENU_ITEM_H__
#define __IDO_SCALE_MENU_ITEM_H__

#include <gtk/gtkmenuitem.h>

G_BEGIN_DECLS

#define IDO_TYPE_SCALE_MENU_ITEM         (ido_scale_menu_item_get_type ())
#define IDO_SCALE_MENU_ITEM(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), IDO_TYPE_SCALE_MENU_ITEM, IdoScaleMenuItem))
#define IDO_SCALE_MENU_ITEM_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), IDO_TYPE_SCALE_MENU_ITEM, IdoScaleMenuItemClass))
#define IDO_IS_SCALE_MENU_ITEM(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), IDO_TYPE_SCALE_MENU_ITEM))
#define IDO_IS_SCALE_MENU_ITEM_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), IDO_TYPE_SCALE_MENU_ITEM))
#define IDO_SCALE_MENU_ITEM_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), IDO_TYPE_SCALE_MENU_ITEM, IdoScaleMenuItemClass))


typedef struct _IdoScaleMenuItem        IdoScaleMenuItem;
typedef struct _IdoScaleMenuItemClass   IdoScaleMenuItemClass;
typedef struct _IdoScaleMenuItemPrivate IdoScaleMenuItemPrivate;

struct _IdoScaleMenuItem
{
  GtkMenuItem parent_instance;

  IdoScaleMenuItemPrivate *priv;
};

struct _IdoScaleMenuItemClass
{
  GtkMenuItemClass parent_class;
};


GType	   ido_scale_menu_item_get_type          (void) G_GNUC_CONST;
GtkWidget *ido_scale_menu_item_new               (const gchar      *label,
                                                  GtkAdjustment    *adjustment);
GtkWidget *ido_scale_menu_item_new_with_range    (const gchar      *label,
                                                  gdouble           min,
                                                  gdouble           max,
                                                  gdouble           step);
GtkWidget *ido_scale_menu_item_get_scale         (IdoScaleMenuItem *menuitem);

G_END_DECLS

#endif /* __IDO_SCALE_MENU_ITEM_H__ */
