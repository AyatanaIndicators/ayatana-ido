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

#include <string.h>

#include <gtk/gtk.h>

#include "idomessagedialog.h"
#include "idotimeline.h"

#define IDO_MESSAGE_DIALOG_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), IDO_TYPE_MESSAGE_DIALOG, IdoMessageDialogPrivate))

static GtkWidget *ido_message_dialog_get_secondary_label (IdoMessageDialog *dialog);
static GtkWidget *ido_message_dialog_get_primary_label   (IdoMessageDialog *dialog);

typedef struct _IdoMessageDialogPrivate      IdoMessageDialogPrivate;
typedef struct _IdoMessageDialogMorphContext IdoMessageDialogMorphContext;

struct _IdoMessageDialogPrivate
{
  GtkWidget   *action_area;
  GtkWidget   *primary_label;
  GtkWidget   *secondary_label;

  gboolean     expanded;
};

struct _IdoMessageDialogMorphContext
{
  GtkWidget   *widget;
  IdoTimeline *timeline;

  GtkRequisition start;
  GtkRequisition end;
};

G_DEFINE_TYPE (IdoMessageDialog, ido_message_dialog, GTK_TYPE_MESSAGE_DIALOG)

static void
ido_message_dialog_map (GtkWidget *widget)
{
  IdoMessageDialog *dialog = IDO_MESSAGE_DIALOG (widget);
  IdoMessageDialogPrivate *priv = IDO_MESSAGE_DIALOG_GET_PRIVATE (dialog);

  GTK_WIDGET_CLASS (ido_message_dialog_parent_class)->map (widget);

  priv->primary_label = ido_message_dialog_get_primary_label (dialog);
  priv->secondary_label = ido_message_dialog_get_secondary_label (dialog);

  gtk_widget_hide (priv->secondary_label);

  gtk_label_set_selectable (GTK_LABEL (priv->primary_label), FALSE);
  gtk_label_set_selectable (GTK_LABEL (priv->secondary_label), FALSE);

  /* XXX: We really want to use gtk_window_set_deletable (GTK_WINDOW (widget), FALSE)
   *      here, but due to a bug in compiz this is more compatible.
   *
   * See: https://bugs.launchpad.net/ubuntu/+source/compiz/+bug/240794
   */
  gdk_window_set_functions (widget->window,
                            GDK_FUNC_RESIZE | GDK_FUNC_MOVE);

  ido_message_dialog_get_secondary_label (IDO_MESSAGE_DIALOG (widget));
}

static IdoMessageDialogMorphContext *
ido_message_dialog_morph_context_new (GtkWidget      *widget,
                                      IdoTimeline    *timeline,
                                      gpointer        identifier,
                                      GtkRequisition *start,
                                      GtkRequisition *end)
{
  IdoMessageDialogMorphContext *context;

  context = g_slice_new (IdoMessageDialogMorphContext);

  context->widget   = widget;
  context->timeline = timeline;
  context->start    = *start;
  context->end      = *end;

  return context;
}

static void
ido_message_dialog_morph_context_free (IdoMessageDialogMorphContext *context)
{
  g_object_unref (context->timeline);

  g_slice_free (IdoMessageDialogMorphContext, context);
}

static void
timeline_frame_cb (IdoTimeline *timeline,
                   gdouble      progress,
                   gpointer     user_data)
{
  IdoMessageDialogMorphContext *context = user_data;
  GtkRequisition start = context->start;
  GtkRequisition end = context->end;
  gint width_diff;
  gint height_diff;
  gint width, height;

  width_diff  = (MAX(start.width,  end.width)  - MIN(start.width,  end.width)) * progress;
  height_diff = (MAX(start.height, end.height) - MIN(start.height, end.height)) * progress;

  gtk_window_get_size (GTK_WINDOW (context->widget),
                       &width,
                       &height);

  gtk_widget_set_size_request (context->widget,
                               width_diff ? start.width + width_diff : -1,
                               height_diff ? start.height + height_diff : -1);
}

