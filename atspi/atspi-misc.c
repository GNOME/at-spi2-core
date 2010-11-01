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

#include "atspi-private.h"

static DBusConnection *bus = NULL;
static GHashTable *apps = NULL;
static GHashTable *live_refs = NULL;
static GQueue *exception_handlers = NULL;
static DBusError exception;

const char *atspi_path_dec = ATSPI_DBUS_PATH_DEC;
const char *atspi_path_registry = ATSPI_DBUS_PATH_REGISTRY;
const char *atspi_path_root = ATSPI_DBUS_PATH_ROOT;
const char *atspi_bus_registry = ATSPI_DBUS_NAME_REGISTRY;
const char *atspi_interface_accessible = ATSPI_DBUS_INTERFACE_ACCESSIBLE;
const char *atspi_interface_action = ATSPI_DBUS_INTERFACE_ACTION;
const char *atspi_interface_application = ATSPI_DBUS_INTERFACE_APPLICATION;
const char *atspi_interface_component = ATSPI_DBUS_INTERFACE_COMPONENT;
const char *atspi_interface_dec = ATSPI_DBUS_INTERFACE_DEC;
const char *atspi_interface_device_event_listener = ATSPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER;
const char *atspi_interface_document = ATSPI_DBUS_INTERFACE_DOCUMENT;
const char *atspi_interface_editable_text = ATSPI_DBUS_INTERFACE_EDITABLE_TEXT;
const char *atspi_interface_hyperlink = ATSPI_DBUS_INTERFACE_HYPERLINK;
const char *atspi_interface_hypertext = ATSPI_DBUS_INTERFACE_HYPERTEXT;
const char *atspi_interface_image = ATSPI_DBUS_INTERFACE_IMAGE;
const char *atspi_interface_registry = ATSPI_DBUS_INTERFACE_REGISTRY;
const char *atspi_interface_selection = ATSPI_DBUS_INTERFACE_SELECTION;
const char *atspi_interface_table = ATSPI_DBUS_INTERFACE_TABLE;
const char *atspi_interface_text = ATSPI_DBUS_INTERFACE_TEXT;
const char *atspi_interface_cache = ATSPI_DBUS_INTERFACE_CACHE;
const char *atspi_interface_value = ATSPI_DBUS_INTERFACE_VALUE;

static const char *interfaces[] =
{
  ATSPI_DBUS_INTERFACE_ACCESSIBLE,
  ATSPI_DBUS_INTERFACE_ACTION,
  ATSPI_DBUS_INTERFACE_APPLICATION,
  ATSPI_DBUS_INTERFACE_COLLECTION,
  ATSPI_DBUS_INTERFACE_COMPONENT,
  ATSPI_DBUS_INTERFACE_DOCUMENT,
  ATSPI_DBUS_INTERFACE_EDITABLE_TEXT,
  ATSPI_DBUS_INTERFACE_HYPERLINK,
  ATSPI_DBUS_INTERFACE_HYPERTEXT,
  ATSPI_DBUS_INTERFACE_IMAGE,
  "org.a11y.atspi.LoginHelper",
  ATSPI_DBUS_INTERFACE_SELECTION,
  ATSPI_DBUS_INTERFACE_TABLE,
  ATSPI_DBUS_INTERFACE_TEXT,
  ATSPI_DBUS_INTERFACE_VALUE,
  NULL
};

gint
_atspi_get_iface_num (const char *iface)
{
  /* TODO: Use a binary search or hash to improve performance */
  int i;

  for (i = 0; interfaces[i]; i++)
  {
    if (!strcmp(iface, interfaces[i])) return i;
  }
  return -1;
}

static GHashTable *
get_live_refs (void)
{
  if (!live_refs) 
    {
      live_refs = g_hash_table_new (g_direct_hash, g_direct_equal);
    }
  return live_refs;
}

/* TODO: Add an application parameter */
DBusConnection *
_atspi_bus ()
{
  if (!bus)
    atspi_init ();
  return bus;
}

#define APP_IS_REGISTRY(app) (!strcmp (app->bus_name, atspi_bus_registry))

