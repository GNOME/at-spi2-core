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

/* registry.c: the main accessibility service registry implementation */

#undef SPI_LISTENER_DEBUG
#undef SPI_DEBUG
#undef SPI_QUEUE_DEBUG

#include <config.h>
#ifdef SPI_DEBUG
#  include <stdio.h>
#endif

#include <bonobo/bonobo-exception.h>
#include "../libspi/spi-private.h"
#include "registry.h"

/* Our parent GObject type  */
#define PARENT_TYPE SPI_LISTENER_TYPE

/* A pointer to our parent object class */
static SpiListenerClass *spi_registry_parent_class;

static GQuark _deactivate_quark = 0;
static GQuark _activate_quark = 0;
static GQuark _state_quark = 0;

int _dbg = 0;

typedef enum {
  ETYPE_FOCUS,
  ETYPE_OBJECT,
  ETYPE_PROPERTY,
  ETYPE_WINDOW,
  ETYPE_TOOLKIT,
  ETYPE_KEYBOARD,
  ETYPE_MOUSE,
  ETYPE_LAST_DEFINED
} EventTypeCategory;

typedef struct {
  const char *event_name;
  EventTypeCategory type_cat;
  GQuark major;  /* from string segment[1] */
  GQuark minor;  /* from string segment[1]+segment[2] */
  GQuark detail; /* from string segment[3] (not concatenated) */
} EventTypeStruct;

typedef struct {
  Accessibility_EventListener listener;
  GQuark            event_type_quark;
  EventTypeCategory event_type_cat;
} SpiListenerStruct;

static void
spi_registry_set_debug (const char *debug_flag_string)
{
  if (debug_flag_string) 
    _dbg = (int) g_ascii_strtod (debug_flag_string, NULL);
}

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

static void
desktop_add_application (SpiDesktop *desktop,
			 guint index, gpointer data)
{
  BonoboObject *registry = BONOBO_OBJECT (data);
  Accessibility_Event e;
  CORBA_Environment ev;
  Accessibility_Accessible a;
  
  CORBA_exception_init (&ev);
  e.type = "object:children-changed:add";
  e.source = BONOBO_OBJREF (desktop);
  e.detail1 = index;
  e.detail2 = 0;
  a = Accessibility_Accessible_getChildAtIndex (BONOBO_OBJREF (desktop), 
						index, &ev);
  /* FIXME
  spi_init_any_object (&e.any_data, a);
  */
  spi_init_any_nil (&e.any_data);
  Accessibility_Registry_notifyEvent (BONOBO_OBJREF (registry),
				      &e, &ev);
  bonobo_object_release_unref (a, &ev);
  CORBA_exception_free (&ev);
}



static void
desktop_remove_application (SpiDesktop *desktop,
			    guint index, gpointer data)
{
  BonoboObject *registry = BONOBO_OBJECT (data);
  Accessibility_Event e;
  Accessibility_Accessible a;
  CORBA_Environment ev;
  
  CORBA_exception_init (&ev);

  e.type = "object:children-changed:remove";
  e.source = BONOBO_OBJREF (desktop);
  e.detail1 = index;
  e.detail2 = 0;
  a = Accessibility_Accessible_getChildAtIndex (BONOBO_OBJREF (desktop), 
						index, &ev);
  /* FIXME
  spi_init_any_object (&e.any_data, a);
  */
  spi_init_any_nil (&e.any_data);
  Accessibility_Accessible_unref (a, &ev);
  Accessibility_Registry_notifyEvent (BONOBO_OBJREF (registry),
				      &e, &ev);
  Accessibility_Desktop_unref (e.source, &ev);
  CORBA_exception_free (&ev);
}


static void
spi_registry_object_finalize (GObject *object)
{
  DBG (1, g_warning ("spi_registry_object_finalize called\n"));

  /* TODO: unref deviceeventcontroller, which disconnects key listener */
  G_OBJECT_CLASS (spi_registry_parent_class)->finalize (object);
}

