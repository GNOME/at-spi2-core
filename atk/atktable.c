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
#include "atkmarshal.h"

enum {
  ROW_INSERTED,
  ROW_DELETED,
  COLUMN_INSERTED,
  COLUMN_DELETED,
  ROW_REORDERED,
  COLUMN_REORDERED,
  LAST_SIGNAL
};

struct _AtkTableIfaceClass
{
  GObjectClass parent;
};

typedef struct _AtkTableIfaceClass AtkTableIfaceClass;

static void atk_table_base_init (gpointer *g_class);

static guint atk_table_signals[LAST_SIGNAL] = { 0 };

GType
atk_table_get_type ()
{
  static GType type = 0;

  if (!type) {
    GTypeInfo tinfo =
    {
      sizeof (AtkTableIface),
      (GBaseInitFunc) atk_table_base_init,
      (GBaseFinalizeFunc) NULL,

    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkTable", &tinfo, 0);
  }

  return type;
}


static void
atk_table_base_init (gpointer *g_class)
{
  static gboolean initialized = FALSE;

  if (! initialized)
  {
    atk_table_signals[ROW_INSERTED] =
      g_signal_newc ("row_inserted",
                     ATK_TYPE_TABLE,
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (AtkTableIface, model_changed),
                     (GSignalAccumulator) NULL, NULL,
                     atk_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     2, G_TYPE_INT, G_TYPE_INT);
    atk_table_signals[COLUMN_INSERTED] =
      g_signal_newc ("column_inserted",
                     ATK_TYPE_TABLE,
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (AtkTableIface, model_changed),
                     (GSignalAccumulator) NULL, NULL,
                     atk_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     2, G_TYPE_INT, G_TYPE_INT);
    atk_table_signals[ROW_DELETED] =
      g_signal_newc ("row_deleted",
                     ATK_TYPE_TABLE,
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (AtkTableIface, model_changed),
                     (GSignalAccumulator) NULL, NULL,
                     atk_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     2, G_TYPE_INT, G_TYPE_INT);
    atk_table_signals[COLUMN_DELETED] =
      g_signal_newc ("column_deleted",
                     ATK_TYPE_TABLE,
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (AtkTableIface, model_changed),
                     (GSignalAccumulator) NULL, NULL,
                     atk_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     2, G_TYPE_INT, G_TYPE_INT);
    atk_table_signals[ROW_REORDERED] =
      g_signal_newc ("row_reordered",
                     ATK_TYPE_TABLE,
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (AtkTableIface, model_changed),
                     (GSignalAccumulator) NULL, NULL,
                     atk_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     0);
    atk_table_signals[COLUMN_REORDERED] =
      g_signal_newc ("column_reordered",
                     ATK_TYPE_TABLE,
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (AtkTableIface, model_changed),
                     (GSignalAccumulator) NULL, NULL,
                     atk_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     0);
    initialized = TRUE;
  }
}

/**
 * atk_table_ref_at:
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 * @column: a #gint representing a column in @table
 *
 * Get a reference to the table cell at @row, @column
 *
 * Returns: a AtkObject* representing the referred to accessible
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
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 * @column: a #gint representing a column in @table
 *
 * Gets a #gint representing the index at the specified @row and @column,
 * or 0 if value does not implement this interface.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a #gint representing the index at specified position, or 0
 * if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableInterface
 * @index: a #gint representing an index in @table
 *
 * Gets a #gint representing the row at the specified @index, or 0
 * if the value does not implement this interface
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gint representing the row at the specified  index, or 0
 * if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableInterface
 * @index: a #gint representing an index in @table
 *
 * Gets a #gint representing the column at the specified @index, or 0
 * if the value does not implement this interface
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gint representing the column at the specified  index, or 0
 * if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableInterface
 *
 * Gets the caption for the @table.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gchar* representing the table caption, or %NULL
 * if value does not implement this interface.
 **/
gchar*
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
 * @table: a GObject instance that implements AtkTableIface
 *
 * Gets the number of columns in the table.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gint representing the number of columns, or 0
 * if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableIface
 * @column: a #gint representing a column in @table
 *
 * Gets the description text of the specified @column in the table
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gchar* representing the column description, or %NULL
 * if value does not implement this interface.
 **/
gchar*
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
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 * @column: a #gint representing a column in @table
 *
 * Gets the number of columns occupied by the accessible object
 * at the specified @row and @column in the @table.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gint representing the column extent at specified position, or 0
 * if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableIface
 * @column: a #gint representing a column in the table
 *
 * Gets the column header of a specified column in an accessible table.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a AtkObject* representing the specified column header, or
 * %NULL if value does not implement this interface.
 **/
AtkObject*
atk_table_get_column_header (AtkTable *obj, gint column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_column_header)
    return (iface->get_column_header) (obj, column);
  else
    return NULL;
}

