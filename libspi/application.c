/*
 * application.c: implements Application.idl
 *
 */
#include <config.h>
#include <bonobo/Bonobo.h>
#include "atksimpleobject.h"

/*
 * This pulls the CORBA definitions for the "Accessibility::Accessible" server
 */
#include "Desktop.h"

/*
 * This pulls the definition for the BonoboObject (GObject Type)
 */
#include "application.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE ACCESSIBLE_TYPE

static void
application_class_init (ApplicationClass *klass)
{
  ;
}

static void
application_init (Application  *application)
{
  ACCESSIBLE (application)->atko = atk_simple_object_new();
}

GType
application_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (ApplicationClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) application_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (Application),
                        0, /* n preallocs */
                        (GInstanceInitFunc) application_init,
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
                        POA_Accessibility_Application__init,
                        NULL,
                        G_STRUCT_OFFSET (ApplicationClass, epv),
                        &tinfo,
                        "Application");
        }

        return type;
}

Application *
application_new (char *name, char *desc, char *id)
{
    Application *retval =
               APPLICATION (g_object_new (application_get_type (), NULL));
    atk_object_set_name (ACCESSIBLE (retval)->atko, CORBA_string_dup (name));
    atk_object_set_description (ACCESSIBLE (retval)->atko, CORBA_string_dup (desc));
    retval->id = CORBA_string_dup (id);
    return retval;
}