static long
_get_unique_id (void)
{
  static long id = 0;

  return ++id;
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

  Accessibility_Application__set_id (application, _get_unique_id (), ev);

  /*
   * TODO: change the implementation below to a WM-aware one;
   * e.g. don't add all apps to the SpiDesktop
   */
}

#ifdef USE_A_HASH_IN_FUTURE
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
#endif

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

#ifdef USE_A_HASH_IN_FUTURE

static gint
compare_listener_quarks (gconstpointer p1, gconstpointer p2)
{
	return (((SpiListenerStruct *)p2)->event_type_quark !=
		((SpiListenerStruct *)p1)->event_type_quark);
}

static gint
compare_listener_corbaref (gconstpointer p1, gconstpointer p2)
{
  return compare_corba_objects (((SpiListenerStruct *)p2)->listener,
                                ((SpiListenerStruct *)p1)->listener);
}
#endif

static void
parse_event_type (EventTypeStruct *etype, const char *event_name)
{
  gchar **split_string;
  gchar *s;

  split_string = g_strsplit (event_name, ":", 4);
  etype->event_name = event_name;

  if (!g_ascii_strncasecmp (event_name, "focus:", 6))
    {
      etype->type_cat = ETYPE_FOCUS;
    }
  else if (!g_ascii_strncasecmp (event_name, "mouse:", 6))
    {
      etype->type_cat = ETYPE_MOUSE;
    }
  else if (!g_ascii_strncasecmp (event_name, "object:", 7))
    {
      etype->type_cat = ETYPE_OBJECT;
    }
  else if (!g_ascii_strncasecmp (event_name, "window:", 7))
    {
      etype->type_cat = ETYPE_WINDOW;
    }
  else if (!g_ascii_strncasecmp (event_name, "keyboard:", 9))
    {
      etype->type_cat = ETYPE_KEYBOARD;
    }
  else
    {
      etype->type_cat = ETYPE_TOOLKIT;
    }

  if (split_string[1])
    {
      etype->major = g_quark_from_string (split_string[1]);
      if (split_string[2])
        {
          etype->minor = g_quark_from_string (s = g_strconcat (split_string[1], split_string[2], NULL));
          g_free (s);
          if (split_string[3])
            {
              etype->detail = g_quark_from_string (split_string[3]);
            }
          else
            {
	      etype->detail = g_quark_from_static_string ("");
            }
        }
      else
        {
	  etype->minor = etype->major;
	  etype->detail = g_quark_from_static_string (""); //etype->major;
        }
    }
  else
    {
      etype->major = g_quark_from_static_string ("");
      etype->minor = etype->major;
      etype->detail = etype->major;
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

static GList **
get_listener_list (SpiRegistry      *registry,
		   EventTypeCategory cat)
{
  GList **ret;
  
  switch (cat)
    {
      case ETYPE_OBJECT:
      case ETYPE_PROPERTY:
      case ETYPE_FOCUS:
      case ETYPE_KEYBOARD:
        ret = &registry->object_listeners;
	break;
      case ETYPE_WINDOW:
	ret = &registry->window_listeners;
	break;
      case ETYPE_MOUSE:
      case ETYPE_TOOLKIT:
	ret = &registry->toolkit_listeners;
	break;
      default:
        ret = NULL;
	break;
    }
  return ret;
}

/*
 * CORBA Accessibility::Registry::registerGlobalEventListener method implementation
 */
static void
impl_accessibility_registry_register_global_event_listener (
	PortableServer_Servant      servant,
	Accessibility_EventListener listener,
	const CORBA_char           *event_name,
	CORBA_Environment          *ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant)); 
  SpiListenerStruct *ls = spi_listener_struct_new (listener, ev);
  EventTypeStruct etype;
  GList          **list;

#ifdef SPI_LISTENER_DEBUG
  fprintf (stderr, "registering for events of type %s\n", event_name);
#endif

  /* parse, check major event type and add listener accordingly */
  parse_event_type (&etype, event_name);
  ls->event_type_quark = etype.minor;
  ls->event_type_cat = etype.type_cat;

  list = get_listener_list (registry, etype.type_cat);

  if (list)
    {
      *list = g_list_prepend (*list, ls);

      if (etype.type_cat == ETYPE_TOOLKIT)
        {
          register_with_toolkits (registry, &etype, ev);
	}
    }
  else
    {
      spi_listener_struct_free (ls, ev);
    }
}

