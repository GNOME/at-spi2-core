int
SpiAccessibleHyperlink_ref (SpiAccessibleHyperlink *obj)
{
  Accessibility_SpiHyperlink_ref (*obj, &ev);
  return 0;
}



int
SpiAccessibleHyperlink_unref (SpiAccessibleHyperlink *obj)
{
  Accessibility_SpiHyperlink_unref (*obj, &ev);
  return 0;
}



long
SpiAccessibleHyperlink_getNAnchors (SpiAccessibleHyperlink *obj)
{
  return (long)
    Accessibility_SpiHyperlink__get_nAnchors (*obj, &ev);
}



char *
SpiAccessibleHyperlink_getURI (SpiAccessibleHyperlink *obj,
                            long i)
{
  return (char *)
    Accessibility_SpiHyperlink_getURI (*obj,
				    (CORBA_long) i, &ev);
}



SpiAccessible
SpiAccessibleHyperlink_getObject (SpiAccessibleHyperlink *obj,
                               long i)
{
  return (SpiAccessible)
    Accessibility_SpiHyperlink_getObject (*obj,
				       (CORBA_long) i, &ev);
}



void
SpiAccessibleHyperlink_getIndexRange (SpiAccessibleHyperlink *obj,
                                   long *startIndex,
                                   long *endIndex)
{
  *startIndex = (long)
    Accessibility_SpiHyperlink__get_startIndex (*obj, &ev);
  *endIndex = (long)
    Accessibility_SpiHyperlink__get_endIndex (*obj, &ev);
}



boolean
SpiAccessibleHyperlink_isValid (SpiAccessibleHyperlink *obj)
{
  return (boolean)
    Accessibility_SpiHyperlink_isValid (*obj, &ev);
}


