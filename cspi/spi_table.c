/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
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

#include <stdlib.h> /* for malloc */
#include <cspi/spi-private.h>

/**
 * AccessibleTable_ref:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Increment the reference count for an #AccessibleTable object.
 **/
void
AccessibleTable_ref (AccessibleTable *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleTable_unref:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Decrement the reference count for an #AccessibleTable object.
 **/
void
AccessibleTable_unref (AccessibleTable *obj)
{
  cspi_object_unref (obj);
}

/**
 * AccessibleTable_getCaption:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Get an accessible representation of the caption for an #AccessibleTable.
 *
 * Returns: an #Accessible object that serves as the table's caption.
 **/
Accessible *
AccessibleTable_getCaption (AccessibleTable *obj)
{
  char *path;
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_get_property (obj, spi_interface_table, "caption", NULL, "o", &path);
  cspi_return_val_if_ev ("getCaption", NULL);
  retval =  cspi_ref_related_accessible (obj, path);
  g_free (path);
  return retval;
}

/**
 * AccessibleTable_getSummary:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Get an accessible object which summarizes the contents of an #AccessibleTable.
 *
 * Returns: an #Accessible object that serves as the table's summary (often a
 *          reduced #AccessibleTable).
 **/
Accessible *
AccessibleTable_getSummary (AccessibleTable *obj)
{
  char *path;
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_get_property (obj, spi_interface_table, "summary", NULL, "o", &path);
  cspi_return_val_if_ev ("getSummary", NULL);
  retval =  cspi_ref_related_accessible (obj, path);
  g_free (path);
 return retval;
}

/**
 * AccessibleTable_getNRows:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Get the number of rows in an #AccessibleTable,
 *        exclusive of any rows that are programmatically hidden, but inclusive
 *        of rows that may be outside of the current scrolling window or viewport.
 *
 * Returns: a #long integer indicating the number of rows in the table.
 **/
long
AccessibleTable_getNRows (AccessibleTable *obj)
{
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_get_property (obj, spi_interface_table, "nRows", NULL, "i", &retval);
	  
  cspi_return_val_if_ev ("getNRows", -1);

  return retval;
	  
}

/**
 * AccessibleTable_getNColumns:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Get the number of columns in an #AccessibleTable,
 *        exclusive of any columns that are programmatically hidden, but inclusive
 *        of columns that may be outside of the current scrolling window or viewport.
 *
 * Returns: a #long integer indicating the number of columns in the table.
 **/
long
AccessibleTable_getNColumns (AccessibleTable *obj)
{
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_get_property (obj, spi_interface_table, "nColumns", NULL, "i", &retval);
	  
  cspi_return_val_if_ev ("getNColumns", -1);

  return retval;
}

/**
 * AccessibleTable_getAccessibleAt:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the specified table row, zero-indexed.
 * @column: the specified table column, zero-indexed.
 *
 * Get the table cell at the specified row and column indices.
 *          To get the accessible object at a particular (x, y) screen coordinate,
 *          use #Accessible_getAccessibleAtPoint ().
 *
 * Returns: an #Accessible object representing the specified table cell.
 **/
Accessible *
AccessibleTable_getAccessibleAt (AccessibleTable *obj,
                                 long int row,
                                 long int column)
{
  dbus_int32_t d_row = row, d_column = column;
  char *path;
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_table, "getAccessibleAt", NULL, "ii=>o", &d_row, &d_column, &path);
  cspi_return_val_if_ev ("getAccessibleAt", NULL);
  retval = cspi_ref_related_accessible (obj, path);
  g_free (path);
 return retval;
}

/**
 * AccessibleTable_getIndexAt:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the specified table row, zero-indexed.
 * @column: the specified table column, zero-indexed.
 *
 * Get the 1-D child index corresponding to the specified 2-D row and column indices.
 *          To get the accessible object at a particular (x, y) screen coordinate,
 *          use #Accessible_getAccessibleAtPoint ().
 * @see #AccessibleTable_getRowAtIndex(), #AccessibleTable_getColumnAtIndex()
 *
 * Returns: a long integer which serves as the index of a specified cell in the
 *          table, in a form usable by #Accessible_getChildAtIndex().
 **/
