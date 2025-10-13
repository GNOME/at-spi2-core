/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008, 2009 Codethink Ltd.
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
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

#include "config.h"

#include <atk/atk.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

#include <atk-bridge.h>
#include <atspi/atspi.h>
#include <droute/droute.h>

#include "accessible-stateset.h"
#include "adaptors.h"
#include "bridge.h"
#include "event.h"
#include "object.h"

#include "accessible-cache.h"
#include "accessible-leasing.h"
#include "accessible-register.h"

#include "spi-dbus.h"

/*---------------------------------------------------------------------------*/

static DBusHandlerResult
signal_filter (DBusConnection *bus, DBusMessage *message, void *user_data);

SpiBridge *spi_global_app_data = NULL;

static gboolean inited = FALSE;
static gboolean atexit_added = FALSE;

/*---------------------------------------------------------------------------*/

static event_data *
add_event (const char *bus_name, const char *event)
{
  event_data *evdata;
  gchar **data;

  spi_atk_add_client (bus_name);
  evdata = g_new0 (event_data, 1);
  data = g_strsplit (event, ":", 3);
  if (!data)
    {
      g_free (evdata);
      return NULL;
    }
  evdata->bus_name = g_strdup (bus_name);
  evdata->data = data;
  spi_global_app_data->events = g_list_append (spi_global_app_data->events, evdata);
  return evdata;
}

static GSList *clients = NULL;

static void
tally_event_reply ()
{
  if (!spi_global_app_data)
    return;

  spi_global_app_data->replies_received++;
  if (spi_global_app_data->replies_received == 3)
    {
      if (!clients)
        spi_atk_deregister_event_listeners ();
      spi_global_app_data->events_initialized = TRUE;
    }
}

GType
_atk_bridge_type_from_iface (const char *iface)
{
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_ACCESSIBLE))
    return ATK_TYPE_OBJECT;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_ACTION))
    return ATK_TYPE_ACTION;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_COMPONENT))
    return ATK_TYPE_COMPONENT;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_DOCUMENT))
    return ATK_TYPE_DOCUMENT;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_HYPERTEXT))
    return ATK_TYPE_HYPERTEXT;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_HYPERLINK))
    return ATK_TYPE_HYPERLINK;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_IMAGE))
    return ATK_TYPE_IMAGE;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_SELECTION))
    return ATK_TYPE_SELECTION;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_TABLE))
    return ATK_TYPE_TABLE;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_TEXT))
    return ATK_TYPE_TEXT;
  if (!strcmp (iface, ATSPI_DBUS_INTERFACE_VALUE))
    return ATK_TYPE_VALUE;
  return 0;
}

DRoutePropertyFunction
_atk_bridge_find_property_func (const char *property, GType *type)
{
  const char *iface;
  const char *member;
  DRouteProperty *dp;

  if (!strncasecmp (property, "action.", 7))
    {
      iface = ATSPI_DBUS_INTERFACE_ACTION;
      member = property + 7;
    }
  else if (!strncasecmp (property, "component.", 10))
    {
      iface = ATSPI_DBUS_INTERFACE_COMPONENT;
      member = property + 10;
    }
  else if (!strncasecmp (property, "selection.", 10))
    {
      iface = ATSPI_DBUS_INTERFACE_SELECTION;
      member = property + 10;
    }
  else if (!strncasecmp (property, "table.", 6))
    {
      iface = ATSPI_DBUS_INTERFACE_TABLE;
      member = property + 6;
    }
  else if (!strncasecmp (property, "text.", 5))
    {
      iface = ATSPI_DBUS_INTERFACE_TEXT;
      member = property + 5;
    }
  else if (!strncasecmp (property, "value.", 6))
    {
      iface = ATSPI_DBUS_INTERFACE_VALUE;
      member = property + 6;
    }
  else
    {
      iface = ATSPI_DBUS_INTERFACE_ACCESSIBLE;
      member = property;
    }

  *type = _atk_bridge_type_from_iface (iface);

  dp = g_hash_table_lookup (spi_global_app_data->property_hash, iface);

  if (!dp)
    return NULL;

  for (; dp->name; dp++)
    {
      if (!strcasecmp (dp->name, member))
        {
          return dp->get;
        }
    }
  return NULL;
}

static void
add_property_to_event (event_data *evdata, const char *property)
{
  AtspiPropertyDefinition *prop = g_new0 (AtspiPropertyDefinition, 1);
  prop->func = _atk_bridge_find_property_func (property, &prop->type);
  if (!prop->func)
    {
      g_warning ("atk-bridge: Request for unknown property '%s'", property);
      g_free (prop);
      return;
    }

  prop->name = g_strdup (property);
  evdata->properties = g_slist_append (evdata->properties, prop);
}

