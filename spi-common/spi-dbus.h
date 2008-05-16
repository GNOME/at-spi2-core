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

#define SPI_DBUS_NAME_REGISTRY "org.freedesktop.atspi.registry"
#define SPI_DBUS_PATH_REGISTRY "/org/freedesktop/atspi/registry"
#define SPI_DBUS_INTERFACE_REGISTRY "org.freedesktop.atspi.registry"
#define SPI_DBUS_PATH_DEC "/org/freedesktop/atspi/registry/dec"
#define SPI_DBUS_INTERFACE_DEC "org.freedesktop.atspi.deviceEventController"
#define SPI_DBUS_PATH_DESKTOP "/org/freedesktop/atspi/registry/desktop"
#define SPI_DBUS_PATH_NULL "/"

DBusMessage *spi_dbus_general_error(DBusMessage *message);
DBusMessage *spi_dbus_return_rect(DBusMessage *message, gint ix, gint iy, gint iwidth, gint iheight);

#define SPI_DBUS_RETURN_ERROR(m, e) dbus_message_new_error(m, (e)->name, (e)->message)

void spi_dbus_emit_valist(DBusConnection *bus, const char *path, const char *interface, const char *name, int first_arg_type, va_list args);
dbus_bool_t spi_dbus_message_iter_get_struct(DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_message_iter_append_struct(DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_marshall_deviceEvent(DBusMessage *message, const Accessibility_DeviceEvent *e);
dbus_bool_t spi_dbus_demarshall_deviceEvent(DBusMessage *message, Accessibility_DeviceEvent *e);

#endif /* SPI_DBUS_H_ */
