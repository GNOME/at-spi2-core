/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

/**
 * AtspiMisc:
 *
 * Miscellaneous methods for using AT-SPI services.
 */

/* These are listed here for extraction by intltool */
#if 0
  N_("invalid")
  N_("accelerator label")
  N_("alert")
  N_("animation")
  N_("arrow")
  N_("calendar")
  N_("canvas")
  N_("check box")
  N_("check menu item")
  N_("color chooser")
  N_("column header")
  N_("combo box")
  N_("dateeditor")
  N_("desktop icon")
  N_("desktop frame")
  N_("dial")
  N_("dialog")
  N_("directory pane")
  N_("drawing area")
  N_("file chooser")
  N_("filler")
  /* I know it looks wrong but that is what Java returns */
  N_("fontchooser")
  N_("frame")
  N_("glass pane")
  N_("html container")
  N_("icon")
  N_("image")
  N_("internal frame")
  N_("label")
  N_("layered pane")
  N_("list")
  N_("list item")
  N_("menu")
  N_("menu bar")
  N_("menu button")
  N_("menu item")
  N_("option pane")
  N_("page tab")
  N_("page tab list")
  N_("panel")
  N_("password text")
  N_("popup menu")
  N_("progress bar")
  N_("button")
  N_("radio button")
  N_("radio menu item")
  N_("root pane")
  N_("row header")
  N_("scroll bar")
  N_("scroll pane")
  N_("separator")
  N_("slider")
  N_("split pane")
  N_("spin button")
  N_("statusbar")
  N_("switch")
  N_("table")
  N_("table cell")
  N_("table column header")
  N_("table row header")
  N_("tear off menu item")
  N_("terminal")
  N_("text")
  N_("toggle button")
  N_("tool bar")
  N_("tool tip")
  N_("tree")
  N_("tree table")
  N_("unknown")
  N_("viewport")
  N_("window")
  N_("header")
  N_("footer")
  N_("paragraph")
  N_("ruler")
  N_("application")
  N_("autocomplete")
  N_("edit bar")
  N_("embedded component")
  N_("entry")
  N_("chart")
  N_("caption")
  N_("document frame")
  N_("heading")
  N_("page")
  N_("section")
  N_("redundant object")
  N_("form")
  N_("link")
  N_("input method window")
  N_("table row")
  N_("tree item")
  N_("document spreadsheet")
  N_("document presentation")
  N_("document text")
  N_("document web")
  N_("document email")
  N_("comment")
  N_("list box")
  N_("grouping")
  N_("image map")
  N_("notification")
  N_("info bar")
  N_("level bar")
  N_("title bar")
  N_("block quote")
  N_("audio")
  N_("video")
  N_("definition")
  N_("article")
  N_("landmark")
  N_("log")
  N_("marquee")
  N_("math")
  N_("rating")
  N_("timer")
  N_("description list")
  N_("description term")
  N_("description value")
#endif /* 0 */

static void
_gettext_initialization (void)
{
  static gboolean gettext_initialized = FALSE;

  if (!gettext_initialized)
    {
      gettext_initialized = TRUE;
      setlocale (LC_ALL, "");
      bindtextdomain (GETTEXT_PACKAGE, ATSPI_LOCALEDIR);
      bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    }
}

static void handle_get_items (DBusPendingCall *pending, void *user_data);

static DBusConnection *bus = NULL;
static GHashTable *live_refs = NULL;
static gint method_call_timeout = 800;
static gint app_startup_time = 15000;

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

static const char *interfaces[] = {
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
  ATSPI_DBUS_INTERFACE_SELECTION,
  ATSPI_DBUS_INTERFACE_SOCKET,
  ATSPI_DBUS_INTERFACE_TABLE,
  ATSPI_DBUS_INTERFACE_TABLE_CELL,
  ATSPI_DBUS_INTERFACE_TEXT,
  ATSPI_DBUS_INTERFACE_VALUE,
  NULL
};

