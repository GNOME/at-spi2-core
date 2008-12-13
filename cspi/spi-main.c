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

/*
 *
 * Basic SPI initialization and event loop function prototypes
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cspi/spi-private.h>
#include "spi.h"

#undef DEBUG_OBJECTS

static DBusConnection *bus = NULL;
static GHashTable *apps = NULL;
static GHashTable *live_refs = NULL;
static GQueue *exception_handlers = NULL;
static DBusError exception;

static void
cspi_object_release (gpointer value)
{
}

gboolean
cspi_exception_throw (DBusError *error, const char *desc_prefix)
{
  SPIExceptionHandler *handler = NULL;
  SPIException ex;
  if (exception_handlers) handler = g_queue_peek_head (exception_handlers);

  ex.type = SPI_EXCEPTION_SOURCE_UNSPECIFIED;
  ex.source = NULL;
  ex.code = SPI_EXCEPTION_UNSPECIFIED;
  ex.error = error;
  // TODO: Fill in description
  
  if (handler)
    return (*handler) (&ex, FALSE);
  else
    return FALSE; /* means exception was not handled */
}

const char *spi_path_dec = SPI_DBUS_PATH_DEC;
const char *spi_path_registry = SPI_DBUS_PATH_REGISTRY;
const char *spi_bus_registry = SPI_DBUS_NAME_REGISTRY;
const char *spi_path_desktop = SPI_DBUS_PATH_DESKTOP;
const char *spi_interface_accessible = SPI_DBUS_INTERFACE_ACCESSIBLE;
const char *spi_interface_action = SPI_DBUS_INTERFACE_ACTION;
const char *spi_interface_application = SPI_DBUS_INTERFACE_APPLICATION;
const char *spi_interface_component = SPI_DBUS_INTERFACE_COMPONENT;
const char *spi_interface_dec = SPI_DBUS_INTERFACE_DEC;
const char *spi_interface_desktop = SPI_DBUS_INTERFACE_DESKTOP;
const char *spi_interface_device_event_listener = SPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER;
const char *spi_interface_document = SPI_DBUS_INTERFACE_DOCUMENT;
const char *spi_interface_editable_text = SPI_DBUS_INTERFACE_EDITABLE_TEXT;
const char *spi_interface_hyperlink = SPI_DBUS_INTERFACE_HYPERLINK;
const char *spi_interface_hypertext = SPI_DBUS_INTERFACE_HYPERTEXT;
const char *spi_interface_image = SPI_DBUS_INTERFACE_IMAGE;
const char *spi_interface_registry = SPI_DBUS_INTERFACE_REGISTRY;
const char *spi_interface_selection = SPI_DBUS_INTERFACE_SELECTION;
const char *spi_interface_table = SPI_DBUS_INTERFACE_TABLE;
const char *spi_interface_text = SPI_DBUS_INTERFACE_TEXT;
const char *spi_interface_tree = SPI_DBUS_INTERFACE_TREE;
const char *spi_interface_value = SPI_DBUS_INTERFACE_VALUE;

static const char *interfaces[] =
{
  SPI_DBUS_INTERFACE_ACCESSIBLE,
  SPI_DBUS_INTERFACE_ACTION,
  SPI_DBUS_INTERFACE_APPLICATION,
  SPI_DBUS_INTERFACE_COLLECTION,
  SPI_DBUS_INTERFACE_COMPONENT,
  SPI_DBUS_INTERFACE_DOCUMENT,
  SPI_DBUS_INTERFACE_EDITABLE_TEXT,
  SPI_DBUS_INTERFACE_HYPERLINK,
  SPI_DBUS_INTERFACE_HYPERTEXT,
  SPI_DBUS_INTERFACE_IMAGE,
  "org.freedesktop.atspi.LoginHelper",
  SPI_DBUS_INTERFACE_SELECTION,
  "org.freedesktop.atspi.Selector",
  SPI_DBUS_INTERFACE_TABLE,
  SPI_DBUS_INTERFACE_TEXT,
  SPI_DBUS_INTERFACE_VALUE,
  NULL
};

static gint get_iface_num (const char *iface)
{
  // TODO: Use a binary search or hash to improve performance
  int i;

  for (i = 0; interfaces[i]; i++)
  {
    if (!strcmp(iface, interfaces[i])) return i;
  }
  return -1;
}