static void
timeline_finished_cb (IdoTimeline *timeline,
                      gpointer     user_data)
{
  IdoMessageDialogMorphContext *context = user_data;
  IdoMessageDialogPrivate *priv = IDO_MESSAGE_DIALOG_GET_PRIVATE (context->widget);

  gtk_widget_show (priv->action_area);
  gtk_widget_show (priv->secondary_label);

  ido_message_dialog_morph_context_free (context);
}

static gboolean
button_pressed (GtkWidget      *event_box,
                GdkEventButton *event,
                gpointer        user_data)
{
  IdoMessageDialog *dialog = (IdoMessageDialog *) user_data;
  IdoMessageDialogPrivate *priv = IDO_MESSAGE_DIALOG_GET_PRIVATE (dialog);

  if (!priv->expanded)
    {
      GtkRequisition start;
      GtkRequisition end;
      IdoTimeline *timeline;
      IdoMessageDialogMorphContext *context;

      start = GTK_WIDGET (dialog)->requisition;

      priv->expanded = TRUE;

      gtk_widget_show (priv->action_area);
      gtk_widget_show (priv->secondary_label);

      gtk_widget_size_request (GTK_WIDGET (dialog), &end);

      gtk_widget_hide (priv->action_area);
      gtk_widget_hide (priv->secondary_label);

      timeline = ido_timeline_new (500);
      context = ido_message_dialog_morph_context_new (GTK_WIDGET (dialog),
                                                      timeline,
                                                      "foo",
                                                      &start,
                                                      &end);
      g_signal_connect (timeline,
                        "frame",
                        G_CALLBACK (timeline_frame_cb),
                        context);
      g_signal_connect (timeline,
                        "finished",
                        G_CALLBACK (timeline_finished_cb),
                        context);

      ido_timeline_start (timeline);
    }

  return FALSE;
}

static void
ido_message_dialog_constructed (GObject *object)
{
  IdoMessageDialogPrivate *priv = IDO_MESSAGE_DIALOG_GET_PRIVATE (object);
  GtkWidget *vbox;
  GtkWidget *event_box;

  event_box = gtk_event_box_new ();
  gtk_widget_show (event_box);
  g_signal_connect (event_box,
                    "button-press-event",
                    G_CALLBACK (button_pressed),
                    object);

  vbox = GTK_DIALOG (object)->vbox;
  priv->action_area = gtk_dialog_get_action_area (GTK_DIALOG (object));

  g_object_ref (G_OBJECT (vbox));
  gtk_container_remove (GTK_CONTAINER (object), vbox);
  gtk_container_add (GTK_CONTAINER (event_box), vbox);
  gtk_container_add (GTK_CONTAINER (object), event_box);

  gtk_widget_hide (priv->action_area);
}

static void
ido_message_dialog_class_init (IdoMessageDialogClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->constructed = ido_message_dialog_constructed;

  widget_class->map         = ido_message_dialog_map;

  g_type_class_add_private (object_class, sizeof (IdoMessageDialogPrivate));
}

static void
ido_message_dialog_init (IdoMessageDialog *dialog)
{
  IdoMessageDialogPrivate *priv;

  gtk_window_set_focus_on_map (GTK_WINDOW (dialog), FALSE);

  priv = IDO_MESSAGE_DIALOG_GET_PRIVATE (dialog);
}

/**
 * ido_message_dialog_new:
 * @parent: transient parent, or %NULL for none
 * @flags: flags
 * @type: type of message
 * @buttons: a set of buttons to use
 * @message_format: printf()-style format string, or %NULL
 * @Varargs: arguments for @message_format
 *
 * Creates a new message dialog, which is based upon
 * GtkMessageDialog so it shares API and functionality
 * with it.  IdoMessageDialog differs in that it has two
 * states.  The initial state hides the action buttons
 * and the secondary message.  When a user clicks on the
 * dialog it will expand to provide the secondary message
 * and the action buttons.
 *
 * Return value: a new #IdoMessageDialog
 **/
