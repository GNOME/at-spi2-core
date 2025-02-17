/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include "bridge.h"
#include <atk/atk.h>
#include <droute/droute.h>

#include "accessible-stateset.h"
#include "atspi/atspi.h"
#include "introspection.h"
#include "object.h"
#include "spi-dbus.h"
#include <string.h>

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_ACCESSIBLE_VERSION);
}

static dbus_bool_t
impl_get_Name (DBusMessageIter *iter, void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data), FALSE);

  return droute_return_v_string (iter, atk_object_get_name (object));
}

static dbus_bool_t
impl_get_Description (DBusMessageIter *iter, void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data), FALSE);

  return droute_return_v_string (iter, atk_object_get_description (object));
}

static dbus_bool_t
impl_get_Locale (DBusMessageIter *iter, void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data), FALSE);

  return droute_return_v_string (iter, atk_object_get_object_locale (object));
}

static dbus_bool_t
impl_get_Parent (DBusMessageIter *iter, void *user_data)
{
  AtkObject *obj = (AtkObject *) user_data;
  AtkObject *parent;
  DBusMessageIter iter_variant;
  dbus_uint32_t role;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data), FALSE);

  role = spi_accessible_role_from_atk_role (atk_object_get_role (obj));

  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "(so)",
                                    &iter_variant);

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
                  dbus_message_iter_open_container (&iter_variant, DBUS_TYPE_STRUCT, NULL,
                                                    &iter_parent);
                  dbus_message_iter_append_basic (&iter_parent, DBUS_TYPE_STRING, &bus_parent);
                  dbus_message_iter_append_basic (&iter_parent, DBUS_TYPE_OBJECT_PATH, &path_parent);
                  dbus_message_iter_close_container (&iter_variant, &iter_parent);
                }
              else
                {
                  spi_object_append_null_reference (&iter_variant);
                }
            }
          else
            {
              spi_object_append_null_reference (&iter_variant);
            }
        }
      else if (role != ATSPI_ROLE_APPLICATION)
        spi_object_append_null_reference (&iter_variant);
      else
        spi_object_append_desktop_reference (&iter_variant);
    }
  else
    {
      spi_object_append_reference (&iter_variant, parent);
    }

  dbus_message_iter_close_container (iter, &iter_variant);
  return TRUE;
}

static dbus_bool_t
impl_get_ChildCount (DBusMessageIter *iter, void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  int childCount;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data), FALSE);

  childCount = (ATK_IS_SOCKET (object) && atk_socket_is_occupied (ATK_SOCKET (object)))
                   ? 1
                   : atk_object_get_n_accessible_children (object);
  return droute_return_v_int32 (iter, childCount);
}

static DBusMessage *
impl_GetChildAtIndex (DBusConnection *bus,
                      DBusMessage *message,
                      void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  DBusMessage *reply;
  dbus_int32_t i;
  AtkObject *child;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &i, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  if (ATK_IS_SOCKET (object) && atk_socket_is_occupied (ATK_SOCKET (object)) && i == 0)
    {
      AtkSocket *socket = ATK_SOCKET (object);
      gchar *child_name, *child_path;
      child_name = g_strdup (socket->embedded_plug_id);
      child_path = g_utf8_strchr (child_name + 1, -1, ':');
      if (child_path)
        {
          DBusMessageIter iter, iter_socket;
          *(child_path++) = '\0';
          reply = dbus_message_new_method_return (message);
          if (!reply)
            return NULL;
          dbus_message_iter_init_append (reply, &iter);
          dbus_message_iter_open_container (&iter, DBUS_TYPE_STRUCT, NULL,
                                            &iter_socket);
          dbus_message_iter_append_basic (&iter_socket, DBUS_TYPE_STRING, &child_name);
          dbus_message_iter_append_basic (&iter_socket, DBUS_TYPE_OBJECT_PATH, &child_path);
          g_free (child_name);
          dbus_message_iter_close_container (&iter, &iter_socket);
          return reply;
        }
      g_free (child_name);
    }
  child = atk_object_ref_accessible_child (object, i);
  reply = spi_object_return_reference (message, child);
  if (child)
    g_object_unref (child);

  return reply;
}

