/*
 * Copyright 2010 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of either or both of the following licenses:
 *
 * 1) the GNU Lesser General Public License version 3, as published by the
 * Free Software Foundation; and/or
 * 2) the GNU Lesser General Public License version 2.1, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 and version 2.1 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authors:
 *    Cody Russell <crussell@canonical.com>
 */

#include "idogesturemanager.h"

#include <gdk/gdkx.h>
#include <geis/geis.h>

typedef struct _IdoGestureRegistration IdoGestureRegistration;
typedef struct _IdoGestureBinding      IdoGestureBinding;

struct _IdoGestureManagerPrivate
{
  GHashTable *hash;
};

struct _IdoGestureBinding
{
  IdoGestureType     type;
  gint               touches;
  IdoGestureCallback start;
  IdoGestureCallback update;
  IdoGestureCallback end;
};

struct _IdoGestureRegistration
{
  GtkWindow         *window;
  GList             *bindings;
  GeisInstance       instance;
  GIOChannel        *iochannel;
};

static void gesture_added (void              *cookie,
                           GeisGestureType    gesture_type,
                           GeisGestureId      gesture_id,
                           GeisSize           attr_count,
                           GeisGestureAttr   *attrs);

static void gesture_removed (void              *cookie,
                             GeisGestureType    gesture_type,
                             GeisGestureId      gesture_id,
                             GeisSize           attr_count,
                             GeisGestureAttr   *attrs);

static void gesture_start (void              *cookie,
                           GeisGestureType    gesture_type,
                           GeisGestureId      gesture_id,
                           GeisSize           attr_count,
                           GeisGestureAttr   *attrs);

static void gesture_update (void              *cookie,
                            GeisGestureType    gesture_type,
                            GeisGestureId      gesture_id,
                            GeisSize           attr_count,
                            GeisGestureAttr   *attrs);

static void gesture_finish (void              *cookie,
                            GeisGestureType    gesture_type,
                            GeisGestureId      gesture_id,
                            GeisSize           attr_count,
                            GeisGestureAttr   *attrs);

static IdoGestureManager *manager_singleton = NULL;
static GeisGestureFuncs gesture_funcs = {
  gesture_added,
  gesture_removed,
  gesture_start,
  gesture_update,
  gesture_finish
};

G_DEFINE_TYPE (IdoGestureManager, ido_gesture_manager, G_TYPE_OBJECT)

#define IDO_GESTURE_MANAGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), IDO_TYPE_GESTURE_MANAGER, IdoGestureManagerPrivate))

static void
ido_gesture_manager_dispose (GObject *object)
{
  IdoGestureManagerPrivate *priv = IDO_GESTURE_MANAGER (object)->priv;

  if (priv->hash != NULL)
    {
      g_hash_table_unref (priv->hash);
      priv->hash = NULL;
    }
}

static void
ido_gesture_manager_finalize (GObject *object)
{
}

static GObject *
ido_gesture_manager_constructor (GType                  type,
                                 guint                  n_params,
                                 GObjectConstructParam *params)
{
  GObject *object;

  if (manager_singleton != NULL)
    {
      object = g_object_ref (manager_singleton);
    }
  else
    {
      object = G_OBJECT_CLASS (ido_gesture_manager_parent_class)->constructor (type,
                                                                               n_params,
                                                                               params);

      manager_singleton = IDO_GESTURE_MANAGER (object);
      g_object_add_weak_pointer (object, (gpointer) &manager_singleton);
    }

  return object;
}

static void
ido_gesture_manager_class_init (IdoGestureManagerClass *class)
{
  GObjectClass     *gobject_class;

  gobject_class = G_OBJECT_CLASS (class);

  ido_gesture_manager_parent_class = g_type_class_peek_parent (class);

  g_type_class_add_private (gobject_class, sizeof (IdoGestureManagerPrivate));

  gobject_class->constructor = ido_gesture_manager_constructor;
  gobject_class->dispose     = ido_gesture_manager_dispose;
  gobject_class->finalize    = ido_gesture_manager_finalize;
}

/*
static void
print_attr (GeisGestureAttr *attr)
{
  return;

  g_print ("\tattr '%s'=", attr->name);
  switch (attr->type)
    {
    case GEIS_ATTR_TYPE_BOOLEAN:
      g_print ("%s\n", attr->boolean_val ? "true" : "false");
      break;
    case GEIS_ATTR_TYPE_FLOAT:
      g_print ("%f\n", attr->float_val);
      break;
    case GEIS_ATTR_TYPE_INTEGER:
      g_print ("%d\n", (gint)attr->integer_val);
      break;
    case GEIS_ATTR_TYPE_STRING:
      g_print ("\"%s\"\n", attr->string_val);
      break;
    default:
      g_print ("<unknown>\n");
      break;
    }
}
*/