/* Holds a dbus object reference as a pair of app_name/path.  These have the lifetime
 * of the DBusMessage that is being processed.
 */
typedef struct
{
  const char *app_name;
  const char *path;
} ReferenceFromMessage;

static void
get_reference_from_iter (DBusMessageIter *iter, ReferenceFromMessage *ref)
{
  DBusMessageIter iter_struct;

  dbus_message_iter_recurse (iter, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &ref->app_name);
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &ref->path);
  dbus_message_iter_next (iter);
}

gint
_atspi_get_iface_num (const char *iface)
{
  /* TODO: Use a binary search or hash to improve performance */
  int i;

  for (i = 0; interfaces[i]; i++)
    {
      if (!strcmp (iface, interfaces[i]))
        return i;
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

static void cleanup_deferred_message (void);

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

  if (!desktop)
    goto end;

  /* TODO: Do we need this code, or should we just dispose the desktop? */
  for (i = desktop->children->len - 1; i >= 0; i--)
    {
      AtspiAccessible *child = g_ptr_array_index (desktop->children, i);
      if (child->parent.app)
        g_object_run_dispose (G_OBJECT (child->parent.app));
      g_object_run_dispose (G_OBJECT (child));
    }

  g_object_run_dispose (G_OBJECT (desktop->parent.app));
  g_object_unref (desktop);
  desktop = NULL;

end:
  if (bus)
    {
      dbus_connection_close (bus);
      dbus_connection_unref (bus);
      bus = NULL;
    }

  cleanup_deferred_message ();
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
                                 DBUS_TYPE_INVALID) &&
          address[0])
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
              atspi_dbus_connection_setup_with_g_main (bus, g_main_context_default ());
            }
          else
            {
              if (!strcmp (error.name, DBUS_ERROR_FILE_NOT_FOUND) &&
                  !g_getenv ("ATSPI_IN_TESTS"))
                g_warning ("AT-SPI: Unable to open bus connection: %s", error.message);
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
      app_hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GDestroyNotify) g_object_unref);
    }
  app = g_hash_table_lookup (app_hash, bus_name);
  if (app)
    return app;
  bus_name_dup = g_strdup (bus_name);

  // TODO: change below to something that will send state-change:defunct notification if necessary */
  app = _atspi_application_new (bus_name);
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
ref_accessible (ReferenceFromMessage *ref)
{
  AtspiApplication *app;
  AtspiAccessible *a;

  if (!strcmp (ref->path, ATSPI_DBUS_PATH_NULL))
    return NULL;

  app = get_application (ref->app_name);

  if (!strcmp (ref->path, "/org/a11y/atspi/accessible/root"))
    {
      if (!app->root)
        {
          app->root = _atspi_accessible_new (app, atspi_path_root);
          app->root->accessible_parent = atspi_get_desktop (0);
          g_ptr_array_add (app->root->accessible_parent->children, g_object_ref (app->root));
        }
      return g_object_ref (app->root);
    }

  a = g_hash_table_lookup (app->hash, ref->path);
  if (a)
    {
      return g_object_ref (a);
    }
  a = _atspi_accessible_new (app, ref->path);
  g_hash_table_insert (app->hash, g_strdup (a->parent.path), a);
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
handle_remove_accessible (DBusConnection *bus, DBusMessage *message)
{
  ReferenceFromMessage ref;
  DBusMessageIter iter;
  const char *signature = dbus_message_get_signature (message);
  AtspiAccessible *a;

  if (strcmp (signature, "(so)") != 0)
    {
      g_warning ("AT-SPI: Unknown signature %s for RemoveAccessible", signature);
      return DBUS_HANDLER_RESULT_HANDLED;
    }

  dbus_message_iter_init (message, &iter);

  get_reference_from_iter (&iter, &ref);
  a = ref_accessible (&ref);
  if (!a)
    return DBUS_HANDLER_RESULT_HANDLED;
  g_object_run_dispose (G_OBJECT (a));
  g_object_unref (a); /* unref our own ref */
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult
handle_name_owner_changed (DBusConnection *bus, DBusMessage *message)
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
      if (app && !strcmp (app->bus_name, old))
        g_object_run_dispose (G_OBJECT (app));
    }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static gboolean
add_app_to_desktop (AtspiAccessible *a, const char *bus_name)
{
  ReferenceFromMessage ref = {
    .app_name = bus_name,
    .path = atspi_path_root,
  };
  AtspiAccessible *obj = ref_accessible (&ref);
  /* The app will be added to the desktop as a side-effect of calling
   * ref_accessible */
  g_object_unref (obj);
  return (obj != NULL);
}

static void
add_accessible_from_iter (DBusMessageIter *iter)
{
  DBusMessageIter iter_struct, iter_array;
  AtspiAccessible *accessible;
  AtspiAccessible *parent;
  const char *name, *description;
  dbus_uint32_t role;
  gboolean children_cached = FALSE;
  dbus_int32_t count, index;

  dbus_message_iter_recurse (iter, &iter_struct);

  /* get accessible */
  accessible = _atspi_dbus_consume_accessible (&iter_struct);
  if (!accessible)
    return;

  /* Get application: TODO */
  dbus_message_iter_next (&iter_struct);

  /* get parent */
  parent = _atspi_dbus_consume_accessible (&iter_struct);
  if (accessible->accessible_parent)
    g_object_unref (accessible->accessible_parent);
  if (parent == accessible)
    {
      guint pid = atspi_accessible_get_process_id (accessible, NULL);
      if (!g_getenv ("ATSPI_IN_TESTS"))
        g_warning ("Process %d sent an accessible with itself as its parent. This shouldn't happen.", pid);
      accessible->accessible_parent = NULL;
    }
  else
    accessible->accessible_parent = parent;

  if (dbus_message_iter_get_arg_type (&iter_struct) == 'i')
    {
      /* Get index in parent */
      dbus_message_iter_get_basic (&iter_struct, &index);
      if (index >= 0 && index < ATSPI_MAX_CHILDREN && accessible->accessible_parent)
        {
          AtspiAccessible *old_child = NULL;
          if (index >= accessible->accessible_parent->children->len)
            {
              /* There is no room for this object */
              g_ptr_array_set_size (accessible->accessible_parent->children, index + 1);
            }
          else
            {
              /* This place is already taken - let's free this place with dignity */
              if (g_ptr_array_index (accessible->accessible_parent->children, index))
                old_child = g_ptr_array_index (accessible->accessible_parent->children, index);
            }
          g_ptr_array_index (accessible->accessible_parent->children, index) = g_object_ref (accessible);
          g_clear_object (&old_child);
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
          child = _atspi_dbus_consume_accessible (&iter_array);
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

  _atspi_accessible_set_cached (accessible, TRUE);

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
      const char *error_name = dbus_message_get_error_name (reply);
      if (!strcmp (error_name, DBUS_ERROR_SERVICE_UNKNOWN) || !strcmp (error_name, DBUS_ERROR_NO_REPLY))
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
  g_hash_table_insert (app->hash, g_strdup (desktop->parent.path), desktop);
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
      ReferenceFromMessage ref;

      get_reference_from_iter (&iter_array, &ref);
      add_app_to_desktop (desktop, ref.app_name);
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
  ReferenceFromMessage ref = {
    .app_name = app,
    .path = path,
  };
  AtspiApplication *a = get_application (app);
  if (!a)
    return NULL;
  if (APP_IS_REGISTRY (a))
    {
      if (!a->root)
        g_object_unref (ref_accessible_desktop (a)); /* sets a->root */
      return g_object_ref (a->root);
    }
  return ref_accessible (&ref);
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
      retval = _atspi_dbus_consume_accessible (&iter);
    }
  else
    {
      g_warning ("AT-SPI: Called _atspi_dbus_return_accessible_from_message with strange signature %s", signature);
    }
  dbus_message_unref (message);
  return retval;
}

/* Enters an iter which must be already pointing to a (so) and returns the accessible for it */
AtspiAccessible *
_atspi_dbus_consume_accessible (DBusMessageIter *iter)
{
  ReferenceFromMessage ref;

  get_reference_from_iter (iter, &ref);
  return ref_accessible (&ref);
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
      retval = _atspi_dbus_return_hyperlink_from_iter (&iter);
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
  ReferenceFromMessage ref;

  get_reference_from_iter (iter, &ref);
  return ref_hyperlink (ref.app_name, ref.path);
}

const char *cache_signal_type = "((so)(so)(so)iiassusau)";
const char *old_cache_signal_type = "((so)(so)(so)a(so)assusau)";

static DBusHandlerResult
handle_add_accessible (DBusConnection *bus, DBusMessage *message)
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
      _atspi_dbus_handle_event (closure->message);
    }
  else if (dbus_message_is_method_call (closure->message, atspi_interface_device_event_listener, "NotifyEvent"))
    {
      _atspi_dbus_handle_DeviceEvent (closure->bus, closure->message);
    }
  else if (dbus_message_is_signal (closure->message, atspi_interface_cache, "AddAccessible"))
    {
      handle_add_accessible (closure->bus, closure->message);
    }
  else if (dbus_message_is_signal (closure->message, atspi_interface_cache, "RemoveAccessible"))
    {
      handle_remove_accessible (closure->bus, closure->message);
    }
  else if (dbus_message_is_signal (closure->message, "org.freedesktop.DBus", "NameOwnerChanged"))
    {
      handle_name_owner_changed (closure->bus, closure->message);
    }
}