static SpiReEntrantContinue
remove_listener_cb (GList * const *list, gpointer user_data)
{
  SpiListenerStruct *ls = (SpiListenerStruct *) (*list)->data;
  CORBA_Environment  ev;
  Accessibility_EventListener listener = user_data;

  CORBA_exception_init (&ev);
	
  if (CORBA_Object_is_equivalent (ls->listener, listener, &ev))
    {
       spi_re_entrant_list_delete_link (list);
       spi_listener_struct_free (ls, &ev);
    }

  CORBA_exception_free (&ev);

  return SPI_RE_ENTRANT_CONTINUE;
}

/*
 * CORBA Accessibility::Registry::deregisterGlobalEventListenerAll method implementation
 */
static void
impl_accessibility_registry_deregister_global_event_listener_all (
	PortableServer_Servant      servant,
	Accessibility_EventListener listener,
	CORBA_Environment          *ev)
{
  int i;
  GList **lists[3];
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));

  lists[0] = &registry->object_listeners;
  lists[1] = &registry->window_listeners;
  lists[2] = &registry->toolkit_listeners;

  for (i = 0; i < sizeof (lists) / sizeof (lists[0]); i++)
    {
      spi_re_entrant_list_foreach (lists [i], remove_listener_cb, listener);
    }
}


/*
 * CORBA Accessibility::Registry::deregisterGlobalEventListener method implementation
 */
static void
impl_accessibility_registry_deregister_global_event_listener (
	PortableServer_Servant      servant,
	Accessibility_EventListener listener,
	const CORBA_char           *event_name,
	CORBA_Environment          *ev)
{
  SpiRegistry    *registry;
  EventTypeStruct etype;

  registry = SPI_REGISTRY (bonobo_object_from_servant (servant));

  parse_event_type (&etype, (char *) event_name);

  spi_re_entrant_list_foreach (get_listener_list (registry, etype.type_cat),
				remove_listener_cb, listener);
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
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));
  Accessibility_DesktopSeq *desktops;

  desktops = Accessibility_DesktopSeq__alloc ();
  desktops->_length = desktops->_maximum = 1;
  desktops->_buffer = Accessibility_DesktopSeq_allocbuf (desktops->_length);
  desktops->_buffer [0] = bonobo_object_dup_ref (BONOBO_OBJREF (registry->desktop), ev);

  return desktops;
}


static Accessibility_DeviceEventController
impl_accessibility_registry_get_device_event_controller (PortableServer_Servant servant,
							 CORBA_Environment     *ev)
{
  SpiRegistry *registry = SPI_REGISTRY (bonobo_object_from_servant (servant));

  if (!registry->de_controller)
    {
      registry->de_controller = spi_device_event_controller_new (registry);
    }

  return bonobo_object_dup_ref (BONOBO_OBJREF (registry->de_controller), ev);
}

typedef struct {
  CORBA_Environment  *ev;
  Bonobo_Unknown      source;
  EventTypeStruct     etype;
  Accessibility_Event e_out;
} NotifyContext;

