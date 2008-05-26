#ifndef MY_ATK_IMPLEMENTOR_H
#define MY_ATK_IMPLEMENTOR_H

#include <atk/atk.h>

#define MY_TYPE_ATK_IMPLEMENTOR             (my_atk_implementor_get_type ())
#define MY_ATK_IMPLEMENTOR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_IMPLEMENTOR, MyAtkImplementor))
#define MY_ATK_IMPLEMENTOR_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_IMPLEMENTOR, MyAtkImplementorClass))
#define MY_IS_ATK_IMPLEMENTOR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_IMPLEMENTOR))
#define MY_IS_ATK_IMPLEMENTOR_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_IMPLEMENTOR))
#define MY_ATK_IMPLEMENTOR_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_IMPLEMENTOR, MyAtkImplementorClass))


typedef struct _MyAtkImplementor MyAtkImplementor;
typedef struct _MyAtkImplementorClass MyAtkImplementorClass;

struct _MyAtkImplementor
{
    GObject parent;
    AtkObject *accessible;
    gboolean is_disposed;
};
struct _MyAtkImplementorClass
{
    GObjectClass parent;
};

GType my_atk_implementor_get_type();
#endif /*MY_ATK_IMPLEMETOR_H*/
