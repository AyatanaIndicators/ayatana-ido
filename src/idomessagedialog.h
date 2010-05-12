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
 *
 * Design and specification:
 *    Matthew Paul Thomas <mpt@canonical.com>
 */

#ifndef __IDO_MESSAGE_DIALOG_H__
#define __IDO_MESSAGE_DIALOG_H__

#include <gtk/gtkmessagedialog.h>

#define IDO_TYPE_MESSAGE_DIALOG         (ido_message_dialog_get_type ())
#define IDO_MESSAGE_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), IDO_TYPE_MESSAGE_DIALOG, IdoMessageDialog))
#define IDO_MESSAGE_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), IDO_TYPE_MESSAGE_DIALOG, IdoMessageDialogClass))
#define IDO_IS_MESSAGE_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), IDO_TYPE_MESSAGE_DIALOG))
#define IDO_IS_MESSAGE_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), IDO_TYPE_MESSAGE_DIALOG))
#define IDO_MESSAGE_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), IDO_TYPE_MESSAGE_DIALOG, IdoMessageDialogClass))

typedef struct _IdoMessageDialog        IdoMessageDialog;
typedef struct _IdoMessageDialogClass   IdoMessageDialogClass;

struct _IdoMessageDialog
{
  GtkMessageDialog parent_instance;
};

struct _IdoMessageDialogClass
{
  GtkMessageDialogClass parent_class;

  /* Padding for future expansion */
  void (*_ido_reserved1) (void);
  void (*_ido_reserved2) (void);
  void (*_ido_reserved3) (void);
  void (*_ido_reserved4) (void);
};

GType      ido_message_dialog_get_type (void) G_GNUC_CONST;

GtkWidget* ido_message_dialog_new      (GtkWindow      *parent,
                                        GtkDialogFlags  flags,
                                        GtkMessageType  type,
                                        GtkButtonsType  buttons,
                                        const gchar    *message_format,
                                        ...) G_GNUC_PRINTF (5, 6);

GtkWidget* ido_message_dialog_new_with_markup   (GtkWindow      *parent,
                                                 GtkDialogFlags  flags,
                                                 GtkMessageType  type,
                                                 GtkButtonsType  buttons,
                                                 const gchar    *message_format,
                                                 ...) G_GNUC_PRINTF (5, 6);

G_END_DECLS

#endif /* __IDO_MESSAGE_DIALOG_H__ */
