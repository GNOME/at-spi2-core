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

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * Structure used to encapsulate event information
 */
typedef struct {
  const char  *type;
  Accessible  *source;
  long         detail1;
  long         detail2;
} AccessibleEvent;
  
/**
 *AccessibleDeviceEventType:
 *@SPI_KEY_PRESSED:
 *@SPI_KEY_RELEASED:
 *@SPI_BUTTON_PRESSED:
 *@SPI_BUTTON_RELEASED:
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
typedef void       (*AccessibleEventListenerCB)     (const AccessibleEvent     *event,
						     void                      *user_data);
typedef SPIBoolean (*AccessibleKeystrokeListenerCB) (const AccessibleKeystroke *stroke,
						     void                      *user_data);
typedef SPIBoolean (*AccessibleDeviceListenerCB)    (const AccessibleDeviceEvent *stroke,
						     void                      *user_data);

#ifdef  __cplusplus
}
#endif

#endif
