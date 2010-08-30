/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008, 2009 Codethink Ltd.
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
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

#include "config.h"
#include "dbus/dbus-glib-lowlevel.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <atk/atk.h>

#include <droute/droute.h>
#include <gmodule.h>

#include "bridge.h"
#include "event.h"
#include "adaptors.h"
#include "object.h"

#include "accessible-register.h"
#include "accessible-leasing.h"
#include "accessible-cache.h"

#include "common/spi-dbus.h"

/*---------------------------------------------------------------------------*/

SpiBridge *spi_global_app_data = NULL;

static const AtkMisc *atk_misc = NULL;

/*static Display *bridge_display = NULL;*/

/*---------------------------------------------------------------------------*/

/*
 * Returns a 'canonicalized' value for DISPLAY,
 * with the screen number stripped off if present.
 *
 */
static const gchar *
spi_display_name (void)
{
  static const char *canonical_display_name = NULL;
  if (!canonical_display_name)
    {
      const gchar *display_env = g_getenv ("AT_SPI_DISPLAY");
      if (!display_env)
        {
          display_env = g_getenv ("DISPLAY");
          if (!display_env || !display_env[0])
            canonical_display_name = ":0";
          else
            {
              gchar *display_p, *screen_p;
              canonical_display_name = g_strdup (display_env);
              display_p = strrchr (canonical_display_name, ':');
              screen_p = strrchr (canonical_display_name, '.');
              if (screen_p && display_p && (screen_p > display_p))
                {
                  *screen_p = '\0';
                }
            }
        }
      else
        {
          canonical_display_name = display_env;
        }
    }
  return canonical_display_name;
}

/*---------------------------------------------------------------------------*/

/*
 * Gets the IOR from the XDisplay.
 * Not currently used in D-Bus version, but something similar
 * may be employed in the future for accessing the registry daemon
 * bus name.
 */

static DBusConnection *
spi_atk_bridge_get_bus (void)
{
  Atom AT_SPI_BUS;
  Atom actual_type;
  Display *bridge_display;
  int actual_format;
  unsigned char *data = NULL;
  unsigned long nitems;
  unsigned long leftover;

  DBusConnection *bus = NULL;
  DBusError error;

  bridge_display = XOpenDisplay (spi_display_name ());
  if (!bridge_display)
    {
      g_warning ("AT_SPI: Could not get the display\n");
      return NULL;
    }

  AT_SPI_BUS = XInternAtom (bridge_display, "AT_SPI_BUS", False);
  XGetWindowProperty (bridge_display,
                      XDefaultRootWindow (bridge_display),
                      AT_SPI_BUS, 0L,
                      (long) BUFSIZ, False,
                      (Atom) 31, &actual_type, &actual_format,
                      &nitems, &leftover, &data);

  dbus_error_init (&error);

  if (data == NULL)
    {
      g_warning
        ("AT-SPI: Accessibility bus not found - Using session bus.\n");
      bus = dbus_bus_get (DBUS_BUS_SESSION, &error);
      if (!bus)
        {
          g_warning ("AT-SPI: Couldn't connect to bus: %s\n", error.message);
          return NULL;
        }
    }
  else
    {
      bus = dbus_connection_open (data, &error);
      if (!bus)
        {
          g_warning ("AT-SPI: Couldn't connect to bus: %s\n", error.message);
          return NULL;
        }
      else
        {
          if (!dbus_bus_register (bus, &error))
            {
              g_warning ("AT-SPI: Couldn't register with bus: %s\n", error.message);
              return NULL;
            }
        }
    }

  return bus;
}

static void
set_reply (DBusPendingCall *pending, void *user_data)
{
    void **replyptr = (void **)user_data;

    *replyptr = dbus_pending_call_steal_reply (pending);
}

static DBusMessage *
send_and_allow_reentry (DBusConnection *bus, DBusMessage *message, DBusError *error)
{
    DBusPendingCall *pending;
    DBusMessage *reply = NULL;

    if (!dbus_connection_send_with_reply (bus, message, &pending, -1))
    {
        return NULL;
    }
    dbus_pending_call_set_notify (pending, set_reply, (void *)&reply, NULL);
    while (!reply)
    {
      if (!dbus_connection_read_write_dispatch (bus, -1)) return NULL;
    }
    return reply;
}
/*---------------------------------------------------------------------------*/