static DBusMessage *
impl_GetChildren (DBusConnection *bus,
                  DBusMessage *message,
                  void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  gint i;
  gint count;
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));
  count = atk_object_get_n_accessible_children (object);

  if (count > ATSPI_MAX_CHILDREN)
    {
      gchar *errmsg = g_strdup_printf (
          "Accessible's child count %d exceeds the maximum of %d handled by GetChildren.", count, ATSPI_MAX_CHILDREN);
      reply = dbus_message_new_error (message, DBUS_ERROR_INVALID_ARGS, errmsg);
      g_free (errmsg);
      return reply;
    }

  reply = dbus_message_new_method_return (message);
  if (!reply)
    goto oom;
  dbus_message_iter_init_append (reply, &iter);
  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(so)", &iter_array))
    goto oom;

  if (ATK_IS_SOCKET (object) && atk_socket_is_occupied (ATK_SOCKET (object)))
    {
      AtkSocket *socket = ATK_SOCKET (object);
      gchar *child_name, *child_path;
      child_name = g_strdup (socket->embedded_plug_id);
      child_path = g_utf8_strchr (child_name + 1, -1, ':');
      if (child_path)
        {
          DBusMessageIter iter_struct;
          *(child_path++) = '\0';
          dbus_message_iter_open_container (&iter_array, DBUS_TYPE_STRUCT, NULL,
                                            &iter_struct);
          dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING,
                                          &child_name);
          dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH,
                                          &child_path);
          dbus_message_iter_close_container (&iter_array, &iter_struct);
          g_free (child_name);
          if (!dbus_message_iter_close_container (&iter, &iter_array))
            goto oom;
          return reply;
        }
      g_free (child_name);
    }

  for (i = 0; i < count; i++)
    {
      AtkObject *child = atk_object_ref_accessible_child (object, i);
      spi_object_append_reference (&iter_array, child);
      if (child)
        g_object_unref (child);
    }
  if (!dbus_message_iter_close_container (&iter, &iter_array))
    goto oom;
  return reply;
oom:
  // TODO: handle out-of-memory
  return reply;
}

static DBusMessage *
impl_GetIndexInParent (DBusConnection *bus,
                       DBusMessage *message,
                       void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  dbus_int32_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));

  rv = atk_object_get_index_in_parent (object);
  reply = dbus_message_new_method_return (message);
  dbus_message_append_args (reply, DBUS_TYPE_INT32, &rv, DBUS_TYPE_INVALID);
  return reply;
}

static gboolean
spi_init_relation_type_table (AtspiRelationType *types)
{
  gint i;

  for (i = 0; i < ATK_RELATION_LAST_DEFINED; i++)
    types[i] = ATSPI_RELATION_NULL;

  types[ATK_RELATION_CONTROLLED_BY] = ATSPI_RELATION_CONTROLLED_BY;
  types[ATK_RELATION_CONTROLLER_FOR] = ATSPI_RELATION_CONTROLLER_FOR;
  types[ATK_RELATION_LABEL_FOR] = ATSPI_RELATION_LABEL_FOR;
  types[ATK_RELATION_LABELLED_BY] = ATSPI_RELATION_LABELLED_BY;
  types[ATK_RELATION_MEMBER_OF] = ATSPI_RELATION_MEMBER_OF;
  types[ATK_RELATION_NODE_CHILD_OF] = ATSPI_RELATION_NODE_CHILD_OF;
  types[ATK_RELATION_FLOWS_TO] = ATSPI_RELATION_FLOWS_TO;
  types[ATK_RELATION_FLOWS_FROM] = ATSPI_RELATION_FLOWS_FROM;
  types[ATK_RELATION_SUBWINDOW_OF] = ATSPI_RELATION_SUBWINDOW_OF;
  types[ATK_RELATION_EMBEDS] = ATSPI_RELATION_EMBEDS;
  types[ATK_RELATION_EMBEDDED_BY] = ATSPI_RELATION_EMBEDDED_BY;
  types[ATK_RELATION_POPUP_FOR] = ATSPI_RELATION_POPUP_FOR;
  types[ATK_RELATION_PARENT_WINDOW_OF] =
      ATSPI_RELATION_PARENT_WINDOW_OF;
  types[ATK_RELATION_DESCRIPTION_FOR] =
      ATSPI_RELATION_DESCRIPTION_FOR;
  types[ATK_RELATION_DESCRIBED_BY] = ATSPI_RELATION_DESCRIBED_BY;
  types[ATK_RELATION_DETAILS] = ATSPI_RELATION_DETAILS;
  types[ATK_RELATION_DETAILS_FOR] = ATSPI_RELATION_DETAILS_FOR;
  types[ATK_RELATION_ERROR_MESSAGE] = ATSPI_RELATION_ERROR_MESSAGE;
  types[ATK_RELATION_ERROR_FOR] = ATSPI_RELATION_ERROR_FOR;
  types[ATK_RELATION_NODE_PARENT_OF] = ATSPI_RELATION_NODE_PARENT_OF;

  return TRUE;
}