static SpiReEntrantContinue
notify_listeners_cb (GList * const *list, gpointer user_data)
{
  SpiListenerStruct *ls;
  NotifyContext     *ctx = user_data;

  ls = (*list)->data;

#ifdef SPI_LISTENER_DEBUG
  fprintf (stderr, "event quarks: %lx %lx %lx\n", ls->event_type_quark, ctx->etype.major, ctx->etype.minor);
  fprintf (stderr, "event name: %s\n", ctx->etype.event_name);
#endif
  if ((ls->event_type_quark == ctx->etype.major) ||
      (ls->event_type_quark == ctx->etype.minor))
    {
#ifdef SPI_DEBUG
      CORBA_string s;
      fprintf (stderr, "notifying listener %d\n", 0);
/* g_list_index (list, l->data)); */
      s = Accessibility_Accessible__get_name (ctx->source, ctx->ev);
      fprintf (stderr, "event source name %s\n", s);
      CORBA_free (s);
      if (BONOBO_EX (ctx->ev))
        {
	  CORBA_exception_free (ctx->ev);
          return SPI_RE_ENTRANT_CONTINUE;
	}
#endif
      
      ctx->e_out.source = ctx->source;
      
      if ((*list) && (*list)->data == ls)
        {
          Accessibility_EventListener_notifyEvent (
            (Accessibility_EventListener) ls->listener, &ctx->e_out, ctx->ev);
          if (ctx->ev->_major != CORBA_NO_EXCEPTION)
            {
              DBG (1, g_warning ("Accessibility app error: exception during "
		        "event notification: %s\n",
		        CORBA_exception_id (ctx->ev)));
	      CORBA_exception_free (ctx->ev);
	      /* FIXME: check that this item is removed from the list
	       * on system exception by a 'broken' listener */
	    }
	}
    }  

  return SPI_RE_ENTRANT_CONTINUE;
}

static void
registry_emit_event (SpiRegistry *registry, NotifyContext *ctx)
{
  GList       **list = get_listener_list (registry, ctx->etype.type_cat);

  if (list && *list)
    {
    
      spi_re_entrant_list_foreach (list, notify_listeners_cb, ctx);
    }
}

static NotifyContext*
registry_clone_notify_context (NotifyContext *ctx)
{
  NotifyContext *new_ctx = g_new0 (NotifyContext, 1);

  new_ctx->ev = NULL;
  new_ctx->source = bonobo_object_dup_ref (ctx->source, NULL); 
  new_ctx->etype.event_name = CORBA_string_dup (ctx->etype.event_name);
  new_ctx->etype.type_cat = ctx->etype.type_cat;
  new_ctx->etype.major = ctx->etype.major;
  new_ctx->etype.minor = ctx->etype.minor;
  new_ctx->etype.detail = ctx->etype.detail;
  new_ctx->e_out.type = CORBA_string_dup (ctx->e_out.type);
  new_ctx->e_out.source = ctx->e_out.source;
  new_ctx->e_out.detail1 = ctx->e_out.detail1;
  new_ctx->e_out.detail2 = ctx->e_out.detail2;
  CORBA_any__copy (&(new_ctx->e_out.any_data), &(ctx->e_out.any_data));
  return new_ctx;
}

static void
registry_flush_event_queue (SpiRegistry       *registry,
			    gboolean      discard,
			    CORBA_Environment *ev)
{
  NotifyContext *q_ctx;
  while (!g_queue_is_empty (registry->deferred_event_queue)) {
    q_ctx = g_queue_pop_tail (registry->deferred_event_queue);
#ifdef SPI_QUEUE_DEBUG
    fprintf (stderr, "%s! %s [n=%d] %p\n", (discard ? "discard" : "pop"), 
	     q_ctx->etype.event_name, 
	     (int) registry->deferred_event_queue->length, q_ctx);
#endif
    if (!discard)  {
      q_ctx->ev = ev;
      registry_emit_event (registry, q_ctx);
    }
    bonobo_object_release_unref (q_ctx->source, NULL);
    CORBA_free ((void *)q_ctx->etype.event_name);
    CORBA_free ((void *)q_ctx->e_out.type);
    g_free (q_ctx);
  }
  registry->is_queueing = FALSE;
}

