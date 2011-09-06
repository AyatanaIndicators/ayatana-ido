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

#include <gtk/gtk.h>
#include "idooffscreenproxy.h"

struct _IdoOffscreenProxyPrivate
{
  GtkWidget *child;
  
  GdkWindow *offscreen_window;
};

static void ido_offscreen_proxy_realize (GtkWidget *widget);
static void ido_offscreen_proxy_unrealize (GtkWidget *widget);
static void ido_offscreen_proxy_get_preferred_width (GtkWidget *widget,
						   gint *minimum,
						   gint *natural);
static void ido_offscreen_proxy_get_preferred_height (GtkWidget *widget,
						    gint *minimum,
						    gint *natural);

static void ido_offscreen_proxy_size_allocate (GtkWidget *widget,
					       GtkAllocation *allocation);
static gboolean ido_offscreen_proxy_damage (GtkWidget *widget,
					    GdkEventExpose *event);
static gboolean ido_offscreen_proxy_draw (GtkWidget *widget,
					  cairo_t *cr);
static void ido_offscreen_proxy_add (GtkContainer *container,
				   GtkWidget *child);
static void ido_offscreen_proxy_remove (GtkContainer *container,
				      GtkWidget *widget);
static void ido_offscreen_proxy_forall (GtkContainer *container,
				      gboolean include_internals,
				      GtkCallback callback,
				      gpointer callback_data);
static GType ido_offscreen_proxy_child_type (GtkContainer *container);

static cairo_surface_t * ido_offscreen_proxy_create_alpha_image_surface (GdkWindow *offscreen, gint width, gint height);



G_DEFINE_TYPE (IdoOffscreenProxy, ido_offscreen_proxy, GTK_TYPE_CONTAINER);

#define IDO_OFFSCREEN_PROXY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), IDO_TYPE_OFFSCREEN_PROXY, IdoOffscreenProxyPrivate))

static void
ido_offscreen_proxy_class_init (IdoOffscreenProxyClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  g_type_class_add_private (klass, sizeof (IdoOffscreenProxyPrivate));
  
  widget_class->realize = ido_offscreen_proxy_realize;
  widget_class->unrealize = ido_offscreen_proxy_unrealize;
  widget_class->get_preferred_width = ido_offscreen_proxy_get_preferred_width;
  widget_class->get_preferred_height = ido_offscreen_proxy_get_preferred_height;
  widget_class->size_allocate = ido_offscreen_proxy_size_allocate;
  widget_class->draw = ido_offscreen_proxy_draw;
  
  g_signal_override_class_closure (g_signal_lookup ("damage-event", GTK_TYPE_WIDGET),
				   IDO_TYPE_OFFSCREEN_PROXY,
				   g_cclosure_new (G_CALLBACK (ido_offscreen_proxy_damage),
						   NULL, NULL));
  
  container_class->add = ido_offscreen_proxy_add;
  container_class->remove = ido_offscreen_proxy_remove;
  container_class->forall = ido_offscreen_proxy_forall;
  container_class->child_type = ido_offscreen_proxy_child_type;

}

static void
ido_offscreen_proxy_init (IdoOffscreenProxy *proxy)
{
  proxy->priv = IDO_OFFSCREEN_PROXY_GET_PRIVATE (proxy);
  
  gtk_widget_set_has_window (GTK_WIDGET (proxy), TRUE);
  
  gtk_widget_set_events (GTK_WIDGET(proxy), gtk_widget_get_events (GTK_WIDGET(proxy))
    | GDK_EXPOSURE_MASK
    | GDK_POINTER_MOTION_MASK
    | GDK_BUTTON_PRESS_MASK
    | GDK_BUTTON_RELEASE_MASK
    | GDK_SCROLL_MASK
    | GDK_ENTER_NOTIFY_MASK
			 | GDK_LEAVE_NOTIFY_MASK);
  
  gtk_container_set_border_width (GTK_CONTAINER (proxy), 0);
}

