/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 *           
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

#ifndef _ATSPI_MISC_PRIVATE_H_
#define _ATSPI_MISC_PRIVATE_H_

/* Private internal implementation details of at-spi. */

#include "atspi.h"

#include "dbus/dbus.h"

#include "dbind/dbind.h"

#define ATSPI_CACHE_PARENT      0x0001
#define ATSPI_CACHE_CHILDREN    0x0002
#define ATSPI_CACHE_NAME        0x0004
#define ATSPI_CACHE_DESCRIPTION 0x0008
#define ATSPI_CACHE_STATES      0x0010
#define ATSPI_CACHE_ROLE        0x0010

typedef struct _AtspiReference AtspiReference;
struct _AtspiReference
{
  char *name;
  char *path;
};

/* constants */
#define ATSPI_DBUS_NAME_REGISTRY "org.a11y.atspi.Registry"
#define ATSPI_DBUS_PATH_REGISTRY "/org/a11y/atspi/registry"
#define ATSPI_DBUS_INTERFACE_REGISTRY "org.a11y.atspi.Registry"

#define ATSPI_DBUS_PATH_NULL "/org/a11y/atspi/null"
#define ATSPI_DBUS_PATH_ROOT "/org/a11y/atspi/accessible/root"

#define ATSPI_DBUS_PATH_DEC "/org/a11y/atspi/registry/deviceeventcontroller"
#define ATSPI_DBUS_INTERFACE_DEC "org.a11y.atspi.DeviceEventController"
#define ATSPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER "org.a11y.atspi.DeviceEventListener"

#define ATSPI_DBUS_INTERFACE_CACHE "org.a11y.atspi.Cache"
#define ATSPI_DBUS_INTERFACE_ACCESSIBLE "org.a11y.atspi.Accessible"
#define ATSPI_DBUS_INTERFACE_ACTION "org.a11y.atspi.Action"
#define ATSPI_DBUS_INTERFACE_APPLICATION "org.a11y.atspi.Application"
#define ATSPI_DBUS_INTERFACE_COLLECTION "org.a11y.atspi.Collection"
#define ATSPI_DBUS_INTERFACE_COMPONENT "org.a11y.atspi.Component"
#define ATSPI_DBUS_INTERFACE_DOCUMENT "org.a11y.atspi.Document"
#define ATSPI_DBUS_INTERFACE_EDITABLE_TEXT "org.a11y.atspi.EditableText"
#define ATSPI_DBUS_INTERFACE_EVENT_KEYBOARD "org.a11y.atspi.Event.Keyboard"
#define ATSPI_DBUS_INTERFACE_EVENT_MOUSE "org.a11y.atspi.Event.Mouse"
#define ATSPI_DBUS_INTERFACE_EVENT_OBJECT "org.a11y.atspi.Event.Object"
#define ATSPI_DBUS_INTERFACE_HYPERLINK "org.a11y.atspi.Hyperlink"
#define ATSPI_DBUS_INTERFACE_HYPERTEXT "org.a11y.atspi.Hypertext"
#define ATSPI_DBUS_INTERFACE_IMAGE "org.a11y.atspi.Image"
#define ATSPI_DBUS_INTERFACE_SELECTION "org.a11y.atspi.Selection"
#define ATSPI_DBUS_INTERFACE_TABLE "org.a11y.atspi.Table"
#define ATSPI_DBUS_INTERFACE_TEXT "org.a11y.atspi.Text"
#define ATSPI_DBUS_INTERFACE_VALUE "org.a11y.atspi.Value"
#define ATSPI_DBUS_INTERFACE_SOCKET "org.a11y.atspi.Socket"

/* externs */
extern const char *atspi_path_dec;
extern const char *atspi_path_registry;
extern const char *atspi_path_root;
extern const char *atspi_bus_registry;
extern const char *atspi_interface_accessible;
extern const char *atspi_interface_action;
extern const char *atspi_interface_application;
extern const char *atspi_interface_collection;
extern const char *atspi_interface_component;
extern const char *atspi_interface_dec;
extern const char *atspi_interface_device_event_listener;
extern const char *atspi_interface_document;
extern const char *atspi_interface_editable_text;
extern const char *atspi_interface_hyperlink;
extern const char *atspi_interface_hypertext;
extern const char *atspi_interface_image;
extern const char *atspi_interface_registry;
extern const char *atspi_interface_selection;
extern const char *atspi_interface_table;
extern const char *atspi_interface_text;
extern const char *atspi_interface_cache;
extern const char *atspi_interface_value;

/* function prototypes */
gint _atspi_get_iface_num (const char *iface);

DBusConnection * _atspi_bus ();

AtspiAccessible * _atspi_ref_accessible (const char *app, const char *path);

AtspiAccessible *
_atspi_dbus_return_accessible_from_message (DBusMessage *message);

AtspiAccessible *
_atspi_dbus_return_accessible_from_iter (DBusMessageIter *iter);

AtspiHyperlink *
_atspi_dbus_return_hyperlink_from_message (DBusMessage *message);

AtspiHyperlink *
_atspi_dbus_return_hyperlink_from_iter (DBusMessageIter *iter);

dbus_bool_t _atspi_dbus_call (gpointer obj, const char *interface, const char *method, GError **error, const char *type, ...);

DBusMessage *_atspi_dbus_call_partial (gpointer obj, const char *interface, const char *method, GError **error, const char *type, ...);

DBusMessage *_atspi_dbus_call_partial_va (gpointer obj, const char *interface, const char *method, GError **error, const char *type, va_list args);

dbus_bool_t _atspi_dbus_get_property (gpointer obj, const char *interface, const char *name, GError **error, const char *type, void *data);

DBusMessage * _atspi_dbus_send_with_reply_and_block (DBusMessage *message);

GHashTable *_atspi_dbus_hash_from_message (DBusMessage *message);

GHashTable *_atspi_dbus_hash_from_iter (DBusMessageIter *iter);

GArray *_atspi_dbus_attribute_array_from_message (DBusMessage *message);

GArray *_atspi_dbus_attribute_array_from_iter (DBusMessageIter *iter);

#define _ATSPI_DBUS_CHECK_SIG(message, type, ret) \
  if (!message) { \
    g_warning ("at-spi: Got no message at %s line %d\n", __FILE__, __LINE__); \
    return (ret); \
  } \
  if (strcmp (dbus_message_get_signature (message), type) != 0) \
  { \
    g_warning ("at-spi: Expected message signature %s but got %s at %s line %d", type, dbus_message_get_signature (message), __FILE__, __LINE__); \
    dbus_message_unref (message); \
    return (ret); \
  }
#endif	/* _ATSPI_MISC_PRIVATE_H_ */
