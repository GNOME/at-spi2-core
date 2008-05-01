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

#ifndef _ATSPI_DBUS_H
#define _ATSPI_DBUS_H

#include <dbus/dbus.h>
#include <atk/atk.h>
#include <stdio.h>

#include "droute.h"

AtkObject *spi_dbus_get_object(const char *path);

gchar *spi_dbus_get_path(AtkObject *obj);
DBusMessage *spi_dbus_general_error(DBusMessage *message);
DBusMessage *spi_dbus_return_rect(DBusMessage *message, gint ix, gint iy, gint iwidth, gint iheight);
DBusMessage *spi_dbus_return_object(DBusMessage *message, AtkObject *obj, int unref);
dbus_bool_t spi_dbus_return_v_object(DBusMessageIter *iter, AtkObject *obj, int unref);

#define SPI_DBUS_RETURN_ERROR(m, e) dbus_message_new_error(m, (e)->name, (e)->message)

void spi_dbus_initialize(DRouteData *data);

/* tree.c */
void spi_dbus_notify_change(AtkObject *obj, gboolean new, DRouteData *data);
void spi_dbus_notify_remove(AtkObject *obj, DRouteData *data);
gboolean spi_dbus_update_cache(DRouteData *data);
#endif	/* _ATSPI_DBUS_H */