static GQueue *deferred_messages = NULL;

static void
destroy_deferred_message_item (gpointer ptr)
{
  BusDataClosure *c = ptr;
  dbus_message_unref (c->message);
  dbus_connection_unref (c->bus);
  g_free (c);
}

static void
cleanup_deferred_message (void)
{
  if (deferred_messages)
    {
      g_queue_free_full (deferred_messages, destroy_deferred_message_item);
      deferred_messages = NULL;
    }
}

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
      destroy_deferred_message_item (closure);
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
defer_message (DBusConnection *connection, DBusMessage *message)
{
  BusDataClosure *closure = g_new (BusDataClosure, 1);

  closure->bus = dbus_connection_ref (bus);
  closure->message = dbus_message_ref (message);

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
  /* Check that we don't start passing stuff from whatever calls dbus_connection_add_filter() */
  g_assert (data == NULL);

  int type = dbus_message_get_type (message);
  const char *interface = dbus_message_get_interface (message);

  if (type == DBUS_MESSAGE_TYPE_SIGNAL &&
      !strncmp (interface, "org.a11y.atspi.Event.", 21))
    {
      return defer_message (bus, message);
    }
  if (dbus_message_is_method_call (message, atspi_interface_device_event_listener, "NotifyEvent"))
    {
      return defer_message (bus, message);
    }
  if (dbus_message_is_signal (message, atspi_interface_cache, "AddAccessible"))
    {
      return defer_message (bus, message);
    }
  if (dbus_message_is_signal (message, atspi_interface_cache, "RemoveAccessible"))
    {
      return defer_message (bus, message);
    }
  if (dbus_message_is_signal (message, "org.freedesktop.DBus", "NameOwnerChanged"))
    {
      defer_message (bus, message);
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
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

  _atspi_get_live_refs ();

  bus = atspi_get_a11y_bus ();
  if (!bus)
    return 2;
  dbus_bus_register (bus, NULL);
  atspi_dbus_connection_setup_with_g_main (bus, g_main_context_default ());
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
  g_main_loop_unref (atspi_main_loop);
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
  if (atspi_main_loop)
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
                           _ ("The application no longer exists"));
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
      dbind_set_timeout (MAX (method_call_timeout, app_startup_time - diff));
    }
  else
    dbind_set_timeout (method_call_timeout);
}

