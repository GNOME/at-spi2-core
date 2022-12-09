/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2009  Codethink Ltd
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#ifndef SPI_DE_TYPES_H_
#define SPI_DE_TYPES_H_

#include <dbus/dbus.h>

typedef unsigned long Accessibility_ControllerEventMask;

typedef enum
{
  Accessibility_KEY_PRESSED_EVENT,
  Accessibility_KEY_RELEASED_EVENT,
} Accessibility_EventType;

typedef enum
{
  Accessibility_KEY_PRESSED,
  Accessibility_KEY_RELEASED,
} Accessibility_KeyEventType;

typedef enum
{
  Accessibility_KEY_PRESS,
  Accessibility_KEY_RELEASE,
  Accessibility_KEY_PRESSRELEASE,
  Accessibility_KEY_SYM,
  Accessibility_KEY_STRING,
  Accessibility_KEY_LOCKMODIFIERS,
  Accessibility_KEY_UNLOCKMODIFIERS,
} Accessibility_KeySynthType;

typedef struct
{
  Accessibility_EventType type;
  dbus_uint32_t id;
  dbus_uint32_t hw_code;
  dbus_uint32_t modifiers;
  dbus_uint32_t timestamp;
  char *event_string;
  dbus_bool_t is_text;
} Accessibility_DeviceEvent;

typedef struct
{
  dbus_bool_t synchronous;
  dbus_bool_t preemptive;
  dbus_bool_t global;
} Accessibility_EventListenerMode;

typedef struct
{
  dbus_int32_t keycode;
  dbus_int32_t keysym;
  char *keystring;
  dbus_int32_t unused;
} Accessibility_KeyDefinition;

#endif /* SPI_DE_TYPES_H_ */
