/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

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

/**
 * AccessibleValue_getMinimumIncrement:
 * @obj: a pointer to the #AccessibleValue implementor on which to operate. 
 *
 * Get the minimum increment by which an #AccessibleValue can be adjusted.
 *
 * Returns: the minimum increment by which the value may be changed, or
 * zero if the minimum increment cannot be determined.
 *
 **/
double
AccessibleValue_getMinimumIncrement (AccessibleValue *obj)
{
  double retval;

  cspi_return_val_if_fail (obj != NULL, 0.0);

  retval = 
    Accessibility_Value__get_minimumIncrement (CSPI_OBJREF (obj), cspi_ev ());
  
  cspi_return_val_if_ev ("getMinimumIncrement", 0.0);

  return retval;
}


