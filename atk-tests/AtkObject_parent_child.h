#ifndef ATK_OBJECT_PARENT_CHILD_H
#define ATK_OBJECT_PARENT_CHILD_H

#include "atk/atk.h"

//parent_child
#define MY_TYPE_PARENT_CHILD             (my_parent_child_get_type ())
#define MY_PARENT_CHILD(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_PARENT_CHILD, MyParentChild))
#define MY_PARENT_CHILD_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_PARENT_CHILD, MyParentChildClass))
#define MY_IS_PARENT_CHILD(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_PARENT_CHILD))
#define MY_IS_PARENT_CHILD_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_PARENT_CHILD))
#define MY_PARENT_CHILD_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_PARENT_CHILD, MyParentChildClass))

typedef struct _MyParentChild MyParentChild;
typedef struct _MyParentChildClass MyParentChildClass;

struct _MyParentChild
{
    AtkObject parent;
    //array of children
    GPtrArray* children;
};

struct _MyParentChildClass
{
    AtkObjectClass parent;
};

GType my_parent_child_get_type();

#endif /*ATK_OBJECT_PARENT_CHILD_H*/
