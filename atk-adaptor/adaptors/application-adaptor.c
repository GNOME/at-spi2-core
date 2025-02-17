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

#include "introspection.h"
#include "spi-dbus.h"

/* for spi_global_app_data  is there a better way? */
#include "../bridge.h"

static dbus_bool_t
impl_get_ToolkitName (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_string (iter, atk_get_toolkit_name ());
}

static dbus_bool_t
impl_get_ToolkitVersion (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_string (iter, atk_get_toolkit_version ());
}

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_string (iter, atk_get_toolkit_version ());
}

static dbus_bool_t
impl_get_InterfaceVersion (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_APPLICATION_VERSION);
}

static dbus_bool_t
impl_get_AtspiVersion (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_string (iter, "2.1");
}

static dbus_int32_t id;

static dbus_bool_t
impl_get_Id (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_int32 (iter, id);
}

static dbus_bool_t
impl_set_Id (DBusMessageIter *iter, void *user_data)
{
  id = droute_get_v_int32 (iter);
  return TRUE;
}

static DBusMessage *
impl_registerToolkitEventListener (DBusConnection *bus,
                                   DBusMessage *message,
                                   void *user_data)
{
  return NULL;
}

static DBusMessage *
impl_registerObjectEventListener (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  return NULL;
}

static DBusMessage *
impl_GetLocale (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  return NULL;
}

static DBusMessage *
impl_get_app_bus (DBusConnection *bus, DBusMessage *msg, void *data)
{
  DBusMessage *reply;

  if (bus == spi_global_app_data->bus)
    spi_atk_add_client (dbus_message_get_sender (msg));

  if (!spi_global_app_data->app_bus_addr)
    spi_atk_create_socket (spi_global_app_data);

  reply = dbus_message_new_method_return (msg);
  if (reply)
    {
      const char *retval = (atspi_is_initialized () ? "" : spi_global_app_data->app_bus_addr);
      if (!retval)
        retval = "";
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &retval, DBUS_TYPE_INVALID);
    }

  return reply;
}

static DRouteMethod methods[] = {
  { impl_registerToolkitEventListener, "registerToolkitEventListener" },
  { impl_registerObjectEventListener, "registerObjectEventListener" },
  { impl_GetLocale, "GetLocale" },
  { impl_get_app_bus, "GetApplicationBusAddress" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_ToolkitName, NULL, "ToolkitName" },
  { impl_get_ToolkitVersion, NULL, "ToolkitVersion" },
  { impl_get_Version, NULL, "Version" },
  { impl_get_AtspiVersion, NULL, "AtspiVersion" },
  { impl_get_InterfaceVersion, NULL, "InterfaceVersion" },
  { impl_get_Id, impl_set_Id, "Id" },
  { NULL, NULL, NULL }
};

/*static long
obj_is_root (const char *path, void *user_data)
{
  AtkObject *obj = atk_dbus_get_object (path);
  return (obj == atk_get_root ());
}*/

void
spi_initialize_application (DRoutePath *path)
{
  droute_path_add_interface (path,
                             ATSPI_DBUS_INTERFACE_APPLICATION,
                             spi_org_a11y_atspi_Application,
                             methods, properties);
};
