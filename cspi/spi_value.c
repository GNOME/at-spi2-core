/**
 * AccessibleValue_ref:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate.
 *
 * Increment the reference count for an #AccessibleValue object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleValue_ref (AccessibleValue *obj)
{
  Accessibility_Value_ref (*obj, &ev);
  return 0;
}



/**
 * AccessibleValue_unref:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Decrement the reference count for an #AccessibleValue object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleValue_unref (AccessibleValue *obj)
{
  Accessibility_Value_unref (*obj, &ev);
  return 0;
}



/**
 * AccessibleValue_getMinimumValue:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Get the minimum allowed value for an #AccessibleValue.
 *
 * Returns: the minimum allowed value for this object.
 *
 **/
float
AccessibleValue_getMinimumValue (AccessibleValue *obj)
{
  return (float)
    Accessibility_Value__get_minimumValue (*obj, &ev);
}



/**
 * AccessibleValue_getCurrentValue:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Get the current value for an #AccessibleValue.
 *
 * Returns: the current value for this object.
 *
 **/
float
AccessibleValue_getCurrentValue (AccessibleValue *obj)
{
  return (float)
    Accessibility_Value__get_currentValue (*obj, &ev);
}



/**
 * AccessibleValue_getMaximumValue:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Get the maximum allowed value for an #AccessibleValue.
 *
 * Returns: the maximum allowed value for this object.
 *
 **/
float
AccessibleValue_getMaximumValue (AccessibleValue *obj)
{
  return (float)
    Accessibility_Value__get_maximumValue (*obj, &ev);
}



/**
 * AccessibleValue_setCurrentValue:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate.
 * @newValue: a #float value which is the desired new value of the object.
 *
 * Set the current value of an #AccessibleValue.
 *
 * Returns: #TRUE if the value could be assigned the specified value,
 *          #FALSE otherwise.
 *
 **/
boolean
AccessibleValue_setCurrentValue (AccessibleValue *obj,
                                 float newValue)
{
  Accessibility_Value__set_currentValue (*obj,
					 (CORBA_float) newValue, &ev);
  return TRUE;
}