static void
add_event (const char *bus_name, const char *event)
{
  event_data *evdata;
  gchar **data;
  GList *new_list;

  evdata = (event_data *) g_malloc (sizeof (*evdata));
  if (!evdata)
    return;
  data = g_strsplit (event, ":", 3);
  if (!data)
    {
      g_free (evdata);
      return;
    }
  evdata->bus_name = g_strdup (bus_name);
  evdata->data = data;
  new_list = g_list_append (spi_global_app_data->events, evdata);
  if (new_list)
    spi_global_app_data->events = new_list;
}

static void
get_registered_event_listeners (SpiBridge *app)
{
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_array, iter_struct;

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                         SPI_DBUS_PATH_REGISTRY,
                                         SPI_DBUS_INTERFACE_REGISTRY,
                                         "GetRegisteredEvents");
  if (!message)
    return;

  reply = dbus_connection_send_with_reply_and_block (app->bus, message, 5000, NULL);
  dbus_message_unref (message);
  if (!reply)
    return;
  if (strcmp (dbus_message_get_signature (reply), "a(ss)") != 0)
    {
      /* TODO: Add a warning when it's okay to add strings */
      dbus_message_unref (reply);
      return;
    }
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  /* TODO: This is bad. Need to determine that the array is non-empty,
     so that we don't initially read a value rom it in that case, but using
     a deprecated function. */
  if (dbus_message_iter_get_array_len (&iter_array) > 0) do
    {
      char *bus_name, *event;
      dbus_message_iter_recurse (&iter_array, &iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &bus_name);
      dbus_message_iter_next (&iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &event);
      add_event (bus_name, event);
    }
  while (dbus_message_iter_next (&iter_array));
  dbus_message_unref (reply);
}

static void
register_reply (DBusPendingCall *pending, void *user_data)
{
  DBusMessage *reply;
  SpiBridge *app = user_data;
  DBusMessage *message;

    reply = dbus_pending_call_steal_reply (pending);
  if (reply)
    {
      gchar *app_name, *obj_path;

      if (strcmp (dbus_message_get_signature (reply), "(so)") != 0)
        {
          g_warning ("AT-SPI: Could not obtain desktop path or name\n");
printf("sig: %s\n", dbus_message_get_signature(reply));
        }
      else
        {
          DBusMessageIter iter, iter_struct;
          dbus_message_iter_init (reply, &iter);
          dbus_message_iter_recurse (&iter, &iter_struct);
          dbus_message_iter_get_basic (&iter_struct, &app_name);
          dbus_message_iter_next (&iter_struct);
          dbus_message_iter_get_basic (&iter_struct, &obj_path);

          app->desktop_name = g_strdup (app_name);
          app->desktop_path = g_strdup (obj_path);
        }
    }
  else
    {
      g_warning ("AT-SPI: Could not embed inside desktop");
      return;
    }
  dbus_message_unref (reply);

  get_registered_event_listeners (spi_global_app_data);
}

static gboolean
register_application (SpiBridge * app)
{
  DBusMessage *message, *reply;
  DBusMessageIter iter;
  DBusError error;
  DBusPendingCall *pending;

  dbus_error_init (&error);

  /* These will be overridden when we get a reply, but in practice these
     defaults should always be correct */
  app->desktop_name = SPI_DBUS_NAME_REGISTRY;
  app->desktop_path = SPI_DBUS_PATH_ROOT;

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                          SPI_DBUS_PATH_ROOT,
                                          SPI_DBUS_INTERFACE_SOCKET,
                                          "Embed");

  dbus_message_iter_init_append (message, &iter);
  spi_object_append_reference (&iter, app->root);
  
    if (!dbus_connection_send_with_reply (app->bus, message, &pending, -1))
    {
        return FALSE;
    }

    dbus_pending_call_set_notify (pending, register_reply, app, NULL);

  if (message)
    dbus_message_unref (message);

  return TRUE;
}

/*---------------------------------------------------------------------------*/

static void
deregister_application (SpiBridge * app)
{
  DBusMessage *message;
  DBusMessageIter iter;
  DBusError error;
  const char *uname;

  dbus_error_init (&error);

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                          SPI_DBUS_PATH_REGISTRY,
                                          SPI_DBUS_INTERFACE_REGISTRY,
                                          "DeregisterApplication");
  dbus_message_set_no_reply (message, TRUE);

  uname = dbus_bus_get_unique_name (app->bus);

  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &uname);
  dbus_connection_send (app->bus, message, NULL);
  if (message)
    dbus_message_unref (message);
}