SPIBoolean
cspi_accessible_is_a (Accessible *accessible,
		      const char *interface_name)
{
  int n;

  if (accessible == NULL)
    {
      return FALSE;
    }

  n = get_iface_num (interface_name);
  if (n == -1) return FALSE;
  return (SPIBoolean)((accessible->interfaces & (1 << n))? TRUE: FALSE);
}

static GHashTable *
cspi_get_live_refs (void)
{
  if (!live_refs) 
    {
      live_refs = g_hash_table_new (g_direct_hash, g_direct_equal);
    }
  return live_refs;
}

DBusConnection *
SPI_bus (void)
{
  return bus;
}

SPIBoolean
cspi_exception (void)
{
  if (dbus_error_is_set (&exception))
  {
    dbus_error_free (&exception);
    return TRUE;
  }
  return FALSE;
}

SPIBoolean
cspi_check_ev (const char *error_string)
{
  if (dbus_error_is_set (&exception))
  {
    cspi_exception_throw (&exception, error_string);
    return FALSE;
  }
  return TRUE;
}

Accessible *
cspi_object_add (Accessible *accessible)
{
  if (accessible) cspi_object_ref (accessible);
  return accessible;
}

void
cspi_object_ref (Accessible *accessible)
{
  g_return_if_fail (accessible != NULL);

  accessible->ref_count++;
  g_hash_table_insert (live_refs, accessible, accessible);
}

#define APP_IS_REGISTRY(app) (!strcmp (app->bus_name, spi_bus_registry))

static void
cspi_object_unref_internal (Accessible *accessible, gboolean defunct)
{
  gboolean cached;

  if (accessible == NULL)
    {
      return;
    }

  if (--accessible->ref_count == 0 || (accessible->ref_count == 1 && !defunct) && g_hash_table_lookup (live_refs, accessible))
  {
    AccessibleEvent e;
    memset (&e, 0, sizeof(e));
    e.type = "object:state-change:defunct";
    e.source = accessible;
    e.detail1 = 1;
    cspi_dispatch_event (&e);
    g_hash_table_remove (live_refs, accessible);
  }
  if (accessible->ref_count == 0)
  {
    if (APP_IS_REGISTRY (accessible->app))
    {
      g_free (accessible->v.path);
    }
    spi_state_set_cache_unref (accessible->states);
    g_free (accessible->description);
    g_free (accessible->name);
    g_free(accessible);
  }
}

void
cspi_object_unref (Accessible *accessible)
{
  cspi_object_unref_internal (accessible, FALSE);
}

static void
cspi_cleanup (void)
{
  GHashTable *refs;

  cspi_streams_close_all ();

  refs = live_refs;
  live_refs = NULL;
  if (refs)
    {
      g_hash_table_destroy (refs);
    }
}

static gboolean SPI_inited = FALSE;

static GHashTable *app_hash = NULL;

static Accessible *
ref_accessible (CSpiApplication *app, const char *path)
{
  int id;
  guint *id_val;

  if (sscanf (path, "/org/freedesktop/atspi/accessible/%d", &id) != 1)
  {
    return NULL;
  }
  Accessible *a = g_hash_table_lookup (app->hash, &id);
  if (a)
  {
    cspi_object_ref (a);
    return a;
  }
  id_val = g_new (guint, 1);
  if (!id_val) return NULL;
  *id_val = id;
  a = g_new0 (Accessible, 1);
  if (!a)
  {
    g_free (id_val);
    return NULL;
  }
  g_hash_table_insert (app->hash, id_val, a);
  a->app = app;
  a->v.id = id;
  a->ref_count = 2;	/* one for the caller, one for the hash */
  return a;
}

static CSpiApplication *
cspi_get_application (const char *bus_name)
{
  CSpiApplication *app = NULL;
  char *bus_name_dup;

  if (!app_hash)
  {
    app_hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GDestroyNotify)g_hash_table_unref);
    if (!app_hash) return;
  }
  app = g_hash_table_lookup (app_hash, bus_name);
  if (app) return app;
  bus_name_dup = g_strdup (bus_name);
  if (!bus_name_dup) return NULL;
  // TODO: change below to something that will send state-change:defunct notification if necessary */
  app = g_new (CSpiApplication, 1);
  if (!app) return NULL;
  app->bus_name = bus_name_dup;
  if (APP_IS_REGISTRY (app))
  {
    app->hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, cspi_object_unref);
  }
  else
  {
    app->hash = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, cspi_object_unref);
  }
  g_hash_table_insert (app_hash, bus_name_dup, app);
  return app;
}

