

#include <stdio.h>
#include <stdarg.h>
#include <glib.h>

#include "config.h"
#include "dbind/dbind.h"

/*
 * FIXME: compare types - to ensure they match &
 *        do dynamic padding of structures etc.
 */

/*---------------------------------------------------------------------------*/

static void
set_reply (DBusPendingCall *pending, void *user_data)
{
    void **replyptr = (void **)user_data;

    *replyptr = dbus_pending_call_steal_reply (pending);
}

static DBusMessage *
send_and_allow_reentry (DBusConnection *bus, DBusMessage *message, DBusError *error)
{
    DBusPendingCall *pending;
    DBusMessage *reply = NULL;

    if (!dbus_connection_send_with_reply (bus, message, &pending, -1))
    {
        return NULL;
    }
    dbus_pending_call_set_notify (pending, set_reply, (void *)&reply, NULL);
    while (!reply)
    {
      if (!dbus_connection_read_write_dispatch (bus, -1)) return NULL;
    }
    return reply;
}

dbus_bool_t
dbind_method_call_reentrant_va (DBusConnection *cnx,
                                const char     *bus_name,
                                const char     *path,
                                const char     *interface,
                                const char     *method,
                                DBusError      *opt_error,
                                const char     *arg_types,
                                va_list         args)
{
    dbus_bool_t success = FALSE;
    DBusMessage *msg = NULL, *reply = NULL;
    DBusMessageIter iter;
    DBusError *err, real_err;
    const char *p;

    if (opt_error)
        err = opt_error;
    else {
        dbus_error_init (&real_err);
        err = &real_err;
    }

    msg = dbus_message_new_method_call (bus_name, path, interface, method);
    if (!msg)
        goto out;

    p = arg_types;
    dbus_message_iter_init_append (msg, &iter);
    dbind_any_marshal_va (&iter, &p, args);

    reply = send_and_allow_reentry (cnx, msg, err);
    if (!reply)
        goto out;

    if (dbus_message_get_type (reply) == DBUS_MESSAGE_TYPE_ERROR)
    {
      const char *name = dbus_message_get_error_name (reply);
      dbus_set_error (err, name, g_strdup (""));
      goto out;
    }
    /* demarshal */
    if (p[0] == '=' && p[1] == '>')
    {
        DBusMessageIter iter;
        p += 2;
        dbus_message_iter_init (reply, &iter);
        dbind_any_demarshal_va (&iter, &p, args);
    }

    success = TRUE;
out:
    if (msg)
        dbus_message_unref (msg);

    if (reply)
        dbus_message_unref (reply);

    if (err == &real_err)
        dbus_error_free (err);

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
 * argument data.This function is re-entrant. It continuously reads from the D-Bus
 * bus and dispatches messages until a reply has been recieved.
 **/
dbus_bool_t
dbind_method_call_reentrant (DBusConnection *cnx,
                             const char     *bus_name,
                             const char     *path,
                             const char     *interface,
                             const char     *method,
                             DBusError      *opt_error,
                             const char     *arg_types,
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

/*---------------------------------------------------------------------------*/

dbus_bool_t
dbind_emit_signal_va (DBusConnection *cnx,
                      const char     *path,
                      const char     *interface,
                      const char     *signal,
                      DBusError      *opt_error,
                      const char     *arg_types,
                      va_list         args)
{
    dbus_bool_t success = FALSE;
    DBusMessage *msg = NULL;
    DBusMessageIter iter;
    DBusError *err, real_err;
    const char *p;

    if (opt_error)
        err = opt_error;
    else {
        dbus_error_init (&real_err);
        err = &real_err;
    }

    msg = dbus_message_new_signal (path, interface, signal);
    if (!msg)
        goto out;

    p = arg_types;
    dbus_message_iter_init_append (msg, &iter);
    dbind_any_marshal_va (&iter, &p, args);

    if (!dbus_connection_send (cnx, msg, NULL))
       goto out;

    success = TRUE;
out:

    if (msg)
        dbus_message_unref (msg);

    if (err == &real_err)
        dbus_error_free (err);

    return success;
}

/**
 * dbind_emit_signal:
 *
 * @cnx:       A D-Bus Connection used to make the method call.
 * @path:      The D-Bus object path that this signal is emitted from.
 * @interface: The D-Bus interface used to scope the method name.
 * @signal:    Name of signal to emit.
 * @opt_error: D-Bus error.
 * @arg_types: Variable length arguments interleaving D-Bus argument types
 *             and pointers to argument data.
 *
 * Emits a D-Bus signal  using the supplied signal name and argument data.
 **/
dbus_bool_t
dbind_emit_signal (DBusConnection *cnx,
                   const char     *path,
                   const char     *interface,
                   const char     *signal,
                   DBusError      *opt_error,
                   const char     *arg_types,
                   ...)
{
    dbus_bool_t success = FALSE;
    va_list args;

    va_start (args, arg_types);
    success = dbind_emit_signal_va (cnx, path, interface, signal, opt_error, arg_types, args);
    va_end (args);

    return success;
}

/*END------------------------------------------------------------------------*/
