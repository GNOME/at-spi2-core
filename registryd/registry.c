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
 * registry.c: the main accessibility service registry implementation
 */

#ifdef SPI_DEBUG
#include <stdio.h>
#endif
#include <config.h>
#include <bonobo/Bonobo.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::SpiRegistry" server
 */
#include <libspi/Accessibility.h>

/*
 * We'd like to replace the dependance on X-isms with a wrapper layer,
 * to the extent that it can't be done with pure GDK.
 * Anyone want to help?
 */
#include <X11/Xlib.h>
#include <gdk/gdkx.h>

/*
 * This pulls the definition for the BonoboObject (GType)
 */
#include "registry.h"

/*
 * Our parent GObject type
 */
#define PARENT_TYPE SPI_LISTENER_TYPE

/*
 * A pointer to our parent object class
 */
static SpiListenerClass *spi_registry_parent_class;

typedef enum {
  ETYPE_FOCUS,
  ETYPE_OBJECT,
  ETYPE_PROPERTY,
  ETYPE_WINDOW,
  ETYPE_TOOLKIT,
  ETYPE_LAST_DEFINED
} EventTypeCategory;

typedef struct {
  char *event_name;
  EventTypeCategory type_cat;
  char * major;
  char * minor;
  char * detail;
  guint hash;
} EventTypeStruct;

typedef struct {
  Accessibility_EventListener listener;
  guint event_type_hash;
  EventTypeCategory event_type_cat;
} SpiListenerStruct;

/* static function prototypes */
static void _spi_registry_notify_listeners ( GList *listeners,
                                        const Accessibility_Event *e,
                                        CORBA_Environment *ev);

static long _get_unique_id();

static gboolean _spi_device_event_controller_hook (gpointer source);

/*
 * Implemented GObject::finalize
 */
static void
spi_registry_object_finalize (GObject *object)
{
/*        SpiRegistry *registry = SPI_REGISTRY (object); */
        GObjectClass *object_class = G_OBJECT_GET_CLASS( object);

        printf("spi_registry_object_finalize called\n");

        object_class->finalize (object);
}

/**
 * registerSpiApplication:
 * @application: a reference to the requesting @SpiApplication
 * return values: void
 *
 * Register a new application with the accessibility broker.
 *
 **/
static void
impl_accessibility_spi_registry_register_application (PortableServer_Servant servant,
                                                  const Accessibility_SpiApplication application,
                                                  CORBA_Environment * ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));

#ifdef SPI_DEBUG
  fprintf (stderr, "registering app %p\n", application);
#endif
  registry->desktop->applications = g_list_append (registry->desktop->applications,
                                                   bonobo_object_dup_ref (application, ev));

  /* TODO: create unique string here (with libuuid call ?) and hash ? */
  Accessibility_SpiApplication__set_id (application, _get_unique_id(), ev);

  /*
   * TODO: change the implementation below to a WM-aware one;
   * e.g. don't add all apps to the SpiDesktop
   */
}

static gint
compare_corba_objects (gconstpointer p1, gconstpointer p2)
{
  CORBA_Environment ev;
  gint retval;

#ifdef SPI_DEBUG
  fprintf (stderr, "comparing %p to %p\n",
	   p1, p2);
#endif
  
  retval = !CORBA_Object_is_equivalent ((CORBA_Object) p1, (CORBA_Object) p2, &ev);
  return retval;  
}

