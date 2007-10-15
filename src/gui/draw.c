#include "config.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <math.h>

#include "vcap.h"
#include "gui/vcap_gui.h"

static gboolean
vcap_gui_delete_event (GtkWidget * widget __attribute__((unused)), 
					   GdkEvent * event   __attribute__((unused)), 
					   gpointer data      __attribute__((unused)))
{
  /* If you return FALSE in the "delete_event" signal handler,
   * GTK will emit the "destroy" signal. Returning TRUE means
   * you don't want the window to be destroyed.
   * This is useful for popping up 'are you sure you want to quit?'
   * type dialogs. */

  /* Change TRUE to FALSE and the main window will be destroyed with
   * a "delete_event". */

  return FALSE;
}


static void
vcap_gui_destroy (GtkWidget * widget __attribute__((unused)), gpointer data)
{
  pthread_cancel((pthread_t*) data); /* cancel worker thread */

  gtk_main_quit ();
  exit (EXIT_SUCCESS);		/* terminate all threads hard */
}

static gboolean
vcap_gui_expose (GtkWidget *widget, GdkEventExpose *event __attribute__((unused)))
{
  cairo_t * cr;
  cr = gdk_cairo_create(widget->window);

  cairo_scale(cr, widget->allocation.width / 2, widget->allocation.height /2);
  cairo_translate(cr, 1, 1);

  vcap_sunburst_draw(cr);

  cairo_destroy(cr);
  
  return FALSE;
}

static void *
vcap_gui_cdrawing_worker(void * userp)
{
  GtkWidget *widget;
  GdkRegion *region;
  
  widget = GTK_WIDGET (userp);
  
  if (!widget->window) return NULL;

  while(1) 
	{
	  region = gdk_drawable_get_clip_region (widget->window);
	  /* redraw the cairo canvas completely by exposing it */
	  gdk_window_invalidate_region (widget->window, region, TRUE);
	  gdk_window_process_updates (widget->window, TRUE);
	  sleep(1);
	}

  pthread_exit(NULL);
}

void
vcap_gui_process (int *argc, char ***argv)
{
  GtkWidget *window, *area;
  pthread_t worker;

  gtk_init (argc, argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  g_signal_connect (G_OBJECT (window), "delete_event",
					G_CALLBACK (vcap_gui_delete_event), NULL);
  g_signal_connect (G_OBJECT (window), "destroy",
					G_CALLBACK (vcap_gui_destroy), &worker);
  gtk_container_set_border_width (GTK_CONTAINER (window), 10); 

  area = g_object_new( GTK_TYPE_DRAWING_AREA, NULL);
  gtk_container_add (GTK_CONTAINER(window), area);

  g_signal_connect (G_OBJECT (area), "expose-event",
					G_CALLBACK (vcap_gui_expose), NULL);

  gtk_widget_set_size_request (area, 200, 200);

  gtk_widget_show_all (window);

#ifdef DEBUG
  pthread_t dworker;
  pthread_create(&dworker, NULL, vcap_gui_bgdisplayer, NULL);
#endif /* DEBUG */

  /* 
   * prepare our cairo context and the drawer that will draw
   * our map periodicly. Its backed by the ctx_mutex
   */
  pthread_create(&worker, NULL, vcap_gui_cdrawing_worker, area);

  gtk_main ();

  pthread_exit (NULL);
}

static void
vcap_foreach_print(struct vcap_data_entry * res, void * userp __attribute__((unused)))
{
  printf ("%s\t%li packages in level %d\n", res->ident, res->amount, res->level);
}

void *
vcap_gui_bgdisplayer (void *param  __attribute__((unused)))
{
  while (1)
    {
      sleep (1);
	  vcap_data_foreach(NULL, vcap_foreach_print, NULL);
    }

  pthread_exit (NULL);
}
