#include "atksimpleobject.h"

static void atk_simple_object_class_init (AtkSimpleObjectClass *klass);

static gpointer parent_class = NULL;

GType
atk_simple_object_get_type (void)
{
  static GType type = 0;

  if (!type)
  {
    static const GTypeInfo tinfo =
    {
      sizeof (AtkSimpleObjectClass),
      (GBaseInitFunc) NULL, /* base init */
      (GBaseFinalizeFunc) NULL, /* base finalize */
      (GClassInitFunc) atk_simple_object_class_init, /* class init */
      (GClassFinalizeFunc) NULL, /* class finalize */
      NULL, /* class data */
      sizeof (AtkSimpleObject), /* instance size */
      0, /* nb preallocs */
      (GInstanceInitFunc) NULL, /* instance init */
      NULL /* value table */
    };

    type = g_type_register_static (ATK_TYPE_OBJECT,
                                    "AtkSimpleObject", &tinfo, 0);
  }
  return type;
}

static void
atk_simple_object_set_name (AtkObject *o, const gchar *name)
{
  printf("set name to %s\n", name);
  o->name = name;
}

static G_CONST_RETURN gchar *
atk_simple_object_get_name (AtkObject *o)
{
  printf("get name: %s\n", o->name);
  return (o->name);
}


static void
atk_simple_object_set_description (AtkObject *o, const gchar *desc)
{
  printf("set description to %s\n", desc);
  o->description = desc;
}

static G_CONST_RETURN gchar *
atk_simple_object_get_description (AtkObject *o)
{
  printf("get description: %s\n", o->description);
  return (o->description);
}

static void
atk_simple_object_class_init (AtkSimpleObjectClass *klass)
{
  AtkObjectClass *oc = ATK_OBJECT_CLASS (klass);
  parent_class = g_type_class_ref (ATK_TYPE_OBJECT);
  oc->set_name = atk_simple_object_set_name;
  oc->get_name = atk_simple_object_get_name;
  oc->set_description = atk_simple_object_set_description;
  oc->get_description = atk_simple_object_get_description;
}

AtkObject*
atk_simple_object_new ()
{
  GObject *object;
  AtkObject* accessible;

  object = g_object_new (ATK_TYPE_SIMPLE_OBJECT, NULL);
  accessible = ATK_OBJECT (object);

  return accessible;
}

