#ifndef MY_ATK_COMPONENT_H
#define MY_ATK_COMPONENT_H
/*
 * MyAtkComponent: derives AtkObject(with parent-child accessibilities)
 * and implements AtkComponent.
 */
#include <atk/atk.h>

#include "my-atk-object.h"

#define MY_TYPE_ATK_COMPONENT             (my_atk_component_get_type ())
#define MY_ATK_COMPONENT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ATK_COMPONENT, MyAtkComponent))
#define MY_ATK_COMPONENT_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), MY_TYPE_ATK_COMPONENT, MyAtkComponentClass))
#define MY_IS_ATK_COMPONENT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ATK_COMPONENT))
#define MY_IS_ATK_COMPONENT_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), MY_TYPE_ATK_COMPONENT))
#define MY_ATK_COMPONENT_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), MY_TYPE_ATK_COMPONENT, MyAtkComponentClass))

typedef struct _MyAtkComponent MyAtkComponent;
typedef struct _MyAtkComponentClass MyAtkComponentClass;

struct _MyAtkComponent
{
    MyAtkObject parent;
    //relative coordinates, which coincides with absolute ones
    AtkRectangle extent;
    //whether component may be relocated
    gboolean is_extent_may_changed;
    //for emit "active-descendant-changed" signal
    gboolean is_manage_descendants;
    //
    AtkLayer layer;
    gint zorder;
};

struct _MyAtkComponentClass
{
    MyAtkObjectClass parent;
};

GType my_atk_component_get_type();
#endif /*MY_ATK_COMPONENT_H*/