static void
register_with_toolkits (SpiRegistry *spi_registry_bonobo_object, EventTypeStruct *etype, CORBA_Environment *ev)
{
  gint n_desktops;
  gint n_apps;
  gint i, j;
  Accessibility_SpiDesktop desktop;
  Accessibility_SpiApplication app;
  Accessibility_SpiRegistry registry;
  registry  = BONOBO_OBJREF (spi_registry_bonobo_object);

  /* for each app in each desktop, call ...SpiApplication_registerToolkitEventListener */

  n_desktops = Accessibility_SpiRegistry_getDesktopCount (registry, ev);

  for (i=0; i<n_desktops; ++i)
    {
      desktop = Accessibility_SpiRegistry_getDesktop (registry, i, ev);
      n_apps = Accessibility_SpiDesktop__get_childCount (desktop, ev);
      for (j=0; j<n_apps; ++j)
        {
          app = (Accessibility_SpiApplication) Accessibility_SpiDesktop_getChildAtIndex (desktop,
                                                                                   j,
                                                                                   ev);
	  Accessibility_SpiApplication_registerToolkitEventListener (app,
								  registry,
								  CORBA_string_dup (etype->event_name),
								  
								  ev);
        }
    }
}

static gint
compare_spi_listener_hash (gconstpointer p1, gconstpointer p2)
{
  return (((SpiListenerStruct *)p2)->event_type_hash - ((SpiListenerStruct *)p1)->event_type_hash);
}

static gint
compare_spi_listener_corbaref (gconstpointer p1, gconstpointer p2)
{
  return compare_corba_objects (((SpiListenerStruct *)p2)->listener,
                                ((SpiListenerStruct *)p1)->listener);
}

static void
parse_event_type (EventTypeStruct *etype, char *event_name)
{
  guint nbytes = 0;
  gchar **split_string;

  split_string = g_strsplit(event_name, ":", 4);
  etype->event_name = g_strndup(event_name, 255);

  if (!g_ascii_strncasecmp (event_name, "focus:", 6))
    {
      etype->type_cat = ETYPE_FOCUS;
    }
  else if (!g_ascii_strncasecmp (event_name, "object:", 7))
    {
      etype->type_cat = ETYPE_OBJECT;
    }
  else if (!g_ascii_strncasecmp (event_name, "window:", 7))
    {
      etype->type_cat = ETYPE_WINDOW;
    }
  else
    {
      etype->type_cat = ETYPE_TOOLKIT;
    }

  if (split_string[1])
    {
      etype->major = split_string[1];
      if (split_string[2])
        {
          etype->minor = split_string[2];
          if (split_string[3])
            {
              etype->detail = split_string[3];
              etype->hash = g_str_hash ( g_strconcat (split_string[1], split_string[2], split_string[3], NULL));
            }
          else
            {
              etype->detail = g_strdup ("");
              etype->hash = g_str_hash ( g_strconcat (split_string[1], split_string[2], NULL));
            }
        }
      else
        {
          etype->minor = g_strdup ("");
          etype->hash = g_str_hash ( split_string[1]);
        }
    }
  else
    {
      etype->major = g_strdup ("");
      etype->minor = g_strdup ("");
      etype->detail = g_strdup ("");
      etype->hash = g_str_hash ("");
    }

  /* TODO: don't forget to free the strings from caller when done ! */
}

/**
 * deregisterSpiApplication:
 * @application: a reference to the @SpiApplication
 * to be deregistered.
 * return values: void
 *
 * De-register an application previously registered with the broker.
 *
 **/
static void
impl_accessibility_spi_registry_deregister_application (PortableServer_Servant servant,
                                                    const Accessibility_SpiApplication application,
                                                    CORBA_Environment * ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  GList *list = g_list_find_custom (registry->desktop->applications, &application, compare_corba_objects);

#ifdef SPI_DEBUG
  gint i;
#endif

  if (list)
    {
#ifdef SPI_DEBUG
      fprintf (stderr, "deregistering application %p\n", application);
#endif
      registry->desktop->applications = g_list_delete_link (registry->desktop->applications, list);
#ifdef SPI_DEBUG
      fprintf (stderr, "there are now %d apps registered.\n", g_list_length (registry->desktop->applications));
      for (i = 0; i < g_list_length (registry->desktop->applications); ++i) {
          fprintf (stderr, "getting application %d\n", i);
          fprintf (stderr, "object address %p\n",
               g_list_nth_data (registry->desktop->applications, i));
      }
#endif      
    }
  else
    fprintf (stderr, "could not deregister application\n");
}

