/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2019 SUSE LLC.
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

/* atspi-mutter.c: support for keyboard/mouse handling using the
 * mutter/gnome-shell remote desktop interfaces
 *
 * This functionality is analogous to the X11-based code in
 * device-event-controller-x11.c. Placing the code here, rather than in the
 * registry daemon, allows the relevant dbus calls to come directly from the
 * AT-SPI client, rather than at-spi2-registryd being an intermediary,
 * which may be useful if a distribution wishes to lock down access to the
 * remote desktop interfaces.
 */

#include "atspi-private.h"

typedef struct
{
  DBusConnection *bus;
  const char *rd_session_id;
  const char *rd_session_path;
  const char *sc_session_id;
  const char *sc_session_path;
  const char *sc_stream_path;
  dbus_uint64_t window_id;
  gboolean window_id_is_explicit;
} ATSPI_MUTTER_DATA;

static ATSPI_MUTTER_DATA data;

#define MUTTER_REMOTE_DESKTOP_BUS_NAME "org.gnome.Mutter.RemoteDesktop"
#define MUTTER_REMOTE_DESKTOP_OBJECT_PATH "/org/gnome/Mutter/RemoteDesktop"
#define MUTTER_REMOTE_DESKTOP_INTERFACE "org.gnome.Mutter.RemoteDesktop"
#define MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE "org.gnome.Mutter.RemoteDesktop.Session"
#define MUTTER_SCREEN_CAST_BUS_NAME "org.gnome.Mutter.ScreenCast"
#define MUTTER_SCREEN_CAST_OBJECT_PATH "/org/gnome/Mutter/ScreenCast"
#define MUTTER_SCREEN_CAST_INTERFACE "org.gnome.Mutter.ScreenCast"
#define MUTTER_SCREEN_CAST_SESSION_INTERFACE "org.gnome.Mutter.ScreenCast.Session"

/* TODO: consider porting this to gdbus */

static void
ensure_bus ()
{
  if (data.bus)
    return;
  data.bus = dbus_bus_get (DBUS_BUS_SESSION, NULL);
}

static gboolean
ensure_rd_session_path (GError **error)
{
  char *session_path;
  DBusError d_error;

  if (data.rd_session_path)
    return (data.rd_session_path[0] != '\0');
  ensure_bus ();

  dbus_error_init (&d_error);
  dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, MUTTER_REMOTE_DESKTOP_OBJECT_PATH, MUTTER_REMOTE_DESKTOP_INTERFACE, "CreateSession", &d_error, "=>o", &session_path);

  data.rd_session_path = g_strdup (session_path);
  if (!data.rd_session_path[0])
    return FALSE;

  dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "Start", &d_error, "");
  return TRUE;
}

static dbus_uint64_t
get_window_id (const char *name)
{
  DBusMessage *message, *reply;
  DBusError d_error;
  dbus_uint64_t window_id;
  DBusMessageIter iter, iter_array, iter_dict, iter_sub_array, iter_sub_dict;
  const char *prop_name;
  const char *cur_name;
  dbus_bool_t cur_focus;
  gboolean have_focus;

  dbus_error_init (&d_error);
  message = dbus_message_new_method_call (MUTTER_REMOTE_DESKTOP_BUS_NAME, "/org/gnome/Shell/Introspect", "org.gnome.Shell.Introspect", "GetWindows");
  reply = dbus_connection_send_with_reply_and_block (data.bus, message, -1, &d_error);
  dbus_message_unref (message);

  if (!reply)
    return FALSE;
  if (strcmp (dbus_message_get_signature (reply), "a{ta{sv}}") != 0)
    {
      dbus_message_unref (reply);
      return FALSE;
    }

  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
    {
      dbus_message_iter_recurse (&iter_array, &iter_dict);
      dbus_message_iter_get_basic (&iter_dict, &window_id);
      dbus_message_iter_next (&iter_dict);
      dbus_message_iter_recurse (&iter_dict, &iter_sub_array);
      cur_name = NULL;
      have_focus = FALSE;
      while (dbus_message_iter_get_arg_type (&iter_sub_array) != DBUS_TYPE_INVALID)
        {
          dbus_message_iter_recurse (&iter_sub_array, &iter_sub_dict);
          dbus_message_iter_get_basic (&iter_sub_dict, &prop_name);
          if (!strcmp (prop_name, "wm-class"))
            {
              DBusMessageIter iter_variant;
              dbus_message_iter_next (&iter_sub_dict);
              dbus_message_iter_recurse (&iter_sub_dict, &iter_variant);
              dbus_message_iter_get_basic (&iter_variant, &cur_name);
            }
          if (!strcmp (prop_name, "has-focus"))
            {
              DBusMessageIter iter_variant;
              dbus_message_iter_next (&iter_sub_dict);
              dbus_message_iter_recurse (&iter_sub_dict, &iter_variant);
              dbus_message_iter_get_basic (&iter_variant, &cur_focus);
              have_focus = TRUE;
            }
          if (cur_name && have_focus)
            {
              if ((name && !strcmp (name, cur_name)) || cur_focus)
                {
                  dbus_message_unref (reply);
                  return window_id;
                }
              break;
            }
          dbus_message_iter_next (&iter_sub_array);
        }
      dbus_message_iter_next (&iter_array);
    }

  dbus_message_unref (reply);
  return 0;
}

