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
#include "accessible.h"


static dbus_bool_t
impl_get_toolkitName (const char *path, DBusMessageIter * iter,
		      void *user_data)
{
  return droute_return_v_string (iter, atk_get_toolkit_name ());
}

static char *
impl_get_toolkitName_str (void *datum)
{
  return g_strdup (atk_get_toolkit_name ());
}

static dbus_bool_t
impl_get_version (const char *path, DBusMessageIter * iter, void *user_data)
{
  return droute_return_v_string (iter, atk_get_toolkit_version ());
}

static char *
impl_get_version_str (void *datum)
{
  return g_strdup (atk_get_toolkit_version ());
}

static dbus_int32_t id;

static dbus_bool_t
impl_get_id (const char *path, DBusMessageIter * iter, void *user_data)
{
  return droute_return_v_int32 (iter, id);
}

static char *
impl_get_id_str (void *datum)
{
  return g_strdup_printf ("%d", id);
}

static dbus_bool_t
impl_set_id (const char *path, DBusMessageIter * iter, void *user_data)
{
  id = droute_get_v_int32 (iter);
  return TRUE;
}

static DBusMessage *
impl_registerToolkitEventListener (DBusConnection * bus,
				   DBusMessage * message, void *user_data)
{
}

static DBusMessage *
impl_registerObjectEventListener (DBusConnection * bus, DBusMessage * message,
				  void *user_data)
{
}

static DBusMessage *
impl_pause (DBusConnection * bus, DBusMessage * message, void *user_data)
{
}

static DBusMessage *
impl_resume (DBusConnection * bus, DBusMessage * message, void *user_data)
{
}

static DBusMessage *
impl_getLocale (DBusConnection * bus, DBusMessage * message, void *user_data)
{
}

static DRouteMethod methods[] = {
  {DROUTE_METHOD, impl_registerToolkitEventListener,
   "registerToolkitEventListener", "o,listener,i:s,eventName,i"},
  {DROUTE_METHOD, impl_registerObjectEventListener,
   "registerObjectEventListener", "o,listener,i:s,eventName,i"},
  {DROUTE_METHOD, impl_pause, "pause", "b,,o"},
  {DROUTE_METHOD, impl_resume, "resume", "b,,o"},
  {DROUTE_METHOD, impl_getLocale, "getLocale", "u,lctype,i:s,,o"},
  {0, NULL, NULL, NULL}
};

static DRouteProperty properties[] = {
  {impl_get_toolkitName, impl_get_toolkitName_str, NULL, NULL, "toolkitName",
   "s"},
  {impl_get_version, impl_get_version_str, NULL, NULL, "version", "s"},
  {impl_get_id, impl_get_id_str, impl_set_id, NULL, "id", "i"},
  {NULL, NULL, NULL, NULL, NULL, NULL}
};

static long
obj_is_root (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  return (obj == atk_get_root ());
}

void
spi_initialize_application (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.accessibility.Application",
			methods, properties,
			(DRouteGetDatumFunction) obj_is_root, NULL);
};
