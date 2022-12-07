/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "atspi-private.h"

/**
 * AtspiValue:
 *
 * An interface supporting a one-dimensional scalar
 * to be modified, or which reflects its value.
 *
 * An interface supporting a one-dimensional scalar
 * to be modified, or which reflects its value. If
 * STATE_EDITABLE is not present, the value is
 * treated as "read only".
 */

/**
 * atspi_value_get_minimum_value:
 * @obj: a pointer to the #AtspiValue implementor on which to operate.
 *
 * Gets the minimum allowed value for an #AtspiValue.
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
 * Gets the current value for an #AtspiValue.
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
 * Gets the maximum allowed value for an #AtspiValue.
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
 * @new_value: a #gdouble value which is the desired new value of the object.
 *
 * Sets the current value of an #AtspiValue.
 *
 * Returns: #TRUE if the value could be assigned the specified value,
 *          #FALSE otherwise.
 **/
gboolean
atspi_value_set_current_value (AtspiValue *obj, gdouble new_value, GError **error)
{
  double d_new_value = new_value;
  DBusMessage *message, *reply;
  DBusMessageIter iter, iter_variant;
  static const char *str_curval = "CurrentValue";
  AtspiAccessible *accessible = ATSPI_ACCESSIBLE (obj);

  g_return_val_if_fail (accessible != NULL, FALSE);

  if (!accessible->parent.app || !accessible->parent.app->bus_name)
    {
      g_set_error_literal (error, ATSPI_ERROR, ATSPI_ERROR_APPLICATION_GONE,
                           _ ("The application no longer exists"));
      return FALSE;
    }

  message = dbus_message_new_method_call (accessible->parent.app->bus_name,
                                          accessible->parent.path,
                                          DBUS_INTERFACE_PROPERTIES, "Set");
  if (!message)
    return FALSE;
  dbus_message_append_args (message, DBUS_TYPE_STRING, &atspi_interface_value,
                            DBUS_TYPE_STRING, &str_curval,
                            DBUS_TYPE_INVALID);
  dbus_message_iter_init_append (message, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT, "d", &iter_variant);
  dbus_message_iter_append_basic (&iter_variant, DBUS_TYPE_DOUBLE, &d_new_value);
  dbus_message_iter_close_container (&iter, &iter_variant);
  reply = _atspi_dbus_send_with_reply_and_block (message, error);
  dbus_message_unref (reply);

  return TRUE;
}

/**
 * atspi_value_get_minimum_increment:
 * @obj: a pointer to the #AtspiValue implementor on which to operate.
 *
 * Gets the minimum increment by which an #AtspiValue can be adjusted.
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

/**
 * atspi_value_get_text:
 * @obj: a pointer to the #AtspiValue implementor on which to operate.
 *
 * Gets the human readable text alternative associated with the value.
 * @text is a newly created string, that must be freed by the
 * caller. Can be NULL if no descriptor is available.
 *
 * Since: 2.46
 **/
gchar *
atspi_value_get_text (AtspiValue *obj, GError **error)
{
  gchar *retval = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_get_property (obj, atspi_interface_value, "Text", error, "s", &retval);

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

  if (!type)
    {
      static const GTypeInfo tinfo = {
        sizeof (AtspiValue),
        (GBaseInitFunc) atspi_value_base_init,
        (GBaseFinalizeFunc) NULL,
      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtspiValue", &tinfo, 0);
    }
  return type;
}
