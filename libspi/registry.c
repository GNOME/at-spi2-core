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

/* registry.c: the main accessibility service registry implementation */

#include <config.h>
#ifdef SPI_DEBUG
#  include <stdio.h>
#endif

#include <libspi/registry.h>

/* Our parent GObject type  */
#define PARENT_TYPE SPI_LISTENER_TYPE

/* A pointer to our parent object class */
static SpiListenerClass *spi_registry_parent_class;

typedef enum {
  ETYPE_FOCUS,
  ETYPE_OBJECT,
  ETYPE_PROPERTY,
  ETYPE_WINDOW,
  ETYPE_TOOLKIT,
  ETYPE_KEYBOARD,
  
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
static void _registry_notify_listeners (GList *listeners,
                                        const Accessibility_Event *e,
                                        CORBA_Environment *ev);

static long _get_unique_id();

static gboolean _device_event_controller_hook (gpointer source);

SpiListenerStruct *
spi_listener_struct_new (Accessibility_EventListener listener, CORBA_Environment *ev)
{
  SpiListenerStruct *retval = g_malloc (sizeof (SpiListenerStruct));
  retval->listener = bonobo_object_dup_ref (listener, ev);
  return retval;
}

void
spi_listener_struct_free (SpiListenerStruct *ls, CORBA_Environment *ev)
{
  bonobo_object_release_unref (ls->listener, ev);
  g_free (ls);
}

/* GObject::finalize */
static void
spi_registry_object_finalize (GObject *object)
{
        SpiRegistry *registry = SPI_REGISTRY (object);

        printf("spi_registry_object_finalize called\n");
	/* TODO: unref deviceeventcontroller, which disconnects key listener */
        G_OBJECT_CLASS (spi_registry_parent_class)->finalize (object);
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
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));

#ifdef SPI_DEBUG
  fprintf (stderr, "registering app %p\n", application);
#endif
  spi_desktop_add_application (registry->desktop, application);

  Accessibility_Application__set_id (application, _get_unique_id(), ev);

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
  Accessibility_Desktop desktop;
  Accessibility_Application app;
  Accessibility_Registry registry;
  registry  = BONOBO_OBJREF (spi_registry_bonobo_object);

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
  return (((SpiListenerStruct *)p2)->event_type_hash - ((SpiListenerStruct *)p1)->event_type_hash);
}

static gint
compare_listener_corbaref (gconstpointer p1, gconstpointer p2)
{
  return compare_corba_objects (((SpiListenerStruct *)p2)->listener,
                                ((SpiListenerStruct *)p1)->listener);
}

static void
parse_event_type (EventTypeStruct *etype, char *event_name)
{
  gchar **split_string;
  gchar *s;

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
      etype->major = g_strdup (split_string[1]);
      if (split_string[2])
        {
          etype->minor = g_strdup (split_string[2]);
          if (split_string[3])
            {
              etype->detail = g_strdup (split_string[3]);
	      s = g_strconcat (split_string[1], split_string[2], split_string[3], NULL);
              etype->hash = g_str_hash (s);
	      g_free (s);
            }
          else
            {
              etype->detail = g_strdup ("");
	      s = g_strconcat (split_string[1], split_string[2], NULL);
	      etype->hash = g_str_hash (s);
	      g_free (s);
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

  g_strfreev (split_string);
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
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));

  spi_desktop_remove_application (registry->desktop, application);

#ifdef SPI_DEBUG
  fprintf (stderr, "de-registered app %p\n", application);
#endif
}

/*
 * CORBA Accessibility::Registry::registerGlobalEventListener method implementation
 */
