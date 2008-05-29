#include <atk/atk.h>

#include "my-atk-object.h"
#include "my-atk-streamable-content.h"

//*************************implementation***********************
//implementation of virtual functions
//*****************get_n_mime_types************
static gint my_atk_streamable_content_get_n_mime_types(
    AtkStreamableContent *streamable)
{
    g_return_val_if_fail(MY_IS_ATK_STREAMABLE_CONTENT(streamable), 0);
    
    return sizeof(mime_types) / sizeof(mime_types[0]);
}
//*****************get_mime_type****************
static const gchar* my_atk_streamable_content_get_mime_type(
    AtkStreamableContent *streamable,
    gint i)
{
    g_return_val_if_fail(MY_IS_ATK_STREAMABLE_CONTENT(streamable), NULL);
    
    if((i < 0) || (i >= sizeof(mime_types) / sizeof(mime_types[0])))
    {
        return NULL;
    }
    return mime_types[i];
}
//**********************get_stream*******************
static GIOChannel* my_atk_streamable_content_get_stream(
    AtkStreamableContent *streamable,
    const gchar* mime_type)
{
    gint i;
    g_return_val_if_fail(MY_IS_ATK_STREAMABLE_CONTENT(streamable), NULL);
    
    for(i = 0; i < sizeof(mime_types) / sizeof(mime_types[0]); i++)
    {
        if(strcmp(mime_type, mime_types[i]) == 0)
        {
            GError *error = NULL;
            gchar* full_filename = T2C_GET_DATA_PATH(file_names[i]);
            GIOChannel* channel = g_io_channel_new_file(full_filename, "r", &error);
            if(error != NULL)
            {
                TRACE("Cannot open file '%s' for read: %s", full_filename,
                    error->message);
                g_error_free(error);
            }
            g_free(full_filename);
            return channel;
        }
    }
    return NULL;
}
//others functions
static void my_atk_streamable_content_interface_init(gpointer g_iface, gpointer iface_data)
{
    AtkStreamableContentIface *klass = (AtkStreamableContentIface*)g_iface;
    
    klass->get_n_mime_types = my_atk_streamable_content_get_n_mime_types;
    klass->get_mime_type = my_atk_streamable_content_get_mime_type;
    klass->get_stream = my_atk_streamable_content_get_stream;
}

GType my_atk_streamable_content_get_type()
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo typeInfo = 
        {
            sizeof(MyAtkStreamableContentClass),
            NULL, //base_init
            NULL, //base_finalize
            NULL, //class_init
            NULL, //class_finalize
            NULL, //class_data
            sizeof(MyAtkStreamableContent),
            0, //n_preallocs
            NULL //instance_init
        };

        static const GInterfaceInfo iface_info = 
        {
            my_atk_streamable_content_interface_init,    /* interface_init*/
            NULL,                               /* interface_finalize*/
            NULL                                /* interface_data */
        };
        type = g_type_register_static(MY_TYPE_ATK_OBJECT, "MyAtkStreamableContent", &typeInfo, 0);
        g_type_add_interface_static(type,
            ATK_TYPE_STREAMABLE_CONTENT,
            &iface_info);
    }
    return type;    
}
