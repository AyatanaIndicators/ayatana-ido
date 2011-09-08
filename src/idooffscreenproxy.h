/*
 * Copyright 2011 Canonical, Ltd.
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
 *    Robert Carr <racarr@canonical.com>
 */

#ifndef __IDO_OFFSCREEN_PROXY_H__
#define __IDO_OFFSCREEN_PROXy_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define IDO_TYPE_OFFSCREEN_PROXY              (ido_offscreen_proxy_get_type ())
#define IDO_OFFSCREEN_PROXY(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), IDO_TYPE_OFFSCREEN_PROXY, IdoOffscreenProxy))
#define IDO_OFFSCREEN_PROXY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), IDO_TYPE_OFFSCREEN_PROXY, IdoOffscreenProxyClass))
#define IDO_IS_OFFSCREEN_PROXY(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IDO_TYPE_OFFSCREEN_PROXY))
#define IDO_IS_OFFSCREEN_PROXY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), IDO_TYPE_OFFSCREEN_PROXY))
#define IDO_OFFSCREEN_PROXY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), IDO_TYPE_OFFSCREEN_PROXY, IdoOffscreenProxyClass))

typedef struct _IdoOffscreenProxy IdoOffscreenProxy;
typedef struct _IdoOffscreenProxyClass IdoOffscreenProxyClass;
typedef struct _IdoOffscreenProxyPrivate IdoOffscreenProxyPrivate;

struct _IdoOffscreenProxyClass
{
  GtkBinClass parent_class;
};

struct _IdoOffscreenProxy
{
  GtkContainer container;
  
  IdoOffscreenProxyPrivate *priv;
};

GType ido_offscreen_proxy_get_type (void) G_GNUC_CONST;
GtkWidget *ido_offscreen_proxy_new (void);

G_END_DECLS

#endif
