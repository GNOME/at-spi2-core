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

#include <atk/atk.h>
#include <droute/droute.h>

#include "common/spi-dbus.h"

static dbus_bool_t
impl_get_toolkitName (DBusMessageIter * iter,
		      void *user_data)
{
  return droute_return_v_string (iter, atk_get_toolkit_name ());
}

static dbus_bool_t
impl_get_version (DBusMessageIter * iter, void *user_data)
{
  return droute_return_v_string (iter, atk_get_toolkit_version ());
}

static dbus_int32_t id;

static dbus_bool_t
impl_get_id (DBusMessageIter * iter, void *user_data)
{
  return droute_return_v_int32 (iter, id);
}

static dbus_bool_t
impl_set_id (DBusMessageIter * iter, void *user_data)
{
  id = droute_get_v_int32 (iter);
  return TRUE;
}

static DBusMessage *
impl_registerToolkitEventListener (DBusConnection * bus,
				   DBusMessage * message, void *user_data)
{
  return NULL;
}

static DBusMessage *
impl_registerObjectEventListener (DBusConnection * bus, DBusMessage * message,
				  void *user_data)
{
  return NULL;
}

static DBusMessage *
impl_pause (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  return NULL;
}

static DBusMessage *
impl_resume (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  return NULL;
}

static DBusMessage *
impl_getLocale (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  return NULL;
}

static DRouteMethod methods[] = {
  {impl_registerToolkitEventListener, "registerToolkitEventListener"},
  {impl_registerObjectEventListener, "registerObjectEventListener"},
  {impl_pause, "pause"},
  {impl_resume, "resume"},
  {impl_getLocale, "getLocale"},
  {NULL, NULL}
};

static DRouteProperty properties[] = {
  {impl_get_toolkitName, NULL, "toolkitName"},
  {impl_get_version, NULL, "version"},
  {impl_get_id, impl_set_id, "id"},
  {NULL, NULL, NULL}
};

/*static long
obj_is_root (const char *path, void *user_data)
{
  AtkObject *obj = atk_dbus_get_object (path);
  return (obj == atk_get_root ());
}*/

void
spi_initialize_application (DRoutePath *path)
{
  droute_path_add_interface (path,
                             SPI_DBUS_INTERFACE_APPLICATION,
                             methods,
                             properties);
};