long
AccessibleTable_getIndexAt (AccessibleTable *obj,
                            long int row,
                            long int column)
{
  dbus_int32_t d_row = row, d_column = column;
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_call (obj, spi_interface_table, "getIndexAt", NULL, "ii=>i", d_row, d_column, &retval);
	  
  cspi_return_val_if_ev ("getIndexAt", -1);

  return retval;
}

/**
 * AccessibleTable_getRowAtIndex:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @index: the specified child index, zero-indexed.
 *
 * Get the table row index occupied by the child at a particular 1-D child index.
 *
 * @see #AccessibleTable_getIndexAt(), #AccessibleTable_getColumnAtIndex()
 *
 * Returns: a long integer indicating the first row spanned by the child of a
 *          table, at the specified 1-D (zero-offset) @index.
 **/
long
AccessibleTable_getRowAtIndex (AccessibleTable *obj,
                               long index)
{
  dbus_int32_t d_index = index;
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_call (obj, spi_interface_table, "getRowAtIndex", NULL, "i=>i", d_index, &retval);
	  
  cspi_return_val_if_ev ("getRowAtIndex", -1);

  return retval;
}

/**
 * AccessibleTable_getColumnAtIndex:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @index: the specified child index, zero-indexed.
 *
 * Get the table column index occupied by the child at a particular 1-D child index.
 *
 * @see #AccessibleTable_getIndexAt(), #AccessibleTable_getRowAtIndex()
 *
 * Returns: a long integer indicating the first column spanned by the child of a
 *          table, at the specified 1-D (zero-offset) @index.
 **/
long
AccessibleTable_getColumnAtIndex (AccessibleTable *obj,
                                  long index)
{
  dbus_int32_t d_index = index;
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_call (obj, spi_interface_table, "getColumnAtIndex", NULL, "i=>i", d_index, &retval);
	  
  cspi_return_val_if_ev ("getColumnAtIndex", -1);

  return retval;
}

/**
 * AccessibleTable_getRowDescription:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the specified table row, zero-indexed.
 *
 * Get a text description of a particular table row.  This differs from
 * AccessibleTable_getRowHeader, which returns an #Accessible.
 *
 * Returns: a UTF-8 string describing the specified table row, if available.
 **/
char *
AccessibleTable_getRowDescription (AccessibleTable *obj,
				   long int         row)
{
  dbus_int32_t d_row = row;
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_table, "getRowDescription", NULL, "i=>s", d_row, &retval);
	  
  cspi_return_val_if_ev ("getRowDescription", NULL);

  return retval;
}

/**
 * AccessibleTable_getColumnDescription:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @column: the specified table column, zero-indexed.
 *
 * Get a text description of a particular table column.  This differs from
 * AccessibleTable_getColumnHeader, which returns an #Accessible.
 *
 * Returns: a UTF-8 string describing the specified table column, if available.
 **/
char *
AccessibleTable_getColumnDescription (AccessibleTable *obj,
				      long int         column)
{
  dbus_int32_t d_column = column;
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_table, "getColumnDescription", NULL, "i=>s", d_column, &retval);

  cspi_return_val_if_ev ("getColumnDescription", NULL);

  return retval;
}

/**
 * AccessibleTable_getRowExtentAt:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the specified table row, zero-indexed.
 * @column: the specified table column, zero-indexed.
 *
 * Get the number of rows spanned by the table cell at the specific row and column.
 * (some tables can have cells which span multiple rows and/or columns).
 *
 * Returns: a long integer indicating the number of rows spanned by the specified cell.
 **/
long
AccessibleTable_getRowExtentAt (AccessibleTable *obj,
                                long int         row,
                                long int         column)
{
  dbus_int32_t d_row = row, d_column = column;
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_call (obj, spi_interface_table, "getRowExtentAt", NULL, "ii=>i", d_row, d_column, &retval);
	  
  cspi_return_val_if_ev ("getRowExtentAt", -1);

  return retval;
}