/**
 * atk_table_get_n_rows:
 * @table: a GObject instance that implements AtkTableIface
 *
 * Gets the number of rows in the table.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gint representing the number of rows, or 0
 * if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableIface
 * @r: a #gint representing a row in @table
 *
 * Gets the description text of the specified row in the table
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gchar* representing the row description, or %NULL
 * if value does not implement this interface.
 **/
gchar*
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
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 * @column: a #gint representing a column in @table
 *
 * Gets the number of rows occupied by the accessible object
 * at a specified @row and @column in the @table.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gint representing the row extent at specified position, or 0
 * if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in the table
 *
 * Gets the row header of a specified row in an accessible table.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a AtkObject* representing the specified row header, or
 * %NULL if value does not implement this interface.
 **/
AtkObject*
atk_table_get_row_header (AtkTable *obj, gint row)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TABLE (obj), NULL);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_row_header)
    return (iface->get_row_header) (obj, row);
  else
    return NULL;
}

/**
 * atk_table_get_summary:
 * @table: a GObject instance that implements AtkTableIface
 *
 * Gets the summary description of the table.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a AtkObject* representing a sumary description of the table,
 * or zero if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableIface
 *
 * Gets the selected rows of the table by initializing **selected with 
 * the selected row numbers. This array should be freed by the caller.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gint representing the number of selected rows,
 * or zero if value does not implement this interface.
 **/
gint
atk_table_get_selected_rows (AtkTable *obj, gint **selected)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_TABLE (obj), 0);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_selected_rows)
    return (iface->get_selected_rows) (obj, selected);
  else
    return 0;
}

/**
 * atk_table_get_selected_columns:
 * @table: a GObject instance that implements AtkTableIface
 *
 * Gets the selected columns of the table by initializing **selected with 
 * the selected column numbers. This array should be freed by the caller.
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gint representing the number of selected columns,
 * or %0 if value does not implement this interface.
 *
 **/
gint 
atk_table_get_selected_columns (AtkTable *obj, gint **selected)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, 0);
  g_return_val_if_fail (ATK_IS_TABLE (obj), 0);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->get_selected_columns)
    return (iface->get_selected_columns) (obj, selected);
  else
    return 0;
}

/**
 * atk_table_is_column_selected:
 * @table: a GObject instance that implements AtkTableIface
 * @column: a #gint representing a column in @table
 *
 * Gets a boolean value indicating whether the specified @column
 * is selected
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gboolean representing if the column is selected, or 0
 * if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 *
 * Gets a boolean value indicating whether the specified @row
 * is selected
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gboolean representing if the row is selected, or 0
 * if value does not implement this interface.
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
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 * @column: a #gint representing a column in @table
 *
 * Gets a boolean value indicating whether the acessible object
 * at the specified @row and @column is selected
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gboolean representing if the cell is selected, or 0
 * if value does not implement this interface.
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
 * atk_table_add_row_selection:
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 *
 * Adds the specified @row to the selection. 
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gboolean representing if row was successfully added to selection,
 * or 0 if value does not implement this interface.
 **/
gboolean
atk_table_add_row_selection (AtkTable *obj,
                       		 gint     row)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TABLE (obj), FALSE);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->add_row_selection)
    return (iface->add_row_selection) (obj, row);
  else
    return FALSE;
}
/**
 * atk_table_remove_row_selection:
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 *
 * Removes the specified @row from the selection. 
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gboolean representing if the row was successfully removed from
 * the selection, or 0 if value does not implement this interface.
 **/
