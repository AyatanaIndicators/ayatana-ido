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

#ifndef __IDO_GESTURE_MANAGER_H__
#define __IDO_GESTURE_MANAGER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define IDO_TYPE_GESTURE_MANAGER         (ido_gesture_manager_get_type ())
#define IDO_GESTURE_MANAGER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), IDO_TYPE_GESTURE_MANAGER, IdoGestureManager))
#define IDO_GESTURE_MANAGER_CLASS(c)     (G_TYPE_CHECK_CLASS_CAST ((c), IDO_TYPE_GESTURE_MANAGER, IdoGestureManagerClass))
#define IDO_IS_GESTURE_MANAGER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), IDO_TYPE_GESTURE_MANAGER))
#define IDO_IS_GESTURE_MANAGER_CLASS(c)  (G_TYPE_CHECK_CLASS_TYPE ((c), IDO_TYPE_GESTURE_MANAGER))
#define IDO_GESTURE_MANAGER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), IDO_TYPE_GESTURE_MANAGER, IdoGestureManagerClass))

typedef struct _IdoGestureManager        IdoGestureManager;
typedef struct _IdoGestureManagerClass   IdoGestureManagerClass;
typedef struct _IdoGestureManagerPrivate IdoGestureManagerPrivate;

typedef union  _IdoGestureEvent       IdoGestureEvent;
typedef struct _IdoEventGestureDrag   IdoEventGestureDrag;
typedef struct _IdoEventGesturePinch  IdoEventGesturePinch;
typedef struct _IdoEventGestureRotate IdoEventGestureRotate;

typedef enum {
  IDO_GESTURE_DRAG1,
  IDO_GESTURE_PINCH1,
  IDO_GESTURE_ROTATE1,

  IDO_GESTURE_DRAG2,
  IDO_GESTURE_PINCH2,
  IDO_GESTURE_ROTATE2,

  IDO_GESTURE_DRAG3,
  IDO_GESTURE_PINCH3,
  IDO_GESTURE_ROTATE3,

  IDO_GESTURE_DRAG4,
  IDO_GESTURE_PINCH4,
  IDO_GESTURE_ROTATE4,

  IDO_GESTURE_DRAG5,
  IDO_GESTURE_PINCH5,
  IDO_GESTURE_ROTATE5,

  IDO_GESTURE_TAP1,
  IDO_GESTURE_TAP2,
  IDO_GESTURE_TAP3,
  IDO_GESTURE_TAP4,
  IDO_GESTURE_TAP5
} IdoGestureType;

struct _IdoEventGestureDrag
{
  IdoGestureType  type;
  guint           id;
  GdkWindow      *window;
  GdkWindow      *root;
  GdkWindow      *child;
  guint32         timestamp;
  gint            fingers;
  gdouble         focus_x;
  gdouble         focus_y;
  gint            delta_x;
  gint            delta_y;
  gdouble         velocity_x;
  gdouble         velocity_y;
  gdouble         position_x;
  gdouble         position_y;
};

struct _IdoEventGesturePinch
{
  IdoGestureType  type;
  guint           id;
  GdkWindow      *window;
  GdkWindow      *root;
  GdkWindow      *child;
  guint32         timestamp;
  guint           fingers;
  gdouble         focus_x;
  gdouble         focus_y;
  gdouble         radius_delta;
  gdouble         radial_velocity;
  gdouble         radius;
};

struct _IdoEventGestureRotate
{
  IdoGestureType   type;
  guint            id;
  GdkWindow       *window;
  GdkWindow       *root;
  GdkWindow       *child;
  guint32          timestamp;
  guint            fingers;
  gdouble          focus_x;
  gdouble          focus_y;
  gdouble          angle_delta;
  gdouble          angular_velocity;
  gdouble          angle;
};

union _IdoGestureEvent
{
  IdoGestureType        type;
  IdoEventGestureDrag   drag;
  IdoEventGesturePinch  pinch;
  IdoEventGestureRotate rotate;
};

struct _IdoGestureManager
{
  GObject parent_instance;

  IdoGestureManagerPrivate *priv;
};

struct _IdoGestureManagerClass
{
  GObjectClass parent_class;
};

typedef void (* IdoGestureCallback) (GtkWindow        *window,
				     IdoGestureEvent  *gesture);

GType              ido_gesture_manager_get_type        (void) G_GNUC_CONST;
IdoGestureManager *ido_gesture_manager_get             (void);
void               ido_gesture_manager_register_window (IdoGestureManager *manager,
							GtkWindow         *window,
							IdoGestureType     gesture_type,
                                                        IdoGestureCallback start,
                                                        IdoGestureCallback update,
                                                        IdoGestureCallback end);

G_END_DECLS

#endif /* __IDO_GESTURE_MANAGER_H__ */
