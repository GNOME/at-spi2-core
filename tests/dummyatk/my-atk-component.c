#include <stdio.h>
#include <atk/atk.h>
#include <limits.h>

#include "my-atk-object.h"
#include "my-atk-component.h"

//*************************implementation***********************
static MyAtkObjectClass *component_parent_class = NULL;
//current focus object
static AtkComponent* focus_object = NULL;

static guint focus_signal_id = 0;
/*
 * Because of implementation of AtkUtils, we need to ensure that list of focus_trackers 
 * is not empty. Otherwise function atk_focus_tracker_notify will not change focus.
 */
static guint focus_tracker_id = 0;
static void my_event_listener(AtkObject* obj)
{
    //simply exist for register as focus_tracker
}
/*
 * If this flag is TRUE, then focus cannot be changed until someone clears the flag
 * via my_atk_component_set_modal(FALSE).
 */
static gboolean is_modal = FALSE;
//for debug
void print_extent(AtkRectangle *extent)
{
    printf("{%d,%d,%d,%d}", extent->x, extent->y, extent->width, extent->height);
}
//for internal use
static void emit_bounds_changed(MyAtkComponent *component)
{
    static guint bounds_changed_id = 0;
    if(bounds_changed_id == 0)
    {
        bounds_changed_id = g_signal_lookup("bounds-changed", ATK_TYPE_COMPONENT);
    }
    AtkRectangle *param = g_boxed_copy(ATK_TYPE_RECTANGLE, &(component->extent));
    g_signal_emit(component, bounds_changed_id, 0, param);
}
static void change_focus(AtkComponent* component, gboolean is_gain)
{
    const gchar* state_name = atk_state_type_get_name(ATK_STATE_FOCUSED);
    
    g_signal_emit_by_name(component, "focus-event", is_gain);
    g_signal_emit_by_name(component, "state-change::focused",
        state_name, is_gain);
    
    AtkObject* parent = atk_object_get_parent((AtkObject*)component);
    if(parent != NULL)
    {
        AtkStateSet* stateSet = atk_object_ref_state_set(parent);
        if(atk_state_set_contains_state(stateSet, ATK_STATE_MANAGES_DESCENDANTS))
            g_signal_emit_by_name(parent, "active-descendant-changed",
                atk_get_focus_object());
        g_object_unref(stateSet);
    }
}
//implementation of virtual functions
//******************ref_state_set(AtkObject)*****************************
static AtkStateSet* my_atk_component_ref_state_set(AtkObject *object)
{
    MyAtkComponent *self = (MyAtkComponent*)object;
    
    AtkStateSet* result = ((AtkObjectClass*)component_parent_class)->
        ref_state_set(object);
    if(self->is_manage_descendants) 
        atk_state_set_add_state(result, ATK_STATE_MANAGES_DESCENDANTS);
    return result;
}
//******************get_size*******************
static void my_atk_component_get_size(AtkComponent *component, gint *width, gint *height)
{
    g_return_if_fail(MY_IS_ATK_COMPONENT(component));
    
    MyAtkComponent *self = MY_ATK_COMPONENT(component);
    *width = self->extent.width;
    *height = self->extent.height;
}
//*********************get_position*******************
static void my_atk_component_get_position(AtkComponent *component, gint *x, gint *y, AtkCoordType coord_type)
{
    g_return_if_fail(MY_IS_ATK_COMPONENT(component));
    
    MyAtkComponent *self = MY_ATK_COMPONENT(component);
    *x = self->extent.x;
    *y = self->extent.y;
    
//**********************get_extents*******************
}
static void my_atk_component_get_extents(AtkComponent *component, gint *x, gint *y,
    gint *width, gint *height, AtkCoordType coord_type)
{
    g_return_if_fail(MY_IS_ATK_COMPONENT(component));
    
    MyAtkComponent *self = MY_ATK_COMPONENT(component);
    *x = self->extent.x;
    *y = self->extent.y;
    *width = self->extent.width;
    *height = self->extent.height;
}

