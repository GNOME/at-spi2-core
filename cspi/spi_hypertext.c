int
AccessibleHypertext_ref (AccessibleHypertext *obj)
{
  Accessibility_Hypertext_ref (*obj, &ev);
  return 0;
}



int
AccessibleHypertext_unref (AccessibleHypertext *obj)
{
  Accessibility_Hypertext_unref (*obj, &ev);
  return 0;
}



long
AccessibleHypertext_getNLinks (AccessibleHypertext *obj)
{
  return (long)
    Accessibility_Hypertext_getNLinks (*obj, &ev);
}


AccessibleHyperlink *
AccessibleHyperText_getLink (AccessibleHypertext *obj,
                             long linkIndex)
{
  return (AccessibleHyperlink *)
    Accessibility_Hypertext_getLink (*obj,
				     (CORBA_long) linkIndex, &ev);
}



long
AccessibleHypertext_getLinkIndex (AccessibleHypertext *obj,
                                  long characterIndex)
{
  return (long)
    Accessibility_Hypertext_getLinkIndex (*obj,
					  (CORBA_long) characterIndex, &ev);
}


