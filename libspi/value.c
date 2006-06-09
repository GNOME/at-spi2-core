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

/* value.c : implements the Value interface */
#include <config.h>
#include <math.h>
#include <stdio.h>
#include <libspi/value.h>

#define PARENT_TYPE SPI_TYPE_BASE

/* Static function declarations */

static double
get_double_from_gvalue (GValue *gvalue);
static void
gvalue_set_from_double (GValue *gvalue, double value);
static void
spi_value_class_init (SpiValueClass *klass);
static void
spi_value_init (SpiValue *value);
static double
impl__get_minimumValue (PortableServer_Servant _servant,
			CORBA_Environment *    ev);
static double
impl__get_maximumValue (PortableServer_Servant _servant,
			CORBA_Environment *    ev);
static double
impl__get_currentValue (PortableServer_Servant _servant,
			CORBA_Environment *    ev);
static void 
impl__set_currentValue (PortableServer_Servant _servant,
			const CORBA_double     value,
			CORBA_Environment *    ev);
static double
impl__get_minimumIncrement (PortableServer_Servant _servant,
			CORBA_Environment *    ev);


BONOBO_TYPE_FUNC_FULL (SpiValue,
		       Accessibility_Value,
		       PARENT_TYPE,
		       spi_value)


static void
spi_value_class_init (SpiValueClass *klass)
{
  POA_Accessibility_Value__epv *epv = &klass->epv;

  /* Initialize epv table */

  epv->_get_minimumValue = impl__get_minimumValue;
  epv->_get_maximumValue = impl__get_maximumValue;
  epv->_get_currentValue = impl__get_currentValue;
  epv->_set_currentValue = impl__set_currentValue;
  epv->_get_minimumIncrement = impl__get_minimumIncrement;
}


static void
spi_value_init (SpiValue *value)
{
}


SpiValue *
spi_value_interface_new (AtkObject *obj)
{
  SpiValue *new_value = g_object_new (SPI_VALUE_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_value), G_OBJECT(obj));

  return new_value;
}


static AtkValue *
get_value_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT(object->gobj), NULL);
  return ATK_VALUE (object->gobj);
}

static double
get_double_from_gvalue (GValue *gvalue)
{
  double	retval = 0;
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

static void
gvalue_set_from_double (GValue *gvalue, double value)
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

static double
impl__get_minimumValue (PortableServer_Servant servant,
			CORBA_Environment     *ev)
{
  GValue    gvalue = {0, };
  AtkValue *value = get_value_from_servant (servant);

  g_return_val_if_fail (value != NULL, 0.0);

  atk_value_get_minimum_value (value, &gvalue);

  return get_double_from_gvalue (&gvalue);
}


static double
impl__get_maximumValue (PortableServer_Servant servant,
			CORBA_Environment     *ev)
{
  GValue   gvalue = {0, };
  AtkValue *value = get_value_from_servant (servant);

  g_return_val_if_fail (value != NULL, 0.0);

  atk_value_get_maximum_value (value, &gvalue);

  return get_double_from_gvalue (&gvalue);
}


static double
impl__get_currentValue (PortableServer_Servant servant,
			CORBA_Environment     *ev)
{
  GValue   gvalue = {0, };
  AtkValue *value = get_value_from_servant (servant);

  g_return_val_if_fail (value != NULL, 0.0);

  atk_value_get_current_value (value, &gvalue);

  return get_double_from_gvalue (&gvalue);
}


static void 
impl__set_currentValue (PortableServer_Servant servant,
			const CORBA_double     value,
			CORBA_Environment     *ev)
{
  GValue    gvalue = { 0, };
  AtkValue *avalue = get_value_from_servant (servant);

  g_return_if_fail (avalue != NULL);

  atk_value_get_current_value (avalue, &gvalue);
  gvalue_set_from_double (&gvalue, value);

  atk_value_set_current_value (avalue, &gvalue);
}

static double
impl__get_minimumIncrement (PortableServer_Servant servant,
			    CORBA_Environment     *ev)
{
  GValue    gvalue = {0, };
  AtkValue *value = get_value_from_servant (servant);

  g_return_val_if_fail (value != NULL, 0.0);

  atk_value_get_minimum_increment (value, &gvalue);

  return get_double_from_gvalue (&gvalue);
}


