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

static AtkHyperlink *
get_hyperlink (DBusMessage * message)
{
  AtkObject *obj = spi_dbus_get_object (dbus_message_get_path (message));
  if (!obj)
    return NULL;
  return ATK_HYPERLINK (obj);
}

static AtkHyperlink *
get_hyperlink_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj || !ATK_IS_HYPERLINK(obj))
    return NULL;
  return ATK_HYPERLINK (obj);
}

static dbus_bool_t
impl_get_nAnchors (const char *path, DBusMessageIter * iter, void *user_data)
{
  AtkHyperlink *link = get_hyperlink_from_path (path, user_data);
  if (!link)
    return FALSE;
  return droute_return_v_int32 (iter, atk_hyperlink_get_n_anchors (link));
}


static dbus_bool_t
impl_get_startIndex (const char *path, DBusMessageIter * iter,
		     void *user_data)
{
  AtkHyperlink *link = get_hyperlink_from_path (path, user_data);
  if (!link)
    return FALSE;
  return droute_return_v_int32 (iter, atk_hyperlink_get_start_index (link));
}

static dbus_bool_t
impl_get_endIndex (const char *path, DBusMessageIter * iter, void *user_data)
{
  AtkHyperlink *link = get_hyperlink_from_path (path, user_data);
  if (!link)
    return FALSE;
  return droute_return_v_int32 (iter, atk_hyperlink_get_end_index (link));
}

static DBusMessage *
impl_getObject (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkHyperlink *link = get_hyperlink (message);
  DBusError error;
  dbus_int32_t i;
  AtkObject *atk_object;

  if (!link)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &i, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  atk_object = atk_hyperlink_get_object (link, i);
  return spi_dbus_return_object (message, atk_object, FALSE);
}

static DBusMessage *
impl_getURI (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkHyperlink *link = get_hyperlink (message);
  dbus_int32_t i;
  DBusError error;
  gchar *rv;
  DBusMessage *reply;

  if (!link)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &i, DBUS_TYPE_INT32, &i,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
  AtkHyperlink *link = get_hyperlink (message);
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!link)
    return spi_dbus_general_error (message);

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
spi_initialize_hyperlink (DRouteData * data)
{
  droute_add_interface (data, SPI_DBUS_INTERFACE_HYPERLINK,
			methods, properties,
			(DRouteGetDatumFunction) get_hyperlink_from_path,
			NULL);
};