static void
add_event_from_iter (DBusMessageIter *iter)
{
  const char *bus_name, *event;
  event_data *evdata;

  dbus_message_iter_get_basic (iter, &bus_name);
  dbus_message_iter_next (iter);
  dbus_message_iter_get_basic (iter, &event);
  dbus_message_iter_next (iter);
  evdata = add_event (bus_name, event);
  if (dbus_message_iter_get_arg_type (iter) == DBUS_TYPE_ARRAY)
    {
      DBusMessageIter iter_sub_array;
      dbus_message_iter_recurse (iter, &iter_sub_array);
      while (dbus_message_iter_get_arg_type (&iter_sub_array) != DBUS_TYPE_INVALID)
        {
          const char *property;
          dbus_message_iter_get_basic (&iter_sub_array, &property);
          add_property_to_event (evdata, property);
          dbus_message_iter_next (&iter_sub_array);
        }
    }
}

static void
get_events_reply (DBusPendingCall *pending, void *user_data)
{
  DBusMessage *reply = dbus_pending_call_steal_reply (pending);
  DBusMessageIter iter, iter_array, iter_struct;

  if (!reply || !spi_global_app_data)
    goto done;

  if (strcmp (dbus_message_get_signature (reply), "a(ss)") != 0 &&
      strcmp (dbus_message_get_signature (reply), "a(ssas)") != 0)
    {
      g_warning ("atk-bridge: GetRegisteredEvents returned message with unknown signature");
      goto done;
    }

  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      dbus_message_iter_recurse (&iter_array, &iter_struct);
      add_event_from_iter (&iter_struct);
      dbus_message_iter_next (&iter_array);
    }

done:
  if (reply)
    dbus_message_unref (reply);
  if (pending)
    dbus_pending_call_unref (pending);

  tally_event_reply ();
}

static void
get_device_events_reply (DBusPendingCall *pending, void *user_data)
{
  DBusMessage *reply = dbus_pending_call_steal_reply (pending);
  DBusMessageIter iter, iter_array, iter_struct;

  if (!reply)
    goto done;

  if (strncmp (dbus_message_get_signature (reply), "a(s", 3) != 0)
    {
      g_warning ("atk-bridge: get_device_events_reply: unknown signature");
      goto done;
    }

  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      char *bus_name;
      dbus_message_iter_recurse (&iter_array, &iter_struct);
      dbus_message_iter_get_basic (&iter_struct, &bus_name);
      spi_atk_add_client (bus_name);
      dbus_message_iter_next (&iter_array);
    }

done:
  if (reply)
    dbus_message_unref (reply);
  if (pending)
    dbus_pending_call_unref (pending);

  tally_event_reply ();
}

static void
get_registered_event_listeners (SpiBridge *app)
{
  DBusMessage *message;
  DBusPendingCall *pending = NULL;

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                          ATSPI_DBUS_PATH_REGISTRY,
                                          ATSPI_DBUS_INTERFACE_REGISTRY,
                                          "GetRegisteredEvents");
  if (!message)
    return;

  dbus_connection_send_with_reply (app->bus, message, &pending, -1);
  dbus_message_unref (message);
  if (!pending)
    {
      spi_global_app_data->events_initialized = TRUE;
      return;
    }
  dbus_pending_call_set_notify (pending, get_events_reply, NULL, NULL);

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                          ATSPI_DBUS_PATH_DEC,
                                          ATSPI_DBUS_INTERFACE_DEC,
                                          "GetKeystrokeListeners");
  if (!message)
    return;
  pending = NULL;
  dbus_connection_send_with_reply (app->bus, message, &pending, -1);
  dbus_message_unref (message);
  if (!pending)
    {
      spi_global_app_data->events_initialized = TRUE;
      return;
    }
  dbus_pending_call_set_notify (pending, get_device_events_reply, NULL, NULL);
}

