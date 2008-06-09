#include <atk/atk.h>
#include <limits.h>

#include "my-atk-object.h"
#include "my-atk-value.h"

//*************************implementation***********************
//implementation of virtual functions
//******************get_current_value*******************
static void my_atk_value_get_current_value(AtkValue *obj, GValue *value)
{
    g_return_if_fail(MY_IS_ATK_VALUE(obj));
    MyAtkValue* self = (MyAtkValue*)obj;
    
    g_value_init(value, G_TYPE_INT);
    g_value_set_int(value, self->current);
}
//******************get_maximum_value*******************
static void my_atk_value_get_maximum_value(AtkValue *obj, GValue *value)
{
    g_return_if_fail(MY_IS_ATK_VALUE(obj));
    MyAtkValue* self = (MyAtkValue*)obj;
    
    g_value_init(value, G_TYPE_INT);
    g_value_set_int(value, self->maximum);
}
//******************get_minimum_value*******************
static void my_atk_value_get_minimum_value(AtkValue *obj, GValue *value)
{
    g_return_if_fail(MY_IS_ATK_VALUE(obj));
    MyAtkValue* self = (MyAtkValue*)obj;
    
    g_value_init(value, G_TYPE_INT);
    g_value_set_int(value, self->minimum);
}
//******************set_current_value*******************
static gboolean my_atk_value_set_current_value(AtkValue *obj, const GValue *value)
{
    g_return_val_if_fail(MY_IS_ATK_VALUE(obj), FALSE);
    MyAtkValue* self = (MyAtkValue*)obj;
    
    if(self->readonly) return FALSE;
    
    gint new_value = g_value_get_int(value);
    if(new_value < self->minimum || new_value > self->maximum) return FALSE;
    
    self->current = new_value;
    return TRUE;
}

//others
MyAtkValue* my_atk_value_new(gint minimum, gint maximum, gint current)
{
    MyAtkValue* result = g_object_new(MY_TYPE_ATK_VALUE, NULL);
    if(result == NULL) return NULL;
    result->minimum = minimum;
    result->maximum = maximum;
    result->current = current;
    
    return result;
}
static void my_atk_value_instance_init(GTypeInstance *obj, gpointer g_class)
{
    MyAtkValue *self = (MyAtkValue*)obj;
    
    self->minimum = 0;
    self->maximum = 0;
    self->current = 0;
    
    self->readonly = FALSE;
}
static void my_atk_value_interface_init(gpointer g_iface, gpointer iface_data)
{
    AtkValueIface *klass = (AtkValueIface*)g_iface;
    
    klass->get_current_value = my_atk_value_get_current_value;
    klass->get_minimum_value = my_atk_value_get_minimum_value;
    klass->get_maximum_value = my_atk_value_get_maximum_value;
    
    klass->set_current_value = my_atk_value_set_current_value;
}

GType my_atk_value_get_type()
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo typeInfo = 
        {
            sizeof(MyAtkValueClass),
            NULL, //base_init
            NULL, //base_finalize
            NULL, //class_init
            NULL, //class_finalize
            NULL, //class_data
            sizeof(MyAtkValue),
            0, //n_preallocs
            my_atk_value_instance_init //instance_init
        };

        static const GInterfaceInfo iface_info = 
        {
            my_atk_value_interface_init,        /* interface_init*/
            NULL,                               /* interface_finalize*/
            NULL                                /* interface_data */
        };
        type = g_type_register_static(MY_TYPE_ATK_OBJECT, "MyAtkValue", &typeInfo, 0);
        g_type_add_interface_static(type,
            ATK_TYPE_VALUE,
            &iface_info);
    }
    return type;    
}