/* Makes a DBus call and returns a success value.  Simple return values can be demarshaled automatically
 * by passing their types after a "=>" marker in the @type argument (e.g. s=>i for a call that takes a string
 * and returns an int).
 */
dbus_bool_t
_atspi_dbus_call (gpointer obj, const char *interface, const char *method, GError **error, const char *type, ...)
{
  va_list args;
  dbus_bool_t retval;
  DBusError err;
  AtspiObject *aobj = ATSPI_OBJECT (obj);

  if (!check_app (aobj->app, error))
    return FALSE;

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
      g_set_error (error, ATSPI_ERROR, ATSPI_ERROR_IPC, "%s", err.message);
      dbus_error_free (&err);
    }
  return retval;
}

static DBusMessage *
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

/* Makes a DBus call but returns the raw DBusMessage reply.  Use this
 * when you need to demarshal complex return values by hand.  Remember
 * to dbus_message_unref() the return value when you are done.
 */
DBusMessage *
_atspi_dbus_call_partial (gpointer obj,
                          const char *interface,
                          const char *method,
                          GError **error,
                          const char *type,
                          ...)
{
  DBusMessage *ret;
  va_list args;

  va_start (args, type);
  ret = _atspi_dbus_call_partial_va (obj, interface, method, error, type, args);
  va_end (args);

  return ret;
}