GtkWidget *
ido_offscreen_proxy_new (void)
{
  return g_object_new (IDO_TYPE_OFFSCREEN_PROXY, NULL);
}

static GdkWindow *
pick_offscreen_child (GdkWindow *offscreen_window,
		       double widget_x, double widget_y,
		       IdoOffscreenProxy *proxy)
{
  GtkAllocation child_area;
  
  if (proxy->priv->child && gtk_widget_get_visible (proxy->priv->child))
    {
      gtk_widget_get_allocation (proxy->priv->child, &child_area);
      
      // if (widget_x >= 0 && widget_x < child_area.width &&
      //  widget_y >= 0 && widget_y < child_area.height)
      //return proxy->priv->offscreen_window;
      return proxy->priv->offscreen_window;
    }
  
  return NULL;
}

static void
offscreen_to_parent (GdkWindow *offscreen_window,
		     double offscreen_x,
		     double offscreen_y,
		     double *parent_x,
		     double *parent_y,
		     gpointer user_data)
{
  *parent_x = offscreen_x;
  *parent_y = offscreen_y;
}

static void
offscreen_from_parent (GdkWindow *window,
		       double parent_x,
		       double parent_y,
		       double *offscreen_x,
		       double *offscreen_y,
		       gpointer user_data)
{
  *offscreen_x = parent_x;
  *offscreen_y = parent_y;
}

static cairo_surface_t *
ido_offscreen_proxy_create_alpha_image_surface (GdkWindow *offscreen, gint width, gint height)
{
  return cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
}

static void
ido_offscreen_proxy_realize (GtkWidget *widget)
{
  IdoOffscreenProxy *proxy = IDO_OFFSCREEN_PROXY (widget);
  GtkAllocation allocation, child_area;
  GtkStyleContext *context;
  GdkWindow *window;
  GdkWindowAttr attributes;
  gint attributes_mask;
  //  GtkRequisition child_requisition;
  
  gtk_widget_set_realized (widget, TRUE);
  
  gtk_widget_get_allocation (widget, &allocation);
  
  attributes.x = allocation.x;
  attributes.y = allocation.y;
  attributes.width = allocation.width;
  attributes.height = allocation.height;
  
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events (widget)
    | GDK_EXPOSURE_MASK
    | GDK_POINTER_MOTION_MASK
    | GDK_BUTTON_PRESS_MASK
    | GDK_BUTTON_RELEASE_MASK
    | GDK_SCROLL_MASK
    | GDK_ENTER_NOTIFY_MASK
    | GDK_LEAVE_NOTIFY_MASK;
  attributes.visual = gdk_screen_get_rgba_visual (gdk_screen_get_default ());//gtk_widget_get_visual (widget);
  attributes.wclass = GDK_INPUT_OUTPUT;
  
  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
  
  window = gdk_window_new (gtk_widget_get_parent_window (widget),
			   &attributes, attributes_mask);
  
  gtk_widget_set_window (widget, window);
  gdk_window_set_user_data (window, widget);
  
  g_signal_connect (window, "pick-embedded-child",
		    G_CALLBACK (pick_offscreen_child), proxy);
  
  attributes.window_type = GDK_WINDOW_OFFSCREEN;
  attributes.x = attributes.y = 0;
  
  if (proxy->priv->child && gtk_widget_get_visible (proxy->priv->child))
    {
      gtk_widget_get_allocation (proxy->priv->child, &child_area);
      attributes.width = child_area.width;
      attributes.height = child_area.height;
    }
  
  proxy->priv->offscreen_window = gdk_window_new (gtk_widget_get_root_window (widget),
					    &attributes, attributes_mask);
  gdk_window_set_user_data (proxy->priv->offscreen_window, widget);
  
  if (proxy->priv->child)
    gtk_widget_set_parent_window (proxy->priv->child, proxy->priv->offscreen_window);
  
  gdk_offscreen_window_set_embedder (proxy->priv->offscreen_window,
				     window);
  
  g_signal_connect(proxy->priv->offscreen_window, "create-surface",
		   G_CALLBACK (ido_offscreen_proxy_create_alpha_image_surface),
		   proxy);
  g_signal_connect (proxy->priv->offscreen_window, "to-embedder",
		    G_CALLBACK (offscreen_to_parent), NULL);
  g_signal_connect (proxy->priv->offscreen_window, "from-embedder",
		    G_CALLBACK (offscreen_from_parent), NULL);
  
  context = gtk_widget_get_style_context (widget);
  gtk_style_context_add_class (context, GTK_STYLE_CLASS_MENUITEM);
  gtk_style_context_set_background (context, window);
  gtk_style_context_set_background (context, proxy->priv->offscreen_window);
  gtk_style_context_set_junction_sides (context, GTK_JUNCTION_TOP | GTK_JUNCTION_BOTTOM | GTK_JUNCTION_LEFT | GTK_JUNCTION_RIGHT);

  gdk_window_show (proxy->priv->offscreen_window);
}

