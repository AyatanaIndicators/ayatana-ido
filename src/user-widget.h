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
#ifndef __USER_WIDGET_H__
#define __USER_WIDGET_H__

#include <gtk/gtk.h>
#include <libdbusmenu-gtk/menuitem.h>

G_BEGIN_DECLS

#define USER_WIDGET_TYPE            (user_widget_get_type ())
#define USER_WIDGET(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), USER_WIDGET_TYPE, UserWidget))
#define USER_WIDGET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), USER_WIDGET_TYPE, UserWidgetClass))
#define IS_USER_WIDGET(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), USER_WIDGET_TYPE))
#define IS_USER_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), USER_WIDGET_TYPE))
#define USER_WIDGET_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), USER_WIDGET_TYPE, UserWidgetClass))

typedef struct _UserWidget        UserWidget;
typedef struct _UserWidgetClass   UserWidgetClass;
typedef struct _UserWidgetPrivate UserWidgetPrivate;

struct _UserWidgetClass
{
  GtkMenuItemClass parent_class;
};

struct _UserWidget
{
  /*< private >*/
  GtkMenuItem parent;
  UserWidgetPrivate * priv;
};

GType user_widget_get_type (void) G_GNUC_CONST;
GtkWidget* user_widget_new(DbusmenuMenuitem *twin_item);

G_END_DECLS

#endif
