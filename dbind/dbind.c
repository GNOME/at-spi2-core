/*
 * Copyright 2008-2011 Novell, Inc.
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

#include <glib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "atspi/atspi-gmain.h"
#include "config.h"
#include "dbind/dbind.h"

static int dbind_timeout = -1;

/*
 * FIXME: compare types - to ensure they match &
 *        do dynamic padding of structures etc.
 */

/*---------------------------------------------------------------------------*/

typedef struct _SpiReentrantCallClosure
{
  DBusMessage *reply;
} SpiReentrantCallClosure;

static void
set_reply (DBusPendingCall *pending, void *user_data)
{
  SpiReentrantCallClosure *closure = (SpiReentrantCallClosure *) user_data;

  closure->reply = dbus_pending_call_steal_reply (pending);
  dbus_pending_call_unref (pending);
}

static gint
time_elapsed (struct timeval *origin)
{
  struct timeval tv;

  gettimeofday (&tv, NULL);
  return (tv.tv_sec - origin->tv_sec) * 1000 + (tv.tv_usec - origin->tv_usec) / 1000;
}

DBusMessage *
dbind_send_and_allow_reentry (DBusConnection *bus, DBusMessage *message, DBusError *error)
{
  DBusPendingCall *pending;
  SpiReentrantCallClosure *closure;
  const char *unique_name = dbus_bus_get_unique_name (bus);
  const char *destination = dbus_message_get_destination (message);
  struct timeval tv;
  DBusMessage *ret;
  static gboolean in_dispatch = FALSE;

  if (unique_name && destination &&
      strcmp (destination, unique_name) != 0)
    {
      ret = dbus_connection_send_with_reply_and_block (bus, message,
                                                       dbind_timeout, error);
      if (g_main_depth () == 0 && !in_dispatch)
        {
          in_dispatch = TRUE;
          while (dbus_connection_dispatch (bus) == DBUS_DISPATCH_DATA_REMAINS)
            ;
          in_dispatch = FALSE;
        }
      return ret;
    }

  closure = g_new0 (SpiReentrantCallClosure, 1);
  closure->reply = NULL;
  if (!dbus_connection_send_with_reply (bus, message, &pending, dbind_timeout) || !pending)
    {
      g_free (closure);
      return NULL;
    }
  dbus_pending_call_set_notify (pending, set_reply, (void *) closure, g_free);

  closure->reply = NULL;
  gettimeofday (&tv, NULL);
  dbus_pending_call_ref (pending);
  while (!closure->reply)
    {
      if (!dbus_connection_read_write_dispatch (bus, dbind_timeout))
        {
          // dbus_pending_call_set_notify (pending, NULL, NULL, NULL);
          dbus_pending_call_cancel (pending);
          dbus_pending_call_unref (pending);
          return NULL;
        }
      if (time_elapsed (&tv) > dbind_timeout)
        {
          // dbus_pending_call_set_notify (pending, NULL, NULL, NULL);
          dbus_pending_call_cancel (pending);
          dbus_pending_call_unref (pending);
          dbus_set_error_const (error, "org.freedesktop.DBus.Error.NoReply",
                                "timeout from dbind");
          return NULL;
        }
    }

  ret = closure->reply;
  dbus_pending_call_unref (pending);
  return ret;
}

dbus_bool_t
dbind_method_call_reentrant_va (DBusConnection *cnx,
                                const char *bus_name,
                                const char *path,
                                const char *interface,
                                const char *method,
                                DBusError *opt_error,
                                const char *arg_types,
                                va_list args)
{
  dbus_bool_t success = FALSE;
  DBusMessage *msg = NULL, *reply = NULL;
  DBusMessageIter iter;
  DBusError *err, real_err;
  const char *p;
  va_list args_demarshal;

  dbus_error_init (&real_err);

  va_copy (args_demarshal, args);
  if (opt_error)
    err = opt_error;
  else
    {
      err = &real_err;
    }

  msg = dbus_message_new_method_call (bus_name, path, interface, method);
  if (!msg)
    goto out;

  p = arg_types;
  dbus_message_iter_init_append (msg, &iter);
  dbind_any_marshal_va (&iter, &p, args);

  reply = dbind_send_and_allow_reentry (cnx, msg, err);
  if (!reply)
    goto out;

  if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR)
    {
      goto out;
    }
  /* demarshal */
  if (p[0] == '=' && p[1] == '>')
    {
      DBusMessageIter iter;
      dbus_message_iter_init (reply, &iter);
      if (strcmp (p + 2, dbus_message_get_signature (reply)) != 0)
        {
          g_warning ("dbind: Call to \"%s\" returned signature %s; expected %s",
                     method, dbus_message_get_signature (reply), p + 2);
          if (opt_error)
            dbus_set_error (opt_error, DBUS_ERROR_INVALID_ARGS,
                            "Call to \"%s\" returned signature %s; expected %s",
                            method, dbus_message_get_signature (reply),
                            p + 2);
          goto out;
        }
      p = arg_types;
      dbind_any_demarshal_va (&iter, &p, args_demarshal);
    }

  success = TRUE;
