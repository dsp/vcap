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
#include "gui/sunburst.h"

static void
vcap_gui_destroy (GtkWidget * widget __attribute__((unused)), gpointer data)
{
  pthread_cancel(*((pthread_t*) data)); /* cancel worker thread */

  gtk_main_quit ();
  exit (EXIT_SUCCESS);		/* terminate all threads hard */
}

void
vcap_gui_process (int *argc, char ***argv)
{
  GtkWidget *window, *area;
  pthread_t worker;

  gtk_init (argc, argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  g_signal_connect (G_OBJECT (window), "destroy",
		    G_CALLBACK (vcap_gui_destroy), &worker);
  gtk_container_set_border_width (GTK_CONTAINER (window), 10); 

  area = sunburst_new();
  gtk_container_add (GTK_CONTAINER(window), area);

  gtk_widget_set_size_request (area, 200, 200);

  gtk_widget_show_all (window);

#ifdef DEBUG
  pthread_t dworker;
  pthread_create(&dworker, NULL, vcap_gui_bgdisplayer, NULL);
#endif /* DEBUG */

  gtk_main ();

  pthread_exit (NULL);
}

static void
vcap_foreach_print(struct vcap_data_entry * res, void * userp __attribute__((unused)))
{
  unsigned int i;
  double percent;

  for (i = 0; i < res->level; i++)
	{
	  printf(" ");
	}

  percent = 1.0;
  if (res->parent) {
	percent = (res->amount / (float)res->parent->amount);
  }

  printf ("%s\t%5li packages in level %d %0.2f\n", res->ident, res->amount, res->level, percent);
}

void *
vcap_gui_bgdisplayer (void *param  __attribute__((unused)))
{
  printf ("Start background print\n");
  while (1)
    {
      sleep (1);
	  vcap_data_foreach(NULL, vcap_foreach_print, NULL);
	  printf("\n");
    }

  pthread_exit (NULL);
}
