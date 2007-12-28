#include <stdio.h>
#include <stdlib.h>
#include <string.h>  /* memset */
#include <glib.h>
#include <gtk/gtk.h> /* for cairo */
#include <math.h>
#include "vcap.h"

static GList * list;

static double
vcap_sunburst_draw_element (cairo_t * cr, const struct vcap_data_entry * e, unsigned int m_radius, double arc_start)
{
  unsigned int i;
  double arc_end, radius, nrsum, c;
  cairo_text_extents_t extends;

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
		  nrsum = vcap_sunburst_draw_element(cr, e->entries[i], m_radius, nrsum);
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

/*  if (e->ident) {
	cairo_text_extents (cr, e->ident, &extends);

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

	cairo_move_to (cr, 0, 0);
	cairo_rel_move_to (cr, cos(arc_end) * radius, sin(arc_end) * radius);

	cairo_show_text(cr, e->ident);
	cairo_stroke (cr); 

  } */

  return arc_end;
}


void
vcap_sunburst_draw (cairo_t * cr)
{
  unsigned int m_radius;

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

	  vcap_sunburst_draw_element(cr, vcap_data_root(NULL), m_radius, 0);
	}
}
