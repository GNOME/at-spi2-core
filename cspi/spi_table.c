int
AccessibleTable_ref (AccessibleTable *obj)
{
  Accessibility_Table_ref (*obj, &ev);
  return 0;
}



int
AccessibleTable_unref (AccessibleTable *obj)
{
  Accessibility_Table_unref (*obj, &ev);
  return 0;
}



Accessible *
AccessibleTable_getCaption (AccessibleTable *obj)
{
  return (Accessible *)
    Accessibility_Table__get_caption (*obj, &ev);
}



Accessible *
AccessibleTable_getSummary (AccessibleTable *obj)
{
  return (Accessible *)
    Accessibility_Table__get_summary (*obj, &ev);
}



long
AccessibleTable_getNRows (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table__get_nRows (*obj, &ev);
}



long
AccessibleTable_getNColumns (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table__get_nColumns (*obj, &ev);
}


Accessible *
AccessibleTable_getAccessibleAt (AccessibleTable *obj,
                                 long row,
                                 long column)
{
  return (Accessible *)
    Accessibility_Table_getAccessibleAt (*obj,
			       (CORBA_long) row, (CORBA_long) column, &ev);
}


long
AccessibleTable_getIndexAt (AccessibleTable *obj,
                            long row,
                            long column)
{
  return (long)
    Accessibility_Table_getIndexAt (*obj,
				    (CORBA_long) row, (CORBA_long) column, &ev);
}



long
AccessibleTable_getRowAtIndex (AccessibleTable *obj,
                               long index)
{
  return (long)
    Accessibility_Table_getRowAtIndex (*obj,
				       (CORBA_long) index, &ev);
}



long
AccessibleTable_getColumnAtIndex (AccessibleTable *obj,
                                  long index)
{
  return (long)
    Accessibility_Table_getColumnAtIndex (*obj,
					  (CORBA_long) index, &ev);
}



char *
AccessibleTable_getRowDescription (AccessibleTable *obj,
				   long row)
{
  return (char *)
    Accessibility_Table_getRowDescription (*obj,
					   (CORBA_long) row, &ev);
}



char *
AccessibleTable_getColumnDescription (AccessibleTable *obj,
				      long column)
{
  return (char *)
    Accessibility_Table_getColumnDescription (*obj,
					      (CORBA_long) column, &ev);
}



long
AccessibleTable_getRowExtentAt (AccessibleTable *obj,
                                long row,
                                long column)
{
  return (long)
    Accessibility_Table_getRowExtentAt (*obj,
					(CORBA_long) row, (CORBA_long) column, &ev);
}



long
AccessibleTable_getColumnExtentAt (AccessibleTable *obj,
                                   long row,
                                   long column)
{
  return (long)
    Accessibility_Table_getColumnExtentAt (*obj,
					(CORBA_long) row, (CORBA_long) column, &ev);
}



Accessible *
AccessibleTable_getRowHeader (AccessibleTable *obj,
			      long row)
{
  return (Accessible *)
    Accessibility_Table_getRowHeader (*obj,
				      (CORBA_long) row, &ev);
}



Accessible *
AccessibleTable_getColumnHeader (AccessibleTable *obj,
				 long column)
{
  return (Accessible *)
    Accessibility_Table_getColumnHeader (*obj,
				      (CORBA_long) column, &ev);
}



long
AccessibleTable_getNSelectedRows (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table__get_nSelectedRows (*obj, &ev);
}



long
AccessibleTable_getSelectedRows (AccessibleTable *obj,
                                 long **selectedRows)
{
  Accessibility_LongSeq *rows = Accessibility_Table_getSelectedRows (*obj, &ev);
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
AccessibleTable_getNSelectedColumns (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table__get_nSelectedColumns (*obj, &ev);
}


long
AccessibleTable_getSelectedColumns (AccessibleTable *obj,
                                    long **selectedColumns)
{
  Accessibility_LongSeq *columns = Accessibility_Table_getSelectedColumns (*obj, &ev);
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
AccessibleTable_isRowSelected (AccessibleTable *obj,
                               long row)
{
  return (boolean)
    Accessibility_Table_isRowSelected (*obj,
				       (CORBA_long) row, &ev);
}



boolean
AccessibleTable_isColumnSelected (AccessibleTable *obj,
                                  long column)
{
  return (boolean)
    Accessibility_Table_isColumnSelected (*obj,
				       (CORBA_long) column, &ev);
}



boolean
AccessibleTable_isSelected (AccessibleTable *obj,
                            long row,
                            long column)
{
return (boolean)
  Accessibility_Table_isSelected (*obj,
				  (CORBA_long) row, (CORBA_long) column, &ev);
}

