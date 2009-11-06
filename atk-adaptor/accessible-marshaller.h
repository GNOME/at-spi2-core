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

#ifndef ACCESSIBLE_MARSHALLER
#define ACCESSIBLE_MARSHALLER

#include <dbus/dbus.h>
#include <atk/atk.h>

void
spi_dbus_append_name_and_path_inner (DBusMessageIter *iter, const char *bus_name, const char *path);

void
spi_dbus_append_name_and_path (DBusMessage *message, DBusMessageIter *iter, AtkObject *obj, gboolean unref);

DBusMessage *
spi_dbus_return_object (DBusMessage *message, AtkObject *obj, gboolean do_register, gboolean unref);

DBusMessage *
spi_dbus_return_hyperlink (DBusMessage *message, AtkHyperlink *link, AtkObject *container, gboolean unref);

DBusMessage *
spi_dbus_return_sub_object (DBusMessage *message, GObject *sub, GObject *container, gboolean unref);

dbus_bool_t
spi_dbus_return_v_object (DBusMessageIter *iter, AtkObject *obj, int unref);

void
spi_atk_append_accessible(AtkObject *obj, gpointer iter);

void
spi_atk_append_attribute_set (DBusMessageIter *iter, AtkAttributeSet *attr);

void
spi_atk_append_attribute_set_inner (DBusMessageIter *iter, AtkAttributeSet *attr);

void
append_atk_object_interfaces (AtkObject *object, DBusMessageIter *iter);
#endif /* ACCESSIBLE_MARSHALLER */
