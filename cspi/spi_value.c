int
SpiAccessibleValue_ref (SpiAccessibleValue *obj)
     {
       Accessibility_Value_ref (*obj, &ev);
       return 0;
     }



int
SpiAccessibleValue_unref (SpiAccessibleValue *obj)
     {
       Accessibility_Value_unref (*obj, &ev);
       return 0;
     }



float
SpiAccessibleValue_getMinimumValue (SpiAccessibleValue *obj)
{
  return (float)
    Accessibility_Value__get_minimumValue (*obj, &ev);
}



float
SpiAccessibleValue_getCurrentValue (SpiAccessibleValue *obj)
{
  return (float)
    Accessibility_Value__get_currentValue (*obj, &ev);
}



float
SpiAccessibleValue_getMaximumValue (SpiAccessibleValue *obj)
{
  return (float)
    Accessibility_Value__get_maximumValue (*obj, &ev);
}



boolean
SpiAccessibleValue_setCurrentValue (SpiAccessibleValue *obj,
                                 float newValue)
{
  Accessibility_Value__set_currentValue (*obj,
					 (CORBA_float) newValue, &ev);
  return TRUE;
}


