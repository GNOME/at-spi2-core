#include "spi-util.h"

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



char *
AccessibleTable_getCaption (AccessibleTable *obj)
{
  return string_from_corba_string (
				   Accessibility_Table_getCaption (*obj, &ev));
}



Accessible *
AccessibleTable_getSummary (AccessibleTable *obj)
{
  return (Accessible *)
    Accessibility_Table_getSummary (*obj, &ev);
}



long
AccessibleTable_getNRows (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table_getNRows (*obj, &ev);
}



long
AccessibleTable_getNColumns (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table_getNColumns (*obj, &ev);
}


Accessible *
AccessibleTable_refAt (AccessibleTable *obj,
                                 long row,
                                 long column)
{
  return (Accessible *)
    Accessibility_Table_refAt (*obj,
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
  return string_from_corba_string (
				   Accessibility_Table_getRowDescription (*obj,
									  (CORBA_long) row, &ev));
}



char *
AccessibleTable_getColumnDescription (AccessibleTable *obj,
				      long column)
{
  return string_from_corba_string (
				   Accessibility_Table_getColumnDescription (*obj,
									     (CORBA_long) column, &ev));
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
				 long column);
{
  return (Accessible *)
    Accessibility_Table_getColumnHeader (*obj,
				      (CORBA_long) column, &ev);
}



long
AccessibleTable_getNSelectedRows (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table_getNSelectedRows (*obj, &ev);
}



void
AccessibleTable_getSelectedRows (AccessibleTable *obj,
                                 long **selectedRows)
{
  Accessibility_Table_getSelectedRows (*obj,
				      (CORBA_long **) selectedRows, &ev);
}



long
AccessibleTable_getNSelectedColumns (AccessibleTable *obj)
{
  return (long)
    Accessibility_Table_getNSelectedColumns (*obj, &ev);
}


void
AccessibleTable_getSelectedColumns (AccessibleTable *obj,
                                    long **selectedColumns)
{
  Accessibility_Table_getSelectedColumns (*obj,
				      (CORBA_long **) selectedColumns, &ev);
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

