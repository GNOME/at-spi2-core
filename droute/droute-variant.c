/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#include "glib.h"
#include <stdlib.h>
#include <string.h>

#include "droute-variant.h"

/*---------------------------------------------------------------------------*/

dbus_bool_t
droute_return_v_int32 (DBusMessageIter *iter, dbus_int32_t val)
{
  DBusMessageIter sub;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, DBUS_TYPE_INT32_AS_STRING, &sub))
    {
      return FALSE;
    }
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &val);
  dbus_message_iter_close_container (iter, &sub);
  return TRUE;
}

dbus_bool_t
droute_return_v_uint32 (DBusMessageIter *iter, dbus_uint32_t val)
{
  DBusMessageIter sub;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, DBUS_TYPE_UINT32_AS_STRING, &sub))
    {
      return FALSE;
    }
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_UINT32, &val);
  dbus_message_iter_close_container (iter, &sub);
  return TRUE;
}

dbus_bool_t
droute_return_v_double (DBusMessageIter *iter, double val)
{
  DBusMessageIter sub;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, DBUS_TYPE_DOUBLE_AS_STRING, &sub))
    {
      return FALSE;
    }
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_DOUBLE, &val);
  dbus_message_iter_close_container (iter, &sub);
  return TRUE;
}

dbus_bool_t
droute_return_v_string (DBusMessageIter *iter, const char *val)
{
  DBusMessageIter sub;

  if (!val)
    val = "";
  if (!g_utf8_validate (val, -1, NULL))
    {
      g_warning ("droute: Received bad UTF-8 string");
      val = "";
    }

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &sub))
    {
      return FALSE;
    }
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_STRING, &val);
  dbus_message_iter_close_container (iter, &sub);
  return TRUE;
}

dbus_bool_t
droute_return_v_object (DBusMessageIter *iter, const char *path)
{
  DBusMessageIter sub;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, DBUS_TYPE_OBJECT_PATH_AS_STRING, &sub))
    {
      return FALSE;
    }
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (iter, &sub);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

dbus_int32_t
droute_get_v_int32 (DBusMessageIter *iter)
{
  DBusMessageIter sub;
  dbus_int32_t rv;

  // TODO- ensure we have the correct type
  dbus_message_iter_recurse (iter, &sub);
  dbus_message_iter_get_basic (&sub, &rv);
  return rv;
}

const char *
droute_get_v_string (DBusMessageIter *iter)
{
  DBusMessageIter sub;
  char *rv;

  // TODO- ensure we have the correct type
  dbus_message_iter_recurse (iter, &sub);
  dbus_message_iter_get_basic (&sub, &rv);
  return rv;
}

/*END------------------------------------------------------------------------*/
