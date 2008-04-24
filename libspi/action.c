/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#include "accessible.h"

static AtkAction *
get_action (DBusMessage * message)
{
  AtkObject *obj = spi_dbus_get_object (dbus_message_get_path (message));
  if (!obj)
    return NULL;
  return ATK_ACTION (obj);
}

static AtkAction *
get_action_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj || !ATK_IS_ACTION(obj))
    return NULL;
  return ATK_ACTION (obj);
}

static DBusMessage *impl_getActions(DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkAction *action = get_action(message);
  DBusMessage *reply;
  gint count;
  gint i;
  DBusMessageIter iter, iter_array, iter_struct;

  if (!action)
    return spi_dbus_general_error (message);
  count = atk_action_get_n_actions(action);
  reply = dbus_message_new_method_return (message);
  if (!reply) goto oom;
  dbus_message_iter_init_append (reply, &iter);
  if (!dbus_message_iter_open_container
      (&iter, DBUS_TYPE_ARRAY, "(sss)", &iter_array))
    goto oom;
  for (i = 0; i < count; i++)
    {
      const char *name = atk_action_get_name(action, i);
      const char *desc = atk_action_get_description(action, i);
      const char *kb = atk_action_get_keybinding(action, i);
      if (!name) name = "";
      if (!desc) desc = "";
      if (!kb) kb = "";
      if (!dbus_message_iter_open_container(&iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct)) goto oom;
      dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_STRING, &name);
      dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_STRING, &desc);
      dbus_message_iter_append_basic(&iter_struct, DBUS_TYPE_STRING, &kb);
      if (!dbus_message_iter_close_container(&iter_array, &iter_struct)) goto oom;
    }
  if (!dbus_message_iter_close_container (&iter, &iter_array))
    goto oom;
  return reply;
oom:
  // TODO: handle out-of-memory
  return reply;
}

static DBusMessage *impl_doAction(DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkAction *action = get_action(message);
  DBusError error;
  dbus_int32_t index;
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!action)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &index, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  rv = atk_action_do_action(action, index);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv, DBUS_TYPE_INVALID);
    }
  return reply;
}

DRouteMethod methods[] =
{
  { DROUTE_METHOD, impl_getActions, "getActions", "a(sss),,o" },
  { DROUTE_METHOD, impl_doAction, "doAction", "i,index,i:b,,o" },
  { 0, NULL, NULL, NULL }
};

void
spi_initialize_action (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Action",
			methods, NULL,
			(DRouteGetDatumFunction) get_action_from_path,
			NULL);
};
