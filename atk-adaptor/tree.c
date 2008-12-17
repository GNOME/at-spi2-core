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

#include <string.h>

#include <atk/atk.h>
#include <droute/droute.h>

#include "atk-dbus.h"
#include "spi-common/spi-dbus.h"
#include "bridge.h"

static gboolean update_pending = FALSE;

/*---------------------------------------------------------------------------*/

static void
append_atk_object_interfaces (AtkObject *object, DBusMessageIter *iter)
{
  const gchar *itf;

  itf = SPI_DBUS_INTERFACE_ACCESSIBLE;
  dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);

  if (ATK_IS_ACTION (object))
    {
      itf = SPI_DBUS_INTERFACE_ACTION;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_COMPONENT (object))
    {
      itf = SPI_DBUS_INTERFACE_COMPONENT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_EDITABLE_TEXT (object))
    {
      itf = SPI_DBUS_INTERFACE_EDITABLE_TEXT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_TEXT (object))
    {
      itf = SPI_DBUS_INTERFACE_TEXT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_HYPERTEXT (object))
    {
      itf = SPI_DBUS_INTERFACE_HYPERTEXT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_IMAGE (object))
    {
      itf = SPI_DBUS_INTERFACE_IMAGE;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_SELECTION (object))
    {
      itf = SPI_DBUS_INTERFACE_SELECTION;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_TABLE (object))
    {
      itf = SPI_DBUS_INTERFACE_TABLE;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_VALUE (object))
    {
      itf = SPI_DBUS_INTERFACE_VALUE;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_STREAMABLE_CONTENT (object))
    {
      itf = "org.freedesktop.atspi.StreamableContent";
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_DOCUMENT (object))
    {
      itf = "org.freedesktop.atspi.Collection";
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
      itf = SPI_DBUS_INTERFACE_DOCUMENT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_HYPERLINK_IMPL (object))
    {
      itf = SPI_DBUS_INTERFACE_HYPERLINK;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }
}

/*---------------------------------------------------------------------------*/

static const char *dumm = "/APath/1";

/*
 * Marshals the given AtkObject into the provided D-Bus iterator.
 *
 * The object is marshalled including all its client side cache data.
 * The format of the structure is (ooaoassusau).
 * This is used in the updateTree signal and the getTree method
 * of the org.freedesktop.atspi.Tree interface.
 */
static void
append_accessible(gpointer ref, gpointer obj_data, gpointer iter)
{
  AtkObject *obj;
  DBusMessageIter *iter_array;
  DBusMessageIter iter_struct, iter_sub_array;
  dbus_int32_t states [2];
  int count;

  const char *name, *desc;
  int i;
  dbus_uint32_t role;
  GSList *l;

  obj = ATK_OBJECT(obj_data);
  iter_array = (DBusMessageIter *) iter;

  dbus_message_iter_open_container (iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct);
    {
      AtkObject *parent;
      gchar *path, *path_parent;

      path = atk_dbus_get_path_from_ref(GPOINTER_TO_INT(ref));
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);

      parent = atk_object_get_parent(obj);
      if (parent == NULL)
        path_parent = g_strdup("/");
      else
        path_parent = atk_dbus_get_path (parent);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path_parent);
      g_free(path_parent);

      dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "o", &iter_sub_array);
        {
          gint childcount, i;

          childcount = atk_object_get_n_accessible_children (obj);
          for (i = 0; i < childcount; i++)
            {
              AtkObject *child;
              gchar *child_path;

              child = atk_object_ref_accessible_child (obj, i);
              child_path = atk_dbus_get_path (child);
              g_object_unref(G_OBJECT(child));
              dbus_message_iter_append_basic (&iter_sub_array, DBUS_TYPE_OBJECT_PATH, &child_path);
              g_free (child_path);
            }
        }
      dbus_message_iter_close_container (&iter_struct, &iter_sub_array);

      dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "s", &iter_sub_array);
      append_atk_object_interfaces (obj, &iter_sub_array);
      dbus_message_iter_close_container (&iter_struct, &iter_sub_array);

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

      g_free(path);
    }
  spi_atk_state_to_dbus_array (obj, &states);
      dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "u", &iter_sub_array);
  for (count = 0; count < 2; count++)
    dbus_message_iter_append_basic (&iter_sub_array, DBUS_TYPE_UINT32, &states[count]);
      dbus_message_iter_close_container (&iter_struct, &iter_sub_array);
  dbus_message_iter_close_container (iter_array, &iter_struct);
}

/*---------------------------------------------------------------------------*/

/*
 * Used to marshal array of objects to remove.
 * Marshalls an object path onto the iter provided.
 */
static void
append_accessible_path(gpointer ref_data, gpointer null, gpointer data)
{
  guint ref;
  gchar *path;
  DBusMessageIter *iter_array;

  iter_array = (DBusMessageIter *) data;
  ref = GPOINTER_TO_INT(ref_data);
  path = atk_dbus_get_path_from_ref(ref);
  dbus_message_iter_append_basic (iter_array, DBUS_TYPE_OBJECT_PATH, &path);
  g_free(path);
}

/*---------------------------------------------------------------------------*/

static gboolean
send_cache_update(gpointer data)
{
  DBusMessage *message;
  DBusMessageIter iter;
  DBusMessageIter iter_array;
  DBusConnection *bus = (DBusConnection *) data;

  message = dbus_message_new_signal ("/org/freedesktop/atspi/tree", SPI_DBUS_INTERFACE_TREE, "updateTree");

  dbus_message_iter_init_append (message, &iter);

  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(ooaoassusau)", &iter_array);
  atk_dbus_foreach_update_list(append_accessible, &iter_array);
  dbus_message_iter_close_container(&iter, &iter_array);

  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "o", &iter_array);
  atk_dbus_foreach_remove_list(append_accessible_path, &iter_array);
  dbus_message_iter_close_container(&iter, &iter_array);

  dbus_connection_send(bus, message, NULL);
  update_pending = FALSE;

  return FALSE;
}

/*---------------------------------------------------------------------------*/

void
atk_tree_cache_needs_update(DBusConnection *bus)
{
  if (!update_pending)
    {
      g_idle_add(send_cache_update, bus);
      update_pending = TRUE;
    }
}

/*---------------------------------------------------------------------------*/

static DBusMessage *
impl_getRoot (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkObject *root = atk_get_root();
  char *path;
  DBusMessage *reply;

  if (root) path = atk_dbus_get_path(root);
  if (!root || !path)
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
  AtkObject *root = atk_get_root();

  if (!root) 
     return spi_dbus_general_error(message);
  reply = dbus_message_new_method_return (message);

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "(ooaoassusau)", &iter_array);
  atk_dbus_foreach_registered(append_accessible, &iter_array);
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
