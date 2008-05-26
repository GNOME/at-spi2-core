#include "MyAtkImplementor.h"

//implementaion
static GObjectClass *parent_class = NULL;

static AtkObject* my_atk_implementor_ref_accessible(AtkImplementor *implementor)
{
    AtkObject *result = ((MyAtkImplementor*)implementor)->accessible;
    g_object_ref((GObject*)result);
    return result;
}

static void my_atk_implementor_instance_init(GTypeInstance *instance, gpointer g_class)
{
    MyAtkImplementor *self = (MyAtkImplementor*)instance;
    
    self->accessible = ATK_OBJECT(g_object_new(ATK_TYPE_OBJECT,NULL));
    self->is_disposed = FALSE;
}

static void my_atk_implementor_interface_init(gpointer g_iface,gpointer iface_data)
{
    AtkImplementorIface *iface = ((AtkImplementorIface*)g_iface);
    iface->ref_accessible = my_atk_implementor_ref_accessible;
}

static void my_atk_implementor_dispose(GObject *obj)
{
    MyAtkImplementor *self = (MyAtkImplementor*)obj;
    if(!self->is_disposed)
    {
        g_object_unref(self->accessible);
        self->is_disposed = TRUE;
    }
    parent_class->dispose(obj);
}

static void my_atk_implementor_class_init(gpointer g_class,gpointer g_data)
{
    MyAtkImplementorClass *klass = MY_ATK_IMPLEMENTOR_CLASS(g_class);
    parent_class = g_type_class_peek_parent(klass);
}

GType my_atk_implementor_get_type(void)
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo typeinfo = {
            sizeof(MyAtkImplementorClass),
            NULL, /*base_init */
            NULL, /*base_finalize */
            my_atk_implementor_class_init,/*class_init*/
            NULL, /*class_finalize*/
            NULL,/*class_data*/
            sizeof(MyAtkImplementor),
            0, /*n_prealloc*/
            my_atk_implementor_instance_init/* instance_init*/
        };
        type = g_type_register_static(G_TYPE_OBJECT,"MyAtkImplementor",&typeinfo,0);
        static const GInterfaceInfo interface_info = 
        {
            my_atk_implementor_interface_init, /*interface_init*/
            NULL, /*interface_finalize*/
            NULL, /*interface_data*/
        };
        g_type_add_interface_static(type, ATK_TYPE_IMPLEMENTOR,&interface_info);
    }
    return type;
}
