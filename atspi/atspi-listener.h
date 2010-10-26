/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 *           
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

#ifndef _ATSPI_LISTENER_H_
#define _ATSPI_LISTENER_H_

#include "atspi-event-types.h"

/*
 * Function prototype typedefs for Event Listener Callbacks.
 *
 * usage: signatures should be
 * void (*AtspiEventListenerCB) (AtspiEvent *event);
 *
 * gboolean (*AtspiKeystrokeListenerCB) (AtspiKeystrokeEvent *Event);
 * Note that AtspiKeystrokeListeners may consume the event received
 * if one of their callbacks returns TRUE (see atspi_register_accessible_keystroke_listener)
 */

/**
 * AtspiEventListenerCB:
 * @event: The event for which notification is sent.
 * @user_data: User data which is passed to the callback each time a notification takes place.
 *
 * A function prototype for callbacks via which clients are notified of AT-SPI events.
 * 
 **/
typedef void       (*AtspiEventListenerCB)     (const AtspiEvent     *event,
						     void                      *user_data);

/**
 * AtspiDeviceListenerCB:
 * @stroke: The #AtspiDeviceEvent for which notification is being received.
 * @user_data: Data which is passed to the client each time this callback is notified.
 *
 * A callback function prototype via which clients receive device event notifications.
 *
 * Returns: %TRUE if the client wishes to consume/preempt the event, preventing it from being
 * relayed to the currently focussed application, %FALSE if the event delivery should proceed as normal.
 **/
typedef gboolean (*AtspiDeviceListenerCB)    (const AtspiDeviceEvent *stroke,
						     void                      *user_data);

#define ATSPI_TYPE_DEVICE_LISTENER                        (atspi_device_listener_get_type ())
#define ATSPI_DEVICE_LISTENER(obj)                        (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATSPI_TYPE_DEVICE_LISTENER, AtspiDeviceListener))
#define ATSPI_DEVICE_LISTENER_CLASS(klass)                (G_TYPE_CHECK_CLASS_CAST ((klass), ATSPI_TYPE_DEVICE_LISTENER, AtspiDeviceListenerClass))
#define ATSPI_IS_DEVICE_LISTENER(obj)                     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATSPI_TYPE_DEVICE_LISTENER))
#define ATSPI_IS_DEVICE_LISTENER_CLASS(klass)             (G_TYPE_CHECK_CLASS_TYPE ((klass), ATSPI_TYPE_DEVICE_LISTENER))
#define ATSPI_DEVICE_LISTENER_GET_CLASS(obj)              (G_TYPE_INSTANCE_GET_CLASS ((obj), ATSPI_TYPE_DEVICE_LISTENER, AtspiDeviceListenerClass))

#define ATSPI_TYPE_EVENT_LISTENER                        (atspi_event_listener_get_type ())
#define ATSPI_EVENT_LISTENER(obj)                        (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATSPI_TYPE_EVENT_LISTENER, AtspiEventListener))
#define ATSPI_EVENT_LISTENER_CLASS(klass)                (G_TYPE_CHECK_CLASS_CAST ((klass), ATSPI_TYPE_EVENT_LISTENER, AtspiEventListenerClass))
#define ATSPI_IS_EVENT_LISTENER(obj)                     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATSPI_TYPE_EVENT_LISTENER))
#define ATSPI_IS_EVENT_LISTENER_CLASS(klass)             (G_TYPE_CHECK_CLASS_TYPE ((klass), ATSPI_TYPE_EVENT_LISTENER))
#define ATSPI_EVENT_LISTENER_GET_CLASS(obj)              (G_TYPE_INSTANCE_GET_CLASS ((obj), ATSPI_TYPE_EVENT_LISTENER, AtspiEventListenerClass))

typedef struct _AtspiEventListener AtspiEventListener;
struct _AtspiEventListener
{
  GObject parent;
  GList *callbacks;
};

typedef struct _AtspiEventListenerClass AtspiEventListenerClass;
struct _AtspiEventListenerClass
{
  GObject parent_class;
  void (*event) (AtspiEventListener *, const AtspiEvent *);
};

typedef struct _AtspiDeviceListener AtspiDeviceListener;
struct _AtspiDeviceListener
{
  GObject parent;
  GList *callbacks;
  guint id;
};

typedef struct _AtspiDeviceListenerClass AtspiDeviceListenerClass;
struct _AtspiDeviceListenerClass
{
  GObject parent_class;
  gboolean (*device_event) (AtspiDeviceListener *, const AtspiDeviceEvent *);
};

AtspiEventListener *atspi_event_listener_new (void);

void atspi_event_listener_add_cb (AtspiEventListener  *listener,
			    AtspiEventListenerCB callback,
			    void                     *user_data);

void atspi_event_listener_remove_cb (AtspiEventListener  *listener,
			       AtspiEventListenerCB callback);

AtspiDeviceListener *
atspi_device_listener_new (void);

void
atspi_device_listener_add_cb (AtspiDeviceListener  *listener,
			     AtspiDeviceListenerCB callback,
			     void                      *user_data);

void
atspi_device_listener_remove_cb (AtspiDeviceListener  *listener,
				AtspiDeviceListenerCB callback);

/* private */
DBusHandlerResult
atspi_dbus_handle_deviceEvent (DBusConnection *bus, DBusMessage *message, void *data);

gchar *
_atspi_device_listener_get_path (AtspiDeviceListener *listener);
#endif	/* _ATSPI_LISTENER_H_ */
