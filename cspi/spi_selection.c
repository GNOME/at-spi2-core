int
AccessibleSelection_ref (AccessibleSelection *obj)
{
  Accessibility_Selection_ref (*obj, &ev);
  return 0;
}



int
AccessibleSelection_unref (AccessibleSelection *obj)
{
  Accessibility_Selection_unref (*obj, &ev);
  return 0;
}



long
AccessibleSelection_getNSelectedChildren (AccessibleSelection *obj)
{
  return (long)
    Accessibility_Selection__get_nSelectedChildren (*obj, &ev);
}



Accessible *
AccessibleSelection_getSelectedChild (AccessibleSelection *obj,
                                      long selectedChildIndex)
{
  Accessibility_Accessible child = 
    Accessibility_Selection_getSelectedChild (*obj,
					      (CORBA_long) selectedChildIndex, &ev);
  spi_warn_ev (&ev, "getSelectedChild");

  return (Accessible *) ((CORBA_Object_is_nil (child, &ev)) ? NULL : Obj_Add (child));
}

boolean
AccessibleSelection_selectChild (AccessibleSelection *obj,
                                 long childIndex)
{
  return (boolean)
    Accessibility_Selection_selectChild (*obj,
					 (CORBA_long) childIndex, &ev);
}



boolean
AccessibleSelection_deselectSelectedChild (AccessibleSelection *obj,
                                           long selectedChildIndex)
{
  Accessibility_Selection_deselectSelectedChild (*obj,
						 (CORBA_long) selectedChildIndex, &ev);
}



boolean
AccessibleSelection_isChildSelected (AccessibleSelection *obj,
                                     long childIndex)
{
  return (boolean)
    Accessibility_Selection_isChildSelected (*obj,
					     (CORBA_long) childIndex, &ev);
}



void
AccessibleSelection_selectAll (AccessibleSelection *obj)
{
  Accessibility_Selection_selectAll (*obj, &ev);
}



void
AccessibleSelection_clearSelection (AccessibleSelection *obj)
{
  Accessibility_Selection_clearSelection (*obj, &ev);
}


