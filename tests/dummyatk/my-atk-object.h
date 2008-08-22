#ifndef MY_ATK_OBJECT_H
#define MY_ATK_OBJECT_H

#include <atk/atk.h>

#define MY_TYPE_ATK_OBJECT             (my_atk_object_get_type ())
#define MY_ATK_OBJECT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_OBJECT, MyAtkObject))
#define MY_ATK_OBJECT_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_OBJECT, MyAtkObjectClass))
#define MY_IS_ATK_OBJECT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_OBJECT))
#define MY_IS_ATK_OBJECT_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_OBJECT))
#define MY_ATK_OBJECT_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_OBJECT, MyAtkObjectClass))

typedef struct _MyAtkObject MyAtkObject;
typedef struct _MyAtkObjectClass MyAtkObjectClass;

struct _MyAtkObject
{
    AtkObject parent;
    //array of children
    AtkStateSet *state_set;
    AtkAttributeSet *attributes;
    GPtrArray* children;
    gint id;
};

struct _MyAtkObjectClass
{
    AtkObjectClass parent;
};

GType my_atk_object_get_type();

void my_atk_object_add_child(MyAtkObject* parent, MyAtkObject* child);

void my_atk_object_remove_child(MyAtkObject* parent, MyAtkObject* child);

#endif /*MY_ATK_OBJECT_H*/