static AtspiRelationType
spi_relation_type_from_atk_relation_type (AtkRelationType type)
{
  static gboolean is_initialized = FALSE;
  static AtspiRelationType
      spi_relation_type_table[ATK_RELATION_LAST_DEFINED];
  AtspiRelationType spi_type;

  if (!is_initialized)
    is_initialized = spi_init_relation_type_table (spi_relation_type_table);

  if (type > ATK_RELATION_NULL && type < ATK_RELATION_LAST_DEFINED)
    spi_type = spi_relation_type_table[type];
  else
    spi_type = ATSPI_RELATION_EXTENDED;
  return spi_type;
}

static DBusMessage *
impl_GetRelationSet (DBusConnection *bus,
                     DBusMessage *message,
                     void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  DBusMessage *reply;
  AtkRelationSet *set;
  DBusMessageIter iter, iter_array, iter_struct, iter_targets;
  gint count;
  gint i, j;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));
  reply = dbus_message_new_method_return (message);
  if (!reply)
    return NULL;
  set = atk_object_ref_relation_set (object);
  dbus_message_iter_init_append (reply, &iter);
  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(ua(so))", &iter_array))
    {
      goto oom;
    }
  count = 0;
  if (set)
    count = atk_relation_set_get_n_relations (set);
  for (i = 0; i < count; i++)
    {
      AtkRelation *r = atk_relation_set_get_relation (set, i);
      AtkRelationType rt;
      GPtrArray *target;
      dbus_uint32_t type;
      if (!r)
        continue;
      rt = atk_relation_get_relation_type (r);
      type = spi_relation_type_from_atk_relation_type (rt);
      target = atk_relation_get_target (r);
      if (!dbus_message_iter_open_container (&iter_array, DBUS_TYPE_STRUCT, NULL, &iter_struct))
        {
          goto oom;
        }
      dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &type);
      if (!dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY, "(so)", &iter_targets))
        {
          goto oom;
        }
      for (j = 0; j < target->len; j++)
        {
          AtkObject *obj = target->pdata[j];
          if (!obj)
            continue;
          spi_object_append_reference (&iter_targets, obj);
        }
      dbus_message_iter_close_container (&iter_struct, &iter_targets);
      dbus_message_iter_close_container (&iter_array, &iter_struct);
    }
  dbus_message_iter_close_container (&iter, &iter_array);
oom:
  if (set)
    g_object_unref (set);
  // TODO: handle out of memory */
  return reply;
}