/*---------------------------------------------------------------------------*/

static void
exit_func (void)
{
  if (!spi_global_app_data)
    {
      return;
    }

  spi_atk_tidy_windows ();
  spi_atk_deregister_event_listeners ();
  deregister_application (spi_global_app_data);

  g_free (spi_global_app_data);
  spi_global_app_data = NULL;

  /* Not currently creating an XDisplay */
#if 0
  if (bridge_display)
    XCloseDisplay (bridge_display);
#endif
}

/*---------------------------------------------------------------------------*/

static AtkPlugClass *plug_class;
static AtkSocketClass *socket_class;

static gchar *
get_plug_id (AtkPlug * plug)
{
  const char *uname = dbus_bus_get_unique_name (spi_global_app_data->bus);
  gchar *path;
  GString *str = g_string_new (NULL);

  path = spi_register_object_to_path (spi_global_register, G_OBJECT (plug));
  g_string_printf (str, "%s:%s", uname, path);
  g_free (path);
  return g_string_free (str, FALSE);
}

AtkStateSet *
socket_ref_state_set (AtkObject *accessible)
{
  char *child_name, *child_path;
  AtkSocket *socket = ATK_SOCKET (accessible);
  int count = 0;
  int j;
  int v;
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_array;
  AtkStateSet *set;

  if (!socket->embedded_plug_id)
    return NULL;

  child_name = g_strdup (socket->embedded_plug_id);
  if (!child_name)
    return NULL;
  child_path = g_utf8_strchr (child_name + 1, -1, ':');
  if (!child_path)
    {
      g_free (child_name);
      return NULL;
    }
  *(child_path++) = '\0';
  message = dbus_message_new_method_call (child_name, child_path, SPI_DBUS_INTERFACE_ACCESSIBLE, "GetState");
  g_free (child_name);
  reply = dbus_connection_send_with_reply_and_block (spi_global_app_data->bus, message, 1, NULL);
  dbus_message_unref (message);
  if (reply == NULL)
    return NULL;
  if (strcmp (dbus_message_get_signature (reply), "au") != 0)
    {
      dbus_message_unref (reply);
      return NULL;
    }
  set = atk_state_set_new ();
  if (!set)
    return  NULL;
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  do
    {
      dbus_message_iter_get_basic (&iter_array, &v);
      for (j = 0; j < 32; j++)
        {
          if (v & (1 << j))
            {
              AtkState state = spi_atk_state_from_spi_state ((count << 5) + j);
              atk_state_set_add_state (set, state);
            }
        }
      count++;
    }
  while (dbus_message_iter_next (&iter_array));
  dbus_message_unref (reply);
  return set;
}

static void
socket_embed_hook (AtkSocket * socket, gchar * plug_id)
{
  AtkObject *accessible = ATK_OBJECT(socket);
  gchar *plug_name, *plug_path;
  AtkObjectClass *klass;

  /* Force registration */
  gchar *path = spi_register_object_to_path (spi_global_register, G_OBJECT (accessible));
  /* Let the plug know that it has been embedded */
  plug_name = g_strdup (plug_id);
  if (!plug_name)
    {
      g_free (path);
      return;
    }
  plug_path = g_utf8_strchr (plug_name + 1, -1, ':');
  if (plug_path)
    {
      DBusMessage *message;
      *(plug_path++) = '\0';
      message = dbus_message_new_method_call (plug_name, plug_path, SPI_DBUS_INTERFACE_SOCKET, "Embedded");
      dbus_message_append_args (message, DBUS_TYPE_STRING, &path, DBUS_TYPE_INVALID);
      dbus_connection_send (spi_global_app_data->bus, message, NULL);
    }
  g_free (plug_name);
  g_free (path);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  klass->ref_state_set = socket_ref_state_set;
}

static void
install_plug_hooks ()
{
  gpointer data;

  data = g_type_class_ref (ATK_TYPE_PLUG);
  plug_class = ATK_PLUG_CLASS (data);
  data = g_type_class_ref (ATK_TYPE_SOCKET);
  socket_class = ATK_SOCKET_CLASS (data);
  plug_class->get_object_id = get_plug_id;
  socket_class->embed = socket_embed_hook;
}

