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

#ifndef SPI_DBUS_H_
#define SPI_DBUS_H_

#include <glib.h>
#include <spi-common/spi-types.h>

#define DBG(a,b) if(_dbg>=(a))b

extern int _dbg;

#define SPI_DBUS_PATH_DEC "/org/freedesktop/atspi/registry/dec"
#define SPI_DBUS_INTERFACE_ACCESSIBLE "org.freedesktop.atspi.Accessible"
#define SPI_DBUS_INTERFACE_ACTION "org.freedesktop.atspi.Action"
#define SPI_DBUS_INTERFACE_APPLICATION "org.freedesktop.atspi.Application"
#define SPI_DBUS_INTERFACE_COLLECTION "org.freedesktop.atspi.Collection"
#define SPI_DBUS_INTERFACE_COMPONENT "org.freedesktop.atspi.Component"
#define SPI_DBUS_INTERFACE_DEC "org.freedesktop.atspi.DeviceEventController"
#define SPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER "org.freedesktop.atspi.DeviceEventListener"
#define SPI_DBUS_INTERFACE_DESKTOP "org.freedesktop.atspi.Desktop"
#define SPI_DBUS_INTERFACE_DOCUMENT "org.freedesktop.atspi.Document"
#define SPI_DBUS_INTERFACE_EDITABLE_TEXT "org.freedesktop.atspi.EditableText"
#define SPI_DBUS_INTERFACE_EVENT_KEYBOARD "org.freedesktop.atspi.Event.Keyboard"
#define SPI_DBUS_INTERFACE_EVENT_MOUSE "org.freedesktop.atspi.Event.Mouse"
#define SPI_DBUS_INTERFACE_HYPERLINK "org.freedesktop.atspi.Hyperlink"
#define SPI_DBUS_INTERFACE_HYPERTEXT "org.freedesktop.atspi.Hypertext"
#define SPI_DBUS_INTERFACE_IMAGE "org.freedesktop.atspi.Image"
#define SPI_DBUS_INTERFACE_REGISTRY "org.freedesktop.atspi.Registry"
#define SPI_DBUS_INTERFACE_SELECTION "org.freedesktop.atspi.Selection"
#define SPI_DBUS_INTERFACE_TABLE "org.freedesktop.atspi.Table"
#define SPI_DBUS_INTERFACE_TEXT "org.freedesktop.atspi.Text"
#define SPI_DBUS_INTERFACE_TREE "org.freedesktop.atspi.Tree"
#define SPI_DBUS_INTERFACE_VALUE "org.freedesktop.atspi.Value"
#define SPI_DBUS_NAME_REGISTRY "org.freedesktop.atspi.Registry"
#define SPI_DBUS_PATH_DESKTOP "/org/freedesktop/atspi/registry/desktop"
#define SPI_DBUS_PATH_NULL "/org/freedesktop/accessible/null"
#define SPI_DBUS_PATH_REGISTRY "/org/freedesktop/atspi/registry"

DBusMessage *spi_dbus_general_error(DBusMessage *message);
DBusMessage *spi_dbus_return_rect(DBusMessage *message, gint ix, gint iy, gint iwidth, gint iheight);

void spi_dbus_emit_valist(DBusConnection *bus, const char *path, const char *interface, const char *name, int first_arg_type, va_list args);
dbus_bool_t spi_dbus_message_iter_get_struct(DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_message_iter_append_struct(DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_marshall_deviceEvent(DBusMessage *message, const Accessibility_DeviceEvent *e);
dbus_bool_t spi_dbus_demarshall_deviceEvent(DBusMessage *message, Accessibility_DeviceEvent *e);
dbus_bool_t spi_dbus_get_simple_property (DBusConnection *bus, const char *dest, const char *path, const char *interface, const char *prop, int *type, void *ptr, DBusError *error);
void spi_dbus_emit_signal(DBusConnection *bus, const char *path, const char *klass, const char *major, const char *minor, dbus_int32_t detail1, dbus_int32_t detail2, const char *type, const void *val);
/*
void spi_dbus_add_disconnect_match (DBusConnection *bus, const char *name);
void spi_dbus_remove_disconnect_match (DBusConnection *bus, const char *name);
*/

#endif /* SPI_DBUS_H_ */