/**
 * AccessibleTable_getColumnExtentAt:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the specified table row, zero-indexed.
 * @column: the specified table column, zero-indexed.
 *
 * Get the number of columns spanned by the table cell at the specific row and column.
 * (some tables can have cells which span multiple rows and/or columns).
 *
 * Returns: a long integer indicating the number of columns spanned by the specified cell.
 **/
long
AccessibleTable_getColumnExtentAt (AccessibleTable *obj,
                                   long int         row,
                                   long int         column)
{
  dbus_int32_t d_row = row, d_column = column;
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_call (obj, spi_interface_table, "getColumnExtentAt", NULL, "ii=>i", d_row, d_column, &retval);
	  
  cspi_return_val_if_ev ("getColumnExtentAt", -1);

  return retval;
}

/**
 * AccessibleTable_getRowHeader:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the specified table row, zero-indexed.
 *
 * Get the header associated with a table row, if available.  This differs from
 * AccessibleTable_getRowDescription, which returns a string.
 *
 * Returns: a #Accessible representatin of the specified table row, if available.
 **/
Accessible *
AccessibleTable_getRowHeader (AccessibleTable *obj,
			      long int         row)
{
  dbus_int32_t d_row = row;
  char *path;
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_table, "getRowHeader", NULL, "i=>o", d_row, &path);
  cspi_return_val_if_ev ("getRowHeader", NULL);
  retval = cspi_ref_related_accessible (obj, path);
  g_free (path);

 return retval;
}

/**
 * AccessibleTable_getColumnHeader:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @column: the specified table column, zero-indexed.
 *
 * Get the header associated with a table column, if available.  This differs from
 * AccessibleTable_getColumnDescription, which returns a string.
 *
 * Returns: a #Accessible representatin of the specified table column, if available.
 **/
Accessible *
AccessibleTable_getColumnHeader (AccessibleTable *obj,
				 long int column)
{
  dbus_int32_t d_column = column;
  char *path;
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_table, "getColumnHeader", NULL, "i=>o", d_column, &path);
  cspi_return_val_if_ev ("getColumnHeader", NULL);
  retval = cspi_ref_related_accessible (obj, path);
  g_free (path);

  return retval;
}

/**
 * AccessibleTable_getNSelectedRows:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Query a table to find out how many rows are currently selected.  Not all tables
 *  support row selection.
 *
 * Returns: a long integer indicating the number of rows currently selected.
 **/
long
AccessibleTable_getNSelectedRows (AccessibleTable *obj)
{
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_get_property (obj, spi_interface_table, "nSelectedRows", NULL, "i", &retval);
	  
  cspi_return_val_if_ev ("getNSelectedRows", -1);

  return retval;
}

static long
cspi_long_seq_to_array (GArray *seq, long int **array)
{
  long *j;
  long length, i;

  if (!cspi_check_ev ("getSelectionItems"))
    {
      *array = NULL;
      return 0;
    }

  length = seq->len;

  j = *array = malloc (sizeof (long) * length);

  for (i = 0; i < length; i++)
    {
      j[i] = g_array_index (seq, long, i);
    }

  g_array_free (seq, TRUE);

  return length;
}

/**
 * AccessibleTable_getSelectedRows:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @selectedRows: a doubly indirected pointer which will be set to the address
 *       of an array of long integers, specifying which rows are currently selected.
 *
 * Query a table for a list of indices of rows which are currently selected.
 *
 * Returns: a long integer indicating the length of the array returned in @selectedRows.
 **/
long
AccessibleTable_getSelectedRows (AccessibleTable *obj,
                                 long int       **selectedRows)
{
  GArray *rows;

  *selectedRows = NULL;

  cspi_return_val_if_fail (obj != NULL, 0);

  cspi_dbus_call (obj, spi_interface_table, "getSelectedRows", NULL, "=>ai", &rows);

  cspi_return_val_if_ev ("getSelectedRows", -1);

  return cspi_long_seq_to_array (rows, selectedRows);
}

/**
 * AccessibleTable_getNSelectedColumns:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Query a table to find out how many columns are currently selected.  Not all tables
 *  support column selection.
 *
 * Returns: a long integer indicating the number of columns currently selected.
 **/
