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

#define get_object(message) spi_dbus_get_object(dbus_message_get_path(message))

static dbus_bool_t
spi_dbus_append_tree_helper (DBusMessageIter * iter_array, AtkObject * obj,
			     DRouteData * data)
{
  DBusMessageIter iter_struct, iter_sub_array;
  char *path = NULL;
  char *path_parent;
  const char *name, *desc;
  dbus_uint16_t updating = 1;
  int i;
  dbus_uint32_t role;

  gint childcount;
  GSList *l;

  dbus_message_iter_open_container (iter_array, DBUS_TYPE_STRUCT, NULL,
				    &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT16, &updating);
  path = spi_dbus_get_path (obj);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  path_parent = spi_dbus_get_path (atk_object_get_parent(obj));
  if (!path_parent) path_parent = g_strdup("/");
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path_parent);
  g_free(path_parent);
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "o",
				    &iter_sub_array);
  childcount = atk_object_get_n_accessible_children (obj);
  for (i = 0; i < childcount; i++)
    {
      AtkObject *child = atk_object_ref_accessible_child (obj, i);
      char *child_path = spi_dbus_get_path (child);
      if (child_path)
	{
	  dbus_message_iter_append_basic (&iter_sub_array,
					  DBUS_TYPE_OBJECT_PATH, &child_path);
	  g_free (child_path);
	}
      if (child)
	g_object_unref (child);
    }
  if (!dbus_message_iter_close_container (&iter_struct, &iter_sub_array))
    goto oom;
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "s",
				    &iter_sub_array);
  for (l = data->interfaces; l; l = g_slist_next (l))
    {
      DRouteInterface *iface_def = (DRouteInterface *) l->data;
      void *datum = NULL;
      if (iface_def->get_datum)
	{
	  datum = (*iface_def->get_datum) (path, data->user_data);
	  if (!datum)
	    continue;
	}
      dbus_message_iter_append_basic (&iter_sub_array, DBUS_TYPE_STRING,
				      &iface_def->name);
      if (iface_def->free_datum)
	(*iface_def->free_datum) (datum);
    }
  if (!dbus_message_iter_close_container (&iter_struct, &iter_sub_array))
    goto oom;
  name = atk_object_get_name (obj);
  if (!name)
    name = "";
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);
  role = spi_accessible_role_from_atk_role (atk_object_get_role (obj));
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &role);
  desc = atk_object_get_description (obj);
  if (!desc)
    desc = "";
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &desc);
  if (!dbus_message_iter_close_container (iter_array, &iter_struct))
    goto oom;
  for (i = 0; i < childcount; i++)
    {
      AtkObject *child = atk_object_ref_accessible_child (obj, i);
      dbus_bool_t result;
      if (!child)
	continue;
      result = spi_dbus_append_tree_helper (iter_array, child, data);
      g_object_unref (child);
      if (!result)
	goto oom;
    }
  g_free (path);
  return TRUE;
oom:
  if (path) g_free(path);
  return FALSE;
}

dbus_bool_t
spi_dbus_append_tree (DBusMessage * message, AtkObject * obj,
		      DRouteData * data)
{
  DBusMessageIter iter, iter_array;
  dbus_bool_t result;

  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(qooaoassus)",
				    &iter_array);
  result = spi_dbus_append_tree_helper (&iter_array, obj, data);
  if (result)
    result = dbus_message_iter_close_container (&iter, &iter_array);
  return result;
}

static DBusMessage *
impl_getRoot (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkObject *root = atk_get_root();
  char *path;
  DBusMessage *reply;

  if (root) path = spi_dbus_get_path(root);
  if (!root || !path)
    return spi_dbus_general_error (message);
  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_OBJECT_PATH, &path,
			    DBUS_TYPE_INVALID);
  g_free (path);
  return reply;
}

static DBusMessage *
impl_getTree (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  DBusMessage *reply;
  AtkObject *root = atk_get_root();

  if (!root) return spi_dbus_general_error(message);
  reply = dbus_message_new_method_return (message);
  spi_dbus_append_tree (reply, root, (DRouteData *) user_data);
  return reply;
}

static DRouteMethod methods[] = {
  {DROUTE_METHOD, impl_getRoot, "getRoot", "o,root,o" },
  {DROUTE_METHOD, impl_getTree, "getTree", "a(qooaoassus),tree,o", TRUE},
  {0, NULL, NULL, NULL}
};

void
spi_initialize_tree (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Tree",
			methods, NULL, NULL, NULL);
};
