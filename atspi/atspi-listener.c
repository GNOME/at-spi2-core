/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian Inc.
 * Copyright 2002 Sun Microsystems, Inc.
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

#include "atspi-private.h"

typedef struct
{
  union
    {
      AtspiEventListenerCB     event;
      AtspiDeviceListenerCB    device_event;
      gpointer                      method;
    } cb;
  gpointer user_data;
} EventHandler;

GObjectClass *event_parent_class;
GObjectClass *device_parent_class;

static guint32 _e_id = 0;

/*
 * Misc. helpers.
 */

static EventHandler *
atspi_event_handler_new (gpointer method, gpointer user_data)
{
  EventHandler *eh = g_new0 (EventHandler, 1);

  eh->cb.method = method;
  eh->user_data = user_data;

  return eh;
}

static GList *
event_list_remove_by_cb (GList *list, gpointer callback)
{
  GList *l, *next;
	
  for (l = list; l; l = next)
    {
      EventHandler *eh = l->data;
      next = l->next;

      if (eh->cb.method == callback)
      {
        list = g_list_delete_link (list, l);
	g_free (eh);
      }
    }

  return list;
}

/*
 * Standard event dispatcher
 */

G_DEFINE_TYPE (AtspiEventListener, atspi_event_listener,
			  G_TYPE_OBJECT)

static void
atspi_event_dispatch (AtspiEventListener    *listener,
	    const AtspiEvent *event)
{
  GList *l;
  
  /* FIXME: re-entrancy hazard on this list */
  for (l = listener->callbacks; l; l = l->next)
    {
      EventHandler *eh = l->data;
      /* cast hides our private stuff from client handlers */
      eh->cb.event (event, eh->user_data);
    }
}

static guint listener_id = 0;
static GList *device_listeners = NULL;

static gboolean
id_is_free (guint id)
{
  GList *l;

  for (l = device_listeners; l; l = g_list_next (l))
  {
    AtspiDeviceListener *listener = l->data;
    if (listener->id == id) return FALSE;
  }
  return TRUE;
}

static void
remove_listener (GObject *obj, gpointer data)
{
  device_listeners = g_list_remove (device_listeners, obj);
}

static void
atspi_event_listener_init (AtspiEventListener *listener)
{
}

static void
atspi_event_listener_finalize (GObject *object)
{
  AtspiEventListener *listener = (AtspiEventListener *) object;
  GList *l;
  
  for (l = listener->callbacks; l; l = l->next)
    {
      g_free (l->data);
    }
  
  g_list_free (listener->callbacks);

  event_parent_class->finalize (object);
}

static void
atspi_event_listener_class_init (AtspiEventListenerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  event_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = atspi_event_listener_finalize;

  klass->event = atspi_event_dispatch;
}

AtspiEventListener *
atspi_event_listener_new (void)
{
  AtspiEventListener *listener;

  listener = g_object_new (atspi_event_listener_get_type (), NULL);

  return listener;
}

void
atspi_event_listener_add_cb (AtspiEventListener  *listener,
			    AtspiEventListenerCB callback,
			    void                     *user_data)
{
  g_return_if_fail (ATSPI_IS_EVENT_LISTENER (listener));

  listener->callbacks = g_list_prepend (listener->callbacks,
					atspi_event_handler_new ((void *) callback, user_data));
}

void
atspi_event_listener_remove_cb (AtspiEventListener  *listener,
			       AtspiEventListenerCB callback)
{
  g_return_if_fail (ATSPI_IS_EVENT_LISTENER (listener));

  listener->callbacks = event_list_remove_by_cb (listener->callbacks, (void *) callback);
}

/* 
 * Device event handler
 */
static gboolean
atspi_device_event_dispatch (AtspiDeviceListener               *listener,
		   const AtspiDeviceEvent *event)
{
  GList *l;
  AtspiDeviceEvent anevent;
  gboolean handled = FALSE;

  /* FIXME: re-enterancy hazard on this list */
  for (l = listener->callbacks; l; l = l->next)
    {
      EventHandler *eh = l->data;

      if ((handled = eh->cb.device_event (&anevent, eh->user_data)))
        {
	  break;
	}
    }

  return handled;
}

