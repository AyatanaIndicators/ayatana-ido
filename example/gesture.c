#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "idogesturemanager.h"

static gint    rotate = 0;
static gdouble scale = 1.0;
static gdouble translate[2] = { 200, 200 };
static gint    in_touch = 0;

static gboolean
expose_event (GtkWidget      *widget,
              GdkEventExpose *event,
              gpointer        data)
{
  cairo_t *cr;
  gdouble radians;
  gint width = (in_touch > 0) ? 10 : 1;

  cr = gdk_cairo_create (widget->window);

  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_set_line_width (cr, width);

  radians = rotate * (G_PI / 180);
  cairo_translate (cr, translate[0], translate[1]);
  cairo_scale (cr, scale, scale);
  cairo_rotate (cr, radians);

  cairo_rectangle (cr, -50, -50, 100, 100);
  cairo_stroke_preserve (cr);
  cairo_set_source_rgb (cr, 1, 0, 1);
  cairo_fill (cr);

  cairo_destroy (cr);

  return FALSE;
}

GtkWidget *
create_window (void)
{
  GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  GtkWidget *da;
  const GdkColor white = { 0, 0xffff, 0xffff, 0xffff };

  gtk_window_set_title (GTK_WINDOW (window), "Touch Demo");
  gtk_window_set_default_size (GTK_WINDOW (window), 600, 600);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_widget_destroyed), &window);

  da = gtk_drawing_area_new ();
  gtk_container_add (GTK_CONTAINER (window), da);

  gtk_widget_modify_bg (da, GTK_STATE_NORMAL, &white);

  g_signal_connect (da, "expose-event",
                    G_CALLBACK (expose_event), NULL);

  return window;
}

static void
gesture_start (GtkWindow       *window,
               IdoGestureEvent *event)
{
  in_touch++;

  gtk_widget_queue_draw (GTK_WIDGET (window));
}

static void
gesture_end (GtkWindow       *window,
             IdoGestureEvent *event)
{
  in_touch--;

  gtk_widget_queue_draw (GTK_WIDGET (window));
}

static void
rotate_update (GtkWindow       *window,
               IdoGestureEvent *event)
{
  IdoEventGestureRotate *e = (IdoEventGestureRotate *)event;

  rotate += e->angle_delta * 100;

  gtk_widget_queue_draw (GTK_WIDGET (window));
}

static void
pinch_update (GtkWindow       *window,
              IdoGestureEvent *event)
{
  IdoEventGesturePinch *e = (IdoEventGesturePinch *)event;

  scale += e->radius_delta / 100;

  g_print ("radius_delta == %f, scale is now %f\n", e->radius_delta, scale);

  gtk_widget_queue_draw (GTK_WIDGET (window));
}

static void
drag_update (GtkWindow       *window,
             IdoGestureEvent *event)
{
  IdoEventGestureDrag *e = (IdoEventGestureDrag *)event;

  translate[0] += e->delta_x;
  translate[1] += e->delta_y;

  gtk_widget_queue_draw (GTK_WIDGET (window));
}

static void
window_mapped (GtkWidget *widget)
{
  IdoGestureManager *manager = ido_gesture_manager_get ();
  GtkWindow *window = GTK_WINDOW (widget);

  ido_gesture_manager_register_window (manager,
                                       window,
                                       IDO_GESTURE_PINCH,
                                       2,
                                       gesture_start,
                                       pinch_update,
                                       gesture_end);

  ido_gesture_manager_register_window (manager,
                                       window,
                                       IDO_GESTURE_ROTATE,
                                       2,
                                       gesture_start,
                                       rotate_update,
                                       gesture_end);

  ido_gesture_manager_register_window (manager,
                                       window,
                                       IDO_GESTURE_DRAG,
                                       2,
                                       gesture_start,
                                       drag_update,
                                       gesture_end);
}

static void
abort_handler (int x)
{
  g_print (" **** ABORT ****\n");

  exit (1);
}

int
main (int argc, char **argv)
{
  GtkWidget *window;

  gtk_init (&argc, &argv);

  /* Don't crash X if we're using some shitty Intel graphics like
   * my Dell XT2 has in it. */
  signal (SIGABRT, abort_handler);

  window = create_window ();

  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (window, "map-event", G_CALLBACK (window_mapped), NULL);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}

