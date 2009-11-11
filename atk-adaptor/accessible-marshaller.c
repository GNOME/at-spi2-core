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

#include <droute/droute.h>

#include "common/spi-dbus.h"
#include "common/spi-stateset.h"

#include "accessible-register.h"
#include "accessible-marshaller.h"
#include "bridge.h"

#include "adaptors.h"

/*---------------------------------------------------------------------------*/

void
spi_dbus_append_name_and_path_inner (DBusMessageIter *iter, const char *bus_name, const char *path)
{
  DBusMessageIter iter_struct;

  if (!bus_name)
    bus_name = "";

  dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL, &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &bus_name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (iter, &iter_struct);
}

void
spi_dbus_append_name_and_path (DBusMessage *message, DBusMessageIter *iter, AtkObject *obj, gboolean do_register, gboolean unref)
{
  gchar *path;
  DBusMessageIter iter_struct;
  const char *bus_name = dbus_bus_get_unique_name (atk_adaptor_app_data->bus);

  path = atk_dbus_object_to_path (obj, do_register);

  if (!path)
    path = g_strdup (SPI_DBUS_PATH_NULL);

  spi_dbus_append_name_and_path_inner (iter, bus_name, path);

  g_free (path);
  if (obj && unref)
    g_object_unref (obj);
}

/*
 * Marshals the D-Bus path of an AtkObject into a D-Bus message.
 *
 * Unrefs the AtkObject if unref is true.
 */
DBusMessage *
spi_dbus_return_object (DBusMessage *message, AtkObject *obj, gboolean do_register, gboolean unref)
{
  DBusMessage *reply;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      DBusMessageIter iter;
      dbus_message_iter_init_append (reply, &iter);
      spi_dbus_append_name_and_path (message, &iter, obj, do_register, unref);
    }

  return reply;
}

DBusMessage *
spi_dbus_return_hyperlink (DBusMessage *message, AtkHyperlink *link, AtkObject *container, gboolean unref)
{
  return spi_dbus_return_sub_object (message, G_OBJECT (link), G_OBJECT (container), unref);
}

DBusMessage *
spi_dbus_return_sub_object (DBusMessage *message, GObject *sub, GObject *container, gboolean unref)
{
  DBusMessage *reply;
  gchar *path;

  path = atk_dbus_sub_object_to_path (sub, container);

  if (sub && unref)
    g_object_unref (sub);

  if (!path)
    path = g_strdup (SPI_DBUS_PATH_NULL);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_OBJECT_PATH, &path,
                                DBUS_TYPE_INVALID);
    }

  g_free (path);

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
  DBusMessageIter iter_variant;
  char *path;

  path = atk_dbus_object_to_path (obj, FALSE);

  if (!path)
    path = g_strdup (SPI_DBUS_PATH_NULL);

  if (unref)
    g_object_unref (obj);

  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "(so)", &iter_variant);
  spi_dbus_append_name_and_path_inner (&iter_variant, NULL, path);
  dbus_message_iter_close_container (iter, &iter_variant);
}

/*---------------------------------------------------------------------------*/

void
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
 * The format of the structure is (o(so)a(so)assusau).
 * This is used in the updateTree signal and the GetTree method
 * of the org.freedesktop.atspi.Tree interface.
 *
 * To marshal an object its parent, and all its children must already
 * be registered with D-Bus and have been given a D-Bus object path.
 */
void
spi_atk_append_accessible(AtkObject *obj, gpointer data)
{
  DBusMessageIter iter_struct, iter_sub_array;
  dbus_uint32_t states [2];
  int count;
  AtkStateSet *set;
  DBusMessageIter *iter_array = (DBusMessageIter *)data;

  const char *name, *desc;
  dbus_uint32_t role;

  set = atk_object_ref_state_set (obj);
  dbus_message_iter_open_container (iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct);
    {
      AtkObject *parent;
      gchar *path, *path_parent;

      /* Marshall object path */
      path = atk_dbus_object_to_path (obj, FALSE);

      /* Marshall parent */
      parent = atk_object_get_parent(obj);
      if (parent == NULL)
        {
          path_parent = atk_dbus_desktop_object_path ();
        }
      else
        {
          path_parent = atk_dbus_object_to_path (parent, FALSE);
          if (!path_parent)
            {
              /* This should only happen if a widget is re-parented to
               * an AtkObject that has not been registered and is then
               * updated. Ideally objects would be de-registered when
               * they are removed from a registered tree object, but
               * this would invalidate a huge amount of cache when
               * re-parenting.
               */
#if SPI_ATK_DEBUG
              g_warning ("AT-SPI: Registered accessible marshalled when parent not registered");
#endif
              path_parent = atk_dbus_desktop_object_path ();
            }
        }

      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
      spi_dbus_append_name_and_path_inner (&iter_struct, NULL, path_parent);
      g_free(path_parent);

      /* Marshall children */
      dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "(so)", &iter_sub_array);
      if (!atk_state_set_contains_state (set, ATK_STATE_MANAGES_DESCENDANTS))
        {
          gint childcount, i;

          childcount = atk_object_get_n_accessible_children (obj);
          for (i = 0; i < childcount; i++)
            {
              AtkObject *child;
              gchar *child_path;

              child = atk_object_ref_accessible_child (obj, i);
              child_path = atk_dbus_object_to_path (child, FALSE);
              if (child_path)
                {
		  spi_dbus_append_name_and_path_inner (&iter_sub_array, NULL, child_path);
                  g_free (child_path);
                }
              g_object_unref(G_OBJECT(child));
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
      spi_atk_state_set_to_dbus_array (set, states);
      dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "u", &iter_sub_array);
      for (count = 0; count < 2; count++)
        {
          dbus_message_iter_append_basic (&iter_sub_array, DBUS_TYPE_UINT32, &states[count]);
        }
      dbus_message_iter_close_container (&iter_struct, &iter_sub_array);
    }
  dbus_message_iter_close_container (iter_array, &iter_struct);
  g_object_unref (set);
}

void
spi_atk_append_attribute_set (DBusMessageIter *iter, AtkAttributeSet *attr)
{
  DBusMessageIter dictIter;

  dbus_message_iter_open_container (iter, DBUS_TYPE_ARRAY, "{ss}", &dictIter);
  spi_atk_append_attribute_set_inner (&dictIter, attr);
  dbus_message_iter_close_container (iter, &dictIter);
}

void
spi_atk_append_attribute_set_inner (DBusMessageIter *iter, AtkAttributeSet *attr)
{
  DBusMessageIter dictEntryIter;

  while (attr)
    {
      AtkAttribute *attribute = (AtkAttribute *) attr->data;
      dbus_message_iter_open_container (iter, DBUS_TYPE_DICT_ENTRY, NULL, &dictEntryIter);
      dbus_message_iter_append_basic (&dictEntryIter, DBUS_TYPE_STRING, &attribute->name);
      dbus_message_iter_append_basic (&dictEntryIter, DBUS_TYPE_STRING, &attribute->value);
      dbus_message_iter_close_container (iter, &dictEntryIter);
      attr = g_slist_next (attr);
    }
}

/*END------------------------------------------------------------------------*/

