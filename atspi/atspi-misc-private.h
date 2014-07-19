/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 * Copyright 2010, 2011 Novell, Inc.
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

#include <string.h>

#include "atspi.h"

#include "dbus/dbus.h"

#include "dbind/dbind.h"

G_BEGIN_DECLS

typedef struct _AtspiReference AtspiReference;
struct _AtspiReference
{
  char *name;
  char *path;
};

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
extern const char *atspi_interface_table_cell;
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

DBusMessage * _atspi_dbus_send_with_reply_and_block (DBusMessage *message, GError **error);

GHashTable *_atspi_dbus_return_hash_from_message (DBusMessage *message);

GHashTable *_atspi_dbus_hash_from_iter (DBusMessageIter *iter);

GArray *_atspi_dbus_return_attribute_array_from_message (DBusMessage *message);

GArray *_atspi_dbus_attribute_array_from_iter (DBusMessageIter *iter);

void _atspi_dbus_set_interfaces (AtspiAccessible *accessible, DBusMessageIter *iter);

void _atspi_dbus_set_state (AtspiAccessible *accessible, DBusMessageIter *iter);

#define _ATSPI_DBUS_CHECK_SIG(message, type, error, ret) \
  if (!message) \
    return (ret); \
  if (dbus_message_get_type (message) == DBUS_MESSAGE_TYPE_ERROR) \
  { \
    const char *err; \
    dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &err, DBUS_TYPE_INVALID); \
    if (err) \
      g_set_error_literal (error, ATSPI_ERROR, ATSPI_ERROR_IPC, err); \
    dbus_message_unref (message); \
    return ret; \
  } \
  if (strcmp (dbus_message_get_signature (message), type) != 0) \
  { \
    g_warning ("at-spi: Expected message signature %s but got %s at %s line %d", type, dbus_message_get_signature (message), __FILE__, __LINE__); \
    dbus_message_unref (message); \
    return (ret); \
  }


/**
 * ATSPI_ERROR:
 *
 * Error domain for AT-SPI IPC failures. Errors in this domain will
 * be from the #ATSPIAtspiError enumeration. See #GError for information on
 * error domains.
 */
#define ATSPI_ERROR _atspi_error_quark()
GQuark _atspi_error_quark (void);

/**
 * AtspiError:
 * @ATSPI_APPLICATION_NO_LONGER_EXISTS: The application has quit.
 */
typedef enum
{
  ATSPI_ERROR_APPLICATION_GONE,
  ATSPI_ERROR_IPC,
  ATSPI_ERROR_SYNC_NOT_ALLOWED,
} AtspiError;

extern GMainLoop *atspi_main_loop;
extern gboolean atspi_no_cache;

GHashTable *_atspi_get_live_refs ();

gchar *_atspi_name_compat (gchar *in);

GHashTable *_atspi_dbus_update_cache_from_dict (AtspiAccessible *accessible, DBusMessageIter *iter);

gboolean _atspi_get_allow_sync ();

gboolean _atspi_set_allow_sync (gboolean val);

void _atspi_set_error_no_sync (GError **error);
G_END_DECLS

#endif	/* _ATSPI_MISC_PRIVATE_H_ */
