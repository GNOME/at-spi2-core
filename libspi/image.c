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

/* image.c : implements the Image interface */

#include <config.h>
#include <stdio.h>
#include <libspi/image.h>


SpiImage *
spi_image_interface_new (AtkObject *obj)
{
  SpiImage *new_image = g_object_new (SPI_IMAGE_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_image), G_OBJECT(obj));

  return new_image;
}

static AtkImage *
get_image_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT(object->gobj), NULL);
  return ATK_IMAGE (object->gobj);
}

static void 
impl_getImagePosition (PortableServer_Servant servant,
		       CORBA_long * x, CORBA_long * y,
		       const CORBA_short coordType,
		       CORBA_Environment *ev)
{
  AtkImage *image = get_image_from_servant (servant);
  gint ix, iy;

  g_return_if_fail (image != NULL);

  atk_image_get_image_position (image,
				&ix, &iy,
				(AtkCoordType) coordType);
  *x = ix;
  *y = iy;
}

static void 
impl_getImageSize (PortableServer_Servant servant,
		   CORBA_long * width, CORBA_long * height,
		   CORBA_Environment *ev)
{
  AtkImage *image = get_image_from_servant (servant);
  gint iw, ih;
  
  g_return_if_fail (image != NULL);

  atk_image_get_image_size (image,
			    &iw, &ih);
  *width = iw;
  *height = ih;
}

static Accessibility_BoundingBox
impl_getImageExtents (PortableServer_Servant servant,
		      const CORBA_short      coordType,
		      CORBA_Environment     *ev)
{
  AtkImage *image;
  gint x, y, width, height;
  Accessibility_BoundingBox bbox;

  bbox.x = bbox.y = bbox.width = bbox.height = -1;

  image = get_image_from_servant (servant);

  if (image)
    {
      atk_image_get_image_size (image, &width, &height);
      atk_image_get_image_position (image, &x, &y, coordType);

      bbox.x = x;
      bbox.y = y;
      bbox.width = width;
      bbox.height = height;
    }

  return bbox;
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

static CORBA_string 
impl__get_imageLocale (PortableServer_Servant servant,
		       CORBA_Environment     *ev)
{
  const char *rv;
  AtkImage   *image = get_image_from_servant (servant);

  g_return_val_if_fail (image != NULL, CORBA_string_dup ("C"));

  rv = atk_image_get_image_locale (image);

  if (rv)
    {
      return CORBA_string_dup (rv);
    }
  else
    {
      return CORBA_string_dup ("C");
    }
}

static void
spi_image_class_init (SpiImageClass *klass)
{
  POA_Accessibility_Image__epv *epv = &klass->epv;

  /* Initialize epv table */
  epv->getImagePosition      = impl_getImagePosition;
  epv->getImageSize          = impl_getImageSize;
  epv->getImageExtents       = impl_getImageExtents;
  epv->_get_imageDescription = impl__get_imageDescription;
  epv->_get_imageLocale      = impl__get_imageLocale;
}

static void
spi_image_init (SpiImage *image)
{
}

BONOBO_TYPE_FUNC_FULL (SpiImage,
		       Accessibility_Image,
		       SPI_TYPE_BASE,
		       spi_image)
