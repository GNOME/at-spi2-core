/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#include "accessible.h"
#include <math.h>

static AtkValue *
get_value_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj)
    return NULL;
  return ATK_VALUE (obj);
}

static double
get_double_from_gvalue (GValue * gvalue)
{
  double retval = 0;
  if (G_TYPE_IS_FUNDAMENTAL (G_VALUE_TYPE (gvalue)))
    {
      switch (gvalue->g_type)
	{
	case G_TYPE_DOUBLE:
	  retval = g_value_get_double (gvalue);
	  break;
	case G_TYPE_FLOAT:
	  retval = g_value_get_float (gvalue);
	  break;
	case G_TYPE_ULONG:
	  retval = g_value_get_ulong (gvalue);
	  break;
	case G_TYPE_LONG:
	  retval = g_value_get_long (gvalue);
	  break;
	case G_TYPE_UINT:
	  retval = g_value_get_uint (gvalue);
	  break;
	case G_TYPE_INT:
	  retval = g_value_get_int (gvalue);
	  break;
	case G_TYPE_UCHAR:
	  retval = g_value_get_uchar (gvalue);
	  break;
	case G_TYPE_CHAR:
	  retval = g_value_get_char (gvalue);
	  break;
	case G_TYPE_BOOLEAN:
	  retval = g_value_get_boolean (gvalue);
	  break;
	}
    }
  else
    {
      g_warning ("SpiValue requested from a non-fundamental type\n");
    }
  return retval;
}

static gboolean
get_double_from_variant (DBusMessageIter * iter, double *out)
{
  DBusMessageIter sub;

  dbus_message_iter_recurse (iter, &sub);
  switch (dbus_message_iter_get_arg_type (&sub))
    {
    case DBUS_TYPE_DOUBLE:
      {
	dbus_message_iter_get_basic (&sub, out);
	return TRUE;
      }
    case DBUS_TYPE_UINT32:
      {
	dbus_uint32_t v;
	dbus_message_iter_get_basic (&sub, &v);
	*out = (double) v;
	return TRUE;
      }
    case DBUS_TYPE_INT32:
      {
	dbus_int32_t v;
	dbus_message_iter_get_basic (&sub, &v);
	*out = (double) v;
	return TRUE;
      }
    case DBUS_TYPE_UINT16:
      {
	dbus_uint16_t v;
	dbus_message_iter_get_basic (&sub, &v);
	*out = (double) v;
	return TRUE;
      }
    case DBUS_TYPE_INT16:
      {
	dbus_int16_t v;
	dbus_message_iter_get_basic (&sub, &v);
	*out = (double) v;
	return TRUE;
      }
    case DBUS_TYPE_BYTE:
      {
	char v;
	dbus_message_iter_get_basic (&sub, &v);
	*out = (double) v;
	return TRUE;
      }
    case DBUS_TYPE_BOOLEAN:
      {
	dbus_bool_t v;
	dbus_message_iter_get_basic (&sub, &v);
	*out = (double) v;
	return TRUE;
      }
    default:
      return FALSE;
    }
}

static void
gvalue_set_from_double (GValue * gvalue, double value)
{
  if (G_TYPE_IS_FUNDAMENTAL (G_VALUE_TYPE (gvalue)))
    {
      switch (gvalue->g_type)
	{
	case G_TYPE_DOUBLE:
	  g_value_set_double (gvalue, value);
	  break;
	case G_TYPE_FLOAT:
	  g_value_set_float (gvalue, value);
	  break;
	case G_TYPE_ULONG:
	  g_value_set_ulong (gvalue, value);
	  break;
	case G_TYPE_LONG:
	  g_value_set_long (gvalue, value);
	  break;
	case G_TYPE_UINT:
	  g_value_set_uint (gvalue, value);
	  break;
	case G_TYPE_INT:
	  g_value_set_int (gvalue, value);
	  break;
	case G_TYPE_UCHAR:
	  g_value_set_uchar (gvalue, value);
	  break;
	case G_TYPE_CHAR:
	  g_value_set_char (gvalue, value);
	  break;
	case G_TYPE_BOOLEAN:
	  g_value_set_boolean (gvalue, ((fabs (value) > 0.5) ? 1 : 0));
	  break;
	}
    }
  else
    {
      g_warning ("SpiValue change requested for a non-fundamental type\n");
    }
}

