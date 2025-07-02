/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008 Codethink Ltd.
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

#pragma once

#include <dbus/dbus.h>
#include <glib.h>

#include <droute/droute-variant.h>

typedef DBusMessage *(*DRouteFunction) (DBusConnection *, DBusMessage *, void *);
typedef dbus_bool_t (*DRoutePropertyFunction) (DBusMessageIter *, void *);
typedef gchar *(*DRouteIntrospectChildrenFunction) (const char *, void *);

typedef void *(*DRouteGetDatumFunction) (const char *, void *);
typedef gboolean (*DRouteQueryInterfaceFunction) (void *, const char *);

typedef struct _DRouteMethod DRouteMethod;
struct _DRouteMethod
{
  DRouteFunction func;
  const char *name;
};

typedef struct _DRouteProperty DRouteProperty;
struct _DRouteProperty
{
  DRoutePropertyFunction get;
  DRoutePropertyFunction set;
  const char *name;
};

/*---------------------------------------------------------------------------*/

typedef struct _DRouteContext DRouteContext;

typedef struct _DRoutePath DRoutePath;

/*---------------------------------------------------------------------------*/

DRouteContext *
droute_new ();

void
droute_free (DRouteContext *cnx);

DRoutePath *
droute_add_one (DRouteContext *cnx,
                const char *path,
                const void *data);

DRoutePath *
droute_add_many (DRouteContext *cnx,
                 const char *path,
                 const void *data,
                 DRouteIntrospectChildrenFunction introspect_children_cb,
                 void *introspect_children_data,
                 const DRouteGetDatumFunction get_datum,
                 const DRouteQueryInterfaceFunction query_interface_cb);

void
droute_path_add_interface (DRoutePath *path,
                           const char *name,
                           const char *introspect,
                           const DRouteMethod *methods,
                           const DRouteProperty *properties);

DBusMessage *
droute_not_yet_handled_error (DBusMessage *message);

DBusMessage *
droute_invalid_arguments_error (DBusMessage *message);

DBusMessage *
droute_out_of_memory_error (DBusMessage *message);

void
droute_path_register (DRoutePath *path, DBusConnection *bus);

void
droute_path_unregister (DRoutePath *path, DBusConnection *bus);

void
droute_context_register (DRouteContext *cnx, DBusConnection *bus);

void
droute_context_unregister (DRouteContext *cnx, DBusConnection *bus);

void
droute_intercept_dbus (DBusConnection *connection);

void
droute_unintercept_dbus (DBusConnection *connection);
