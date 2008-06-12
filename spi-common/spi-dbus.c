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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus.h>

#include "spi-types.h"

DBusMessage *
spi_dbus_general_error (DBusMessage * message)
{
  return dbus_message_new_error (message,
				 "org.freedesktop.atspi.GeneralError",
				 "General error");
}


DBusMessage *
spi_dbus_return_rect (DBusMessage * message, gint ix, gint iy, gint iwidth,
		      gint iheight)
{
  DBusMessage *reply;
  dbus_uint32_t x, y, width, height;

  x = ix;
  y = iy;
  width = iwidth;
  height = iheight;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      DBusMessageIter iter, sub;
      dbus_message_iter_init_append (reply, &iter);
      if (!dbus_message_iter_open_container
	  (&iter, DBUS_TYPE_STRUCT, NULL, &sub))
	goto oom;
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &x);
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &y);
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &width);
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &height);
      if (!dbus_message_iter_close_container (&iter, &sub))
	goto oom;
    }
  return reply;
oom:
  /* todo: return an error */
  return reply;
}

void spi_dbus_emit_valist(DBusConnection *bus, const char *path, const char *interface, const char *name, int first_arg_type, va_list args)
{
  DBusMessage *sig;

  sig = dbus_message_new_signal(path, interface, name);
  if (first_arg_type != DBUS_TYPE_INVALID)
  {
    dbus_message_append_args_valist(sig, first_arg_type, args);
  }
  dbus_connection_send(bus, sig, NULL);
  dbus_message_unref(sig);
}

dbus_bool_t spi_dbus_message_iter_get_struct(DBusMessageIter *iter, ...)
{
  va_list args;
  DBusMessageIter iter_struct;
  int type;
  void *ptr;

  dbus_message_iter_recurse(iter, &iter_struct);
  va_start(args, iter);
  for (;;)
  {
    type = va_arg(args, int);
    if (type == DBUS_TYPE_INVALID) break;
    if (type != dbus_message_iter_get_arg_type(&iter_struct))
    {
      va_end(args);
      return FALSE;
    }
    ptr = va_arg(args, void *);
    dbus_message_iter_get_basic(&iter_struct, ptr);
    dbus_message_iter_next(&iter_struct);
  }
  dbus_message_iter_next(iter);
  va_end(args);
  return TRUE;
}

dbus_bool_t spi_dbus_message_iter_append_struct(DBusMessageIter *iter, ...)
{
  va_list args;
  DBusMessageIter iter_struct;
  int type;
  void *ptr;

  if (!dbus_message_iter_open_container(iter, DBUS_TYPE_STRUCT, NULL, &iter_struct)) return FALSE;
  va_start(args, iter);
  for (;;)
  {
    type = va_arg(args, int);
    if (type == DBUS_TYPE_INVALID) break;
    ptr = va_arg(args, void *);
    dbus_message_iter_append_basic(&iter_struct, type, ptr);
  }
  if (!dbus_message_iter_close_container(iter, &iter_struct)) return FALSE;
  va_end(args);
  return TRUE;
}

dbus_bool_t spi_dbus_marshal_deviceEvent(DBusMessage *message, const Accessibility_DeviceEvent *e)
{
  DBusMessageIter iter;

  if (!message) return FALSE;
  dbus_message_iter_init_append(message, &iter);
  return spi_dbus_message_iter_append_struct(&iter, DBUS_TYPE_UINT32, &e->type, DBUS_TYPE_INT32, &e->id, DBUS_TYPE_INT16, &e->hw_code, DBUS_TYPE_INT16, &e->modifiers, DBUS_TYPE_INT32, &e->timestamp, DBUS_TYPE_STRING, &e->event_string, DBUS_TYPE_BOOLEAN, &e->is_text, DBUS_TYPE_INVALID);
}

dbus_bool_t spi_dbus_demarshal_deviceEvent(DBusMessage *message, Accessibility_DeviceEvent *e)
{
  DBusMessageIter iter;

  dbus_message_iter_init(message, &iter);
  return spi_dbus_message_iter_get_struct(&iter, DBUS_TYPE_UINT32, &e->type, DBUS_TYPE_INT32, &e->id, DBUS_TYPE_INT16, &e->hw_code, DBUS_TYPE_INT16, &e->modifiers, DBUS_TYPE_INT32, &e->timestamp, DBUS_TYPE_STRING, &e->event_string, DBUS_TYPE_BOOLEAN, &e->is_text, DBUS_TYPE_INVALID);
}

dbus_bool_t spi_dbus_get_simple_property (DBusConnection *bus, const char *dest, const char *path, const char *interface, const char *prop, int *type, void *ptr, DBusError *error)
{
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_variant;
  int typ;

  dbus_error_init (error);
  message = dbus_message_new_method_call (dest, path, "org.freedesktop.DBus.Properties", "get");
  if (!message) return FALSE;
  if (!dbus_message_append_args (message, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &prop, DBUS_TYPE_INVALID))
  {
    return FALSE;
  }
  reply = dbus_connection_send_with_reply_and_block (bus, message, 1000, error);
  dbus_message_unref (message);
  if (!reply) return FALSE;
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_variant);
  typ = dbus_message_iter_get_arg_type (&iter_variant);
  if (type) *type = typ;
  if (typ == DBUS_TYPE_INVALID || typ == DBUS_TYPE_STRUCT || typ == DBUS_TYPE_ARRAY)
  {
    return FALSE;
  }
  dbus_message_iter_get_basic (&iter_variant, ptr);
  dbus_message_unref (reply);
  return TRUE;
}

dbus_bool_t
spi_dbus_add_disconnect_match (DBusConnection *bus, const char *name)
{
  char *match = g_strdup_printf ("interface=%s,member=NameOwnerChanged,arg0=%s", DBUS_INTERFACE_DBUS, name);
  if (match)
  {
    DBusError error;
    dbus_error_init (&error);
    dbus_bus_add_match (bus, match, &error);
    g_free (match);
    return !dbus_error_is_set (&error);
  }
  else return FALSE;
}

dbus_bool_t
spi_dbus_remove_disconnect_match (DBusConnection *bus, const char *name)
{
  char *match = g_strdup_printf ("interface=%s,member=NameOwnerChanged,arg0=%s", DBUS_INTERFACE_DBUS, name);
  if (match)
  {
    DBusError error;
    dbus_error_init (&error);
    dbus_bus_remove_match (bus, match, &error);
    g_free (match);
    return !dbus_error_is_set (&error);
  }
  else return FALSE;
}