static void
cleanup ()
{
  GHashTable *refs;

  refs = live_refs;
  live_refs = NULL;
  if (refs)
    {
      g_hash_table_destroy (refs);
    }
}

static gboolean atspi_inited = FALSE;

static GHashTable *app_hash = NULL;

static AtspiApplication *
get_application (const char *bus_name)
{
  AtspiApplication *app = NULL;
  char *bus_name_dup;

  if (!app_hash)
  {
    app_hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GDestroyNotify)g_hash_table_unref);
    if (!app_hash) return NULL;
  }
  app = g_hash_table_lookup (app_hash, bus_name);
  if (app) return app;
  bus_name_dup = g_strdup (bus_name);
  if (!bus_name_dup) return NULL;
  // TODO: change below to something that will send state-change:defunct notification if necessary */
  app = _atspi_application_new (bus_name);
  if (!app) return NULL;
  app->bus_name = bus_name_dup;
  if (APP_IS_REGISTRY (app))
  {
    app->hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
  }
  else
  {
    app->hash = g_hash_table_new_full (g_int_hash, g_int_equal, g_free, g_object_unref);
  }
  g_hash_table_insert (app_hash, bus_name_dup, app);
  return app;
}

static AtspiAccessible *
ref_accessible (const char *app_name, const char *path)
{
  AtspiApplication *app = get_application (app_name);
  AtspiAccessible *a;

  if (!strcmp (path, "/org/a11y/atspi/accessible/root"))
  {
    if (!app->root)
    {
      app->root = atspi_accessible_new (app, atspi_path_root);
      app->root->accessible_parent = atspi_get_desktop (0);
    }
    return g_object_ref (app->root);
  }

  a = g_hash_table_lookup (app->hash, path);
  if (a)
  {
    g_object_ref (a);
    return a;
  }
  a = atspi_accessible_new (app, path);
  if (!a)
    return NULL;
  g_hash_table_insert (app->hash, a->path, a);
  g_object_ref (a);	/* for the hash */
  return a;
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

static DBusHandlerResult
handle_remove_accessible (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  const char *sender = dbus_message_get_sender (message);
  AtspiApplication *app;
  const char *path;
  DBusMessageIter iter, iter_struct;
  const char *signature = dbus_message_get_signature (message);
  AtspiAccessible *a;
  int id;

  if (strcmp (signature, "(so)") != 0)
  {
    g_warning ("at-spi: Unknown signature %s for RemoveAccessible", signature);
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &sender);
  dbus_message_iter_get_basic (&iter_struct, &path);
  app = get_application (sender);
  a = ref_accessible (sender, path);
  if (a->accessible_parent && g_list_find (a->accessible_parent->children, a))
  {
    a->accessible_parent->children = g_list_remove (a->accessible_parent->children, a);
    g_object_unref (a);
  }
  g_hash_table_remove (app->hash, app->bus_name);
  g_object_unref (a);	/* unref our own ref */
  return DBUS_HANDLER_RESULT_HANDLED;
}

static gboolean
add_app_to_desktop (AtspiAccessible *a, const char *bus_name)
{
  DBusError error;
  char *root_path;

  dbus_error_init (&error);
  AtspiAccessible *obj = ref_accessible (bus_name, atspi_path_root);
  if (obj)
  {
    GList *new_list = g_list_append (a->children, obj);
    if (new_list)
    {
      a->children = new_list;
      return TRUE;
    }
  }
  else
  {
    g_warning ("Error calling getRoot for %s: %s", bus_name, error.message);
  }
  return FALSE;
}

static void
send_children_changed (AtspiAccessible *parent, AtspiAccessible *child, gboolean add)
{
  AtspiEvent e;

  memset (&e, 0, sizeof(e));
  e.type = (add? "object:children-changed:add": "object:children-changed:remove");
  e.source = parent;
  e.detail1 = g_list_index (parent->children, child);
#if 0
  g_warning ("atspi: TODO: Finish events");
  atspi_dispatch_event (&e);
#endif
}

static void
unref_object_and_descendants (AtspiAccessible *obj)
{
  GList *l;

  for (l = obj->children; l; l = l->next)
  {
    unref_object_and_descendants (l->data);
  }
  g_object_unref (obj);
}

static gboolean
remove_app_from_desktop (AtspiAccessible *a, const char *bus_name)
{
  GList *l;
  AtspiAccessible *child;

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
  unref_object_and_descendants (child);
  return TRUE;
}

static AtspiAccessible *desktop;

static void
get_reference_from_iter (DBusMessageIter *iter, const char **app_name, const char **path)
{
  DBusMessageIter iter_struct;

  dbus_message_iter_recurse (iter, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, app_name);
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, path);
  dbus_message_iter_next (iter);
}

