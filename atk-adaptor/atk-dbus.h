/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#ifndef __ATK_DBUS__
#define __ATK_DBUS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

void
atk_dbus_foreach_registered(GHFunc func, gpointer data);

void
atk_dbus_foreach_update_list(GHFunc func, gpointer data);

void
atk_dbus_foreach_remove_list(GHFunc func, gpointer data);

void 
atk_dbus_notify_change(AtkObject *accessible);

AtkObject *
atk_dbus_get_object (const char *path);

gchar *
atk_dbus_get_path_from_ref(guint ref);

gchar *
atk_dbus_get_path (AtkObject *accessible);

guint
atk_dbus_register_subtree(AtkObject *accessible);

DBusMessage *
spi_dbus_return_object (DBusMessage *message, AtkObject *obj, gboolean unref);

dbus_bool_t
spi_dbus_return_v_object (DBusMessageIter *iter, AtkObject *obj, int unref);

#endif /* __ATK_DBUS__ */
