/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#ifndef _SPI_LISTENER_H_
#define _SPI_LISTENER_H_

#include <cspi/spi-impl.h>
#include <glib-object.h>
#include "spi-common/spi-types.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef enum
{
  EVENT_DATA_STRING,
  EVENT_DATA_OBJECT,
  EVENT_DATA_RECT
} EVENT_DATA_TYPE;

/**
 * AccessibleEvent:
 * @type: a string representing the type of the event, with increasing specificity
 * from left to right, delimited by colons.  Leftmost token represents event source type,
 * next token represents event category, and next token represents the subcategory
 * of the event, for instance the specific property, state, or attribute being notified.
 * @source: The #Accessible from which the event originated.
 * @detail1: an event-type-specific integer value.
 * @detail2: an event-type-specific integer value.
 *
 * A structure used to encapsulate event information.
 **/
typedef struct {
  const char  *type;
  Accessible  *source;
  long         detail1;
  long         detail2;
  EVENT_DATA_TYPE v_type;
  union
  {
    char *text;
    Accessible *accessible;
    SPIRect rect;
  } v;
} AccessibleEvent;

/**
 *AccessibleDeviceEventType:
 *@SPI_KEY_PRESSED: A device key has been pressed.
 *@SPI_KEY_RELEASED: A device key has been released.
 *@SPI_BUTTON_PRESSED: A device button or switch (other than a keyboard key) 
 * has been pressed.
 *@SPI_BUTTON_RELEASED: A device button or switch has been released.
 *
 *The type of an AccessibleDeviceEvent.
 **/
typedef enum {
  SPI_KEY_PRESSED  = 1<<0,
  SPI_KEY_RELEASED = 1<<1,
  SPI_BUTTON_PRESSED = 1<<2,
  SPI_BUTTON_RELEASED = 1<<3
} AccessibleDeviceEventType;

/**
 *AccessibleKeyEventType:
 *
 *This is a synonym for AccessibleDeviceEventType
 **/
typedef AccessibleDeviceEventType AccessibleKeyEventType;

/**
 * AccessibleDeviceEvent:
 * @keyID: Symbolic representation for the key or switch generating the event, e.g. keysym or button number.
 * @keystring: A symbolic name for the key or switch, or, if is_text is true, a string approximating the
 * inserted text characters which would result from this event, if a text entry field has keyboard focus.
 * @timestamp: A time in ms when this event occurred, relative to some unspecified starting point.  
 * Timestamp values should therefore be used to compare events but should not be tested against a 
 * fixed time.
 * @type: The #AccessibleDeviceEventType identifying the specific type of event.
 * @is_text: A boolean value indicating whether the event represents 'printable' text (i.e. whether it
 * changes the current insertion buffer of a focussed text entry component or not).  Whitespace
 * is considered "printable" in this context, since it typically inserts characters into the buffer.
 *
 * A structure encapsulating information relevant to a device event notification.
 **/
typedef struct {
  long                   keyID;
  short                  keycode;
  char *                 keystring;
  long                   timestamp;
  AccessibleDeviceEventType type;
  unsigned short         modifiers;
  SPIBoolean             is_text;	
} AccessibleDeviceEvent;

typedef AccessibleDeviceEvent AccessibleKeystroke;

/*
 * Function prototype typedefs for Event Listener Callbacks.
 * (see libspi/accessibleeventlistener.h for definition of SpiVoidEventListenerCB).
 *
 * usage: signatures should be
 * void (*AccessibleEventListenerCB) (AccessibleEvent *event);
 *
 * SPIBoolean (*AccessibleKeystrokeListenerCB) (AccessibleKeystrokeEvent *Event);
 * Note that AccessibleKeystrokeListeners may consume the event received
 * if one of their callbacks returns TRUE (see SPI_registerAccessibleKeystrokeListener)
 */

/**
 * AccessibleEventListenerCB:
 * @event: The event for which notification is sent.
 * @user_data: User data which is passed to the callback each time a notification takes place.
 *
 * A function prototype for callbacks via which clients are notified of AT-SPI events.
 * 
 **/