typedef struct
{
  char *path;
  char *parent;
  GArray *children;
  GArray *interfaces;
  char *name;
  dbus_uint32_t role;
  char *description;
  GArray *state_bitflags;
} CACHE_ADDITION;

/* Update the cache with added/modified objects and free the array */
static void
handle_additions (CSpiApplication*app, GArray *additions)
{
  gint i, j;
  GList *l, *new_list;

  if (!additions)
  {
    return;
  }
  for (i = 0; i < additions->len; i++)
  {
    CACHE_ADDITION *ca = &g_array_index (additions, CACHE_ADDITION, i);
    Accessible *a = ref_accessible (app, ca->path);
      /* Note: children don't hold refs for their parents or vice versa */
    a->parent = ref_accessible (app, ca->parent);
    if (a->parent) cspi_object_unref (a->parent);
    if (a->children)
    {
      g_list_free (a->children);
      a->children = NULL;
    }
    for (j = 0; j < ca->children->len; j++)
    {
      const char *child_path = g_array_index (ca->children, const char *, j);
      Accessible *child = ref_accessible (app, child_path);
      new_list = g_list_append (a->children, child);
      if (new_list) a->children = new_list;
      cspi_object_unref (child);
    }
    a->interfaces = 0;
    for (j = 0; j < ca->interfaces->len; j++)
    {
      const char *iface = g_array_index (ca->interfaces, const char *, j);
      if (!strcmp (iface, "org.freedesktop.DBus.Introspectable")) continue;
      gint n = get_iface_num (iface);
      if (n == -1)
      {
	g_warning ("Unknown interface %s", iface);
      }
      else a->interfaces |= (1 << n);
      g_free (iface);
    }
    if (a->name) g_free (a->name);
    a->name = ca->name;
    a->role = ca->role;
    if (a->description) g_free (a->description);
    a->description = ca->description;
    a->states = spi_state_set_cache_new (ca->state_bitflags);
    g_array_free (ca->interfaces, TRUE);
    g_array_free (ca->children, TRUE);
    /* spi_state_set_cache_new frees state_bitflags */
    /* This is a bit of a hack since ref_accessible sets ref_count to 2
     * for a new object, one of the refs being for the cache */
    cspi_object_unref (a);
  }
  g_array_free (additions, TRUE);
}

static void
handle_removals (CSpiApplication *app, GArray *removals)
{
  gint j;

  if (!removals) return;
  for (j = 0; j < removals->len; j++)
  {
    const char *path = g_array_index (removals, const char *, j);
    Accessible *a = ref_accessible (app, path);
    if (a->parent && g_list_find (a->parent->children, a))
    {
      a->parent->children = g_list_remove (a->parent->children, a);
      /* Note: children don't hold refs for their parents or vice versa */
    }
    g_hash_table_remove (app->hash, &a->v.id);
    cspi_object_unref_internal (a, TRUE);	/* unref our own ref */
  }
  g_array_free (removals, TRUE);
}

static gboolean
add_app_to_desktop (Accessible *a, const char *bus_name)
{
  DBusError error;
  char *root_path;

  dbus_error_init (&error);
  if (dbind_connection_method_call (bus, bus_name, "/org/freedesktop/atspi/tree", spi_interface_tree, "getRoot", &error, "=>o", &root_path))
  {
    Accessible *obj = cspi_ref_accessible (bus_name, root_path);
    if (obj)
    {
      GList *new_list = g_list_append (a->children, obj);
      if (new_list)
      {
	a->children = new_list;
	return TRUE;
      }
    }
    g_free (root_path);
  }
  else
  {
    g_warning ("Error calling getRoot for %s: %s", bus_name, error.message);
  }
  return FALSE;
}

static void
send_children_changed (Accessible *parent, Accessible *child, gboolean add)
{
  AccessibleEvent e;

  memset (&e, 0, sizeof(e));
  e.type = (add? "object:children-changed:add": "object:children-changed:remove");
  e.source = parent;
  e.detail1 = g_list_index (parent->children, child);
  cspi_dispatch_event (&e);
}

