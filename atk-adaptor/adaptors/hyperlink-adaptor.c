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

#include "bridge.h"
#include <atk/atk.h>
#include <droute/droute.h>

#include "spi-dbus.h"

#include "introspection.h"
#include "object.h"

static AtkHyperlink *
get_hyperlink (void *user_data)
{
  if (ATK_IS_HYPERLINK (user_data))
    return ATK_HYPERLINK (user_data);
  if (ATK_IS_HYPERLINK_IMPL (user_data))
    return atk_hyperlink_impl_get_hyperlink (ATK_HYPERLINK_IMPL (user_data));
  return NULL;
}

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_HYPERLINK_VERSION);
}

static dbus_bool_t
impl_get_NAnchors (DBusMessageIter *iter, void *user_data)
{
  AtkHyperlink *link = get_hyperlink (user_data);
  g_return_val_if_fail (ATK_IS_HYPERLINK (link), FALSE);
  return droute_return_v_int32 (iter, atk_hyperlink_get_n_anchors (link));
}

static dbus_bool_t
impl_get_StartIndex (DBusMessageIter *iter, void *user_data)
{
  AtkHyperlink *link = get_hyperlink (user_data);
  g_return_val_if_fail (ATK_IS_HYPERLINK (link), FALSE);
  return droute_return_v_int32 (iter, atk_hyperlink_get_start_index (link));
}

static dbus_bool_t
impl_get_EndIndex (DBusMessageIter *iter, void *user_data)
{
  AtkHyperlink *link = get_hyperlink (user_data);
  g_return_val_if_fail (ATK_IS_HYPERLINK (link), FALSE);
  return droute_return_v_int32 (iter, atk_hyperlink_get_end_index (link));
}

static DBusMessage *
impl_GetObject (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkHyperlink *link = get_hyperlink (user_data);
  dbus_int32_t i;
  AtkObject *atk_object;

  g_return_val_if_fail (ATK_IS_HYPERLINK (link),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &i, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  atk_object = atk_hyperlink_get_object (link, i);
  return spi_object_return_reference (message, atk_object);
}

static DBusMessage *
impl_GetURI (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkHyperlink *link = get_hyperlink (user_data);
  dbus_int32_t i;
  gchar *rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_HYPERLINK (link),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &i, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  rv = atk_hyperlink_get_uri (link, i);
  if (!rv)
    rv = g_strdup ("");
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &rv,
                                DBUS_TYPE_INVALID);
    }
  g_free (rv);
  return reply;
}

static DBusMessage *
impl_IsValid (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkHyperlink *link = get_hyperlink (user_data);
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_HYPERLINK (link),
                        droute_not_yet_handled_error (message));

  rv = atk_hyperlink_is_valid (link);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DRouteMethod methods[] = {
  { impl_GetObject, "GetObject" },
  { impl_GetURI, "GetURI" },
  { impl_IsValid, "IsValid" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_NAnchors, NULL, "NAnchors" },
  { impl_get_StartIndex, NULL, "StartIndex" },
  { impl_get_EndIndex, NULL, "EndIndex" },
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};

void
spi_initialize_hyperlink (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_HYPERLINK,
                         spi_org_a11y_atspi_Hyperlink,
                         methods, properties);
};
