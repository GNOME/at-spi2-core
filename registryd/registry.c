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
 * registry.c: test for accessibility implementation
 *
 */

#ifdef SPI_DEBUG
#include <stdio.h>
#endif
#include <config.h>
#include <bonobo/Bonobo.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::Registry" server
 */
#include <libspi/Accessibility.h>

/*
 * This pulls the definition for the BonoboObject (GType)
 */
#include "registry.h"

/*
 * Our parent GObject type
 */
#define PARENT_TYPE LISTENER_TYPE

/*
 * A pointer to our parent object class
 */
static ListenerClass *registry_parent_class;

typedef enum {
  ETYPE_FOCUS,
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
} ListenerStruct;

/* static function prototypes */
static void registry_notify_listeners ( GList *listeners,
                                        const Accessibility_Event *e,
                                        CORBA_Environment *ev);

/*
 * Implemented GObject::finalize
 */
static void
registry_object_finalize (GObject *object)
{
/*        Registry *registry = REGISTRY (object); */
        GObjectClass *object_class = G_OBJECT_GET_CLASS( object);

        printf("registry_object_finalize called\n");

        object_class->finalize (object);
}

/**
 * registerApplication:
 * @application: a reference to the requesting @Application
 * return values: void
 *
 * Register a new application with the accessibility broker.
 *
 **/
static void
impl_accessibility_registry_register_application (PortableServer_Servant servant,
                                                  const Accessibility_Application application,
                                                  CORBA_Environment * ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));

#ifdef SPI_DEBUG
  fprintf (stderr, "registering app %p\n", application);
#endif
  registry->desktop->applications = g_list_append (registry->desktop->applications,
                                                   CORBA_Object_duplicate (application, ev));

  /* TODO: create unique string here (with libuuid call ?) */
  Accessibility_Application__set_id (application, "test-some-unique-string", ev);

  /*
   * TODO: change the implementation below to a WM-aware one;
   * e.g. don't add all apps to the Desktop
   */
}

static gint
compare_object_hash (gconstpointer p1, gconstpointer p2)
{
  CORBA_Environment ev;
  long long diff = ((CORBA_Object_hash ((CORBA_Object) p2, (CORBA_unsigned_long) 0, &ev)) -
                    (CORBA_Object_hash ((CORBA_Object) p1, (CORBA_unsigned_long) 0, &ev)));
  return ((diff < 0) ? -1 : ((diff > 0) ? 1 : 0));
}

static void
register_with_toolkits (Registry *registry_bonobo_object, EventTypeStruct *etype, CORBA_Environment *ev)
{
  gint n_desktops;
  gint n_apps;
  gint i, j;
  Accessibility_Desktop desktop;
  Accessibility_Application app;
  Accessibility_Registry registry;
  registry  = bonobo_object_corba_objref (bonobo_object (registry_bonobo_object));

  /* for each app in each desktop, call ...Application_registerToolkitEventListener */

  n_desktops = Accessibility_Registry_getDesktopCount (registry, ev);

  for (i=0; i<n_desktops; ++i)
    {
      desktop = Accessibility_Registry_getDesktop (registry, i, ev);
      n_apps = Accessibility_Desktop__get_childCount (desktop, ev);
      for (j=0; j<n_apps; ++j)
        {
          app = (Accessibility_Application) Accessibility_Desktop_getChildAtIndex (desktop,
                                                                                   j,
                                                                                   ev);
          /* TODO: should we be ref-ing the registry object before each call ? */
          Accessibility_Application_registerToolkitEventListener (app,
                                                                  registry,
                                                                  CORBA_string_dup (etype->event_name),
                                                                  ev);
        }
    }
}

static gint
compare_listener_hash (gconstpointer p1, gconstpointer p2)
{
  return (((ListenerStruct *)p2)->event_type_hash - ((ListenerStruct *)p1)->event_type_hash);
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
 * deregisterApplication:
 * @application: a reference to the @Application
 * to be deregistered.
 * return values: void
 *
 * De-register an application previously registered with the broker.
 *
 **/
static void
impl_accessibility_registry_deregister_application (PortableServer_Servant servant,
                                                    const Accessibility_Application application,
                                                    CORBA_Environment * ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));
  GList *list = g_list_find_custom (registry->applications, application, compare_object_hash);
  if (list)
    {
      fprintf (stderr, "deregistering application\n");
      registry->applications = g_list_delete_link (registry->applications, list);
    }
}

