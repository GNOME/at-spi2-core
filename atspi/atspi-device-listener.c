/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian Inc.
 * Copyright 2002 Sun Microsystems, Inc.
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

#include "atspi-private.h"
#include <stdio.h>

/**
 * AtspiDevicelistener:
 *
 * An interface for creating and manipulating
 * device listeners.
 *
 * An interface for creating and manipulating
 * device listeners with callback functions.
 */

typedef struct
{
  AtspiDeviceListenerCB callback;
  gpointer user_data;
  GDestroyNotify callback_destroyed;
} DeviceEventHandler;

GObjectClass *device_listener_parent_class;

/*
 * Misc. helpers.
 */

static DeviceEventHandler *
device_event_handler_new (AtspiDeviceListenerCB callback,
                          GDestroyNotify callback_destroyed,
                          gpointer user_data)
{
  DeviceEventHandler *eh = g_new0 (DeviceEventHandler, 1);

  eh->callback = callback;
  eh->callback_destroyed = callback_destroyed;
  eh->user_data = user_data;

  return eh;
}

static gboolean
device_remove_datum (AtspiDeviceEvent *event, void *user_data)
{
  AtspiDeviceListenerSimpleCB cb = user_data;
  return cb (event);
}

static void
device_event_handler_free (DeviceEventHandler *eh)
{
#if 0
  /* TODO; Test this; it will probably crash with pyatspi for unknown reasons */
  if (eh->callback_destroyed)
  {
    gpointer rea_callback = (eh->callback == device_remove_datum ?
                            eh->user_data : eh->callback);
    (*eh->callback_destroyed) (real_callback);
  }
#endif
  g_free (eh);
}

static GList *
event_list_remove_by_cb (GList *list, AtspiDeviceListenerCB callback)
{
  GList *l, *next;

  for (l = list; l; l = next)
    {
      DeviceEventHandler *eh = l->data;
      next = l->next;

      if (eh->callback == callback)
        {
          list = g_list_delete_link (list, l);
          device_event_handler_free (eh);
        }
    }

  return list;
}

/*
 * Standard event dispatcher
 */

static guint listener_id = 0;
static GList *device_listeners = NULL;

static gboolean
id_is_free (guint id)
{
  GList *l;

  for (l = device_listeners; l; l = g_list_next (l))
    {
      AtspiDeviceListener *listener = l->data;
      if (listener->id == id)
        return FALSE;
    }
  return TRUE;
}

static AtspiDeviceEvent *
atspi_device_event_copy (const AtspiDeviceEvent *src)
{
  AtspiDeviceEvent *dst = g_new0 (AtspiDeviceEvent, 1);
  dst->type = src->type;
  dst->id = src->id;
  dst->hw_code = src->hw_code;
  dst->modifiers = src->modifiers;
  dst->timestamp = src->timestamp;
  if (src->event_string)
    dst->event_string = g_strdup (src->event_string);
  dst->is_text = src->is_text;
  return dst;
}

void
atspi_device_event_free (AtspiDeviceEvent *event)
{
  if (event->event_string)
    g_free (event->event_string);
  g_free (event);
}

/*
 * Device event handler
 */
static gboolean
atspi_device_event_dispatch (AtspiDeviceListener *listener,
                             const AtspiDeviceEvent *event)
{
  GList *l;
  gboolean handled = FALSE;

  /* FIXME: re-enterancy hazard on this list */
  for (l = listener->callbacks; l; l = l->next)
    {
      DeviceEventHandler *eh = l->data;

      if ((handled = eh->callback (atspi_device_event_copy (event), eh->user_data)))
        {
          break;
        }
    }

  return handled;
}

static void
atspi_device_listener_init (AtspiDeviceListener *listener)
{

  do
    {
      listener->id = listener_id++;
    }
  while (!id_is_free (listener->id));
  device_listeners = g_list_append (device_listeners, listener);
}

static void
atspi_device_listener_finalize (GObject *object)
{
  AtspiDeviceListener *listener = (AtspiDeviceListener *) object;
  GList *l;

  device_listeners = g_list_remove (device_listeners, listener);

  for (l = listener->callbacks; l; l = l->next)
    {
      device_event_handler_free (l->data);
    }

  g_list_free (listener->callbacks);

  device_listener_parent_class->finalize (object);
}

static void
atspi_device_listener_class_init (AtspiDeviceListenerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  device_listener_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = atspi_device_listener_finalize;

  klass->device_event = atspi_device_event_dispatch;
}

G_DEFINE_TYPE (AtspiDeviceListener, atspi_device_listener, G_TYPE_OBJECT)

/**
 * atspi_device_listener_new:
 * @callback: (scope notified) (closure user_data) (destroy callback_destroyed) (nullable): an
 *   #AtspiDeviceListenerCB callback function, or NULL.
 * @user_data: a pointer to data which will be passed to the
 *   callback when invoked.
 * @callback_destroyed: A #GDestroyNotify called when the listener is freed
 *   and data associated with the callback should be freed. It can be NULL.
 *
 * Creates a new #AtspiDeviceListener with a specified callback function.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDeviceListener.
 *
 **/
AtspiDeviceListener *
atspi_device_listener_new (AtspiDeviceListenerCB callback,
                           void *user_data,
                           GDestroyNotify callback_destroyed)
{
  AtspiDeviceListener *listener = g_object_new (atspi_device_listener_get_type (), NULL);

  if (callback)
    atspi_device_listener_add_callback (listener, callback, callback_destroyed,
                                        user_data);
  return listener;
}

