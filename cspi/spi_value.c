int
AccessibleValue_ref (AccessibleValue *obj)
     {
       Accessibility_Value_ref (*obj, &ev);
       return 0;
     }



int
AccessibleValue_unref (AccessibleValue *obj)
     {
       Accessibility_Value_unref (*obj, &ev);
       return 0;
     }



float
AccessibleValue_getMinimumValue (AccessibleValue *obj)
{
  return (float)
    Accessibility_Value__get_minimumValue (*obj, &ev);
}



float
AccessibleValue_getCurrentValue (AccessibleValue *obj)
{
  return (float)
    Accessibility_Value__get_currentValue (*obj, &ev);
}



float
AccessibleValue_getMaximumValue (AccessibleValue *obj)
{
  return (float)
    Accessibility_Value__get_maximumValue (*obj, &ev);
}



boolean
AccessibleValue_setCurrentValue (AccessibleValue *obj,
                                 float newValue)
{
  Accessibility_Value__set_currentValue (*obj,
					 (CORBA_float) newValue, &ev);
  return TRUE;
}