static gboolean
registry_timeout_flush_queue (gpointer data)
{
  SpiRegistry *registry = data;
  CORBA_Environment ev;
#ifdef SPI_QUEUE_DEBUG
  fprintf (stderr, "timeout! flushing queue...\n");
#endif
  CORBA_exception_init (&ev);
  registry_flush_event_queue (registry, FALSE, &ev);
  return FALSE;
}

static gboolean
registry_discard_on_event (SpiRegistry *registry, NotifyContext *ctx)
{
  gboolean retval = FALSE;
  NotifyContext *q_ctx = g_queue_peek_tail (registry->deferred_event_queue);
  if ((q_ctx != NULL) &&
      (ctx->etype.type_cat == ETYPE_WINDOW) && 
      (ctx->etype.major == _activate_quark)) {
    if (CORBA_Object_is_equivalent (ctx->source, q_ctx->source, NULL)) {
      retval = TRUE;
    }
  }
  return retval;
}

static gboolean
registry_reset_on_event (SpiRegistry *registry, NotifyContext *ctx)
{
  return (ctx->etype.type_cat == ETYPE_WINDOW) ? TRUE : FALSE;
}

static void
registry_start_queue (SpiRegistry *registry)
{
    g_timeout_add_full (G_PRIORITY_HIGH_IDLE, 
			registry->exit_notify_timeout,
			registry_timeout_flush_queue, registry, 
			NULL);
    registry->is_queueing = 1;
}

static gboolean
registry_defer_on_event (SpiRegistry *registry, NotifyContext *ctx)
{
  gboolean defer = FALSE;
  if ((ctx->etype.type_cat == ETYPE_WINDOW) && 
      (ctx->etype.major == _deactivate_quark)) {
    defer = TRUE;
    registry_start_queue (registry);
  }
  /* defer all object:state-change events after a window:deactivate */
  else if ((ctx->etype.type_cat == ETYPE_OBJECT) && 
	   (ctx->etype.major == _state_quark)) {
    defer = TRUE;
  }
  return defer;
}

static void
registry_queue_event (SpiRegistry *registry, NotifyContext *ctx)
{
  NotifyContext *q_ctx = registry_clone_notify_context (ctx);
#ifdef SPI_QUEUE_DEBUG
    if (q_ctx->etype.type_cat != ETYPE_MOUSE)
      fprintf (stderr, "push! %s %p\n", q_ctx->etype.event_name, q_ctx);
#endif    
  g_queue_push_head (registry->deferred_event_queue, q_ctx);
}

/**
 * Dispose of event in one of several ways:
 * 1) discard;
 * 2) initiate queuing and push onto queue (below)
 * 3) push on existing queue to either pop on timeout or on subsequent event
 * 4) pass-through immediately
 * 5) pass-through, discarding queued events
 * 6) emit queued events and then pass through
 **/
static gboolean
registry_filter_event (SpiRegistry *registry, NotifyContext *ctx,
		       CORBA_Environment *ev)
{
  gboolean queue_is_empty = FALSE;
  g_assert (ctx != NULL);

  /* case #1 is not yet used */
  if (registry_defer_on_event (registry, ctx)) { /* #2, #3 */
    if (registry->is_queueing) {
      registry_queue_event (registry, ctx);
      return FALSE;
    }
    else { /* #4a */
      return TRUE;
    }
  } 
  else if (registry_reset_on_event (registry, ctx)) { /* #5, #6 */
    gboolean discard = registry_discard_on_event (registry, ctx);
#ifdef SPI_QUEUE_DEBUG
    fprintf (stderr, "event %s caused reset, discard=%d\n",
	     ctx->etype.event_name, (int) discard);
#endif    
    registry_flush_event_queue (registry, discard, ev);
    return (discard ? FALSE : TRUE);
  }
  else { /* #4b */
    return TRUE;
  }
}