static gboolean
ensure_rd_session_id (GError **error)
{
  DBusMessage *message, *reply;
  DBusError d_error;
  const char *interface = "org.gnome.Mutter.RemoteDesktop.Session";
  const char *prop_name = "SessionId";
  DBusMessageIter iter, iter_variant;
  const char *session_id;

  if (data.rd_session_id)
    return (data.rd_session_id[0] != '\0');

  if (!ensure_rd_session_path (error))
    return FALSE;

  message = dbus_message_new_method_call (MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, "org.freedesktop.DBus.Properties", "Get");
  dbus_message_append_args (message, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &prop_name, DBUS_TYPE_INVALID);

  dbus_error_init (&d_error);
  reply = dbus_connection_send_with_reply_and_block (data.bus, message, -1, &d_error);
  dbus_message_unref (message);
  if (!reply)
    return FALSE;
  if (strcmp (dbus_message_get_signature (reply), "v") != 0)
    {
      dbus_message_unref (reply);
      return FALSE;
    }
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_variant);
  dbus_message_iter_get_basic (&iter_variant, &session_id);
  data.rd_session_id = g_strdup (session_id);
  dbus_message_unref (reply);
  return TRUE;
}

static gboolean
ensure_sc_session (GError **error)
{
  DBusMessage *message, *reply;
  DBusError d_error;
  DBusMessageIter iter, iter_array, iter_dict_entry, iter_variant;
  const char *prop_name = "remote-desktop-session-id";
  const char *sc_session_path;

  if (!ensure_rd_session_id (error))
    return FALSE;

  if (data.sc_session_path)
    return (data.sc_session_path[0] != '\0');

  message = dbus_message_new_method_call (MUTTER_SCREEN_CAST_BUS_NAME, MUTTER_SCREEN_CAST_OBJECT_PATH, MUTTER_SCREEN_CAST_INTERFACE, "CreateSession");
  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "{sv}", &iter_array);
  dbus_message_iter_open_container (&iter_array, DBUS_TYPE_DICT_ENTRY, NULL, &iter_dict_entry);
  dbus_message_iter_append_basic (&iter_dict_entry, DBUS_TYPE_STRING, &prop_name);
  dbus_message_iter_open_container (&iter_dict_entry, DBUS_TYPE_VARIANT, "s", &iter_variant);
  dbus_message_iter_append_basic (&iter_variant, DBUS_TYPE_STRING, &data.rd_session_id);
  dbus_message_iter_close_container (&iter_dict_entry, &iter_variant);
  dbus_message_iter_close_container (&iter_array, &iter_dict_entry);
  dbus_message_iter_close_container (&iter, &iter_array);
  dbus_error_init (&d_error);
  reply = dbus_connection_send_with_reply_and_block (data.bus, message, -1, &d_error);
  dbus_message_unref (message);
  if (!reply)
    return FALSE;
  if (!dbus_message_get_args (reply, NULL, DBUS_TYPE_OBJECT_PATH, &sc_session_path, DBUS_TYPE_INVALID))
    {
      dbus_message_unref (reply);
      return FALSE;
    }

  data.sc_session_path = g_strdup (sc_session_path);
  dbus_message_unref (reply);
  return TRUE;
}

static gboolean
init_mutter (gboolean need_window, GError **error)
{
  dbus_uint64_t window_id;
  const char *prop_name = "window-id";
  DBusError d_error;
  DBusMessageIter iter, iter_array, iter_dict_entry, iter_variant;
  DBusMessage *message, *reply;
  const char *sc_stream_path;

  if (!ensure_rd_session_path (error))
    return FALSE;

  if (!need_window)
    return TRUE;

  window_id = (data.window_id_is_explicit) ? data.window_id
                                           : get_window_id (NULL);
  if (!window_id)
    return FALSE;

  if (!ensure_sc_session (error))
    return FALSE;

  if (window_id == data.window_id)
    return TRUE;

  message = dbus_message_new_method_call (MUTTER_SCREEN_CAST_BUS_NAME, data.sc_session_path, MUTTER_SCREEN_CAST_SESSION_INTERFACE, "RecordWindow");
  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "{sv}", &iter_array);
  dbus_message_iter_open_container (&iter_array, DBUS_TYPE_DICT_ENTRY, NULL, &iter_dict_entry);
  dbus_message_iter_append_basic (&iter_dict_entry, DBUS_TYPE_STRING, &prop_name);
  dbus_message_iter_open_container (&iter_dict_entry, DBUS_TYPE_VARIANT, "t", &iter_variant);
  dbus_message_iter_append_basic (&iter_variant, DBUS_TYPE_UINT64, &window_id);
  dbus_message_iter_close_container (&iter_dict_entry, &iter_variant);
  dbus_message_iter_close_container (&iter_array, &iter_dict_entry);
  dbus_message_iter_close_container (&iter, &iter_array);
  dbus_error_init (&d_error);
  reply = dbus_connection_send_with_reply_and_block (data.bus, message, -1, &d_error);
  dbus_message_unref (message);
  if (!reply)
    return FALSE;
  if (!dbus_message_get_args (reply, NULL, DBUS_TYPE_OBJECT_PATH, &sc_stream_path, DBUS_TYPE_INVALID))
    {
      dbus_message_unref (reply);
      return FALSE;
    }

  data.sc_stream_path = g_strdup (sc_stream_path);
  dbus_message_unref (reply);
  data.window_id = window_id;
  return TRUE;
}

