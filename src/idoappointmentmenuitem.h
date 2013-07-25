/**
 * Copyright 2013 Canonical Ltd.
 *
 * Authors:
 *   Charles Kerr <charles.kerr@canonical.com>
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

#ifndef __IDO_APPOINTMENT_MENU_ITEM_H__
#define __IDO_APPOINTMENT_MENU_ITEM_H__

#include <time.h> /* time_t */

#include <gtk/gtk.h>
#include "idobasicmenuitem.h"

G_BEGIN_DECLS

#define IDO_APPOINTMENT_MENU_ITEM_TYPE    (ido_appointment_menu_item_get_type ())
#define IDO_APPOINTMENT_MENU_ITEM(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), IDO_APPOINTMENT_MENU_ITEM_TYPE, IdoAppointmentMenuItem))
#define IDO_IS_APPOINTMENT_MENU_ITEM(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IDO_APPOINTMENT_MENU_ITEM_TYPE))

typedef struct _IdoAppointmentMenuItem        IdoAppointmentMenuItem;
typedef struct _IdoAppointmentMenuItemClass   IdoAppointmentMenuItemClass;
typedef struct _IdoAppointmentMenuItemPrivate IdoAppointmentMenuItemPrivate;

struct _IdoAppointmentMenuItemClass
{
  IdoBasicMenuItemClass parent_class;
};

/**
 * A menuitem that indicates a appointment.
 *
 * It contains a color-coded icon to indicate the appointment type,
 * a primary label showing the appointment summary,
 * and a right-justified secondary label telling when the appointment begins.
 */
struct _IdoAppointmentMenuItem
{
  /*< private >*/
  IdoBasicMenuItem parent;
  IdoAppointmentMenuItemPrivate * priv;
};


GType ido_appointment_menu_item_get_type (void) G_GNUC_CONST;

GtkWidget * ido_appointment_menu_item_new (void);

GtkMenuItem * ido_appointment_menu_item_new_from_model (GMenuItem    * menuitem,
                                                        GActionGroup * actions);

void ido_appointment_menu_item_set_time (IdoAppointmentMenuItem * menuitem,
                                         time_t                   time);

void ido_appointment_menu_item_set_color (IdoAppointmentMenuItem * menuitem,
                                          const char             * color_str);

void ido_appointment_menu_item_set_format (IdoAppointmentMenuItem * menuitem,
                                           const char             * strftime_fmt);


G_END_DECLS

#endif