/*
 * CORBA Accessibility::SpiRegistry::registerGlobalEventListener method implementation
 */
static void
impl_accessibility_spi_registry_register_global_event_listener (
	                                     PortableServer_Servant  servant,
                                             Accessibility_EventListener listener,
                                             const CORBA_char *event_name,
                                             CORBA_Environment      *ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  SpiListenerStruct *ls = g_malloc (sizeof (SpiListenerStruct));
  EventTypeStruct etype;
  gboolean is_toolkit_specific = TRUE;

  fprintf(stderr, "registering for events of type %s\n", event_name);

  /* parse, check major event type and add listener accordingly */
  parse_event_type (&etype, event_name);
  ls->event_type_hash = etype.hash;
  ls->event_type_cat = etype.type_cat;

  switch (etype.type_cat)
    {
    case (ETYPE_FOCUS) :
    case (ETYPE_OBJECT) :
    case (ETYPE_PROPERTY) :
      ls->listener = CORBA_Object_duplicate (listener, ev);
      registry->object_listeners =
        g_list_append (registry->object_listeners, ls);
      break;
    case (ETYPE_WINDOW) :
      /* Support for Window Manager Events is not yet implemented */
      break;
    case (ETYPE_TOOLKIT) :
      ls->listener = CORBA_Object_duplicate (listener, ev);
      registry->toolkit_listeners =
        g_list_append (registry->toolkit_listeners, ls);
      register_with_toolkits (registry, &etype, ev);
      break;
    default:
      break;
    }
}

/*
 * CORBA Accessibility::SpiRegistry::deregisterGlobalEventListenerAll method implementation
 */
static void
impl_accessibility_spi_registry_deregister_global_event_spi_listener_all (
                                                    PortableServer_Servant  servant,
                                                    Accessibility_EventListener listener,
                                                    CORBA_Environment      *ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  SpiListenerStruct *ls = g_malloc (sizeof (SpiListenerStruct));
  GList *list;
  ls->listener = listener;  
  list = g_list_find_custom (registry->object_listeners, ls,
			     compare_spi_listener_corbaref);

  /*
   * TODO : de-register with toolkit if the last instance of a listener
   *        to a particular toolkit event type has been deregistered.
   */

  while (list)
    {
      fprintf (stderr, "deregistering listener\n");
      registry->object_listeners = g_list_delete_link (registry->object_listeners, list);
      list = g_list_find_custom (registry->object_listeners, ls, compare_spi_listener_corbaref);
    }
  list = g_list_find_custom (registry->toolkit_listeners, ls, compare_spi_listener_corbaref);
  while (list)
    {
      fprintf (stderr, "deregistering listener\n");
      registry->toolkit_listeners = g_list_delete_link (registry->toolkit_listeners, list);
      list = g_list_find_custom (registry->toolkit_listeners, ls, compare_spi_listener_corbaref);
    }
}

/*
 * CORBA Accessibility::SpiRegistry::deregisterGlobalEventListener method implementation
 */
static void
impl_accessibility_spi_registry_deregister_global_event_listener (
                                                    PortableServer_Servant  servant,
                                                    Accessibility_EventListener listener,
                                                    const CORBA_char * event_name,
                                                    CORBA_Environment      *ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  SpiListenerStruct ls;
  EventTypeStruct etype;
  GList *list;
  GList **listeners;

  parse_event_type (&etype, event_name);
  switch (etype.type_cat)
    {
    case (ETYPE_OBJECT) :
    case (ETYPE_PROPERTY) :
    case (ETYPE_FOCUS) :
      listeners = &registry->object_listeners;
      break;
    case (ETYPE_WINDOW) :
      /* Support for Window Manager Events is not yet implemented */
      break;
    case (ETYPE_TOOLKIT) :
      listeners = &registry->toolkit_listeners;
      break;
    default:
      break;
    }

  ls.event_type_hash = etype.hash;
  list = g_list_find_custom (*listeners, &ls, compare_spi_listener_hash);

  while (list)
    {
      fprintf (stderr, "deregistering listener\n");
      *listeners = g_list_delete_link (*listeners, list);
      list = g_list_find_custom (*listeners, &ls, compare_spi_listener_hash);
    }
}


