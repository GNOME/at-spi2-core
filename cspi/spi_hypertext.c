int
SpiAccessibleHypertext_ref (SpiAccessibleHypertext *obj)
{
  Accessibility_Hypertext_ref (*obj, &ev);
  return 0;
}



int
SpiAccessibleHypertext_unref (SpiAccessibleHypertext *obj)
{
  Accessibility_Hypertext_unref (*obj, &ev);
  return 0;
}



long
SpiAccessibleHypertext_getNLinks (SpiAccessibleHypertext *obj)
{
  return (long)
    Accessibility_Hypertext_getNLinks (*obj, &ev);
}


SpiAccessibleHyperlink *
SpiAccessibleHyperText_getLink (SpiAccessibleHypertext *obj,
                             long linkIndex)
{
  return (SpiAccessibleHyperlink *)
    Accessibility_Hypertext_getLink (*obj,
				     (CORBA_long) linkIndex, &ev);
}



long
SpiAccessibleHypertext_getLinkIndex (SpiAccessibleHypertext *obj,
                                  long characterIndex)
{
  return (long)
    Accessibility_Hypertext_getLinkIndex (*obj,
					  (CORBA_long) characterIndex, &ev);
}


