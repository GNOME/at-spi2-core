/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
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
#ifdef HAVE_X11
#include "X11/Xlib.h"
#endif
#include "atspi-gmain.h"
#include <stdio.h>
#include <string.h>

static void handle_get_items (DBusPendingCall *pending, void *user_data);

static DBusConnection *bus = NULL;
static GHashTable *live_refs = NULL;
static gint method_call_timeout = 800;
static gint app_startup_time = 15000;
static gboolean allow_sync = TRUE;

GMainLoop *atspi_main_loop;
GMainContext *atspi_main_context;
gboolean atspi_no_cache;

const char *atspi_path_dec = ATSPI_DBUS_PATH_DEC;
const char *atspi_path_registry = ATSPI_DBUS_PATH_REGISTRY;
const char *atspi_path_root = ATSPI_DBUS_PATH_ROOT;
const char *atspi_bus_registry = ATSPI_DBUS_NAME_REGISTRY;
const char *atspi_interface_accessible = ATSPI_DBUS_INTERFACE_ACCESSIBLE;
const char *atspi_interface_action = ATSPI_DBUS_INTERFACE_ACTION;
const char *atspi_interface_application = ATSPI_DBUS_INTERFACE_APPLICATION;
const char *atspi_interface_collection = ATSPI_DBUS_INTERFACE_COLLECTION;
const char *atspi_interface_component = ATSPI_DBUS_INTERFACE_COMPONENT;
const char *atspi_interface_dec = ATSPI_DBUS_INTERFACE_DEC;
const char *atspi_interface_device_event_listener = ATSPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER;
const char *atspi_interface_document = ATSPI_DBUS_INTERFACE_DOCUMENT;
const char *atspi_interface_editable_text = ATSPI_DBUS_INTERFACE_EDITABLE_TEXT;
const char *atspi_interface_event_object = ATSPI_DBUS_INTERFACE_EVENT_OBJECT;
const char *atspi_interface_hyperlink = ATSPI_DBUS_INTERFACE_HYPERLINK;
const char *atspi_interface_hypertext = ATSPI_DBUS_INTERFACE_HYPERTEXT;
const char *atspi_interface_image = ATSPI_DBUS_INTERFACE_IMAGE;
const char *atspi_interface_registry = ATSPI_DBUS_INTERFACE_REGISTRY;
const char *atspi_interface_selection = ATSPI_DBUS_INTERFACE_SELECTION;
const char *atspi_interface_table = ATSPI_DBUS_INTERFACE_TABLE;
const char *atspi_interface_table_cell = ATSPI_DBUS_INTERFACE_TABLE_CELL;
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
  ATSPI_DBUS_INTERFACE_TABLE_CELL,
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

GHashTable *
_atspi_get_live_refs (void)
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
  if (!bus)
    g_error ("AT-SPI: Couldn't connect to accessibility bus. Is at-spi-bus-launcher running?");
  return bus;
}

#define APP_IS_REGISTRY(app) (!strcmp (app->bus_name, atspi_bus_registry))

static AtspiAccessible *desktop;

static void
cleanup ()
{
  GHashTable *refs;
  gint i;

  refs = live_refs;
  live_refs = NULL;
  if (refs)
    {
      g_hash_table_destroy (refs);
    }

  if (bus)
    {
      dbus_connection_close (bus);
      dbus_connection_unref (bus);
      bus = NULL;
    }

  if (!desktop)
    return;

  /* TODO: Do we need this code, or should we just dispose the desktop? */
  for (i = desktop->children->len - 1; i >= 0; i--)
  {
    AtspiAccessible *child = g_ptr_array_index (desktop->children, i);
    g_object_run_dispose (G_OBJECT (child->parent.app));
    g_object_run_dispose (G_OBJECT (child));
  }

  g_object_run_dispose (G_OBJECT (desktop->parent.app));
  g_object_unref (desktop);
  desktop = NULL;
}

static gboolean atspi_inited = FALSE;

static GHashTable *app_hash = NULL;

static void
handle_get_bus_address (DBusPendingCall *pending, void *user_data)
{
  AtspiApplication *app = user_data;
  DBusMessage *reply = dbus_pending_call_steal_reply (pending);
  DBusMessage *message;
  const char *address;
  DBusPendingCall *new_pending;

  if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN)
  {
    if (dbus_message_get_args (reply, NULL, DBUS_TYPE_STRING, &address,
                               DBUS_TYPE_INVALID) && address [0])
    {
      DBusError error;
      DBusConnection *bus;

      dbus_error_init (&error);
      bus = dbus_connection_open_private (address, &error);
      if (bus)
      {
        if (app->bus)
          {
            dbus_connection_unref (app->bus);
          }
        app->bus = bus;
      }
      else
      {
        if (!strcmp (error.name, DBUS_ERROR_FILE_NOT_FOUND))
          g_warning ("Unable to open bus connection: %s", error.message);
        dbus_error_free (&error);
      }
    }
  }
  dbus_message_unref (reply);
  dbus_pending_call_unref (pending);

  if (!app->bus)
    return; /* application has gone away / been disposed */

  message = dbus_message_new_method_call (app->bus_name,
                                          "/org/a11y/atspi/cache",
                                          atspi_interface_cache, "GetItems");

  dbus_connection_send_with_reply (app->bus, message, &new_pending, 2000);
  dbus_message_unref (message);
  if (!new_pending)
    return;
  dbus_pending_call_set_notify (new_pending, handle_get_items, app, NULL);
}

static AtspiApplication *
get_application (const char *bus_name)
{
  AtspiApplication *app = NULL;
  char *bus_name_dup;
  DBusMessage *message;
  DBusPendingCall *pending = NULL;

  if (!app_hash)
  {
    app_hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GDestroyNotify)g_object_unref);
    if (!app_hash) return NULL;
  }
  app = g_hash_table_lookup (app_hash, bus_name);
  if (app) return app;
  bus_name_dup = g_strdup (bus_name);
  if (!bus_name_dup) return NULL;
  // TODO: change below to something that will send state-change:defunct notification if necessary */
  app = _atspi_application_new (bus_name);
  app->hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
  app->bus = dbus_connection_ref (_atspi_bus ());
  gettimeofday (&app->time_added, NULL);
  app->cache = ATSPI_CACHE_UNDEFINED;
  g_hash_table_insert (app_hash, bus_name_dup, app);
  message = dbus_message_new_method_call (bus_name, atspi_path_root,
                                          atspi_interface_application, "GetApplicationBusAddress");

  dbus_connection_send_with_reply (app->bus, message, &pending, 2000);
  dbus_message_unref (message);
  if (!pending)
  {
    g_hash_table_remove (app_hash, bus_name_dup);
    return NULL;
  }
  dbus_pending_call_set_notify (pending, handle_get_bus_address, app, NULL);
  return app;
}