static void
register_reply (DBusPendingCall *pending, void *user_data)
{
  DBusMessage *reply;
  SpiBridge *app = user_data;

  reply = dbus_pending_call_steal_reply (pending);
  dbus_pending_call_unref (pending);

  if (!spi_global_app_data)
    {
      if (reply)
        dbus_message_unref (reply);
      return;
    }

  if (reply)
    {
      gchar *app_name, *obj_path;

      if (strcmp (dbus_message_get_signature (reply), "(so)") != 0)
        {
          g_warning ("AT-SPI: Could not obtain desktop path or name\n");
        }
      else
        {
          DBusMessageIter iter, iter_struct;
          dbus_message_iter_init (reply, &iter);
          dbus_message_iter_recurse (&iter, &iter_struct);
          dbus_message_iter_get_basic (&iter_struct, &app_name);
          dbus_message_iter_next (&iter_struct);
          dbus_message_iter_get_basic (&iter_struct, &obj_path);

          g_free (app->desktop_name);
          app->desktop_name = g_strdup (app_name);
          g_free (app->desktop_path);
          app->desktop_path = g_strdup (obj_path);
        }
    }
  else
    {
      g_warning ("AT-SPI: Could not embed inside desktop");
      return;
    }
  dbus_message_unref (reply);

  if (!spi_global_app_data->events_initialized)
    get_registered_event_listeners (spi_global_app_data);
}

static gboolean
register_application (gpointer data)
{
  SpiBridge *app = data;
  DBusMessage *message;
  DBusMessageIter iter;
  DBusPendingCall *pending;

  spi_global_app_data->registration_pending = 0;

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                          ATSPI_DBUS_PATH_ROOT,
                                          ATSPI_DBUS_INTERFACE_SOCKET,
                                          "Embed");

  dbus_message_iter_init_append (message, &iter);
  spi_object_append_reference (&iter, app->root);

  if (!dbus_connection_send_with_reply (app->bus, message, &pending, -1) || !pending)
    {
      if (pending)
        dbus_pending_call_unref (pending);

      dbus_message_unref (message);
      return FALSE;
    }

  dbus_pending_call_set_notify (pending, register_reply, app, NULL);

  if (message)
    dbus_message_unref (message);

  return FALSE;
}

void
_atk_bridge_schedule_application_registration (SpiBridge *app)
{
  /* We need the callback to be called first thing, before any other of ours
   * (and possibly of client apps), so use a high priority and a short timeout
   * to try and be called first by the main loop. */
  if (!app->registration_pending)
    app->registration_pending = spi_timeout_add_full (G_PRIORITY_HIGH, 0,
                                                      register_application,
                                                      app, NULL);
}

gboolean
_atk_bridge_remove_pending_application_registration (SpiBridge *app)
{
  if (app->registration_pending)
    {
      g_source_remove (app->registration_pending);
      app->registration_pending = 0;
      return TRUE;
    }

  return FALSE;
}

/*---------------------------------------------------------------------------*/

static void
remove_socket ()
{
  if (!spi_global_app_data)
    return;

  if (spi_global_app_data->app_bus_addr &&
      !strncmp (spi_global_app_data->app_bus_addr, "unix:path=", 10))
    {
      unlink (spi_global_app_data->app_bus_addr + 10);
      g_free (spi_global_app_data->app_bus_addr);
      spi_global_app_data->app_bus_addr = NULL;
    }

  if (spi_global_app_data->app_tmp_dir)
    {
      rmdir (spi_global_app_data->app_tmp_dir);
      g_free (spi_global_app_data->app_tmp_dir);
      spi_global_app_data->app_tmp_dir = NULL;
    }
}

static void
deregister_application (SpiBridge *app)
{
  DBusMessage *message;
  DBusMessageIter iter;
  const char *uname;

  if (_atk_bridge_remove_pending_application_registration (spi_global_app_data))
    return;

  message = dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                          ATSPI_DBUS_PATH_REGISTRY,
                                          ATSPI_DBUS_INTERFACE_REGISTRY,
                                          "DeregisterApplication");
  dbus_message_set_no_reply (message, TRUE);

  uname = dbus_bus_get_unique_name (app->bus);

  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &uname);
  dbus_connection_send (app->bus, message, NULL);
  if (message)
    dbus_message_unref (message);

  remove_socket ();

  g_free (app->desktop_name);
  app->desktop_name = NULL;
  g_free (app->desktop_path);
  app->desktop_path = NULL;
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

static AtkPlugClass *plug_class;
static AtkSocketClass *socket_class;