static void
unref_object_and_children (Accessible *obj)
{
  GList *l;

  for (l = obj->children; l; l = l->next)
  {
    unref_object_and_children (l->data);
  }
  cspi_object_unref_internal (obj, TRUE);
}

static gboolean
remove_app_from_desktop (Accessible *a, const char *bus_name)
{
  GList *l;
  Accessible *child;

  for (l = a->children; l; l = l->next)
  {
    child = l->data;
    if (!strcmp (bus_name, child->app->bus_name)) break;
  }
  if (!l)
  {
    g_warning ("Removing unregistered app %s; doing nothing\n", bus_name);
    return FALSE;
  }
  send_children_changed (a, child, FALSE);
  a->children = g_list_remove (a->children, child);
  unref_object_and_children (child);
  return TRUE;
}

static Accessible *desktop;

static Accessible *
ref_accessible_desktop (CSpiApplication *app)
{
  DBusError error;
  GArray *apps = NULL;
  GArray *additions;
  gint i;

  if (desktop)
  {
    cspi_object_ref (desktop);
    return desktop;
  }
  desktop = g_new0 (Accessible, 1);
  if (!desktop)
  {
    return NULL;
  }
  g_hash_table_insert (app->hash, "", desktop);
  desktop->app = app;
  desktop->ref_count = 2;	/* one for the caller, one for the hash */
  desktop->name = g_strdup ("");
  dbus_error_init (&error);
  if (!dbind_connection_method_call (bus, spi_bus_registry, spi_path_registry, spi_interface_registry, "getApplications", &error, "=>as", &apps))
  {
    g_error ("Couldn't get application list: %s", error.message);
  }
  for (i = 0; i < apps->len; i++)
  {
    const char *app_name = g_array_index (apps, char *, i);
    if (app_name[0] == '\0')
    {
      g_warning ("Got empty app name");
      continue;
    }
    CSpiApplication *app = cspi_get_application (app_name);
    additions = NULL;
    dbus_error_init (&error);
    dbind_connection_method_call (bus, app_name, "/org/freedesktop/atspi/tree", spi_interface_tree, "getTree", &error, "=>a(ooaoassusau)", &additions);
    if (error.message)
    {
      g_warning ("getTree (%s): %s", app_name, error.message);
    }
    handle_additions (app, additions);
    add_app_to_desktop (desktop, app_name);
  }
  g_array_free (apps, TRUE);
  return desktop;
}

Accessible *
cspi_ref_accessible (const char *app, const char *path)
{
  CSpiApplication *a = cspi_get_application (app);
  if (!a) return NULL;
  if ( APP_IS_REGISTRY(a))
  {
    return ref_accessible_desktop (a);
  }
  return ref_accessible (a, path);
}

Accessible *
cspi_ref_related_accessible (Accessible *obj, const char *path)
{
  return ref_accessible (obj->app, path);
}

typedef struct
{
  GArray *additions;
  GArray *removals;
} CacheSignalData;

static const char *cacheSignalType = "a(ooaoassusau)ao";

static DBusHandlerResult
cspi_dbus_handle_update_tree (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessageIter iter;
  CacheSignalData cd;
  void *p = &cd;
  const char *sender = dbus_message_get_sender (message);
  CSpiApplication *app = cspi_get_application (sender);
  const char *type = cacheSignalType;

  if (!app)
  {
    g_warning ("UpdateTree from unknown app.  Should we add it?", sender);
    return;
  }
  dbus_message_iter_init (message, &iter);
  // TODO: Check signature
  dbind_any_demarshal (&iter, &type, &p);	/* additions */
  dbind_any_demarshal (&iter, &type, &p);	/* removals */
  handle_additions (app, cd.additions);
  handle_removals (app, cd.removals);
}