static gint
pinch_gesture_handle_properties (IdoEventGesturePinch *event,
                                 GeisSize              attr_count,
                                 GeisGestureAttr      *attrs)
{
  gint i = 0;
  gint touches = 0;

  for (i = 0; i < attr_count; ++i)
    {
      if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_TOUCHES) == 0 &&
          attrs[i].type == GEIS_ATTR_TYPE_INTEGER)
        {
          touches = attrs[i].integer_val;
        }
      if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_TIMESTAMP) == 0 &&
          attrs[i].type == GEIS_ATTR_TYPE_INTEGER)
        {
          event->timestamp = attrs[i].integer_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_FOCUS_X) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->focus_x = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_FOCUS_Y) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->focus_y = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_RADIUS_DELTA) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->radius_delta = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_RADIAL_VELOCITY) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->radial_velocity = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_RADIUS) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->radius = attrs[i].float_val;
        }
    }

  return touches;
}

static gint
drag_gesture_handle_properties (IdoEventGestureDrag *event,
                                GeisSize             attr_count,
                                GeisGestureAttr     *attrs)
{
  gint i;
  gint touches = 0;

  for (i = 0; i < attr_count; ++i)
    {
      if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_TOUCHES) == 0 &&
          attrs[i].type == GEIS_ATTR_TYPE_INTEGER)
        {
          touches = attrs[i].integer_val;
        }
      if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_TIMESTAMP) == 0 &&
          attrs[i].type == GEIS_ATTR_TYPE_INTEGER)
        {
          event->timestamp = attrs[i].integer_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_FOCUS_X) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->focus_x = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_FOCUS_Y) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->focus_y = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_DELTA_X) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->delta_x = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_DELTA_Y) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->delta_y = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_VELOCITY_X) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->velocity_x = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_VELOCITY_Y) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->velocity_y = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_POSITION_X) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->position_x = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_POSITION_Y) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->position_y = attrs[i].float_val;
        }
    }

  return touches;
}

static gint
rotate_gesture_handle_properties (IdoEventGestureRotate *event,
                                  GeisSize               attr_count,
                                  GeisGestureAttr       *attrs)
{
  gint i;
  gint touches = 0;

  for (i = 0; i < attr_count; ++i)
    {
      if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_TOUCHES) == 0 &&
          attrs[i].type == GEIS_ATTR_TYPE_INTEGER)
        {
          touches = attrs[i].integer_val;
        }
      if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_TIMESTAMP) == 0 &&
          attrs[i].type == GEIS_ATTR_TYPE_INTEGER)
        {
          event->timestamp = attrs[i].integer_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_FOCUS_X) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->focus_x = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_FOCUS_Y) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->focus_y = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_ANGLE_DELTA) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->angle_delta = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_ANGULAR_VELOCITY) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->angular_velocity = attrs[i].float_val;
        }
      else if (g_strcmp0 (attrs[i].name, GEIS_GESTURE_ATTRIBUTE_ANGLE) == 0 &&
               attrs[i].type == GEIS_ATTR_TYPE_FLOAT)
        {
          event->angle = attrs[i].float_val;
        }
    }

  return touches;
}


static void
gesture_added (void              *cookie,
               GeisGestureType    gesture_type,
               GeisGestureId      gesture_id,
               GeisSize           attr_count,
               GeisGestureAttr   *attrs)
{
}

static void
gesture_removed (void              *cookie,
                 GeisGestureType    gesture_type,
                 GeisGestureId      gesture_id,
                 GeisSize           attr_count,
                 GeisGestureAttr   *attrs)
{
}

static void
gesture_start (void              *cookie,
               GeisGestureType    type,
               GeisGestureId      id,
               GeisSize           attr_count,
               GeisGestureAttr   *attrs)
{
  IdoGestureRegistration *reg = (IdoGestureRegistration *)cookie;
  GList *l = NULL;

  for (l = reg->bindings; l != NULL; l = l->next)
    {
      IdoGestureBinding *binding = (IdoGestureBinding *)l->data;

      if (binding->type == type)
        {
          if (type == IDO_GESTURE_DRAG)
            {
              IdoEventGestureDrag drag;

              drag.type    = type;
              drag.id      = id;
              drag.fingers = drag_gesture_handle_properties (&drag,
                                                             attr_count,
                                                             attrs);

              if (drag.fingers == binding->touches)
                {
                  binding->start (reg->window,
                                  ((IdoGestureEvent*)&drag));
                }
            }
          else if (type == IDO_GESTURE_PINCH)
            {
              IdoEventGesturePinch pinch;

              pinch.type    = type;
              pinch.id      = id;
              pinch.fingers = pinch_gesture_handle_properties (&pinch,
                                                               attr_count,
                                                               attrs);

              if (pinch.fingers == binding->touches)
                {
                  binding->start (reg->window,
                                  ((IdoGestureEvent*)&pinch));
                }
            }
          else if (type == IDO_GESTURE_ROTATE)
            {
              IdoEventGestureRotate rotate;

              rotate.type    = type;
              rotate.id      = id;
              rotate.fingers = rotate_gesture_handle_properties (&rotate,
                                                                 attr_count,
                                                                 attrs);

              if (rotate.fingers == binding->touches)
                {
                  binding->start (reg->window,
                                  ((IdoGestureEvent*)&rotate));
                }
            }

          return;
        }
    }
}

