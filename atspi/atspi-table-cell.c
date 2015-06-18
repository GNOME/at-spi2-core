/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2013 SUSE LLC.
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

#include <stdlib.h> /* for malloc */
#include "atspi-private.h"

static GPtrArray *
get_object_array_and_unref (DBusMessage *reply)
{
  DBusMessageIter iter, iter_array;
  GPtrArray *array;

  if (!reply)
    return NULL;

  if (strcmp (dbus_message_get_signature (reply), "(so)") != 0)
  {
    dbus_message_unref (reply);
    return NULL;
  }

  array = g_ptr_array_new ();

  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    AtspiAccessible *accessible = _atspi_dbus_return_accessible_from_iter (&iter_array);
    g_ptr_array_add (array, accessible);
  }
  dbus_message_unref (reply);
  return array;
}

/**
 * atspi_table_cell_get_column_span:
 * @obj: a GObject instance that implements AtspiTableCellIface
 *
 * Returns the number of columns occupied by this cell accessible.
 *
 * Returns: a gint representing the number of columns occupied by this cell,
 * or 0 if the cell does not implement this method.
 */
gint
atspi_table_cell_get_column_span (AtspiTableCell *obj, GError **error)
{
  dbus_int32_t retval = -1;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_get_property (obj, atspi_interface_table_cell, "ColumnSpan",
                            error, "i", &retval);
	  
  return retval;
}

/**
 * atspi_table_cell_get_column_header_cells:
 * @obj: a GObject instance that implements AtspiTableCellIface
 *
 * Returns the column headers as an array of cell accessibles.
 *
 * Returns: (element-type AtspiAccessible) (transfer full): a GPtrArray of
 * AtspiAccessibles representing the column header cells.
  */
GPtrArray *
atspi_table_cell_get_column_header_cells (AtspiTableCell *obj, GError **error)
{
  DBusMessage *reply;

  g_return_val_if_fail (obj != NULL, NULL);

  reply = _atspi_dbus_call_partial (obj, atspi_interface_table_cell, "GetColumnHeaderCells", error, "");
	  
  return get_object_array_and_unref (reply);
}

/**
 * atspi_table_cell_get_column_index:
 * @obj: a GObject instance that implements AtspiTableCellIface
 *
 * Translates this cell accessible into the corresponding column index.
 *
 * Returns: the column index for this cell, or -1 if unimplemented.
 */
gint
atspi_table_cell_get_column_index (AtspiTableCell *obj, GError **error)
{
  dbus_int32_t retval = -1;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_get_property (obj, atspi_interface_table_cell, "ColumnIndex",
                            error, "i", &retval);
	  
  return retval;
}

/**
 * atspi_table_cell_get_row_span:
 * @obj: a GObject instance that implements AtspiTableCellIface
 *
 * Returns the number of rows occupied by this cell accessible.
 *
 * Returns: a gint representing the number of rows occupied by this cell,
 * or 0 if the cell does not implement this method.
 */
gint
atspi_table_cell_get_row_span (AtspiTableCell *obj, GError **error)
{
  dbus_int32_t retval = -1;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_get_property (obj, atspi_interface_table_cell, "RowSpan",
                            error, "i", &retval);
	  
  return retval;
}

/**
 * atspi_table_cell_get_row_header_cells:
 * @obj: a GObject instance that implements AtspiTableCellIface
 *
 * Returns the row headers as an array of cell accessibles.
 *
 * Returns: (element-type AtspiAccessible) (transfer full): a GPtrArray of
 * AtspiAccessibles representing the row header cells.
 */
GPtrArray *
atspi_table_cell_get_row_header_cells (AtspiTableCell *obj, GError **error)
{
  DBusMessage *reply;

  g_return_val_if_fail (obj != NULL, NULL);

  reply = _atspi_dbus_call_partial (obj, atspi_interface_table_cell, "GetRowHeaderCells", error, "");
	  
  return get_object_array_and_unref (reply);
}

/**
 * atspi_table_cell_get_position:
 * @obj: a GObject instance that implements AtspiTableCellIface
 * @row: (out): the row of the given cell.
 * @column: (out): the column of the given cell.
 *
 * Retrieves the tabular position of this cell.
 *
 * Returns: TRUE if successful, FALSE otherwise.
 */
gint
atspi_table_cell_get_position (AtspiTableCell *obj,
                               gint *row,
                               gint *column,
                               GError *error)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_struct, iter_variant;
  dbus_int32_t d_row = -1, d_column = -1;
  char *iter_sig;

  g_return_val_if_fail (obj != NULL, -1);

  reply = _atspi_dbus_call_partial (obj, "org.freedesktop.DBus.Properties",
                                    "Get", NULL, "ss",
                                    atspi_interface_table_cell, "Position");
	  
  dbus_message_iter_init (reply, &iter);

  /* TODO: Return error here */
  if (dbus_message_iter_get_arg_type (&iter) != 'v')
    return FALSE;

  dbus_message_iter_recurse (&iter, &iter_variant);
  iter_sig = dbus_message_iter_get_signature (&iter_variant);
  /* TODO: Also report error here */
  if (strcmp (iter_sig, "(ii)") != 0)
  {
    dbus_free (iter_sig);
    return FALSE;
  }
  dbus_free (iter_sig);

  dbus_message_iter_recurse (&iter_variant, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &d_row);
  if (row)
    *row = d_row;
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &d_column);
  if (column)
    *column = d_column;
  dbus_message_unref (reply);
  return TRUE;
}

/**
 * atspi_table_cell_get_row_column_span:
 * @obj: a GObject instance that implements AtspiTableCellIface
 * @row: (out): the row index of the given cell.
 * @column: (out): the column index of the given cell.
 * @row_span: (out): the number of rows occupied by this cell.
 * @column_span: (out): the number of columns occupied by this cell.
 *
 * Gets the row and column indexes and extents of this cell accessible.
 */
void
atspi_table_cell_get_row_column_span (AtspiTableCell *obj,
                                       gint *row,
                                       gint *column,
                                       gint *row_span,
                                       gint *column_span,
                                       GError **error)
{
  dbus_int32_t d_row = 0,  d_column = 0, d_row_span = 0, d_column_span = 0;

  if (row)
    *row = -1;
  if (column)
    *column = -1;
  if (row_span)
    *row_span = -1;
  if (column_span)
    *column_span = -1;

  g_return_if_fail (obj != NULL);

  _atspi_dbus_call (obj, atspi_interface_table_cell, "GetRowColumnSpan",
                    error, "=>iiii", &d_row, &d_column,
                    &d_row_span, &d_column_span);

  if (row)
    *row = d_row;
  if (column)
    *column = d_column;
  if (row_span)
    *row_span = d_row_span;
  if (column_span)
    *column_span = d_column_span;
}

/**
 * atspi_table_cell_get_table:
 * @obj: a GObject instance that implements AtspiTableCellIface
 *
 * Returns a reference to the accessible of the containing table.
 *
 * Returns: (transfer full): the AtspiAccessible for the containing table.
 */
AtspiAccessible *
atspi_table_cell_get_table (AtspiTableCell *obj, GError **error)
{
  AtspiAccessible *retval = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_get_property (obj, atspi_interface_table_cell, "Table",
                            error, "(so)", &retval);
	  
  return retval;
}

static void
atspi_table_cell_base_init (AtspiTableCell *klass)
{
}

GType
atspi_table_cell_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtspiTableCell),
      (GBaseInitFunc) atspi_table_cell_base_init,
      (GBaseFinalizeFunc) NULL,
    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtspiTableCell", &tinfo, 0);

  }
  return type;
}
