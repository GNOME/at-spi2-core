/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
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

#include "atspi-private.h"

/**
 * atspi_value_get_minimum_value:
 * @obj: a pointer to the #AtspiValue implementor on which to operate. 
 *
 * Get the minimum allowed value for an #AtspiValue.
 *
 * Returns: the minimum allowed value for this object.
 *
 **/
gdouble
atspi_value_get_minimum_value (AtspiValue *obj, GError **error)
{
  double retval;

  g_return_val_if_fail (obj != NULL, 0.0);
  _atspi_dbus_get_property (obj, atspi_interface_value, "MinimumValue", error, "d", &retval);
  
  return retval;
}

/**
 * atspi_value_get_current_value:
 * @obj: a pointer to the #AtspiValue implementor on which to operate. 
 *
 * Get the current value for an #AtspiValue.
 *
 * Returns: the current value for this object.
 **/
gdouble
atspi_value_get_current_value (AtspiValue *obj, GError **error)
{
  double retval;

  g_return_val_if_fail (obj != NULL, 0.0);

  _atspi_dbus_get_property (obj, atspi_interface_value, "CurrentValue", error, "d", &retval);

  return retval;
}

/**
 * atspi_value_get_maximum_value:
 * @obj: a pointer to the #AtspiValue implementor on which to operate. 
 *
 * Get the maximum allowed value for an #AtspiValue.
 *
 * Returns: the maximum allowed value for this object.
 **/
gdouble
atspi_value_get_maximum_value (AtspiValue *obj, GError **error)
{
  double retval;	

  g_return_val_if_fail (obj != NULL, 0.0);

  _atspi_dbus_get_property (obj, atspi_interface_value, "MaximumValue", error, "d", &retval);

  return retval;
}

/**
 * atspi_value_set_current_value:
 * @obj: a pointer to the #AtspiValue implementor on which to operate.
 * @new_value: a #float value which is the desired new value of the object.
 *
 * Set the current value of an #AtspiValue.
 *
 * Returns: #TRUE if the value could be assigned the specified value,
 *          #FALSE otherwise.
 **/
gboolean
atspi_value_set_current_value (AtspiValue *obj, gdouble new_value, GError **error)
{
  double d_new_value = new_value;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_value, "SetCurrentValue", error, "d", &d_new_value);

  return TRUE;
}

/**
 * atspi_value_get_minimum_increment:
 * @obj: a pointer to the #AtspiValue implementor on which to operate. 
 *
 * Get the minimum increment by which an #AtspiValue can be adjusted.
 *
 * Returns: the minimum increment by which the value may be changed, or
 * zero if the minimum increment cannot be determined.
 *
 **/
gdouble
atspi_value_get_minimum_increment (AtspiValue *obj, GError **error)
{
  double retval;

  g_return_val_if_fail (obj != NULL, 0.0);

  _atspi_dbus_get_property (obj, atspi_interface_value, "MinimumIncrement", error, "d", &retval);
  
  return retval;
}

static void
atspi_value_base_init (AtspiValue *klass)
{
}

GType
atspi_value_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtspiValue),
      (GBaseInitFunc) atspi_value_base_init,
      (GBaseFinalizeFunc) NULL,
    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtspiValue", &tinfo, 0);

  }
  return type;
}