static AtspiAccessible *
ref_accessible (const char *app_name, const char *path)
{
  AtspiApplication *app;
  AtspiAccessible *a;

  if (!strcmp (path, ATSPI_DBUS_PATH_NULL))
    return NULL;

  app = get_application (app_name);

  if (!strcmp (path, "/org/a11y/atspi/accessible/root"))
  {
    if (!app->root)
    {
      app->root = _atspi_accessible_new (app, atspi_path_root);
      app->root->accessible_parent = atspi_get_desktop (0);
      g_ptr_array_add (app->root->accessible_parent->children, g_object_ref (app->root));
    }
    return g_object_ref (app->root);
  }

  a = g_hash_table_lookup (app->hash, path);
  if (a)
  {
    return g_object_ref (a);
  }
  a = _atspi_accessible_new (app, path);
  if (!a)
    return NULL;
  g_hash_table_insert (app->hash, g_strdup (a->parent.path), g_object_ref (a));
  return a;
}

static AtspiHyperlink *
ref_hyperlink (const char *app_name, const char *path)
{
  AtspiApplication *app = get_application (app_name);
  AtspiHyperlink *hyperlink;

  if (!strcmp (path, ATSPI_DBUS_PATH_NULL))
    return NULL;

  hyperlink = g_hash_table_lookup (app->hash, path);
  if (hyperlink)
  {
    return g_object_ref (hyperlink);
  }
  hyperlink = _atspi_hyperlink_new (app, path);
  g_hash_table_insert (app->hash, g_strdup (hyperlink->parent.path), hyperlink);
  /* TODO: This should be a weak ref */
  g_object_ref (hyperlink);	/* for the hash */
  return hyperlink;
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

  if (strcmp (signature, "(so)") != 0)
  {
    g_warning ("AT-SPI: Unknown signature %s for RemoveAccessible", signature);
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &sender);
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &path);
  app = get_application (sender);
  a = ref_accessible (sender, path);
  if (!a)
    return DBUS_HANDLER_RESULT_HANDLED;
  g_object_run_dispose (G_OBJECT (a));
  g_hash_table_remove (app->hash, a->parent.path);
  g_object_unref (a);	/* unref our own ref */
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
handle_name_owner_changed (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  const char *name, *new, *old;
  static gboolean registry_lost = FALSE;

  if (!dbus_message_get_args (message, NULL,
                              DBUS_TYPE_STRING, &name,
                              DBUS_TYPE_STRING, &old,
                              DBUS_TYPE_STRING, &new,
                              DBUS_TYPE_INVALID))
  {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  if (!strcmp (name, "org.a11y.atspi.Registry"))
  {
    if (registry_lost && !old[0])
    {
      _atspi_reregister_event_listeners ();
      _atspi_reregister_device_listeners ();
      registry_lost = FALSE;
    }
    else if (!new[0])
      registry_lost = TRUE;
  }
  else if (app_hash)
  {
    AtspiApplication *app = g_hash_table_lookup (app_hash, old);
    if (app)
      g_object_run_dispose (G_OBJECT (app));
  }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static gboolean
add_app_to_desktop (AtspiAccessible *a, const char *bus_name)
{
  AtspiAccessible *obj = ref_accessible (bus_name, atspi_path_root);
  /* The app will be added to the desktop as a side-effect of calling
   * ref_accessible */
  g_object_unref (obj);
  return (obj != NULL);
}

void
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
  DBusMessageIter iter_struct, iter_array;
  const char *app_name, *path;
  AtspiAccessible *accessible;
  const char *name, *description;
  dbus_uint32_t role;
  gboolean children_cached = FALSE;
  dbus_int32_t count, index;

  dbus_message_iter_recurse (iter, &iter_struct);

  /* get accessible */
  get_reference_from_iter (&iter_struct, &app_name, &path);
  accessible = ref_accessible (app_name, path);
  if (!accessible)
    return;

  /* Get application: TODO */
  dbus_message_iter_next (&iter_struct);

  /* get parent */
  get_reference_from_iter (&iter_struct, &app_name, &path);
  if (accessible->accessible_parent)
    g_object_unref (accessible->accessible_parent);
  accessible->accessible_parent = ref_accessible (app_name, path);

  if (dbus_message_iter_get_arg_type (&iter_struct) == 'i')
  {
    /* Get index in parent */
    dbus_message_iter_get_basic (&iter_struct, &index);
    if (index >= 0 && accessible->accessible_parent)
    {
      if (index >= accessible->accessible_parent->children->len)
        g_ptr_array_set_size (accessible->accessible_parent->children, index + 1);
      g_ptr_array_index (accessible->accessible_parent->children, index) = g_object_ref (accessible);
    }

    /* get child count */
    dbus_message_iter_next (&iter_struct);
    dbus_message_iter_get_basic (&iter_struct, &count);
    if (count >= 0)
    {
      g_ptr_array_set_size (accessible->children, count);
      children_cached = TRUE;
    }
  }
  else if (dbus_message_iter_get_arg_type (&iter_struct) == 'a')
  {
    /* It's the old API with a list of children */
    /* TODO: Perhaps remove this code eventually */
    dbus_message_iter_recurse (&iter_struct, &iter_array);
    while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      AtspiAccessible *child;
      get_reference_from_iter (&iter_array, &app_name, &path);
      child = ref_accessible (app_name, path);
      g_ptr_array_remove (accessible->children, child);
      g_ptr_array_add (accessible->children, child);
    }
    children_cached = TRUE;
  }

  /* interfaces */
  dbus_message_iter_next (&iter_struct);
  _atspi_dbus_set_interfaces (accessible, &iter_struct);
  dbus_message_iter_next (&iter_struct);

  /* name */
  if (accessible->name)
    g_free (accessible->name);
  dbus_message_iter_get_basic (&iter_struct, &name);
  accessible->name = g_strdup (name);
  dbus_message_iter_next (&iter_struct);

  /* role */
  dbus_message_iter_get_basic (&iter_struct, &role);
  accessible->role = role;
  dbus_message_iter_next (&iter_struct);

  /* description */
  if (accessible->description)
    g_free (accessible->description);
  dbus_message_iter_get_basic (&iter_struct, &description);
  accessible->description = g_strdup (description);
  dbus_message_iter_next (&iter_struct);

  _atspi_dbus_set_state (accessible, &iter_struct);
  dbus_message_iter_next (&iter_struct);

  _atspi_accessible_add_cache (accessible, ATSPI_CACHE_NAME | ATSPI_CACHE_ROLE |
                               ATSPI_CACHE_PARENT | ATSPI_CACHE_DESCRIPTION);
  if (!atspi_state_set_contains (accessible->states,
                                       ATSPI_STATE_MANAGES_DESCENDANTS) &&
                                       children_cached)
    _atspi_accessible_add_cache (accessible, ATSPI_CACHE_CHILDREN);

  /* This is a bit of a hack since the cache holds a ref, so we don't need
   * the one provided for us anymore */
  g_object_unref (accessible);
}

