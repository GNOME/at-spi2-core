/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2020 SUSE LLC.
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

#include "atspi-private.h"

#include "atspi-device-legacy.h"
#include "atspi-device-mutter.h"
#ifdef HAVE_X11
#include "atspi-device-x11.h"
#endif
#include "atspi-dbus-mutter-remote-desktop.h"
#include "atspi-dbus-mutter-screen-cast.h"
#include "atspi-dbus-shell-introspect.h"
#include <libei.h>

#include <linux/input-event-codes.h> /* for BTN_LEFT, etc */

#define MUTTER_REMOTE_DESKTOP_BUS_NAME "org.gnome.Mutter.RemoteDesktop"
#define MUTTER_REMOTE_DESKTOP_OBJECT_PATH "/org/gnome/Mutter/RemoteDesktop"
#define MUTTER_REMOTE_DESKTOP_INTERFACE "org.gnome.Mutter.RemoteDesktop"
#define MUTTER_REMOTE_DESKTOP_SESSION_INTERFACE "org.gnome.Mutter.RemoteDesktop.Session"
#define MUTTER_SCREEN_CAST_BUS_NAME "org.gnome.Mutter.ScreenCast"
#define MUTTER_SCREEN_CAST_OBJECT_PATH "/org/gnome/Mutter/ScreenCast"
#define MUTTER_SCREEN_CAST_INTERFACE "org.gnome.Mutter.ScreenCast"
#define MUTTER_SCREEN_CAST_SESSION_INTERFACE "org.gnome.Mutter.ScreenCast.Session"

typedef struct _AtspiDeviceMutterPrivate AtspiDeviceMutterPrivate;
struct _AtspiDeviceMutterPrivate
{
  AtspiDevice *keyboard_device;
  GDBusConnection *bus;
  guint mutter_remote_desktop_watch_name_id;
  guint mutter_screen_cast_watch_name_id;
  AtspiDBusMutterRemoteDesktop *mutter_remote_desktop_proxy;
  AtspiDBusMutterScreenCast *mutter_screen_cast_proxy;
  guint64 window_id;

  AtspiDBusMutterRemoteDesktopSession *remote_desktop_session;
  AtspiDBusMutterScreenCastSession *screen_cast_session;
  AtspiDBusMutterScreenCastStream *stream_proxy;
  AtspiDBusShellIntrospect *introspect_proxy;

  struct ei *ei;
  struct ei_seat *ei_seat;
  struct ei_device *ei_abs_pointer;
  struct ei_device *ei_keyboard;
  uint32_t ei_sequence;
  GSource *ei_source;

  GCancellable *cancellable;

  gboolean started;

  GSList *pending_clicks;
};

typedef struct _GrdFdSource
{
  GSource source;

  GSourceFunc prepare;
  GSourceFunc dispatch;
  gpointer user_data;

  GPollFD poll_fd;
} GrdFdSource;

typedef struct _MouseClickData
{
  AtspiAccessible *accessible;
  AtspiAccessible *root_accessible;
  gchar *window_name;
  gint x;
  gint y;
  gchar *event;
} MouseClickData;

GObjectClass *device_mutter_parent_class;

G_DEFINE_TYPE_WITH_CODE (AtspiDeviceMutter, atspi_device_mutter, ATSPI_TYPE_DEVICE, G_ADD_PRIVATE (AtspiDeviceMutter))

enum
{
  DEVICE_TYPE_KEYBOARD = 1,
  DEVICE_TYPE_POINTER = 2,
  DEVICE_TYPE_TOUCHSCREEN = 4,
};

static void start_mouse_click (AtspiDeviceMutter *device);
static void atspi_device_mutter_record_window (AtspiDeviceMutter *device);
static GQuark window_id_quark;

