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
  return (Accessible *)
    Accessibility_Table__get_caption (CSPI_OBJREF (obj), cspi_ev ());
}

/**
 * AccessibleTable_getSummary:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Get an accessible object which summarizes the contents of an #AccessibleTable.
 *
 * Returns: an #Accessible object that serves as the table's summary (often a
 *          reduced #AccessibleTable).
 *
 **/
Accessible *
AccessibleTable_getSummary (AccessibleTable *obj)
{
  return (Accessible *)
    Accessibility_Table__get_summary (CSPI_OBJREF (obj), cspi_ev ());
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
 *
 **/
long
AccessibleTable_getNRows (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table__get_nRows (CSPI_OBJREF (obj), cspi_ev ());
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
 *
 **/
long
AccessibleTable_getNColumns (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table__get_nColumns (CSPI_OBJREF (obj), cspi_ev ());
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
 *
 **/
Accessible *
AccessibleTable_getAccessibleAt (AccessibleTable *obj,
                                 long int row,
                                 long int column)
{
  return (Accessible *)
    Accessibility_Table_getAccessibleAt (CSPI_OBJREF (obj),
			       (CORBA_long) row, (CORBA_long) column, cspi_ev ());
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
 *
 **/
long
AccessibleTable_getIndexAt (AccessibleTable *obj,
                            long int row,
                            long int column)
{
  return (long)
    Accessibility_Table_getIndexAt (CSPI_OBJREF (obj),
				    (CORBA_long) row, (CORBA_long) column, cspi_ev ());
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
 *
 **/
long
AccessibleTable_getRowAtIndex (AccessibleTable *obj,
                               long index)
{
  return (long)
    Accessibility_Table_getRowAtIndex (CSPI_OBJREF (obj),
				       (CORBA_long) index, cspi_ev ());
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
 *
 **/
long
AccessibleTable_getColumnAtIndex (AccessibleTable *obj,
                                  long index)
{
  return (long)
    Accessibility_Table_getColumnAtIndex (CSPI_OBJREF (obj),
					  (CORBA_long) index, cspi_ev ());
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
 *
 **/
char *
AccessibleTable_getRowDescription (AccessibleTable *obj,
				   long int row)
{
  return (char *)
    Accessibility_Table_getRowDescription (CSPI_OBJREF (obj),
					   (CORBA_long) row, cspi_ev ());
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
 *
 **/
char *
AccessibleTable_getColumnDescription (AccessibleTable *obj,
				      long int column)
{
  return (char *)
    Accessibility_Table_getColumnDescription (CSPI_OBJREF (obj),
					      (CORBA_long) column, cspi_ev ());
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
 *
 **/
long
AccessibleTable_getRowExtentAt (AccessibleTable *obj,
                                long int row,
                                long int column)
{
  return (long)
    Accessibility_Table_getRowExtentAt (CSPI_OBJREF (obj),
					(CORBA_long) row, (CORBA_long) column, cspi_ev ());
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
 *
 **/
long
AccessibleTable_getColumnExtentAt (AccessibleTable *obj,
                                   long int row,
                                   long int column)
{
  return (long)
    Accessibility_Table_getColumnExtentAt (CSPI_OBJREF (obj),
					(CORBA_long) row, (CORBA_long) column, cspi_ev ());
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
 *
 **/
Accessible *
AccessibleTable_getRowHeader (AccessibleTable *obj,
			      long int row)
{
  return (Accessible *)
    Accessibility_Table_getRowHeader (CSPI_OBJREF (obj),
				      (CORBA_long) row, cspi_ev ());
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
 *
 **/
Accessible *
AccessibleTable_getColumnHeader (AccessibleTable *obj,
				 long int column)
{
  return (Accessible *)
    Accessibility_Table_getColumnHeader (CSPI_OBJREF (obj),
				      (CORBA_long) column, cspi_ev ());
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
  return (long)
    Accessibility_Table__get_nSelectedRows (CSPI_OBJREF (obj), cspi_ev ());
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
                                 long int **selectedRows)
{
  Accessibility_LongSeq *rows = Accessibility_Table_getSelectedRows (CSPI_OBJREF (obj), cspi_ev ());
  CORBA_long *i;
  long *j;
  long length;

  i = rows->_buffer;
  length = (long) rows->_length;
  j = *selectedRows = (long *) malloc (sizeof(long)*length);
  
  while (length--)
    *j++ = (CORBA_long) (*i++);

  length = rows->_length;
  CORBA_free (rows);
  return length;
}



/**
 * AccessibleTable_getNSelectedColumns:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 *
 * Query a table to find out how many columnss are currently selected.  Not all tables
 *  support column selection.
 *
 * Returns: a long integer indicating the number of columns currently selected.
 **/
long
AccessibleTable_getNSelectedColumns (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table__get_nSelectedColumns (CSPI_OBJREF (obj), cspi_ev ());
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
                                    long int **selectedColumns)
{
  Accessibility_LongSeq *columns = Accessibility_Table_getSelectedColumns (CSPI_OBJREF (obj), cspi_ev ());
  CORBA_long *i;
  long *j;
  long length;

  i = columns->_buffer;
  length = (long) columns->_length;
  j = *selectedColumns = (long *) malloc (sizeof(long)*length);
  
  while (length--)
    *j++ = (CORBA_long) (*i++);

  length = columns->_length;
  CORBA_free (columns);
  return length;
}


/**
 * AccessibleTable_isRowSelected:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row:
 *
 * Determine whether a table row is selected.  Not all tables support row selection.
 *
 * Returns: #TRUE if the specified row is currently selected, #FALSE if not.
 *
 **/
SPIBoolean
AccessibleTable_isRowSelected (AccessibleTable *obj,
                               long int row)
{
  return (SPIBoolean)
    Accessibility_Table_isRowSelected (CSPI_OBJREF (obj),
				       (CORBA_long) row, cspi_ev ());
}



/**
 * AccessibleTable_isColumnSelected:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @column:
 *
 * Determine whether specified table column is selected.
 * Not all tables support column selection.
 *
 * Returns: #TRUE if the specified column is currently selected, #FALSE if not.
 *
 **/
SPIBoolean
AccessibleTable_isColumnSelected (AccessibleTable *obj,
                                  long int column)
{
  return (SPIBoolean)
    Accessibility_Table_isColumnSelected (CSPI_OBJREF (obj),
				       (CORBA_long) column, cspi_ev ());
}



/**
 * AccessibleTable_isSelected:
 * @obj: a pointer to the #AccessibleTable implementor on which to operate.
 * @row:
 * @column:
 *
 * Determine whether the cell at a specific row and column is selected.
 *
 * Returns: #TRUE if the specified cell is currently selected, #FALSE if not.
 *
 **/
SPIBoolean
AccessibleTable_isSelected (AccessibleTable *obj,
                            long int row,
                            long int column)
{
  return (SPIBoolean)
  Accessibility_Table_isSelected (CSPI_OBJREF (obj),
				  (CORBA_long) row, (CORBA_long) column, cspi_ev ());
}