static void
handle_get_items (DBusPendingCall *pending, void *user_data)
{
  DBusMessage *reply = dbus_pending_call_steal_reply (pending);
  DBusMessageIter iter, iter_array;

  if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR)
  {
    const char *sender = dbus_message_get_sender (reply);
    const char *error = NULL;
    if (!strcmp (dbus_message_get_error_name (reply),
                 DBUS_ERROR_SERVICE_UNKNOWN))
    {
    }
    else
    {
      dbus_message_get_args (reply, NULL, DBUS_TYPE_STRING, &error,
                             DBUS_TYPE_INVALID);
      g_warning ("AT-SPI: Error in GetItems, sender=%s, error=%s", sender, error);
    }
    dbus_message_unref (reply);
    dbus_pending_call_unref (pending);
    return;
  }

  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    add_accessible_from_iter (&iter_array);
    dbus_message_iter_next (&iter_array);
  }
  dbus_message_unref (reply);
  dbus_pending_call_unref (pending);
}

/* TODO: Do we stil need this function? */
static AtspiAccessible *
ref_accessible_desktop (AtspiApplication *app)
{
  GError *error;
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_array;
  gchar *bus_name_dup;

  if (desktop)
  {
    g_object_ref (desktop);
    return desktop;
  }
  desktop = _atspi_accessible_new (app, atspi_path_root);
  if (!desktop)
  {
    return NULL;
  }
  g_hash_table_insert (app->hash, g_strdup (desktop->parent.path),
                       g_object_ref (desktop));
  app->root = g_object_ref (desktop);
  desktop->name = g_strdup ("main");
  message = dbus_message_new_method_call (atspi_bus_registry,
	atspi_path_root,
	atspi_interface_accessible,
	"GetChildren");
  if (!message)
    return NULL;
  error = NULL;
  reply = _atspi_dbus_send_with_reply_and_block (message, &error);
  if (!reply || strcmp (dbus_message_get_signature (reply), "a(so)") != 0)
  {
    if (error != NULL)
    {
      g_warning ("Couldn't get application list: %s", error->message);
      g_clear_error (&error);
    }
    if (reply)
      dbus_message_unref (reply);
    return NULL;
  }
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    const char *app_name, *path;
    get_reference_from_iter (&iter_array, &app_name, &path);
    add_app_to_desktop (desktop, app_name);
  }

  /* Record the alternate name as an alias for org.a11y.atspi.Registry */
  bus_name_dup = g_strdup (dbus_message_get_sender (reply));
  if (bus_name_dup)
    g_hash_table_insert (app_hash, bus_name_dup, app);

  dbus_message_unref (reply);

  return g_object_ref (desktop);
}

AtspiAccessible *
_atspi_ref_accessible (const char *app, const char *path)
{
  AtspiApplication *a = get_application (app);
  if (!a)
    return NULL;
  if ( APP_IS_REGISTRY(a))
  {
    if (!a->root)
      g_object_unref (ref_accessible_desktop (a));	/* sets a->root */
    return g_object_ref (a->root);
  }
  return ref_accessible (app, path);
}

AtspiAccessible *
_atspi_dbus_return_accessible_from_message (DBusMessage *message)
{
  DBusMessageIter iter;
  AtspiAccessible *retval = NULL;
  const char *signature;

  if (!message)
    return NULL;

  signature = dbus_message_get_signature (message);
  if (!strcmp (signature, "(so)"))
  {
    dbus_message_iter_init (message, &iter);
    retval =  _atspi_dbus_return_accessible_from_iter (&iter);
  }
  else
  {
    g_warning ("AT-SPI: Called _atspi_dbus_return_accessible_from_message with strange signature %s", signature);
  }
  dbus_message_unref (message);
  return retval;
}

AtspiAccessible *
_atspi_dbus_return_accessible_from_iter (DBusMessageIter *iter)
{
  const char *app_name, *path;

  get_reference_from_iter (iter, &app_name, &path);
  return ref_accessible (app_name, path);
}

AtspiHyperlink *
_atspi_dbus_return_hyperlink_from_message (DBusMessage *message)
{
  DBusMessageIter iter;
  AtspiHyperlink *retval = NULL;
  const char *signature;
   
  if (!message)
    return NULL;

  signature = dbus_message_get_signature (message);
  if (!strcmp (signature, "(so)"))
  {
    dbus_message_iter_init (message, &iter);
    retval =  _atspi_dbus_return_hyperlink_from_iter (&iter);
  }
  else
  {
    g_warning ("AT-SPI: Called _atspi_dbus_return_hyperlink_from_message with strange signature %s", signature);
  }
  dbus_message_unref (message);
  return retval;
}

AtspiHyperlink *
_atspi_dbus_return_hyperlink_from_iter (DBusMessageIter *iter)
{
  const char *app_name, *path;

  get_reference_from_iter (iter, &app_name, &path);
  return ref_hyperlink (app_name, path);
}

const char *cache_signal_type = "((so)(so)(so)iiassusau)";
const char *old_cache_signal_type = "((so)(so)(so)a(so)assusau)";

