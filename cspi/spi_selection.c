int
SpiAccessibleSelection_ref (SpiAccessibleSelection *obj)
{
  Accessibility_Selection_ref (*obj, &ev);
  return 0;
}



int
SpiAccessibleSelection_unref (SpiAccessibleSelection *obj)
{
  Accessibility_Selection_unref (*obj, &ev);
  return 0;
}



long
SpiAccessibleSelection_getNSelectedChildren (SpiAccessibleSelection *obj)
{
  return (long)
    Accessibility_Selection__get_nSelectedChildren (*obj, &ev);
}



SpiAccessible *
SpiAccessibleSelection_getSelectedChild (SpiAccessibleSelection *obj,
                                      long selectedChildIndex)
{
  Accessibility_Accessible child = 
    Accessibility_Selection_getSelectedChild (*obj,
					      (CORBA_long) selectedChildIndex, &ev);
  spi_warn_ev (&ev, "getSelectedChild");

  return (SpiAccessible *) ((CORBA_Object_is_nil (child, &ev)) ? NULL : Obj_Add (child));
}

boolean
SpiAccessibleSelection_selectChild (SpiAccessibleSelection *obj,
                                 long childIndex)
{
  return (boolean)
    Accessibility_Selection_selectChild (*obj,
					 (CORBA_long) childIndex, &ev);
}



boolean
SpiAccessibleSelection_deselectSelectedChild (SpiAccessibleSelection *obj,
                                           long selectedChildIndex)
{
  Accessibility_Selection_deselectSelectedChild (*obj,
						 (CORBA_long) selectedChildIndex, &ev);
}



boolean
SpiAccessibleSelection_isChildSelected (SpiAccessibleSelection *obj,
                                     long childIndex)
{
  return (boolean)
    Accessibility_Selection_isChildSelected (*obj,
					     (CORBA_long) childIndex, &ev);
}



void
SpiAccessibleSelection_selectAll (SpiAccessibleSelection *obj)
{
  Accessibility_Selection_selectAll (*obj, &ev);
}



void
SpiAccessibleSelection_clearSelection (SpiAccessibleSelection *obj)
{
  Accessibility_Selection_clearSelection (*obj, &ev);
}


