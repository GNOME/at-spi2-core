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

#include <stdio.h>
#include <stdlib.h>
#include <libbonobo.h>
#include <orbit/orbit.h>
#include <atk/atk.h>
#include <atk/atkobject.h>
#include <libspi/Accessibility.h>
#include "accessible.h"
#include "application.h"

#define APP_STATIC_BUFF_SZ 64

typedef struct _ArgStruct ArgStruct;

struct _ArgStruct {
  gint c;
  char **v;
};

static CORBA_Environment ev;
static Accessibility_SpiRegistry registry;
static SpiApplication *this_app;

static gboolean bridge_register_app (gpointer p);
static void bridge_focus_tracker (AtkObject *object);
static void bridge_exit_func(void);
static gboolean bridge_register_event_listener ();
static void register_atk_event_listeners();
static gboolean bridge_property_event_listener (GSignalInvocationHint *signal_hint,
						guint n_param_values,
						const GValue *param_values,
						gpointer data);
static gboolean bridge_signal_listener (GSignalInvocationHint *signal_hint,
					guint n_param_values,
					const GValue *param_values,
					gpointer data);

int
gtk_module_init(gint *argc, gchar **argv[])
{
  ArgStruct *args = (ArgStruct *) g_new0(ArgStruct, 1);
  args->c = *argc;
  args->v = *argv;
  g_idle_add (bridge_register_app, args);
  g_atexit (bridge_exit_func);
}

static gboolean
bridge_register_app (gpointer gp)
{
  CORBA_Object oclient;
  AtkObject *atko;
  char *obj_id;
  ArgStruct *args = (ArgStruct *)gp;

  CORBA_exception_init(&ev);

  if (!bonobo_init (&(args->c), args->v))
    {
      g_error ("Could not initialize Bonobo");
    }

  /* Create the accesssible application server object */
  this_app = spi_application_new(atk_get_root ());

  obj_id = "OAFIID:Accessibility_SpiRegistry:proto0.1";

  oclient = bonobo_activation_activate_from_id (obj_id, 0, NULL, &ev);
  if (ev._major != CORBA_NO_EXCEPTION) {
    fprintf(stderr,
            ("Accessibility app error: exception during registry activation from id: %s\n"),
            CORBA_exception_id(&ev));
    CORBA_exception_free(&ev);
    exit(-1);
  }

  if (CORBA_Object_is_nil (oclient, &ev))
    {
      g_error ("Could not locate registry");
    }

  registry = (Accessibility_SpiRegistry) oclient;

  fprintf(stderr, "About to register application\n");

  bonobo_activate ();

  Accessibility_SpiRegistry_registerSpiApplication (registry,
                                              CORBA_Object_duplicate (BONOBO_OBJREF (this_app), &ev),
                                              &ev);

  register_atk_event_listeners ();

  return FALSE;
}

static void
register_atk_event_listeners ()
{
  /* Register for focus event notifications, and register app with central registry  */

/* kludge to make sure the Atk interface types are registered, otherwise
   the AtkText signal handlers below won't get registered */

  ATK_TYPE_TEXT;

  atk_add_focus_tracker (bridge_focus_tracker);
  atk_add_global_event_listener (bridge_property_event_listener, "Gtk:AtkObject:property-change");
/*  atk_add_global_event_listener (bridge_signal_listener, "Gtk:AtkObject:children-changed");
  atk_add_global_event_listener (bridge_signal_listener, "Gtk:AtkText:text-changed");
  atk_add_global_event_listener (bridge_signal_listener, "Gtk:AtkText:text-caret-moved");*/
}

static void bridge_exit_func()
{
  fprintf (stderr, "exiting bridge\n");
  Accessibility_SpiRegistry_deregisterSpiApplication (registry,
						CORBA_Object_duplicate (BONOBO_OBJREF (this_app), &ev),
						&ev);
  fprintf (stderr, "bridge exit func complete.\n");
}