/*
 * CORBA Accessibility::Registry::registerGlobalEventListener method implementation
 */
static void
impl_accessibility_registry_register_global_event_listener
                                            (PortableServer_Servant  servant,
                                             Accessibility_EventListener listener,
                                             const CORBA_char *event_name,
                                             CORBA_Environment      *ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));
  ListenerStruct *ls = g_malloc (sizeof (ListenerStruct));
  EventTypeStruct etype;

  fprintf(stderr, "registering for events of type %s\n", event_name);

  /* parse, check major event type and add listener accordingly */
  parse_event_type (&etype, event_name);
  ls->event_type_hash = etype.hash;
  ls->event_type_cat = etype.type_cat;

  switch (etype.type_cat)
    {
    case (ETYPE_FOCUS) :
      ls->listener = CORBA_Object_duplicate (listener, ev);
      registry->focus_listeners =
        g_list_append (registry->focus_listeners, ls);
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
 * CORBA Accessibility::Registry::deregisterGlobalEventListenerAll method implementation
 */
static void
impl_accessibility_registry_deregister_global_event_listener_all
                                                   (PortableServer_Servant  servant,
                                                    Accessibility_EventListener listener,
                                                    CORBA_Environment      *ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));
  GList *list = g_list_find_custom (registry->focus_listeners, listener, compare_object_hash);

  /*
   * TODO : de-register with toolkit if the last instance of a listener
   *        to a particular toolkit event type has been deregistered.
   */

  while (list)
    {
      fprintf (stderr, "deregistering listener\n");
      registry->focus_listeners = g_list_delete_link (registry->focus_listeners, list);
      list = g_list_find_custom (registry->focus_listeners, listener, compare_object_hash);
    }
  list = g_list_find_custom (registry->toolkit_listeners, listener, compare_object_hash);
  while (list)
    {
      fprintf (stderr, "deregistering listener\n");
      registry->toolkit_listeners = g_list_delete_link (registry->toolkit_listeners, list);
      list = g_list_find_custom (registry->toolkit_listeners, listener, compare_object_hash);
    }
}

/*
 * CORBA Accessibility::Registry::deregisterGlobalEventListener method implementation
 */
static void
impl_accessibility_registry_deregister_global_event_listener
                                                   (PortableServer_Servant  servant,
                                                    Accessibility_EventListener listener,
                                                    const CORBA_char * event_name,
                                                    CORBA_Environment      *ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));
  ListenerStruct ls;
  EventTypeStruct etype;
  GList *list;
  GList **listeners;

  parse_event_type (&etype, event_name);
  switch (etype.type_cat)
    {
    case (ETYPE_FOCUS) :
      listeners = &registry->focus_listeners;
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
  list = g_list_find_custom (*listeners, &ls, compare_listener_hash);

  while (list)
    {
      fprintf (stderr, "deregistering listener\n");
      *listeners = g_list_delete_link (*listeners, list);
      list = g_list_find_custom (*listeners, &ls, compare_listener_hash);
    }
}


/**
 * getDesktopCount:
 * return values: a short integer indicating the current number of
 * @Desktops.
 *
 * Get the current number of desktops.
 *
 **/
static short
impl_accessibility_registry_get_desktop_count (PortableServer_Servant servant,
                                               CORBA_Environment * ev)
{
  /* TODO: implement support for multiple virtual desktops */
  CORBA_short n_desktops;
  n_desktops = (CORBA_short) 1;
  return n_desktops;
}

/**
 * getDesktop:
 * @n: the index of the requested @Desktop.
 * return values: a reference to the requested @Desktop.
 *
 * Get the nth accessible desktop.
 *
 **/
static Accessibility_Desktop
impl_accessibility_registry_get_desktop (PortableServer_Servant servant,
                                         const CORBA_short n,
                                         CORBA_Environment * ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));

  /* TODO: implement support for multiple virtual desktops */
  if (n == 0)
    {
      return (Accessibility_Desktop)
        CORBA_Object_duplicate (
             bonobo_object_corba_objref (bonobo_object (registry->desktop)), ev);
    }
  else
    {
      return (Accessibility_Desktop) CORBA_OBJECT_NIL;
    }
}

/**
 * getDesktopList:
 * return values: a sequence containing references to
 * the @Desktops.
 *
 * Get a list of accessible desktops.
 *
 **/
