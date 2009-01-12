/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008, 2009 Codethink Ltd.
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

#ifndef __ACCESSIBLE_MARSHALLER__
#define __ACCESSIBLE_MARSHALLER__

#include <dbus/dbus.h>
#include <atk/atk.h>

DBusMessage *
spi_dbus_return_object (DBusMessage *message, AtkObject *obj, gboolean unref);

dbus_bool_t
spi_dbus_return_v_object (DBusMessageIter *iter, AtkObject *obj, int unref);

#endif /* __ACCESSIBLE_MARSHALLER__ */