static void
impl_accessibility_registry_register_global_event_listener (
	                                     PortableServer_Servant  servant,
                                             Accessibility_EventListener listener,
                                             const CORBA_char *event_name,
                                             CORBA_Environment      *ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  SpiListenerStruct *ls = spi_listener_struct_new (listener, ev);
  EventTypeStruct etype;

  fprintf(stderr, "registering for events of type %s\n", event_name);

  /* parse, check major event type and add listener accordingly */
  parse_event_type (&etype, (char*) event_name);
  ls->event_type_hash = etype.hash;
  ls->event_type_cat = etype.type_cat;

  switch (etype.type_cat)
    {
    case (ETYPE_FOCUS) :
    case (ETYPE_OBJECT) :
    case (ETYPE_PROPERTY) :
      registry->object_listeners =
        g_list_append (registry->object_listeners, ls);
      break;
    case (ETYPE_WINDOW) :
      /* Support for Window Manager Events is not yet implemented */
      spi_listener_struct_free (ls, ev);
      break;
    case (ETYPE_TOOLKIT) :
      registry->toolkit_listeners =
        g_list_append (registry->toolkit_listeners, ls);
      register_with_toolkits (registry, &etype, ev);
      break;
    default:
      spi_listener_struct_free (ls, ev);
      break;
    }
}

/*
 * CORBA Accessibility::Registry::deregisterGlobalEventListenerAll method implementation
 */
static void
impl_accessibility_registry_deregister_global_event_listener_all (
                                                    PortableServer_Servant  servant,
                                                    Accessibility_EventListener listener,
                                                    CORBA_Environment      *ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  SpiListenerStruct *ls = spi_listener_struct_new (listener, ev);
  GList *list;
  list = g_list_find_custom (registry->object_listeners, ls,
			     compare_listener_corbaref);

  /*
   * TODO : de-register with toolkit if the last instance of a listener
   *        to a particular toolkit event type has been deregistered.
   */

  while (list)
    {
      spi_listener_struct_free ((SpiListenerStruct *) list->data, ev);
      registry->object_listeners = g_list_delete_link (registry->object_listeners, list);
      list = g_list_find_custom (registry->object_listeners, ls, compare_listener_corbaref);
    }
  list = g_list_find_custom (registry->toolkit_listeners, ls, compare_listener_corbaref);
  while (list)
    {
      spi_listener_struct_free ((SpiListenerStruct *) list->data, ev);
      registry->toolkit_listeners = g_list_delete_link (registry->toolkit_listeners, list);
      list = g_list_find_custom (registry->toolkit_listeners, ls, compare_listener_corbaref);
    }
  spi_listener_struct_free (ls, ev);
}

/*
 * CORBA Accessibility::Registry::deregisterGlobalEventListener method implementation
 */
static void
impl_accessibility_registry_deregister_global_event_listener (
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

  parse_event_type (&etype, (char *) event_name);
  switch (etype.type_cat)
    {
    case (ETYPE_OBJECT) :
    case (ETYPE_PROPERTY) :
    case (ETYPE_FOCUS) :
      listeners = &registry->object_listeners;
      break;
    case (ETYPE_WINDOW) :
      /* Support for Window Manager Events is not yet implemented */
      listeners = NULL;
      break;
    case (ETYPE_TOOLKIT) :
      listeners = &registry->toolkit_listeners;
      break;
    default:
      listeners = NULL;
      break;
    }

  if (!listeners)
	  return;

  ls.event_type_hash = etype.hash;
  list = g_list_find_custom (*listeners, &ls, compare_listener_hash);

  while (list)
    {
      spi_listener_struct_free ((SpiListenerStruct *) list->data, ev);
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
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));

  /* TODO: implement support for multiple virtual desktops */
  if (n == 0)
    {
      return (Accessibility_Desktop)
        bonobo_object_dup_ref (BONOBO_OBJREF (registry->desktop), ev);
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

static Accessibility_DeviceEventController
impl_accessibility_registry_get_device_event_controller (PortableServer_Servant servant,
                                                         CORBA_Environment * ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  if (!registry->device_event_controller)
    registry->device_event_controller = spi_device_event_controller_new (registry);

  return bonobo_object_dup_ref (BONOBO_OBJREF (registry->device_event_controller), ev);
}

static void
impl_registry_notify_event (PortableServer_Servant servant,
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
      _registry_notify_listeners (registry->object_listeners, e, ev); 
      break;
    case (ETYPE_WINDOW) :
      _registry_notify_listeners (registry->window_listeners, e, ev);
      break;
    case (ETYPE_TOOLKIT) :
      _registry_notify_listeners (registry->toolkit_listeners, e, ev); 
      break;
    case (ETYPE_KEYBOARD) :
    default:
      break;
    }
  if (e->source != CORBA_OBJECT_NIL)
    {
      Accessibility_Accessible_unref (e->source, ev);
    }
}

