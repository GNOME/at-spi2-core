int
SpiAccessibleHyperlink_ref (SpiAccessibleHyperlink *obj)
{
  Accessibility_Hyperlink_ref (*obj, &ev);
  return 0;
}



int
SpiAccessibleHyperlink_unref (SpiAccessibleHyperlink *obj)
{
  Accessibility_Hyperlink_unref (*obj, &ev);
  return 0;
}



long
SpiAccessibleHyperlink_getNAnchors (SpiAccessibleHyperlink *obj)
{
  return (long)
    Accessibility_Hyperlink__get_nAnchors (*obj, &ev);
}



char *
SpiAccessibleHyperlink_getURI (SpiAccessibleHyperlink *obj,
                            long i)
{
  return (char *)
    Accessibility_Hyperlink_getURI (*obj,
				    (CORBA_long) i, &ev);
}



SpiAccessible
SpiAccessibleHyperlink_getObject (SpiAccessibleHyperlink *obj,
                               long i)
{
  return (SpiAccessible)
    Accessibility_Hyperlink_getObject (*obj,
				       (CORBA_long) i, &ev);
}



void
SpiAccessibleHyperlink_getIndexRange (SpiAccessibleHyperlink *obj,
                                   long *startIndex,
                                   long *endIndex)
{
  *startIndex = (long)
    Accessibility_Hyperlink__get_startIndex (*obj, &ev);
  *endIndex = (long)
    Accessibility_Hyperlink__get_endIndex (*obj, &ev);
}



boolean
SpiAccessibleHyperlink_isValid (SpiAccessibleHyperlink *obj)
{
  return (boolean)
    Accessibility_Hyperlink_isValid (*obj, &ev);
}


