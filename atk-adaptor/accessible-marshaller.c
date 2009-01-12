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

#include "accessible-register.h"
#include "accessible-marshaller.h"

#include "spi-common/spi-dbus.h"

/*---------------------------------------------------------------------------*/

/*
 * Marshals the D-Bus path of an AtkObject into a D-Bus message.
 *
 * Unrefs the AtkObject if unref is true.
 */
DBusMessage *
spi_dbus_return_object (DBusMessage *message, AtkObject *obj, gboolean unref)
{
  DBusMessage *reply;
  gchar *path;

  path = atk_dbus_object_to_path (obj);

  if (unref)
    g_object_unref (obj);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_OBJECT_PATH, path,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

/*---------------------------------------------------------------------------*/

/*
 * Marshals a variant containing the D-Bus path of an AtkObject into a D-Bus
 * message.
 *
 * Unrefs the object if unref is true.
 */
dbus_bool_t
spi_dbus_return_v_object (DBusMessageIter *iter, AtkObject *obj, int unref)
{
  char *path;

  path = atk_dbus_object_to_path (obj);

  if (unref)
    g_object_unref (obj);

  return droute_return_v_object (iter, path);
}

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

/*
 * Marshals the given AtkObject into the provided D-Bus iterator.
 *
 * The object is marshalled including all its client side cache data.
 * The format of the structure is (ooaoassusau).
 * This is used in the updateTree signal and the getTree method
 * of the org.freedesktop.atspi.Tree interface.
 *
 * To marshal an object its parent, and all its children must already
 * be registered with D-Bus and have been given a D-Bus object path.
 */
void
spi_atk_append_accessible(AtkObject *obj, gpointer iter)
{
  DBusMessageIter *iter_array;
  DBusMessageIter iter_struct, iter_sub_array;
  dbus_int32_t states [2];
  int count;

  const char *name, *desc;
  int i;
  dbus_uint32_t role;
  GSList *l;

  iter_array = (DBusMessageIter *) iter;

  dbus_message_iter_open_container (iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct);
    {
      AtkObject *parent;
      gchar *path, *path_parent;

      /* Marshall object path */
      path = atk_dbus_object_to_path (obj);
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);

      /* Marshall parent */
      parent = atk_object_get_parent(obj);
      if (parent == NULL)
        {
          path_parent = g_strdup("/");
        }
      else
        {
          path_parent = atk_dbus_object_to_path (parent);
          if (!path_parent)
            {
              g_critical ("AT-SPI: Object registered without registering parent");
              path_parent = g_strdup("/");
            }
        }
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path_parent);
      g_free(path_parent);

      /* Marshall children */
      dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "o", &iter_sub_array);
        {
          gint childcount, i;

          childcount = atk_object_get_n_accessible_children (obj);
          for (i = 0; i < childcount; i++)
            {
              AtkObject *child;
              gchar *child_path;

              child = atk_object_ref_accessible_child (obj, i);
              child_path = atk_dbus_object_to_path (child);
              g_object_unref(G_OBJECT(child));
              if (G_LIKELY (child_path))
                {
                  dbus_message_iter_append_basic (&iter_sub_array, DBUS_TYPE_OBJECT_PATH, &child_path);
                }
              else
                {
                  g_critical ("AT-SPI: Child object exists in accessible tree but has not been registered");
                  g_free (child_path);
                }
            }
        }
      dbus_message_iter_close_container (&iter_struct, &iter_sub_array);

      /* Marshall interfaces */
      dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "s", &iter_sub_array);
      append_atk_object_interfaces (obj, &iter_sub_array);
      dbus_message_iter_close_container (&iter_struct, &iter_sub_array);

      /* Marshall name */
      name = atk_object_get_name (obj);
      if (!name)
        name = "";
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);

      /* Marshall role */
      role = spi_accessible_role_from_atk_role (atk_object_get_role (obj));
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &role);

      /* Marshall description */
      desc = atk_object_get_description (obj);
      if (!desc)
        desc = "";
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &desc);

      g_free(path);

      /* Marshall state set */
      spi_atk_state_to_dbus_array (obj, &states);
      dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "u", &iter_sub_array);
      for (count = 0; count < 2; count++)
        {
          dbus_message_iter_append_basic (&iter_sub_array, DBUS_TYPE_UINT32, &states[count]);
        }
      dbus_message_iter_close_container (&iter_struct, &iter_sub_array);
    }
  dbus_message_iter_close_container (iter_array, &iter_struct);
}

/*END------------------------------------------------------------------------*/