dbus_bool_t
_atspi_dbus_get_property (gpointer obj, const char *interface, const char *name, GError **error, const char *type, void *data)
{
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_variant;
  DBusError err;
  dbus_bool_t retval = FALSE;
  AtspiObject *aobj = ATSPI_OBJECT (obj);
  char expected_type = (type[0] == '(' ? 'r' : type[0]);

  if (!aobj)
    return FALSE;

  if (!check_app (aobj->app, error))
    return FALSE;

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
      /* TODO: throw exception */
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
      g_warning ("atspi_dbus_get_property: expected a variant when fetching %s:%s; got %s instead", interface, name, dbus_message_get_signature (reply));
      goto done;
    }
  dbus_message_iter_recurse (&iter, &iter_variant);
  if (dbus_message_iter_get_arg_type (&iter_variant) != expected_type)
    {
      g_warning ("atspi_dbus_get_property: Wrong type: expected %s when fetching %s:%s; got %c instead", type, interface, name, dbus_message_iter_get_arg_type (&iter_variant));
      goto done;
    }
  if (!strcmp (type, "(so)"))
    {
      *((AtspiAccessible **) data) = _atspi_dbus_consume_accessible (&iter_variant);
    }
  else
    {
      if (type[0] == 's')
        {
          *(char **) data = NULL;
        }

      dbus_message_iter_get_basic (&iter_variant, data);
      if (type[0] == 's')
        *(char **) data = g_strdup (*(char **) data);
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
    return NULL; /* will fail anyway; app has been disposed */

  bus = (app ? app->bus : _atspi_bus ());
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
      dbus_message_iter_next (&iter_array);
      ;
    }
  return array;
}

typedef enum
{
  DEMARSHAL_STATUS_SUCCESS,
  DEMARSHAL_STATUS_INVALID_SIGNATURE,
  DEMARSHAL_STATUS_INVALID_VALUE,
} DemarshalStatus;

typedef struct
{
  /* array of (char *) */
  GPtrArray *names;
} InterfaceNames;