static guint
atspi_device_mutter_map_modifier (AtspiDevice *device, gint keycode)
{
  AtspiDeviceMutter *mutter_device = ATSPI_DEVICE_MUTTER (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (mutter_device);

  return atspi_device_map_modifier (priv->keyboard_device, keycode);
}

static void
atspi_device_mutter_unmap_modifier (AtspiDevice *device, gint keycode)
{
  AtspiDeviceMutter *mutter_device = ATSPI_DEVICE_MUTTER (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (mutter_device);

  atspi_device_unmap_modifier (priv->keyboard_device, keycode);
}

static void
clear_session (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  g_clear_pointer (&priv->ei_abs_pointer, ei_device_unref);
  g_clear_pointer (&priv->ei_seat, ei_seat_unref);
  g_clear_pointer (&priv->ei_source, g_source_destroy);
  g_clear_pointer (&priv->ei, ei_unref);

  g_clear_object (&priv->remote_desktop_session);
  g_clear_object (&priv->screen_cast_session);
}

static gboolean
fd_source_prepare (GSource *source,
                   int *timeout_ms)
{
  GrdFdSource *fd_source = (GrdFdSource *) source;

  *timeout_ms = -1;

  return fd_source->prepare (fd_source->user_data);
}

static gboolean
fd_source_check (GSource *source)
{
  GrdFdSource *fd_source = (GrdFdSource *) source;

  return !!(fd_source->poll_fd.revents & G_IO_IN);
}

static gboolean
fd_source_dispatch (GSource *source,
                    GSourceFunc callback,
                    gpointer user_data)
{
  GrdFdSource *fd_source = (GrdFdSource *) source;

  return fd_source->dispatch (fd_source->user_data);
}

static void
fd_source_finalize (GSource *source)
{
  GrdFdSource *fd_source = (GrdFdSource *) source;

  close (fd_source->poll_fd.fd);
}

static GSourceFuncs fd_source_funcs = {
  .prepare = fd_source_prepare,
  .check = fd_source_check,
  .dispatch = fd_source_dispatch,
  .finalize = fd_source_finalize,
};

GSource *
create_fd_source (int fd,
                  const char *name,
                  GSourceFunc prepare,
                  GSourceFunc dispatch,
                  gpointer user_data,
                  GDestroyNotify notify)
{
  GSource *source;
  GrdFdSource *fd_source;

  source = g_source_new (&fd_source_funcs, sizeof (GrdFdSource));
  g_source_set_name (source, name);
  fd_source = (GrdFdSource *) source;

  fd_source->poll_fd.fd = fd;
  fd_source->poll_fd.events = G_IO_IN;

  fd_source->prepare = prepare;
  fd_source->dispatch = dispatch;
  fd_source->user_data = user_data;

  g_source_set_callback (source, dispatch, user_data, notify);
  g_source_add_poll (source, &fd_source->poll_fd);

  return source;
}

static void
session_stop (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  if (priv->remote_desktop_session && priv->started)
    {
      AtspiDBusMutterRemoteDesktopSession *proxy = priv->remote_desktop_session;
      g_autoptr (GError) error = NULL;

      if (!atspi_dbus_mutter_remote_desktop_session_call_stop_sync (proxy, NULL, &error))
        {
          if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CLOSED) &&
              !g_error_matches (error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT))
            g_warning ("Failed to stop: %s", error->message);
        }
    }

  clear_session (device);
}

static gboolean
ei_source_prepare (gpointer user_data)
{
  AtspiDeviceMutter *device = ATSPI_DEVICE_MUTTER (user_data);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  return !!ei_peek_event (priv->ei);
}

static void
notify_pointer_button (AtspiDeviceMutter *device,
                       gint button,
                       gboolean pressed)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  int32_t button_code;

  if (!priv->ei_abs_pointer)
    return;

  switch (button)
    {
    case 1:
      button_code = BTN_LEFT;
      break;
    case 2:
      button_code = BTN_MIDDLE;
      break;
    case 3:
      button_code = BTN_RIGHT;
      break;
    case 8:
      button_code = BTN_SIDE;
      break;
    case 9:
      button_code = BTN_EXTRA;
      break;
    default:
      g_warning ("Unsupported button number %i", button);
      button_code = 0;
    }

  ei_device_button_button (priv->ei_abs_pointer, button_code, pressed);
  ei_device_frame (priv->ei_abs_pointer, g_get_monotonic_time ());
}

static void
notify_pointer_motion_absolute (AtspiDeviceMutter *device,
                                double x,
                                double y)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  ei_device_pointer_motion_absolute (priv->ei_abs_pointer, x, y);
  ei_device_frame (priv->ei_abs_pointer, ei_now (priv->ei));
}

static void
notify_pointer_motion_relative (AtspiDeviceMutter *device,
                                double dx,
                                double dy)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  ei_device_scroll_delta (priv->ei_abs_pointer, dx, dy);
  ei_device_frame (priv->ei_abs_pointer, g_get_monotonic_time ());
}

static MouseClickData *
get_pending_click (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  if (!priv->pending_clicks)
    return NULL;

  return priv->pending_clicks->data;
}

static void
mouse_click_data_free (MouseClickData *click_data)
{
  g_clear_object (&click_data->accessible);
  g_clear_object (&click_data->root_accessible);
  g_clear_pointer (&click_data->window_name, g_free);
  g_clear_pointer (&click_data->event, g_free);
  g_free (click_data);
}

static void
mouse_click_end (AtspiDeviceMutter *device, gboolean do_next)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  MouseClickData *click_data = get_pending_click (device);

  mouse_click_data_free (click_data);
  priv->pending_clicks = g_slist_remove (priv->pending_clicks, click_data);
  if (do_next && priv->pending_clicks)
    start_mouse_click (device);
  else if (priv->pending_clicks)
    g_slist_free_full (priv->pending_clicks, (GDestroyNotify) mouse_click_data_free);
}

