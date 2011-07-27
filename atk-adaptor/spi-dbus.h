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
#include <atspi/atspi.h>

#define DBG(a,b) if(_dbg>=(a))b

extern int _dbg;

#define SPI_DBUS_NAME_REGISTRY "org.a11y.atspi.Registry"
#define SPI_DBUS_PATH_REGISTRY "/org/a11y/atspi/registry"
#define SPI_DBUS_INTERFACE_REGISTRY "org.a11y.atspi.Registry"

#define SPI_DBUS_PATH_NULL "/org/a11y/atspi/null"
#define SPI_DBUS_PATH_ROOT "/org/a11y/atspi/accessible/root"

DBusMessage *spi_dbus_general_error(DBusMessage *message);
DBusMessage *spi_dbus_return_rect(DBusMessage *message, gint ix, gint iy, gint iwidth, gint iheight);

void spi_dbus_emit_valist(DBusConnection *bus, const char *path, const char *interface, const char *name, int first_arg_type, va_list args);
dbus_bool_t spi_dbus_message_iter_get_struct(DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_message_iter_append_struct(DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_marshal_deviceEvent(DBusMessage *message, const AtspiDeviceEvent *e);
dbus_bool_t spi_dbus_demarshal_deviceEvent(DBusMessage *message, AtspiDeviceEvent *e);
dbus_bool_t spi_dbus_get_simple_property (DBusConnection *bus, const char *dest, const char *path, const char *interface, const char *prop, int *type, void *ptr, DBusError *error);
void spi_dbus_emit_signal(DBusConnection *bus, const char *path, const char *klass, const char *major, const char *minor, dbus_int32_t detail1, dbus_int32_t detail2, const char *type, const void *val);
/*
void spi_dbus_add_disconnect_match (DBusConnection *bus, const char *name);
void spi_dbus_remove_disconnect_match (DBusConnection *bus, const char *name);
*/

#endif /* SPI_DBUS_H_ */
