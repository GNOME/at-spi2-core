/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "atktable.h"

GType
atk_table_get_type ()
{
  static GType type = 0;

  if (!type) {
    GTypeInfo tinfo =
    {
      sizeof (AtkTableIface),
      NULL,
      NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkTable", &tinfo, 0);
  }

  return type;
}

/**
 * atk_table_ref_at:
 * @obj: a GObject instance that implements AtkTableIface
 * return values: a AtkObject* representing the referred to accessible
 *
 * Get a reference to the table cell at #row, #column
 *
 **/
AtkObject*
atk_table_ref_at (AtkTable *obj,
                  gint     row,
                  gint     column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->ref_at)
    return (iface->ref_at) (obj, row, column);
  else
    return NULL;
}

/**
 * atk_table_get_index_at:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gint representing the index at specified position, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gint
atk_table_get_index_at (AtkTable *obj,
                        gint     row,
                        gint     column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_TABLE (obj), 0);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_index_at)
    return (iface->get_index_at) (obj, row, column);
  else
    return 0;
}

/**
 * atk_table_get_row_at_index:
 * @value: a GObject instance that implements AtkTableInterface
 * @return: a gint representing the row at the specified  index, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gint
atk_table_get_row_at_index (AtkTable *obj,
                            gint     index)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_TABLE (obj), 0);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_row_at_index)
    return (iface->get_row_at_index) (obj, index);
  else
    return 0;
}

/**
 * atk_table_get_column_at_index:
 * @value: a GObject instance that implements AtkTableInterface
 * @return: a gint representing the column at the specified  index, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gint
atk_table_get_column_at_index (AtkTable *obj,
                               gint     index)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_TABLE (obj), 0);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_column_at_index)
    return (iface->get_column_at_index) (obj, index);
  else
    return 0;
}

/**
 * atk_table_get_caption:
 * @value: a GObject instance that implements AtkTableInterface
 * @return: a AtkObject* representing the table caption, or NULL
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
AtkObject*
atk_table_get_caption (AtkTable *obj)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_caption)
    return (iface->get_caption) (obj);
  else
    return NULL;
}

/**
 * atk_table_get_n_columns:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gint representing the number of columns, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gint
atk_table_get_n_columns (AtkTable *obj)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_TABLE (obj), 0);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_n_columns)
    return (iface->get_n_columns) (obj);
  else
    return 0;
}

/**
 * atk_table_get_column_description:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a AtkObject* representing the table description, or NULL
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
AtkObject*
atk_table_get_column_description (AtkTable *obj,
                                  gint     column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_column_description)
    return (iface->get_column_description) (obj, column);
  else
    return NULL;
}

/**
 * atk_table_get_column_extent_at:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gint representing the column extent at specified position, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gint
atk_table_get_column_extent_at (AtkTable *obj,
                                gint     row,
                                gint     column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_TABLE (obj), 0);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_column_extent_at)
    return (iface->get_column_extent_at) (obj, row, column);
  else
    return 0;
}

/**
 * atk_table_get_column_header:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a AtkObject* representing the column headers, or NULL
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
AtkTable*
atk_table_get_column_header (AtkTable *obj)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_column_header)
    return (iface->get_column_header) (obj);
  else
    return NULL;
}

/**
 * atk_table_get_n_rows:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gint representing the number of rows, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gint
atk_table_get_n_rows (AtkTable *obj)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_TABLE (obj), 0);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_n_rows)
    return (iface->get_n_rows) (obj);
  else
    return 0;
}

/**
 * atk_table_get_row_description:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a AtkObject* representing the table description, or NULL
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
AtkObject*
atk_table_get_row_description (AtkTable *obj,
                               gint      row)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_row_description)
    return (iface->get_row_description) (obj, row);
  else
    return NULL;
}

/**
 * atk_table_get_row_extent_at:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gint representing the row extent at specified position, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gint
atk_table_get_row_extent_at (AtkTable *obj,
                             gint     row,
                             gint     column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_TABLE (obj), 0);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_row_extent_at)
    return (iface->get_row_extent_at) (obj, row, column);
  else
    return 0;
}

/**
 * atk_table_get_row_header:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a AtkTable* representing the row headers, or NULL
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
AtkTable*
atk_table_get_row_header (AtkTable *obj)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_row_header)
    return (iface->get_row_header) (obj);
  else
    return NULL;
}

/**
 * atk_table_get_summary:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a AtkObject* representing a sumary description of the table,
 * or NULL if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
AtkObject*
atk_table_get_summary (AtkTable *obj)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_summary)
    return (iface->get_summary) (obj);
  else
    return NULL;
}

/**
 * atk_table_get_selected_rows:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gint* representing the selected rows,
 * or NULL if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gint*
atk_table_get_selected_rows (AtkTable *obj)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_selected_rows)
    return (iface->get_selected_rows) (obj);
  else
    return NULL;
}

/**
 * atk_table_get_selected_columns:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gint* representing the selected columns,
 * or NULL if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gint*
atk_table_get_selected_columns (AtkTable *obj)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_selected_columns)
    return (iface->get_selected_columns) (obj);
  else
    return NULL;
}

/**
 * atk_table_is_column_selected:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gboolean representing the column is selected, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gboolean
atk_table_is_column_selected (AtkTable *obj,
                              gint     column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TABLE (obj), FALSE);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->is_column_selected)
    return (iface->is_column_selected) (obj, column);
  else
    return FALSE;
}

/**
 * atk_table_is_row_selected:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gboolean representing the row is selected, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gboolean
atk_table_is_row_selected (AtkTable *obj,
                           gint     row)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TABLE (obj), FALSE);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->is_row_selected)
    return (iface->is_row_selected) (obj, row);
  else
    return FALSE;
}

/**
 * atk_table_is_selected:
 * @value: a GObject instance that implements AtkTableIface
 * @return: a gboolean representing the cell is selected, or 0
 * if value does not implement this interface.
 *
 * WARNING: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 **/