gchar *atspi_dbus_name = NULL;
static gboolean atspi_no_register = FALSE;

static GOptionEntry atspi_option_entries[] = {
  {"atspi-dbus-name", 0, 0, G_OPTION_ARG_STRING, &atspi_dbus_name,
   "D-Bus bus name to register as", NULL},
  {"atspi-no-register", 0, 0, G_OPTION_ARG_NONE, &atspi_no_register,
   "Do not register with Registry Daemon", NULL},
  {NULL}
};

static gchar *
introspect_children_cb (const char *path, void *data)
{
  if (!strcmp (path, "/org/a11y/atspi/accessible"))
    {
      return g_strdup ("<node name=\"root\"/>\n");
      /* TODO: Should we place the whole hierarchy here? */
    }
  return NULL;
}

static void
handle_event_listener_registered (DBusConnection *bus, DBusMessage *message,
                                  void *user_data)
{
  const char *name;
  char *sender;

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &sender,
    DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID))
    return;

  add_event (sender, name);
}

static void
remove_events (const char *bus_name, const char *event)
{
  event_data *evdata;
  gchar **remove_data;
  GList *list;

  remove_data = g_strsplit (event, ":", 3);
  if (!remove_data)
    {
      return;
    }

  for (list = spi_global_app_data->events; list;)
    {
      event_data *evdata = list->data;
      if (!g_strcmp0 (evdata->bus_name, bus_name) &&
          spi_event_is_subtype (evdata->data, remove_data))
        {
          GList *events = spi_global_app_data->events;
          list = list->next;
          g_strfreev (evdata->data);
          g_free (evdata->bus_name);
          g_free (evdata);
          spi_global_app_data->events = g_list_remove (events, evdata);
        }
      else
        {
          list = list->next;
        }
    }
}

static void
handle_event_listener_deregistered (DBusConnection *bus, DBusMessage *message,
                                    void *user_data)
{
  const char *orig_name;
  gchar *name;
  char *sender;

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &sender,
                              DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID))
    return;

  remove_events (sender, name);
}

static DBusHandlerResult
signal_filter (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  const char *interface = dbus_message_get_interface (message);
  const char *member = dbus_message_get_member (message);
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (dbus_message_get_type (message) != DBUS_MESSAGE_TYPE_SIGNAL)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (!strcmp (interface, SPI_DBUS_INTERFACE_REGISTRY))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if (!strcmp (member, "EventListenerRegistered"))
        handle_event_listener_registered (bus, message, user_data);
      else if (!strcmp (member, "EventListenerDeregistered"))
        handle_event_listener_deregistered (bus, message, user_data);
      else
        result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
  return result;
}

/*
 * spi_app_init
 *
 * The following needs to be initialized.
 *
 * - DRoute for routing message to their accessible objects.
 * - Event handlers for emmitting signals on specific ATK events.
 * - Application registration with the AT-SPI registry.
 *
 */
