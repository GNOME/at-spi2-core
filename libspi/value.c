/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc.
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

/* value.c : implements the Value interface */
#include <config.h>
#include <stdio.h>
#include <libspi/value.h>

#define PARENT_TYPE SPI_TYPE_BASE

/* Static function declarations */

static Accessibility_SValue
gvalue_to_svalue (GValue *gvalue);
static void
spi_value_class_init (SpiValueClass *klass);
static void
spi_value_init (SpiValue *value);
static Accessibility_SValue
impl__get_minimumValue (PortableServer_Servant _servant,
			CORBA_Environment *    ev);
static Accessibility_SValue
impl__get_maximumValue (PortableServer_Servant _servant,
			CORBA_Environment *    ev);
static Accessibility_SValue
impl__get_currentValue (PortableServer_Servant _servant,
			CORBA_Environment *    ev);
static void 
impl__set_currentValue (PortableServer_Servant       _servant,
			const Accessibility_SValue * value,
			CORBA_Environment *          ev);


BONOBO_TYPE_FUNC_FULL (SpiValue,
		       Accessibility_Value,
		       PARENT_TYPE,
		       spi_value);


static void
spi_value_class_init (SpiValueClass *klass)
{
  POA_Accessibility_Value__epv *epv = &klass->epv;

  /* Initialize epv table */

  epv->_get_minimumValue = impl__get_minimumValue;
  epv->_get_maximumValue = impl__get_maximumValue;
  epv->_get_currentValue = impl__get_currentValue;
  epv->_set_currentValue = impl__set_currentValue;
}


static void
spi_value_init (SpiValue *value)
{
}


SpiValue *
spi_value_interface_new (AtkObject *obj)
{
  SpiValue *new_value = g_object_new (SPI_VALUE_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_value), obj);

  return new_value;
}


static AtkValue *
get_value_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  if (!object)
    {
      return NULL;
    }

  return ATK_VALUE (object->atko);
}

static Accessibility_SValue
gvalue_to_svalue (GValue *gvalue)
{
  Accessibility_SValue sval;
  if (G_VALUE_HOLDS_DOUBLE (gvalue))
  {
      sval._d = Accessibility_DOUBLEVAL;
      sval._u.val_double = g_value_get_double (gvalue);
  }
  else if (G_VALUE_HOLDS_FLOAT (gvalue))
  {
      sval._d = Accessibility_FLOATVAL;
      sval._u.val_float = g_value_get_float (gvalue);
  }
  else if (G_VALUE_HOLDS_ULONG (gvalue))
  {
      sval._d = Accessibility_ULONGVAL;
      sval._u.val_ulong = g_value_get_ulong (gvalue); 
  }
  else if (G_VALUE_HOLDS_LONG (gvalue))
  {
      sval._d = Accessibility_LONGVAL;
      sval._u.val_long = g_value_get_long (gvalue);
  }
  else if (G_VALUE_HOLDS_UINT (gvalue))
  {
      sval._d = Accessibility_USHORTVAL;
      sval._u.val_ushort = CLAMP (g_value_get_uint (gvalue), 0, G_MAXUSHORT);
  }
  else if (G_VALUE_HOLDS_INT (gvalue))
  {
      sval._d = Accessibility_SHORTVAL;
      sval._u.val_short = CLAMP (g_value_get_int (gvalue), G_MINSHORT, G_MAXSHORT);
  }
  else
  {
      sval._d = Accessibility_SHORTVAL;
      sval._u.val_short = 0;
  }
  return sval;
}

static void
gvalue_set_from_svalue (GValue *gvalue, Accessibility_SValue *sval)
{
  switch (sval->_d)
    {
      case Accessibility_DOUBLEVAL:
        g_value_init (gvalue, G_TYPE_DOUBLE);
        g_value_set_double (gvalue, sval->_u.val_double);
        break;
      case Accessibility_FLOATVAL:
        g_value_init (gvalue, G_TYPE_FLOAT);
        g_value_set_float (gvalue, sval->_u.val_float);
        break;
      case Accessibility_ULONGVAL:
        g_value_init (gvalue, G_TYPE_ULONG);
        g_value_set_ulong (gvalue, sval->_u.val_ulong);
        break;
      case Accessibility_LONGVAL:
        g_value_init (gvalue, G_TYPE_LONG);
        g_value_set_long (gvalue, sval->_u.val_long);
        break;
      case Accessibility_USHORTVAL:
        g_value_init (gvalue, G_TYPE_UINT);
        g_value_set_uint (gvalue, sval->_u.val_ushort);
        break;
      case Accessibility_SHORTVAL:
        g_value_init (gvalue, G_TYPE_INT);
        g_value_set_int (gvalue, sval->_u.val_short);
        break;
      default:
        g_value_init (gvalue, G_TYPE_INT);
	g_value_set_int (gvalue, 0);
    }
}

static Accessibility_SValue
impl__get_minimumValue (PortableServer_Servant servant,
			CORBA_Environment     *ev)
{
  GValue    gvalue = {0, };
  AtkValue *value = get_value_from_servant (servant);

  g_return_val_if_fail (value != NULL, gvalue_to_svalue (&gvalue));

  g_value_init (&gvalue, G_TYPE_DOUBLE);
  atk_value_get_minimum_value (value, &gvalue);

  return gvalue_to_svalue (&gvalue);
}


static Accessibility_SValue
impl__get_maximumValue (PortableServer_Servant servant,
			CORBA_Environment     *ev)
{
  GValue   gvalue = {0, };
  AtkValue *value = get_value_from_servant (servant);

  g_return_val_if_fail (value != NULL, gvalue_to_svalue (&gvalue));

  g_value_init (&gvalue, G_TYPE_DOUBLE);
  atk_value_get_maximum_value (value, &gvalue);

  return gvalue_to_svalue (&gvalue);
}


static Accessibility_SValue
impl__get_currentValue (PortableServer_Servant servant,
			CORBA_Environment     *ev)
{
  GValue   gvalue = {0, };
  AtkValue *value = get_value_from_servant (servant);

  g_return_val_if_fail (value != NULL, gvalue_to_svalue (&gvalue));

  g_value_init (&gvalue, G_TYPE_DOUBLE);
  atk_value_get_current_value (value, &gvalue);

  return gvalue_to_svalue (&gvalue);
}


static void 
impl__set_currentValue (PortableServer_Servant servant,
			const Accessibility_SValue *value,
			CORBA_Environment     *ev)
{
  GValue    gvalue = {0, };
  AtkValue *avalue = get_value_from_servant (servant);

  g_return_if_fail (avalue != NULL);

  gvalue_set_from_svalue (&gvalue, value);

  atk_value_set_current_value (avalue, &gvalue);
}


