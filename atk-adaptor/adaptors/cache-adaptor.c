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

#include <string.h>

#include <atk/atk.h>
#include <droute/droute.h>

#include "accessible-cache.h"
#include "accessible-stateset.h"
#include "bridge.h"
#include "introspection.h"
#include "object.h"
#include "spi-dbus.h"

/* TODO - This should possibly be a common define */
#define SPI_OBJECT_PREFIX "/org/a11y/atspi"
#define SPI_CACHE_OBJECT_SUFFIX "/cache"
#define SPI_CACHE_OBJECT_PATH SPI_OBJECT_PREFIX SPI_CACHE_OBJECT_SUFFIX

#define SPI_OBJECT_REFERENCE_SIGNATURE "(" DBUS_TYPE_STRING_AS_STRING \
    DBUS_TYPE_OBJECT_PATH_AS_STRING                                   \
                                       ")"

#define SPI_CACHE_ITEM_SIGNATURE "(" SPI_OBJECT_REFERENCE_SIGNATURE    \
    SPI_OBJECT_REFERENCE_SIGNATURE                                     \
        SPI_OBJECT_REFERENCE_SIGNATURE                                 \
            DBUS_TYPE_INT32_AS_STRING                                  \
                DBUS_TYPE_INT32_AS_STRING                              \
                    DBUS_TYPE_ARRAY_AS_STRING                          \
                        DBUS_TYPE_STRING_AS_STRING                     \
                            DBUS_TYPE_STRING_AS_STRING                 \
                                DBUS_TYPE_UINT32_AS_STRING             \
                                    DBUS_TYPE_STRING_AS_STRING         \
                                        DBUS_TYPE_ARRAY_AS_STRING      \
                                            DBUS_TYPE_UINT32_AS_STRING \
                                 ")"

/*---------------------------------------------------------------------------*/

static const char *
get_toolkit_name (AtkObject *obj)
{
  static const char *toolkit_name = NULL;

  if (!toolkit_name)
    toolkit_name = atk_get_toolkit_name ();

  if (!toolkit_name)
    return "no toolkit name set yet";

  /* TODO: query object attributes */
  return toolkit_name;
}

static gboolean
should_call_index_in_parent (AtkObject *obj, AtkStateSet *set)
{
  if (atk_state_set_contains_state (set, ATK_STATE_TRANSIENT))
    return FALSE;

  if (!strcmp (get_toolkit_name (obj), "gtk") &&
      atk_object_get_role (obj) == ATK_ROLE_MENU_ITEM)
    return FALSE;

  return TRUE;
}

static gboolean
should_cache_children (AtkObject *obj, AtkStateSet *set)
{
  if (atk_state_set_contains_state (set, ATK_STATE_MANAGES_DESCENDANTS) ||
      atk_state_set_contains_state (set, ATK_STATE_DEFUNCT))
    return FALSE;

  if (!strcmp (get_toolkit_name (obj), "gtk") &&
      atk_object_get_role (obj) == ATK_ROLE_MENU)
    return FALSE;

  return TRUE;
}

/*
 * Marshals the given AtkObject into the provided D-Bus iterator.
 *
 * The object is marshalled including all its client side cache data.
 * The format of the structure is (o(so)iiassusau).
 */
static void
append_cache_item (AtkObject *obj, gpointer data)
{
  DBusMessageIter iter_struct, iter_sub_array;
  dbus_uint32_t states[2];
  dbus_int32_t count, index;
  AtkStateSet *set;
  DBusMessageIter *iter_array = (DBusMessageIter *) data;
  const char *name, *desc;
  dbus_uint32_t role;

  set = atk_object_ref_state_set (obj);
  AtkObject *application, *parent;

  dbus_message_iter_open_container (iter_array, DBUS_TYPE_STRUCT, NULL,
                                    &iter_struct);

  /* Marshal object path */
  spi_object_append_reference (&iter_struct, obj);

  role = spi_accessible_role_from_atk_role (atk_object_get_role (obj));

  /* Marshal application */
  application = spi_global_app_data->root;
  spi_object_append_reference (&iter_struct, application);

  /* Marshal parent */
  parent = atk_object_get_parent (obj);
  if (parent == NULL)
    {
      /* TODO, move in to a 'Plug' wrapper. */
      if (ATK_IS_PLUG (obj))
        {
          char *id = g_object_get_data (G_OBJECT (obj), "dbus-plug-parent");
          char *bus_parent;
          char *path_parent;

          if (id)
            {
              bus_parent = g_strdup (id);
              if (bus_parent && (path_parent = g_utf8_strchr (bus_parent + 1, -1, ':')))
                {
                  DBusMessageIter iter_parent;
                  *(path_parent++) = '\0';
                  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_STRUCT, NULL,
                                                    &iter_parent);
                  dbus_message_iter_append_basic (&iter_parent, DBUS_TYPE_STRING, &bus_parent);
                  dbus_message_iter_append_basic (&iter_parent, DBUS_TYPE_OBJECT_PATH, &path_parent);
                  dbus_message_iter_close_container (&iter_struct, &iter_parent);
                }
              else
                {
                  spi_object_append_null_reference (&iter_struct);
                }
            }
          else
            {
              spi_object_append_null_reference (&iter_struct);
            }
        }
      else if (role != ATSPI_ROLE_APPLICATION)
        spi_object_append_null_reference (&iter_struct);
      else
        spi_object_append_desktop_reference (&iter_struct);
    }
  else
    {
      spi_object_append_reference (&iter_struct, parent);
    }

  /* Marshal index in parent */
  index = (should_call_index_in_parent (obj, set)
               ? atk_object_get_index_in_parent (obj)
               : -1);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &index);

  /* marshal child count */
  count = (should_cache_children (obj, set)
               ? atk_object_get_n_accessible_children (obj)
               : -1);

  if (ATK_IS_SOCKET (obj) && atk_socket_is_occupied (ATK_SOCKET (obj)))
    count = 1;
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &count);

  /* Marshal interfaces */
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "s",
                                    &iter_sub_array);
  spi_object_append_interfaces (&iter_sub_array, obj);
  dbus_message_iter_close_container (&iter_struct, &iter_sub_array);

  /* Marshal name */
  name = atk_object_get_name (obj);
  if (!name)
    name = "";
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);

  /* Marshal role */
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &role);

  /* Marshal description */
  desc = atk_object_get_description (obj);
  if (!desc)
    desc = "";
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &desc);

  /* Marshal state set */
  spi_atk_state_set_to_dbus_array (set, states);
  dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "u",
                                    &iter_sub_array);
  for (count = 0; count < 2; count++)
    {
      dbus_message_iter_append_basic (&iter_sub_array, DBUS_TYPE_UINT32,
                                      &states[count]);
    }
  dbus_message_iter_close_container (&iter_struct, &iter_sub_array);

  dbus_message_iter_close_container (iter_array, &iter_struct);
  g_object_unref (set);
}

