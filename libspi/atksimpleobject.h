#ifndef __ATK_SIMPLE_OBJECT_H__
#define __ATK_SIMPLE_OBJECT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <glib-object.h>
#include <atk/atkobject.h>

#define ATK_TYPE_SIMPLE_OBJECT                (atk_simple_object_get_type ())
#define ATK_SIMPLE_OBJECT(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATK_TYPE_SIMPLE_OBJECT, AtkSimpleObject))
#define ATK_SIMPLE_OBJECT_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), ATK_TYPE_SIMPLE_OBJECT, AtkSimpleObjectClass))
#define ATK_IS_SIMPLE_OBJECT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATK_TYPE_SIMPLE_OBJECT))
#define ATK_IS_SIMPLE_OBJECT_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), ATK_TYPE_SIMPLE_OBJECT))
#define ATK_SIMPLE_OBJECT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), ATK_TYPE_SIMPLE_OBJECT, AtkSimpleObjectClass))

typedef struct _AtkSimpleObject                   AtkSimpleObject;
typedef struct _AtkSimpleObjectClass              AtkSimpleObjectClass;

struct _AtkSimpleObject
{
  AtkObject     parent;
};

GType atk_simple_object_get_type (void);

struct _AtkSimpleObjectClass
{
  AtkObjectClass parent_class;
};

AtkObject *atk_simple_object_new ();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ATK_SIMPLE_OBJECT_H__ */