gboolean
atk_table_is_selected (AtkTable *obj,
                       gint     row,
                       gint     column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TABLE (obj), FALSE);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->is_selected)
    return (iface->is_selected) (obj, row, column);
  else
    return FALSE;
}

/**
 * atk_table_set_caption:
 * @value: a GObject instance that implements AtkTableIface
 * @return: void
 **/
void
atk_table_set_caption (AtkTable       *obj,
                       AtkObject      *accessible)
{
  AtkTableIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_TABLE (obj));

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->set_caption)
    (iface->set_caption) (obj, accessible);
}

/**
 * atk_table_set_column_description:
 * @value: a GObject instance that implements AtkTableIface
 * @return: void
 **/
void
atk_table_set_column_description (AtkTable       *obj,
                                  gint           column,
                                  AtkObject      *accessible)
{
  AtkTableIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_TABLE (obj));

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->set_column_description)
    (iface->set_column_description) (obj, column, accessible);
}

/**
 * atk_table_set_column_header:
 * @value: a GObject instance that implements AtkTableIface
 * @return: void
 **/
void
atk_table_set_column_header (AtkTable *obj,
                             gint     column,
                             AtkTable *header)
{
  AtkTableIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_TABLE (obj));

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->set_column_header)
    (iface->set_column_header) (obj, column, header);
}

/**
 * atk_table_set_row_description:
 * @value: a GObject instance that implements AtkTableIface
 * @return: void
 **/
void
atk_table_set_row_description (AtkTable       *obj,
                               gint           row,
                               AtkObject      *accessible)
{
  AtkTableIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_TABLE (obj));

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->set_row_description)
    (iface->set_row_description) (obj, row, accessible);
}

/**
 * atk_table_set_row_header:
 * @value: a GObject instance that implements AtkTableIface
 * @return: void
 **/
void
atk_table_set_row_header (AtkTable *obj,
                          gint     row,
                          AtkTable *header)
{
  AtkTableIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_TABLE (obj));

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->set_row_header)
    (iface->set_row_header) (obj, row, header);
}

/**
 * atk_table_set_summary:
 * @value: a GObject instance that implements AtkTableIface
 * @return: void
 **/
void
atk_table_set_summary (AtkTable       *obj,
                       AtkObject      *accessible)
{
  AtkTableIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_TABLE (obj));

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->set_summary)
    (iface->set_summary) (obj, accessible);
}