static dbus_bool_t
impl_get_minimumValue (const char *path, DBusMessageIter * iter,
		       void *user_data)
{
  AtkValue *value = get_value_from_path (path, user_data);
  GValue gvalue = { 0, };
  if (!value)
    return FALSE;
  atk_value_get_minimum_value (value, &gvalue);
  return droute_return_v_double (iter, get_double_from_gvalue (&gvalue));
}

static char *
impl_get_minimumValue_str (void *datum)
{
  AtkValue *value = (AtkValue *) datum;
  GValue gvalue = { 0, };
  if (!value)
    return FALSE;
  atk_value_get_minimum_value (value, &gvalue);
  return g_strdup_printf ("%lf", get_double_from_gvalue (&gvalue));
}

static dbus_bool_t
impl_get_maximumValue (const char *path, DBusMessageIter * iter,
		       void *user_data)
{
  AtkValue *value = get_value_from_path (path, user_data);
  GValue gvalue = { 0, };
  if (!value)
    return FALSE;
  atk_value_get_maximum_value (value, &gvalue);
  return droute_return_v_double (iter, get_double_from_gvalue (&gvalue));
}

static char *
impl_get_maximumValue_str (void *datum)
{
  AtkValue *value = (AtkValue *) datum;
  GValue gvalue = { 0, };
  if (!value)
    return FALSE;
  atk_value_get_maximum_value (value, &gvalue);
  return g_strdup_printf ("%lf", get_double_from_gvalue (&gvalue));
}

static dbus_bool_t
impl_get_minimumIncrement (const char *path, DBusMessageIter * iter,
			   void *user_data)
{
  AtkValue *value = get_value_from_path (path, user_data);
  GValue gvalue = { 0, };
  if (!value)
    return FALSE;
  atk_value_get_minimum_value (value, &gvalue);
  return droute_return_v_double (iter, get_double_from_gvalue (&gvalue));
}

static char *
impl_get_minimumIncrement_str (void *datum)
{
  AtkValue *value = (AtkValue *) datum;
  GValue gvalue = { 0, };
  if (!value)
    return FALSE;
  atk_value_get_minimum_value (value, &gvalue);
  return g_strdup_printf ("%lf", get_double_from_gvalue (&gvalue));
}

static dbus_bool_t
impl_get_currentValue (const char *path, DBusMessageIter * iter,
		       void *user_data)
{
  AtkValue *value = get_value_from_path (path, user_data);
  GValue gvalue = { 0, };
  if (!value)
    return FALSE;
  atk_value_get_current_value (value, &gvalue);
  return droute_return_v_double (iter, get_double_from_gvalue (&gvalue));
}

static char *
impl_get_currentValue_str (void *datum)
{
  AtkValue *value = (AtkValue *) datum;
  GValue gvalue = { 0, };
  if (!value)
    return FALSE;
  atk_value_get_current_value (value, &gvalue);
  return g_strdup_printf ("%lf", get_double_from_gvalue (&gvalue));
}

static dbus_bool_t
impl_set_currentValue (const char *path, DBusMessageIter * iter,
		       void *user_data)
{
  AtkValue *value = get_value_from_path (path, user_data);
  GValue gvalue = { 0, };
  double dbl;

  if (!value)
    return FALSE;
  if (!get_double_from_variant (iter, &dbl))
    return FALSE;
  atk_value_get_current_value (value, &gvalue);
  gvalue_set_from_double (&gvalue, dbl);
  return TRUE;
}

static DRouteProperty properties[] = {
  {impl_get_minimumValue, impl_get_minimumValue_str, NULL, NULL,
   "minimumValue", "d"},
  {impl_get_maximumValue, impl_get_maximumValue_str, NULL, NULL,
   "maximumValue", "d"},
  {impl_get_minimumIncrement, impl_get_minimumIncrement_str, NULL, NULL,
   "minimumIncrement", "d"},
  {impl_get_currentValue, impl_get_currentValue_str, impl_set_currentValue,
   NULL, "currentValue", "d"},
  {NULL, NULL, NULL, NULL, NULL, NULL}
};

void
spi_initialize_value (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Value", NULL,
			properties,
			(DRouteGetDatumFunction) get_value_from_path, NULL);
};