static void
gesture_update (void              *cookie,
                GeisGestureType    type,
                GeisGestureId      id,
                GeisSize           attr_count,
                GeisGestureAttr   *attrs)
{
  IdoGestureRegistration *reg = (IdoGestureRegistration *)cookie;
  GList *l = NULL;

  for (l = reg->bindings; l != NULL; l = l->next)
    {
      IdoGestureBinding *binding = (IdoGestureBinding *)l->data;

      if (binding->type == type)
        {
          if (type == IDO_GESTURE_DRAG)
            {
              IdoEventGestureDrag drag;

              drag.type    = type;
              drag.id      = id;
              drag.fingers = drag_gesture_handle_properties (&drag,
                                                             attr_count,
                                                             attrs);

              if (drag.fingers == binding->touches)
                {
                  binding->update (reg->window,
                                   ((IdoGestureEvent*)&drag));
                }
            }
          else if (type == IDO_GESTURE_PINCH)
            {
              IdoEventGesturePinch pinch;

              pinch.type    = type;
              pinch.id      = id;
              pinch.fingers = pinch_gesture_handle_properties (&pinch,
                                                               attr_count,
                                                               attrs);

              if (pinch.fingers == binding->touches)
                {
                  binding->update (reg->window,
                                   ((IdoGestureEvent*)&pinch));
                }
            }
          else if (type == IDO_GESTURE_ROTATE)
            {
              IdoEventGestureRotate rotate;

              rotate.type    = type;
              rotate.id      = id;
              rotate.fingers = rotate_gesture_handle_properties (&rotate,
                                                                 attr_count,
                                                                 attrs);

              if (rotate.fingers == binding->touches)
                {
                  binding->update (reg->window,
                                   ((IdoGestureEvent*)&rotate));
                }
            }
        }
    }
}

static void
gesture_finish (void              *cookie,
                GeisGestureType    type,
                GeisGestureId      id,
                GeisSize           attr_count,
                GeisGestureAttr   *attrs)
{
  IdoGestureRegistration *reg = (IdoGestureRegistration *)cookie;
  GList *l = NULL;

  for (l = reg->bindings; l != NULL; l = l->next)
    {
      IdoGestureBinding *binding = (IdoGestureBinding *)l->data;

      if (binding->type == type)
        {
          if (type == IDO_GESTURE_DRAG)
            {
              IdoEventGestureDrag drag;

              drag.type    = type;
              drag.id      = id;
              drag.fingers = drag_gesture_handle_properties (&drag,
                                                             attr_count,
                                                             attrs);

              if (drag.fingers == binding->touches)
                {
                  binding->end (reg->window,
                                ((IdoGestureEvent*)&drag));
                }
            }
          else if (type == IDO_GESTURE_PINCH)
            {
              IdoEventGesturePinch pinch;

              pinch.type    = type;
              pinch.id      = id;
              pinch.fingers = pinch_gesture_handle_properties (&pinch,
                                                               attr_count,
                                                               attrs);

              if (pinch.fingers == binding->touches)
                {
                  binding->end (reg->window,
                                ((IdoGestureEvent*)&pinch));
                }
            }
          else if (type == IDO_GESTURE_ROTATE)
            {
              IdoEventGestureRotate rotate;

              rotate.type    = type;
              rotate.id      = id;
              rotate.fingers = rotate_gesture_handle_properties (&rotate,
                                                                 attr_count,
                                                                 attrs);

              if (rotate.fingers == binding->touches)
                {
                  binding->end (reg->window,
                                ((IdoGestureEvent*)&rotate));
                }
            }
        }
    }
}

static void
ido_gesture_manager_init (IdoGestureManager *item)
{
  IdoGestureManagerPrivate *priv;

  priv = item->priv = IDO_GESTURE_MANAGER_GET_PRIVATE (item);

  priv->hash = g_hash_table_new (g_direct_hash, g_direct_equal);
}

