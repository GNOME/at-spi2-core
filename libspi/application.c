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
 * application.c: implements SpiApplication.idl
 *
 */
#include <string.h>
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
#define PARENT_TYPE SPI_ACCESSIBLE_TYPE

/*
 * A pointer to our parent object class
 */
static SpiAccessibleClass *spi_application_parent_class;

static SpiApplication *the_app;

/* static methods */

static void notify_listeners (GList *listeners,
			      Accessibility_Event *e,
			      CORBA_Environment *ev);

static char *reverse_lookup_name_for_toolkit_event (char *toolkit_name);

static const char *
lookup_toolkit_event_for_name (const char *generic_name)
{
    char *toolkit_specific_name;
    SpiApplicationClass *klass = g_type_class_peek (SPI_APPLICATION_TYPE);
#ifdef SPI_DEBUG
    fprintf (stderr, "looking for %s in hash table.\n", generic_name);
#endif
    toolkit_specific_name =
	    (char *) g_hash_table_lookup (klass->toolkit_event_names, generic_name);
#ifdef SPI_DEBUG
    fprintf (stderr, "generic event %s converted to %s\n", generic_name, toolkit_specific_name);
#endif
    return toolkit_specific_name;
}

/*
 * Implemented GObject::finalize
 */
static void
spi_accessible_application_finalize (GObject *object)
{
  /* TODO: any necessary cleanup */
  (G_OBJECT_CLASS (spi_application_parent_class))->finalize (object);
}

static CORBA_string
impl_accessibility_application_get_toolkit_name (PortableServer_Servant servant,
                                                 CORBA_Environment *ev)
{
	return CORBA_string_dup (atk_get_toolkit_name ());
}

static CORBA_string
impl_accessibility_application_get_version (PortableServer_Servant servant,
                                            CORBA_Environment *ev)
{
	return CORBA_string_dup (atk_get_toolkit_version ());
}

static CORBA_long
impl_accessibility_application_get_id (PortableServer_Servant servant,
                                       CORBA_Environment *ev)
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

#define APP_STATIC_BUFF_SZ 64

static gboolean
spi_application_object_event_listener (GSignalInvocationHint *signal_hint,
				   guint n_param_values,
				   const GValue *param_values,
				   gpointer data)
{
  Accessibility_Event *e = Accessibility_Event__alloc();
  AtkObject *aobject;
  GObject *gobject;
  SpiAccessible *source;
  CORBA_Environment ev;
  GSignalQuery signal_query;
  const gchar *name;
  char sbuf[APP_STATIC_BUFF_SZ];
  char *generic_name;
  
  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;
  fprintf (stderr, "Received (object) signal %s:%s\n",
	   g_type_name (signal_query.itype), name);

  /* TODO: move GTK dependency out of app.c into bridge */
  snprintf(sbuf, APP_STATIC_BUFF_SZ, "Gtk:%s:%s", g_type_name (signal_query.itype), name);

  generic_name = reverse_lookup_name_for_toolkit_event (sbuf);
  gobject = g_value_get_object (param_values + 0);

  /* notify the actual listeners */
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
    g_error("received event from non-AtkImplementor");
  }

  g_return_val_if_fail (generic_name, FALSE);
  if (generic_name)
    {
        source = spi_accessible_new (aobject);
	e->type = CORBA_string_dup (generic_name);
	e->source = BONOBO_OBJREF (source);
        /*
	 * no need to dup this ref, since it's inprocess               
	 * and will be dup'ed by (inprocess) notify_listeners() call below
	 */
	e->detail1 = 0;
	e->detail2 = 0;
	if (the_app) notify_listeners (the_app->toolkit_listeners, e, &ev);
        /* unref because the in-process notify has called b_o_dup_ref (e->source) */
        bonobo_object_release_unref (e->source, &ev); 
    }
  /* and, decrement the refcount on atkobject, incremented moments ago:
   *  the call to spi_accessible_new() above should have added an extra ref */
  g_object_unref (G_OBJECT (aobject));

  return TRUE;
}