/**
 * getDesktopCount:
 * return values: a short integer indicating the current number of
 * @SpiDesktops.
 *
 * Get the current number of desktops.
 *
 **/
static short
impl_accessibility_spi_registry_get_spi_desktop_count (PortableServer_Servant servant,
                                               CORBA_Environment * ev)
{
  /* TODO: implement support for multiple virtual desktops */
  CORBA_short n_desktops;
  n_desktops = (CORBA_short) 1;
  return n_desktops;
}

/**
 * getDesktop:
 * @n: the index of the requested @SpiDesktop.
 * return values: a reference to the requested @SpiDesktop.
 *
 * Get the nth accessible desktop.
 *
 **/
static Accessibility_SpiDesktop
impl_accessibility_spi_registry_get_desktop (PortableServer_Servant servant,
                                         const CORBA_short n,
                                         CORBA_Environment * ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));

  /* TODO: implement support for multiple virtual desktops */
  if (n == 0)
    {
      return (Accessibility_SpiDesktop)
        CORBA_Object_duplicate (BONOBO_OBJREF (registry->desktop), ev);
    }
  else
    {
      return (Accessibility_SpiDesktop) CORBA_OBJECT_NIL;
    }
}

/**
 * getDesktopList:
 * return values: a sequence containing references to
 * the @SpiDesktops.
 *
 * Get a list of accessible desktops.
 *
 **/
static Accessibility_SpiDesktopSeq *
impl_accessibility_spi_registry_get_spi_desktop_list (PortableServer_Servant servant,
                                              CORBA_Environment * ev)
{
  /* TODO: implement support for multiple virtual desktops */
  return (Accessibility_SpiDesktopSeq *) NULL;
}

static Accessibility_SpiDeviceEventController
impl_accessibility_spi_registry_get_device_event_controller (PortableServer_Servant servant,
                                                         CORBA_Environment * ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  if (!registry->device_event_controller)
    registry->device_event_controller = g_object_new (SPI_DEVICE_EVENT_CONTROLLER_TYPE, NULL);
  return CORBA_Object_duplicate (BONOBO_OBJREF (registry->device_event_controller), ev);
}

static void
impl_spi_registry_notify_event (PortableServer_Servant servant,
                            const Accessibility_Event *e,
                            CORBA_Environment *ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  EventTypeStruct etype;

  parse_event_type (&etype, e->type);

  switch (etype.type_cat)
    {
    case (ETYPE_OBJECT) :
    case (ETYPE_PROPERTY) :
    case (ETYPE_FOCUS) :
      _spi_registry_notify_listeners (registry->object_listeners, e, ev); 
      break;
    case (ETYPE_WINDOW) :
      _spi_registry_notify_listeners (registry->window_listeners, e, ev);
      break;
    case (ETYPE_TOOLKIT) :
      _spi_registry_notify_listeners (registry->toolkit_listeners, e, ev); 
      break;
    default:
      break;
    }
  /* Accessibility_SpiAccessible_unref (e->source, ev);*/ /* This should be here! */
}

static long
_get_unique_id ()
{
  static long id = 0;
  return ++id;
}

