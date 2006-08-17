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
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval =  cspi_object_add (
			     Accessibility_Table__get_caption (CSPI_OBJREF (obj), cspi_ev ()));
  cspi_return_val_if_ev ("getCaption", NULL);
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
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

retval = cspi_object_add (
			  Accessibility_Table__get_summary (CSPI_OBJREF (obj), cspi_ev ()));
 cspi_return_val_if_ev ("getSummary", NULL);
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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Table__get_nRows (CSPI_OBJREF (obj), cspi_ev ());
	  
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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Table__get_nColumns (CSPI_OBJREF (obj), cspi_ev ());
	  
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
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = cspi_object_add (
			    Accessibility_Table_getAccessibleAt (
								 CSPI_OBJREF (obj), row,
								 column, cspi_ev ()));
  cspi_return_val_if_ev ("getAccessibleAt", NULL);
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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Table_getIndexAt (
      CSPI_OBJREF (obj), row,
      column, cspi_ev ());
	  
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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Table_getRowAtIndex (CSPI_OBJREF (obj),
				       index, cspi_ev ());
	  
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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Table_getColumnAtIndex (CSPI_OBJREF (obj),
					  index, cspi_ev ());
	  
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
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval =
    Accessibility_Table_getRowDescription (CSPI_OBJREF (obj),
					   row, cspi_ev ());
	  
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
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval =
    Accessibility_Table_getColumnDescription (CSPI_OBJREF (obj),
					      column, cspi_ev ());

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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Table_getRowExtentAt (
      CSPI_OBJREF (obj), row,
      column, cspi_ev ());
	  
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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Table_getColumnExtentAt (
      CSPI_OBJREF (obj), row,
      column, cspi_ev ());
	  
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
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = cspi_object_add (
			    Accessibility_Table_getRowHeader (CSPI_OBJREF (obj),
							      row, cspi_ev ()));
  cspi_return_val_if_ev ("getRowHeader", NULL);

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
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = cspi_object_add (
			    Accessibility_Table_getColumnHeader (CSPI_OBJREF (obj),
								 column, cspi_ev ()));
  cspi_return_val_if_ev ("getColumnHeader", NULL);

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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Table__get_nSelectedRows (CSPI_OBJREF (obj), cspi_ev ());
	  
  cspi_return_val_if_ev ("getNSelectedRows", -1);

  return retval;
}

static long
cspi_long_seq_to_array (Accessibility_LongSeq *seq, long int **array)
{
  long *j;
  long length, i;

  if (!cspi_check_ev ("getSelectionItems"))
    {
      *array = NULL;
      return 0;
    }

  length = seq->_length;

  j = *array = malloc (sizeof (long) * length);

  for (i = 0; i < length; i++)
    {
      j[i] = seq->_buffer [i];
    }

  CORBA_free (seq);

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
  Accessibility_LongSeq *rows;

  *selectedRows = NULL;

  cspi_return_val_if_fail (obj != NULL, 0);

  rows = Accessibility_Table_getSelectedRows (CSPI_OBJREF (obj), cspi_ev ());

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
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval =
    Accessibility_Table__get_nSelectedColumns (CSPI_OBJREF (obj), cspi_ev ());
	  
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
  Accessibility_LongSeq *columns;

  *selectedColumns = NULL;

  cspi_return_val_if_fail (obj != NULL, 0);

  columns = Accessibility_Table_getSelectedColumns (CSPI_OBJREF (obj), cspi_ev ());

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
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Table_isRowSelected (CSPI_OBJREF (obj),
				       row, cspi_ev ());

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
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Table_isColumnSelected (CSPI_OBJREF (obj),
					  column, cspi_ev ());
	  
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
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Table_addRowSelection (CSPI_OBJREF (obj),
					 row, cspi_ev ());
	  
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
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Table_addColumnSelection (CSPI_OBJREF (obj),
					    column, cspi_ev ());
	  
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
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Table_removeRowSelection (CSPI_OBJREF (obj),
					    row, cspi_ev ());
	  
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
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Table_removeColumnSelection (CSPI_OBJREF (obj),
					       column, cspi_ev ());
	  
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
					    long int *is_selected){
  SPIBoolean retval;
  CORBA_long cRow,  cCol, cRow_extents, cCol_extents; 
  CORBA_boolean cIs_selected;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_Table_getRowColumnExtentsAtIndex (CSPI_OBJREF (obj),
							   index, &cRow, &cCol,
							   &cRow_extents, &cCol_extents,
							   &cIs_selected,
							   cspi_ev ());

  if (!cspi_check_ev ("getRowColumnExtentsAtIndex")){
    
    *row = 0;
    *col = 0;
    *row_extents = 0;
    *col_extents = 0;
    *is_selected = FALSE;
    retval = FALSE;
  }

  else {
    *row = cRow;
    *col = cCol;
    *row_extents = cRow_extents;;
    *col_extents = cCol_extents;
    *is_selected = cIs_selected;;
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
  SPIBoolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval =
    Accessibility_Table_isSelected (CSPI_OBJREF (obj),
				    row,
				    column, cspi_ev ());
	  
  cspi_return_val_if_ev ("isSelected", FALSE);

  return retval;
}