static gchar *
get_plug_id (AtkPlug *plug)
{
  const char *uname;
  gchar *path;
  GString *str = g_string_new (NULL);

  if (!spi_global_app_data || !spi_global_app_data->bus)
    return NULL;

  uname = dbus_bus_get_unique_name (spi_global_app_data->bus);
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

  set = atk_state_set_new ();

  if (!socket->embedded_plug_id)
    return set;

  child_name = g_strdup (socket->embedded_plug_id);
  if (!child_name)
    return set;
  child_path = g_utf8_strchr (child_name + 1, -1, ':');
  if (!child_path)
    {
      g_free (child_name);
      return set;
    }
  *(child_path++) = '\0';
  message = dbus_message_new_method_call (child_name, child_path, ATSPI_DBUS_INTERFACE_ACCESSIBLE, "GetState");
  g_free (child_name);
  reply = dbus_connection_send_with_reply_and_block (spi_global_app_data->bus, message, 1, NULL);
  dbus_message_unref (message);
  if (reply == NULL)
    return set;
  if (strcmp (dbus_message_get_signature (reply), "au") != 0)
    {
      dbus_message_unref (reply);
      return set;
    }

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
socket_embed_hook (AtkSocket *socket, const gchar *plug_id)
{
  g_return_if_fail (spi_global_register != NULL);

  AtkObject *accessible = ATK_OBJECT (socket);
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
      message = dbus_message_new_method_call (plug_name, plug_path, ATSPI_DBUS_INTERFACE_SOCKET, "Embedded");
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

static guint
get_ancestral_uid (guint pid)
{
  FILE *fp;
  char buf[80];
  int ppid = 0;
  int uid = 0;
  gboolean got_ppid = 0;
  gboolean got_uid = 0;

  sprintf (buf, "/proc/%d/status", pid);
  fp = fopen (buf, "r");
  if (!fp)
    return 0;
  while ((!got_ppid || !got_uid) && fgets (buf, sizeof (buf), fp))
    {
      if (sscanf (buf, "PPid:\t%d", &ppid) == 1)
        got_ppid = TRUE;
      else if (sscanf (buf, "Uid:\t%d", &uid) == 1)
        got_uid = TRUE;
    }
  fclose (fp);

  if (!got_ppid || !got_uid)
    return 0;
  if (uid != 0)
    return uid;
  if (ppid == 0 || ppid == 1)
    return 0;
  return get_ancestral_uid (ppid);
}

static dbus_bool_t
user_check (DBusConnection *bus, unsigned long uid, void *data)
{
  if (uid == getuid () || uid == geteuid ())
    return TRUE;
  if (getuid () == 0)
    {
      guint ancestor = get_ancestral_uid (getpid ());
      return (ancestor == uid || ancestor == 1 || ancestor == 0);
    }
  return FALSE;
}

static void
new_connection_cb (DBusServer *server, DBusConnection *con, void *data)
{
  dbus_connection_set_unix_user_function (con, user_check, NULL, NULL);
  dbus_connection_ref (con);
  atspi_dbus_connection_setup_with_g_main (con, spi_context);
  droute_intercept_dbus (con);
  droute_context_register (spi_global_app_data->droute, con);

  spi_global_app_data->direct_connections = g_list_append (spi_global_app_data->direct_connections, con);
}

static gchar *atspi_dbus_name = NULL;
static gboolean atspi_no_register = FALSE;

static GOptionEntry atspi_option_entries[] = {
  { "atspi-dbus-name", 0, 0, G_OPTION_ARG_STRING, &atspi_dbus_name,
    "D-Bus bus name to register as", NULL },
  { "atspi-no-register", 0, 0, G_OPTION_ARG_NONE, &atspi_no_register,
    "Do not register with Registry Daemon", NULL },
  { NULL }
};

static void
add_objects_for_introspection (AtkObject *obj, GString *str)
{
  gchar *path;
  AtkStateSet *set;
  char *p;
  gint i;
  gint count;

  if (!obj)
    return;

  path = spi_register_object_to_path (spi_global_register, G_OBJECT (obj));
  p = strrchr (path, '/') + 1;
  g_string_append_printf (str, "<node name=\"%s\"/>\n", p);
  g_free (path);

  if (ATK_IS_SOCKET (obj))
    return;

  set = atk_object_ref_state_set (obj);
  if (atk_state_set_contains_state (set, ATK_STATE_MANAGES_DESCENDANTS))
    {
      g_object_unref (set);
      return;
    }
  g_object_unref (set);

  count = atk_object_get_n_accessible_children (obj);
  for (i = 0; i < count; i++)
    {
      AtkObject *child = atk_object_ref_accessible_child (obj, i);
      add_objects_for_introspection (child, str);
      g_object_unref (child);
    }
}

static gchar *
introspect_children_cb (const char *path, void *data)
{
  if (!strcmp (path, "/org/a11y/atspi/accessible"))
    {
      GString *str = g_string_new (NULL);
      add_objects_for_introspection (spi_global_app_data->root, str);
      return g_string_free (str, FALSE);
    }

  return NULL;
}

static void
handle_event_listener_registered (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessageIter iter;
  const char *signature = dbus_message_get_signature (message);

  if (strcmp (signature, "ssas") != 0 &&
      strcmp (signature, "ss") != 0)
    {
      g_warning ("got RegisterEvent with invalid signature '%s'", signature);
      return;
    }

  dbus_message_iter_init (message, &iter);
  add_event_from_iter (&iter);
}

static void
free_property_definition (void *data)
{
  AtspiPropertyDefinition *pd = data;

  g_free (pd->name);
  g_free (pd);
}

static void
remove_events (const char *bus_name, const char *event)
{
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
          GList *next;
          GList *events = spi_global_app_data->events;

          g_strfreev (evdata->data);
          g_free (evdata->bus_name);
          g_slist_free_full (evdata->properties, free_property_definition);
          g_free (evdata);

          next = list->next;
          spi_global_app_data->events = g_list_delete_link (events, list);
          list = next;
        }
      else
        {
          list = list->next;
        }
    }

  g_strfreev (remove_data);
}