static void
ido_offscreen_proxy_unrealize (GtkWidget *widget)
{
  IdoOffscreenProxy *proxy = IDO_OFFSCREEN_PROXY (widget);
  
  gdk_window_set_user_data (proxy->priv->offscreen_window, NULL);
  gdk_window_destroy (proxy->priv->offscreen_window);
  proxy->priv->offscreen_window = NULL;
  
  GTK_WIDGET_CLASS (ido_offscreen_proxy_parent_class)->unrealize (widget);
}

static GType
ido_offscreen_proxy_child_type (GtkContainer *container)
{
  IdoOffscreenProxy *proxy = IDO_OFFSCREEN_PROXY (container);
  
  if (proxy->priv->child)
    return G_TYPE_NONE;
  
  return GTK_TYPE_WIDGET;
}

static void
ido_offscreen_proxy_add (GtkContainer *container,
			 GtkWidget *child)
{
  IdoOffscreenProxy *proxy = IDO_OFFSCREEN_PROXY (container);
  
  if (!proxy->priv->child)
    {
      gtk_widget_set_parent_window (child, proxy->priv->offscreen_window);
      gtk_widget_set_parent (child, GTK_WIDGET (proxy));
      proxy->priv->child = child;
    }
  else
    {
      g_warning ("IdoOffscreenProxy can only have a single child\n");
    }
}

static void
ido_offscreen_proxy_remove (GtkContainer *container,
			  GtkWidget *widget)
{
  IdoOffscreenProxy *proxy = IDO_OFFSCREEN_PROXY (container);
  gboolean was_visible;
  
  was_visible = gtk_widget_get_visible (widget);
  
  if (proxy->priv->child == widget)
    {
      gtk_widget_unparent (widget);
      proxy->priv->child = NULL;
      
      if (was_visible && gtk_widget_get_visible (GTK_WIDGET (container)))
	gtk_widget_queue_resize (GTK_WIDGET (container));
    }
}

static void
ido_offscreen_proxy_forall (GtkContainer *container,
			    gboolean include_internals,
			    GtkCallback callback,
			    gpointer callback_data)
{
  IdoOffscreenProxy *proxy = IDO_OFFSCREEN_PROXY (container);
  
  g_return_if_fail (callback != NULL);
  
  if (proxy->priv->child)
    (*callback) (proxy->priv->child, callback_data);
}

static void
ido_offscreen_proxy_size_request (GtkWidget *widget,
				  GtkRequisition *requisition)
{
  IdoOffscreenProxy *proxy = IDO_OFFSCREEN_PROXY (widget);
  int w, h;
  
  w = 0;
  h = 0;
  
  if (proxy->priv->child && gtk_widget_get_visible (proxy->priv->child))
    {
      GtkRequisition child_requisition;
      
      gtk_widget_get_preferred_size (proxy->priv->child,
				     &child_requisition, NULL);
      w = child_requisition.width;
      h = child_requisition.height;
    }
  
  requisition->width = w;
  requisition->height = h;

}

static void
ido_offscreen_proxy_get_preferred_width (GtkWidget *widget,
					 gint *minimum,
					 gint *natural)
{
  GtkRequisition requisition;
  
  ido_offscreen_proxy_size_request (widget, &requisition);
  
  *minimum = *natural = requisition.width;
}

