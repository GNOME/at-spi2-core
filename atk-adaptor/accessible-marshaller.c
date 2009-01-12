/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#include "accessible-register.h"
#include "accessible-marshaller.h"

/*---------------------------------------------------------------------------*/

/*
 * Marshals the D-Bus path of an AtkObject into a D-Bus message.
 *
 * Unrefs the AtkObject if unref is true.
 */
DBusMessage *
spi_dbus_return_object (DBusMessage *message, AtkObject *obj, gboolean unref)
{
  DBusMessage *reply;
  gchar *path;

  path = atk_dbus_object_to_path (obj);

  if (unref)
    g_object_unref (obj);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_OBJECT_PATH, path,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

/*---------------------------------------------------------------------------*/

/*
 * Marshals a variant containing the D-Bus path of an AtkObject into a D-Bus
 * message.
 *
 * Unrefs the object if unref is true.
 */
dbus_bool_t
spi_dbus_return_v_object (DBusMessageIter *iter, AtkObject *obj, int unref)
{
  char *path;

  path = atk_dbus_object_to_path (obj);

  if (unref)
    g_object_unref (obj);

  return droute_return_v_object (iter, path);
}

/*END------------------------------------------------------------------------*/