static void
handle_event_listener_deregistered (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  gchar *name;
  char *sender;

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &sender,
                              DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID))
    return;

  remove_events (sender, name);
}

static void
handle_device_listener_registered (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  char *sender;
  DBusMessageIter iter, iter_struct;

  if (strncmp (dbus_message_get_signature (message), "(s", 2) != 0)
    {
      g_warning ("atk-bridge: handle_device_listener_register: unknown signature");
      return;
    }

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &sender);
  spi_atk_add_client (sender);
}

static DBusHandlerResult
signal_filter (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  const char *interface = dbus_message_get_interface (message);
  const char *member = dbus_message_get_member (message);
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  static gboolean registry_lost = FALSE;

  if (dbus_message_get_type (message) != DBUS_MESSAGE_TYPE_SIGNAL)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_REGISTRY))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if (!strcmp (member, "EventListenerRegistered"))
        handle_event_listener_registered (bus, message, user_data);
      else if (!strcmp (member, "EventListenerDeregistered"))
        handle_event_listener_deregistered (bus, message, user_data);
      else
        result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
  else if (!strcmp (interface, ATSPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if (!strcmp (member, "KeystrokeListenerRegistered"))
        handle_device_listener_registered (bus, message, user_data);
      else
        result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (!g_strcmp0 (interface, DBUS_INTERFACE_DBUS) &&
      !g_strcmp0 (member, "NameOwnerChanged"))
    {
      char *name, *old, *new;
      if (dbus_message_get_args (message, NULL,
                                 DBUS_TYPE_STRING, &name,
                                 DBUS_TYPE_STRING, &old,
                                 DBUS_TYPE_STRING, &new,
                                 DBUS_TYPE_INVALID))
        {
          if (!strcmp (name, "org.a11y.atspi.Registry"))
            {
              if (registry_lost && !old[0])
                {
                  register_application (spi_global_app_data);
                  registry_lost = FALSE;
                }
              else if (!new[0])
                registry_lost = TRUE;
            }
          else if (*old != '\0' && *new == '\0')
            spi_atk_remove_client (old);
        }
    }

  return result;
}

int
spi_atk_create_socket (SpiBridge *app)
{
  DBusServer *server;
  DBusError error;
  const gchar *user_runtime_dir = g_get_user_runtime_dir ();
  char *socket_path;
  char *escaped_socket_path;
  const char *disable_p2p;

  disable_p2p = g_getenv ("ATSPI_DISABLE_P2P");
  if (disable_p2p && atoi (disable_p2p) > 0)
    return 0;

  if (g_mkdir_with_parents (user_runtime_dir, 0700) != 0)
    return -1;

  if (getuid () != 0)
    {
      app->app_tmp_dir = g_build_filename (user_runtime_dir,
                                           "at-spi2-XXXXXX", NULL);
      if (!g_mkdtemp (app->app_tmp_dir))
        {
          g_free (app->app_tmp_dir);
          app->app_tmp_dir = NULL;
          return -1;
        }
    }

  if (app->app_tmp_dir)
    {
      socket_path = g_strdup_printf ("%s/socket", app->app_tmp_dir);
    }
  else
    {
      socket_path = g_strdup_printf ("%s/at-spi2-socket-%d",
                                     user_runtime_dir, getpid ());
    }

  escaped_socket_path = dbus_address_escape_value (socket_path);
  g_free (socket_path);

  app->app_bus_addr = g_strconcat ("unix:path=", escaped_socket_path, NULL);
  dbus_free (escaped_socket_path);

  dbus_error_init (&error);
  server = dbus_server_listen (app->app_bus_addr, &error);
  if (server == NULL)
    {
      g_warning ("atk-bridge: Couldn't listen on dbus server: %s", error.message);
      dbus_error_free (&error);
      app->app_bus_addr[0] = '\0';
      return -1;
    }

  atspi_dbus_server_setup_with_g_main (server, spi_context);
  dbus_server_set_new_connection_function (server, new_connection_cb, NULL, NULL);

  app->server = server;

  return 0;
}

/*
 * Checks the status of the environment variables
 *
 * At this moment it only checks NO_AT_BRIDGE
 *
 * Returns TRUE if there isn't anything on the environment preventing
 * you to load the bridge, FALSE otherwise
 */
static gboolean
check_envvar (void)
{
  const gchar *envvar;

  envvar = g_getenv ("NO_AT_BRIDGE");

  if (envvar && atoi (envvar) == 1)
    return FALSE;
  else
    return TRUE;
}

void
spi_atk_activate ()
{
  DRoutePath *treepath;

  spi_atk_register_event_listeners ();
  if (!spi_global_cache)
    {
      spi_global_cache = g_object_new (SPI_CACHE_TYPE, NULL);
      treepath = droute_add_one (spi_global_app_data->droute,
                                 "/org/a11y/atspi/cache", spi_global_cache);

      if (!treepath)
        {
          g_warning ("atk-bridge: Error in droute_add_one().  Already running?");
          return;
        }
      spi_initialize_cache (treepath);
      if (spi_global_app_data->bus)
        droute_path_register (treepath, spi_global_app_data->bus);
    }
}

static gboolean
spi_object_has_dbus_interface (void *obj, const char *interface)
{
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_ACCESSIBLE))
    return TRUE;
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_ACTION))
    return ATK_IS_ACTION (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_APPLICATION))
      return ATK_IS_OBJECT(obj) && (atk_object_get_role (obj) == ATK_ROLE_APPLICATION);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_COLLECTION))
    return TRUE;
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_COMPONENT))
    return ATK_IS_COMPONENT (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_DOCUMENT))
    return ATK_IS_DOCUMENT (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_EDITABLE_TEXT))
    return ATK_IS_EDITABLE_TEXT (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_HYPERLINK))
    return ATK_IS_HYPERLINK (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_HYPERTEXT))
    return ATK_IS_HYPERTEXT (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_IMAGE))
    return ATK_IS_IMAGE (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_SELECTION))
    return ATK_IS_SELECTION (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_SOCKET))
    return TRUE;
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_TABLE))
    return ATK_IS_TABLE (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_TABLE_CELL))
    return ATK_IS_TABLE_CELL (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_TEXT))
    return ATK_IS_TEXT (obj);
  if (!strcmp (interface, ATSPI_DBUS_INTERFACE_VALUE))
    return ATK_IS_VALUE (obj);

  return FALSE;
}