static void
add_accessible_from_iter (DBusMessageIter *iter)
{
  gint i;
  GList *new_list;
  DBusMessageIter iter_struct, iter_array;
  const char *app_name, *path;
  AtspiApplication *app;
  AtspiAccessible *accessible;
  const char *name, *description;
  dbus_uint32_t role;
  dbus_uint32_t *states;
  int count;

  dbus_message_iter_recurse (iter, &iter_struct);

  /* get accessible */
  get_reference_from_iter (&iter_struct, &app_name, &path);
  accessible = ref_accessible (app_name, path);

  /* Get application: TODO */
  dbus_message_iter_next (&iter_struct);

  /* get parent */
  get_reference_from_iter (&iter_struct, &app_name, &path);
  accessible->accessible_parent = ref_accessible (app_name, path);

  /* Get children */
  dbus_message_iter_recurse (&iter_struct, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    AtspiAccessible *child;
    get_reference_from_iter (&iter_array, &app_name, &path);
    child = ref_accessible (app_name, path);
    new_list = g_list_append (accessible->children, child);
    if (new_list) accessible->children = new_list;
  }

  /* interfaces */
  accessible->interfaces = 0;
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_recurse (&iter_struct, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    const char *iface;
    gint n;
    dbus_message_iter_get_basic (&iter_array, &iface);
    if (!strcmp (iface, "org.freedesktop.DBus.Introspectable")) continue;
    n = _atspi_get_iface_num (iface);
    if (n == -1)
    {
      g_warning ("Unknown interface %s", iface);
    }
    else accessible->interfaces |= (1 << n);
    dbus_message_iter_next (&iter_array);
  }
  dbus_message_iter_next (&iter_struct);

  /* name */
  dbus_message_iter_get_basic (&iter_struct, &name);
  accessible->name = g_strdup (name);
  dbus_message_iter_next (&iter_struct);

  /* role */
  dbus_message_iter_get_basic (&iter_struct, &role);
  accessible->role = role;
  dbus_message_iter_next (&iter_struct);

  /* description */
  dbus_message_iter_get_basic (&iter_struct, &description);
  accessible->description = g_strdup (description);
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_recurse (&iter_struct, &iter_array);
  dbus_message_iter_get_fixed_array (&iter_array, &states, &count);
  if (count != 2)
  {
    g_warning ("at-spi: expected 2 values in states array; got %d\n", count);
    accessible->states = atspi_state_set_new (0);
  }
  else
  {
    guint64 val = ((guint64)states [1]) << 32;
    val += states [0];
    accessible->states = atspi_state_set_new (val);
  }
  dbus_message_iter_next (&iter_struct);

  /* This is a bit of a hack since the cache holds a ref, so we don't need
   * the one provided for us anymore */
  g_object_unref (accessible);
}

static void
add_accessibles (const char *app_name)
{
  DBusError error;
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_array;

  AtspiApplication *app = get_application (app_name);
  /* TODO: Move this functionality into app initializer? */
  dbus_error_init (&error);
  message = dbus_message_new_method_call (app_name, "/org/a11y/atspi/cache", atspi_interface_cache, "GetItems");
  reply = _atspi_dbus_send_with_reply_and_block (message);
  if (!reply || strcmp (dbus_message_get_signature (reply), "a((so)(so)(so)a(so)assusau)") != 0)
  {
    g_warning ("at-spi: Error in GetItems");
    return;
    if (reply)
      dbus_message_unref (reply);
  }
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    add_accessible_from_iter (&iter_array);
    dbus_message_iter_next (&iter_array);
  }
  dbus_message_unref (reply);
}