gboolean
atk_table_remove_row_selection (AtkTable *obj,
                       		    gint     row)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TABLE (obj), FALSE);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->remove_row_selection)
    return (iface->remove_row_selection) (obj, row);
  else
    return FALSE;
}
/**
 * atk_table_add_column_selection:
 * @table: a GObject instance that implements AtkTableIface
 * @column: a #gint representing a column in @table
 *
 * Adds the specified @column to the selection. 
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gboolean representing if the column was successfully added to 
 * the selection, or 0 if value does not implement this interface.
 **/
gboolean
atk_table_add_column_selection (AtkTable *obj,
                       		    gint     column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TABLE (obj), FALSE);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->add_column_selection)
    return (iface->add_column_selection) (obj, column);
  else
    return FALSE;
}
/**
 * atk_table_remove_column_selection:
 * @table: a GObject instance that implements AtkTableIface
 * @column: a #gint representing a column in @table
 *
 * Adds the specified @column to the selection. 
 * Note: callers should not rely on %NULL or on a zero value for
 * indication of whether AtkSelectionIface is implemented, they should
 * use type checking/interface checking macros or the
 * atk_get_accessible_table() convenience method.
 *
 * Returns: a gboolean representing if the column was successfully removed from
 * the selection, or 0 if value does not implement this interface.
 **/
gboolean
atk_table_remove_column_selection (AtkTable *obj,
                       			   gint     column)
{
  AtkTableIface *iface;

  g_return_val_if_fail (obj != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TABLE (obj), FALSE);

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->remove_column_selection)
    return (iface->remove_column_selection) (obj, column);
  else
    return FALSE;
}

/**
 * atk_table_set_caption:
 * @table: a GObject instance that implements AtkTableIface
 * @caption: a #gchar representing the caption to set for @table
 *
 * Sets the caption for the table.
 **/
void
atk_table_set_caption (AtkTable       *obj,
                       gchar          *caption)
{
  AtkTableIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_TABLE (obj));

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->set_caption)
    (iface->set_caption) (obj, caption);
}

/**
 * atk_table_set_column_description:
 * @table: a GObject instance that implements AtkTableIface
 * @column: a #gint representing a column in @table
 * @description: a #gchar representing the description text
 * to set for the specified @column of the @table
 *
 * Sets the description text for the specified @column of the @table.
 **/
void
atk_table_set_column_description (AtkTable       *obj,
                                  gint           column,
                                  gchar          *description)
{
  AtkTableIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_TABLE (obj));

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->set_column_description)
    (iface->set_column_description) (obj, column, description);
}

/**
 * atk_table_set_column_header:
 * @table: a GObject instance that implements AtkTableIface
 * @column: a #gint representing a column in @table
 * @header: an #AtkTable
 *
 * Sets the specified column header to @header
 **/
void
atk_table_set_column_header (AtkTable  *obj,
                             gint      column,
                             AtkObject *header)
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
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 * @description: a #gchar representing the description text
 * to set for the specified @row of @table
 *
 * Sets the description text for the specified @row of @table.
 **/
void
atk_table_set_row_description (AtkTable       *obj,
                               gint           row,
                               gchar          *description)
{
  AtkTableIface *iface;

  g_return_if_fail (obj != NULL);
  g_return_if_fail (ATK_IS_TABLE (obj));

  iface = ATK_TABLE_GET_IFACE (obj);

  if (iface->set_row_description)
    (iface->set_row_description) (obj, row, description);
}

/**
 * atk_table_set_row_header:
 * @table: a GObject instance that implements AtkTableIface
 * @row: a #gint representing a row in @table
 * @header: an #AtkTable 
 *
 * Sets the specified row header to @header
 **/
void
atk_table_set_row_header (AtkTable  *obj,
                          gint      row,
                          AtkObject *header)
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
 * @table: a GObject instance that implements AtkTableIface
 * @accessible: an #AtkObject representing the summary description
 * to set for @table
 *
 * Sets the summary description of the table
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
