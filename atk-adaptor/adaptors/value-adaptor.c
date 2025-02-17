/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <math.h>

#define ATK_DISABLE_DEPRECATION_WARNINGS
#include "bridge.h"
#include <atk/atk.h>
#include <droute/droute.h>

#include "introspection.h"
#include "spi-dbus.h"

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_VALUE_VERSION);
}

static dbus_bool_t
impl_get_MinimumValue (DBusMessageIter *iter, void *user_data)
{
  AtkValue *value = (AtkValue *) user_data;
  GValue src = { 0 };
  GValue dest = { 0 };
  gdouble dub;

  g_return_val_if_fail (ATK_IS_VALUE (user_data), FALSE);
  AtkValueIface *iface = ATK_VALUE_GET_IFACE (value);
  if (iface->get_range)
    {
      AtkRange *range = atk_value_get_range (value);
      dub = atk_range_get_lower_limit (range);
      atk_range_free (range);
      return droute_return_v_double (iter, dub);
    }

  g_value_init (&src, G_TYPE_DOUBLE);
  atk_value_get_minimum_value (value, &src);
  g_value_init (&dest, G_TYPE_DOUBLE);

  if (g_value_transform (&src, &dest))
    {
      dub = g_value_get_double (&dest);
      return droute_return_v_double (iter, dub);
    }
  else
    {
      return FALSE;
    }
}

static dbus_bool_t
impl_get_MaximumValue (DBusMessageIter *iter, void *user_data)
{
  AtkValue *value = (AtkValue *) user_data;
  GValue src = { 0 };
  GValue dest = { 0 };
  gdouble dub = 0;

  g_return_val_if_fail (ATK_IS_VALUE (user_data), FALSE);

  AtkValueIface *iface = ATK_VALUE_GET_IFACE (value);
  if (iface->get_range)
    {
      AtkRange *range = atk_value_get_range (value);
      dub = atk_range_get_upper_limit (range);
      atk_range_free (range);
      return droute_return_v_double (iter, dub);
    }

  g_value_init (&src, G_TYPE_DOUBLE);
  atk_value_get_maximum_value (value, &src);
  g_value_init (&dest, G_TYPE_DOUBLE);

  if (g_value_transform (&src, &dest))
    {
      dub = g_value_get_double (&dest);
    }
  return droute_return_v_double (iter, dub);
}

static dbus_bool_t
impl_get_MinimumIncrement (DBusMessageIter *iter, void *user_data)
{
  AtkValue *value = (AtkValue *) user_data;
  GValue src = { 0 };
  GValue dest = { 0 };
  gdouble dub = 0;

  g_return_val_if_fail (ATK_IS_VALUE (user_data), FALSE);

  AtkValueIface *iface = ATK_VALUE_GET_IFACE (value);
  if (iface->get_increment)
    {
      dub = atk_value_get_increment (value);
      return droute_return_v_double (iter, dub);
    }

  g_value_init (&src, G_TYPE_DOUBLE);
  atk_value_get_minimum_increment (value, &src);
  g_value_init (&dest, G_TYPE_DOUBLE);

  if (g_value_transform (&src, &dest))
    {
      dub = g_value_get_double (&dest);
    }
  return droute_return_v_double (iter, dub);
}

static dbus_bool_t
impl_get_CurrentValue (DBusMessageIter *iter, void *user_data)
{
  AtkValue *value = (AtkValue *) user_data;
  GValue src = { 0 };
  GValue dest = { 0 };
  gdouble dub = 0;

  g_return_val_if_fail (ATK_IS_VALUE (user_data), FALSE);

  AtkValueIface *iface = ATK_VALUE_GET_IFACE (value);
  if (iface->get_value_and_text)
    {
      gchar *text = NULL;
      atk_value_get_value_and_text (value, &dub, &text);
      return droute_return_v_double (iter, dub);
    }

  g_value_init (&src, G_TYPE_DOUBLE);
  atk_value_get_current_value (value, &src);
  g_value_init (&dest, G_TYPE_DOUBLE);

  if (g_value_transform (&src, &dest))
    {
      dub = g_value_get_double (&dest);
    }
  return droute_return_v_double (iter, dub);
}

static dbus_bool_t
impl_set_CurrentValue (DBusMessageIter *iter, void *user_data)
{
  AtkValue *value = (AtkValue *) user_data;
  GValue src = { 0 };
  GValue dest = { 0 };
  gdouble dub;
  DBusMessageIter iter_variant;

  g_return_val_if_fail (ATK_IS_VALUE (user_data), FALSE);

  dbus_message_iter_recurse (iter, &iter_variant);
  if (dbus_message_iter_get_arg_type (&iter_variant) != DBUS_TYPE_DOUBLE)
    {
      g_warning ("TODO: Support setting value from a non-double");
      return FALSE;
    }
  dbus_message_iter_get_basic (&iter_variant, &dub);

  AtkValueIface *iface = ATK_VALUE_GET_IFACE (value);
  if (iface->set_value)
    {
      atk_value_set_value (value, dub);
      return TRUE;
    }

  g_value_init (&src, G_TYPE_DOUBLE);
  g_value_set_double (&src, dub);

  atk_value_get_current_value (value, &dest);

  if (g_value_transform (&src, &dest))
    {
      atk_value_set_current_value (value, &dest);
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

/* keeping this method around for backwards-compatibility for now; see
 *  * BGO#652596 */
static DBusMessage *
impl_SetCurrentValue (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkValue *value = (AtkValue *) user_data;
  dbus_bool_t rv;
  DBusMessage *reply;
  gdouble dub = 0;
  GValue new_value = { 0 };

  g_return_val_if_fail (ATK_IS_VALUE (value),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_DOUBLE, &dub, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  g_value_init (&new_value, G_TYPE_DOUBLE);
  g_value_set_double (&new_value, dub);
  rv = atk_value_set_current_value (value, &new_value);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static dbus_bool_t
impl_get_Text (DBusMessageIter *iter, void *user_data)
{
  AtkValue *value = (AtkValue *) user_data;
  gdouble dub;
  gchar *text = NULL;
  dbus_bool_t ret;

  g_return_val_if_fail (ATK_IS_VALUE (user_data), FALSE);

  AtkValueIface *iface = ATK_VALUE_GET_IFACE (value);
  if (iface->get_value_and_text)
    {
      atk_value_get_value_and_text (value, &dub, &text);
      ret = droute_return_v_string (iter, text);
      g_free (text);
      return ret;
    }

  return droute_return_v_string (iter, "");
}

static DRouteMethod methods[] = {
  { impl_SetCurrentValue, "SetCurrentValue" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_MinimumValue, NULL, "MinimumValue" },
  { impl_get_MaximumValue, NULL, "MaximumValue" },
  { impl_get_MinimumIncrement, NULL, "MinimumIncrement" },
  { impl_get_CurrentValue, impl_set_CurrentValue, "CurrentValue" },
  { impl_get_Text, NULL, "Text" },
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};

void
spi_initialize_value (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_VALUE,
                         spi_org_a11y_atspi_Value,
                         methods, properties);
};
