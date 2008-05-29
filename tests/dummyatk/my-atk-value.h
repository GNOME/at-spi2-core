#ifndef MY_ATK_VALUE_H
#define MY_ATK_VALUE_H
/*
 * MyAtkValue: derives AtkObject
 * and implements AtkValue
 */
#include <atk/atk.h>

#define MY_TYPE_ATK_VALUE             (my_atk_value_get_type ())
#define MY_ATK_VALUE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_VALUE, MyAtkValue))
#define MY_ATK_VALUE_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_VALUE, MyAtkValueClass))
#define MY_IS_ATK_VALUE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_VALUE))
#define MY_IS_ATK_VALUE_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_VALUE))
#define MY_ATK_VALUE_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_VALUE, MyAtkValueClass))

typedef struct _MyAtkValue MyAtkValue;
typedef struct _MyAtkValueClass MyAtkValueClass;

struct _MyAtkValue
{
    MyAtkObject parent;
    
    gint minimum, maximum, current;
    gboolean readonly;
};

struct _MyAtkValueClass
{
    MyAtkObjectClass parent;
};

MyAtkValue* my_atk_value_new(gint minimum, gint maximium, gint current);
GType my_atk_value_get_type();
#endif /*MY_ATK_VALUE_H*/
