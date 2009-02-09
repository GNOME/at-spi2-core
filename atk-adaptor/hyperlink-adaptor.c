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

#include <atk/atk.h>
#include <droute/droute.h>

#include "accessible-marshaller.h"
#include "spi-common/spi-dbus.h"

static dbus_bool_t
impl_get_nAnchors (DBusMessageIter * iter, void *user_data)
{
  AtkHyperlink *link = (AtkHyperlink *) user_data;
  g_return_val_if_fail (ATK_IS_HYPERLINK (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_hyperlink_get_n_anchors (link));
}


static dbus_bool_t
impl_get_startIndex (DBusMessageIter * iter,
                     void *user_data)
{
  AtkHyperlink *link = (AtkHyperlink *) user_data;
  g_return_val_if_fail (ATK_IS_HYPERLINK (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_hyperlink_get_start_index (link));
}

static dbus_bool_t
impl_get_endIndex (DBusMessageIter * iter, void *user_data)
{
  AtkHyperlink *link = (AtkHyperlink *) user_data;
  g_return_val_if_fail (ATK_IS_HYPERLINK (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_hyperlink_get_end_index (link));
}

static DBusMessage *
impl_getObject (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkHyperlink *link = (AtkHyperlink *) user_data;
  DBusError error;
  dbus_int32_t i;
  AtkObject *atk_object;

  g_return_val_if_fail (ATK_IS_HYPERLINK (user_data),
                        droute_not_yet_handled_error (message));
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &i, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  atk_object = atk_hyperlink_get_object (link, i);
  return spi_dbus_return_object (message, atk_object, FALSE);
}

static DBusMessage *
impl_getURI (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkHyperlink *link = (AtkHyperlink *) user_data;
  dbus_int32_t i;
  DBusError error;
  gchar *rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_HYPERLINK (user_data),
                        droute_not_yet_handled_error (message));
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &i, DBUS_TYPE_INT32, &i,
       DBUS_TYPE_INVALID))
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
impl_isValid (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkHyperlink *link = (AtkHyperlink *) user_data;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_HYPERLINK (user_data),
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
  {impl_getObject, "getObject"},
  {impl_getURI, "getURI"},
  {impl_isValid, "isValid"},
  {NULL, NULL}
};

static DRouteProperty properties[] = {
  {impl_get_nAnchors, NULL, "nAnchors"},
  {impl_get_startIndex, NULL, "startIndex"},
  {impl_get_endIndex, NULL, "endIndex"},
  {NULL, NULL, NULL}
};

void
spi_initialize_hyperlink (DRoutePath *path)
{
  droute_path_add_interface (path,
                             SPI_DBUS_INTERFACE_HYPERLINK,
                             methods,
                             properties);
};