/*---------------------------------------------------------------------------*/

static void
ref_accessible_hf (gpointer key, gpointer obj_data, gpointer data)
{
  g_object_ref (key);
}

/* For use as a GHFunc */
static void
append_accessible_hf (gpointer key, gpointer obj_data, gpointer data)
{
  /* Make sure it isn't a hyperlink */
  if (ATK_IS_OBJECT (key))
    append_cache_item (ATK_OBJECT (key), data);
}

static void
add_to_list_hf (gpointer key, gpointer obj_data, gpointer data)
{
  GSList **listptr = data;
  *listptr = g_slist_prepend (*listptr, key);
}

/*---------------------------------------------------------------------------*/

static void
emit_cache_remove (SpiCache *cache, GObject *obj)
{
  DBusMessage *message;

  if (!spi_global_app_data->bus)
    return;

  if ((message = dbus_message_new_signal (SPI_CACHE_OBJECT_PATH,
                                          ATSPI_DBUS_INTERFACE_CACHE,
                                          "RemoveAccessible")))
    {
      DBusMessageIter iter;

      dbus_message_iter_init_append (message, &iter);

      spi_object_append_reference (&iter, ATK_OBJECT (obj));

      dbus_connection_send (spi_global_app_data->bus, message, NULL);

      dbus_message_unref (message);
    }
}

static void
emit_cache_add (SpiCache *cache, GObject *obj)
{
  AtkObject *accessible = ATK_OBJECT (obj);
  DBusMessage *message;

  if (!spi_global_app_data->bus)
    return;

  if ((message = dbus_message_new_signal (SPI_CACHE_OBJECT_PATH,
                                          ATSPI_DBUS_INTERFACE_CACHE,
                                          "AddAccessible")))
    {
      DBusMessageIter iter;

      dbus_message_iter_init_append (message, &iter);
      g_object_ref (accessible);
      append_cache_item (accessible, &iter);
      g_object_unref (accessible);

      dbus_connection_send (spi_global_app_data->bus, message, NULL);

      dbus_message_unref (message);
    }
}

/*---------------------------------------------------------------------------*/

static DBusMessage *
impl_GetRoot (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  return spi_object_return_reference (message, spi_global_app_data->root);
}

/*---------------------------------------------------------------------------*/

static DBusMessage *
impl_GetItems (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;
  GSList *pending_unrefs = NULL;

  if (bus == spi_global_app_data->bus)
    spi_atk_add_client (dbus_message_get_sender (message));

  reply = dbus_message_new_method_return (message);

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY,
                                    SPI_CACHE_ITEM_SIGNATURE, &iter_array);
  spi_cache_foreach (spi_global_cache, ref_accessible_hf, NULL);
  spi_cache_foreach (spi_global_cache, append_accessible_hf, &iter_array);
  spi_cache_foreach (spi_global_cache, add_to_list_hf, &pending_unrefs);
  g_slist_free_full (pending_unrefs, g_object_unref);
  dbus_message_iter_close_container (&iter, &iter_array);
  return reply;
}

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_CACHE_VERSION);
}

/*---------------------------------------------------------------------------*/

static DRouteMethod methods[] = {
  { impl_GetRoot, "GetRoot" },
  { impl_GetItems, "GetItems" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_Version, NULL, "version" },
  { NULL, NULL }
};

void
spi_initialize_cache (DRoutePath *path)
{
  droute_path_add_interface (path, ATSPI_DBUS_INTERFACE_CACHE, spi_org_a11y_atspi_Cache,
                             methods, properties);

  g_signal_connect (spi_global_cache, "object-added",
                    (GCallback) emit_cache_add, NULL);

  g_signal_connect (spi_global_cache, "object-removed",
                    (GCallback) emit_cache_remove, NULL);
};

/*END------------------------------------------------------------------------*/
