#include <cspi/spi-private.h>

static void
svalue_set_from_float (Accessibility_SValue *sval, float newValue)
{
  switch (sval->_d)
    {
      case Accessibility_SHORTVAL:
        sval->_u.val_short = CLAMP (newValue, G_MINSHORT, G_MAXSHORT);
        break;
      case Accessibility_USHORTVAL:	    
        sval->_u.val_ushort = CLAMP (newValue, 0, G_MAXUSHORT);
        break;
      case Accessibility_LONGVAL:	    
        sval->_u.val_long = CLAMP (newValue, G_MINLONG, G_MAXLONG);
        break;
      case Accessibility_ULONGVAL:	    
        sval->_u.val_ulong = CLAMP (newValue, 0, G_MAXULONG);
        break;
      case Accessibility_FLOATVAL:	    
        sval->_u.val_float = newValue;
        break;
      case Accessibility_DOUBLEVAL:	    
        sval->_u.val_double = newValue;
        break;
    }
}


static float
svalue_to_float (Accessibility_SValue *sval)
{
  float retval = 0.0;
  switch (sval->_d)
    {
      case Accessibility_SHORTVAL:
        retval = sval->_u.val_short;
        break;
      case Accessibility_USHORTVAL:	    
        retval = sval->_u.val_ushort;
        break;
      case Accessibility_LONGVAL:	    
        retval = sval->_u.val_long;
        break;
      case Accessibility_ULONGVAL:	    
        retval = sval->_u.val_ulong;
        break;
      case Accessibility_FLOATVAL:	    
        retval = sval->_u.val_float;
        break;
      case Accessibility_DOUBLEVAL:	    
        retval = sval->_u.val_double;
        break;
    }

  return retval;
}


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
float
AccessibleValue_getMinimumValue (AccessibleValue *obj)
{
  Accessibility_SValue sval;

  cspi_return_val_if_fail (obj != NULL, 0.0);

  sval = 
    Accessibility_Value__get_minimumValue (CSPI_OBJREF (obj), cspi_ev ());
  
  cspi_return_val_if_ev ("getMinimumValue", 0.0);

  return svalue_to_float (&sval);
}

/**
 * AccessibleValue_getCurrentValue:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Get the current value for an #AccessibleValue.
 *
 * Returns: the current value for this object.
 **/
float
AccessibleValue_getCurrentValue (AccessibleValue *obj)
{
  Accessibility_SValue sval;

  cspi_return_val_if_fail (obj != NULL, 0.0);

  sval =
    Accessibility_Value__get_currentValue (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getCurrentValue", 0.0);

  return svalue_to_float (&sval);
}

/**
 * AccessibleValue_getMaximumValue:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Get the maximum allowed value for an #AccessibleValue.
 *
 * Returns: the maximum allowed value for this object.
 **/
float
AccessibleValue_getMaximumValue (AccessibleValue *obj)
{
  Accessibility_SValue sval;

  cspi_return_val_if_fail (obj != NULL, 0.0);

  sval =
    Accessibility_Value__get_maximumValue (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getMaximumValue", 0.0);

  return svalue_to_float (&sval);
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
                                 float newValue)
{
  Accessibility_SValue sval;	
  cspi_return_val_if_fail (obj != NULL, FALSE);

  /* erk, this is ugly */
  sval = Accessibility_Value__get_currentValue (
    CSPI_OBJREF (obj), cspi_ev ());

  svalue_set_from_float (&sval, newValue);
  
  Accessibility_Value__set_currentValue (
    CSPI_OBJREF (obj), &sval, cspi_ev ());

  cspi_return_val_if_ev ("setCurrentValue", FALSE);

  return TRUE;
}


