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

#define ATK_DISABLE_DEPRECATION_WARNINGS
#include "bridge.h"
#include <atk/atk.h>
#include <droute/droute.h>

#include "spi-dbus.h"

#include "introspection.h"
#include "object.h"

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_TABLE_VERSION);
}

static dbus_bool_t
impl_get_NRows (DBusMessageIter *iter, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  g_return_val_if_fail (ATK_IS_TABLE (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_table_get_n_rows (table));
}

static dbus_bool_t
impl_get_NColumns (DBusMessageIter *iter, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  g_return_val_if_fail (ATK_IS_TABLE (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_table_get_n_columns (table));
}

static dbus_bool_t
impl_get_Caption (DBusMessageIter *iter, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  g_return_val_if_fail (ATK_IS_TABLE (user_data), FALSE);
  spi_object_append_v_reference (iter, atk_table_get_caption (table));
  return TRUE;
}

static dbus_bool_t
impl_get_Summary (DBusMessageIter *iter, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  g_return_val_if_fail (ATK_IS_TABLE (user_data), FALSE);
  spi_object_append_v_reference (iter, atk_table_get_summary (table));
  return TRUE;
}

static dbus_bool_t
impl_get_NSelectedRows (DBusMessageIter *iter, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  gint *selected_rows = NULL;
  int count;
  g_return_val_if_fail (ATK_IS_TABLE (user_data), FALSE);
  count = atk_table_get_selected_rows (table, &selected_rows);
  if (selected_rows)
    g_free (selected_rows);
  return droute_return_v_int32 (iter, count);
}

static dbus_bool_t
impl_get_NSelectedColumns (DBusMessageIter *iter, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  gint *selected_columns = NULL;
  int count;
  g_return_val_if_fail (ATK_IS_TABLE (user_data), FALSE);
  count = atk_table_get_selected_columns (table, &selected_columns);
  if (selected_columns)
    g_free (selected_columns);
  return droute_return_v_int32 (iter, count);
}

static DBusMessage *
impl_GetAccessibleAt (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t row, column;
  DBusMessage *reply;
  AtkObject *obj;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  obj = atk_table_ref_at (table, row, column);
  reply = spi_object_return_reference (message, obj);
  if (obj)
    g_object_unref (obj);

  return reply;
}

static DBusMessage *
impl_GetIndexAt (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t row, column;
  dbus_int32_t index;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  index = atk_table_get_index_at (table, row, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &index,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetRowAtIndex (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t index;
  dbus_int32_t row;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &index, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  row = atk_table_get_row_at_index (table, index);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &row,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetColumnAtIndex (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t index;
  dbus_int32_t column;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &index, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  column = atk_table_get_column_at_index (table, index);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &column,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static const gchar *
validate_unallocated_string (const gchar *str)
{
  if (!str)
    return "";
  if (!g_utf8_validate (str, -1, NULL))
    {
      g_warning ("atk-bridge: received bad UTF-8 string from a table function");
      return "";
    }
  return str;
}

static DBusMessage *
impl_GetRowDescription (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  dbus_int32_t row;
  AtkTable *table = (AtkTable *) user_data;
  const gchar *description;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  description = atk_table_get_row_description (table, row);
  description = validate_unallocated_string (description);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &description,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetColumnDescription (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t column;
  const char *description;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  description = atk_table_get_column_description (table, column);
  description = validate_unallocated_string (description);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &description,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetRowExtentAt (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t row, column;
  dbus_int32_t extent;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  extent = atk_table_get_row_extent_at (table, row, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &extent,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetColumnExtentAt (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t row, column;
  dbus_int32_t extent;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  extent = atk_table_get_column_extent_at (table, row, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &extent,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetRowHeader (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t row;
  AtkObject *obj = NULL;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  obj = atk_table_get_row_header (table, row);
  return spi_object_return_reference (message, obj);
}

static DBusMessage *
impl_GetColumnHeader (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t column;
  AtkObject *obj;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  obj = atk_table_get_column_header (table, column);
  return spi_object_return_reference (message, obj);
}

static DBusMessage *
impl_GetSelectedRows (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  gint *selected_rows = NULL;
  gint count;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  count = atk_table_get_selected_rows (table, &selected_rows);
  if (!selected_rows)
    count = 0;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      /* tbd - figure out if this is safe for a 0-length array */
      dbus_message_append_args (reply, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32,
                                &selected_rows, count, DBUS_TYPE_INVALID);
    }
  if (selected_rows)
    g_free (selected_rows);
  return reply;
}

static DBusMessage *
impl_GetSelectedColumns (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  gint *selected_columns = NULL;
  gint count;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  count = atk_table_get_selected_columns (table, &selected_columns);
  if (!selected_columns)
    count = 0;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      /* tbd - figure out if this is safe for a 0-length array */
      dbus_message_append_args (reply, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32,
                                &selected_columns, count, DBUS_TYPE_INVALID);
    }
  if (selected_columns)
    g_free (selected_columns);
  return reply;
}

static DBusMessage *
impl_IsRowSelected (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t row;
  DBusMessage *reply;
  dbus_bool_t ret;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  ret = atk_table_is_row_selected (table, row);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_IsColumnSelected (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t column;
  DBusMessage *reply;
  dbus_bool_t ret;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  ret = atk_table_is_column_selected (table, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_IsSelected (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t row, column;
  DBusMessage *reply;
  dbus_bool_t ret;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32, &column,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  ret = atk_table_is_selected (table, row, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_AddRowSelection (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t row;
  DBusMessage *reply;
  dbus_bool_t ret;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  ret = atk_table_add_row_selection (table, row);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_AddColumnSelection (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t column;
  DBusMessage *reply;
  dbus_bool_t ret;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  ret = atk_table_add_column_selection (table, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_RemoveRowSelection (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t row;
  DBusMessage *reply;
  dbus_bool_t ret;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &row, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  ret = atk_table_remove_row_selection (table, row);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_RemoveColumnSelection (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t column;
  DBusMessage *reply;
  dbus_bool_t ret;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &column, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  ret = atk_table_remove_column_selection (table, column);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetRowColumnExtentsAtIndex (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTable *table = (AtkTable *) user_data;
  dbus_int32_t index;
  dbus_int32_t row, column, row_extents, col_extents;
  dbus_bool_t is_selected;
  dbus_bool_t ret;
  DBusMessage *reply;
  AtkObject *cell;
  AtkRole role = ATK_ROLE_INVALID;

  g_return_val_if_fail (ATK_IS_TABLE (user_data),
                        droute_not_yet_handled_error (message));
  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &index, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  column = atk_table_get_column_at_index (table, index);
  row = atk_table_get_row_at_index (table, index);
  row_extents = atk_table_get_row_extent_at (table, row, column);
  col_extents = atk_table_get_column_extent_at (table, row, column);
  is_selected = atk_table_is_selected (table, row, column);
  cell = atk_table_ref_at (table, row, column);
  if (cell)
    {
      role = atk_object_get_role (cell);
      g_object_unref (cell);
    }
  ret = (role == ATK_ROLE_TABLE_CELL ? TRUE : FALSE);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INT32, &row, DBUS_TYPE_INT32,
                                &column, DBUS_TYPE_INT32, &row_extents,
                                DBUS_TYPE_INT32, &col_extents,
                                DBUS_TYPE_BOOLEAN, &is_selected,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DRouteMethod methods[] = {
  { impl_GetAccessibleAt, "GetAccessibleAt" },
  { impl_GetIndexAt, "GetIndexAt" },
  { impl_GetRowAtIndex, "GetRowAtIndex" },
  { impl_GetColumnAtIndex, "GetColumnAtIndex" },
  { impl_GetRowDescription, "GetRowDescription" },
  { impl_GetColumnDescription, "GetColumnDescription" },
  { impl_GetRowExtentAt, "GetRowExtentAt" },
  { impl_GetColumnExtentAt, "GetColumnExtentAt" },
  { impl_GetRowHeader, "GetRowHeader" },
  { impl_GetColumnHeader, "GetColumnHeader" },
  { impl_GetSelectedRows, "GetSelectedRows" },
  { impl_GetSelectedColumns, "GetSelectedColumns" },
  { impl_IsRowSelected, "IsRowSelected" },
  { impl_IsColumnSelected, "IsColumnSelected" },
  { impl_IsSelected, "IsSelected" },
  { impl_AddRowSelection, "AddRowSelection" },
  { impl_AddColumnSelection, "AddColumnSelection" },
  { impl_RemoveRowSelection, "RemoveRowSelection" },
  { impl_RemoveColumnSelection, "RemoveColumnSelection" },
  { impl_GetRowColumnExtentsAtIndex, "GetRowColumnExtentsAtIndex" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_NRows, NULL, "NRows" },
  { impl_get_NColumns, NULL, "NColumns" },
  { impl_get_Caption, NULL, "Caption" },
  { impl_get_Summary, NULL, "Summary" },
  { impl_get_NSelectedRows, NULL, "NSelectedRows" },
  { impl_get_NSelectedColumns, NULL, "NSelectedColumns" },
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};

void
spi_initialize_table (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_TABLE,
                         spi_org_a11y_atspi_Table,
                         methods, properties);
};