static int
adaptor_init (gint * argc, gchar ** argv[])
{
  GOptionContext *opt;
  GError *err = NULL;
  DBusError error;
  AtkObject *root;
  gchar *introspection_directory;
  static gboolean inited = FALSE;

  if (inited)
    return 0;

  inited = TRUE;

  DRoutePath *treepath, *accpath;

  root = atk_get_root ();
  g_warn_if_fail (root);
  if (!root)
    {
      inited = FALSE;
      return -1;
    }

  /* Parse command line options */
  opt = g_option_context_new (NULL);
  g_option_context_add_main_entries (opt, atspi_option_entries, NULL);
  g_option_context_set_ignore_unknown_options (opt, TRUE);
  if (!g_option_context_parse (opt, argc, argv, &err))
    g_warning ("AT-SPI Option parsing failed: %s\n", err->message);

  /* Allocate global data and do ATK initializations */
  spi_global_app_data = g_new0 (SpiBridge, 1);
  atk_misc = atk_misc_get_instance ();
  spi_global_app_data->root = g_object_ref (root);

  /* Set up D-Bus connection and register bus name */
  dbus_error_init (&error);
  spi_global_app_data->bus = spi_atk_bridge_get_bus ();
  if (!spi_global_app_data->bus)
    {
      g_free (spi_global_app_data);
      spi_global_app_data = NULL;
      inited = FALSE;
      return -1;
    }

  if (atspi_dbus_name != NULL)
    {
      if (dbus_bus_request_name
          (spi_global_app_data->bus, atspi_dbus_name, 0, &error))
        {
          g_print ("AT-SPI Recieved D-Bus name - %s\n", atspi_dbus_name);
        }
      else
        {
          g_print
            ("AT-SPI D-Bus name requested but could not be allocated - %s\n",
             atspi_dbus_name);
        }
    }

  dbus_connection_setup_with_g_main (spi_global_app_data->bus,
                                     g_main_context_default ());

  /* Hook our plug-and socket functions */
  install_plug_hooks ();

  /* 
   * Create the leasing, register and cache objects.
   * The order is important here, the cache depends on the
   * register object.
   */
  spi_global_register = g_object_new (SPI_REGISTER_TYPE, NULL);
  spi_global_leasing  = g_object_new (SPI_LEASING_TYPE, NULL);
  spi_global_cache    = g_object_new (SPI_CACHE_TYPE, NULL);

  /* Register droute for routing AT-SPI messages */
  spi_global_app_data->droute =
    droute_new (spi_global_app_data->bus);

  treepath = droute_add_one (spi_global_app_data->droute,
                             "/org/a11y/atspi/cache", spi_global_cache);

  if (!treepath)
    {
      g_warning ("atk-bridge: Error in droute_add_one().  Already running?");
      return -1;
    }

  accpath = droute_add_many (spi_global_app_data->droute,
                             "/org/a11y/atspi/accessible",
                             NULL,
                             introspect_children_cb,
                             NULL,
                             (DRouteGetDatumFunction)
                             spi_global_register_path_to_object);


  /* Register all interfaces with droute and set up application accessible db */
  spi_initialize_cache (treepath);
  spi_initialize_accessible (accpath);
  spi_initialize_application (accpath);
  spi_initialize_action (accpath);
  spi_initialize_collection (accpath);
  spi_initialize_component (accpath);
  spi_initialize_document (accpath);
  spi_initialize_editabletext (accpath);
  spi_initialize_hyperlink (accpath);
  spi_initialize_hypertext (accpath);
  spi_initialize_image (accpath);
  spi_initialize_selection (accpath);
  spi_initialize_socket (accpath);
  spi_initialize_table (accpath);
  spi_initialize_text (accpath);
  spi_initialize_value (accpath);

  /* Register methods to send D-Bus signals on certain ATK events */
  spi_atk_register_event_listeners ();

  /* Set up filter and match rules to catch signals */
  dbus_bus_add_match (spi_global_app_data->bus, "type='signal', interface='org.a11y.atspi.Registry', sender='org.a11y.atspi.Registry'", NULL);
  dbus_connection_add_filter (spi_global_app_data->bus, signal_filter, NULL,
                              NULL);

  /* Register this app by sending a signal out to AT-SPI registry daemon */
  if (!atspi_no_register && (!root || !ATK_IS_PLUG (root)))
    register_application (spi_global_app_data);
  else
    get_registered_event_listeners (spi_global_app_data);

  g_atexit (exit_func);

  return 0;
}

/*---------------------------------------------------------------------------*/

int
gtk_module_init (gint * argc, gchar ** argv[])
{
  const gchar *load_bridge = g_getenv ("NO_AT_BRIDGE");

  if (!load_bridge || g_ascii_strtod (load_bridge, NULL) == 0)
    {
      return adaptor_init (argc, argv);
    }
  return 0;
}

gchar*
g_module_check_init (GModule *module)
{
  g_module_make_resident (module);

  return NULL;
}

void
gnome_accessibility_module_init (void)
{
  const gchar *load_bridge = g_getenv ("NO_AT_BRIDGE");

  if (!load_bridge || g_ascii_strtod (load_bridge, NULL) == 0)
    {
      adaptor_init (NULL, NULL);

      if (g_getenv ("AT_SPI_DEBUG"))
        {
          g_print ("Atk Accessibility bridge initialized\n");
        }
    }
}

void
gnome_accessibility_module_shutdown (void)
{
  spi_atk_deregister_event_listeners ();
  exit_func ();
}

/*END------------------------------------------------------------------------*/
