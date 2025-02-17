/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#pragma once

#include <dbus/dbus.h>

dbus_bool_t droute_return_v_int32 (DBusMessageIter *iter, dbus_int32_t val);
dbus_bool_t droute_return_v_uint32 (DBusMessageIter *iter, dbus_uint32_t val);
dbus_bool_t droute_return_v_double (DBusMessageIter *iter, double val);
dbus_bool_t droute_return_v_string (DBusMessageIter *iter, const char *val);
dbus_bool_t droute_return_v_object (DBusMessageIter *iter, const char *path);

dbus_int32_t droute_get_v_int32 (DBusMessageIter *iter);
const char *droute_get_v_string (DBusMessageIter *iter);
