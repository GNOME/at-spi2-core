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
 * This pulls the definition of the image bonobo object
 */
#include "image.h"

/*
 * Static function declarations
 */

static void
image_class_init (ImageClass *klass);
static void
image_init (Image *image);
static void
image_finalize (GObject *obj);
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
image_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (ImageClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) image_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (Image),
      0, /* n preallocs */
      (GInstanceInitFunc) image_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_Image__init,
			       NULL,
			       G_STRUCT_OFFSET (ImageClass, epv),
			       &tinfo,
			       "AccessibleImage");
  }

  return type;
}

static void
image_class_init (ImageClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Image__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = image_finalize;


  /* Initialize epv table */

  epv->getImagePosition = impl_getImagePosition;
  epv->getImageSize = impl_getImageSize;
  epv->_get_imageDescription = impl__get_imageDescription;
}

static void
image_init (Image *image)
{
}

static void
image_finalize (GObject *obj)
{
  Image *image = IMAGE (obj);
  g_object_unref (image->atko);
  image->atko = NULL;
  parent_class->finalize (obj);
}

Image *
image_interface_new (AtkObject *obj)
{
  Image *new_image = 
    IMAGE(g_object_new (IMAGE_TYPE, NULL));
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
  Image *image = IMAGE (bonobo_object_from_servant(_servant));
  atk_image_get_image_position (ATK_IMAGE(image->atko),
				(gint *) x, (gint *) y,
				(AtkCoordType) coordType);
}



static void 
impl_getImageSize (PortableServer_Servant _servant,
		   CORBA_long * width, CORBA_long * height,
			    CORBA_Environment * ev)
{
  Image *image = IMAGE (bonobo_object_from_servant(_servant));
  atk_image_get_image_size (ATK_IMAGE(image->atko),
			    (gint *) width, (gint *) height);
}



static CORBA_string 
impl__get_imageDescription (PortableServer_Servant _servant,
			  CORBA_Environment * ev)
{
  Image *image = IMAGE (bonobo_object_from_servant(_servant));
  return CORBA_string_dup (
			   atk_image_get_image_description (ATK_IMAGE(image->atko)));
}



