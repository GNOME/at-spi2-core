/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

/* application.c: implements SpiApplication.idl */

#include <string.h>
#include <config.h>
#include <glib.h>
#include <atk/atkutil.h>
#include <libspi/application.h>
#include <locale.h>
#include "spi-private.h"

/* Our parent Gtk object type */
#define PARENT_TYPE SPI_ACCESSIBLE_TYPE

/* A pointer to our parent object class */
static SpiAccessibleClass *spi_application_parent_class;

static SpiApplication *the_app;

/* static methods */

static void notify_listeners (GList *listeners,
			      SpiAccessible *source,
			      Accessibility_Event *e);

/*
 * Implemented GObject::finalize
 */
static void
spi_accessible_application_finalize (GObject *object)
{
  GList *l;
  SpiApplication *application = (SpiApplication *) object;
  CORBA_Environment ev;

  CORBA_exception_init (&ev);

  for (l = application->toolkit_listeners; l; l = l->next)
    {
      CORBA_Object_release ((CORBA_Object) l->data, &ev);
    }

  CORBA_exception_free (&ev);

  g_list_free (application->toolkit_listeners);
  application->toolkit_listeners = NULL;

  (G_OBJECT_CLASS (spi_application_parent_class))->finalize (object);
}

static CORBA_string
impl_accessibility_application_get_toolkit_name (PortableServer_Servant servant,
                                                 CORBA_Environment     *ev)
{
  return CORBA_string_dup (atk_get_toolkit_name ());
}

static CORBA_string
impl_accessibility_application_get_version (PortableServer_Servant servant,
                                            CORBA_Environment     *ev)
{
  return CORBA_string_dup (atk_get_toolkit_version ());
}

static CORBA_long
impl_accessibility_application_get_id (PortableServer_Servant servant,
                                       CORBA_Environment     *ev)
{
  SpiApplication *application = SPI_APPLICATION (
    bonobo_object_from_servant (servant));

  return application->id;
}

static void
impl_accessibility_application_set_id (PortableServer_Servant servant,
                                       const CORBA_long id,
                                       CORBA_Environment *ev)
{
  SpiApplication *application = SPI_APPLICATION (
    bonobo_object_from_servant (servant));

  application->id = id;
}

static AtkObject *
get_atk_object_ref (GObject *gobject)
{
  AtkObject *aobject;

  if (ATK_IS_IMPLEMENTOR (gobject))
    {
      aobject = atk_implementor_ref_accessible (ATK_IMPLEMENTOR (gobject));
    }
  else if (ATK_IS_OBJECT (gobject))
    {
      aobject = ATK_OBJECT (gobject);
      g_object_ref (G_OBJECT (aobject));
    }
  else
    {
      aobject = NULL;
      g_error ("received event from non-AtkImplementor");
    }

  return aobject;
}

static gboolean
spi_application_toolkit_event_listener (GSignalInvocationHint *signal_hint,
					guint                  n_param_values,
					const GValue          *param_values,
					gpointer               data)
{
  Accessibility_Event e;
  AtkObject      *aobject;
  SpiAccessible *source;
  GSignalQuery   signal_query;
  char          *event_name;

  g_return_val_if_fail (the_app != NULL, FALSE);

  g_signal_query (signal_hint->signal_id, &signal_query);

  /* TODO: move GTK reference out of app.c into bridge */
  event_name = g_strdup_printf ("Gtk:%s:%s",
				g_type_name (signal_query.itype), 
				signal_query.signal_name);

#ifdef SPI_DEBUG  
  fprintf (stderr, "Received signal %s\n", event_name);
#endif
  
  aobject = get_atk_object_ref (g_value_get_object (param_values + 0));

  source = spi_accessible_new (aobject);
  e.type = CORBA_string_dup (event_name);
  e.source = CORBA_OBJECT_NIL;
  e.detail1 = 0;
  e.detail2 = 0;
  spi_init_any_nil (&e.any_data, 
		    spi_accessible_new_return (atk_get_root (), FALSE, NULL),
		    Accessibility_ROLE_UNKNOWN,
		    "");
  notify_listeners (the_app->toolkit_listeners, source, &e);

  bonobo_object_unref (BONOBO_OBJECT (source));
  g_object_unref (G_OBJECT (aobject));

  g_free (event_name);

  return TRUE;
}