static gboolean
io_callback (GIOChannel *source,
             GIOCondition condition,
             gpointer     data)
{
  IdoGestureRegistration *reg = (IdoGestureRegistration *)data;

  geis_event_dispatch (reg->instance);

  return TRUE;
}

static void
window_destroyed_cb (GtkObject *object,
                     gpointer   user_data)
{
  IdoGestureManager *manager = (IdoGestureManager *)user_data;
  IdoGestureManagerPrivate *priv = manager->priv;
  IdoGestureRegistration *reg = g_hash_table_lookup (priv->hash, object);
  GList *list;

  for (list = reg->bindings; list != NULL; list = list->next)
    {
      IdoGestureBinding *binding = (IdoGestureBinding *)list->data;

      g_free (binding);
    }

  g_list_free (reg->bindings);

  g_io_channel_shutdown (reg->iochannel, TRUE, NULL);

  geis_finish (reg->instance);

  g_hash_table_remove (priv->hash, object);
  g_free (reg);
}


/* Public API */
IdoGestureManager *
ido_gesture_manager_get (void)
{
  return g_object_new (IDO_TYPE_GESTURE_MANAGER, NULL);
}

/**
 * ido_gesture_manager_register_window:
 * @window: A #GtkWindow to register the gesture event for.
 * @gesture_type: The type of gesture event to register.
 * @touch_points: Number of touch points for this gesture.
 * @start: Called when a user initiates a gesture.
 * @update: Called each time the user updates the gesture.
 * @end: Called when the user ends the gesture.
 *
 * Registers a toplevel window to receive gesture events.
 * The callback parameters provided will be called by the
 * #IdoGestureManager whenever the user initiates a gesture
 * on the specified window.
 */
void
ido_gesture_manager_register_window (IdoGestureManager *manager,
                                     GtkWindow         *window,
                                     IdoGestureType     gesture_type,
                                     gint               touch_points,
                                     IdoGestureCallback start,
                                     IdoGestureCallback update,
                                     IdoGestureCallback end)
{
  IdoGestureManagerPrivate *priv;
  IdoGestureRegistration *reg;
  IdoGestureBinding *binding;

  g_return_if_fail (IDO_IS_GESTURE_MANAGER (manager));
  g_return_if_fail (GTK_IS_WINDOW (window));
  g_return_if_fail (gtk_widget_get_realized (GTK_WIDGET (window)));

  priv = manager->priv;

  if (!(reg = g_hash_table_lookup (priv->hash, window)))
    {
      GeisInstance instance;
      GIOChannel *iochannel;
      gint fd = -1;
      GeisXcbWinInfo xcb_win_info = {
        .display_name = NULL,
        .screenp      = NULL,
        .window_id    = GDK_DRAWABLE_XID (GTK_WIDGET (window)->window)
      };
      GeisWinInfo win_info = {
        GEIS_XCB_FULL_WINDOW,
        &xcb_win_info
      };

      if (geis_init (&win_info, &instance) != GEIS_STATUS_SUCCESS)
        {
          g_warning ("Failed to initialize gesture manager.");
          return;
        }

      if (geis_configuration_supported (instance,
                                        GEIS_CONFIG_UNIX_FD) != GEIS_STATUS_SUCCESS)
        {
          g_warning ("Gesture manager does not support UNIX fd.");
          return;
        }

      if (geis_configuration_get_value (instance,
                                        GEIS_CONFIG_UNIX_FD,
                                        &fd) != GEIS_STATUS_SUCCESS)
        {
          g_error ("Gesture manager failed to obtain UNIX fd.");
          return;
        }

      reg = g_new0 (IdoGestureRegistration, 1);

      reg->window   = window;
      reg->instance = instance;

      g_signal_connect (window,
                        "destroy",
                        G_CALLBACK (window_destroyed_cb),
                        manager);

      geis_subscribe (reg->instance,
                      GEIS_ALL_INPUT_DEVICES,
                      GEIS_ALL_GESTURES,
                      &gesture_funcs,
                      reg);

      iochannel = g_io_channel_unix_new (fd);
      g_io_add_watch (iochannel,
                      G_IO_IN,
                      io_callback,
                      reg);

      reg->iochannel = iochannel;
    }

  /* XXX - check for duplicates in reg->bindings first */
  binding = g_new0 (IdoGestureBinding, 1);

  binding->type    = gesture_type;
  binding->touches = touch_points;
  binding->start   = start;
  binding->update  = update;
  binding->end     = end;

  reg->bindings = g_list_append (reg->bindings, binding);

  g_hash_table_insert (priv->hash,
                       window,
                       reg);
}
