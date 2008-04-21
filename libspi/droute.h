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
#ifndef _DROUTE_H
#define _DROUTE_H

#include <dbus/dbus.h>
#include "glib.h"	/* needed for GString */

#define DROUTE_METHOD   0
#define DROUTE_SIGNAL   1

typedef DBusMessage *(*DRouteFunction)(DBusConnection *, DBusMessage *, void *);
typedef dbus_bool_t (*DRoutePropertyFunction)(const char *, DBusMessageIter *, void *);
typedef char *(*DRoutePropertyStrFunction)(void *);

typedef struct _DRouteMethod DRouteMethod;
struct _DRouteMethod
{
  /* DROUTE_(METHOD|SIGNAL) */
  int type;
  DRouteFunction func;
  const char *name;
  /* arg_desc contains argument information used for introspection.
   * It is a colon-delimited string of type,name,dir values */
  const char *arg_desc;
  dbus_bool_t wants_droute_data;
};

typedef struct _DRouteProperty DRouteProperty;
struct _DRouteProperty
{
  DRoutePropertyFunction get;
  DRoutePropertyStrFunction get_str;
  DRoutePropertyFunction set;
  DRoutePropertyStrFunction set_str;
  const char *name;
  const char *type;
};

  typedef void *(*DRouteGetDatumFunction)(const char *, void *);
  typedef void (*DRouteFreeDatumFunction)(void *);

typedef struct _DRouteInterface DRouteInterface;
struct _DRouteInterface
{
  DRouteMethod *methods;
  DRouteProperty *properties;
  DRouteGetDatumFunction get_datum;
  DRouteFreeDatumFunction free_datum;
  char *name;
};

typedef struct _DRouteData DRouteData;
struct _DRouteData
{
  GSList *interfaces;
  char (*introspect_children)(const char *, GString *, void *);
  void *user_data;
};

DBusHandlerResult droute_message(DBusConnection *bus, DBusMessage *message, void *user_data);

dbus_bool_t droute_return_v_int32(DBusMessageIter *iter, dbus_int32_t val);
dbus_bool_t droute_return_v_double(DBusMessageIter *iter, double val);
dbus_bool_t droute_return_v_string(DBusMessageIter *iter, const char *val);
dbus_int32_t droute_get_v_int32(DBusMessageIter *iter);
const char *droute_get_v_string(DBusMessageIter *iter);
dbus_bool_t droute_return_v_object(DBusMessageIter *iter, const char *path);

dbus_bool_t droute_add_interface(DRouteData *data, const char *name, DRouteMethod *methods, DRouteProperty *properties, DRouteGetDatumFunction get_datum, DRouteFreeDatumFunction free_datum);
#endif	/* _DROUTE_H */