long
AccessibleTable_getNSelectedColumns (AccessibleTable *obj)
{
  dbus_int32_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_get_property (obj, spi_interface_table, "nSelectedColumns", NULL, "i", &retval);
	  
  cspi_return_val_if_ev ("getNSelectedColumns", -1);

  return retval;
}

/**
 * AccessibleTable_getSelectedColumns:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @selectedColumns: a doubly indirected pointer which will be set to the address
 *       of an array of long integers, specifying which columns are currently selected.
 *
 * Query a table for a list of indices of columns which are currently selected.
 *       Not all tables support column selection.
 *
 * Returns: a long integer indicating the length of the array returned in @selectedColumns.
 **/
long
AccessibleTable_getSelectedColumns (AccessibleTable *obj,
                                    long int       **selectedColumns)
{
  GArray *columns;

  *selectedColumns = NULL;

  cspi_return_val_if_fail (obj != NULL, 0);

  cspi_dbus_call (obj, spi_interface_table, "getSelectedColumns", NULL, "=>ai", &columns);

  cspi_return_val_if_ev ("getSelectedColumns", -1);
  return cspi_long_seq_to_array (columns, selectedColumns);
}

/**
 * AccessibleTable_isRowSelected:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the zero-indexed row number of the row being queried.
 *
 * Determine whether a table row is selected.  Not all tables support row selection.
 *
 * Returns: #TRUE if the specified row is currently selected, #FALSE if not.
 **/
SPIBoolean
AccessibleTable_isRowSelected (AccessibleTable *obj,
                               long int         row)
{
  dbus_int32_t d_row = row;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_table, "isRowSelected", NULL, "i=>b", d_row, &retval);

  cspi_return_val_if_ev ("isRowSelected", FALSE);

  return retval;
}

/**
 * AccessibleTable_isColumnSelected:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @column: the zero-indexed column number of the column being queried.
 *
 * Determine whether specified table column is selected.
 * Not all tables support column selection.
 *
 * Returns: #TRUE if the specified column is currently selected, #FALSE if not.
 **/
SPIBoolean
AccessibleTable_isColumnSelected (AccessibleTable *obj,
                                  long int         column)
{
  dbus_int32_t d_column = column;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_table, "isColumnSelected", NULL, "i=>b", d_column, &retval);
	  
  cspi_return_val_if_ev ("isColumnSelected", FALSE);

  return retval;
}

/**
 * AccessibleTable_addRowSelection:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the zero-indexed row number of the row being selected.
 *
 * Select the specified row, adding it to the current row selection.
 * Not all tables support row selection.
 *
 * Returns: #TRUE if the specified row was successfully selected, #FALSE if not.
 **/
SPIBoolean
AccessibleTable_addRowSelection (AccessibleTable *obj,
				 long int         row)
{
  dbus_int32_t d_row = row;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_table, "addRowSelection", NULL, "i=>b", d_row, &retval);
	  
  cspi_return_val_if_ev ("addRowSelection", FALSE);

  return retval;
}

/**
 * AccessibleTable_addColumnSelection:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @column: the zero-indexed column number of the column being selected.
 *
 * Select the specified column, adding it to the current column selection.
 * Not all tables support column selection.
 *
 * Returns: #TRUE if the specified column was successfully selected, #FALSE if not.
 **/
SPIBoolean
AccessibleTable_addColumnSelection (AccessibleTable *obj,
				    long int         column)
{
  dbus_int32_t d_column = column;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_table, "addColumnSelection", NULL, "i=>b", d_column, &retval);
	  
  cspi_return_val_if_ev ("addColumnSelection", FALSE);

  return retval;
}

/**
 * AccessibleTable_removeRowSelection:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the zero-indexed number of the row being deselected.
 *
 * De-select the specified row, removing it to the current row selection.
 * Not all tables support row selection.
 *
 * Returns: #TRUE if the specified row was successfully de-selected, #FALSE if not.
 **/
SPIBoolean
AccessibleTable_removeRowSelection (AccessibleTable *obj,
				    long int         row)
{
  dbus_int32_t d_row = row;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_table, "removeRowSelection", NULL, "i=>b", d_row, &retval);
	  
  cspi_return_val_if_ev ("removeRowSelection", FALSE);

  return retval;
}

