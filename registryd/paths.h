/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
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

#ifndef SPI_PATHS_H_
#define SPI_PATHS_H_

#define SPI_DBUS_NAME_PREFIX "org.a11y.atspi."
#define SPI_DBUS_PATH_PREFIX "/org/a11y/atspi/"
#define SPI_DBUS_INTERFACE_PREFIX "org.a11y.atspi."

#define SPI_DBUS_PATH_NULL SPI_DBUS_PATH_PREFIX "null"
#define SPI_DBUS_PATH_ROOT SPI_DBUS_PATH_PREFIX "accessible/root"

#define SPI_DBUS_NAME_REGISTRY SPI_DBUS_NAME_PREFIX "Registry"
#define SPI_DBUS_INTERFACE_REGISTRY SPI_DBUS_INTERFACE_PREFIX "Registry"
#define SPI_DBUS_PATH_REGISTRY SPI_DBUS_PATH_PREFIX "registry"

#define SPI_DBUS_PATH_DEC SPI_DBUS_PATH_PREFIX "registry/deviceeventcontroller"
#define SPI_DBUS_INTERFACE_DEC SPI_DBUS_INTERFACE_PREFIX "DeviceEventController"
#define SPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER SPI_DBUS_INTERFACE_PREFIX "DeviceEventListener"

#define SPI_DBUS_PATH_CACHE SPI_DBUS_PATH_PREFIX "cache"
#define SPI_DBUS_INTERFACE_CACHE SPI_DBUS_INTERFACE_PREFIX "Cache"

#define SPI_DBUS_INTERFACE_ACCESSIBLE SPI_DBUS_INTERFACE_PREFIX "Accessible"
#define SPI_DBUS_INTERFACE_APPLICATION SPI_DBUS_INTERFACE_PREFIX "Application"
#define SPI_DBUS_INTERFACE_COMPONENT SPI_DBUS_INTERFACE_PREFIX "Component"
#define SPI_DBUS_INTERFACE_EVENT_KEYBOARD SPI_DBUS_INTERFACE_PREFIX "Keyboard"
#define SPI_DBUS_INTERFACE_EVENT_MOUSE SPI_DBUS_INTERFACE_PREFIX "Event.Mouse"
#define SPI_DBUS_INTERFACE_EVENT_OBJECT SPI_DBUS_INTERFACE_PREFIX "Event.Object"
#define SPI_DBUS_INTERFACE_SOCKET SPI_DBUS_INTERFACE_PREFIX "Socket"

#endif /* SPI_PATHS_H_ */
