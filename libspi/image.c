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

/* image.c : implements the Image interface */

#include <config.h>
#include <stdio.h>
#include <libspi/image.h>


SpiImage *
spi_image_interface_new (AtkObject *obj)
{
  SpiImage *new_image = g_object_new (SPI_IMAGE_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_image), obj);

  return new_image;
}


static AtkImage *
get_image_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  if (!object)
    {
      return NULL;
    }

  return ATK_IMAGE (object->atko);
}


static void 
impl_getImagePosition (PortableServer_Servant servant,
		       CORBA_long * x, CORBA_long * y,
		       const CORBA_short coordType,
		       CORBA_Environment *ev)
{
  AtkImage *image = get_image_from_servant (servant);

  g_return_if_fail (image != NULL);

  atk_image_get_image_position (image,
				(gint *) x, (gint *) y,
				(AtkCoordType) coordType);
}


static void 
impl_getImageSize (PortableServer_Servant servant,
		   CORBA_long * width, CORBA_long * height,
		   CORBA_Environment *ev)
{
  AtkImage *image = get_image_from_servant (servant);

  g_return_if_fail (image != NULL);

  atk_image_get_image_size (image,
			    (gint *) width, (gint *) height);
}


static CORBA_string 
impl__get_imageDescription (PortableServer_Servant servant,
			    CORBA_Environment     *ev)
{
  const char *rv;
  AtkImage   *image = get_image_from_servant (servant);

  g_return_val_if_fail (image != NULL, CORBA_string_dup (""));

  rv = atk_image_get_image_description (image);

  if (rv)
    {
      return CORBA_string_dup (rv);
    }
  else
    {
      return CORBA_string_dup ("");
    }
}


static void
spi_image_class_init (SpiImageClass *klass)
{
  POA_Accessibility_Image__epv *epv = &klass->epv;

  /* Initialize epv table */

  epv->getImagePosition = impl_getImagePosition;
  epv->getImageSize = impl_getImageSize;
  epv->_get_imageDescription = impl__get_imageDescription;
}

static void
spi_image_init (SpiImage *image)
{
}

BONOBO_TYPE_FUNC_FULL (SpiImage,
		       Accessibility_Image,
		       SPI_TYPE_BASE,
		       spi_image);