static void
_spi_registry_notify_listeners ( GList *listeners,
                            const Accessibility_Event *e,
                            CORBA_Environment *ev)
{
  int n;
  int len;
  SpiListenerStruct *ls;
  EventTypeStruct etype;
  guint minor_hash;
  parse_event_type (&etype, e->type);
  minor_hash = g_str_hash (g_strconcat (etype.major, etype.minor, NULL));
  len = g_list_length (listeners);

  for (n=0; n<len; ++n)
    {
      ls =  (SpiListenerStruct *) g_list_nth_data (listeners, n);
#ifdef SPI_SPI_LISTENER_DEBUG
      fprintf(stderr, "event hashes: %lx %lx %lx\n", ls->event_type_hash, etype.hash, minor_hash);
      fprintf(stderr, "event name: %s\n", etype.event_name);
#endif
      if ((ls->event_type_hash == etype.hash) || (ls->event_type_hash == minor_hash))
        {
#ifdef SPI_DEBUG
          fprintf(stderr, "notifying listener #%d\n", n);
          fprintf(stderr, "event source name %s\n", Accessibility_SpiAccessible__get_name(e->source, ev));
#endif
	  e->source = CORBA_Object_duplicate (e->source, ev);
          Accessibility_SpiAccessible_ref ( e->source, ev);
          Accessibility_EventListener_notifyEvent ((Accessibility_EventListener) ls->listener,
                                                   e,
                                                   ev);
          if (ev->_major != CORBA_NO_EXCEPTION) {
                fprintf(stderr,
                ("Accessibility app error: exception during event notification: %s\n"),
                        CORBA_exception_id(ev));
                exit(-1);
          }
        }
    }
}

static gboolean _spi_device_event_controller_hook (gpointer p)
{
    SpiRegistry *registry = (SpiRegistry *)p;
    SpiDeviceEventController *controller = registry->device_event_controller;
    if (controller)
	spi_device_event_controller_check_key_event (controller);
    return TRUE;
}

static void
spi_registry_class_init (SpiRegistryClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_SpiRegistry__epv *epv = &klass->epv;

        spi_registry_parent_class = g_type_class_ref (SPI_LISTENER_TYPE);

        object_class->finalize = spi_registry_object_finalize;

        epv->registerSpiApplication = impl_accessibility_spi_registry_register_application;
        epv->deregisterSpiApplication = impl_accessibility_spi_registry_deregister_application;
        epv->registerGlobalEventListener = impl_accessibility_spi_registry_register_global_event_listener;
        epv->deregisterGlobalEventListener = impl_accessibility_spi_registry_deregister_global_event_listener;
        epv->deregisterGlobalEventListenerAll = impl_accessibility_spi_registry_deregister_global_event_spi_listener_all;
        epv->getDeviceEventController = impl_accessibility_spi_registry_get_device_event_controller;
        epv->getDesktopCount = impl_accessibility_spi_registry_get_spi_desktop_count;
        epv->getDesktop = impl_accessibility_spi_registry_get_desktop;
        epv->getDesktopList = impl_accessibility_spi_registry_get_spi_desktop_list;

        ((SpiListenerClass *) klass)->epv.notifyEvent = impl_spi_registry_notify_event;
}

static void
spi_registry_init (SpiRegistry *registry)
{
  registry->object_listeners = NULL;
  registry->window_listeners = NULL;
  registry->toolkit_listeners = NULL;
  registry->applications = NULL;
  registry->desktop = spi_desktop_new();
  registry->device_event_controller = NULL;
  registry->kbd_event_hook = _spi_device_event_controller_hook;
}

GType
spi_registry_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (SpiRegistryClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) spi_registry_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (SpiRegistry),
                        0, /* n preallocs */
                        (GInstanceInitFunc) spi_registry_init,
                        NULL /* value table */
                };
                /*
                 *   Here we use bonobo_type_unique instead of
                 * gtk_type_unique, this auto-generates a load of
                 * CORBA structures for us. All derived types must
                 * use bonobo_type_unique.
                 */
                type = bonobo_type_unique (
                        PARENT_TYPE,
                        POA_Accessibility_SpiRegistry__init,
                        NULL,
                        G_STRUCT_OFFSET (SpiRegistryClass, epv),
                        &tinfo,
                        "SpiRegistry");
        }

        return type;
}

SpiRegistry *
spi_registry_new (void)
{
    SpiRegistry *retval =
               SPI_REGISTRY (g_object_new (spi_registry_get_type (), NULL));
    return retval;
}