static gboolean
finish_mouse_click (AtspiDeviceMutter *device)
{
  MouseClickData *click_data = get_pending_click (device);
  gint button = 0;

  if (!click_data)
    return TRUE;

  switch (click_data->event[0])
    {
    case 'b':
      button = click_data->event[1] - '0';
      if (button < 1 || button > 5)
        {
          mouse_click_end (device, TRUE);
          return FALSE;
        }

      if (click_data->x != -1 && click_data->y != -1)
        notify_pointer_motion_absolute (device, click_data->x, click_data->y);
      switch (click_data->event[2])
        {
        case 'p':
          notify_pointer_button (device, button, TRUE);
          break;
        case 'r':
          notify_pointer_button (device, button, FALSE);
          break;
        case 'c':
          notify_pointer_button (device, button, TRUE);
          notify_pointer_button (device, button, FALSE);
          break;
        case 'd':
          notify_pointer_button (device, button, TRUE);
          notify_pointer_button (device, button, FALSE);
          notify_pointer_button (device, button, TRUE);
          notify_pointer_button (device, button, FALSE);
          break;
        default:
          mouse_click_end (device, TRUE);
          return FALSE;
        }
      break;
    case 'a': /* absolute motion */
      notify_pointer_motion_absolute (device, click_data->x, click_data->y);
      break;
    case 'r': /* relative */
      notify_pointer_motion_relative (device, click_data->x, click_data->y);
      break;
    default:
      mouse_click_end (device, TRUE);
    }

  mouse_click_end (device, TRUE);
  return TRUE;
}

static gboolean
ei_source_dispatch (gpointer user_data)
{
  AtspiDeviceMutter *device = ATSPI_DEVICE_MUTTER (user_data);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  struct ei_event *event;

  ei_dispatch (priv->ei);

  while ((event = ei_get_event (priv->ei)))
    {
      switch (ei_event_get_type (event))
        {
        case EI_EVENT_CONNECT:
          break;
        case EI_EVENT_DISCONNECT:
          break;
        case EI_EVENT_SEAT_ADDED:
          if (priv->ei_seat)
            break;

          priv->ei_seat = ei_seat_ref (ei_event_get_seat (event));
          ei_seat_bind_capabilities (priv->ei_seat,
                                     EI_DEVICE_CAP_POINTER,
                                     EI_DEVICE_CAP_POINTER_ABSOLUTE,
                                     EI_DEVICE_CAP_BUTTON,
                                     EI_DEVICE_CAP_SCROLL,
                                     NULL);
          break;
        case EI_EVENT_SEAT_REMOVED:
          if (ei_event_get_seat (event) == priv->ei_seat)
            g_clear_pointer (&priv->ei_seat, ei_seat_unref);
          break;
        case EI_EVENT_DEVICE_ADDED:
          {
            struct ei_device *ei_device = ei_event_get_device (event);

            if (ei_device_has_capability (ei_device, EI_DEVICE_CAP_POINTER_ABSOLUTE) &&
                strstr (ei_device_get_name (ei_device), "standalone"))
              {
                g_clear_pointer (&priv->ei_abs_pointer, ei_device_unref);
                priv->ei_abs_pointer = ei_device_ref (ei_device);
              }
            break;
          }
        case EI_EVENT_DEVICE_RESUMED:
          if (ei_event_get_device (event) == priv->ei_abs_pointer)
            ei_device_start_emulating (priv->ei_abs_pointer, ++priv->ei_sequence);
          if (ei_event_get_device (event) == priv->ei_keyboard)
            ei_device_start_emulating (priv->ei_keyboard, ++priv->ei_sequence);
          break;
        case EI_EVENT_DEVICE_PAUSED:
          break;
        case EI_EVENT_DEVICE_REMOVED:
          if (ei_event_get_device (event) == priv->ei_abs_pointer)
            g_clear_pointer (&priv->ei_abs_pointer, ei_device_unref);
          if (ei_event_get_device (event) == priv->ei_keyboard)
            g_clear_pointer (&priv->ei_keyboard, ei_device_unref);
          break;
        default:
          break;
        }
      ei_event_unref (event);
    }

  return G_SOURCE_CONTINUE;
}

static void
on_remote_desktop_session_closed (AtspiDBusMutterRemoteDesktopSession *session_proxy,
                                  AtspiDeviceMutter *device)
{
  g_debug ("Remote desktop session closed, stopping session");

  clear_session (device);
  session_stop (device);
}

static void
on_eis_connected (GObject *object,
                  GAsyncResult *result,
                  gpointer user_data)
{
  AtspiDBusMutterRemoteDesktopSession *proxy =
      ATSPI_DBUS_MUTTER_REMOTE_DESKTOP_SESSION (object);
  g_autoptr (GVariant) fd_variant = NULL;
  g_autoptr (GUnixFDList) fd_list = NULL;
  AtspiDeviceMutter *device;
  AtspiDeviceMutterPrivate *priv;
  int fd;
  g_autoptr (GError) error = NULL;
  int ret;

  if (!atspi_dbus_mutter_remote_desktop_session_call_connect_to_eis_finish (proxy,
                                                                            &fd_variant,
                                                                            &fd_list,
                                                                            result,
                                                                            &error))
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Failed to connect to EIS: %s",
                 error->message);
      session_stop (ATSPI_DEVICE_MUTTER (user_data));
      return;
    }

  device = ATSPI_DEVICE_MUTTER (user_data);
  priv = atspi_device_mutter_get_instance_private (device);

  fd = g_unix_fd_list_get (fd_list, g_variant_get_handle (fd_variant), &error);

  priv->ei = ei_new_sender (device);
  /* TODO: allow name to be configurable? */
  ei_configure_name (priv->ei, "at-spi");

  ret = ei_setup_backend_fd (priv->ei, fd);
  if (ret < 0)
    {
      g_warning ("Failed to setup libei backend: %s", g_strerror (-ret));

      session_stop (device);
      return;
    }

  priv->ei_source = create_fd_source (ei_get_fd (priv->ei),
                                      "libei",
                                      ei_source_prepare,
                                      ei_source_dispatch,
                                      device, NULL);
  g_source_attach (priv->ei_source, NULL);
  g_source_unref (priv->ei_source);

  g_signal_connect (priv->remote_desktop_session, "closed",
                    G_CALLBACK (on_remote_desktop_session_closed),
                    device);
}

