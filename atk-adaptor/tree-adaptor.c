/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2008, 2009 Codethink Ltd.
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

#include <string.h>

#include <atk/atk.h>
#include <droute/droute.h>

#include "bridge.h"
#include "accessible-register.h"
#include "accessible-marshaller.h"
#include "spi-common/spi-dbus.h"

/*---------------------------------------------------------------------------*/

/* For use as a GHFunc */
static void
append_accessible_hf (gpointer key, gpointer obj_data, gpointer iter)
{
  spi_atk_append_accessible (ATK_OBJECT(obj_data), iter);
}

/*---------------------------------------------------------------------------*/

void
spi_emit_cache_removal (guint ref,  DBusConnection *bus)
{
  DBusMessage *message;
  DBusMessageIter iter;
  gchar *path;

  message = dbus_message_new_signal ("/org/freedesktop/atspi/tree", SPI_DBUS_INTERFACE_TREE, "removeAccessible");

  dbus_message_iter_init_append (message, &iter);

  path = atk_dbus_ref_to_path (ref);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &path);

  dbus_connection_send(bus, message, NULL);
}

void
spi_emit_cache_update (AtkObject *accessible, DBusConnection *bus)
{
   DBusMessage *message;
   DBusMessageIter iter;
   message = dbus_message_new_signal ("/org/freedesktop/atspi/tree", SPI_DBUS_INTERFACE_TREE, "updateAccessible");

   dbus_message_iter_init_append (message, &iter);
   spi_atk_append_accessible (accessible, &iter);

   dbus_connection_send(bus, message, NULL);
}


/*---------------------------------------------------------------------------*/

static DBusMessage *
impl_getRoot (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkObject *root = atk_get_root();
  char *path;
  DBusMessage *reply;

  if (!root)
      return spi_dbus_general_error (message);
  path = atk_dbus_object_to_path (root);
  if (!path)
      return spi_dbus_general_error (message);
  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_OBJECT_PATH, &path, DBUS_TYPE_INVALID);
  g_free (path);
  return reply;
}

/*---------------------------------------------------------------------------*/

static DBusMessage *
impl_getTree (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;

  reply = dbus_message_new_method_return (message);

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "(ooaoassusau)", &iter_array);
  atk_dbus_foreach_registered(append_accessible_hf, &iter_array);
  dbus_message_iter_close_container(&iter, &iter_array);
  return reply;
}

/*---------------------------------------------------------------------------*/

static DRouteMethod methods[] = {
  {impl_getRoot, "getRoot"},
  {impl_getTree, "getTree"},
  {NULL, NULL}
};

void
spi_initialize_tree (DRoutePath *path)
{
  droute_path_add_interface (path,
                             SPI_DBUS_INTERFACE_TREE,
                             methods,
                             NULL);
};

/*END------------------------------------------------------------------------*/