static gboolean
spi_application_toolkit_event_listener (GSignalInvocationHint *signal_hint,
				    guint n_param_values,
				    const GValue *param_values,
				    gpointer data)
{
  Accessibility_Event *e = Accessibility_Event__alloc();
  AtkObject *aobject;
  GObject *gobject;
  SpiAccessible *source;
  CORBA_Environment ev;
  GSignalQuery signal_query;
  const char *name;
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
      source = spi_accessible_new (aobject);
      e->type = CORBA_string_dup (sbuf);
      e->source = BONOBO_OBJREF (source);
      e->detail1 = 0;
      e->detail2 = 0;
      if (the_app) notify_listeners (the_app->toolkit_listeners, e, &ev);
      bonobo_object_unref (BONOBO_OBJECT (source));
      g_object_unref (G_OBJECT (aobject));
    }
  return TRUE;
}

static void
impl_accessibility_application_register_toolkit_event_listener (PortableServer_Servant servant,
								Accessibility_EventListener listener,
                                                                const CORBA_char *event_name,
                                                                CORBA_Environment *ev)
{
  guint spi_listener_id;
  spi_listener_id =
     atk_add_global_event_listener (spi_application_toolkit_event_listener, (char *) event_name);
  the_app->toolkit_listeners = g_list_append (the_app->toolkit_listeners,
					      CORBA_Object_duplicate (listener, ev));
#ifdef SPI_DEBUG
  fprintf (stderr, "registered %d for toolkit events named: %s\n",
           spi_listener_id,
           event_name);
#endif
}

static void
impl_accessibility_application_register_object_event_listener (PortableServer_Servant servant,
							       Accessibility_EventListener listener,
							       const CORBA_char *event_name,
							       CORBA_Environment *ev)
{
  guint spi_listener_id = 0;
  const char *toolkit_specific_event_name =
	  lookup_toolkit_event_for_name (event_name);
  if (toolkit_specific_event_name)
  {
    spi_listener_id =
       atk_add_global_event_listener (spi_application_object_event_listener,
				      CORBA_string_dup (toolkit_specific_event_name));
    the_app->toolkit_listeners = g_list_append (the_app->toolkit_listeners,
					      CORBA_Object_duplicate (listener, ev));
  }
#ifdef SPI_DEBUG
  fprintf (stderr, "registered %d for object events named: %s\n",
           spi_listener_id,
           event_name);
#endif
}

static void
notify_listeners (GList *listeners, Accessibility_Event *e, CORBA_Environment *ev)
{
    int n_listeners=0;
    int i;
    if (listeners) n_listeners = g_list_length (listeners);

    for (i=0; i<n_listeners; ++i) {
        Accessibility_EventListener listener;
	e->source = bonobo_object_dup_ref (e->source, ev); 
	listener = (Accessibility_EventListener) g_list_nth_data (listeners, i);
	Accessibility_EventListener_notifyEvent (listener, e, ev);
	/*
	 * when this (oneway) call completes, the CORBA refcount and
	 * Bonobo_Unknown refcount will be decremented by the recipient
	 */
    }
}

static char *
reverse_lookup_name_for_toolkit_event (char *toolkit_specific_name)
{
    char *generic_name;
    SpiApplicationClass *klass = g_type_class_peek (SPI_APPLICATION_TYPE);
#ifdef SPI_DEBUG
    fprintf (stderr, "(reverse lookup) looking for %s in hash table.\n", toolkit_specific_name);
#endif
    generic_name =
	    (char *) g_hash_table_lookup (klass->generic_event_names, toolkit_specific_name);
#ifdef SPI_DEBUG
    fprintf (stderr, "toolkit event %s converted to %s\n", toolkit_specific_name, generic_name);
#endif
    return generic_name;
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
  init_toolkit_names (&klass->generic_event_names, &klass->toolkit_event_names);
}

static void
spi_application_init (SpiApplication  *application)
{
  SPI_ACCESSIBLE (application)->atko = g_object_new (atk_object_get_type(), NULL);
  application->toolkit_listeners = (GList *) NULL;
  the_app = application;
}

BONOBO_TYPE_FUNC_FULL (SpiApplication,
		       Accessibility_Application,
		       PARENT_TYPE, spi_application);

SpiApplication *
spi_application_new (AtkObject *app_root)
{
    SpiApplication *retval =
               SPI_APPLICATION (g_object_new (spi_application_get_type (), NULL));
    SPI_ACCESSIBLE (retval)->atko = app_root;
    g_object_ref (G_OBJECT (app_root));
    return retval;
}
