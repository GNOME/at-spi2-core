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

/* Static function declarations */

static void
spi_image_class_init (SpiImageClass *klass);
static void
spi_image_init (SpiImage *image);
static void
spi_image_finalize (GObject *obj);
static void 
impl_getImagePosition (PortableServer_Servant _servant,
		       CORBA_long * x, CORBA_long * y,
		       const CORBA_short coordType,
		       CORBA_Environment * ev);
static void 
impl_getImageSize (PortableServer_Servant _servant,
		   CORBA_long * width, CORBA_long * height,
		   CORBA_Environment * ev);
static CORBA_string 
impl__get_imageDescription (PortableServer_Servant _servant,
			  CORBA_Environment * ev);


static GObjectClass *parent_class;

GType
spi_image_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (SpiImageClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) spi_image_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (SpiImage),
      0, /* n preallocs */
      (GInstanceInitFunc) spi_image_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_TYPE_OBJECT,
			       POA_Accessibility_Image__init,
			       NULL,
			       G_STRUCT_OFFSET (SpiImageClass, epv),
			       &tinfo,
			       "SpiAccessibleImage");
  }

  return type;
}

static void
spi_image_class_init (SpiImageClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Image__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = spi_image_finalize;


  /* Initialize epv table */

  epv->getImagePosition = impl_getImagePosition;
  epv->getImageSize = impl_getImageSize;
  epv->_get_imageDescription = impl__get_imageDescription;
}

static void
spi_image_init (SpiImage *image)
{
}

static void
spi_image_finalize (GObject *obj)
{
  SpiImage *image = SPI_IMAGE (obj);
  g_object_unref (image->atko);
  image->atko = NULL;
  parent_class->finalize (obj);
}

SpiImage *
spi_image_interface_new (AtkObject *obj)
{
  SpiImage *new_image = 
    SPI_IMAGE(g_object_new (SPI_IMAGE_TYPE, NULL));
  new_image->atko = obj;
  g_object_ref (obj);
  return new_image;
}



static void 
impl_getImagePosition (PortableServer_Servant _servant,
		       CORBA_long * x, CORBA_long * y,
		       const CORBA_short coordType,
		       CORBA_Environment * ev)
{
  SpiImage *image = SPI_IMAGE (bonobo_object_from_servant(_servant));
  atk_image_get_image_position (ATK_IMAGE(image->atko),
				(gint *) x, (gint *) y,
				(AtkCoordType) coordType);
}



static void 
impl_getImageSize (PortableServer_Servant _servant,
		   CORBA_long * width, CORBA_long * height,
			    CORBA_Environment * ev)
{
  SpiImage *image = SPI_IMAGE (bonobo_object_from_servant(_servant));
  atk_image_get_image_size (ATK_IMAGE(image->atko),
			    (gint *) width, (gint *) height);
}



static CORBA_string 
impl__get_imageDescription (PortableServer_Servant servant,
			    CORBA_Environment     *ev)
{
  SpiImage *image;
  const char *rv;

  image = SPI_IMAGE (bonobo_object_from_servant (servant));

  rv = atk_image_get_image_description (ATK_IMAGE (image->atko));
  if (rv)
    return CORBA_string_dup (rv);
  else
    return CORBA_string_dup ("");
}



