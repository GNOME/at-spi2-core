
#include <stdio.h>
#include <atk/atk.h>

#include "my-atk-object.h"

static AtkObjectClass *atk_object_parent_class = NULL;

void my_atk_object_add_child(MyAtkObject* parent, MyAtkObject* child)
{
    g_ptr_array_add(parent->children, child);
    g_object_ref_sink(child);

    atk_object_set_parent(ATK_OBJECT(child), ATK_OBJECT(parent));
    
    g_signal_emit_by_name(parent, "children-changed::add",
        parent->children->len - 1, child);
}

void my_atk_object_remove_child(MyAtkObject* parent, MyAtkObject* child)
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

static void my_atk_object_set_parent(AtkObject *accessible, AtkObject *parent)
{
    g_return_if_fail(parent != NULL);

    MyAtkObject *self = MY_ATK_OBJECT(accessible);
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
        my_atk_object_remove_child((MyAtkObject*)parent_old, self);
    }
}

static gint my_atk_object_get_n_children(AtkObject *accessible)
{
    return MY_ATK_OBJECT(accessible)->children->len;
}

static AtkObject* my_atk_object_ref_child(AtkObject *accessible, gint i)
{
    MyAtkObject *self = MY_ATK_OBJECT(accessible); 
    if(i < 0 || i >= self->children->len)
    {
        printf("ref_child: Incorrect index of child.\n");
        return NULL;
    }

    AtkObject* child = (AtkObject*)
        g_ptr_array_index(self->children, i);

    return (child == NULL) ? NULL : g_object_ref(child);
}

static gint my_atk_object_get_index_in_parent(AtkObject *accessible)
{
    AtkObject *parent = atk_object_get_parent(accessible);
    if(parent == NULL) return -1;//no parent
    
    MyAtkObject *parent_my = MY_ATK_OBJECT(parent);

    int i = parent_my->children->len;
    for(; i>=0; i--)
    {
        if(g_ptr_array_index(parent_my->children,i) == accessible)
            break;
    }
    if(i < 0)printf("Something wrong in parent-child strucutre.\n");
    return i;//if error, i will be equal to -1
}

static AtkStateSet *my_atk_object_ref_state_set(AtkObject *accessible)
{
    MyAtkObject *obj = MY_ATK_OBJECT(accessible);

    if (obj->state_set == NULL)
        obj->state_set = atk_state_set_new();
    return g_object_ref(G_OBJECT(obj->state_set));
}

static AtkAttributeSet *my_atk_object_get_attributes (AtkObject *accessible)
{
    MyAtkObject *obj = MY_ATK_OBJECT(accessible);
    AtkAttributeSet *rs = obj->attributes = NULL;
    AtkAttribute *a, *b, *c;

    a = g_new(AtkAttribute, 1);
    b = g_new(AtkAttribute, 1);
    c = g_new(AtkAttribute, 1);

    a->name = g_strdup("foo");
    a->value = g_strdup("bar");
    b->name = g_strdup("baz");
    b->value = g_strdup("qux");
    c->name = g_strdup("quux");
    c->value = g_strdup("corge");

    rs = g_slist_append(rs, (gpointer) a); 
    rs = g_slist_append(rs, (gpointer) b); 
    rs = g_slist_append(rs, (gpointer) c); 
}

//function, needed in instance_finalize()
static void my_unref1(gpointer data, gpointer user_data)
{
    g_object_unref(data);
}

static void my_atk_object_instance_finalize(GObject *obj)
{
    MyAtkObject *self = (MyAtkObject*) obj;
    //unrefs all children
    g_ptr_array_foreach(self->children, my_unref1, NULL);
    //then free array (without frees pointers)
    g_ptr_array_free(self->children, FALSE);
    //chain to parent class
    G_OBJECT_CLASS(atk_object_parent_class)->finalize(obj);
}

void my_atk_object_class_init(gpointer g_class, gpointer g_class_data)
{
    AtkObjectClass *atkObjectClass = (AtkObjectClass*)g_class;
    
    ((GObjectClass*)g_class)->finalize = my_atk_object_instance_finalize;
    //set pointers to new functions in table of virtuals functions
    atkObjectClass->set_parent = my_atk_object_set_parent;
    atkObjectClass->get_n_children = my_atk_object_get_n_children;
    atkObjectClass->ref_child = my_atk_object_ref_child;
    atkObjectClass->get_index_in_parent = my_atk_object_get_index_in_parent;
    atkObjectClass->ref_state_set = my_atk_object_ref_state_set;
    atkObjectClass->get_attributes = my_atk_object_get_attributes;

    atk_object_parent_class = g_type_class_peek_parent(g_class);
}

static void my_atk_object_instance_init(GTypeInstance *obj, gpointer g_class)
{
    MyAtkObject *self = (MyAtkObject*)obj;

    self->children = g_ptr_array_sized_new(10);
    self->attributes =  g_slist_alloc();
}

GType my_atk_object_get_type()
{
    static GType type = 0;
    if(type == 0)
    {
        static const GTypeInfo typeInfo = 
        {
            sizeof(MyAtkObjectClass),
            NULL, //base_init
            NULL, //base_finalize
            my_atk_object_class_init, //class_init
            NULL, //class_finalize
            NULL, //class_data
            sizeof(MyAtkObject),
            0, //n_preallocs
            my_atk_object_instance_init //instance_init
        };
        type = g_type_register_static(ATK_TYPE_OBJECT,"MyAtkObject",&typeInfo,0);
    }
    return type;
}