static void
atspi_device_listener_init (AtspiDeviceListener *listener)
{
  GList *new_list;

  do
  {
    listener->id = listener_id++;
  } while (!id_is_free (listener->id));
  new_list = g_list_append (device_listeners, listener);
  if (new_list) device_listeners = new_list;
}

static void
atspi_device_listener_finalize (GObject *object)
{
  AtspiDeviceListener *listener = (AtspiDeviceListener *) object;
  GList *l;
  
  for (l = listener->callbacks; l; l = l->next)
    {
      g_free (l->data);
    }
  
  g_list_free (listener->callbacks);

  device_parent_class->finalize (object);
}

static void
atspi_device_listener_class_init (AtspiDeviceListenerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  device_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = atspi_device_listener_finalize;

  klass->device_event = atspi_device_event_dispatch;
}

G_DEFINE_TYPE (AtspiDeviceListener, atspi_device_listener,
			  G_TYPE_OBJECT)

AtspiDeviceListener *
atspi_device_listener_new (void)
{
  AtspiDeviceListener *listener = g_object_new (atspi_device_listener_get_type (), NULL);

  return listener;
}

void
atspi_device_listener_add_cb (AtspiDeviceListener  *listener,
			     AtspiDeviceListenerCB callback,
			     void                      *user_data)
{
  g_return_if_fail (ATSPI_IS_DEVICE_LISTENER (listener));

  listener->callbacks = g_list_prepend (listener->callbacks,
					atspi_event_handler_new ((void *)callback, user_data));
}

void
atspi_device_listener_remove_cb (AtspiDeviceListener  *listener,
				AtspiDeviceListenerCB callback)
{
  g_return_if_fail (ATSPI_IS_DEVICE_LISTENER (listener));

  listener->callbacks = event_list_remove_by_cb (listener->callbacks, (void *) callback);
}

static const char *device_event_type = "(uinnisb)";

static void
read_device_event_from_iter (DBusMessageIter *iter, AtspiDeviceEvent *event)
{
  dbus_uint32_t type;
  dbus_int32_t id;
  dbus_int16_t hw_code;
  dbus_int16_t modifiers;
  dbus_int32_t timestamp;
  char *event_string;
  dbus_bool_t is_text;
  DBusMessageIter iter_struct;

  dbus_message_iter_recurse (iter, &iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &type);
  event->type = type;
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &id);
  event->id = id;
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &hw_code);
  event->hw_code = hw_code;
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &modifiers);
  event->modifiers = modifiers;
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &timestamp);
  event->timestamp = timestamp;
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &event->event_string);
  dbus_message_iter_next (&iter_struct);

  dbus_message_iter_get_basic (&iter_struct, &is_text);
  event->is_text = is_text;
}

/*
 * atspi_dbus_handle_DeviceEvent: (skip)
 */
DBusHandlerResult
atspi_dbus_handle_DeviceEvent (DBusConnection *bus, DBusMessage *message, void *data)
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
  void *p = &event;

  if (strcmp (dbus_message_get_signature (message), "(uinnisb)") != 0)
  {
    g_warning ("Atspi: Unknown signature for an event");
    goto done;
  }

  if (sscanf (path, "/org/a11y/atspi/listeners/%d", &id) != 1)
  {
    g_warning ("Atspi: Bad listener path: %s\n", path);
    goto done;
  }

  for (l = device_listeners; l; l = g_list_next (l))
  {
    listener = l->data;
    if (listener->id == id) break;
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
  }
done:
  reply = dbus_message_new_method_return (message);
  if (reply)
  {
    dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &retval, DBUS_TYPE_INVALID);
    dbus_connection_send (_atspi_bus(), reply, NULL);
    dbus_message_unref (reply);
  }
  return DBUS_HANDLER_RESULT_HANDLED;
}

gchar *
_atspi_device_listener_get_path (AtspiDeviceListener *listener)
{
  return g_strdup_printf ("/org/a11y/atspi/listeners/%d", listener->id);
}