static void
connect_to_eis (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  GVariantBuilder options_builder;
  guint device_types = DEVICE_TYPE_POINTER;

  g_variant_builder_init (&options_builder, G_VARIANT_TYPE ("a{sv}"));
  g_variant_builder_add (&options_builder,
                         "{sv}",
                         "device-types",
                         g_variant_new_uint32 (device_types));

  atspi_dbus_mutter_remote_desktop_session_call_connect_to_eis (
      priv->remote_desktop_session,
      g_variant_builder_end (&options_builder),
      NULL,
      priv->cancellable,
      on_eis_connected,
      device);
}

static void
on_session_start_finished (GObject *object,
                           GAsyncResult *result,
                           gpointer user_data)
{
  AtspiDBusMutterRemoteDesktopSession *proxy;
  AtspiDeviceMutter *device;
  AtspiDeviceMutterPrivate *priv;
  g_autoptr (GError) error = NULL;

  proxy = ATSPI_DBUS_MUTTER_REMOTE_DESKTOP_SESSION (object);
  if (!atspi_dbus_mutter_remote_desktop_session_call_start_finish (proxy,
                                                                   result,
                                                                   &error))
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Failed to start session: %s", error->message);
      session_stop (ATSPI_DEVICE_MUTTER (user_data));
      return;
    }

  device = ATSPI_DEVICE_MUTTER (user_data);
  priv = atspi_device_mutter_get_instance_private (device);

  priv->started = TRUE;

  connect_to_eis (device);

  if (priv->pending_clicks)
    atspi_device_mutter_record_window (device);
}

static void
start_session (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  AtspiDBusMutterRemoteDesktopSession *proxy = priv->remote_desktop_session;

  atspi_dbus_mutter_remote_desktop_session_call_start (proxy,
                                                       priv->cancellable,
                                                       on_session_start_finished,
                                                       device);
}

static void
on_screen_cast_session_proxy_acquired (GObject *object,
                                       GAsyncResult *result,
                                       gpointer user_data)
{
  AtspiDBusMutterScreenCastSession *session_proxy;
  AtspiDeviceMutter *device;
  AtspiDeviceMutterPrivate *priv;
  g_autoptr (GError) error = NULL;

  session_proxy =
      atspi_dbus_mutter_screen_cast_session_proxy_new_finish (result, &error);
  if (!session_proxy)
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Failed to acquire screen cast session proxy: %s\n",
                 error->message);
      session_stop (ATSPI_DEVICE_MUTTER (user_data));
      return;
    }

  device = ATSPI_DEVICE_MUTTER (user_data);
  priv = atspi_device_mutter_get_instance_private (device);

  priv->screen_cast_session = session_proxy;

  start_session (device);
}

static void
on_screen_cast_session_created (GObject *source_object,
                                GAsyncResult *res,
                                gpointer user_data)
{
  AtspiDBusMutterScreenCast *screen_cast_proxy;
  AtspiDeviceMutter *device;
  AtspiDeviceMutterPrivate *priv;
  GDBusConnection *connection;
  g_autofree char *session_path = NULL;
  g_autoptr (GError) error = NULL;

  screen_cast_proxy = ATSPI_DBUS_MUTTER_SCREEN_CAST (source_object);
  if (!atspi_dbus_mutter_screen_cast_call_create_session_finish (screen_cast_proxy,
                                                                 &session_path,
                                                                 res,
                                                                 &error))
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Failed to start screen cast session: %s\n", error->message);
      session_stop (ATSPI_DEVICE_MUTTER (user_data));
      return;
    }

  device = ATSPI_DEVICE_MUTTER (user_data);
  priv = atspi_device_mutter_get_instance_private (device);
  connection = g_dbus_proxy_get_connection (G_DBUS_PROXY (screen_cast_proxy));

  atspi_dbus_mutter_screen_cast_session_proxy_new (connection,
                                                   G_DBUS_PROXY_FLAGS_NONE,
                                                   MUTTER_SCREEN_CAST_BUS_NAME,
                                                   session_path,
                                                   priv->cancellable,
                                                   on_screen_cast_session_proxy_acquired,
                                                   device);
}