/**
 * AccessibleTable_removeColumnSelection:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @column: the zero-indexed column number of the column being de-selected.
 *
 * De-select the specified column, removing it to the current column selection.
 * Not all tables support column selection.
 *
 * Returns: #TRUE if the specified column was successfully de-selected, #FALSE if not.
 **/
SPIBoolean
AccessibleTable_removeColumnSelection (AccessibleTable *obj,
				       long int         column)
{
  dbus_int32_t d_column = column;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_table, "removeColumnSelection", NULL, "i=>b", d_column, &retval);
	  
  cspi_return_val_if_ev ("removeColumnSelection", FALSE);

  return retval;
}

/**
 * AccessibleTable_getRowColumnExtentsAtIndex:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @index: the index of the Table child whose row/column 
 * extents are requested.
 * @row: back-filled with the first table row associated with
 * the cell with child index \c index.
 * @col: back-filled with the first table column associated 
 * with the cell with child index \c index.
 * @row_extents: back-filled with the number of table rows 
 * across which child \c i extends.
 * @col_extents: back-filled with the number of table columns
 * across which child \c i extends.
 * @is_selected: a boolean which is back-filled with \c True
 * if the child at index \c i corresponds to a selected table cell,
 * \c False otherwise.
 *
 * Given a child index, determine the row and column indices and 
 * extents, and whether the cell is currently selected.  If
 * the child at \c index is not a cell (for instance, if it is 
 * a summary, caption, etc.), \c False is returned.
 *
 * Example:
 * If the Table child at index '6' extends across columns 5 and 6 of
 * row 2 of a Table instance, and is currently selected, then
 * 
 * retval = table::getRowColumnExtentsAtIndex (6, row, col, 
 *                                             row_extents,
 *                                             col_extents,
 *                                             is_selected);
 * 
 * will return True, and after the call
 * row, col, row_extents, col_extents,
 * and \c is_selected will contain 2, 5, 1, 2, and 
 * True, respectively.
 *
 * Returns: \c True if the index is associated with a valid table
 * cell, \c False if the index does not correspond to a cell.  If 
 * \c False is returned, the values of the out parameters are 
 * undefined.
 * 
 * Since AT-SPI 1.7.0
 **/
SPIBoolean
AccessibleTable_getRowColumnExtentsAtIndex (AccessibleTable *obj,
					    long int index, long int *row, long int *col, 
					    long int *row_extents, long int *col_extents, 
					    long int *is_selected)
{
  dbus_int32_t d_index = index;
  dbus_bool_t retval;
  dbus_int32_t d_row,  d_col, d_row_extents, d_col_extents; 
  dbus_bool_t d_is_selected;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_table, "getRowColumnExtentsAtIndex", NULL, "i=>iiiibb", d_index, &d_row, &d_col, &d_row_extents, &d_col_extents, &d_is_selected, &retval);

  if (!cspi_check_ev ("getRowColumnExtentsAtIndex")){
    
    *row = 0;
    *col = 0;
    *row_extents = 0;
    *col_extents = 0;
    *is_selected = FALSE;
    retval = FALSE;
  }

  else {
    *row = d_row;
    *col = d_col;
    *row_extents = d_row_extents;;
    *col_extents = d_col_extents;
    *is_selected = d_is_selected;;
  }
  
  return retval;
}


/**
 * AccessibleTable_isSelected:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row: the zero-indexed row of the cell being queried.
 * @column: the zero-indexed column of the cell being queried.
 *
 * Determine whether the cell at a specific row and column is selected.
 *
 * Returns: #TRUE if the specified cell is currently selected, #FALSE if not.
 **/
SPIBoolean
AccessibleTable_isSelected (AccessibleTable *obj,
                            long int row,
                            long int column)
{
  dbus_int32_t d_row = row, d_column = column;
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_table, "isSelected", NULL, "ii=>b", d_row, d_column, &retval);
	  
  cspi_return_val_if_ev ("isSelected", FALSE);

  return retval;
}