typedef void       (*AccessibleEventListenerCB)     (const AccessibleEvent     *event,
						     void                      *user_data);
/**
 * AccessibleKeystrokeListenerCB:
 * @stroke: the #AccessibleKeystroke event for which notification is taking place.
 * @user_data: data passed to the callback each time it is notified, according to the data
 * which was passed in when the listener/callback was registered.
 *
 * A function prototype for a callback function called when a key event notification takes place.
 *
 * Returns: %TRUE if the client wishes to consume the event and prevent its
 * dispatch to other listeners or the currently focussed application, if
 * the relevant callback was registered with the #SPI_KEYLISTENER_CANCONSUME flag.
 **/
typedef SPIBoolean (*AccessibleKeystrokeListenerCB) (const AccessibleKeystroke *stroke,
						     void                      *user_data);
/**
 * AccessibleDeviceListenerCB:
 * @stroke: The #AccessibleDeviceEvent for which notification is being received.
 * @user_data: Data which is passed to the client each time this callback is notified.
 *
 * A callback function prototype via which clients receive device event notifications.
 *
 * Returns: %TRUE if the client wishes to consume/preempt the event, preventing it from being
 * relayed to the currently focussed application, %FALSE if the event delivery should proceed as normal.
 **/
typedef SPIBoolean (*AccessibleDeviceListenerCB)    (const AccessibleDeviceEvent *stroke,
						     void                      *user_data);

#define CSPI_EVENT_LISTENER_TYPE        (cspi_event_listener_get_type ())
#define CSPI_EVENT_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), CSPI_EVENT_LISTENER_TYPE, CSpiEventListener))
#define CSPI_EVENT_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), CSPI_EVENT_LISTENER_TYPE, CSpiEventListenerClass))
#define CSPI_IS_EVENT_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), CSPI_EVENT_LISTENER_TYPE))
#define CSPI_IS_EVENT_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), CSPI_EVENT_LISTENER_TYPE))
#define CSPI_EVENT_LISTENER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CSPI_EVENT_LISTENER_TYPE, CSpiEventListenerClass))

typedef struct {
	GObject parent;
	GList           *callbacks;
} CSpiEventListener;

typedef struct {
  GObjectClass parent_class;

  /* signals */
  void (*event) (CSpiEventListener    *listener,
		 AccessibleEvent *e);
} CSpiEventListenerClass;

GType cspi_event_listener_get_type (void);

#define CSPI_DEVICE_LISTENER_TYPE        (cspi_device_listener_get_type ())
#define CSPI_DEVICE_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), CSPI_DEVICE_LISTENER_TYPE, CSpiDeviceListener))
#define CSPI_DEVICE_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), CSPI_DEVICE_LISTENER_TYPE, CSpiDeviceListenerClass))
#define CSPI_IS_DEVICE_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), CSPI_DEVICE_LISTENER_TYPE))
#define CSPI_IS_DEVICE_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), CSPI_DEVICE_LISTENER_TYPE))
#define CSPI_DEVICE_LISTENER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CSPI_DEVICE_LISTENER_TYPE, CSpiDeviceListenerClass))

typedef struct {
	GObject parent;
	guint id;
	GList            *callbacks;
} CSpiDeviceListener;

typedef struct {
	GObjectClass parent_class;
	gboolean (*device_event) (CSpiDeviceListener *listener, const Accessibility_DeviceEvent *key);
} CSpiDeviceListenerClass;

GType cspi_device_listener_get_type (void);
gpointer cspi_device_listener_new (void);
void cspi_device_listener_add_cb (AccessibleDeviceListener  *al,
                                  AccessibleDeviceListenerCB callback,
                                  void                      *user_data);
void cspi_device_listener_remove_cb (AccessibleDeviceListener  *al,
                                     AccessibleDeviceListenerCB callback);
void cspi_device_listener_unref (AccessibleDeviceListener *listener);
#ifdef  __cplusplus
}
#endif

#endif
