#include <cspi/spi-private.h>

/**
 * AccessibleValue_ref:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate.
 *
 * Increment the reference count for an #AccessibleValue object.
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
double
AccessibleValue_getMinimumValue (AccessibleValue *obj)
{
  double retval;

  cspi_return_val_if_fail (obj != NULL, 0.0);

  retval = 
    Accessibility_Value__get_minimumValue (CSPI_OBJREF (obj), cspi_ev ());
  
  cspi_return_val_if_ev ("getMinimumValue", 0.0);

  return retval;
}

/**
 * AccessibleValue_getCurrentValue:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Get the current value for an #AccessibleValue.
 *
 * Returns: the current value for this object.
 **/
double
AccessibleValue_getCurrentValue (AccessibleValue *obj)
{
  double retval;

  cspi_return_val_if_fail (obj != NULL, 0.0);

  retval =
    Accessibility_Value__get_currentValue (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getCurrentValue", 0.0);

  return retval;
}

/**
 * AccessibleValue_getMaximumValue:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Get the maximum allowed value for an #AccessibleValue.
 *
 * Returns: the maximum allowed value for this object.
 **/
double
AccessibleValue_getMaximumValue (AccessibleValue *obj)
{
  double retval;	
  cspi_return_val_if_fail (obj != NULL, 0.0);

  retval =
    Accessibility_Value__get_maximumValue (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getMaximumValue", 0.0);

  return retval;
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
 **/
SPIBoolean
AccessibleValue_setCurrentValue (AccessibleValue *obj,
                                 double newValue)
{
  cspi_return_val_if_fail (obj != NULL, FALSE);

  Accessibility_Value__set_currentValue (
    CSPI_OBJREF (obj), newValue, cspi_ev ());

  cspi_return_val_if_ev ("setCurrentValue", FALSE);

  return TRUE;
}