/* TODO: Do we stil need this function? */
static AtspiAccessible *
ref_accessible_desktop (AtspiApplication *app)
{
  DBusError error;
  GArray *apps = NULL;
  GArray *additions;
  gint i;
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_array;

  if (desktop)
  {
    g_object_ref (desktop);
    return desktop;
  }
  desktop = atspi_accessible_new (app, atspi_path_root);
  if (!desktop)
  {
    return NULL;
  }
  g_hash_table_insert (app->hash, desktop->path, desktop);
  g_object_ref (desktop);	/* for the hash */
  desktop->name = g_strdup ("main");
  dbus_error_init (&error);
  message = dbus_message_new_method_call (atspi_bus_registry,
	atspi_path_root,
	atspi_interface_accessible,
	"GetChildren");
  if (!message)
    return;
  reply = _atspi_dbus_send_with_reply_and_block (message);
  if (!reply || strcmp (dbus_message_get_signature (reply), "a(so)") != 0)
  {
    g_error ("Couldn't get application list: %s", error.message);
    if (reply)
      dbus_message_unref (reply);
    return;
  }
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    const char *app_name, *path;
    get_reference_from_iter (&iter_array, &app_name, &path);
    add_accessibles (app_name);
    add_app_to_desktop (desktop, app_name);
  }
  dbus_message_unref (reply);
  return desktop;
}

AtspiAccessible *
_atspi_ref_accessible (const char *app, const char *path)
{
  AtspiApplication *a = get_application (app);
  if (!a) return NULL;
  if ( APP_IS_REGISTRY(a))
  {
    return ref_accessible_desktop (a);
  }
  return ref_accessible (app, path);
}

AtspiAccessible *
_atspi_dbus_return_accessible_from_message (DBusMessage *message)
{
  DBusMessageIter iter;
  const char *app_name, *path;
  AtspiAccessible *retval = NULL;
  const char *signature = dbus_message_get_signature (message);
   
  if (!strcmp (signature, "(so)"))
  {
    dbus_message_iter_init (message, &iter);
    get_reference_from_iter (&iter, &app_name, &path);
    retval = _atspi_ref_accessible (app_name, path);
  }
  else
  {
    g_warning ("Atspi: Called __atspi_dbus_return_accessible_from_message with strange signature %s", signature);
  }
  dbus_message_unref (message);
  return retval;
}

/* TODO: Remove this function. We should not need it anymore.
 * If we do, it's a bug.
 */
AtspiAccessible *
_atspi_ref_related_accessible (AtspiAccessible *obj, const AtspiReference *ref)
{
  const char *app = (ref->name && ref->name[0]? ref->name: obj->app->bus_name);
  return ref_accessible (app, obj->path);
}

const char *cache_signal_type = "((so)(so)a(so)assusau)";

static DBusHandlerResult
handle_add_accessible (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessageIter iter;
  const char *sender = dbus_message_get_sender (message);
  AtspiApplication *app = get_application (sender);
  const char *type = cache_signal_type;

  if (strcmp (dbus_message_get_signature (message), cache_signal_type) != 0)
  {
    g_warning ("atspi: AddAccessible with unknown signature %s\n", dbus_message_get_signature (message));
    return;
  }

  dbus_message_iter_init (message, &iter);
  add_accessible_from_iter (&iter);
}

