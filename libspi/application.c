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

Accessibility_EventListener the_toolkit_listener;

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

#define APP_STATIC_BUFF_SZ 64

static gboolean
application_toolkit_listener (GSignalInvocationHint *signal_hint,
                              guint n_param_values,
                              const GValue *param_values,
                              gpointer data)
{
  Accessibility_Event *e = g_new0(Accessibility_Event, 1);
  AtkObject *aobject;
  GObject *gobject;
  CORBA_Environment ev;
  GSignalQuery signal_query;
  gchar *name;
  char sbuf[APP_STATIC_BUFF_SZ];

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;
  fprintf (stderr, "Received signal %s:%s\n", g_type_name (signal_query.itype), name);

  /* TODO: move GTK dependency out of app.c into bridge */
  snprintf(sbuf, APP_STATIC_BUFF_SZ, "Gtk:%s:%s", g_type_name (signal_query.itype), name);


  gobject = g_value_get_object (param_values + 0);
  /* notify the actual listeners */
  if (ATK_IS_IMPLEMENTOR (gobject))
    {
      aobject = atk_implementor_ref_accessible (ATK_IMPLEMENTOR (gobject));
      e->type = CORBA_string_dup (sbuf);
      e->target = bonobo_object_corba_objref (bonobo_object (accessible_new (aobject)));
      e->detail1 = 0;
      e->detail2 = 0;
      Accessibility_EventListener_notifyEvent (the_toolkit_listener, e, &ev);
      g_object_unref (aobject);
    }
  return TRUE;
}

static void
impl_accessibility_application_register_toolkit_event_listener (PortableServer_Servant servant,
                                                                Accessibility_EventListener listener,
                                                                const CORBA_char *event_name,
                                                                CORBA_Environment *ev)
{
  guint listener_id;
  listener_id =
     atk_add_global_event_listener (application_toolkit_listener, event_name);
  the_toolkit_listener = CORBA_Object_duplicate (listener, ev);
#ifdef SPI_DEBUG
  fprintf (stderr, "registered %d for toolkit events named: %s\n",
           listener_id,
           event_name);
#endif
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
  epv->registerToolkitEventListener = impl_accessibility_application_register_toolkit_event_listener;
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