/**
 * atspi_device_listener_new_simple: (skip)
 * @callback: (scope notified): an #AtspiDeviceListenerCB callback function,
 *            or NULL.
 * @callback_destroyed: A #GDestroyNotify called when the listener is freed
 * and data associated with the callback should be freed.  It can be NULL.
 *
 * Creates a new #AtspiDeviceListener with a specified callback function.
 * This method is similar to #atspi_device_listener_new, but callback
 * takes no user data.
 *
 * Returns: a pointer to a newly-created #AtspiDeviceListener.
 *
 **/
AtspiDeviceListener *
atspi_device_listener_new_simple (AtspiDeviceListenerSimpleCB callback,
                                  GDestroyNotify callback_destroyed)
{
  return atspi_device_listener_new (device_remove_datum, callback, callback_destroyed);
}

/**
 * atspi_device_listener_add_callback:
 * @listener: the #AtspiDeviceListener instance to modify.
 * @callback: (scope notified) (closure user_data) (destroy callback_destroyed): an
 *   #AtspiDeviceListenerCB function pointer.
 * @callback_destroyed: A #GDestroyNotify called when the listener is freed
 *   and data associated with the callback should be freed. It can be NULL.
 * @user_data: a pointer to data which will be passed to the
 *   callback when invoked.
 *
 * Adds an in-process callback function to an existing #AtspiDeviceListener.
 *
 **/
void
atspi_device_listener_add_callback (AtspiDeviceListener *listener,
                                    AtspiDeviceListenerCB callback,
                                    GDestroyNotify callback_destroyed,
                                    void *user_data)
{
  g_return_if_fail (ATSPI_IS_DEVICE_LISTENER (listener));
  DeviceEventHandler *new_handler;

  new_handler = device_event_handler_new (callback,
                                          callback_destroyed, user_data);

  listener->callbacks = g_list_prepend (listener->callbacks, new_handler);
}

/**
 * atspi_device_listener_remove_callback:
 * @listener: the #AtspiDeviceListener instance to modify.
 * @callback: (scope call): an #AtspiDeviceListenerCB function pointer.
 *
 * Removes an in-process callback function from an existing
 * #AtspiDeviceListener.
 *
 **/
void
atspi_device_listener_remove_callback (AtspiDeviceListener *listener,
                                       AtspiDeviceListenerCB callback)
{
  g_return_if_fail (ATSPI_IS_DEVICE_LISTENER (listener));

  listener->callbacks = event_list_remove_by_cb (listener->callbacks, (void *) callback);
}

static void
read_device_event_from_iter (DBusMessageIter *iter, AtspiDeviceEvent *event)
{
  dbus_uint32_t type;
  dbus_int32_t id;
  dbus_int32_t hw_code;
  dbus_int32_t modifiers;
  dbus_int32_t timestamp;
  dbus_bool_t is_text;
  DBusMessageIter iter_struct;

  dbus_message_iter_recurse (iter, &iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &type);
  event->type = type;
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &id);
  event->id = id;
  dbus_message_iter_next (&iter_struct);

  /* TODO: Remove cast from next two on ABI break */
  dbus_message_iter_get_basic (&iter_struct, &hw_code);
  event->hw_code = (gushort) hw_code;
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &modifiers);
  event->modifiers = (gushort) modifiers;
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &timestamp);
  event->timestamp = timestamp;
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &event->event_string);
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &is_text);
  event->is_text = is_text;
}

DBusHandlerResult
_atspi_dbus_handle_DeviceEvent (DBusConnection *bus, DBusMessage *message)
{
  const char *path = dbus_message_get_path (message);
  int id;
  AtspiDeviceEvent event;
  AtspiDeviceListener *listener;
  DBusMessageIter iter;
  AtspiDeviceListenerClass *klass;
  dbus_bool_t retval = FALSE;
  GList *l;
  DBusMessage *reply;

  if (strcmp (dbus_message_get_signature (message), "(uiuuisb)") != 0)
    {
      g_warning ("AT-SPI: Unknown signature for an event");
      goto done;
    }

  if (sscanf (path, "/org/a11y/atspi/listeners/%d", &id) != 1)
    {
      g_warning ("AT-SPI: Bad listener path: %s\n", path);
      goto done;
    }

  for (l = device_listeners; l; l = g_list_next (l))
    {
      listener = l->data;
      if (listener->id == id)
        break;
    }

  if (!l)
    {
      goto done;
    }
  dbus_message_iter_init (message, &iter);
  read_device_event_from_iter (&iter, &event);
  klass = ATSPI_DEVICE_LISTENER_GET_CLASS (listener);
  if (klass->device_event)
    {
      retval = (*klass->device_event) (listener, &event);
      if (retval != 0 && retval != 1)
        {
          g_warning ("AT-SPI: device event handler returned %d; should be 0 or 1", retval);
          retval = 0;
        }
    }
done:
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &retval, DBUS_TYPE_INVALID);
      dbus_connection_send (_atspi_bus (), reply, NULL);
      dbus_message_unref (reply);
    }
  return DBUS_HANDLER_RESULT_HANDLED;
}

gchar *
_atspi_device_listener_get_path (AtspiDeviceListener *listener)
{
  return g_strdup_printf ("/org/a11y/atspi/listeners/%d", listener->id);
}

G_DEFINE_BOXED_TYPE (AtspiDeviceEvent,
                     atspi_device_event,
                     atspi_device_event_copy,
                     atspi_device_event_free)
