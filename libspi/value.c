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

/*
 * component.c : bonobo wrapper for accessible component implementation
 *
 */
#include <config.h>
#include <bonobo/Bonobo.h>

#include <stdio.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::Accessible" server
 */
#include <libspi/Accessibility.h>

/*
 * This pulls the definition of the SpiValue bonobo object
 */
#include "value.h"

/*
 * Static function declarations
 */

static void
spi_value_class_init (SpiValueClass *klass);
static void
spi_value_init (SpiValue *value);
static void
spi_value_finalize (GObject *obj);
static CORBA_float
impl__get_minimumValue (PortableServer_Servant _servant,
			CORBA_Environment * ev);
static        CORBA_float
impl__get_maximumValue (PortableServer_Servant _servant,
			CORBA_Environment * ev);
static CORBA_float
impl__get_currentValue (PortableServer_Servant _servant,
			CORBA_Environment * ev);
static void 
impl__set_currentValue (PortableServer_Servant _servant,
			const CORBA_float value,
			CORBA_Environment * ev);



static GObjectClass *parent_class;


GType
spi_value_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (SpiValueClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) spi_value_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (SpiValue),
      0, /* n preallocs */
      (GInstanceInitFunc) spi_value_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_Value__init,
			       NULL,
			       G_STRUCT_OFFSET (SpiValueClass, epv),
			       &tinfo,
			       "SpiAccessibleValue");
  }

  return type;
}

static void
spi_value_class_init (SpiValueClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Value__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = spi_value_finalize;


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

static void
spi_value_finalize (GObject *obj)
{
  SpiValue *value = SPI_VALUE (obj);
  g_object_unref (value->atko);
  value->atko = NULL;
  parent_class->finalize (obj);
}

SpiValue *
spi_value_interface_new (AtkObject *obj)
{
  SpiValue *new_value = 
    SPI_VALUE(g_object_new (SPI_VALUE_TYPE, NULL));
  new_value->atko = obj;
  g_object_ref (obj);
  return new_value;
}



static CORBA_float
impl__get_minimumValue (PortableServer_Servant _servant,
		       CORBA_Environment * ev)
{
  SpiValue *value = SPI_VALUE (bonobo_object_from_servant (_servant));
  GValue gvalue = {0, };

  g_value_init (&gvalue, G_TYPE_FLOAT);
  atk_value_get_minimum_value (ATK_VALUE(value->atko), &gvalue);
  return (CORBA_float) g_value_get_float (&gvalue);
}



static        CORBA_float
impl__get_maximumValue (PortableServer_Servant _servant,
			CORBA_Environment * ev)
{
  SpiValue *value = SPI_VALUE (bonobo_object_from_servant (_servant));
  GValue gvalue = {0, };

  g_value_init (&gvalue, G_TYPE_FLOAT);
  atk_value_get_maximum_value (ATK_VALUE(value->atko), &gvalue);
  return (CORBA_float) g_value_get_float (&gvalue);
}



static CORBA_float
impl__get_currentValue (PortableServer_Servant _servant,
			CORBA_Environment * ev)
{
  SpiValue *value = SPI_VALUE (bonobo_object_from_servant (_servant));
  GValue gvalue = {0, };

  g_value_init (&gvalue, G_TYPE_FLOAT);
  atk_value_get_current_value (ATK_VALUE(value->atko), &gvalue);
  return (CORBA_float) g_value_get_float (&gvalue);
}


static void 
impl__set_currentValue (PortableServer_Servant _servant,
			const CORBA_float value,
			CORBA_Environment * ev)
{
  SpiValue *val = SPI_VALUE (bonobo_object_from_servant (_servant));
  GValue gvalue = {0, };

  g_value_init (&gvalue, G_TYPE_FLOAT);
  g_value_set_float (&gvalue, (gfloat) value);
  atk_value_set_current_value (ATK_VALUE(val->atko), &gvalue);
}


