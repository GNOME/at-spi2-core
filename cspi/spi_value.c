int
SpiAccessibleValue_ref (SpiAccessibleValue *obj)
     {
       Accessibility_SpiValue_ref (*obj, &ev);
       return 0;
     }



int
SpiAccessibleValue_unref (SpiAccessibleValue *obj)
     {
       Accessibility_SpiValue_unref (*obj, &ev);
       return 0;
     }



float
SpiAccessibleValue_getMinimumSpiValue (SpiAccessibleValue *obj)
{
  return (float)
    Accessibility_SpiValue__get_minimumSpiValue (*obj, &ev);
}



float
SpiAccessibleValue_getCurrentValue (SpiAccessibleValue *obj)
{
  return (float)
    Accessibility_SpiValue__get_currentValue (*obj, &ev);
}



float
SpiAccessibleValue_getMaximumSpiValue (SpiAccessibleValue *obj)
{
  return (float)
    Accessibility_SpiValue__get_maximumSpiValue (*obj, &ev);
}



boolean
SpiAccessibleValue_setCurrentValue (SpiAccessibleValue *obj,
                                 float newSpiValue)
{
  Accessibility_SpiValue__set_currentValue (*obj,
					 (CORBA_float) newSpiValue, &ev);
  return TRUE;
}


