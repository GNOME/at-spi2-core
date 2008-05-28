#include <atk/atk.h>

#include "my-atk-text.h"
#include "my-atk-hyperlink.h"
#include "my-atk-hypertext.h"

//***************************implementation****************************************
static MyAtkTextClass *parent_class_atk_text = NULL;

typedef struct
{
    gint start_offset, end_offset;
    gint index;
}HyperlinkRange;
// Implementation of virtual functions

//***************************my_atk_hypertext_get_n_links*************************
gint my_atk_hypertext_get_n_links(AtkHypertext* hypertext)
{
    MyAtkHypertext* self = (MyAtkHypertext*)hypertext;
    
    return self->hyperlinks->len; 
}
//***************************my_atk_hypertext_get_link***********************
AtkHyperlink* my_atk_hypertext_get_link(AtkHypertext* hypertext, gint link_index)
{
    MyAtkHypertext* self = (MyAtkHypertext*)hypertext;
    
    if(link_index < 0 || link_index >= self->hyperlinks->len)
        return NULL;
    return g_ptr_array_index(self->hyperlinks, link_index);
}
//*************************my_atk_hypertext_get_link_index*******************
gint my_atk_hypertext_get_link_index(AtkHypertext* hypertext, gint char_index)
{
    gint i;
    MyAtkHypertext* self = (MyAtkHypertext*)hypertext;
    GArray* ranges = self->hyperlink_ranges; 
    for(i = ranges->len - 1; i >= 0; i--)
    {
        HyperlinkRange *range = &g_array_index(ranges, HyperlinkRange, i);
        if(range->start_offset <= char_index)
        {
            if(range->end_offset > char_index)return range->index;
            break;
        }
    }
    return -1;
}
//others functions
gboolean my_atk_hypertext_add_hyperlink(MyAtkHypertext* hypertext,
    gint start_index, gint end_index, const gchar* uri)
{
    MyAtkHyperlink* new_hyperlink;
    GArray* ranges = hypertext->hyperlink_ranges;
    gint i;
    for(i = 0; i < ranges->len; i++)
    {
        HyperlinkRange *range = &g_array_index(ranges, HyperlinkRange, i);
        if(range->end_offset <= start_index) continue;
        if(range->start_offset < end_index) return FALSE;
        break;
    }
    new_hyperlink = my_atk_hyperlink_new(start_index, end_index, uri);
    g_ptr_array_add(hypertext->hyperlinks, new_hyperlink);
    HyperlinkRange new_range;
    new_range.start_offset = start_index;
    new_range.end_offset = end_index;
    new_range.index = hypertext->hyperlinks->len - 1;
    g_array_insert_val(ranges, i, new_range);
    return TRUE;
}
//
void my_atk_hypertext_select_link(MyAtkHypertext* hypertext, gint index)
{
    if(index < 0 || index >= my_atk_hypertext_get_n_links((AtkHypertext*)hypertext))
        return;
    
    if(hypertext->current_selected_link != -1)
    {
        MyAtkHyperlink *selected_link_old = 
            (MyAtkHyperlink*)my_atk_hypertext_get_link(
            (AtkHypertext*)hypertext, hypertext->current_selected_link);
        selected_link_old->is_selected = FALSE;
    }
    
    hypertext->current_selected_link = index;
    MyAtkHyperlink *selected_link_new = (MyAtkHyperlink*)my_atk_hypertext_get_link(
        (AtkHypertext*)hypertext, hypertext->current_selected_link);
    selected_link_new->is_selected = TRUE;
    
    g_signal_emit_by_name(hypertext,
        "link-selected", hypertext->current_selected_link);    
}
//initialize/finalize functions
static void my_atk_hypertext_instance_init(GTypeInstance *obj, gpointer g_class)
{
    MyAtkHypertext *self = (MyAtkHypertext*)obj;
    
    self->hyperlink_ranges = g_array_new(FALSE, FALSE, sizeof(HyperlinkRange));
    self->hyperlinks = g_ptr_array_new();
    
    self->current_selected_link = -1;
}
static void my_atk_hypertext_instance_finalize(GObject* obj)
{
    MyAtkHypertext *self = (MyAtkHypertext*)obj;
    
    g_array_free(self->hyperlink_ranges, FALSE);
    
    g_ptr_array_foreach(self->hyperlinks,(GFunc)g_object_unref, NULL);
    g_ptr_array_free(self->hyperlinks, FALSE);
}

static void my_atk_hypertext_class_init(gpointer g_class, gpointer class_data)
{
    GObjectClass* g_object_class = (GObjectClass*)g_class;
    //GObject virtual table
    g_object_class->finalize = my_atk_hypertext_instance_finalize;
    //parent_class
    parent_class_atk_text = g_type_class_peek_parent(g_class);
}
static void my_atk_hypertext_interface_init(gpointer g_iface, gpointer iface_data)
{
    AtkHypertextIface *klass = (AtkHypertextIface*)g_iface;
    
    klass->get_link = my_atk_hypertext_get_link;
    klass->get_n_links = my_atk_hypertext_get_n_links;
    klass->get_link_index = my_atk_hypertext_get_link_index;
}

GType my_atk_hypertext_get_type()
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo typeInfo = 
        {
            sizeof(MyAtkHypertextClass),
            NULL,                       //base_init
            NULL,                       //base_finalize
            my_atk_hypertext_class_init,     //class_init
            NULL,                       //class_finalize
            NULL,                       //class_data
            sizeof(MyAtkHypertext),
            0,                          //n_preallocs
            my_atk_hypertext_instance_init   //instance_init
        };

        static const GInterfaceInfo AtkTextIface_info = 
        {
            my_atk_text_interface_init,         /* interface_init*/
            NULL,                               /* interface_finalize*/
            NULL                                /* interface_data */
        };
        static const GInterfaceInfo AtkHypertextIface_info = 
        {
            my_atk_hypertext_interface_init,/* interface_init*/
            NULL,                               /* interface_finalize*/
            NULL                                /* interface_data */
        };
        type = g_type_register_static(MY_TYPE_ATK_TEXT, "MyAtkHypertext", &typeInfo, 0);
        g_type_add_interface_static(type,
            ATK_TYPE_TEXT,
            &AtkTextIface_info);
        
        g_type_add_interface_static(type,
            ATK_TYPE_HYPERTEXT,
            &AtkHypertextIface_info);
    }
    return type;    
}
