#include <cspi/spi-private.h>

/**
 * AccessibleValue_ref:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate.
 *
 * Increment the reference count for an #AccessibleValue object.
 *
 **/
void
AccessibleValue_ref (AccessibleValue *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleValue_unref:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Decrement the reference count for an #AccessibleValue object.
 *
 **/
void
AccessibleValue_unref (AccessibleValue *obj)
{
  cspi_object_unref (obj);
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
    Accessibility_Value__get_minimumValue (CSPI_OBJREF (obj), cspi_ev ());
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
    Accessibility_Value__get_currentValue (CSPI_OBJREF (obj), cspi_ev ());
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
    Accessibility_Value__get_maximumValue (CSPI_OBJREF (obj), cspi_ev ());
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
SPIBoolean
AccessibleValue_setCurrentValue (AccessibleValue *obj,
                                 float newValue)
{
  Accessibility_Value__set_currentValue (CSPI_OBJREF (obj),
					 (CORBA_float) newValue, cspi_ev ());
  return TRUE;
}