static void bridge_focus_tracker (AtkObject *object)
{
  Accessibility_Event *e = Accessibility_Event__alloc();
  e->type = CORBA_string_dup ("focus:");
  e->source = CORBA_Object_duplicate (BONOBO_OBJREF (spi_accessible_new (object)), &ev);
  e->detail1 = 0;
  e->detail2 = 0;
  Accessibility_SpiRegistry_notifyEvent (registry, e, &ev);
}

static gboolean
bridge_property_event_listener (GSignalInvocationHint *signal_hint,
				guint n_param_values,
				const GValue *param_values,
				gpointer data)
{
  Accessibility_Event *e = Accessibility_Event__alloc();
  Bonobo_Unknown source = NULL;
  AtkObject *aobject;
  AtkPropertyValues *values;
  GObject *gobject;
  GSignalQuery signal_query;
  gchar *name;
  char sbuf[APP_STATIC_BUFF_SZ];
  
  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;
#ifdef SPI_BRIDGE_DEBUG
  fprintf (stderr, "Received (property) signal %s:%s\n",
	   g_type_name (signal_query.itype), name);
#endif
  gobject = g_value_get_object (param_values + 0);
  values = (AtkPropertyValues*) g_value_get_pointer (param_values + 1);

  /* notify the actual listeners */
  if (ATK_IS_IMPLEMENTOR (gobject))
  {
    aobject = atk_implementor_ref_accessible (ATK_IMPLEMENTOR (gobject));
    source = CORBA_Object_duplicate (BONOBO_OBJREF (spi_accessible_new (aobject)), &ev);
    g_object_unref (G_OBJECT(aobject));
  }
  else if (ATK_IS_OBJECT (gobject))
  {
    aobject = ATK_OBJECT (gobject);
    source = CORBA_Object_duplicate (BONOBO_OBJREF (spi_accessible_new (aobject)), &ev);
  }
  else
  {
    g_error("received property-change event from non-AtkImplementor");
  }
  
  snprintf(sbuf, APP_STATIC_BUFF_SZ, "object:property-change:%s", values->property_name);
  e->type = CORBA_string_dup (sbuf);
  e->source = source;
  e->detail1 = 0;
  e->detail2 = 0;
  if (source)
    Accessibility_SpiRegistry_notifyEvent (registry, e, &ev);
  return TRUE;
}

static gboolean
bridge_signal_listener (GSignalInvocationHint *signal_hint,
			guint n_param_values,
			const GValue *param_values,
			gpointer data)
{
  Accessibility_Event *e = g_new0(Accessibility_Event, 1);
  AtkObject *aobject;
  Bonobo_Unknown source;
  AtkPropertyValues *values;
  GObject *gobject;
  GSignalQuery signal_query;
  gchar *name;
  char sbuf[APP_STATIC_BUFF_SZ];
  
  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;
#ifdef SPI_BRIDGE_DEBUG
  fprintf (stderr, "Received (property) signal %s:%s\n",
	   g_type_name (signal_query.itype), name);
#endif
  gobject = g_value_get_object (param_values + 0);

  /* notify the actual listeners */
  if (ATK_IS_IMPLEMENTOR (gobject))
  {
    aobject = atk_implementor_ref_accessible (ATK_IMPLEMENTOR (gobject));
  }
  else if (ATK_IS_OBJECT (gobject))
  {
    aobject = ATK_OBJECT (gobject);
    g_object_ref (aobject);
  }
  else
  {
    g_error("received property-change event from non-AtkImplementor");
  }

  snprintf(sbuf, APP_STATIC_BUFF_SZ, "%s:%s", name, g_type_name (signal_query.itype));
  source =  CORBA_Object_duplicate (BONOBO_OBJREF (spi_accessible_new (aobject)), &ev);
  e->type = CORBA_string_dup (sbuf);
  e->source = source;
  e->detail1 = 0;
  e->detail2 = 0;
  Accessibility_SpiRegistry_notifyEvent (registry, e, &ev);
  g_object_unref (aobject);
  return TRUE;
}

static Accessibility_SpiRegistry bridge_get_registry ()
{
  return registry;
}
