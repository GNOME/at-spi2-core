/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2013 SUSE LLC.
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

#include <atk/atk.h>
#include <droute/droute.h>

#include "spi-dbus.h"

#include "introspection.h"
#include "object.h"

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_TABLE_CELL_VERSION);
}

static dbus_bool_t
impl_get_ColumnSpan (DBusMessageIter *iter, void *user_data)
{
  AtkTableCell *cell = (AtkTableCell *) user_data;
  g_return_val_if_fail (ATK_IS_TABLE_CELL (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_table_cell_get_column_span (cell));
}

static DBusMessage *
message_from_object_array (DBusMessage *message, GPtrArray *array)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;
  gint len;
  gint i;

  reply = dbus_message_new_method_return (message);
  if (!reply)
    return NULL;

  dbus_message_iter_init_append (reply, &iter);

  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "(so)", &iter_array))
    return reply; /* TODO: handle out of memory */
  len = (array ? array->len : 0);
  for (i = 0; i < len; i++)
    {
      spi_object_append_reference (&iter_array, g_ptr_array_index (array, i));
    }
  dbus_message_iter_close_container (&iter, &iter_array);
  if (array)
    g_ptr_array_unref (array);
  return reply;
}

static DBusMessage *
impl_GetColumnHeaderCells (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTableCell *cell = user_data;
  GPtrArray *array;

  g_return_val_if_fail (ATK_IS_TABLE_CELL (user_data),
                        droute_not_yet_handled_error (message));

  array = atk_table_cell_get_column_header_cells (cell);
  return message_from_object_array (message, array);
}

static dbus_bool_t
impl_get_RowSpan (DBusMessageIter *iter, void *user_data)
{
  AtkTableCell *cell = (AtkTableCell *) user_data;
  g_return_val_if_fail (ATK_IS_TABLE_CELL (user_data), FALSE);
  return droute_return_v_int32 (iter, atk_table_cell_get_row_span (cell));
}

static DBusMessage *
impl_GetRowHeaderCells (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTableCell *cell = user_data;
  GPtrArray *array;

  g_return_val_if_fail (ATK_IS_TABLE_CELL (user_data),
                        droute_not_yet_handled_error (message));

  array = atk_table_cell_get_row_header_cells (cell);
  return message_from_object_array (message, array);
}

static dbus_bool_t
impl_get_Position (DBusMessageIter *iter, void *user_data)
{
  AtkTableCell *cell = (AtkTableCell *) user_data;
  gint row = -1, column = -1;
  DBusMessageIter iter_struct, iter_variant;

  g_return_val_if_fail (ATK_IS_TABLE_CELL (user_data), FALSE);
  if (!atk_table_cell_get_position (cell, &row, &column))
    return FALSE;

  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "(ii)", &iter_variant);
  dbus_message_iter_open_container (&iter_variant, DBUS_TYPE_STRUCT, NULL, &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, (dbus_int32_t *) &row);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, (dbus_int32_t *) &column);
  dbus_message_iter_close_container (&iter_variant, &iter_struct);
  dbus_message_iter_close_container (iter, &iter_variant);
  return TRUE;
}

static dbus_bool_t
impl_get_Table (DBusMessageIter *iter, void *user_data)
{
  AtkTableCell *cell = (AtkTableCell *) user_data;
  AtkObject *table;
  DBusMessageIter iter_variant;

  g_return_val_if_fail (ATK_IS_TABLE_CELL (user_data), FALSE);

  table = atk_table_cell_get_table (cell);
  if (!table)
    return FALSE;
  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "(so)", &iter_variant);
  spi_object_append_reference (&iter_variant, table);
  dbus_message_iter_close_container (iter, &iter_variant);
  return TRUE;
}

static DBusMessage *
impl_GetRowColumnSpan (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkTableCell *cell = (AtkTableCell *) user_data;
  gint row, column, row_span, column_span;
  dbus_int32_t d_row, d_column, d_row_span, d_column_span;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_TABLE_CELL (user_data),
                        droute_not_yet_handled_error (message));
  atk_table_cell_get_row_column_span (cell, &row, &column, &row_span,
                                      &column_span);
  d_row = row;
  d_column = column;
  d_row_span = row_span;
  d_column_span = column_span;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &d_row, DBUS_TYPE_INT32,
                                &d_column, DBUS_TYPE_INT32, &d_row_span,
                                DBUS_TYPE_INT32, &d_column_span,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DRouteMethod methods[] = {
  { impl_GetRowHeaderCells, "GetRowHeaderCells" },
  { impl_GetColumnHeaderCells, "GetColumnHeaderCells" },
  { impl_GetRowColumnSpan, "GetRowColumnSpan" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_ColumnSpan, NULL, "ColumnSpan" },
  { impl_get_Position, NULL, "Position" },
  { impl_get_RowSpan, NULL, "RowSpan" },
  { impl_get_Table, NULL, "Table" },
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};

void
spi_initialize_table_cell (DRoutePath *path)
{
  droute_path_add_interface (path,
                             ATSPI_DBUS_INTERFACE_TABLE_CELL,
                             spi_org_a11y_atspi_TableCell,
                             methods, properties);
};
