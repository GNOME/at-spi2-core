/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * LoginHelper interface 
 * Copyright 2004 Sun Microsystems Inc.,
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

/* login-helper.c: minimal implementation of Accessibility_LoginHelper.idl */
#include <config.h>
#include "login-helper.h"
#include "spi-common/spi-dbus.h"
#include "droute/droute.h"

/* Our parent Gtk object type */
#define PARENT_TYPE G_TYPE_OBJECT

/* A pointer to our parent object class */
static GObjectClass *g_object_parent_class;

static void
login_helper_finalize (GObject *object)
{
  (G_OBJECT_CLASS (g_object_parent_class))->finalize (object);
}

static gboolean
login_helper_set_safe (LoginHelper *helper, gboolean safe)
{
  LoginHelperClass *klass = LOGIN_HELPER_GET_CLASS (helper);

  if (klass->set_safe)
    return (* klass->set_safe)(helper, safe);
  else
    return FALSE;
}

static DBusMessage *
impl_set_safe (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  LoginHelper *helper = user_data;

  DBusError error;
  dbus_bool_t safe;
  dbus_bool_t rv;
  DBusMessage *reply;

  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_BOOLEAN, &safe, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  rv = login_helper_set_safe (helper, safe);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv, DBUS_TYPE_INVALID);
    }
  return reply;
}

static LoginHelperDeviceReqFlags
login_helper_get_device_reqs (LoginHelper *helper)
{
  LoginHelperClass *klass = LOGIN_HELPER_GET_CLASS (helper);

  if (klass->get_device_reqs)
    return  (* klass->get_device_reqs)(helper);
  else
    return 0;
}

static DBusMessage *
impl_get_device_reqs (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  LoginHelper *helper = user_data;
  dbus_uint32_t flags = 0;
  DBusMessage *reply;

  flags = login_helper_get_device_reqs (helper);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_UINT32, &flags, DBUS_TYPE_INVALID);
    }
  return reply;
}

static Window*
login_helper_get_raise_windows (LoginHelper *helper)
{
  LoginHelperClass *klass = LOGIN_HELPER_GET_CLASS (helper);
  
  if (klass->get_raise_windows)
    return (* klass->get_raise_windows)(helper);
  else
    return NULL;
}

static DBusMessage *
impl_get_raise_windows (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  LoginHelper *helper = user_data;
  unsigned long *wids;
  gint count = 0;
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;

  wids = login_helper_get_raise_windows (helper);
  reply = dbus_message_new_method_return (message);
  if (!reply) return NULL;
  dbus_message_iter_init_append (message, &iter);
  if (dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "i", &iter_array) && wids)
  {
    while (*wids)
    {
      // TODO: figure out if this will break on 64-bit systems
      dbus_int32_t id = *wids++;
      dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_INT32, &id);
    }
    dbus_message_iter_close_container (&iter, &iter_array);
  }
  if (wids) g_free (wids);
  return reply;
}

static void
login_helper_class_init (LoginHelperClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;

  g_object_parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = login_helper_finalize;
}

static void
login_helper_init (LoginHelper *object)
{
}

G_DEFINE_TYPE (LoginHelper, login_helper, PARENT_TYPE)