static Accessibility_DesktopSeq *
impl_accessibility_registry_get_desktop_list (PortableServer_Servant servant,
                                              CORBA_Environment * ev)
{
  /* TODO: implement support for multiple virtual desktops */
  return (Accessibility_DesktopSeq *) NULL;
}

static CORBA_Object
impl_accessibility_registry_get_device_event_controller (PortableServer_Servant servant,
                                                         CORBA_Environment * ev)
{
  /* TODO: not yet implemented! */
  return CORBA_OBJECT_NIL;
}

static void
impl_registry_notify_event (PortableServer_Servant servant,
                            const Accessibility_Event *e,
                            CORBA_Environment *ev)
{
  Registry *registry = REGISTRY (bonobo_object_from_servant (servant));
  EventTypeStruct etype;

  parse_event_type (&etype, e->type);

  switch (etype.type_cat)
    {
    case (ETYPE_FOCUS) :
      registry_notify_listeners (registry->focus_listeners, e, ev);
      break;
    case (ETYPE_WINDOW) :
      registry_notify_listeners (registry->window_listeners, e, ev);
      break;
    case (ETYPE_TOOLKIT) :
      registry_notify_listeners (registry->toolkit_listeners, e, ev);
      break;
    default:
      break;
    }
  bonobo_object_release_unref (e->source, ev);
}

static void
registry_notify_listeners ( GList *listeners,
                            const Accessibility_Event *e,
                            CORBA_Environment *ev)
{
  int n;
  int len;
  ListenerStruct *ls;
  EventTypeStruct etype;
  guint minor_hash;
  parse_event_type (&etype, e->type);
  minor_hash = g_str_hash (g_strconcat (etype.major, etype.minor, NULL));
  len = g_list_length (listeners);

  for (n=0; n<len; ++n)
    {
      ls =  (ListenerStruct *) g_list_nth_data (listeners, n);
#ifdef SPI_DEBUG
      fprintf(stderr, "event hashes: %lx %lx %lx\n", ls->event_type_hash, etype.hash, minor_hash);
      fprintf(stderr, "event name: %s\n", etype.event_name);
#endif
      if ((ls->event_type_hash == etype.hash) || (ls->event_type_hash == minor_hash))
        {
#ifdef SPI_DEBUG
          fprintf(stderr, "notifying listener #%d\n", n);
          fprintf(stderr, "event name %s\n", Accessibility_Accessible__get_name(e->source, ev));
#endif
          bonobo_object_dup_ref ( e->source, ev);
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

static void
registry_class_init (RegistryClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_Registry__epv *epv = &klass->epv;

        registry_parent_class = g_type_class_ref (LISTENER_TYPE);

        object_class->finalize = registry_object_finalize;

        epv->registerApplication = impl_accessibility_registry_register_application;
        epv->deregisterApplication = impl_accessibility_registry_deregister_application;
        epv->registerGlobalEventListener = impl_accessibility_registry_register_global_event_listener;
        epv->deregisterGlobalEventListener = impl_accessibility_registry_deregister_global_event_listener;
        epv->deregisterGlobalEventListenerAll = impl_accessibility_registry_deregister_global_event_listener_all;
        epv->getDesktopCount = impl_accessibility_registry_get_desktop_count;
        epv->getDesktop = impl_accessibility_registry_get_desktop;
        epv->getDesktopList = impl_accessibility_registry_get_desktop_list;
        epv->getDeviceEventController = impl_accessibility_registry_get_device_event_controller;
        ((ListenerClass *) klass)->epv.notifyEvent = impl_registry_notify_event;
}

static void
registry_init (Registry *registry)
{
  registry->focus_listeners = NULL;
  registry->window_listeners = NULL;
  registry->toolkit_listeners = NULL;
  registry->applications = NULL;
  registry->desktop = desktop_new();
}

GType
registry_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (RegistryClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) registry_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (Registry),
                        0, /* n preallocs */
                        (GInstanceInitFunc) registry_init,
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
                        POA_Accessibility_Registry__init,
                        NULL,
                        G_STRUCT_OFFSET (RegistryClass, epv),
                        &tinfo,
                        "Registry");
        }

        return type;
}

Registry *
registry_new (void)
{
    Registry *retval =
               REGISTRY (g_object_new (registry_get_type (), NULL));
    return retval;
}
