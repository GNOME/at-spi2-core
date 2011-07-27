/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008, 2009, 2010 Codethink Ltd.
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

#ifndef ACCESSIBLE_OBJECT_H
#define ACCESSIBLE_OBJECT_H

#include <atk/atk.h>
#include <dbus/dbus.h>

void
spi_object_lease_if_needed (GObject *obj);

void
spi_object_append_reference (DBusMessageIter * iter, AtkObject * obj);

void
spi_hyperlink_append_reference (DBusMessageIter * iter, AtkObject * obj);

void
spi_object_append_v_reference (DBusMessageIter * iter, AtkObject * obj);

void
spi_object_append_desktop_reference (DBusMessageIter * iter);

void
spi_object_append_null_reference (DBusMessageIter * iter);

DBusMessage *
spi_object_return_reference (DBusMessage * msg, AtkObject * obj);

DBusMessage *
spi_hyperlink_return_reference (DBusMessage * msg, AtkHyperlink * obj);

void
spi_object_append_interfaces (DBusMessageIter * iter, AtkObject * obj);

void
spi_object_append_attribute_set (DBusMessageIter * iter, AtkAttributeSet * attr);

AtspiRole
spi_accessible_role_from_atk_role (AtkRole role);

#endif /* ACCESSIBLE_OBJECT_H */