GtkWidget*
ido_message_dialog_new (GtkWindow      *parent,
                        GtkDialogFlags  flags,
                        GtkMessageType  type,
                        GtkButtonsType  buttons,
                        const gchar    *message_format,
                        ...)
{
  GtkWidget *widget;
  GtkDialog *dialog;
  gchar* msg = NULL;
  va_list args;

  g_return_val_if_fail (parent == NULL || GTK_IS_WINDOW (parent), NULL);

  widget = g_object_new (IDO_TYPE_MESSAGE_DIALOG,
                         "message-type", type,
                         "buttons", buttons,
                         NULL);
  dialog = GTK_DIALOG (widget);

  if (flags & GTK_DIALOG_NO_SEPARATOR)
    {
      g_warning ("The GTK_DIALOG_NO_SEPARATOR flag cannot be used for IdoMessageDialog");
      flags &= ~GTK_DIALOG_NO_SEPARATOR;
    }

  if (message_format)
    {
      va_start (args, message_format);
      msg = g_strdup_vprintf (message_format, args);
      va_end (args);

      gtk_label_set_text (GTK_LABEL (GTK_MESSAGE_DIALOG (widget)->label),
                          msg);

      g_free (msg);
    }

  if (parent != NULL)
    gtk_window_set_transient_for (GTK_WINDOW (widget),
                                  GTK_WINDOW (parent));

  if (flags & GTK_DIALOG_MODAL)
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);

  if (flags & GTK_DIALOG_DESTROY_WITH_PARENT)
    gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);

  return widget;
}

GtkWidget*
ido_message_dialog_new_with_markup (GtkWindow      *parent,
                                    GtkDialogFlags  flags,
                                    GtkMessageType  type,
                                    GtkButtonsType  buttons,
                                    const gchar    *message_format,
                                    ...)
{
  GtkWidget *widget;
  va_list args;
  gchar *msg = NULL;

  g_return_val_if_fail (parent == NULL || GTK_IS_WINDOW (parent), NULL);

  widget = ido_message_dialog_new (parent, flags, type, buttons, NULL);

  if (message_format)
    {
      va_start (args, message_format);
      msg = g_markup_vprintf_escaped (message_format, args);
      va_end (args);

      gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (widget), msg);

      g_free (msg);
    }

  return widget;
}

 /*
  * This is almost humorously stupid.  We jump through some hoops and kill
  * a few kittens here because we want to preserve API compatibility with
  * GtkMessageDialog and extend it instead of duplicating its functionality.
  * If only GtkMessageDialog were easier to extend then maybe all those
  * kittens wouldn't have had to die...
  */
static GtkWidget *
ido_message_dialog_get_label (IdoMessageDialog *dialog, gboolean primary)
{
  GList *list;
  gchar *text;
  gchar *secondary_text;

  g_object_get (G_OBJECT (dialog),
                "text", &text,
                "secondary-text", &secondary_text,
                NULL);

  g_return_val_if_fail (IDO_IS_MESSAGE_DIALOG (dialog), NULL);

  for (list = GTK_BOX (GTK_DIALOG (dialog)->vbox)->children; list != NULL; list = list->next)
    {
      GtkBoxChild *child = (GtkBoxChild *)list->data;

      if (G_TYPE_FROM_INSTANCE (child->widget) == GTK_TYPE_HBOX)
        {
          GList *hlist;
          GtkWidget *hbox = child->widget;

          for (hlist = GTK_BOX (hbox)->children; hlist != NULL; hlist = hlist->next)
            {
              GtkBoxChild *hchild = (GtkBoxChild *)hlist->data;

              if (G_TYPE_FROM_INSTANCE (hchild->widget) == GTK_TYPE_VBOX)
                {
                  GList *vlist;
                  GtkWidget *vbox = hchild->widget;

                  for (vlist = GTK_BOX (vbox)->children; vlist != NULL; vlist = vlist->next)
                    {
                      GtkBoxChild *vchild = (GtkBoxChild *)vlist->data;
                      GtkLabel *label;

                      label = GTK_LABEL (vchild->widget);

                      if (strcmp ((primary ? text : secondary_text), label->label) == 0)
                        {
                          return vchild->widget;
                        }
                    }
                }
            }
        }
    }

  return NULL;
}

static GtkWidget *
ido_message_dialog_get_secondary_label (IdoMessageDialog *dialog)
{
  return ido_message_dialog_get_label (dialog, FALSE);
}

static GtkWidget *
ido_message_dialog_get_primary_label (IdoMessageDialog *dialog)
{
  return ido_message_dialog_get_label (dialog, TRUE);
}