static DBusHandlerResult
atspi_dbus_filter (DBusConnection *bus, DBusMessage *message, void *data)
{
  int type = dbus_message_get_type (message);
  const char *interface = dbus_message_get_interface (message);
  const char *member = dbus_message_get_member (message); 
  dbus_uint32_t v;
  char *bus_name;

  if (type == DBUS_MESSAGE_TYPE_SIGNAL &&
      !strncmp (interface, "org.a11y.atspi.Event.", 28))
  {
    g_warning ("atspi: TODO: event");
    //return handle_event (bus, message, data);
  }
  if (dbus_message_is_method_call (message, atspi_interface_device_event_listener, "notifyEvent"))
  {
    g_warning ("atspi: TODO: DeviceEvent");
    //return handle_device_event (bus, message, data);
  }
  if (dbus_message_is_signal (message, atspi_interface_cache, "AddAccessible"))
  {
    return handle_add_accessible (bus, message, data);
  }
  if (dbus_message_is_signal (message, atspi_interface_cache, "RemoveAccessible"))
  {
    return handle_remove_accessible (bus, message, data);
  }
  /* TODO: Handle ChildrenChanged, StateChanged, PropertyChanged */
  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static const char *signal_interfaces[] =
{
  "org.a11y.atspi.Event.Object",
  "org.a11y.atspi.Event.Window",
  "org.a11y.atspi.Event.Mouse",
  "org.a11y.atspi.Event.Terminal",
  "org.a11y.atspi.Event.Document",
  "org.a11y.atspi.Event.Focus",
  NULL
};

/**
 * atspi_init:
 *
 * Connects to the accessibility registry and initializes the SPI.
 *
 * Returns: 0 on success, otherwise an integer error code.  
 **/
int
atspi_init (void)
{
  DBusError error;
  char *match;
  int i;

  if (atspi_inited)
    {
      return 1;
    }

  atspi_inited = TRUE;

  g_type_init ();

  get_live_refs();
  g_atexit (cleanup);

  dbus_error_init (&error);
  bus = dbus_bus_get (DBUS_BUS_SESSION, &error);
  if (!bus)
  {
    g_error ("Couldn't get session bus");
    return 2;
  }
  dbus_bus_register (bus, &error);
  dbus_connection_setup_with_g_main(bus, g_main_context_default());
  dbus_connection_add_filter (bus, atspi_dbus_filter, NULL, NULL);
  match = g_strdup_printf ("type='signal',interface='%s',member='AddAccessible'", atspi_interface_cache);
  dbus_error_init (&error);
  dbus_bus_add_match (bus, match, &error);
  g_free (match);
  match = g_strdup_printf ("type='signal',interface='%s',member='RemoveAccessible'", atspi_interface_cache);
  dbus_bus_add_match (bus, match, &error);
  g_free (match);
  for (i = 0; signal_interfaces[i]; i++)
  {
    match = g_strdup_printf ("type='signal',interface='%s'", signal_interfaces[i]);
    dbus_bus_add_match (bus, match, &error);
    g_free (match);
  }
  return 0;
}

  static GMainLoop *mainloop;

/**
 * atspi_event_main:
 *
 * Starts/enters the main event loop for the AT-SPI services.
 *
 * (NOTE: This method does not return control, it is exited via a call to
 *  atspi_event_quit () from within an event handler).
 *
 **/
void
atspi_event_main (void)
{
  mainloop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (mainloop);
}

/**
 * atspi_event_quit:
 *
 * Quits the last main event loop for the SPI services,
 * see atspi_event_main
 **/
void
atspi_event_quit (void)
{
  g_main_loop_quit (mainloop);
}

/**
 * atspi_exit:
 *
 * Disconnects from the Accessibility Registry and releases 
 * any floating resources. Call only once at exit.
 *
 * Returns: 0 if there were no leaks, otherwise non zero.
 **/
int
atspi_exit (void)
{
  int leaked;

  if (!atspi_inited)
    {
      return 0;
    }

  atspi_inited = FALSE;

  if (live_refs)
    {
      leaked = g_hash_table_size (live_refs);
    }
  else
    {
      leaked = 0;
    }

  cleanup ();

  return leaked;
}

dbus_bool_t
_atspi_dbus_call (AtspiAccessible *obj, const char *interface, const char *method, GError **error, const char *type, ...)
{
  va_list args;
  dbus_bool_t retval;
  DBusError err;

  dbus_error_init (&err);
  va_start (args, type);
  retval = dbind_method_call_reentrant_va (_atspi_bus(), obj->app->bus_name, obj->path, interface, method, &err, type, args);
  va_end (args);
  if (dbus_error_is_set (&err))
  {
    /* TODO: Set gerror */
    dbus_error_free (&err);
  }
  return retval;
}

DBusMessage *
_atspi_dbus_call_partial (AtspiAccessible *obj, const char *interface, const char *method, GError **error, const char *type, ...)
{
  va_list args;
  dbus_bool_t retval;
  DBusError err;
    DBusMessage *msg = NULL, *reply = NULL;
    DBusMessageIter iter;
    const char *p;

  dbus_error_init (&err);
  va_start (args, type);

    msg = dbus_message_new_method_call (obj->app->bus_name, obj->path, interface, method);
    if (!msg)
        goto out;

    p = type;
    dbus_message_iter_init_append (msg, &iter);
    dbind_any_marshal_va (&iter, &p, args);

    reply = dbind_send_and_allow_reentry (_atspi_bus(), msg, &err);
out:
  va_end (args);
  if (dbus_error_is_set (&err))
  {
    /* TODO: Set gerror */
    dbus_error_free (&err);
  }
  return reply;
}

dbus_bool_t
_atspi_dbus_get_property (AtspiAccessible *obj, const char *interface, const char *name, GError **error, const char *type, void *data)
{
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_variant;
  DBusError err;
  dbus_bool_t retval = FALSE;

  message = dbus_message_new_method_call (obj->app->bus_name, obj->path, "org.freedesktop.DBus.Properties", "Get");
  if (!message)
  {
    // TODO: throw exception
    goto done;
  }
  dbus_message_append_args (message, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  dbus_error_init (&err);
  reply = dbus_connection_send_with_reply_and_block (_atspi_bus(), message, 1000, &err);
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
    g_warning ("atspi_dbus_get_property: Wrong type: expected %s, got %c\n", type, dbus_message_iter_get_arg_type (&iter_variant));
    goto done;
  }
  dbus_message_iter_get_basic (&iter_variant, data);
  dbus_message_unref (reply);
  if (type[0] == 's') *(char **)data = g_strdup (*(char **)data);
  retval = TRUE;
done:
  return retval;
}

DBusMessage *
_atspi_dbus_send_with_reply_and_block (DBusMessage *message)
{
  DBusMessage *reply;
  DBusError err;

  dbus_error_init (&err);
  g_warning ("TODO: Write _atspi_dbus_send_with_reply_and_block");
  reply = dbus_connection_send_with_reply_and_block (_atspi_bus(), message, 1000, &err);
  dbus_message_unref (message);
  return reply;
}

GHashTable *
_atspi_dbus_hash_from_message (DBusMessage *message)
{
  GHashTable *hash = g_hash_table_new (g_str_hash, g_str_equal);
  DBusMessageIter iter, iter_array, iter_dict;
  const char *signature;

  signature = dbus_message_get_signature (message);

  if (strcmp (signature, "a{ss}") != 0)
    {
      g_warning ("Trying to get hash from message of unexpected type %s\n", signature);
      return NULL;
    }

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    const char *name, *value;
    dbus_message_iter_recurse (&iter_array, &iter_dict);
    dbus_message_iter_get_basic (&iter_dict, &name);
    dbus_message_iter_get_basic (&iter_dict, &value);
    g_hash_table_insert (hash, g_strdup (name), g_strdup (value));
    dbus_message_iter_next (&iter_array);
  }
  return hash;
}

GArray *
_atspi_dbus_attribute_array_from_message (DBusMessage *message)
{
  GArray *array = g_array_new (TRUE, TRUE, sizeof (gchar *));
  DBusMessageIter iter, iter_array, iter_dict;
  const char *signature;
  gint count = 0;

  signature = dbus_message_get_signature (message);

  if (strcmp (signature, "a{ss}") != 0)
    {
      g_warning ("Trying to get hash from message of unexpected type %s\n", signature);
      return NULL;
    }

  dbus_message_iter_init (message, &iter);

  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    const char *name, *value;
    gchar *str;
    GArray *new_array;
    dbus_message_iter_recurse (&iter_array, &iter_dict);
    dbus_message_iter_get_basic (&iter_dict, &name);
    dbus_message_iter_get_basic (&iter_dict, &value);
    str = g_strdup_printf ("%s:;%s", name, value);
    new_array = g_array_append_val (array, str);
    if (new_array)
      array = new_array;
    dbus_message_iter_next (&iter);;
  }
  return array;
}