static DemarshalStatus
interface_names_demarshal (DBusMessageIter *iter, InterfaceNames **out_interfaces)
{
  char *sig = dbus_message_iter_get_signature (iter);
  gboolean matches = strcmp (sig, "as") == 0;
  dbus_free (sig);

  *out_interfaces = NULL;

  GPtrArray *names = g_ptr_array_new_with_free_func (g_free);

  if (!matches)
    {
      return DEMARSHAL_STATUS_INVALID_SIGNATURE;
    }

  DBusMessageIter iter_array;
  dbus_message_iter_recurse (iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      const char *iface;
      dbus_message_iter_get_basic (&iter_array, &iface);
      g_ptr_array_add (names, g_strdup (iface));
      dbus_message_iter_next (&iter_array);
    }

  InterfaceNames *ifaces = g_new0 (InterfaceNames, 1);
  ifaces->names = names;
  *out_interfaces = ifaces;
  return DEMARSHAL_STATUS_SUCCESS;
}

/* Converts an array of interface names to a value suitable for AtspiAccessible.interfaces */
static gint
interface_names_to_bitmask (const InterfaceNames *ifaces)
{
  gint val = 0;
  guint i;

  g_assert (ifaces->names != NULL);

  for (i = 0; i < ifaces->names->len; i++)
    {
      const char *name = g_ptr_array_index (ifaces->names, i);
      gint iface_num = _atspi_get_iface_num (name);
      if (iface_num == -1)
        {
          g_warning ("AT-SPI: Unknown interface %s", name);
        }
      else
        {
          val |= (1 << iface_num);
        }
    }

  return val;
}

static void
interface_names_free (InterfaceNames *ifaces)
{
  g_ptr_array_free (ifaces->names, TRUE);
  g_free (ifaces);
}

void
_atspi_dbus_set_interfaces (AtspiAccessible *accessible, DBusMessageIter *iter)
{
  InterfaceNames *ifaces;

  accessible->interfaces = 0;

  if (interface_names_demarshal (iter, &ifaces) != DEMARSHAL_STATUS_SUCCESS)
    {
      g_warning ("Passed iterator with invalid signature");
      return;
    }

  accessible->interfaces = interface_names_to_bitmask (ifaces);
  interface_names_free (ifaces);

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
      guint64 val = ((guint64) states[1]) << 32;
      val += states[0];
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

  data = g_strdup ((gchar *) data_x11);
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
      g_warning ("AT-SPI: Error retrieving accessibility bus address: %s: %s",
                 error.name, error.message);
      dbus_error_free (&error);
      goto out;
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
        goto out;
      }
    address = g_strdup (tmp_address);
    dbus_message_unref (reply);
  }

out:
  dbus_connection_unref (session_bus);
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
  const char *address_env = NULL;

  if (a11y_bus && dbus_connection_get_is_connected (a11y_bus))
    return a11y_bus;

  if (a11y_dbus_slot == -1)
    if (!dbus_connection_allocate_data_slot (&a11y_dbus_slot))
      g_warning ("AT-SPI: Unable to allocate D-Bus slot");

  address_env = g_getenv ("AT_SPI_BUS_ADDRESS");
  if (address_env != NULL && *address_env != 0)
    address = g_strdup (address_env);
#ifdef HAVE_X11
  if (!address && g_getenv ("DISPLAY") != NULL &&
      g_getenv ("WAYLAND_DISPLAY") == NULL)
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
      if (!g_getenv ("SSH_CONNECTION"))
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
 * a default of 800 ms is used.
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

  if (desktop)
    {
      gint i;
      for (i = desktop->children->len - 1; i >= 0; i--)
        {
          AtspiAccessible *child = g_ptr_array_index (desktop->children, i);
          if (child->parent.app && child->parent.app->bus)
            atspi_dbus_connection_setup_with_g_main (child->parent.app->bus, cnx);
        }
    }
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

/**
 * atspi_role_get_localized_name:
 * @role: an #AtspiRole object to query.
 *
 * Gets the localized description string describing the #AtspiRole @role.
 *
 * Returns: the localized string describing the AtspiRole
 **/