//**************************set_size*******************
static gboolean my_atk_component_set_size(AtkComponent *component, gint width, gint height)
{
    g_return_val_if_fail(MY_IS_ATK_COMPONENT(component), FALSE);
    
    MyAtkComponent *self = MY_ATK_COMPONENT(component);
    if(self->is_extent_may_changed)
    {
        self->extent.width = width;
        self->extent.height = height;
    
        emit_bounds_changed(self);
        
        return TRUE;
    }
    return FALSE;
}
//**************************set_position********************
static gboolean my_atk_component_set_position(AtkComponent *component,
    gint x, gint y, AtkCoordType coord_type)
{
    g_return_val_if_fail(MY_IS_ATK_COMPONENT(component), FALSE);
    
    MyAtkComponent *self = MY_ATK_COMPONENT(component);
    if(self->is_extent_may_changed)
    {
        self->extent.x = x;
        self->extent.y = y;
        
        emit_bounds_changed(self);

        return TRUE;
    }
    return FALSE;
}
//*************************************set_extents***************
static gboolean my_atk_component_set_extents(AtkComponent *component,
    gint x, gint y, gint width, gint height, AtkCoordType coord_type)
{
    g_return_val_if_fail(MY_IS_ATK_COMPONENT(component), FALSE);
    
    MyAtkComponent *self = MY_ATK_COMPONENT(component);
    
    if(self->is_extent_may_changed)
    {
        self->extent.x = x;
        self->extent.y = y;
        self->extent.width = width;
        self->extent.height = height;
        
        emit_bounds_changed(self);
        
        return TRUE;
    }
    return FALSE;
}
//**************************get_layer****************
static AtkLayer my_atk_component_get_layer(AtkComponent *component)
{
    g_return_val_if_fail(MY_IS_ATK_COMPONENT(component), ATK_LAYER_INVALID);
    
    MyAtkComponent *self = MY_ATK_COMPONENT(component);
    return self->layer;
}
//**************************get_mdi_zorder****************
static gint my_atk_component_get_mdi_zorder(AtkComponent *component)
{
    g_return_val_if_fail(MY_IS_ATK_COMPONENT(component), G_MININT);
    
    MyAtkComponent *self = MY_ATK_COMPONENT(component);
    return self->zorder;
}
//***********************contains**********************
static gboolean my_atk_component_contains(AtkComponent *component,
    gint x, gint y, AtkCoordType coord_type)
{
    g_return_val_if_fail(MY_IS_ATK_COMPONENT(component), FALSE);
    //for extract extent
    gint x_tmp, y_tmp, width_tmp, height_tmp;
    my_atk_component_get_extents(component, &x_tmp, &y_tmp, &width_tmp, &height_tmp, coord_type);
    
    if( (x >= x_tmp) &&(y >= y_tmp) &&(x < x_tmp + width_tmp) && (y < y_tmp + height_tmp) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
//**********************ref_accessible_at_point***********************
/*
 * Retuns accessible child that implements AtkCOmponent and contains the given point.
 */
static AtkObject* my_atk_component_ref_accessible_at_point(AtkComponent* component,
    gint x, gint y, AtkCoordType coord_type)
{
    g_return_val_if_fail(MY_IS_ATK_COMPONENT(component), NULL);
    gint i;
    
    gint n_children = atk_object_get_n_accessible_children((AtkObject*)component);
    for(i = 0; i < n_children; i++)
    {
        AtkObject *child = atk_object_ref_accessible_child((AtkObject*)component, i);
        if(ATK_IS_COMPONENT(child)
            && atk_component_contains((AtkComponent*)child, x, y, coord_type))
        {
            return child;
        }
        g_object_unref(child);
    }
    return NULL;
}
//*************************************grab_focus*********************************
static gboolean my_atk_component_grab_focus(AtkComponent* component)
{
    if(component == focus_object)
    { 
        //Already has focus
        return TRUE;
    }
    if(is_modal)
    {
        //cannot grab focus
        return FALSE;
    }
    AtkComponent *focus_object_old = focus_object;
    focus_object = component;
    
    atk_focus_tracker_notify((AtkObject*)component);
    
    if(focus_object_old != NULL)
    {
        //signals for object which lost focus
        change_focus(focus_object_old, FALSE);
    }
    if(component != NULL)
    {
        //signals for object which grab focus
        change_focus(component, TRUE);
    }
    return TRUE;
}
//***********************my_atk_component_add_focus_handler*********************
static guint my_atk_component_add_focus_handler(AtkComponent *component, AtkFocusHandler handler)
{
    g_return_val_if_fail(MY_IS_ATK_COMPONENT(component),0);
    //verify whether handler already connect to object
    gulong found_handler_id = g_signal_handler_find(component,
        G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_FUNC,
        focus_signal_id,
        0,
        NULL,
        (gpointer)handler,
        NULL);
    if(found_handler_id == 0)
    {
        //handler hasn't been connected yet
        return g_signal_connect_closure_by_id(component,
            focus_signal_id,
            0,
            g_cclosure_new( (GCallback)handler,
                NULL,
                NULL),
            FALSE);
    }
    else/* found_handler_id != 0*/
    {
        //handler has already been connected
        return 0;
    }

}
//***********************my_atk_component_remove_focus_handler*********************
static void my_atk_component_remove_focus_handler(AtkComponent *component, guint handler_id)
{
    g_signal_handler_disconnect(component, handler_id);
}
//***********************my_atk_component_set_modal(my function)***************
void my_atk_component_set_modal(gboolean value)
{
    is_modal = value;
}
//******************my_atk_component_set_manage_descendants(my_function)*******
void my_atk_component_set_manage_descendants(MyAtkComponent* component, gboolean value)
{
    if(component->is_manage_descendants == value)return;
    component->is_manage_descendants = value;
    g_signal_emit_by_name(component, "state-change::manages-descendants",
        "manages-descendants", value);
}
//Others funtions
static void my_atk_component_instance_init(GTypeInstance *obj, gpointer g_class)
{
    MyAtkComponent *self = (MyAtkComponent*)obj;
    //set defaults values
    self->extent.x = 0;
    self->extent.y = 0;
    self->extent.width = 10;
    self->extent.height = 10;
    self->is_extent_may_changed = TRUE;
    self->layer = ATK_LAYER_INVALID;
    self->zorder = -2147;
}
static void my_atk_component_instance_finalize(GObject* obj)
{
    MyAtkComponent* component = (MyAtkComponent*)obj;

    if(((AtkObject*)component) == atk_get_focus_object())
    {
        atk_focus_tracker_notify(NULL);
    }
}

static void my_atk_component_class_init(gpointer g_class, gpointer class_data)
{
    GObjectClass* g_object_class = (GObjectClass*)g_class;
    AtkObjectClass* atkObject_class = (AtkObjectClass*)g_class;
    //GObject virtual table
    g_object_class->finalize = my_atk_component_instance_finalize;
    //AtkObject virtual table
    atkObject_class->ref_state_set = my_atk_component_ref_state_set;
    //parent_class
    component_parent_class = g_type_class_peek_parent(g_class);
    //make focus_tracker's table not empty.
    focus_tracker_id = atk_add_focus_tracker(my_event_listener);
    //store "focus-event"-signal id
    focus_signal_id = g_signal_lookup("focus-event",MY_TYPE_ATK_COMPONENT);
}
/*
 * Though, according to the documentation, this function will never called for 
 * static-registred types.
 * Base_init function doesn't suite for this work,
 * because it will called in every derived classes.
 */
/*static void my_atk_component_class_finalize(gpointer g_class, gpointer class_data)
{
    
    if(focus_tracker_id != 0)
    {
        atk_remove_focus_tracker(focus_tracker_id);
        focus_tracker_id = 0;
    }
}*/
static void my_atk_component_interface_init(gpointer g_iface, gpointer iface_data)
{
    AtkComponentIface *klass = (AtkComponentIface*)g_iface;
    
    klass->get_extents = my_atk_component_get_extents;
    klass->get_position = my_atk_component_get_position;
    klass->get_size = my_atk_component_get_size;
    
    klass->set_extents = my_atk_component_set_extents;
    klass->set_position = my_atk_component_set_position;
    klass->set_size = my_atk_component_set_size;
    
    klass->contains = my_atk_component_contains;
    klass->ref_accessible_at_point = my_atk_component_ref_accessible_at_point;
    
    klass->get_layer = my_atk_component_get_layer;
    klass->get_mdi_zorder = my_atk_component_get_mdi_zorder;
    
    klass->grab_focus = my_atk_component_grab_focus;
    klass->add_focus_handler = my_atk_component_add_focus_handler;
    klass->remove_focus_handler = my_atk_component_remove_focus_handler;
}

GType my_atk_component_get_type()
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo typeInfo = 
        {
            sizeof(MyAtkComponentClass),
            NULL, //base_init
            NULL, //base_finalize
            my_atk_component_class_init, //class_init
            NULL, //class_finalize
            NULL, //class_data
            sizeof(MyAtkComponent),
            0, //n_preallocs
            my_atk_component_instance_init //instance_init
        };

        static const GInterfaceInfo iface_info = 
        {
            my_atk_component_interface_init,    /* interface_init*/
            NULL,                               /* interface_finalize*/
            NULL                                /* interface_data */
        };
        type = g_type_register_static(MY_TYPE_ATK_OBJECT, "MyAtkComponent", &typeInfo, 0);
        g_type_add_interface_static(type,
            ATK_TYPE_COMPONENT,
            &iface_info);
    }
    return type;    
}