static long
_get_unique_id ()
{
  static long id = 0;
  return ++id;
}

static void
_registry_notify_listeners (GList *listeners,
                            const Accessibility_Event *e_in,
                            CORBA_Environment *ev)
{
  GList              *l;
  Accessibility_Event e_out;
  SpiListenerStruct  *ls;
  EventTypeStruct     etype;
  guint               minor_hash;
  CORBA_string        s;

  e_out = *e_in;
  parse_event_type (&etype, e_in->type);

  s = g_strconcat (etype.major, etype.minor, NULL);
  minor_hash = g_str_hash (s);
  g_free (s);

  for (l = listeners; l; l = l->next)
    {
      ls = (SpiListenerStruct *) l->data;

#ifdef SPI_SPI_LISTENER_DEBUG
      fprintf (stderr, "event hashes: %lx %lx %lx\n", ls->event_type_hash, etype.hash, minor_hash);
      fprintf (stderr, "event name: %s\n", etype.event_name);
#endif

      if ((ls->event_type_hash == etype.hash) || (ls->event_type_hash == minor_hash))
        {
#ifdef SPI_DEBUG
          fprintf (stderr, "notifying listener %d\n", g_list_index (listeners, l->data));
          s = Accessibility_Accessible__get_name (e_in->source, ev);
	  fprintf (stderr, "event source name %s\n", s);
	  CORBA_free (s);
#endif
	  e_out.source = bonobo_object_dup_ref (e_in->source, ev);
          Accessibility_EventListener_notifyEvent ((Accessibility_EventListener) ls->listener,
                                                   &e_out,
                                                   ev);
          if (ev->_major != CORBA_NO_EXCEPTION)
            {
              g_warning ("Accessibility app error: exception during event notification: %s\n",
		       CORBA_exception_id (ev));
	    }
        }
    }
}

static gboolean
_device_event_controller_hook (gpointer p)
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
        POA_Accessibility_Registry__epv *epv = &klass->epv;

        spi_registry_parent_class = g_type_class_ref (SPI_LISTENER_TYPE);

        object_class->finalize = spi_registry_object_finalize;

        epv->registerApplication = impl_accessibility_registry_register_application;
        epv->deregisterApplication = impl_accessibility_registry_deregister_application;
        epv->registerGlobalEventListener = impl_accessibility_registry_register_global_event_listener;
        epv->deregisterGlobalEventListener = impl_accessibility_registry_deregister_global_event_listener;
        epv->deregisterGlobalEventListenerAll = impl_accessibility_registry_deregister_global_event_listener_all;
        epv->getDeviceEventController = impl_accessibility_registry_get_device_event_controller;
        epv->getDesktopCount = impl_accessibility_registry_get_desktop_count;
        epv->getDesktop = impl_accessibility_registry_get_desktop;
        epv->getDesktopList = impl_accessibility_registry_get_desktop_list;

        ((SpiListenerClass *) klass)->epv.notifyEvent = impl_registry_notify_event;
}

static void
spi_registry_init (SpiRegistry *registry)
{
  registry->object_listeners = NULL;
  registry->window_listeners = NULL;
  registry->toolkit_listeners = NULL;
  registry->desktop = spi_desktop_new();
  registry->device_event_controller = NULL;
  registry->kbd_event_hook = _device_event_controller_hook;
}

BONOBO_TYPE_FUNC_FULL (SpiRegistry,
		       Accessibility_Registry,
		       PARENT_TYPE,
		       spi_registry);

SpiRegistry *
spi_registry_new (void)
{
    SpiRegistry *retval = g_object_new (SPI_REGISTRY_TYPE, NULL);
    bonobo_object_set_immortal (BONOBO_OBJECT (retval), TRUE);
    return retval;
}
