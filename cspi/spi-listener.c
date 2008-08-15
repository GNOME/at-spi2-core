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

#include "spi-private.h"
#include <stdio.h>

typedef struct
{
  union
    {
      AccessibleEventListenerCB     event;
      AccessibleDeviceListenerCB    device_event;
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
cspi_event_handler_new (gpointer method, gpointer user_data)
{
  EventHandler *eh = g_new0 (EventHandler, 1);

  eh->cb.method = method;
  eh->user_data = user_data;

  return eh;
}

static void
cspi_event_handler_free (EventHandler *handler)
{
  g_free (handler);
}

static GList *
cspi_event_list_remove_by_cb (GList *list, gpointer callback)
{
  GList *l, *next;
	
  for (l = list; l; l = next)
    {
      EventHandler *eh = l->data;
      next = l->next;

      if (eh->cb.method == callback)
      {
        list = g_list_delete_link (list, l);
	cspi_event_handler_free (eh);
      }
    }

  return list;
}

/*
 * Standard event dispatcher
 */

G_DEFINE_TYPE (CSpiEventListener, cspi_event_listener,
			  G_TYPE_OBJECT)

static void
cspi_event (CSpiEventListener    *listener,
	    AccessibleEvent *event)
{
  GList *l;
  CSpiEventListener *clistener = (CSpiEventListener *) listener;
  InternalEvent     *ievent;
  AccessibleEvent   *aevent;
  
  ievent = g_new0(InternalEvent, 1);
  ievent->event.type    = g_strdup (event->type);
  ievent->event.source  = event->source;
  ievent->event.detail1 = event->detail1;
  ievent->event.detail2 = event->detail2;
  ievent->event.v_type = event->v_type;
  if (event->v_type == EVENT_DATA_STRING)
  {
    ievent->event.v.text = g_strdup (event->v.text);
  }
  else memcpy (&ievent->event.v, &event->v, sizeof(event->v));
  ievent->id            = _e_id++;
  ievent->magic         = SPI_INTERNAL_EVENT_MAGIC;
  ievent->ref_count     = 0;
  aevent = (AccessibleEvent *)ievent;
  Accessible_ref (aevent->source);
  AccessibleEvent_ref (aevent);

  /* FIXME: re-enterancy hazard on this list */
  for (l = clistener->callbacks; l; l = l->next)
    {
      EventHandler *eh = l->data;
      /* cast hides our private stuff from client handlers */
      eh->cb.event (aevent, eh->user_data);
    }

  AccessibleEvent_unref (aevent);
}

static guint listener_id = 0;
static GList *device_listeners = NULL;

static gboolean
id_is_free (guint id)
{
  GList *l;

  for (l = device_listeners; l; l = g_list_next (l))
  {
    CSpiDeviceListener *listener = l->data;
    if (listener->id == id) return FALSE;
  }
  return TRUE;
}

static void remove_listener (GObject *obj, gpointer data)
{
  device_listeners = g_list_remove (device_listeners, obj);
}

static void
cspi_event_listener_instance_init (CSpiEventListener *listener)
{
}

static void
cspi_event_listener_finalize (GObject *object)
{
  CSpiEventListener *listener = (CSpiEventListener *) object;
  GList *l;
  
  for (l = listener->callbacks; l; l = l->next)
    {
      cspi_event_handler_free (l->data);
    }
  
  g_list_free (listener->callbacks);

  event_parent_class->finalize (object);
}

static void
cspi_event_listener_class_init (CSpiEventListenerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  event_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = cspi_event_listener_finalize;

  klass->event = cspi_event;
}

gpointer
cspi_event_listener_new (void)
{
  CSpiEventListener *listener;

  listener = g_object_new (cspi_event_listener_get_type (), NULL);

  return listener;
}

static void
cspi_event_listener_init (CSpiEventListener *listener)
{
}

void
cspi_event_listener_add_cb (AccessibleEventListener  *al,
			    AccessibleEventListenerCB callback,
			    void                     *user_data)
{
  CSpiEventListener *listener = al;

  g_return_if_fail (CSPI_IS_EVENT_LISTENER (listener));

  listener->callbacks = g_list_prepend (listener->callbacks,
					cspi_event_handler_new ((void *) callback, user_data));
}

void
cspi_event_listener_remove_cb (AccessibleEventListener  *al,
			       AccessibleEventListenerCB callback)
{
  CSpiEventListener *listener = al;

  g_return_if_fail (CSPI_IS_EVENT_LISTENER (listener));

  listener->callbacks = cspi_event_list_remove_by_cb (listener->callbacks, (void *) callback);
}

/* 
 * Device event handler
 */
static gboolean
cspi_device_event (CSpiDeviceListener               *listener,
		   const Accessibility_DeviceEvent *event)
{
  GList *l;
  CSpiDeviceListener *clistener = (CSpiDeviceListener *) listener;
  AccessibleDeviceEvent anevent;
  gboolean handled = FALSE;

  switch (event->type)
    {
      case Accessibility_KEY_PRESSED_EVENT:
	anevent.type = SPI_KEY_PRESSED;
	break;
      case Accessibility_KEY_RELEASED_EVENT:
	anevent.type = SPI_KEY_RELEASED;
	break;
      case Accessibility_BUTTON_PRESSED_EVENT:
	anevent.type = SPI_BUTTON_PRESSED;
	break;
      case Accessibility_BUTTON_RELEASED_EVENT:
	anevent.type = SPI_BUTTON_RELEASED;
	break;
      default:
	anevent.type = 0;
	break;
    }
  anevent.keyID     = event->id;
  anevent.keycode   = event->hw_code;
  anevent.timestamp = event->timestamp;
  anevent.keystring = g_strdup (event->event_string);
  anevent.modifiers = event->modifiers;
  anevent.is_text = event->is_text;

  /* FIXME: re-enterancy hazard on this list */
  for (l = clistener->callbacks; l; l = l->next)
    {
      EventHandler *eh = l->data;

      if ((handled = eh->cb.device_event (&anevent, eh->user_data)))
        {
	  break;
	}
    }
  g_free (anevent.keystring);

  return handled;
}

static void
cspi_device_listener_init (CSpiDeviceListener *listener)
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
cspi_device_listener_finalize (GObject *object)
{
  CSpiDeviceListener *listener = (CSpiDeviceListener *) object;
  GList *l;
  
  for (l = listener->callbacks; l; l = l->next)
    {
      cspi_event_handler_free (l->data);
    }
  
  g_list_free (listener->callbacks);

  device_parent_class->finalize (object);
}

static void
cspi_device_listener_class_init (CSpiDeviceListenerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  device_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = cspi_device_listener_finalize;

  klass->device_event = cspi_device_event;
}

G_DEFINE_TYPE (CSpiDeviceListener, cspi_device_listener,
			  G_TYPE_OBJECT)

gpointer
cspi_device_listener_new (void)
{
  CSpiEventListener *listener = g_object_new (cspi_device_listener_get_type (), NULL);

  return listener;
}

void
cspi_device_listener_add_cb (AccessibleDeviceListener  *al,
			     AccessibleDeviceListenerCB callback,
			     void                      *user_data)
{
  CSpiDeviceListener *listener = al;

  g_return_if_fail (CSPI_IS_DEVICE_LISTENER (listener));

  listener->callbacks = g_list_prepend (listener->callbacks,
					cspi_event_handler_new ((void *)callback, user_data));
}

void
cspi_device_listener_remove_cb (AccessibleDeviceListener  *al,
				AccessibleDeviceListenerCB callback)
{
  CSpiDeviceListener *listener = al;

  g_return_if_fail (CSPI_IS_DEVICE_LISTENER (listener));

  listener->callbacks = cspi_event_list_remove_by_cb (listener->callbacks, (void *) callback);
}

void
cspi_event_listener_unref (AccessibleEventListener *listener)
{
  g_object_unref (G_OBJECT (listener));
}

void
cspi_device_listener_unref (AccessibleDeviceListener *listener)
{
  g_object_unref (G_OBJECT (listener));
}

static const char *deviceEvent_type = "(uinnisb)";

DBusHandlerResult
cspi_dbus_handle_deviceEvent (DBusConnection *bus, DBusMessage *message, void *data)
{
  const char *path = dbus_message_get_path (message);
  int id;
  Accessibility_DeviceEvent event;
    CSpiDeviceListener *listener;
  DBusMessageIter iter;
  CSpiDeviceListenerClass *klass;
  dbus_bool_t retval = FALSE;
  GList *l;
  DBusMessage *reply;
  void *p = &event;

  if (sscanf (path, "/org/freedesktop/atspi/listeners/%d", &id) != 1)
  {
    g_warning ("Bad listener path: %s\n", path);
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
  dbind_any_demarshal (iter, &deviceEvent_type, &p);
  klass = CSPI_DEVICE_LISTENER_GET_CLASS (listener);
  if (klass->device_event)
  {
    retval = (*klass->device_event) (listener, &event);
  }
done:
  reply = dbus_message_new_method_return (message);
  if (reply)
  {
    dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &retval, DBUS_TYPE_INVALID);
    dbus_connection_send (SPI_bus(), reply, NULL);
    dbus_message_unref (reply);
  }
  return DBUS_HANDLER_RESULT_HANDLED;
}