static guint
atspi_device_mutter_get_modifier (AtspiDevice *device, gint keycode)
{
  AtspiDeviceMutter *mutter_device = ATSPI_DEVICE_MUTTER (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (mutter_device);

  return atspi_device_get_modifier (priv->keyboard_device, keycode);
}

static void
on_remote_desktop_session_proxy_acquired (GObject *object,
                                          GAsyncResult *result,
                                          gpointer user_data)
{
  AtspiDBusMutterRemoteDesktopSession *session_proxy;
  AtspiDeviceMutter *device;
  AtspiDeviceMutterPrivate *priv;
  g_autoptr (GError) error = NULL;
  GVariantBuilder properties_builder;
  GVariant *properties_variant;
  const char *remote_desktop_session_id;

  session_proxy =
      atspi_dbus_mutter_remote_desktop_session_proxy_new_finish (result, &error);
  if (!session_proxy)
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Failed to acquire remote desktop session proxy: %s",
                 error->message);
      session_stop (ATSPI_DEVICE_MUTTER (user_data));
      return;
    }

  device = ATSPI_DEVICE_MUTTER (user_data);
  priv = atspi_device_mutter_get_instance_private (device);

  priv->remote_desktop_session = session_proxy;
  remote_desktop_session_id =
      atspi_dbus_mutter_remote_desktop_session_get_session_id (priv->remote_desktop_session);

  g_variant_builder_init (&properties_builder, G_VARIANT_TYPE ("a{sv}"));
  g_variant_builder_add (&properties_builder, "{sv}",
                         "remote-desktop-session-id",
                         g_variant_new_string (remote_desktop_session_id));
  g_variant_builder_add (&properties_builder, "{sv}",
                         "disable-animations",
                         g_variant_new_boolean (TRUE));
  properties_variant = g_variant_builder_end (&properties_builder);

  atspi_dbus_mutter_screen_cast_call_create_session (priv->mutter_screen_cast_proxy,
                                                     properties_variant,
                                                     priv->cancellable,
                                                     on_screen_cast_session_created,
                                                     device);
}

static void
on_remote_desktop_session_created (GObject *source_object,
                                   GAsyncResult *res,
                                   gpointer user_data)
{
  AtspiDBusMutterRemoteDesktop *remote_desktop_proxy;
  AtspiDeviceMutter *device;
  AtspiDeviceMutterPrivate *priv;
  GDBusConnection *connection;
  g_autofree char *session_path = NULL;
  g_autoptr (GError) error = NULL;

  remote_desktop_proxy = ATSPI_DBUS_MUTTER_REMOTE_DESKTOP (source_object);
  if (!atspi_dbus_mutter_remote_desktop_call_create_session_finish (remote_desktop_proxy,
                                                                    &session_path,
                                                                    res,
                                                                    &error))
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Failed to start remote desktop session: %s\n", error->message);
      session_stop (ATSPI_DEVICE_MUTTER (user_data));
      return;
    }

  device = ATSPI_DEVICE_MUTTER (user_data);
  priv = atspi_device_mutter_get_instance_private (device);
  connection = g_dbus_proxy_get_connection (G_DBUS_PROXY (remote_desktop_proxy));

  atspi_dbus_mutter_remote_desktop_session_proxy_new (connection,
                                                      G_DBUS_PROXY_FLAGS_NONE,
                                                      MUTTER_REMOTE_DESKTOP_BUS_NAME,
                                                      session_path,
                                                      priv->cancellable,
                                                      on_remote_desktop_session_proxy_acquired,
                                                      device);
}

static void
session_start (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  atspi_dbus_mutter_remote_desktop_call_create_session (priv->mutter_remote_desktop_proxy,
                                                        priv->cancellable,
                                                        on_remote_desktop_session_created,
                                                        device);
}

static void
on_mutter_remote_desktop_proxy_acquired (GObject *object,
                                         GAsyncResult *result,
                                         gpointer user_data)
{
  AtspiDeviceMutter *device = user_data;
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  AtspiDBusMutterRemoteDesktop *proxy;
  GError *error = NULL;

  proxy = atspi_dbus_mutter_remote_desktop_proxy_new_finish (result, &error);
  if (!proxy)
    {
      g_warning ("Failed to create remote desktop proxy: %s", error->message);
      g_error_free (error);
      return;
    }

  priv->mutter_remote_desktop_proxy = proxy;

  if (priv->pending_clicks)
    start_mouse_click (device);
  else
    session_start (device);
}

static void
on_mutter_screen_cast_proxy_acquired (GObject *object,
                                      GAsyncResult *result,
                                      gpointer user_data)
{
  AtspiDeviceMutter *device = user_data;
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  AtspiDBusMutterScreenCast *proxy;
  GError *error = NULL;

  proxy = atspi_dbus_mutter_screen_cast_proxy_new_finish (result, &error);
  if (!proxy)
    {
      g_warning ("Failed to create screen cast proxy: %s", error->message);
      return;
    }

  priv->mutter_screen_cast_proxy = proxy;
}

static void
on_mutter_remote_desktop_name_appeared (GDBusConnection *connection,
                                        const char *name,
                                        const char *name_owner,
                                        gpointer user_data)
{
  AtspiDeviceMutter *device = user_data;
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  atspi_dbus_mutter_remote_desktop_proxy_new (connection,
                                              G_DBUS_PROXY_FLAGS_NONE,
                                              MUTTER_REMOTE_DESKTOP_BUS_NAME,
                                              MUTTER_REMOTE_DESKTOP_OBJECT_PATH,
                                              priv->cancellable,
                                              on_mutter_remote_desktop_proxy_acquired,
                                              device);
}

