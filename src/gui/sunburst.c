#include <stdio.h>
#include <stdlib.h>
#include <string.h>  /* memset */
#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h> /* for cairo */
#include <math.h>

#include "vcap.h"
#include "gui/sunburst.h"

G_DEFINE_TYPE (Sunburst, sunburst, GTK_TYPE_DRAWING_AREA);

static double
vcap_sunburst_draw (cairo_t * cr, const struct vcap_data_entry * e, unsigned int m_radius, double arc_start)
{
  unsigned int i;
  double arc_end, radius, nrsum, c;

  radius = ((double)e->level / m_radius); 

  if (e->parent) {
    arc_end = arc_start + ((double)e->amount / vcap_data_lookup("eth")->amount) * 2 * M_PI;
  } else {
    arc_end = 2 * M_PI;
  }

  if (e->entries) 
    {
      nrsum = arc_start;
      for (i = 0; i < e->ecount; i++)
	{
	  nrsum = vcap_sunburst_draw(cr, e->entries[i], m_radius, nrsum);
	}
    }

  cairo_move_to (cr, 0, 0);
  cairo_rel_line_to (cr, cos (arc_start) * radius, sin (arc_start) * radius);
  cairo_arc (cr, 0, 0, radius, arc_start, arc_end);
  cairo_line_to (cr, 0, 0);

  c = ((double)(1/(double)(arc_start+e->level+1)))+0.2;
  cairo_set_source_rgb (cr, c + (e->level%3 > 0)*0.02, c + (e->level/3)*0.05, c + (e->level%2 == 0)*0.1); 
  cairo_fill_preserve (cr);

  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_set_line_width (cr, 0.001);
  cairo_stroke (cr);

  return arc_end;
}

static gboolean
sunburst_expose(GtkWidget * widget, GdkEventExpose *event __attribute__((unused)))
{
  GList * list;
  cairo_t * cr;
  unsigned int m_radius;

  cr = gdk_cairo_create(widget->window);
  cairo_scale(cr, widget->allocation.width / 2, widget->allocation.height /2);
  cairo_translate(cr, 1, 1);

  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
  cairo_select_font_face (cr, "Helvetica",  CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (cr, 0.04);

  list = vcap_data_raw_list();

  if (g_list_length(list) <= 1)
    {
      cairo_move_to(cr, -1, -0.9);
      cairo_show_text(cr, "No packages captures yet");
      cairo_stroke (cr);
    }
  else 
    {
      m_radius = ((vcap_data_entry_t*)g_list_nth_data(list, 0))->level + 1;
      vcap_sunburst_draw(cr, vcap_data_root(NULL), m_radius, 0);
    }

  cairo_destroy(cr);

  return FALSE;
}

static void
sunburst_class_init(SunburstClass *class)
{
  GtkWidgetClass *wclass;
  wclass = GTK_WIDGET_CLASS(class);

  /* set handlers */
  wclass->expose_event = sunburst_expose;
}

static gboolean
sunburst_update (gpointer data)
{
  GtkWidget *widget;
  GdkRegion *region;

  widget = GTK_WIDGET(data);

  VCAP_LOCK_DATA_TSAFE();

  region = gdk_drawable_get_clip_region (widget->window);
  gdk_window_invalidate_region (widget->window, region, TRUE);
  gdk_window_process_updates (widget->window, TRUE);
  gdk_region_destroy (region);

  VCAP_UNLOCK_DATA_TSAFE();

  return TRUE;
}

static void
sunburst_init(Sunburst *obj)
{
  g_timeout_add (1000, sunburst_update, obj);
}

GtkWidget *
sunburst_new(void)
{
  return g_object_new(SUNBURST_TYPE, NULL);
}
