int
SpiAccessibleHyperspi_text_ref (SpiAccessibleHypertext *obj)
{
  Accessibility_SpiHyperspi_text_ref (*obj, &ev);
  return 0;
}



int
SpiAccessibleHyperspi_text_unref (SpiAccessibleHypertext *obj)
{
  Accessibility_SpiHyperspi_text_unref (*obj, &ev);
  return 0;
}



long
SpiAccessibleHyperspi_text_getNLinks (SpiAccessibleHypertext *obj)
{
  return (long)
    Accessibility_SpiHyperspi_text_getNLinks (*obj, &ev);
}


SpiAccessibleHyperlink *
SpiAccessibleHyperSpiText_getLink (SpiAccessibleHypertext *obj,
                             long linkIndex)
{
  return (SpiAccessibleHyperlink *)
    Accessibility_SpiHyperspi_text_getLink (*obj,
				     (CORBA_long) linkIndex, &ev);
}



long
SpiAccessibleHyperspi_text_getLinkIndex (SpiAccessibleHypertext *obj,
                                  long characterIndex)
{
  return (long)
    Accessibility_SpiHyperspi_text_getLinkIndex (*obj,
					  (CORBA_long) characterIndex, &ev);
}


