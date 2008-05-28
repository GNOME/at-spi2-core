#include <atk/atk.h>

#include "my-atk-text.h"
#include "my-atk-hyperlink.h"
#include "resources_storage.h"

//***************************implementation****************************************
static MyAtkTextClass *parent_class_atk_hyperlink = NULL;

// Implementation of virtual functions
//***************************my_atk_hyperlink_get_uri**************
gchar* my_atk_hyperlink_get_uri(AtkHyperlink* link_, gint index)
{
    MyAtkHyperlink* self = (MyAtkHyperlink*)link_;
    
    if(index < 0 || index >= self->number_of_anchors) return NULL;
    return g_strdup(self->uri);
}
//**************************my_atk_hyperlink_is_valid**************************
gboolean my_atk_hyperlink_is_valid(AtkHyperlink* link_)
{
    MyAtkHyperlink* self = (MyAtkHyperlink*)link_;
    
    return (resource_storage_get(self->uri) != NULL);
}
//*************************my_atk_hyperlink_get_object************************
AtkObject* my_atk_hyperlink_get_object(AtkHyperlink* link_, gint index)
{
    MyAtkHyperlink* self = (MyAtkHyperlink*)link_;
    
    if(index < 0 || index >= self->number_of_anchors) return NULL;
    return resource_storage_get(self->uri);
}
//***************************my_atk_hyperlink_get_start_index**************
gint my_atk_hyperlink_get_start_index(AtkHyperlink* link_)
{
    MyAtkHyperlink* self = (MyAtkHyperlink*)link_;
    
    return self->start_index;
}
//***************************my_atk_hyperlink_get_end_index**************
gint my_atk_hyperlink_get_end_index(AtkHyperlink* link_)
{
    MyAtkHyperlink* self = (MyAtkHyperlink*)link_;
    
    return self->end_index;
}
//***************************my_atk_hyperlink_link_state*******************
guint my_atk_hyperlink_link_state(AtkHyperlink* link_)
{
    return 0;
}
//***************************my_atk_hyperlink_get_n_anchors*******************
gboolean my_atk_hyperlink_get_n_anchors(AtkHyperlink* link_)
{
    return ((MyAtkHyperlink*)link_)->number_of_anchors;
}
//***************************my_atk_hypertlink_is_selected_link***********
gboolean my_atk_hyperlink_is_selected_link(AtkHyperlink* link_)
{
    MyAtkHyperlink* self = (MyAtkHyperlink*)link_;
    
    return self->is_selected;
}
//others functions
MyAtkHyperlink* my_atk_hyperlink_new(gint start_index, gint end_index,const gchar* uri)
{
    MyAtkHyperlink* result = g_object_new(MY_TYPE_ATK_HYPERLINK, NULL);
    if(result == NULL) return NULL;
    result->start_index = start_index;
    result->end_index = end_index;
    result->uri = g_strdup(uri);
    result->number_of_anchors = 1;
    return result;
}
void my_atk_hyperlink_activate(MyAtkHyperlink* hyperlink)
{
    g_signal_emit_by_name(hyperlink, "link-activated");
}
//initialize/finalize functions
static void my_atk_hyperlink_instance_init(GTypeInstance *obj, gpointer g_class)
{
    MyAtkHyperlink *self = (MyAtkHyperlink*)obj;
    
    self->start_index = self->end_index = 0;
    self->uri = NULL;
    self->is_selected = FALSE;
    self->number_of_anchors = 0;
}
static void my_atk_hyperlink_instance_finalize(GObject* obj)
{
    MyAtkHyperlink *self = (MyAtkHyperlink*)obj;
    
    g_free(self->uri);
}

static void my_atk_hyperlink_class_init(gpointer g_class, gpointer class_data)
{
    GObjectClass* g_object_class = (GObjectClass*)g_class;
    //GObject virtual table
    g_object_class->finalize = my_atk_hyperlink_instance_finalize;
    //parent_class
    parent_class_atk_hyperlink = g_type_class_peek_parent(g_class);
    //
    AtkHyperlinkClass* atkHyperlinkClass = (AtkHyperlinkClass*)g_class;
    
    atkHyperlinkClass->get_uri = my_atk_hyperlink_get_uri;
    atkHyperlinkClass->get_object = my_atk_hyperlink_get_object;
    atkHyperlinkClass->get_start_index = my_atk_hyperlink_get_start_index;
    atkHyperlinkClass->get_end_index = my_atk_hyperlink_get_end_index;
    atkHyperlinkClass->is_valid = my_atk_hyperlink_is_valid;
    atkHyperlinkClass->link_state = my_atk_hyperlink_link_state;
    atkHyperlinkClass->get_n_anchors = my_atk_hyperlink_get_n_anchors;
    atkHyperlinkClass->is_selected_link = my_atk_hyperlink_is_selected_link; 
}

GType my_atk_hyperlink_get_type()
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo typeInfo = 
        {
            sizeof(MyAtkHyperlinkClass),
            NULL,                       //base_init
            NULL,                       //base_finalize
            my_atk_hyperlink_class_init,     //class_init
            NULL,                       //class_finalize
            NULL,                       //class_data
            sizeof(MyAtkHyperlink),
            0,                          //n_preallocs
            my_atk_hyperlink_instance_init   //instance_init
        };
        type = g_type_register_static(ATK_TYPE_HYPERLINK, "MyAtkHyperlink", &typeInfo, 0);
    }
    return type;    
}