static gboolean
init_bus ()
{
  DBusError error;

  /* Set up D-Bus connection and register bus name */
  dbus_error_init (&error);
  spi_global_app_data->bus = atspi_get_a11y_bus ();
  if (!spi_global_app_data->bus)
    return FALSE;

  if (atspi_dbus_name != NULL)
    {
      if (dbus_bus_request_name (spi_global_app_data->bus, atspi_dbus_name, 0, &error))
        {
          g_print ("AT-SPI Received D-Bus name - %s\n", atspi_dbus_name);
        }
      else
        {
          g_print ("AT-SPI D-Bus name requested but could not be allocated - %s\n",
                   atspi_dbus_name);
        }
    }

  atspi_dbus_connection_setup_with_g_main (spi_global_app_data->bus, NULL);

  droute_context_register (spi_global_app_data->droute,
                           spi_global_app_data->bus);

  /* Set up filter and match rules to catch signals */
  dbus_bus_add_match (spi_global_app_data->bus, "type='signal', interface='org.a11y.atspi.Registry', sender='org.a11y.atspi.Registry'", NULL);
  dbus_bus_add_match (spi_global_app_data->bus, "type='signal', interface='org.a11y.atspi.DeviceEventListener', sender='org.a11y.atspi.Registry'", NULL);
  dbus_bus_add_match (spi_global_app_data->bus, "type='signal', arg0='org.a11y.atspi.Registry', interface='org.freedesktop.DBus', member='NameOwnerChanged'", NULL);
  dbus_connection_add_filter (spi_global_app_data->bus, signal_filter, NULL,
                              NULL);

  /* Register this app by sending a signal out to AT-SPI registry daemon */
  if (!atspi_no_register && !ATK_IS_PLUG (spi_global_app_data->root))
    _atk_bridge_schedule_application_registration (spi_global_app_data);
  else
    get_registered_event_listeners (spi_global_app_data);

  dbus_error_free (&error);
  return TRUE;
}