static CORBA_string
impl_accessibility_application_get_locale (PortableServer_Servant servant,
					   Accessibility_LOCALE_TYPE lctype,
					   CORBA_Environment *ev)
{
    int category;
    switch (lctype) 
    {
	case Accessibility_LOCALE_TYPE_COLLATE:
	    category = LC_COLLATE;
	    break;
	case Accessibility_LOCALE_TYPE_CTYPE:
	    category = LC_CTYPE;
	    break;
	case Accessibility_LOCALE_TYPE_MONETARY:
	    category = LC_MONETARY;
	    break;
	case Accessibility_LOCALE_TYPE_NUMERIC:
	    category = LC_NUMERIC;
	    break;
	case Accessibility_LOCALE_TYPE_MESSAGES:
	default:
	    category = LC_MESSAGES;
	    break;
    }
    return CORBA_string_dup (setlocale (category, NULL));
}

static void
impl_accessibility_application_register_toolkit_event_listener (PortableServer_Servant servant,
								Accessibility_EventListener listener,
                                                                const CORBA_char *event_name,
                                                                CORBA_Environment *ev)
{
  guint spi_listener_id;
  spi_listener_id =
     atk_add_global_event_listener (spi_application_toolkit_event_listener, event_name);
  the_app->toolkit_listeners = g_list_append (the_app->toolkit_listeners,
					      CORBA_Object_duplicate (listener, ev));
#ifdef SPI_DEBUG
  fprintf (stderr, "registered %d for toolkit events named: %s\n",
           spi_listener_id,
           event_name);
#endif
}

static void
notify_listeners (GList *listeners, SpiAccessible *source, Accessibility_Event *e)
{
  GList *l;
  CORBA_Environment ev;

  CORBA_exception_init (&ev);

  for (l = listeners; l; l = l->next)
    {
      Accessibility_EventListener listener = l->data;

      e->source = bonobo_object_dup_ref (BONOBO_OBJREF (source), &ev);

      Accessibility_EventListener_notifyEvent (listener, e, &ev);
      /*
       * when this (oneway) call completes, the CORBA refcount and
       * Bonobo_Unknown refcount will be decremented by the recipient
       */
      CORBA_exception_free (&ev);
    }
}

static void
init_toolkit_names (GHashTable **generic_event_names, GHashTable **toolkit_event_names)
{
	*toolkit_event_names = g_hash_table_new (g_str_hash, g_str_equal);
	*generic_event_names = g_hash_table_new (g_str_hash, g_str_equal);
	g_hash_table_insert (*toolkit_event_names,
			     "object:property-change",
			     "Gtk:AtkObject:property-change");
	g_hash_table_insert (*generic_event_names,
			     "Gtk:AtkObject:property-change",
			     "object:property-change");
#ifdef SPI_DEBUG
	fprintf (stderr, "inserted spi_selection_changed hash\n");
#endif
}

static void
spi_application_class_init (SpiApplicationClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Application__epv *epv = &klass->epv;

  spi_application_parent_class = g_type_class_ref (SPI_ACCESSIBLE_TYPE);

  object_class->finalize = spi_accessible_application_finalize;

  epv->_get_toolkitName = impl_accessibility_application_get_toolkit_name;
  epv->_get_version = impl_accessibility_application_get_version;
  epv->_get_id = impl_accessibility_application_get_id;
  epv->_set_id = impl_accessibility_application_set_id;
  epv->registerToolkitEventListener = impl_accessibility_application_register_toolkit_event_listener;
  epv->getLocale = impl_accessibility_application_get_locale;
  init_toolkit_names (&klass->generic_event_names, &klass->toolkit_event_names);
}

static void
spi_application_init (SpiApplication *application)
{
  application->toolkit_listeners = NULL;
  the_app = application;
}

BONOBO_TYPE_FUNC_FULL (SpiApplication,
		       Accessibility_Application,
		       PARENT_TYPE, spi_application)

SpiApplication *
spi_application_new (AtkObject *app_root)
{
  return SPI_APPLICATION (spi_accessible_construct (
			  SPI_APPLICATION_TYPE, app_root));
}
