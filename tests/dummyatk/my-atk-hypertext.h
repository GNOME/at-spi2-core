#ifndef MY_ATK_HYPERTEXT_H
#define MY_ATK_HYPERTEXT_H
/*
 * MyAtkHypertext: implements AtkHypertext
 */
#include <atk/atk.h>
#include <my-atk-text.h>

#define MY_TYPE_ATK_HYPERTEXT             (my_atk_hypertext_get_type ())
#define MY_ATK_HYPERTEXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_HYPERTEXT, MyAtkHypertext))
#define MY_ATK_HYPERTEXT_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_HYPERTEXT, MyAtkHypertextClass))
#define MY_IS_ATK_HYPERTEXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_HYPERTEXT))
#define MY_IS_ATK_HYPERTEXT_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_HYPERTEXT))
#define MY_ATK_HYPERTEXT_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_HYPERTEXT, MyAtkHypertextClass))

typedef struct _MyAtkHypertext MyAtkHypertext;
typedef struct _MyAtkHypertextClass MyAtkHypertextClass;

struct _MyAtkHypertext
{
    MyAtkText parent;
    
    GArray* hyperlink_ranges;
    GPtrArray* hyperlinks;
    
    gint current_selected_link;
};

struct _MyAtkHypertextClass
{
    MyAtkTextClass parent;
};
#endif /*MY_ATK_HYPERTEXT_H*/
