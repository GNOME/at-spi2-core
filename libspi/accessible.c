/*
 * accessible.c: test for accessibility implementation
 *
 */
#include <config.h>
#include <bonobo/Bonobo.h>

#include <stdio.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::Accessible" server
 */
#include "Accessible.h"

/*
 * This pulls the definition for the BonoboObject (Gtk Type)
 */
#include "accessible.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE BONOBO_X_OBJECT_TYPE

/*
 * A pointer to our parent object class
 */
static GObjectClass *accessible_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
accessible_object_finalize (GObject *object)
{
        Accessible *accessible = ACCESSIBLE (object);

        printf("accessible_object_finalize called\n");
        g_free (accessible->atko);

        printf("atko freed, calling parent finalize\n");
        accessible_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::Accessible::get_name method implementation
 */
static CORBA_char *
impl_accessibility_accessible_get_name (PortableServer_Servant servant,
                                        CORBA_Environment     *ev)
{
  CORBA_char * retval;
  Accessible *accessible = ACCESSIBLE (bonobo_object_from_servant (servant));
  retval = CORBA_string_dup (atk_object_get_name (accessible->atko));
  fprintf (stderr, "Accessible get_name called: %s\n", retval);
  return retval;
}

/*
 * CORBA Accessibility::Accessible::set_name method implementation
 */
static void
impl_accessibility_accessible_set_name (PortableServer_Servant servant,
                                        const CORBA_char      *name,
                                        CORBA_Environment     *ev)
{
  Accessible *accessible = ACCESSIBLE (bonobo_object_from_servant (servant));
  atk_object_set_name (accessible->atko, name);
  printf ("Accessible set_name called: %s\n", name);
}

static void
accessible_class_init (AccessibleClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_Accessible__epv *epv = &klass->epv;
        accessible_parent_class = g_type_class_ref (BONOBO_X_OBJECT_TYPE);
        /*accessible_parent_class = g_type_class_peek_parent (klass);*/

        object_class->finalize = accessible_object_finalize;

        epv->_get_name = impl_accessibility_accessible_get_name;
        epv->_set_name = impl_accessibility_accessible_set_name;
}

static void
accessible_init (Accessible *accessible)
{
}

GType
accessible_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (AccessibleClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) accessible_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (Accessible),
                        0, /* n preallocs */
                        (GInstanceInitFunc) accessible_init,
                        NULL /* value table */
                };
                /*
                 *   Here we use bonobo_x_type_unique instead of
                 * gtk_type_unique, this auto-generates a load of
                 * CORBA structures for us. All derived types must
                 * use bonobo_x_type_unique.
                 */
                type = bonobo_x_type_unique (
                        PARENT_TYPE,
                        POA_Accessibility_Accessible__init,
                        NULL,
                        G_STRUCT_OFFSET (AccessibleClass, epv),
                        &tinfo,
                        "Accessible");
        }

        return type;
}

Accessible *
accessible_new (AtkObject *o)
{
    Accessible *retval =
               ACCESSIBLE (g_object_new (accessible_get_type (), NULL));
    retval->atko = ATK_OBJECT (o);
    return retval;
}