static DBusMessage *
impl_GetRole (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  gint role;
  dbus_uint32_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));
  role = atk_object_get_role (object);
  rv = spi_accessible_role_from_atk_role (role);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_UINT32, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetRoleName (DBusConnection *bus,
                  DBusMessage *message,
                  void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  gint role;
  const char *role_name;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));
  role = atk_object_get_role (object);
  role_name = atk_role_get_name (role);
  if (!role_name)
    role_name = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &role_name,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetLocalizedRoleName (DBusConnection *bus,
                           DBusMessage *message,
                           void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  gint role;
  const char *role_name;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));
  role = atk_object_get_role (object);
  role_name = atk_role_get_localized_name (role);
  if (!role_name)
    role_name = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &role_name,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetState (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;

  DBusMessage *reply = NULL;
  DBusMessageIter iter, iter_array;

  dbus_uint32_t states[2];

  guint count;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);

  spi_atk_state_to_dbus_array (object, states);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "u", &iter_array);
  for (count = 0; count < 2; count++)
    {
      dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_UINT32,
                                      &states[count]);
    }
  dbus_message_iter_close_container (&iter, &iter_array);
  return reply;
}

static DBusMessage *
impl_GetAttributes (DBusConnection *bus,
                    DBusMessage *message,
                    void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  AtkAttributeSet *attributes;
  DBusMessage *reply = NULL;
  DBusMessageIter iter;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));

  attributes = atk_object_get_attributes (object);

  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);
  spi_object_append_attribute_set (&iter, attributes);

  atk_attribute_set_free (attributes);

  return reply;
}

static dbus_bool_t
impl_get_Attributes (DBusMessageIter *iter, void *user_data)
{
  DBusMessageIter iter_variant;
  AtkObject *object = (AtkObject *) user_data;
  AtkAttributeSet *attributes;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data), FALSE);

  attributes = atk_object_get_attributes (object);

  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "a{ss}", &iter_variant);
  spi_object_append_attribute_set (&iter_variant, attributes);
  dbus_message_iter_close_container (iter, &iter_variant);

  atk_attribute_set_free (attributes);

  return TRUE;
}

static DBusMessage *
impl_GetApplication (DBusConnection *bus,
                     DBusMessage *message,
                     void *user_data)
{
  return spi_object_return_reference (message, atk_get_root ());
}

static DBusMessage *
impl_GetInterfaces (DBusConnection *bus,
                    DBusMessage *message,
                    void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data),
                        droute_not_yet_handled_error (message));
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_iter_init_append (reply, &iter);
      dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "s",
                                        &iter_array);
      spi_object_append_interfaces (&iter_array, object);
      dbus_message_iter_close_container (&iter, &iter_array);
    }
  return reply;
}

static dbus_bool_t
impl_get_AccessibleId (DBusMessageIter *iter, void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data), FALSE);

  return droute_return_v_string (iter, atk_object_get_accessible_id (object));
}

static dbus_bool_t
impl_get_HelpText (DBusMessageIter *iter, void *user_data)
{
  AtkObject *object = (AtkObject *) user_data;

  g_return_val_if_fail (ATK_IS_OBJECT (user_data), FALSE);

  return droute_return_v_string (iter, atk_object_get_help_text (object));
}

static DRouteMethod methods[] = {
  { impl_GetChildAtIndex, "GetChildAtIndex" },
  { impl_GetChildren, "GetChildren" },
  { impl_GetIndexInParent, "GetIndexInParent" },
  { impl_GetRelationSet, "GetRelationSet" },
  { impl_GetRole, "GetRole" },
  { impl_GetRoleName, "GetRoleName" },
  { impl_GetLocalizedRoleName, "GetLocalizedRoleName" },
  { impl_GetState, "GetState" },
  { impl_GetAttributes, "GetAttributes" },
  { impl_GetApplication, "GetApplication" },
  { impl_GetInterfaces, "GetInterfaces" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_Name, NULL, "Name" },
  { impl_get_Description, NULL, "Description" },
  { impl_get_Locale, NULL, "Locale" },
  { impl_get_Parent, NULL, "Parent" },
  { impl_get_ChildCount, NULL, "ChildCount" },
  { impl_get_Attributes, NULL, "Attributes" },
  { impl_get_AccessibleId, NULL, "AccessibleId" },
  { impl_get_HelpText, NULL, "HelpText" },
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};

void
spi_initialize_accessible (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_ACCESSIBLE,
                         spi_org_a11y_atspi_Accessible,
                         methods, properties);
};
