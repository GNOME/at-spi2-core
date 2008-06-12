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

#ifndef SPI_ACCESSIBLE_H_
#define SPI_ACCESSIBLE_H_

#include <droute/droute.h>
#include <spi-common/spi-dbus.h>
#include <spi-common/spi-types.h>

#include "atk/atk.h"

G_BEGIN_DECLS

Accessibility_Role spi_accessible_role_from_atk_role (AtkRole role);

void spi_initialize_accessible(DRouteData *data);
void spi_initialize_action(DRouteData *data);
void spi_initialize_application(DRouteData *data);
void spi_initialize_component(DRouteData *data);
void spi_initialize_document(DRouteData *data);
void spi_initialize_editabletext(DRouteData *data);
void spi_initialize_hyperlink(DRouteData *data);
void spi_initialize_hypertext(DRouteData *data);
void spi_initialize_image(DRouteData *data);
void spi_initialize_selection(DRouteData *data);
void spi_initialize_table(DRouteData *data);
void spi_initialize_text(DRouteData *data);
void spi_initialize_value(DRouteData *data);
void spi_initialize_introspectable(DRouteData *data, DRouteGetDatumFunction verify_object);
void spi_dbus_initialize(DRouteData *data);

AtkObject *spi_dbus_get_object(const char *path);
gchar *spi_dbus_get_path(AtkObject *obj);

DBusMessage *spi_dbus_return_object(DBusMessage *message, AtkObject *obj, int unref);
dbus_bool_t spi_dbus_return_v_object(DBusMessageIter *iter, AtkObject *obj, int unref);

/* tree.c */
void spi_register_tree_object(DBusConnection *bus, DRouteData *data, const char *path);

dbus_bool_t spi_dbus_append_tree (DBusMessage * message, AtkObject * obj, DRouteData * data);

void spi_dbus_notify_change(AtkObject *obj, gboolean new, DRouteData *data);
void spi_dbus_notify_remove(AtkObject *obj, DRouteData *data);
gboolean spi_dbus_update_cache(DRouteData *data);
gboolean spi_dbus_object_is_known(AtkObject *obj);

G_END_DECLS

#endif /* SPI_ACCESSIBLE_H_ */