static void
deactivate_bus ()
{
  if (!spi_global_app_data->bus)
    return;

  dbus_connection_remove_filter (spi_global_app_data->bus, signal_filter, NULL);
  droute_context_unregister (spi_global_app_data->droute, spi_global_app_data->bus);

  if (atspi_dbus_name != NULL)
    {
      DBusError error;
      int result;

      dbus_error_init (&error);
      result = dbus_bus_release_name (spi_global_app_data->bus, atspi_dbus_name, &error);
      if (result == -1)
        {
          g_warning ("atk-bridge: could not release dbus name: %s", error.message);
        }
      else
        {
          g_print ("bridge: released name, result %d\n", result);
        }

      dbus_error_free (&error);
    }

  dbus_connection_close (spi_global_app_data->bus);
  dbus_connection_unref (spi_global_app_data->bus);
  spi_global_app_data->bus = NULL;
}

static void
on_session_name_owner_changed (GDBusConnection *connection,
                               const gchar *sender_name,
                               const gchar *object_path,
                               const gchar *interface_name,
                               const gchar *signal_name,
                               GVariant *parameters,
                               gpointer user_data)
{
  const gchar *old_owner;
  const gchar *new_owner;

  g_variant_get (parameters,
                 "(&s&s&s)",
                 NULL,
                 &old_owner,
                 &new_owner);

  if (strlen (new_owner) == 0)
    {
      spi_atk_deregister_event_listeners ();
      deactivate_bus ();
    }
  else
    {
      init_bus ();
      spi_atk_register_event_listeners ();
    }
}

/**
 * atk_bridge_adaptor_init: initializes the atk bridge adaptor
 *
 * The following needs to be initialized.
 *
 * - DRoute for routing message to their accessible objects.
 * - Event handlers for emmitting signals on specific ATK events.
 * - setup the bus for p2p communication
 * - Application registration with the AT-SPI registry.
 *
 * Returns: 0 if the bridge gets or was already initialized
 * succesfully, -1 otherwise
 */
int
atk_bridge_adaptor_init (gint *argc, gchar **argv[])
{
  GOptionContext *opt;
  GError *err = NULL;
  AtkObject *root;
  gboolean load_bridge;
  DRoutePath *accpath;

  load_bridge = check_envvar ();
  if (inited && !load_bridge)
    g_warning ("ATK Bridge is disabled but a11y has already been enabled.");

  if (inited)
    return 0;
  if (!load_bridge)
    return -1;

  inited = TRUE;

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
    {
      g_warning ("AT-SPI Option parsing failed: %s\n", err->message);
      g_error_free (err);
    }
  g_option_context_free (opt);

  /* Allocate global data and do ATK initializations */
  spi_global_app_data = g_new0 (SpiBridge, 1);
  spi_global_app_data->root = g_object_ref (root);
  spi_global_app_data->desktop_name = g_strdup (ATSPI_DBUS_NAME_REGISTRY);
  spi_global_app_data->desktop_path = g_strdup (ATSPI_DBUS_PATH_ROOT);

  /* Hook our plug-and socket functions */
  install_plug_hooks ();

  /*
   * Create the leasing, register and cache objects.
   * The order is important here, the cache depends on the
   * register object.
   */
  spi_global_register = g_object_new (SPI_REGISTER_TYPE, NULL);
  spi_global_leasing = g_object_new (SPI_LEASING_TYPE, NULL);

  /* Register droute for routing AT-SPI messages */
  spi_global_app_data->droute =
      droute_new ();

  accpath = droute_add_many (spi_global_app_data->droute,
                             "/org/a11y/atspi/accessible",
                             NULL,
                             introspect_children_cb,
                             NULL,
                             (DRouteGetDatumFunction)
                                 spi_global_register_path_to_object,
                             spi_object_has_dbus_interface);

  /* Register all interfaces with droute and set up application accessible db */
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
  spi_initialize_table_cell (accpath);
  spi_initialize_text (accpath);
  spi_initialize_value (accpath);

  if (!init_bus ())
    {
      g_object_unref (spi_global_app_data->root);
      g_free (spi_global_app_data->desktop_name);
      g_free (spi_global_app_data->desktop_path);
      droute_free (spi_global_app_data->droute);
      if (spi_global_app_data->property_hash)
        g_hash_table_destroy (spi_global_app_data->property_hash);
      g_free (spi_global_app_data);
      spi_global_app_data = NULL;
      inited = FALSE;
      return -1;
    }

  spi_global_app_data->main_context = g_main_context_new ();

  /* Register methods to send D-Bus signals on certain ATK events */
  if (clients)
    spi_atk_activate ();

  if (!atexit_added)
    atexit (remove_socket);
  atexit_added = TRUE;

  spi_global_app_data->session_bus = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, NULL);
  if (spi_global_app_data->session_bus)
    {
      spi_global_app_data->name_owner_changed_subscription_id =
          g_dbus_connection_signal_subscribe (spi_global_app_data->session_bus,
                                              "org.freedesktop.DBus",  /* name */
                                              "org.freedesktop.DBus",  /* interface */
                                              "NameOwnerChanged",      /* signal name */
                                              "/org/freedesktop/DBus", /* path */
                                              "org.a11y.Bus",          /* arg0 */
                                              G_DBUS_SIGNAL_FLAGS_NONE,
                                              on_session_name_owner_changed,
                                              NULL,
                                              (GDestroyNotify) NULL);
    }

  return 0;
}