gboolean
_atspi_mutter_generate_keyboard_event (glong keyval,
                                       const gchar *keystring,
                                       AtspiKeySynthType synth_type,
                                       GError **error)
{
  DBusError d_error;
  dbus_uint32_t d_keyval = keyval;

  if (!init_mutter (FALSE, error))
    return FALSE;

  dbus_error_init (&d_error);
  switch (synth_type)
    {
    case ATSPI_KEY_PRESS:
      dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyKeyboardKeycode", &d_error, "ub", d_keyval, TRUE);
      break;
    case ATSPI_KEY_RELEASE:
      dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyKeyboardKeycode", &d_error, "ub", d_keyval, FALSE);
      break;
    case ATSPI_KEY_PRESSRELEASE:
      dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyKeyboardKeycode", &d_error, "ub", d_keyval, TRUE);
      dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyKeyboardKeycode", &d_error, "ub", d_keyval, FALSE);
      break;
    case ATSPI_KEY_SYM:
      dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyKeyboardKeysym", &d_error, "ub", d_keyval, TRUE);
      dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyKeyboardKeysym", &d_error, "ub", d_keyval, FALSE);
      break;
    default:
      /* TODO: set error */
      g_warning ("%s: unsupported type", __func__);
      return FALSE;
    }
  if (dbus_error_is_set (&d_error))
    {
      g_warning ("GenerateKeyboardEvent failed: %s", d_error.message);
      dbus_error_free (&d_error);
      return FALSE;
    }

  return TRUE;
}

gboolean
_atspi_mutter_generate_mouse_event (glong x, glong y, const gchar *name, GError **error)
{
  gint button = 0;
  double d_x = x, d_y = y;
  DBusError d_error;

  if (!init_mutter (TRUE, error))
    return FALSE;

  dbus_error_init (&d_error);
  switch (name[0])
    {
    case 'b':
      button = name[1] - '1';
      if (button < 0 || button > 4)
        return FALSE;
      if (x != -1 && y != -1)
        dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerMotionAbsolute", &d_error, "sdd", data.sc_stream_path, d_x, d_y);
      switch (name[2])
        {
        case 'p':
          dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerButton", &d_error, "ib", button, TRUE);
          break;
        case 'r':
          dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerButton", &d_error, "ib", button, FALSE);
          break;
        case 'c':
          dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerButton", &d_error, "ib", button, TRUE);
          dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerButton", &d_error, "ib", button, FALSE);
          break;
        case 'd':
          dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerButton", &d_error, "ib", button, TRUE);
          dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerButton", &d_error, "ib", button, FALSE);
          dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerButton", &d_error, "ib", button, TRUE);
          dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerButton", &d_error, "ib", button, FALSE);
          break;
        default:
          return FALSE;
        }
      break;
    case 'a': /* absolute motion */
      dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerMotionAbsolute", &d_error, "sdd", data.sc_stream_path, d_x, d_y);
      break;
    case 'r': /* relative */
      dbind_method_call_reentrant (data.bus, MUTTER_REMOTE_DESKTOP_BUS_NAME, data.rd_session_path, MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE, "NotifyPointerMotionRelative", &d_error, "dd", d_x, d_y);
      break;
    default:
      return FALSE;
    }
  return TRUE;
}

void
_atspi_mutter_set_reference_window (AtspiAccessible *accessible)
{
  if (accessible)
    {
      AtspiRole role = atspi_accessible_get_role (accessible, NULL);
      gchar *name;
      g_return_if_fail (role != ATSPI_ROLE_APPLICATION);
      name = atspi_accessible_get_name (accessible, NULL);
      data.window_id = get_window_id (name);
      data.window_id_is_explicit = TRUE;
    }

  else
    {
      data.window_id_is_explicit = FALSE;
    }
}