static DBusHandlerResult
handle_add_accessible (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessageIter iter;
  const char *signature = dbus_message_get_signature (message);

  if (strcmp (signature, cache_signal_type) != 0 &&
      strcmp (signature, old_cache_signal_type) != 0)
  {
    g_warning ("AT-SPI: AddAccessible with unknown signature %s\n", signature);
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  dbus_message_iter_init (message, &iter);
  add_accessible_from_iter (&iter);
  return DBUS_HANDLER_RESULT_HANDLED;
}

typedef struct
{
  DBusConnection *bus;
  DBusMessage *message;
  void *data;
} BusDataClosure;

static GSource *process_deferred_messages_source = NULL;

static void
process_deferred_message (BusDataClosure *closure)
{
  int type = dbus_message_get_type (closure->message);
  const char *interface = dbus_message_get_interface (closure->message);

  if (type == DBUS_MESSAGE_TYPE_SIGNAL &&
      !strncmp (interface, "org.a11y.atspi.Event.", 21))
  {
    _atspi_dbus_handle_event (closure->bus, closure->message, closure->data);
  }
  if (dbus_message_is_method_call (closure->message, atspi_interface_device_event_listener, "NotifyEvent"))
  {
    _atspi_dbus_handle_DeviceEvent (closure->bus,
                                   closure->message, closure->data);
  }
  if (dbus_message_is_signal (closure->message, atspi_interface_cache, "AddAccessible"))
  {
    handle_add_accessible (closure->bus, closure->message, closure->data);
  }
  if (dbus_message_is_signal (closure->message, atspi_interface_cache, "RemoveAccessible"))
  {
    handle_remove_accessible (closure->bus, closure->message, closure->data);
  }
  if (dbus_message_is_signal (closure->message, "org.freedesktop.DBus", "NameOwnerChanged"))
  {
    handle_name_owner_changed (closure->bus, closure->message, closure->data);
  }
}

static GQueue *deferred_messages = NULL;

static gboolean
process_deferred_messages (void)
{
  static int in_process_deferred_messages = 0;
  BusDataClosure *closure;

  if (in_process_deferred_messages)
    return TRUE;
  in_process_deferred_messages = 1;
  while ((closure = g_queue_pop_head (deferred_messages)))
  {
    process_deferred_message (closure);
    dbus_message_unref (closure->message);
    dbus_connection_unref (closure->bus);
    g_free (closure);
  }
  in_process_deferred_messages = 0;
  return FALSE;
}

static gboolean
process_deferred_messages_callback (gpointer data)
{
  if (process_deferred_messages ())
    return G_SOURCE_CONTINUE;

  process_deferred_messages_source = NULL;
  return G_SOURCE_REMOVE;
}

static DBusHandlerResult
defer_message (DBusConnection *connection, DBusMessage *message, void *user_data)
{
  BusDataClosure *closure = g_new (BusDataClosure, 1);

  closure->bus = dbus_connection_ref (bus);
  closure->message = dbus_message_ref (message);
  closure->data = user_data;

  g_queue_push_tail (deferred_messages, closure);

  if (process_deferred_messages_source == NULL)
  {
    process_deferred_messages_source = g_idle_source_new ();
    g_source_set_callback (process_deferred_messages_source,
                           process_deferred_messages_callback, NULL, NULL);
    g_source_attach (process_deferred_messages_source, atspi_main_context);
    g_source_unref (process_deferred_messages_source);
  }

  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
atspi_dbus_filter (DBusConnection *bus, DBusMessage *message, void *data)
{
  int type = dbus_message_get_type (message);
  const char *interface = dbus_message_get_interface (message);

  if (type == DBUS_MESSAGE_TYPE_SIGNAL &&
      !strncmp (interface, "org.a11y.atspi.Event.", 21))
  {
    return defer_message (bus, message, data);
  }
  if (dbus_message_is_method_call (message, atspi_interface_device_event_listener, "NotifyEvent"))
  {
    return defer_message (bus, message, data);
  }
  if (dbus_message_is_signal (message, atspi_interface_cache, "AddAccessible"))
  {
    return defer_message (bus, message, data);
  }
  if (dbus_message_is_signal (message, atspi_interface_cache, "RemoveAccessible"))
  {
    return defer_message (bus, message, data);
  }
  if (dbus_message_is_signal (message, "org.freedesktop.DBus", "NameOwnerChanged"))
  {
    defer_message (bus, message, data);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }
  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

/*
 * Returns a 'canonicalized' value for DISPLAY,
 * with the screen number stripped off if present.
 *
 * TODO: Avoid having duplicate functions for this here and in at-spi2-atk
 */
static gchar *
spi_display_name (void)
{
  char *canonical_display_name = NULL;
  const gchar *display_env = g_getenv ("AT_SPI_DISPLAY");

  if (!display_env)
    {
      display_env = g_getenv ("DISPLAY");
      if (!display_env || !display_env[0])
        return NULL;
      else
        {
          gchar *display_p, *screen_p;
          canonical_display_name = g_strdup (display_env);
          display_p = g_utf8_strrchr (canonical_display_name, -1, ':');
          screen_p = g_utf8_strrchr (canonical_display_name, -1, '.');
          if (screen_p && display_p && (screen_p > display_p))
            {
              *screen_p = '\0';
            }
        }
    }
  else
    {
      canonical_display_name = g_strdup (display_env);
    }

  return canonical_display_name;
}

/**
 * atspi_init:
 *
 * Connects to the accessibility registry and initializes the SPI.
 *
 * Returns: 0 on success, 1 if already initialized, or an integer error code.  
 **/
int
atspi_init (void)
{
  char *match;
  const gchar *no_cache;

  if (atspi_inited)
    {
      return 1;
    }

  atspi_inited = TRUE;

  _atspi_get_live_refs();

  bus = atspi_get_a11y_bus ();
  if (!bus)
    return 2;
  dbus_bus_register (bus, NULL);
  atspi_dbus_connection_setup_with_g_main(bus, g_main_context_default());
  dbus_connection_add_filter (bus, atspi_dbus_filter, NULL, NULL);
  match = g_strdup_printf ("type='signal',interface='%s',member='AddAccessible'", atspi_interface_cache);
  dbus_bus_add_match (bus, match, NULL);
  g_free (match);
  match = g_strdup_printf ("type='signal',interface='%s',member='RemoveAccessible'", atspi_interface_cache);
  dbus_bus_add_match (bus, match, NULL);
  g_free (match);
  match = g_strdup_printf ("type='signal',interface='%s',member='ChildrenChanged'", atspi_interface_event_object);
  dbus_bus_add_match (bus, match, NULL);
  g_free (match);
  match = g_strdup_printf ("type='signal',interface='%s',member='PropertyChange'", atspi_interface_event_object);
  dbus_bus_add_match (bus, match, NULL);
  g_free (match);
  match = g_strdup_printf ("type='signal',interface='%s',member='StateChanged'", atspi_interface_event_object);
  dbus_bus_add_match (bus, match, NULL);
  g_free (match);

  dbus_bus_add_match (bus,
                      "type='signal', interface='org.freedesktop.DBus', member='NameOwnerChanged'",
                      NULL);

  no_cache = g_getenv ("ATSPI_NO_CACHE");
  if (no_cache && g_strcmp0 (no_cache, "0") != 0)
    atspi_no_cache = TRUE;

  deferred_messages = g_queue_new ();

  return 0;
}

/**
 * atspi_is_initialized:
 *
 * Indicates whether AT-SPI has been initialized.
 *
 * Returns: %True if initialized; %False otherwise.
 */
gboolean
atspi_is_initialized ()
{
  return atspi_inited;
}

/**
 * atspi_event_main:
 *
 * Starts/enters the main event loop for the AT-SPI services.
 *
 * NOTE: This method does not return control; it is exited via a call to
 * #atspi_event_quit from within an event handler.
 *
 **/
void
atspi_event_main (void)
{
  atspi_main_loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (atspi_main_loop);
  atspi_main_loop = NULL;
}

/**
 * atspi_event_quit:
 *
 * Quits the last main event loop for the AT-SPI services,
 * See: #atspi_event_main
 **/
void
atspi_event_quit (void)
{
  g_main_loop_quit (atspi_main_loop);
}

/**
 * atspi_exit:
 *
 * Disconnects from #AtspiRegistry instances and releases 
 * any floating resources. Call only once at exit.
 *
 * Returns: 0 if there were no leaks, otherwise other integer values.
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

static GSList *hung_processes;

static void
remove_hung_process (DBusPendingCall *pending, void *data)
{

  hung_processes = g_slist_remove (hung_processes, data);
  g_free (data);
  dbus_pending_call_unref (pending);
}

static void
check_for_hang (DBusMessage *message, DBusError *error, DBusConnection *bus, const char *bus_name)
{
  if (!message && error->name &&
      !strcmp (error->name, "org.freedesktop.DBus.Error.NoReply"))
  {
    GSList *l;
    DBusMessage *message;
    gchar *bus_name_dup;
    DBusPendingCall *pending = NULL;
    for (l = hung_processes; l; l = l->next)
      if (!strcmp (l->data, bus_name))
        return;
    message = dbus_message_new_method_call (bus_name, "/",
                                            "org.freedesktop.DBus.Peer",
                                            "Ping");
    if (!message)
      return;
    dbus_connection_send_with_reply (bus, message, &pending, -1);
    dbus_message_unref (message);
    if (!pending)
      return;
    bus_name_dup = g_strdup (bus_name);
    hung_processes = g_slist_append (hung_processes, bus_name_dup);
    dbus_pending_call_set_notify (pending, remove_hung_process, bus_name_dup, NULL);
  }
}

static gboolean
connection_is_hung (const char *bus_name)
{
  GSList *l;

  for (l = hung_processes; l; l = l->next)
    if (!strcmp (l->data, bus_name))
      return TRUE;
  return FALSE;
}

static gboolean
check_app (AtspiApplication *app, GError **error)
{
  if (!app || !app->bus)
  {
    g_set_error_literal (error, ATSPI_ERROR, ATSPI_ERROR_APPLICATION_GONE,
                          _("The application no longer exists"));
    return FALSE;
  }

  if (atspi_main_loop && connection_is_hung (app->bus_name))
  {
      g_set_error_literal (error, ATSPI_ERROR, ATSPI_ERROR_IPC,
                           "The process appears to be hung.");
    return FALSE;
  }

  return TRUE;
}

static void
set_timeout (AtspiApplication *app)
{
  struct timeval tv;
  int diff;

  if (app && app_startup_time > 0)
  {
    gettimeofday (&tv, NULL);
    diff = (tv.tv_sec - app->time_added.tv_sec) * 1000 + (tv.tv_usec - app->time_added.tv_usec) / 1000;
    dbind_set_timeout (MAX(method_call_timeout, app_startup_time - diff));
  }
  else
    dbind_set_timeout (method_call_timeout);
}

dbus_bool_t
_atspi_dbus_call (gpointer obj, const char *interface, const char *method, GError **error, const char *type, ...)
{
  va_list args;
  dbus_bool_t retval;
  DBusError err;
  AtspiObject *aobj = ATSPI_OBJECT (obj);

  if (!check_app (aobj->app, error))
    return FALSE;

  if (!allow_sync)
  {
    _atspi_set_error_no_sync (error);
    return FALSE;
  }

  va_start (args, type);
  dbus_error_init (&err);
  set_timeout (aobj->app);
  retval = dbind_method_call_reentrant_va (aobj->app->bus, aobj->app->bus_name,
                                           aobj->path, interface, method, &err,
                                           type, args);
  va_end (args);
  check_for_hang (NULL, &err, aobj->app->bus, aobj->app->bus_name);
  process_deferred_messages ();
  if (dbus_error_is_set (&err))
  {
    g_set_error(error, ATSPI_ERROR, ATSPI_ERROR_IPC, "%s", err.message);
    dbus_error_free (&err);
  }
  return retval;
}

DBusMessage *
_atspi_dbus_call_partial (gpointer obj,
                          const char *interface,
                          const char *method,
                          GError **error,
                          const char *type, ...)
{
  va_list args;

  va_start (args, type);
  return _atspi_dbus_call_partial_va (obj, interface, method, error, type, args);
}


DBusMessage *
_atspi_dbus_call_partial_va (gpointer obj,
                          const char *interface,
                          const char *method,
                          GError **error,
                          const char *type,
                          va_list args)
{
  AtspiObject *aobj = ATSPI_OBJECT (obj);
  DBusError err;
    DBusMessage *msg = NULL, *reply = NULL;
    DBusMessageIter iter;
    const char *p;

  dbus_error_init (&err);

  if (!check_app (aobj->app, error))
    goto out;

  msg = dbus_message_new_method_call (aobj->app->bus_name, aobj->path, interface, method);
  if (!msg)
    goto out;

  p = type;
  dbus_message_iter_init_append (msg, &iter);
  dbind_any_marshal_va (&iter, &p, args);

  set_timeout (aobj->app);
  reply = dbind_send_and_allow_reentry (aobj->app->bus, msg, &err);
  check_for_hang (reply, &err, aobj->app->bus, aobj->app->bus_name);
out:
  va_end (args);
  if (msg)
    dbus_message_unref (msg);
  process_deferred_messages ();
  if (dbus_error_is_set (&err))
  {
    /* TODO: Set gerror */
    dbus_error_free (&err);
  }

  if (reply && dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR)
  {
    const char *err_str = NULL;
    dbus_message_get_args (reply, NULL, DBUS_TYPE_STRING, &err_str, DBUS_TYPE_INVALID);
    if (err_str)
      g_set_error_literal (error, ATSPI_ERROR, ATSPI_ERROR_IPC, err_str);
    dbus_message_unref (reply);
    return NULL;
  }

  return reply;
}

dbus_bool_t
_atspi_dbus_get_property (gpointer obj, const char *interface, const char *name, GError **error, const char *type, void *data)
{
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_variant;
  DBusError err;
  dbus_bool_t retval = FALSE;
  AtspiObject *aobj = ATSPI_OBJECT (obj);
  char expected_type = (type [0] == '(' ? 'r' : type [0]);

  if (!aobj)
    return FALSE;

  if (!check_app (aobj->app, error))
    return FALSE;

  if (!allow_sync)
  {
    _atspi_set_error_no_sync (error);
    return FALSE;
  }

  message = dbus_message_new_method_call (aobj->app->bus_name,
                                          aobj->path,
                                          "org.freedesktop.DBus.Properties",
                                          "Get");
  if (!message)
  {
    // TODO: throw exception
    return FALSE;
  }
  dbus_message_append_args (message, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
  dbus_error_init (&err);
  set_timeout (aobj->app);
  reply = dbind_send_and_allow_reentry (aobj->app->bus, message, &err);
  check_for_hang (reply, &err, aobj->app->bus, aobj->app->bus_name);
  dbus_message_unref (message);
  process_deferred_messages ();
  if (!reply)
  {
    // TODO: throw exception
    goto done;
  }

  if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR)
  {
    const char *err_str = NULL;
    dbus_message_get_args (reply, NULL, DBUS_TYPE_STRING, &err_str, DBUS_TYPE_INVALID);
    if (err_str)
      g_set_error_literal (error, ATSPI_ERROR, ATSPI_ERROR_IPC, err_str);
    goto done;
  }

  dbus_message_iter_init (reply, &iter);
  if (dbus_message_iter_get_arg_type (&iter) != 'v')
  {
    g_warning ("AT-SPI: expected a variant when fetching %s from interface %s; got %s\n", name, interface, dbus_message_get_signature (reply));
    goto done;
  }
  dbus_message_iter_recurse (&iter, &iter_variant);
  if (dbus_message_iter_get_arg_type (&iter_variant) != expected_type)
  {
    g_warning ("atspi_dbus_get_property: Wrong type: expected %s, got %c\n", type, dbus_message_iter_get_arg_type (&iter_variant));
    goto done;
  }
  if (!strcmp (type, "(so)"))
  {
    *((AtspiAccessible **)data) = _atspi_dbus_return_accessible_from_iter (&iter_variant);
  }
  else
  {
    dbus_message_iter_get_basic (&iter_variant, data);
    if (type [0] == 's')
      *(char **)data = g_strdup (*(char **)data);
  }
  retval = TRUE;
done:
  dbus_error_free (&err);
  if (reply)
    dbus_message_unref (reply);
  return retval;
}

DBusMessage *
_atspi_dbus_send_with_reply_and_block (DBusMessage *message, GError **error)
{
  DBusMessage *reply;
  DBusError err;
  AtspiApplication *app;
  DBusConnection *bus;

  app = get_application (dbus_message_get_destination (message));

  if (app && !app->bus)
    return NULL;	/* will fail anyway; app has been disposed */

  bus = (app ? app->bus : _atspi_bus());
  dbus_error_init (&err);
  set_timeout (app);
  reply = dbind_send_and_allow_reentry (bus, message, &err);
  process_deferred_messages ();
  dbus_message_unref (message);
  if (dbus_error_is_set (&err))
  {
    if (error)
      g_set_error_literal (error, ATSPI_ERROR, ATSPI_ERROR_IPC, err.message);
    dbus_error_free (&err);
  }
  return reply;
}

GHashTable *
_atspi_dbus_return_hash_from_message (DBusMessage *message)
{
  DBusMessageIter iter;
  GHashTable *ret;

  if (!message)
    return NULL;

  _ATSPI_DBUS_CHECK_SIG (message, "a{ss}", NULL, NULL);

  dbus_message_iter_init (message, &iter);
  ret = _atspi_dbus_hash_from_iter (&iter);
  dbus_message_unref (message);
  return ret;
}

GHashTable *
_atspi_dbus_hash_from_iter (DBusMessageIter *iter)
{
  GHashTable *hash = g_hash_table_new_full (g_str_hash, g_str_equal,
                                            (GDestroyNotify) g_free,
                                            (GDestroyNotify) g_free);
  DBusMessageIter iter_array, iter_dict;

  dbus_message_iter_recurse (iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    const char *name, *value;
    dbus_message_iter_recurse (&iter_array, &iter_dict);
    dbus_message_iter_get_basic (&iter_dict, &name);
    dbus_message_iter_next (&iter_dict);
    dbus_message_iter_get_basic (&iter_dict, &value);
    g_hash_table_insert (hash, g_strdup (name), g_strdup (value));
    dbus_message_iter_next (&iter_array);
  }
  return hash;
}

GArray *
_atspi_dbus_return_attribute_array_from_message (DBusMessage *message)
{
  DBusMessageIter iter;
  GArray *ret;

  if (!message)
    return NULL;

  _ATSPI_DBUS_CHECK_SIG (message, "a{ss}", NULL, NULL);

  dbus_message_iter_init (message, &iter);

  ret = _atspi_dbus_attribute_array_from_iter (&iter);
  dbus_message_unref (message);
  return ret;
}

GArray *
_atspi_dbus_attribute_array_from_iter (DBusMessageIter *iter)
{
  DBusMessageIter iter_array, iter_dict;
  GArray *array = g_array_new (TRUE, TRUE, sizeof (gchar *));

  dbus_message_iter_recurse (iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    const char *name, *value;
    gchar *str;
    dbus_message_iter_recurse (&iter_array, &iter_dict);
    dbus_message_iter_get_basic (&iter_dict, &name);
    dbus_message_iter_next (&iter_dict);
    dbus_message_iter_get_basic (&iter_dict, &value);
    str = g_strdup_printf ("%s:%s", name, value);
    array = g_array_append_val (array, str);
    dbus_message_iter_next (&iter_array);;
  }
  return array;
}

void
_atspi_dbus_set_interfaces (AtspiAccessible *accessible, DBusMessageIter *iter)
{
  DBusMessageIter iter_array;
  char *iter_sig = dbus_message_iter_get_signature (iter);

  accessible->interfaces = 0;
  if (strcmp (iter_sig, "as") != 0)
  {
    g_warning ("_atspi_dbus_set_interfaces: Passed iterator with invalid signature %s", dbus_message_iter_get_signature (iter));
    dbus_free (iter_sig);
    return;
  }
  dbus_free (iter_sig);
  dbus_message_iter_recurse (iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    const char *iface;
    gint n;
    dbus_message_iter_get_basic (&iter_array, &iface);
    if (!strcmp (iface, "org.freedesktop.DBus.Introspectable")) continue;
    n = _atspi_get_iface_num (iface);
    if (n == -1)
    {
      g_warning ("AT-SPI: Unknown interface %s", iface);
    }
    else
      accessible->interfaces |= (1 << n);
    dbus_message_iter_next (&iter_array);
  }
  _atspi_accessible_add_cache (accessible, ATSPI_CACHE_INTERFACES);
}

void
_atspi_dbus_set_state (AtspiAccessible *accessible, DBusMessageIter *iter)
{
  DBusMessageIter iter_array;
  gint count;
  dbus_uint32_t *states;

  dbus_message_iter_recurse (iter, &iter_array);
  dbus_message_iter_get_fixed_array (&iter_array, &states, &count);
  if (count != 2)
  {
    g_warning ("AT-SPI: expected 2 values in states array; got %d\n", count);
    if (!accessible->states)
      accessible->states = _atspi_state_set_new_internal (accessible, 0);
  }
  else
  {
    guint64 val = ((guint64)states [1]) << 32;
    val += states [0];
    if (!accessible->states)
      accessible->states = _atspi_state_set_new_internal (accessible, val);
    else
      accessible->states->states = val;
  }
  _atspi_accessible_add_cache (accessible, ATSPI_CACHE_STATES);
}

GQuark
_atspi_error_quark (void)
{
  return g_quark_from_static_string ("atspi_error");
}

/*
 * Gets the IOR from the XDisplay.
 */
#ifdef HAVE_X11
static char *
get_accessibility_bus_address_x11 (void)
{
  Atom AT_SPI_BUS;
  Atom actual_type;
  Display *bridge_display = NULL;
  int actual_format;
  char *data;
  unsigned char *data_x11 = NULL;
  unsigned long nitems;
  unsigned long leftover;
  char *display_name;

  display_name = spi_display_name ();
  if (!display_name)
    return NULL;

  bridge_display = XOpenDisplay (display_name);
  g_free (display_name);

  if (!bridge_display)
    {
      g_warning ("Could not open X display");
      return NULL;
    }
      
  AT_SPI_BUS = XInternAtom (bridge_display, "AT_SPI_BUS", False);
  XGetWindowProperty (bridge_display,
		      XDefaultRootWindow (bridge_display),
		      AT_SPI_BUS, 0L,
		      (long) BUFSIZ, False,
		      (Atom) 31, &actual_type, &actual_format,
		      &nitems, &leftover, &data_x11);
  XCloseDisplay (bridge_display);

  data = g_strdup ((gchar *)data_x11);
  XFree (data_x11);
  return data;
}
#endif

static char *
get_accessibility_bus_address_dbus (void)
{
  DBusConnection *session_bus = NULL;
  DBusMessage *message;
  DBusMessage *reply;
  DBusError error;
  char *address = NULL;

  session_bus = dbus_bus_get (DBUS_BUS_SESSION, NULL);
  if (!session_bus)
    return NULL;

  message = dbus_message_new_method_call ("org.a11y.Bus",
					  "/org/a11y/bus",
					  "org.a11y.Bus",
					  "GetAddress");

  dbus_error_init (&error);
  reply = dbus_connection_send_with_reply_and_block (session_bus,
						     message,
						     -1,
						     &error);
  dbus_message_unref (message);

  if (!reply)
  {
    g_warning ("Error retrieving accessibility bus address: %s: %s",
               error.name, error.message);
    dbus_error_free (&error);
    return NULL;
  }
  
  {
    const char *tmp_address;
    if (!dbus_message_get_args (reply,
				NULL,
				DBUS_TYPE_STRING,
				&tmp_address,
				DBUS_TYPE_INVALID))
      {
	dbus_message_unref (reply);
	return NULL;
      }
    address = g_strdup (tmp_address);
    dbus_message_unref (reply);
  }
  
  return address;
}

static DBusConnection *a11y_bus;
static dbus_int32_t a11y_dbus_slot = -1;

static void
a11y_bus_free (void *data)
{
  if (data == a11y_bus)
    {
      a11y_bus = NULL;
      dbus_connection_free_data_slot (&a11y_dbus_slot);
    }
}

/**
 * atspi_get_a11y_bus: (skip)
 */
DBusConnection *
atspi_get_a11y_bus (void)
{
  DBusError error;
  char *address = NULL;

  if (a11y_bus && dbus_connection_get_is_connected (a11y_bus))
    return a11y_bus;

  if (a11y_dbus_slot == -1)
    if (!dbus_connection_allocate_data_slot (&a11y_dbus_slot))
      g_warning ("at-spi: Unable to allocate D-Bus slot");

#ifdef HAVE_X11
  address = get_accessibility_bus_address_x11 ();
#endif
  if (!address)
    address = get_accessibility_bus_address_dbus ();
  if (!address)
    return NULL;

  dbus_error_init (&error);
  a11y_bus = dbus_connection_open_private (address, &error);
  g_free (address);

  if (!a11y_bus)
    {
      if (!g_getenv("SSH_CONNECTION"))
        g_warning ("Couldn't connect to accessibility bus: %s", error.message);
      dbus_error_free (&error);
      return NULL;
    }
  else
    {
      if (!dbus_bus_register (a11y_bus, &error))
	{
	  g_warning ("Couldn't register with accessibility bus: %s", error.message);
          dbus_error_free (&error);
          dbus_connection_close (a11y_bus);
          dbus_connection_unref (a11y_bus);
          a11y_bus = NULL;
	  return NULL;
	}
    }
  
  /* Simulate a weak ref on the bus */
  dbus_connection_set_data (a11y_bus, a11y_dbus_slot, a11y_bus, a11y_bus_free);

  return a11y_bus;
}

/**
 * atspi_set_timeout:
 * @val: The timeout value, in milliseconds, or -1 to disable the timeout.
 * @startup_time: The amount of time, in milliseconds, to allow to pass
 * before enforcing timeouts on an application. Can be used to prevent
 * timeout exceptions if an application is likely to block for an extended
 * period of time on initialization. -1 can be passed to disable this
 * behavior.
 *
 * Set the timeout used for method calls. If this is not set explicitly,
 * a default of 0.8 ms is used.
 * Note that at-spi2-registryd currently uses a timeout of 3 seconds when
 * sending a keyboard event notification. This means that, if an AT makes
 * a call in response to the keyboard notification and the application
 * being called does not respond before the timeout is reached,
 * at-spi2-registryd will time out on the keyboard event notification and
 * pass the key onto the application (ie, reply to indicate that the key
 * was not consumed), so this may make it undesirable to set a timeout
 * larger than 3 seconds.
 *
 * By default, the normal timeout is set to 800 ms, and the application startup
 * timeout is set to 15 seconds.
 */
void
atspi_set_timeout (gint val, gint startup_time)
{
  method_call_timeout = val;
  app_startup_time = startup_time;
}

/**
 * atspi_set_main_context:
 * @cnx: The #GMainContext to use.
 *
 * Sets the main loop context that AT-SPI should assume is in use when
 * setting an idle callback.
 * This function should be called by application-side implementors (ie,
 * at-spi2-atk) when it is desirable to re-enter the main loop.
 */
void
atspi_set_main_context (GMainContext *cnx)
{
  if (atspi_main_context == cnx)
    return;
  if (process_deferred_messages_source != NULL)
  {
    g_source_destroy (process_deferred_messages_source);
    process_deferred_messages_source = g_idle_source_new ();
    g_source_set_callback (process_deferred_messages_source,
                           process_deferred_messages_callback, NULL, NULL);
    g_source_attach (process_deferred_messages_source, cnx);
    g_source_unref (process_deferred_messages_source);
  }
  atspi_main_context = cnx;
  atspi_dbus_connection_setup_with_g_main (atspi_get_a11y_bus (), cnx);
}

#ifdef DEBUG_REF_COUNTS
static void
print_disposed (gpointer key, gpointer value, gpointer data)
{
  AtspiAccessible *accessible = key;
  if (accessible->parent.app)
    return;
  g_print ("disposed: %s %d\n", accessible->name, accessible->role);
}

void
debug_disposed ()
{
  g_hash_table_foreach (live_refs, print_disposed, NULL);
}
#endif

gchar *
_atspi_name_compat (gchar *name)
{
  gchar *p = name;

  while (*p)
  {
    if (*p == '-')
      *p = ' ';
    p++;
  }
  return name;
}

/**
 * atspi_role_get_name:
 * @role: an #AtspiRole object to query.
 *
 * Gets a localizable string that indicates the name of an #AtspiRole.
 * <em>DEPRECATED.</em>
 *
 * Returns: a localizable string name for an #AtspiRole enumerated type.
 **/
gchar *
atspi_role_get_name (AtspiRole role)
{
  gchar *retval = NULL;
  GTypeClass *type_class;
  GEnumValue *value;

  type_class = g_type_class_ref (ATSPI_TYPE_ROLE);
  g_return_val_if_fail (G_IS_ENUM_CLASS (type_class), NULL);

  value = g_enum_get_value (G_ENUM_CLASS (type_class), role);

  if (value)
    {
      retval = g_strdup (value->value_nick);
    }

  g_type_class_unref (type_class);

  if (retval)
    return _atspi_name_compat (retval);

  return NULL;
}

GHashTable *
_atspi_dbus_update_cache_from_dict (AtspiAccessible *accessible, DBusMessageIter *iter)
{
  GHashTable *cache = _atspi_accessible_ref_cache (accessible);
  DBusMessageIter iter_dict, iter_dict_entry, iter_struct, iter_variant;

  dbus_message_iter_recurse (iter, &iter_dict);
  while (dbus_message_iter_get_arg_type (&iter_dict) != DBUS_TYPE_INVALID)
  {
    const char *key;
    GValue *val = NULL;
    dbus_message_iter_recurse (&iter_dict, &iter_dict_entry);
    dbus_message_iter_get_basic (&iter_dict_entry, &key);
    dbus_message_iter_next (&iter_dict_entry);
    dbus_message_iter_recurse (&iter_dict_entry, &iter_variant);
    if (!strcmp (key, "interfaces"))
    {
      _atspi_dbus_set_interfaces (accessible, &iter_variant);
    }
    else if (!strcmp (key, "Attributes"))
    {
      char *iter_sig = dbus_message_iter_get_signature (&iter_variant);
      val = g_new0 (GValue, 1);;
      g_value_init (val, G_TYPE_HASH_TABLE);
      if (strcmp (iter_sig, "a{ss}") != 0)
      {
        dbus_free (iter_sig);
        break;
      }
      dbus_free (iter_sig);
      g_value_take_boxed (val, _atspi_dbus_hash_from_iter (&iter_variant));
    }
    else if (!strcmp (key, "Component.ScreenExtents"))
    {
      dbus_int32_t d_int;
      AtspiRect extents;
      char *iter_sig = dbus_message_iter_get_signature (&iter_variant);
      val = g_new0 (GValue, 1);;
      g_value_init (val, ATSPI_TYPE_RECT);
      if (strcmp (iter_sig, "(iiii)") != 0)
      {
        dbus_free (iter_sig);
        break;
      }
      dbus_free (iter_sig);
      dbus_message_iter_recurse (&iter_variant, &iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &d_int);
      extents.x = d_int;
      dbus_message_iter_next (&iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &d_int);
      extents.y = d_int;
      dbus_message_iter_next (&iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &d_int);
      extents.width = d_int;
      dbus_message_iter_next (&iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &d_int);
      extents.height = d_int;
      g_value_set_boxed (val, &extents);
    }
    if (val)
      g_hash_table_insert (cache, g_strdup (key), val); 
    dbus_message_iter_next (&iter_dict);
  }

  return cache;
}

gboolean
_atspi_get_allow_sync ()
{
  return allow_sync;
}

gboolean
_atspi_set_allow_sync (gboolean val)
{
  gboolean ret = allow_sync;

  allow_sync = val;
  return ret;
}

void
_atspi_set_error_no_sync (GError **error)
{
  g_set_error_literal (error, ATSPI_ERROR, ATSPI_ERROR_SYNC_NOT_ALLOWED,
                        _("Attempted synchronous call where prohibited"));
}