static DBusHandlerResult
cspi_dbus_handle_register_application (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusError error;
  dbus_uint32_t v;
  Accessible *a;
  char *bus_name;

  dbus_error_init (&error);
  a = cspi_ref_accessible (spi_bus_registry, spi_path_registry);
  if (add_app_to_desktop (a, dbus_message_get_sender (message)))
  {
    send_children_changed (a, g_list_last (a->children)->data, TRUE);
  }
  cspi_object_unref (a);
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
cspi_dbus_handle_deregister_application (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  Accessible *a;
  DBusError error;
  char *bus_name;
 
  dbus_error_init (&error);
  if (!dbus_message_get_args (message, &error, DBUS_TYPE_STRING, &bus_name, DBUS_TYPE_INVALID))
  {
    g_warning ("Error processing %s: %s\n", dbus_message_get_member(message), error.message);
    dbus_error_free (&error);
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  a = cspi_ref_accessible (spi_bus_registry, spi_path_registry);
  remove_app_from_desktop (a, bus_name);
  cspi_object_unref (a);
  return DBUS_HANDLER_RESULT_HANDLED;
}


static DBusHandlerResult
cspi_dbus_filter (DBusConnection *bus, DBusMessage *message, void *data)
{
  int type = dbus_message_get_type (message);
  const char *interface = dbus_message_get_interface (message);
  const char *member = dbus_message_get_member (message); 
  dbus_uint32_t v;
  char *bus_name;

  if (type == DBUS_MESSAGE_TYPE_SIGNAL &&
      !strcmp (interface, SPI_DBUS_INTERFACE_ACCESSIBLE))
  {
    return cspi_dbus_handle_event (bus, message, data);
  }
  if (dbus_message_is_method_call (message, spi_interface_device_event_listener, "notifyEvent"))
  {
    return cspi_dbus_handle_deviceEvent (bus, message, data);
  }
  if (dbus_message_is_signal (message, spi_interface_tree, "updateTree"))
  {
    return cspi_dbus_handle_update_tree (bus, message, data);
  }
  if (dbus_message_is_signal (message, spi_interface_registry, "registerApplication"))
  {
    return cspi_dbus_handle_register_application (bus, message, data);
  }
  if (dbus_message_is_signal (message, spi_interface_registry, "deregisterApplication"))
  {
    return cspi_dbus_handle_deregister_application (bus, message, data);
  }
  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

/**
 * SPI_init:
 *
 * Connects to the accessibility registry and initializes the SPI.
 *
 * Returns: 0 on success, otherwise an integer error code.  
 **/
int
SPI_init (void)
{
  DBusError error;
  char *match;

  if (SPI_inited)
    {
      return 1;
    }

  SPI_inited = TRUE;

  g_type_init ();

  cspi_get_live_refs();
  g_atexit (cspi_cleanup);

  dbus_error_init (&error);
  bus = dbus_bus_get (DBUS_BUS_SESSION, &error);
  if (!bus)
  {
    g_error ("Couldn't get session bus");
    return 2;
  }
  dbus_bus_register (bus, &error);
  dbus_connection_setup_with_g_main(bus, g_main_context_default());
  dbus_connection_add_filter (bus, cspi_dbus_filter, NULL, NULL);
  match = g_strdup_printf ("type='signal',interface='%s',member='updateTree'", spi_interface_tree);
  dbus_error_init (&error);
  dbus_bus_add_match (bus, match, &error);
  g_free (match);
  match = g_strdup_printf ("type='signal',interface='%s'", spi_interface_registry);
  dbus_bus_add_match (bus, match, &error);
  g_free (match);
  return 0;
}

  static GMainLoop *mainloop;

/**
 * SPI_event_main:
 *
 * Starts/enters the main event loop for the SPI services.
 *
 * (NOTE: This method does not return control, it is exited via a call to
 *  SPI_event_quit () from within an event handler).
 *
 **/
void
SPI_event_main (void)
{

  mainloop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (mainloop);
}

/**
 * SPI_event_quit:
 *
 * Quits the last main event loop for the SPI services,
 * see SPI_event_main
 **/
void
SPI_event_quit (void)
{
  g_main_loop_quit (mainloop);
}

/**
 * SPI_eventIsReady:
 *
 * Checks to see if an SPI event is waiting in the event queue.
 * Used by clients that don't wish to use SPI_event_main().
 *
 * Not Yet Implemented.
 *
 * Returns: #TRUE if an event is waiting, otherwise #FALSE.
 *
 **/
SPIBoolean
SPI_eventIsReady (void)
{
  return FALSE;
}

/**
 * SPI_nextEvent:
 * @waitForEvent: a #SPIBoolean indicating whether to block or not.
 *
 * Gets the next event in the SPI event queue; blocks if no event
 * is pending and @waitForEvent is #TRUE.
 * Used by clients that don't wish to use SPI_event_main().
 *
 * Not Yet Implemented.
 *
 * Returns: the next #AccessibleEvent in the SPI event queue.
 **/
AccessibleEvent *
SPI_nextEvent (SPIBoolean waitForEvent)
{
  return NULL;
}

#ifdef PRINT_LEAKS
static void
report_leaked_ref (gpointer key, gpointer val, gpointer user_data)
{
  char *name, *role;
  Accessible *a = (Accessible *) val;
  
  name = Accessible_getName (a);
  if (cspi_exception ())
    {
      name = NULL;
    }

  role = Accessible_getRoleName (a);
  if (cspi_exception ())
    {
      role = NULL;
    }

  fprintf (stderr, "leaked %d references to object %s, role %s %p\n",
	   a->ref_count, name ? name : "<?>", role ? role : "<?>", a);

  SPI_freeString (name);
}
#endif

/**
 * SPI_exit:
 *
 * Disconnects from the Accessibility Registry and releases 
 * any floating resources. Call only once at exit.
 *
 * Returns: 0 if there were no leaks, otherwise non zero.
 **/
int
SPI_exit (void)
{
  int leaked;

  if (!SPI_inited)
    {
      return 0;
    }

  SPI_inited = FALSE;

  if (live_refs)
    {
      leaked = g_hash_table_size (live_refs);
#ifdef DEBUG_OBJECTS
      fprintf (stderr, "Leaked %d SPI handles\n", leaked);

#define PRINT_LEAKS
#ifdef PRINT_LEAKS
      g_hash_table_foreach (live_refs, report_leaked_ref, NULL);
#endif

#endif
    }
  else
    {
      leaked = 0;
    }

  cspi_cleanup ();

  return leaked;
}

/**
 * SPI_freeString:
 * @s: a character string returned from another at-spi call.
 *
 * Free a character string returned from an at-spi call.  Clients of
 * at-spi should use this function instead of free () or g_free().
 * A NULL string @s will be silently ignored.
 * This API should not be used to free strings
 * from other libraries or allocated by the client.
 **/
void
SPI_freeString (char *s)
{
  if (s)
    {
      g_free (s);
    }
}

/**
 * SPI_freeRect:
 * @r: a pointer to an SPIRect returned from another at-spi call.
 *
 * Free a SPIRect structure returned from an at-spi call.  Clients of
 * at-spi should use this function instead of free () or g_free().
 * A NULL rect @r will be silently ignored.
 * This API should not be used to free data
 * from other libraries or allocated by the client.
 *
 * @Since: AT-SPI 1.6
 **/
void
SPI_freeRect (SPIRect *r)
{
  if (r)
    {
      /* err, okay, in this case the client _could_ 
	 have called g_free, but we don't want to guarantee it */
      g_free (r);
    }
}

/**
 * SPI_dupString:
 * @s: a UTF-8 string to be duplicated
 * 
 * @Since: AT-SPI 1.4
 *
 * Returns: a duplicate of the string passed as a parameter, which should
 * be freed via SPI_freeString after use.
 **/
char *
SPI_dupString (char *s)
{
  if (s)
    {
      return g_strdup (s);
    }
  else 
    return NULL;
}

/**
 * SPI_exceptionHandlerPush:
 * @handler: an #SPIExceptionHandler to install as the first code to deal with exceptions.
 *
 * Install a client-side handler for #SPIException instances, which can see and handle any
 * exceptions before chaining them to the next exception handler in the stack.
 *
 * @Since: AT-SPI 1.4
 *
 * Returns %TRUE if the result succeeded, %FALSE if @hander could not be registered.
 **/
SPIBoolean SPI_exceptionHandlerPush (SPIExceptionHandler *handler)
{
  if (!exception_handlers)
    exception_handlers = g_queue_new ();
  g_queue_push_head (exception_handlers, handler);
  return TRUE;
}

/**
 * SPI_exceptionHandlerPop:
 * 
 * Remove/pop an #SPIExceptionHandler off the error handler stack and return the new handler.
 *
 * @Since: AT-SPI 1.4
 *
 * Returns the #SPIExceptionHandler which is now at the top of the error handler stack after the call.
 **/
SPIExceptionHandler* SPI_exceptionHandlerPop (void)
{
  return (SPIExceptionHandler *) g_queue_pop_head (exception_handlers);
}

/**
 * SPIException_getSourceType:
 * @err: the exception being queried
 *
 * Get the #SPIExceptionType of an exception which has been thrown.
 *
 * @Since: AT-SPI 1.4
 *
 * Returns: the #SPIExceptionType corresponding to exception @err.
 **/
SPIExceptionType SPIException_getSourceType (SPIException *err)
{
  if (err)
    return err->type;
  else
    return SPI_EXCEPTION_SOURCE_UNSPECIFIED;
}

/**
 * SPIException_getExceptionCode:
 * @err: the #SPIException being queried.
 *
 * Get the #SPIExceptionCode telling what type of exception condition has occurred.
 *
 * @Since: AT-SPI 1.4
 *
 * Returns: the #SPIExceptionCode corresponding to exception @err.
 **/
SPIExceptionCode SPIException_getExceptionCode (SPIException *err)
{  
  return err->code;
}

/**
 * SPIAccessibleException_getSource:
 * @err: the #SPIException being queried.
 *
 * Get the identity of the object which threw an exception.
 *
 * @Since: AT-SPI 1.4
 *
 * Returns: a pointer to the #Accessible object which threw the exception.
 **/
Accessible* SPIAccessibleException_getSource (SPIException *err)
{
  if (err->type == SPI_EXCEPTION_SOURCE_ACCESSIBLE)
    cspi_object_ref (err->source);
    return err->source;
  return NULL;
}

/**
 * SPIException_getDescription:
 * @err: the #SPIException being queried.
 *
 * Get a text description of the exception that has been thrown.
 * Unfortunately these descriptions tend to be terse and limited in
 * the detail which they can provide.
 *
 * Returns: a brief character string describing the exception.
 **/
char* SPIException_getDescription (SPIException *err)
{
  /* TODO: friendlier error messages? */
  if (err->error)
    return err->error->message;
  return NULL;
}

static char *
get_path (Accessible *obj)
{
  if (APP_IS_REGISTRY (obj->app))
  {
    return g_strdup_printf (SPI_DBUS_PATH_REGISTRY);
  }
  return g_strdup_printf ("/org/freedesktop/atspi/accessible/%d", obj->v.id);
}

dbus_bool_t
cspi_dbus_call (Accessible *obj, const char *interface, const char *method, DBusError *error, const char *type, ...)
{
  va_list args;
  char *path = get_path (obj);
  dbus_bool_t retval;
  DBusError err;

  if (!error) error = &err;
  dbus_error_init (error);
  va_start (args, type);
  retval = dbind_connection_method_call_va (SPI_bus(), obj->app->bus_name, path, interface, method, error, type, args);
  va_end (args);
  g_free (path);
  if (dbus_error_is_set (error))
  {
    if (!dbus_error_is_set (&exception))
    {
      // TODO: Should we call cspi_exception_throw?
      dbus_move_error (error, &exception);
    }
    else if (error == &err) dbus_error_free (error);
  }
  return retval;
}

dbus_bool_t
cspi_dbus_get_property (Accessible *obj, const char *interface, const char *name, DBusError *error, const char *type, void *data)
{
  DBusMessage *message, *reply;
  char *path = get_path (obj);
  DBusMessageIter iter, iter_variant;
  DBusError err;
  dbus_bool_t retval = FALSE;

  message = dbus_message_new_method_call (obj->app->bus_name, path, "org.freedesktop.DBus.Properties", "Get");
  if (!message)
  {
    // TODO: throw exception
    goto done;
  }
  dbus_message_append_args (message, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  if (!error) error = &err;
  dbus_error_init (error);
  reply = dbus_connection_send_with_reply_and_block (SPI_bus(), message, 1000, error);
  dbus_message_unref (message);
  if (!reply)
  {
    // TODO: throw exception
    goto done;
  }
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_variant);
  if (dbus_message_iter_get_arg_type (&iter_variant) != type[0])
  {
    g_warning ("cspi_dbus_get_property: Wrong type: expected %s, got %c\n", type, dbus_message_iter_get_arg_type (&iter_variant));
    goto done;
  }
  dbus_message_iter_get_basic (&iter_variant, data);
  dbus_message_unref (reply);
  if (type[0] == 's') *(char **)data = g_strdup (*(char **)data);
  retval = TRUE;
done:
  g_free (path);
  return retval;
}
