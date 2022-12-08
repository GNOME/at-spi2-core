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

#ifndef SPI_DE_MARSHALLER_H_
#define SPI_DE_MARSHALLER_H_

#include <dbus/dbus.h>

#include "de-types.h"

dbus_bool_t spi_dbus_message_iter_get_struct (DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_message_iter_append_struct (DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_marshal_deviceEvent (DBusMessage *message, const Accessibility_DeviceEvent *e);
dbus_bool_t spi_dbus_demarshal_deviceEvent (DBusMessage *message, Accessibility_DeviceEvent *e);

#endif /* SPI_DE_MARSHALLER_H_ */
