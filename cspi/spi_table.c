int
SpiAccessibleTable_ref (SpiAccessibleTable *obj)
{
  Accessibility_SpiTable_ref (*obj, &ev);
  return 0;
}



int
SpiAccessibleTable_unref (SpiAccessibleTable *obj)
{
  Accessibility_SpiTable_unref (*obj, &ev);
  return 0;
}



SpiAccessible *
SpiAccessibleTable_getCaption (SpiAccessibleTable *obj)
{
  return (SpiAccessible *)
    Accessibility_SpiTable__get_caption (*obj, &ev);
}



SpiAccessible *
SpiAccessibleTable_getSummary (SpiAccessibleTable *obj)
{
  return (SpiAccessible *)
    Accessibility_SpiTable__get_summary (*obj, &ev);
}



long
SpiAccessibleTable_getNRows (SpiAccessibleTable *obj)
{
  return (long)
    Accessibility_SpiTable__get_nRows (*obj, &ev);
}



long
SpiAccessibleTable_getNColumns (SpiAccessibleTable *obj)
{
  return (long)
    Accessibility_SpiTable__get_nColumns (*obj, &ev);
}


SpiAccessible *
SpiAccessibleTable_getAccessibleAt (SpiAccessibleTable *obj,
                                 long row,
                                 long column)
{
  return (SpiAccessible *)
    Accessibility_SpiTable_getAccessibleAt (*obj,
			       (CORBA_long) row, (CORBA_long) column, &ev);
}


long
SpiAccessibleTable_getIndexAt (SpiAccessibleTable *obj,
                            long row,
                            long column)
{
  return (long)
    Accessibility_SpiTable_getIndexAt (*obj,
				    (CORBA_long) row, (CORBA_long) column, &ev);
}



long
SpiAccessibleTable_getRowAtIndex (SpiAccessibleTable *obj,
                               long index)
{
  return (long)
    Accessibility_SpiTable_getRowAtIndex (*obj,
				       (CORBA_long) index, &ev);
}



long
SpiAccessibleTable_getColumnAtIndex (SpiAccessibleTable *obj,
                                  long index)
{
  return (long)
    Accessibility_SpiTable_getColumnAtIndex (*obj,
					  (CORBA_long) index, &ev);
}



char *
SpiAccessibleTable_getRowDescription (SpiAccessibleTable *obj,
				   long row)
{
  return (char *)
    Accessibility_SpiTable_getRowDescription (*obj,
					   (CORBA_long) row, &ev);
}



char *
SpiAccessibleTable_getColumnDescription (SpiAccessibleTable *obj,
				      long column)
{
  return (char *)
    Accessibility_SpiTable_getColumnDescription (*obj,
					      (CORBA_long) column, &ev);
}



long
SpiAccessibleTable_getRowExtentAt (SpiAccessibleTable *obj,
                                long row,
                                long column)
{
  return (long)
    Accessibility_SpiTable_getRowExtentAt (*obj,
					(CORBA_long) row, (CORBA_long) column, &ev);
}



long
SpiAccessibleTable_getColumnExtentAt (SpiAccessibleTable *obj,
                                   long row,
                                   long column)
{
  return (long)
    Accessibility_SpiTable_getColumnExtentAt (*obj,
					(CORBA_long) row, (CORBA_long) column, &ev);
}



SpiAccessible *
SpiAccessibleTable_getRowHeader (SpiAccessibleTable *obj,
			      long row)
{
  return (SpiAccessible *)
    Accessibility_SpiTable_getRowHeader (*obj,
				      (CORBA_long) row, &ev);
}



SpiAccessible *
SpiAccessibleTable_getColumnHeader (SpiAccessibleTable *obj,
				 long column)
{
  return (SpiAccessible *)
    Accessibility_SpiTable_getColumnHeader (*obj,
				      (CORBA_long) column, &ev);
}



long
SpiAccessibleTable_getNSelectedRows (SpiAccessibleTable *obj)
{
  return (long)
    Accessibility_SpiTable__get_nSelectedRows (*obj, &ev);
}



long
SpiAccessibleTable_getSelectedRows (SpiAccessibleTable *obj,
                                 long **selectedRows)
{
  Accessibility_LongSeq *rows = Accessibility_SpiTable_getSelectedRows (*obj, &ev);
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



long
SpiAccessibleTable_getNSelectedColumns (SpiAccessibleTable *obj)
{
  return (long)
    Accessibility_SpiTable__get_nSelectedColumns (*obj, &ev);
}


long
SpiAccessibleTable_getSelectedColumns (SpiAccessibleTable *obj,
                                    long **selectedColumns)
{
  Accessibility_LongSeq *columns = Accessibility_SpiTable_getSelectedColumns (*obj, &ev);
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


boolean
SpiAccessibleTable_isRowSelected (SpiAccessibleTable *obj,
                               long row)
{
  return (boolean)
    Accessibility_SpiTable_isRowSelected (*obj,
				       (CORBA_long) row, &ev);
}



boolean
SpiAccessibleTable_isColumnSelected (SpiAccessibleTable *obj,
                                  long column)
{
  return (boolean)
    Accessibility_SpiTable_isColumnSelected (*obj,
				       (CORBA_long) column, &ev);
}



boolean
SpiAccessibleTable_isSelected (SpiAccessibleTable *obj,
                            long row,
                            long column)
{
return (boolean)
  Accessibility_SpiTable_isSelected (*obj,
				  (CORBA_long) row, (CORBA_long) column, &ev);
}