static void
on_mutter_remote_desktop_name_vanished (GDBusConnection *connection,
                                        const char *name,
                                        gpointer user_data)
{
  AtspiDeviceMutter *device = user_data;
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  priv->mutter_remote_desktop_proxy = NULL;
}

static void
on_mutter_screen_cast_name_appeared (GDBusConnection *connection,
                                     const char *name,
                                     const char *name_owner,
                                     gpointer user_data)
{
  AtspiDeviceMutter *device = user_data;
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  atspi_dbus_mutter_screen_cast_proxy_new (connection,
                                           G_DBUS_PROXY_FLAGS_NONE,
                                           MUTTER_SCREEN_CAST_BUS_NAME,
                                           MUTTER_SCREEN_CAST_OBJECT_PATH,
                                           priv->cancellable,
                                           on_mutter_screen_cast_proxy_acquired,
                                           device);
}

static void
on_mutter_screen_cast_name_vanished (GDBusConnection *connection,
                                     const char *name,
                                     gpointer user_data)
{
  AtspiDeviceMutter *device = user_data;
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  priv->mutter_screen_cast_proxy = NULL;
}

static void
acquire_mutter_dbus_proxies (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  g_clear_handle_id (&priv->mutter_remote_desktop_watch_name_id, g_bus_unwatch_name);
  priv->mutter_remote_desktop_watch_name_id =
      g_bus_watch_name_on_connection (priv->bus,
                                      MUTTER_REMOTE_DESKTOP_BUS_NAME,
                                      G_BUS_NAME_WATCHER_FLAGS_NONE,
                                      on_mutter_remote_desktop_name_appeared,
                                      on_mutter_remote_desktop_name_vanished,
                                      g_object_ref (device), NULL);

  g_clear_handle_id (&priv->mutter_screen_cast_watch_name_id, g_bus_unwatch_name);
  priv->mutter_screen_cast_watch_name_id =
      g_bus_watch_name_on_connection (priv->bus,
                                      MUTTER_SCREEN_CAST_BUS_NAME,
                                      G_BUS_NAME_WATCHER_FLAGS_NONE,
                                      on_mutter_screen_cast_name_appeared,
                                      on_mutter_screen_cast_name_vanished,
                                      device, NULL);
}

static void
on_screen_cast_stream_start_finished (GObject *object,
                                      GAsyncResult *result,
                                      gpointer user_data)
{
  AtspiDBusMutterScreenCastStream *stream_proxy = ATSPI_DBUS_MUTTER_SCREEN_CAST_STREAM (object);
  g_autoptr (GError) error = NULL;
  AtspiDeviceMutter *device = user_data;

  if (!atspi_dbus_mutter_screen_cast_stream_call_start_finish (stream_proxy,
                                                               result,
                                                               &error))
    {
      mouse_click_end (device, FALSE);
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Failed to start screen cast stream: %s", error->message);
      session_stop (device);
      return;
    }

  finish_mouse_click (device);
}

static void
on_screen_cast_stream_proxy_acquired (GObject *object,
                                      GAsyncResult *result,
                                      gpointer user_data)
{
  AtspiDeviceMutter *device = user_data;
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  g_autoptr (AtspiDBusMutterScreenCastStream) stream_proxy = NULL;
  g_autoptr (GError) error = NULL;

  stream_proxy = atspi_dbus_mutter_screen_cast_stream_proxy_new_finish (result, &error);
  if (!stream_proxy)
    {
      mouse_click_end (device, FALSE);
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Failed to acquire stream proxy: %s", error->message);
      session_stop (device);
      return;
    }

  /* TODO: clear old proxy */
  priv->stream_proxy = stream_proxy;

  atspi_dbus_mutter_screen_cast_stream_call_start (stream_proxy,
                                                   priv->cancellable,
                                                   on_screen_cast_stream_start_finished,
                                                   device);
}

static void
on_record_finished (GObject *object,
                    GAsyncResult *result,
                    gpointer user_data)
{
  AtspiDBusMutterScreenCastSession *proxy = ATSPI_DBUS_MUTTER_SCREEN_CAST_SESSION (object);
  AtspiDeviceMutter *device = user_data;
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  GDBusConnection *connection;
  g_autofree char *stream_path = NULL;
  g_autoptr (GError) error = NULL;
  gboolean retval = FALSE;

  retval = atspi_dbus_mutter_screen_cast_session_call_record_window_finish (
      proxy, &stream_path, result, &error);

  if (!retval)
    {
      mouse_click_end (device, TRUE);
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        return;

      g_warning ("Failed to record window: %s",
                 error->message);
      session_stop (device);
      return;
    }

  connection = g_dbus_proxy_get_connection (G_DBUS_PROXY (proxy));

  atspi_dbus_mutter_screen_cast_stream_proxy_new (connection,
                                                  G_DBUS_PROXY_FLAGS_NONE,
                                                  MUTTER_SCREEN_CAST_BUS_NAME,
                                                  stream_path,
                                                  priv->cancellable,
                                                  on_screen_cast_stream_proxy_acquired,
                                                  device);
}