static void
impl_registry_notify_event (PortableServer_Servant     servant,
                            const Accessibility_Event *e,
                            CORBA_Environment         *ev)
{
  SpiRegistry  *registry;
  NotifyContext ctx;
  static int level = 0;

  level++;
  registry = SPI_REGISTRY (bonobo_object_from_servant (servant));

  parse_event_type (&ctx.etype, e->type);

  ctx.ev = ev;
  ctx.e_out = *e;
  ctx.source = e->source;

  if (registry_filter_event (registry, &ctx, ev)) {
#ifdef SPI_QUEUE_DEBUG
    if (ctx.etype.type_cat != ETYPE_MOUSE)
{
      fprintf (stderr, "emit! %s level: %d\n", ctx.etype.event_name, level);
      fprintf (stderr, "emit! %p %p\n", ctx.e_out, ctx.e_out.type);
}
#endif    
    registry_emit_event (registry, &ctx);
  }
  level--;
}

static void
spi_registry_class_init (SpiRegistryClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Registry__epv *epv = &klass->epv;

  spi_registry_parent_class = g_type_class_ref (SPI_LISTENER_TYPE);
  
  object_class->finalize = spi_registry_object_finalize;

  klass->parent_class.epv.notifyEvent   = impl_registry_notify_event;
  
  epv->registerApplication              = impl_accessibility_registry_register_application;
  epv->deregisterApplication            = impl_accessibility_registry_deregister_application;
  epv->registerGlobalEventListener      = impl_accessibility_registry_register_global_event_listener;
  epv->deregisterGlobalEventListener    = impl_accessibility_registry_deregister_global_event_listener;
  epv->deregisterGlobalEventListenerAll = impl_accessibility_registry_deregister_global_event_listener_all;
  epv->getDeviceEventController         = impl_accessibility_registry_get_device_event_controller;
  epv->getDesktopCount                  = impl_accessibility_registry_get_desktop_count;
  epv->getDesktop                       = impl_accessibility_registry_get_desktop;
  epv->getDesktopList                   = impl_accessibility_registry_get_desktop_list;
  _deactivate_quark = g_quark_from_static_string ("deactivate");
  _activate_quark = g_quark_from_static_string ("activate");
  _state_quark = g_quark_from_static_string ("state-changed");
}

static void
spi_registry_init (SpiRegistry *registry)
{
  spi_registry_set_debug (g_getenv ("AT_SPI_DEBUG"));
  /*
   * TODO: fixme, this module makes the foolish assumptions that
   * registryd uses the same display as the apps, and that the
   * DISPLAY environment variable is set.
   */
  gdk_init (NULL, NULL);

  registry->object_listeners = NULL;
  registry->window_listeners = NULL;
  registry->toolkit_listeners = NULL;
  registry->deferred_event_queue = g_queue_new ();
  registry->exit_notify_timeout = 100;
  registry->desktop = spi_desktop_new ();
  /* Register callback notification for application addition and removal */
  g_signal_connect (G_OBJECT (registry->desktop),
		    "application_added",
		    G_CALLBACK (desktop_add_application),
		    registry);

  g_signal_connect (G_OBJECT (registry->desktop),
		    "application_removed",
		    G_CALLBACK (desktop_remove_application),
		    registry);

  registry->de_controller = spi_device_event_controller_new (registry);
}

BONOBO_TYPE_FUNC_FULL (SpiRegistry,
		       Accessibility_Registry,
		       PARENT_TYPE,
		       spi_registry)

SpiRegistry *
spi_registry_new (void)
{
  SpiRegistry *retval = g_object_new (SPI_REGISTRY_TYPE, NULL);
  bonobo_object_set_immortal (BONOBO_OBJECT (retval), TRUE);
  return retval;
}
