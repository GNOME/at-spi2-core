int
AccessibleHyperlink_ref (AccessibleHyperlink *obj)
{
  Accessibility_Hyperlink_ref (*obj, &ev);
  return 0;
}



int
AccessibleHyperlink_unref (AccessibleHyperlink *obj)
{
  Accessibility_Hyperlink_unref (*obj, &ev);
  return 0;
}



long
AccessibleHyperlink_getNAnchors (AccessibleHyperlink *obj)
{
  return (long)
    Accessibility_Hyperlink__get_nAnchors (*obj, &ev);
}



char *
AccessibleHyperlink_getURI (AccessibleHyperlink *obj,
                            long i)
{
  return (char *)
    Accessibility_Hyperlink_getURI (*obj,
				    (CORBA_long) i, &ev);
}



Accessible*
AccessibleHyperlink_getObject (AccessibleHyperlink *obj,
                               long i)
{
  return Obj_Add (
    Accessibility_Hyperlink_getObject (*obj,
				       (CORBA_long) i, &ev));
}



void
AccessibleHyperlink_getIndexRange (AccessibleHyperlink *obj,
                                   long *startIndex,
                                   long *endIndex)
{
  *startIndex = (long)
    Accessibility_Hyperlink__get_startIndex (*obj, &ev);
  *endIndex = (long)
    Accessibility_Hyperlink__get_endIndex (*obj, &ev);
}



boolean
AccessibleHyperlink_isValid (AccessibleHyperlink *obj)
{
  return (boolean)
    Accessibility_Hyperlink_isValid (*obj, &ev);
}


