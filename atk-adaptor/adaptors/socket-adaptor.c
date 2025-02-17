/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <atk/atk.h>
#include <droute/droute.h>

#include "accessible-stateset.h"
#include "bridge.h"
#include "introspection.h"
#include "object.h"
#include "spi-dbus.h"

static DBusMessage *
new_socket_call_message (AtkComponent *component, const char *member)
{
  char *id = g_object_get_data (G_OBJECT (component), "dbus-plug-parent");
  char *bus_parent;
  char *path_parent;

  if (!id)
    {
      g_warning ("new_socket_call_message: no id");
      return NULL;
    }
  bus_parent = g_strdup (id);
  if (bus_parent && (path_parent = g_utf8_strchr (bus_parent + 1, -1, ':')))
    {
      DBusMessage *message;
      *(path_parent++) = '\0';
      message = dbus_message_new_method_call (bus_parent, path_parent,
                                              ATSPI_DBUS_INTERFACE_COMPONENT,
                                              member);
      g_free (bus_parent);
      return message;
    }
  return NULL;
}

static void
atspi_plug_component_get_extents (AtkComponent *component, gint *x, gint *y, gint *width, gint *height, AtkCoordType coord_type)
{
  DBusMessage *message = new_socket_call_message (component, "GetExtents");
  DBusMessage *reply;
  dbus_uint32_t coord_type_dbus = coord_type;
  DBusError error;
  const char *signature;
  DBusMessageIter iter, iter_struct;
  dbus_int32_t tmp;

  dbus_error_init (&error);
  dbus_message_append_args (message, DBUS_TYPE_UINT32, &coord_type_dbus, DBUS_TYPE_INVALID);
  reply = dbus_connection_send_with_reply_and_block (spi_global_app_data->bus,
                                                     message, -1, &error);
  dbus_message_unref (message);
  if (!reply)
    {
      *x = -1;
      *y = -1;
      *width = -1;
      *height = -1;
      return;
    }
  signature = dbus_message_get_signature (reply);
  if (g_strcmp0 (signature, "(iiii)") != 0)
    {
      g_warning ("Got unexpected signature %s from GetExtents\n", signature);
      dbus_message_unref (reply);
      *x = -1;
      *y = -1;
      *width = -1;
      *height = -1;
      return;
    }
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &tmp);
  *x = tmp;
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &tmp);
  *y = tmp;
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &tmp);
  *width = tmp;
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &tmp);
  *height = tmp;
  dbus_message_unref (reply);
}

static void
atspi_plug_component_get_position (AtkComponent *component, gint *x, gint *y, AtkCoordType coord_type)
{
  DBusMessage *message = new_socket_call_message (component, "GetPosition");
  DBusMessage *reply;
  dbus_uint32_t coord_type_dbus = coord_type;
  DBusError error;
  dbus_int32_t x_dbus, y_dbus;

  dbus_error_init (&error);
  dbus_message_append_args (message, DBUS_TYPE_UINT32, &coord_type_dbus, DBUS_TYPE_INVALID);
  reply = dbus_connection_send_with_reply_and_block (spi_global_app_data->bus,
                                                     message, -1, &error);
  dbus_message_unref (message);
  if (!reply)
    {
      *x = -1;
      *y = -1;
      return;
    }
  if (!dbus_message_get_args (reply, NULL, DBUS_TYPE_INT32, &x_dbus,
                              DBUS_TYPE_INT32, &y_dbus, DBUS_TYPE_INVALID))
    {
      g_warning ("GetPosition failed: %s", error.message);
      dbus_error_free (&error);
      *x = -1;
      *y = -1;
    }
  else
    {
      *x = x_dbus;
      *y = y_dbus;
    }
  dbus_message_unref (reply);
}

static void
atspi_plug_component_get_size (AtkComponent *component,
                               gint *width,
                               gint *height)
{
  DBusMessage *message = new_socket_call_message (component, "GetSize");
  DBusMessage *reply;
  DBusError error;
  dbus_uint32_t width_dbus, height_dbus;

  dbus_error_init (&error);
  reply = dbus_connection_send_with_reply_and_block (spi_global_app_data->bus,
                                                     message, -1, &error);
  dbus_message_unref (message);
  if (!reply)
    {
      *width = -1;
      *height = -1;
      return;
    }
  if (!dbus_message_get_args (reply, NULL, DBUS_TYPE_INT32, &width_dbus,
                              DBUS_TYPE_INT32, &height_dbus, DBUS_TYPE_INVALID))
    {
      g_warning ("GetSize failed: %s", error.message);
      dbus_error_free (&error);
      *width = -1;
      *height = -1;
    }
  else
    {
      *width = width_dbus;
      *height = height_dbus;
    }
  dbus_message_unref (reply);
}

static DBusMessage *
impl_Embedded (DBusConnection *bus,
               DBusMessage *message,
               void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  char *path;
  gchar *id;

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_STRING, &path, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  id = g_strconcat (dbus_message_get_sender (message), ":", path, NULL);
  g_object_set_data_full (G_OBJECT (object), "dbus-plug-parent", id, (GDestroyNotify) g_free);

  if (ATK_IS_COMPONENT (object))
    {
      AtkComponent *component = ATK_COMPONENT (object);
      AtkComponentIface *iface = ATK_COMPONENT_GET_IFACE (component);
      iface->get_extents = atspi_plug_component_get_extents;
      iface->get_size = atspi_plug_component_get_size;
      iface->get_position = atspi_plug_component_get_position;
    }

  /* Retrieve some info about the children, if they exist, when
     embedding the plug to ensure the a11y subtree is generated.
     https://bugzilla.gnome.org/show_bug.cgi?id=663876 */
  atk_object_get_n_accessible_children (object);

  return dbus_message_new_method_return (message);
}

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_SOCKET_VERSION);
}

static DRouteMethod methods[] = {
  { impl_Embedded, "Embedded" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL },
};

void
spi_initialize_socket (DRoutePath *path)
{
  droute_path_add_interface (path,
                             ATSPI_DBUS_INTERFACE_SOCKET,
                             spi_org_a11y_atspi_Socket,
                             methods, properties);
};