static void
atspi_device_mutter_record_window (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  GVariantBuilder properties_builder;

  g_variant_builder_init (&properties_builder, G_VARIANT_TYPE ("a{sv}"));
  g_variant_builder_add (&properties_builder, "{sv}",
                         "window-id", g_variant_new_uint64 (priv->window_id));

  atspi_dbus_mutter_screen_cast_session_call_record_window (priv->screen_cast_session,
                                                            g_variant_builder_end (&properties_builder),
                                                            priv->cancellable,
                                                            on_record_finished,
                                                            device);
}

static void
key_watcher (AtspiDevice *device, gboolean pressed, guint keycode, guint keysym, guint modifiers, const gchar *keystring, void *user_data)
{
  AtspiDevice *my_device = ATSPI_DEVICE (user_data);

  atspi_device_notify_key (my_device, pressed, keycode, keysym, modifiers, keystring);
}

static void
atspi_device_mutter_init (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  priv->bus = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, NULL);
  if (!priv->bus)
    return;
  /* TODO: Following line is for testing. Remove before commit. */
  g_bus_own_name (G_BUS_TYPE_SESSION, "org.gnome.orca", 0, NULL, NULL, NULL, NULL, NULL);

  priv->cancellable = g_cancellable_new ();

  acquire_mutter_dbus_proxies (device);

#ifdef HAVE_X11
  if (!g_getenv ("WAYLAND_DISPLAY"))
    priv->keyboard_device = ATSPI_DEVICE (atspi_device_x11_new ());
#endif

  if (!priv->keyboard_device)
    priv->keyboard_device = ATSPI_DEVICE (atspi_device_legacy_new ());
  atspi_device_add_key_watcher (priv->keyboard_device, key_watcher, device, NULL);
}

static void
atspi_device_mutter_finalize (GObject *object)
{
  AtspiDeviceMutter *device = ATSPI_DEVICE_MUTTER (object);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);

  session_stop (device);

  g_clear_handle_id (&priv->mutter_remote_desktop_watch_name_id, g_bus_unwatch_name);
  g_clear_handle_id (&priv->mutter_screen_cast_watch_name_id, g_bus_unwatch_name);
  g_clear_object (&priv->cancellable);
  g_clear_object (&priv->bus);

  g_clear_object (&priv->keyboard_device);

  g_slist_free_full (priv->pending_clicks, (GDestroyNotify) mouse_click_data_free);

  device_mutter_parent_class->finalize (object);
}

