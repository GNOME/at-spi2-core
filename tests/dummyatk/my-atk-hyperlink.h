#ifndef MY_ATK_HYPERLINK_H
#define MY_ATK_HYPERLINK_H
/*
 * MyAtkHyperlink: implements AtkHyperlink
 */
#include <atk/atk.h>

#define MY_TYPE_ATK_HYPERLINK             (my_atk_hyperlink_get_type ())
#define MY_ATK_HYPERLINK(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_HYPERLINK, MyAtkHyperlink))
#define MY_ATK_HYPERLINK_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_HYPERLINK, MyAtkHyperlinkClass))
#define MY_IS_ATK_HYPERLINK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_HYPERLINK))
#define MY_IS_ATK_HYPERLINK_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_HYPERLINK))
#define MY_ATK_HYPERLINK_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_HYPERLINK, MyAtkHyperlinkClass))

typedef struct _MyAtkHyperlink MyAtkHyperlink;
typedef struct _MyAtkHyperlinkClass MyAtkHyperlinkClass;

struct _MyAtkHyperlink
{
    AtkHyperlink parent;
    
    gint start_index, end_index;
    
    gchar* uri;
    gint number_of_anchors;//0 on "clear" hyperlink and 1 after set it
    gboolean is_selected;
};

struct _MyAtkHyperlinkClass
{
    AtkHyperlinkClass parent;
};

GType my_atk_hyperlink_get_type();
#endif /*MY_ATK_HYPERLINK_H*/
