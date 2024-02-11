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

#include <dbus/dbus.h>
#include <glib-object.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "accessible-register.h"
#include <atspi/atspi.h>

DBusMessage *
spi_dbus_general_error (DBusMessage *message)
{
  return dbus_message_new_error (message,
                                 "org.a11y.atspi.GeneralError",
                                 "General error");
}

DBusMessage *
spi_dbus_return_rect (DBusMessage *message, gint ix, gint iy, gint iwidth, gint iheight)
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
      if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_STRUCT, NULL, &sub))
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

dbus_bool_t
spi_dbus_message_iter_get_struct (DBusMessageIter *iter, ...)
{
  va_list args;
  DBusMessageIter iter_struct;
  int type;
  void *ptr;

  dbus_message_iter_recurse (iter, &iter_struct);
  va_start (args, iter);
  for (;;)
    {
      type = va_arg (args, int);
      if (type == DBUS_TYPE_INVALID)
        break;
      if (type != dbus_message_iter_get_arg_type (&iter_struct))
        {
          va_end (args);
          return FALSE;
        }
      ptr = va_arg (args, void *);
      dbus_message_iter_get_basic (&iter_struct, ptr);
      dbus_message_iter_next (&iter_struct);
    }
  dbus_message_iter_next (iter);
  va_end (args);
  return TRUE;
}

dbus_bool_t
spi_dbus_message_iter_append_struct (DBusMessageIter *iter, ...)
{
  va_list args;
  DBusMessageIter iter_struct;
  int type;
  void *ptr;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL, &iter_struct))
    return FALSE;
  va_start (args, iter);
  for (;;)
    {
      type = va_arg (args, int);
      if (type == DBUS_TYPE_INVALID)
        break;
      ptr = va_arg (args, void *);
      dbus_message_iter_append_basic (&iter_struct, type, ptr);
    }
  va_end (args);
  if (!dbus_message_iter_close_container (iter, &iter_struct))
    return FALSE;
  return TRUE;
}

dbus_bool_t
spi_dbus_marshal_deviceEvent (DBusMessage *message, const AtspiDeviceEvent *e)
{
  DBusMessageIter iter;

  if (!message)
    return FALSE;
  dbus_message_iter_init_append (message, &iter);
  return spi_dbus_message_iter_append_struct (&iter, DBUS_TYPE_UINT32, &e->type, DBUS_TYPE_INT32, &e->id, DBUS_TYPE_INT16, &e->hw_code, DBUS_TYPE_INT16, &e->modifiers, DBUS_TYPE_INT32, &e->timestamp, DBUS_TYPE_STRING, &e->event_string, DBUS_TYPE_BOOLEAN, &e->is_text, DBUS_TYPE_INVALID);
}

dbus_bool_t
spi_dbus_demarshal_deviceEvent (DBusMessage *message, AtspiDeviceEvent *e)
{
  DBusMessageIter iter;

  dbus_message_iter_init (message, &iter);
  return spi_dbus_message_iter_get_struct (&iter, DBUS_TYPE_UINT32, &e->type, DBUS_TYPE_INT32, &e->id, DBUS_TYPE_INT16, &e->hw_code, DBUS_TYPE_INT16, &e->modifiers, DBUS_TYPE_INT32, &e->timestamp, DBUS_TYPE_STRING, &e->event_string, DBUS_TYPE_BOOLEAN, &e->is_text, DBUS_TYPE_INVALID);
}

/*
 * This is a rather annoying function needed to replace
 * NULL values of strings with the empty string. Null string
 * values can be created by the atk_object_get_name or text selection
 */
static const void *
provide_defaults (const gint type,
                  const void *val)
{
  switch (type)
    {
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
      if (!val)
        return "";
      else
        return val;
    default:
      return val;
    }
}

/*
 * Appends all the standard parameters to an AT-SPI event.
 */
void
spi_dbus_signal_new (const char *path,
                     const char *klass,
                     const char *major,
                     const char *minor,
                     dbus_int32_t detail1,
                     dbus_int32_t detail2)
{
  DBusMessage *sig;
  DBusMessageIter iter;
  gchar *cname, *t;

  if (!klass)
    klass = "";
  if (!major)
    major = "";
  if (!minor)
    minor = "";

  /*
   * This is very annoying, but as '-' isn't a legal signal
   * name in D-Bus (Why not??!?) The names need converting
   * on this side, and again on the client side.
   */
  cname = g_strdup (major);
  while ((t = strchr (cname, '-')) != NULL)
    *t = '_';

  sig = dbus_message_new_signal (path, klass, cname);
  g_free (cname);

  dbus_message_iter_init_append (sig, &iter);

  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &minor);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &detail1);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &detail2);
}

void
spi_dbus_emit_signal (DBusConnection *bus, const char *path, const char *klass, const char *major, const char *minor, dbus_int32_t detail1, dbus_int32_t detail2, const char *type, const void *val)
{
  gchar *cname, *t;
  DBusMessage *sig;
  DBusMessageIter iter, sub;
  if (!klass)
    klass = "";
  if (!major)
    major = "";
  if (!minor)
    minor = "";
  if (!type)
    type = "u";

  /*
   * This is very annoying, but as '-' isn't a legal signal
   * name in D-Bus (Why not??!?) The names need converting
   * on this side, and again on the client side.
   */
  cname = g_strdup (major);
  while ((t = strchr (cname, '-')) != NULL)
    *t = '_';

  sig = dbus_message_new_signal (path, klass, cname);
  g_free (cname);

  dbus_message_iter_init_append (sig, &iter);

  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &minor);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &detail1);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &detail2);

  dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT, type, &sub);
  /*
   * I need to convert the string signature to an integer type signature.
   * DBUS_TYPE_INT32 is defined as 'i' whereas the string is "i".
   * I should just be able to cast the first character of the string to an
   * integer.
   */
  val = provide_defaults ((int) *type, val);
  dbus_message_iter_append_basic (&sub, (int) *type, &val);
  dbus_message_iter_close_container (&iter, &sub);

  dbus_connection_send (bus, sig, NULL);
  dbus_message_unref (sig);
}

/* Returns an AtkObject from a DBusMessageIter and advances the iter.
 * Expects the iter to point to a (so) structure representing an object. */
GObject *
spi_dbus_get_object_from_iter (DBusMessageIter *iter)
{
  DBusMessageIter iter_struct;
  const char *bus_name, *path;

  dbus_message_iter_recurse (iter, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &bus_name);
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &path);
  dbus_message_iter_next (iter);
  /* TODO: verify bus name */
  return spi_global_register_path_to_object (path);
}