gchar *
atspi_role_get_localized_name (AtspiRole role)
{
  gchar *raw_name = NULL;
  const char *translated_name;

  _gettext_initialization ();

  switch (role)
    {
    case ATSPI_ROLE_EDITBAR:
      raw_name = g_strdup ("edit bar");
      break;
    default:
      raw_name = atspi_role_get_name (role);
    }
  translated_name = dgettext (GETTEXT_PACKAGE, raw_name);
  if (translated_name != raw_name)
    {
      g_free (raw_name);
      return g_strdup (translated_name);
    }
  else
    return raw_name;
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
          val = g_new0 (GValue, 1);
          ;
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
          val = g_new0 (GValue, 1);
          ;
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

static const char *sr_introspection = "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
                                      "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
                                      "<node name=\"/org/a11y/atspi/screenreader\">\n"
                                      "  <interface name=\"org.a11y.Atspi.ScreenReader\">\n"
                                      "    <signal name=\"ReadingPosition\">\n"
                                      "      <arg type=\"i\"/>\n"
                                      "      <arg type=\"i\"/>\n"
                                      "    </signal>\n"
                                      "  </interface>\n"
                                      "</node>";

static DBusHandlerResult
screen_reader_filter (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  if (dbus_message_is_method_call (message, DBUS_INTERFACE_INTROSPECTABLE,
                                   "Introspect"))
    {
      DBusMessage *reply = dbus_message_new_method_return (message);
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &sr_introspection,
                                DBUS_TYPE_INVALID);
      dbus_connection_send (bus, reply, NULL);
      dbus_message_unref (reply);
      return DBUS_HANDLER_RESULT_HANDLED;
    }
  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

gboolean
_atspi_prepare_screen_reader_interface ()
{
  static gint initialized = 0;
  DBusConnection *a11y_bus = _atspi_bus ();

  if (initialized)
    return (initialized > 0);

  if (dbus_bus_request_name (a11y_bus, "org.a11y.Atspi.ScreenReader", 0, NULL) < 0)
    {
      initialized = -1;
      return FALSE;
    }

  initialized = 1;
  dbus_connection_add_filter (a11y_bus, screen_reader_filter, NULL, NULL);
  return TRUE;
}

gchar *
_atspi_strdup_and_adjust_for_dbus (const char *s)
{
  gchar *d = g_strdup (s);
  gchar *p;
  int parts = 0;

  if (!d)
    return NULL;

  for (p = d; *p; p++)
    {
      if (*p == '-')
        {
          memmove (p, p + 1, g_utf8_strlen (p, -1));
          *p = toupper (*p);
        }
      else if (*p == ':')
        {
          parts++;
          if (parts == 2)
            break;
          p[1] = toupper (p[1]);
        }
    }

  d[0] = toupper (d[0]);
  return d;
}

/**
 * atspi_get_version:
 * @major: (out): the major version.
 * @minor: (out): the minor version.
 * @micro: (out): the micro/patch version.
 *
 * Returns the version of the AT-SPI library being used at runtime.

* Since: 2.50
 */
void
atspi_get_version (gint *major, gint *minor, gint *micro)
{
  if (major)
    *major = ATSPI_MAJOR_VERSION;
  if (minor)
    *minor = ATSPI_MINOR_VERSION;
  if (micro)
    *micro = ATSPI_MICRO_VERSION;
}

gboolean
_atspi_key_is_on_keypad (gint keycode)
{
  switch (keycode)
    {
    case 106: /* / */
    case 63:  /* * */
    case 82:  /* - */
    case 79:  /* 7 */
    case 80:  /* 8 */
    case 91:  /* 9 */
    case 86:  /* + */
    case 83:  /* 4 */
    case 84:  /* 5 */
    case 85:  /* 6 */
    case 87:  /* 1 */
    case 88:  /* 2 */
    case 89:  /* 3 */
    case 104: /* enter */
    case 90:  /* 0 */
    case 81:  /* . */
      return TRUE;
    default:
      return FALSE;
    }
}
