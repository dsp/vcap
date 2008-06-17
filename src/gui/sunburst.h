#ifndef __VCAP_SUNBURST_H__
#define __VCAP_SUNBURST_H__

G_BEGIN_DECLS

#define SUNBURST_TYPE             (sunburst_get_type ())
#define SUNBURST(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SUNBURST_TYPE, Sunburst))
#define SUNBURST_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), SUNBURST,  SunburstClass))
#define IS_SUNBURST(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SUNBURST_TYPE))
#define IS_SUNBURST_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), SUNBURST_TYPE))
#define SUNBURST_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), SUNBURST_TYPE, SunburstClass))

typedef struct _Sunburst Sunburst;
typedef struct _SunburstClass SunburstClass;

struct _Sunburst
{
  GtkDrawingArea parent;
};

struct _SunburstClass
{
  GtkDrawingAreaClass parent_class;
};

GtkWidget * sunburst_new(void);

G_END_DECLS

#endif /* __VCAP_SUNBURST_H__ */
