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

#ifndef _DBIND_H_
#define _DBIND_H_

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbind/dbind-any.h>
#include <dbus/dbus.h>

DBusMessage *
dbind_send_and_allow_reentry (DBusConnection *bus, DBusMessage *message, DBusError *error);

dbus_bool_t
dbind_method_call_reentrant_va (DBusConnection *cnx,
                                const char *bus_name,
                                const char *path,
                                const char *interface,
                                const char *method,
                                DBusError *opt_error,
                                const char *arg_types,
                                va_list args);

dbus_bool_t
dbind_method_call_reentrant (DBusConnection *cnx,
                             const char *bus_name,
                             const char *path,
                             const char *interface,
                             const char *method,
                             DBusError *opt_error,
                             const char *arg_types,
                             ...);

dbus_bool_t
dbind_method_call_va (DBusConnection *cnx,
                      const char *bus_name,
                      const char *path,
                      const char *interface,
                      const char *method,
                      const char *arg_types,
                      va_list args);

dbus_bool_t
dbind_method_call (DBusConnection *cnx,
                   const char *bus_name,
                   const char *path,
                   const char *interface,
                   const char *method,
                   const char *arg_types,
                   ...);

void dbind_set_timeout (int timeout);
#endif /* _DBIND_H_ */