out:
  if (msg)
    dbus_message_unref (msg);

  if (reply)
    dbus_message_unref (reply);

  if (dbus_error_is_set (&real_err))
    dbus_error_free (&real_err);

  va_end (args_demarshal);
  return success;
}

/**
 * dbind_method_call_reentrant:
 *
 * @cnx:       A D-Bus Connection used to make the method call.
 * @bus_name:  The D-Bus bus name of the program where the method call should
 *             be made.
 * @path:      The D-Bus object path that should handle the method.
 * @interface: The D-Bus interface used to scope the method name.
 * @method:    Method to be invoked.
 * @opt_error: D-Bus error.
 * @arg_types: Variable length arguments interleaving D-Bus argument types
 *             and pointers to argument data.
 *
 * Makes a D-Bus method call using the supplied location data, method name and
 * argument data. This function is re-entrant. It continuously reads from the D-Bus
 * bus and dispatches messages until a reply has been received.
 **/
dbus_bool_t
dbind_method_call_reentrant (DBusConnection *cnx,
                             const char *bus_name,
                             const char *path,
                             const char *interface,
                             const char *method,
                             DBusError *opt_error,
                             const char *arg_types,
                             ...)
{
  dbus_bool_t success = FALSE;
  va_list args;

  va_start (args, arg_types);
  success = dbind_method_call_reentrant_va (cnx,
                                            bus_name,
                                            path,
                                            interface,
                                            method,
                                            opt_error,
                                            arg_types,
                                            args);
  va_end (args);

  return success;
}

dbus_bool_t
dbind_method_call_va (DBusConnection *cnx,
                      const char *bus_name,
                      const char *path,
                      const char *interface,
                      const char *method,
                      const char *arg_types,
                      va_list args)
{
  dbus_bool_t success = FALSE;
  DBusMessage *msg = NULL;
  DBusMessageIter iter;
  const char *p;
  va_list args_demarshal;

  va_copy (args_demarshal, args);

  msg = dbus_message_new_method_call (bus_name, path, interface, method);
  if (!msg)
    return FALSE;

  p = arg_types;
  dbus_message_iter_init_append (msg, &iter);
  dbind_any_marshal_va (&iter, &p, args);

  dbus_connection_send (cnx, msg, NULL);
  dbus_message_unref (msg);

  va_end (args_demarshal);
  return success;
}

/**
 * dbind_method_call:
 *
 * @cnx:       A D-Bus Connection used to make the method call.
 * @bus_name:  The D-Bus bus name of the program where the method call should
 *             be made.
 * @path:      The D-Bus object path that should handle the method.
 * @interface: The D-Bus interface used to scope the method name.
 * @method:    Method to be invoked.
 * @arg_types: Variable length arguments interleaving D-Bus argument types
 *             and pointers to argument data.
 *
 * Sends a D-Bus method call using the supplied location data, method name and
 * argument data, without waiting for a re ply.
 **/
dbus_bool_t
dbind_method_call (DBusConnection *cnx,
                   const char *bus_name,
                   const char *path,
                   const char *interface,
                   const char *method,
                   const char *arg_types,
                   ...)
{
  dbus_bool_t success = FALSE;
  va_list args;

  va_start (args, arg_types);
  success = dbind_method_call_va (cnx,
                                  bus_name,
                                  path,
                                  interface,
                                  method,
                                  arg_types,
                                  args);
  va_end (args);

  return success;
}

void
dbind_set_timeout (int timeout)
{
  dbind_timeout = timeout;
}

/*END------------------------------------------------------------------------*/
