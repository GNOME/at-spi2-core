/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * application.c: implements Application.idl
 *
 */
#include <config.h>
#include <bonobo/Bonobo.h>
#include <atk/atkutil.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::Accessible" server
 */
#include <libspi/Accessibility.h>

/*
 * This pulls the definition for the BonoboObject (GObject Type)
 */
#include "application.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE ACCESSIBLE_TYPE

/*
 * A pointer to our parent object class
 */
static AccessibleClass *application_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
accessible_application_finalize (GObject *object)
{
  /* TODO: any necessary cleanup */
  Accessible *accessible = ACCESSIBLE (object);

  g_object_unref (accessible->atko);
  accessible->atko = NULL;

  /* TODO: chain to parent class instead */
}

static CORBA_string
impl_accessibility_application_get_toolkit_name (PortableServer_Servant servant,
                                                 CORBA_Environment *ev)
{
  CORBA_char *retval;
  Application *application = APPLICATION (bonobo_object_from_servant (servant));
  retval = CORBA_string_dup (atk_get_toolkit_name ());
  return retval;
}

static CORBA_string
impl_accessibility_application_get_version (PortableServer_Servant servant,
                                            CORBA_Environment *ev)
{
  CORBA_char *retval;
  Application *application = APPLICATION (bonobo_object_from_servant (servant));
  retval = CORBA_string_dup (atk_get_toolkit_version ());
  return retval;
}

static CORBA_string
impl_accessibility_application_get_id (PortableServer_Servant servant,
                                                 CORBA_Environment *ev)
{
  CORBA_char *retval;
  Application *application = APPLICATION (bonobo_object_from_servant (servant));
  retval = CORBA_string_dup (application->id);
  return retval;
}

static void
impl_accessibility_application_set_id (PortableServer_Servant servant,
                                       const CORBA_char *id,
                                       CORBA_Environment *ev)
{
  Application *application = APPLICATION (bonobo_object_from_servant (servant));
  application->id = id;
}

static void
application_class_init (ApplicationClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Application__epv *epv = &klass->epv;

  application_parent_class = g_type_class_ref (ACCESSIBLE_TYPE);

  object_class->finalize = accessible_application_finalize;

  epv->_get_toolkitName = impl_accessibility_application_get_toolkit_name;
  epv->_get_version = impl_accessibility_application_get_version;
  epv->_get_id = impl_accessibility_application_get_id;
  epv->_set_id = impl_accessibility_application_set_id;
}

static void
application_init (Application  *application)
{
  ACCESSIBLE (application)->atko = g_object_new (atk_object_get_type(), NULL);
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
                 * Bonobo_type_unique auto-generates a load of
                 * CORBA structures for us. All derived types must
                 * use bonobo_type_unique.
                 */
                type = bonobo_type_unique (
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
application_new (AtkObject *app_root)
{
    Application *retval =
               APPLICATION (g_object_new (application_get_type (), NULL));
    ACCESSIBLE (retval)->atko = app_root;
    return retval;
}
