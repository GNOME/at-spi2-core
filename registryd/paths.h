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

#ifndef SPI_PATHS_H_
#define SPI_PATHS_H_

#define SPI_DBUS_NAME_REGISTRY                   "org.freedesktop.atspi.Registry"
#define SPI_DBUS_PATH_REGISTRY                   "/org/freedesktop/atspi/registry"
#define SPI_DBUS_INTERFACE_REGISTRY              "org.freedesktop.atspi.Registry"

#define SPI_DBUS_PATH_DESKTOP                    "/org/freedesktop/atspi/accessible/desktop"
#define SPI_DBUS_INTERFACE_DESKTOP               "org.freedesktop.atspi.Desktop"

#define SPI_DBUS_PATH_DEC                        "/org/freedesktop/atspi/registry/deviceeventcontroller"
#define SPI_DBUS_PATH_NULL                       "/org/freedesktop/accessible/null"

#define SPI_DBUS_INTERFACE_DEC                   "org.freedesktop.atspi.DeviceEventController"
#define SPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER "org.freedesktop.atspi.DeviceEventListener"

#define SPI_DBUS_INTERFACE_TREE                  "org.freedesktop.atspi.Tree"
#define SPI_DBUS_INTERFACE_ACCESSIBLE            "org.freedesktop.atspi.Accessible"
#define SPI_DBUS_INTERFACE_ACTION                "org.freedesktop.atspi.Action"
#define SPI_DBUS_INTERFACE_APPLICATION           "org.freedesktop.atspi.Application"
#define SPI_DBUS_INTERFACE_COLLECTION            "org.freedesktop.atspi.Collection"
#define SPI_DBUS_INTERFACE_COMPONENT             "org.freedesktop.atspi.Component"
#define SPI_DBUS_INTERFACE_DOCUMENT              "org.freedesktop.atspi.Document"
#define SPI_DBUS_INTERFACE_EDITABLE_TEXT         "org.freedesktop.atspi.EditableText"
#define SPI_DBUS_INTERFACE_EVENT_KEYBOARD        "org.freedesktop.atspi.Event.Keyboard"
#define SPI_DBUS_INTERFACE_EVENT_MOUSE           "org.freedesktop.atspi.Event.Mouse"
#define SPI_DBUS_INTERFACE_HYPERLINK             "org.freedesktop.atspi.Hyperlink"
#define SPI_DBUS_INTERFACE_HYPERTEXT             "org.freedesktop.atspi.Hypertext"
#define SPI_DBUS_INTERFACE_IMAGE                 "org.freedesktop.atspi.Image"
#define SPI_DBUS_INTERFACE_SELECTION             "org.freedesktop.atspi.Selection"
#define SPI_DBUS_INTERFACE_TABLE                 "org.freedesktop.atspi.Table"
#define SPI_DBUS_INTERFACE_TEXT                  "org.freedesktop.atspi.Text"
#define SPI_DBUS_INTERFACE_VALUE                 "org.freedesktop.atspi.Value"

#endif /* SPI_PATHS_H_ */