void
atk_bridge_adaptor_cleanup (void)
{
  GList *l;
  GSList *ls;

  if (!inited)
    return;

  if (!spi_global_app_data)
    return;

  spi_atk_tidy_windows ();
  spi_atk_deregister_event_listeners ();

  deregister_application (spi_global_app_data);

  deactivate_bus ();

  for (l = spi_global_app_data->direct_connections; l; l = l->next)
    {
      DBusConnection *connection;

      connection = l->data;

      droute_context_unregister (spi_global_app_data->droute, connection);
      droute_unintercept_dbus (connection);
      dbus_connection_close (connection);
      dbus_connection_unref (connection);
    }
  g_list_free (spi_global_app_data->direct_connections);
  spi_global_app_data->direct_connections = NULL;

  for (ls = clients; ls; ls = ls->next)
    g_free (ls->data);
  g_slist_free (clients);
  clients = NULL;

  g_clear_object (&spi_global_cache);
  g_clear_object (&spi_global_leasing);
  g_clear_object (&spi_global_register);

  if (spi_global_app_data->main_context)
    g_main_context_unref (spi_global_app_data->main_context);

  droute_free (spi_global_app_data->droute);

  g_clear_object (&spi_global_app_data->session_bus);

  if (spi_global_app_data->property_hash)
    g_hash_table_destroy (spi_global_app_data->property_hash);

  g_free (spi_global_app_data);
  spi_global_app_data = NULL;

  inited = FALSE;
}

/*---------------------------------------------------------------------------*/

static gchar *name_match_tmpl =
    "type='signal', interface='org.freedesktop.DBus', member='NameOwnerChanged', arg0='%s'";

void
spi_atk_add_client (const char *bus_name)
{
  GSList *l;
  gchar *match;

  for (l = clients; l; l = l->next)
    {
      if (!g_strcmp0 (l->data, bus_name))
        return;
    }
  if (!clients)
    spi_atk_activate ();
  clients = g_slist_append (clients, g_strdup (bus_name));
  match = g_strdup_printf (name_match_tmpl, bus_name);
  dbus_bus_add_match (spi_global_app_data->bus, match, NULL);
  g_free (match);
}

void
spi_atk_remove_client (const char *bus_name)
{
  GSList *l;
  GSList *next_node;

  l = clients;
  while (l)
    {
      next_node = l->next;

      if (!g_strcmp0 (l->data, bus_name))
        {
          gchar *match = g_strdup_printf (name_match_tmpl, l->data);
          dbus_bus_remove_match (spi_global_app_data->bus, match, NULL);
          g_free (match);
          g_free (l->data);
          clients = g_slist_delete_link (clients, l);
          if (!clients)
            spi_atk_deregister_event_listeners ();
          return;
        }

      l = next_node;
    }
}

void
spi_atk_add_interface (DRoutePath *path,
                       const char *name,
                       const char *introspect,
                       const DRouteMethod *methods,
                       const DRouteProperty *properties)
{
  droute_path_add_interface (path, name, introspect, methods, properties);

  if (properties)
    {
      if (!spi_global_app_data->property_hash)
        spi_global_app_data->property_hash = g_hash_table_new_full (g_str_hash,
                                                                    g_str_equal,
                                                                    g_free, NULL);
      g_hash_table_insert (spi_global_app_data->property_hash, g_strdup (name),
                           (gpointer) properties);
    }
}
/*END------------------------------------------------------------------------*/
