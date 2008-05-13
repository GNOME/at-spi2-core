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

#include <libspi/droute.h>
#include "Accessibility.h"
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
void spi_initialize_introspectable(DRouteData *data);

void spi_register_tree_object(DBusConnection *bus, const char *path);

dbus_bool_t spi_dbus_append_tree (DBusMessage * message, AtkObject * obj, DRouteData * data);

typedef unsigned long Accessibility_ControllerEventMask;

// TODO: auto-generate the below structs
typedef struct _Accessibility_DeviceEvent Accessibility_DeviceEvent;
struct _Accessibility_DeviceEvent
{
  Accessibility_EventType type;
  dbus_uint32_t id;
dbus_uint16_t hw_code;
  dbus_uint16_t modifiers;
  dbus_uint32_t timestamp;
  char * event_string;
  dbus_bool_t is_text;
};

typedef struct _Accessibility_EventListenerMode Accessibility_EventListenerMode;
struct _Accessibility_EventListenerMode
{
  dbus_bool_t synchronous;
  dbus_bool_t preemptive;
  dbus_bool_t global;
};

typedef struct _Accessibility_KeyDefinition Accessibility_KeyDefinition;
struct _Accessibility_KeyDefinition
{
  dbus_int32_t keycode;
  dbus_int32_t keysym;
  char *keystring;
  dbus_int32_t unused;
};

#define SPI_DBUS_NAME_REGISTRY "org.freedesktop.atspi.registry"
#define SPI_DBUS_PATH_REGISTRY "/org/freedesktop/atspi/registry"
#define SPI_DBUS_INTERFACE_REGISTRY "org.freedesktop.atspi.registry"
#define SPI_DBUS_PATH_DEC "/org/freedesktop/atspi/registry/dec"
#define SPI_DBUS_INTERFACE_DEC "org.freedesktop.atspi.deviceEventController"
#define SPI_DBUS_PATH_DESKTOP "/org/freedesktop/atspi/registry/desktop"
#define SPI_DBUS_PATH_NULL "/"

AtkObject *spi_dbus_get_object(const char *path);

gchar *spi_dbus_get_path(AtkObject *obj);
DBusMessage *spi_dbus_general_error(DBusMessage *message);
DBusMessage *spi_dbus_return_rect(DBusMessage *message, gint ix, gint iy, gint iwidth, gint iheight);
DBusMessage *spi_dbus_return_object(DBusMessage *message, AtkObject *obj, int unref);
dbus_bool_t spi_dbus_return_v_object(DBusMessageIter *iter, AtkObject *obj, int unref);

#define SPI_DBUS_RETURN_ERROR(m, e) dbus_message_new_error(m, (e)->name, (e)->message)

void spi_dbus_initialize(DRouteData *data);
void spi_dbus_emit_valist(DBusConnection *bus, const char *path, const char *interface, const char *name, int first_arg_type, va_list args);
dbus_bool_t spi_dbus_message_iter_get_struct(DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_message_iter_append_struct(DBusMessageIter *iter, ...);
dbus_bool_t spi_dbus_marshall_deviceEvent(DBusMessage *message, const Accessibility_DeviceEvent *e);
dbus_bool_t spi_dbus_demarshall_deviceEvent(DBusMessage *message, Accessibility_DeviceEvent *e);

/* tree.c */
void spi_dbus_notify_change(AtkObject *obj, gboolean new, DRouteData *data);
void spi_dbus_notify_remove(AtkObject *obj, DRouteData *data);
gboolean spi_dbus_update_cache(DRouteData *data);
gboolean spi_dbus_object_is_known(AtkObject *obj);
G_END_DECLS

#endif /* SPI_ACCESSIBLE_H_ */