static void
ido_offscreen_proxy_get_preferred_height (GtkWidget *widget,
					  gint *minimum,
					  gint *natural)
{
  GtkRequisition requisition;
  
  ido_offscreen_proxy_size_request (widget, &requisition);
  
  *minimum = *natural = requisition.height;

}

static void
ido_offscreen_proxy_size_allocate (GtkWidget *widget,
				   GtkAllocation *allocation)
{
  IdoOffscreenProxy *proxy;
  
  proxy = IDO_OFFSCREEN_PROXY (widget);
  
  gtk_widget_set_allocation (widget, allocation);
  
  if (gtk_widget_get_realized (widget))
    {
      gdk_window_move_resize (gtk_widget_get_window (widget),
			      allocation->x,
			      allocation->y,
			      allocation->width,
			      allocation->height);
    }
  
  if (proxy->priv->child && gtk_widget_get_visible (proxy->priv->child))
    {
      GtkRequisition child_requisition;
      GtkAllocation child_allocation;
      
      gtk_widget_get_preferred_size (proxy->priv->child,
				     &child_requisition, NULL);
      
      child_allocation.x = child_requisition.width;
      child_allocation.y = child_requisition.height;
      child_allocation.width = allocation->width;
      child_allocation.height = allocation->height;
      
      if (gtk_widget_get_realized (widget))
	gdk_window_move_resize (proxy->priv->offscreen_window,
				child_allocation.x,
				child_allocation.y,
				child_allocation.width+4,
				child_allocation.height);
      
      child_allocation.x = child_allocation.y = 0;
      gtk_widget_size_allocate (proxy->priv->child, &child_allocation);
    }
}


static gboolean
ido_offscreen_proxy_damage (GtkWidget *widget,
			    GdkEventExpose *event)
{
  gdk_window_invalidate_rect (gtk_widget_get_window (widget),
			      NULL, FALSE);
  return TRUE;
}

static GtkStyleContext *
get_menu_style_context ()
{
  GtkStyleContext *sc;
  GtkWidgetPath *path;
  
  path = gtk_widget_path_new ();
  gtk_widget_path_append_type (path, GTK_TYPE_MENU);
  
  sc = gtk_style_context_new();
  
  gtk_style_context_set_path (sc, path);
  gtk_style_context_add_class (sc, GTK_STYLE_CLASS_MENU);
  
  gtk_widget_path_free (path);

  return sc;
}

static gboolean
ido_offscreen_proxy_draw (GtkWidget *widget,
			  cairo_t *cr)
{
  IdoOffscreenProxy *proxy = IDO_OFFSCREEN_PROXY (widget);
  GdkWindow *window;
  GtkStyleContext *sc, *wsc;
  GtkBorder border;
  
  window = gtk_widget_get_window (widget);
  
  sc = get_menu_style_context();
  wsc = gtk_widget_get_style_context (widget);
  gtk_style_context_get_border (wsc, gtk_widget_get_state (widget), &border);

  gtk_render_background (sc, cr,
			 -border.left,-border.top,
			 gdk_window_get_width (window) + border.right*2,
			 gdk_window_get_height (window) + border.bottom*2);
  
  g_object_unref (sc);


  
  if (gtk_cairo_should_draw_window (cr, window))
    {
      cairo_surface_t *surface;

      if (proxy->priv->child && gtk_widget_get_visible (proxy->priv->child))
	{
	  surface = gdk_offscreen_window_get_surface (proxy->priv->offscreen_window);

	  cairo_set_source_surface (cr, surface, 0, 0);
	  cairo_paint (cr);
	}
    }
    else if (gtk_cairo_should_draw_window (cr, proxy->priv->offscreen_window))
    {

      if (proxy->priv->child)
	gtk_container_propagate_draw (GTK_CONTAINER (widget),
				      proxy->priv->child,
				      cr);
				      }
  
  return TRUE;
}
