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

#ifndef SPI_DBUS_H_
#define SPI_DBUS_H_

#include <atspi/atspi.h>
#include <glib.h>

#define DBG(a, b)  \
  if (_dbg >= (a)) \
  b

extern int _dbg;

#define SPI_DBUS_NAME_REGISTRY "org.a11y.atspi.Registry"
#define SPI_DBUS_PATH_REGISTRY "/org/a11y/atspi/registry"
#define SPI_DBUS_INTERFACE_REGISTRY "org.a11y.atspi.Registry"

#define SPI_DBUS_PATH_NULL "/org/a11y/atspi/null"
#define SPI_DBUS_PATH_ROOT "/org/a11y/atspi/accessible/root"

DBusMessage *spi_dbus_general_error (DBusMessage *message);
DBusMessage *spi_dbus_return_rect (DBusMessage *message, gint ix, gint iy, gint iwidth, gint iheight);

dbus_bool_t spi_dbus_message_iter_get_struct (DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_message_iter_append_struct (DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_marshal_deviceEvent (DBusMessage *message, const AtspiDeviceEvent *e);
dbus_bool_t spi_dbus_demarshal_deviceEvent (DBusMessage *message, AtspiDeviceEvent *e);
void spi_dbus_emit_signal (DBusConnection *bus, const char *path, const char *klass, const char *major, const char *minor, dbus_int32_t detail1, dbus_int32_t detail2, const char *type, const void *val);
GObject *spi_dbus_get_object_from_iter (DBusMessageIter *iter);
/*
void spi_dbus_add_disconnect_match (DBusConnection *bus, const char *name);
void spi_dbus_remove_disconnect_match (DBusConnection *bus, const char *name);

*/

#endif /* SPI_DBUS_H_ */
