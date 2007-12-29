#ifndef __VCAP_GUI_H__
#define __VCAP_GUI_H__ 1

#include <gtk/gtk.h>

void vcap_gui_process (int *, char ***);
void *vcap_gui_bgdisplayer (void *);
void vcap_sunburst_draw(cairo_t *);

#endif /* __VCAP_GUI_H__ */