static gboolean
atspi_device_mutter_add_key_grab (AtspiDevice *device, AtspiKeyDefinition *kd)
{
  AtspiDeviceMutter *mutter_device = ATSPI_DEVICE_MUTTER (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (mutter_device);
  guint ret;

  ret = atspi_device_add_key_grab (priv->keyboard_device, kd, NULL, NULL, NULL);
  return (ret > 0);
}

static void
atspi_device_mutter_remove_key_grab (AtspiDevice *device, guint id)
{
  AtspiDeviceMutter *mutter_device = ATSPI_DEVICE_MUTTER (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (mutter_device);

  atspi_device_remove_key_grab (priv->keyboard_device, id);
}

static guint
atspi_device_mutter_get_locked_modifiers (AtspiDevice *device)
{
  AtspiDeviceMutter *mutter_device = ATSPI_DEVICE_MUTTER (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (mutter_device);

  return atspi_device_get_locked_modifiers (priv->keyboard_device);
}

static gboolean
atspi_device_mutter_grab_keyboard (AtspiDevice *device)
{
  AtspiDeviceMutter *mutter_device = ATSPI_DEVICE_MUTTER (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (mutter_device);

  return atspi_device_grab_keyboard (priv->keyboard_device);
}

static void
atspi_device_mutter_ungrab_keyboard (AtspiDevice *device)
{
  AtspiDeviceMutter *mutter_device = ATSPI_DEVICE_MUTTER (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (mutter_device);

  atspi_device_grab_keyboard (priv->keyboard_device);
}

static void
on_get_windows_finished (GObject *object,
                         GAsyncResult *result,
                         gpointer user_data)
{
  AtspiDBusShellIntrospect *proxy = ATSPI_DBUS_SHELL_INTROSPECT (object);
  AtspiDeviceMutter *device = user_data;
  MouseClickData *click_data = get_pending_click (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  const char *wm_class;
  gboolean has_focus;
  g_autoptr (GVariant) windows = NULL;
  GVariantIter iter;
  uint64_t cur_id, window_id = 0;
  GVariant *params = NULL;
  GError *error = NULL;
  gboolean retval = FALSE;

  retval = atspi_dbus_shell_introspect_call_get_windows_finish (
      proxy, &windows, result, &error);

  if (!retval)
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        {
          mouse_click_end (device, FALSE);
          return;
        }

      g_warning ("Failed to record window: %s",
                 error->message);
      session_stop (device);
      mouse_click_end (device, FALSE);
      return;
    }

  g_variant_iter_init (&iter, windows);

  while (g_variant_iter_loop (&iter, "{t@a{sv}}", &cur_id, &params))
    {
      g_variant_lookup (params, "wm-class", "s", &wm_class);
      g_variant_lookup (params, "has-focus", "b", &has_focus);
      if (wm_class && has_focus)
        {
          if ((click_data->window_name && !strcmp (click_data->window_name, wm_class)) || has_focus)
            {
              window_id = cur_id;
              break;
            }
          break;
        }
    }

  if (!window_id)
    {
      mouse_click_end (device, FALSE);
      return;
    }

  if (window_id == priv->window_id && priv->screen_cast_session)
    {
      finish_mouse_click (device);
      return;
    }

  priv->window_id = window_id;
  g_object_set_qdata (G_OBJECT (click_data->root_accessible), window_id_quark, (gpointer) window_id);
  if (priv->screen_cast_session)
    atspi_device_mutter_record_window (device);
}

static void
start_mouse_click (AtspiDeviceMutter *device)
{
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (device);
  gint i;
  AtspiAccessible *parent;
  g_autoptr (GError) error = NULL;
  MouseClickData *click_data = get_pending_click (device);
  AtspiAccessible *accessible = click_data->accessible;
  gulong window_id;

  for (i = 0; accessible; i++)
    {
      AtspiRole role = atspi_accessible_get_role (accessible, NULL);
      if (role == ATSPI_ROLE_APPLICATION)
        break;
      parent = atspi_accessible_get_parent (accessible, NULL);
      if (i)
        g_object_unref (accessible);
      accessible = parent;
    }

  click_data->window_name = atspi_accessible_get_name (accessible, NULL);
  click_data->root_accessible = accessible;
  if (!i)
    g_object_ref (click_data->root_accessible);

  if (!priv->screen_cast_session)
    session_start (device);

  window_id = (gulong) g_object_get_qdata (G_OBJECT (click_data->root_accessible), window_id_quark);
  if (window_id)
    {
      if (window_id == priv->window_id && priv->screen_cast_session)
        {
          finish_mouse_click (device);
          return;
        }

      priv->window_id = window_id;
      if (priv->screen_cast_session)
        atspi_device_mutter_record_window (device);
      return;
    }

  if (!priv->introspect_proxy)
    priv->introspect_proxy = atspi_dbus_shell_introspect_proxy_new_sync (priv->bus,
                                                                         G_DBUS_PROXY_FLAGS_NONE,
                                                                         MUTTER_REMOTE_DESKTOP_BUS_NAME,
                                                                         "/org/gnome/Shell/Introspect",
                                                                         priv->cancellable, &error);

  atspi_dbus_shell_introspect_call_get_windows (priv->introspect_proxy,
                                                priv->cancellable,
                                                on_get_windows_finished,
                                                device);
}

/* TODO: remove bool param */
static void
atspi_device_mutter_generate_mouse_event (AtspiDevice *device, AtspiAccessible *obj, gint x, gint y, const gchar *name, GError **error)
{
  AtspiDeviceMutter *mutter_device = ATSPI_DEVICE_MUTTER (device);
  AtspiDeviceMutterPrivate *priv = atspi_device_mutter_get_instance_private (mutter_device);
  AtspiPoint *p;
  MouseClickData *click_data;
  gboolean need_start;

  if (atspi_accessible_get_role (obj, NULL) != ATSPI_ROLE_APPLICATION)
    {
      p = atspi_component_get_position (ATSPI_COMPONENT (obj), ATSPI_COORD_TYPE_WINDOW, error);
      if (!p)
        return;
      x += p->x;
      y += p->y;
      g_free (p);
    }

  click_data = g_malloc0 (sizeof (MouseClickData));
  click_data->x = x;
  click_data->y = y;
  click_data->event = g_strdup (name);
  click_data->accessible = g_object_ref (obj);
  need_start = (priv->pending_clicks == NULL);
  priv->pending_clicks = g_slist_append (priv->pending_clicks, click_data);
  if (need_start && priv->mutter_remote_desktop_proxy)
    start_mouse_click (mutter_device);
}

static void
atspi_device_mutter_class_init (AtspiDeviceMutterClass *klass)
{
  AtspiDeviceClass *device_class = ATSPI_DEVICE_CLASS (klass);
  GObjectClass *object_class = (GObjectClass *) klass;

  window_id_quark = g_quark_from_static_string ("mutter-window-id");
  device_mutter_parent_class = g_type_class_peek_parent (klass);
  device_class->add_key_grab = atspi_device_mutter_add_key_grab;
  device_class->map_modifier = atspi_device_mutter_map_modifier;
  device_class->unmap_modifier = atspi_device_mutter_unmap_modifier;
  device_class->get_modifier = atspi_device_mutter_get_modifier;
  device_class->remove_key_grab = atspi_device_mutter_remove_key_grab;
  device_class->get_locked_modifiers = atspi_device_mutter_get_locked_modifiers;
  device_class->grab_keyboard = atspi_device_mutter_grab_keyboard;
  device_class->ungrab_keyboard = atspi_device_mutter_ungrab_keyboard;
  device_class->generate_mouse_event = atspi_device_mutter_generate_mouse_event;

  object_class->finalize = atspi_device_mutter_finalize;
}

/**
 * atspi_device_mutter_new:
 *
 * Creates a new #AtspiDeviceMutter.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDeviceMutter.
 *
 **/
AtspiDeviceMutter *
atspi_device_mutter_new ()
{
  AtspiDeviceMutter *device = g_object_new (atspi_device_mutter_get_type (), NULL);

  return device;
}
