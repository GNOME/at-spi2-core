#include "AtkObject_parent_child.h"

//implementation
static AtkObjectClass *atk_object_parent_class = NULL;
//redefines 'set_parent' function, and defines functions, which absents in AtkObject

static void my_parent_child_set_parent(AtkObject *accessible, AtkObject *parent);

static gint my_parent_child_get_n_children(AtkObject *accessible);

static AtkObject* my_parent_child_ref_child(AtkObject *accessible, gint i);

static gint my_parent_child_get_index_in_parent(AtkObject *accessible);

//function, needed in instance_finalize()
static void my_unref1(gpointer data, gpointer user_data)
{
    g_object_unref(data);
}
//add/remove child to/from array of parent(for internal use)
static void my_parent_child_add_child(MyParentChild* parent, MyParentChild* child)
{
    g_ptr_array_add(parent->children, child);
    g_object_ref(child);
    
    g_signal_emit_by_name(parent, "children-changed::add",
        parent->children->len - 1, child);
}
static void my_parent_child_remove_child(MyParentChild* parent, MyParentChild* child)
{
    gint i;
    for(i = parent->children->len - 1; i >= 0; i--)
    {
        if(g_ptr_array_index(parent->children, i) == child) break;
    }
    if(i < 0)return;

    g_ptr_array_remove_index(parent->children, i);
    g_object_unref(child);
    g_signal_emit_by_name(parent, "children-changed::remove",
        i, child);
}

static void my_parent_child_instance_init(GTypeInstance *obj, gpointer g_class)
{
    MyParentChild *self = (MyParentChild*)obj;

    self->children = g_ptr_array_sized_new(10);
}

static void my_parent_child_instance_finalize(GObject *obj)
{
    MyParentChild *self = (MyParentChild*) obj;
    //unrefs all children
    g_ptr_array_foreach(self->children, my_unref1, NULL);
    //then free array (without frees pointers)
    g_ptr_array_free(self->children, FALSE);
    //chain to parent class
    G_OBJECT_CLASS(atk_object_parent_class)->finalize(obj);
}

void my_parent_child_class_init(gpointer g_class, gpointer g_class_data)
{
    AtkObjectClass *atkObjectClass = (AtkObjectClass*)g_class;
    
    ((GObjectClass*)g_class)->finalize = my_parent_child_instance_finalize;
    //set pointers to new functions in table of virtuals functions
    atkObjectClass->set_parent = my_parent_child_set_parent;
    atkObjectClass->get_n_children = my_parent_child_get_n_children;
    atkObjectClass->ref_child = my_parent_child_ref_child;
    atkObjectClass->get_index_in_parent = my_parent_child_get_index_in_parent;

    atk_object_parent_class = g_type_class_peek_parent(g_class);
}

static void my_parent_child_set_parent(AtkObject *accessible, AtkObject *parent)
{
    //applicable only with corresponding type of parent
    g_return_if_fail(parent == NULL || MY_IS_PARENT_CHILD(parent));

    MyParentChild *self = MY_PARENT_CHILD(accessible);
    AtkObject *parent_old = (atk_object_get_parent(accessible));
    
    if(parent_old == parent)
    {
        //nothing to do because parent does not change
        return;
    }
    
    //set field 'parent' in child using 'base-method'
    atk_object_parent_class->set_parent(accessible, parent);
    
    if(parent_old != NULL)
    {
        my_parent_child_remove_child((MyParentChild*)parent_old, self);
    }
    if(parent != NULL)
    {
        my_parent_child_add_child((MyParentChild*)parent, self);
    }
}

static gint my_parent_child_get_n_children(AtkObject *accessible)
{
    return MY_PARENT_CHILD(accessible)->children->len;
}

static AtkObject* my_parent_child_ref_child(AtkObject *accessible, gint i)
{
    MyParentChild *self = MY_PARENT_CHILD(accessible); 
    if(i < 0 || i >= self->children->len)
    {
        printf("ref_child: Incorrect index of child.\n");
        return NULL;
    }

    AtkObject* child = (AtkObject*)
        g_ptr_array_index(self->children, i);

    return (child == NULL) ? NULL : g_object_ref(child);
}

static gint my_parent_child_get_index_in_parent(AtkObject *accessible)
{
    AtkObject *parent = atk_object_get_parent(accessible);
    if(parent == NULL) return -1;//no parent
    
    MyParentChild *parent_my = MY_PARENT_CHILD(parent);

    int i = parent_my->children->len;
    for(; i>=0; i--)
    {
        if(g_ptr_array_index(parent_my->children,i) == accessible)
            break;
    }
    if(i < 0)printf("Something wrong in parent-child strucutre.\n");
    return i;//if error, i will be equal to -1
}

GType my_parent_child_get_type()
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo typeInfo = 
        {
            sizeof(MyParentChildClass),
            NULL, //base_init
            NULL, //base_finalize
            my_parent_child_class_init, //class_init
            NULL, //class_finalize
            NULL, //class_data
            sizeof(MyParentChild),
            0, //n_preallocs
            my_parent_child_instance_init //instance_init
        };
        type = g_type_register_static(ATK_TYPE_OBJECT,"MyParentChild",&typeInfo,0);
    }
    return type;
}
