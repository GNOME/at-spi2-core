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
AccessibleValue_getMinimumValue (AccessibleValue *value)
{
  return (float)
    Accessibility_Value__get_minimumValue (*obj, &ev);
}



float
AccessibleValue_getCurrentValue (AccessibleValue *value)
{
  return (float)
    Accessibility_Value__get_currentValue (*obj, &ev);
}



float
AccessibleValue_getMaximumValue (AccessibleValue *value)
{
  return (float)
    Accessibility_Value__get_maximumValue (*obj, &ev);
}



boolean
AccessibleValue_setCurrentValue (AccessibleValue *value,
                                 float newValue)
{
  Accessibility_Value__set_currentValue (*obj,
					 (CORBA_float) newValue, &ev);
  return TRUE;
}


