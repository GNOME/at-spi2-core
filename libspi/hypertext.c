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
 * This pulls the CORBA definitions for the "Accessibility::SpiAccessible" server
 */
#include <libspi/Accessibility.h>

/*
 * This pulls the definition of the hypertext bonobo object
 */
#include "hypertext.h"

/*
 * Static function declarations
 */

static void
spi_hypertext_class_init (SpiHypertextClass *klass);
static void
spi_hypertext_init (SpiHypertext *hypertext);
static void
spi_hypertext_finalize (GObject *obj);
static CORBA_short
impl__get_n_anchors (PortableServer_Servant _servant,
		     CORBA_Environment * ev);
static CORBA_string
impl__get_uri (PortableServer_Servant _servant,
	       CORBA_Environment * ev);
static CORBA_long
impl__get_startIndex (PortableServer_Servant _servant,
		      CORBA_Environment * ev);
static CORBA_long
impl__get_endIndex (PortableServer_Servant _servant,
		    CORBA_Environment * ev);
static Accessibility_SpiAccessible
impl_getAnchor (PortableServer_Servant _servant,
		const CORBA_long i,
		CORBA_Environment * ev);
static Accessibility_SpiAccessible
impl_getObject (PortableServer_Servant _servant,
		const CORBA_long i,
		CORBA_Environment * ev);
static CORBA_long
impl_getNLinks (PortableServer_Servant _servant,
		CORBA_Environment * ev);
static Accessibility_SpiHyperlink
impl_getLink (PortableServer_Servant _servant,
	      const CORBA_long linkIndex,
	      CORBA_Environment * ev);
static CORBA_long
impl_getLinkIndex (PortableServer_Servant _servant,
		   const CORBA_long characterIndex,
		   CORBA_Environment * ev);

static GObjectClass *parent_class;

GType
spi_hypertext_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (SpiHypertextClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) spi_hypertext_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (SpiHypertext),
      0, /* n preallocs */
      (GInstanceInitFunc) spi_hypertext_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_SpiHyperspi_text__init,
			       NULL,
			       G_STRUCT_OFFSET (SpiHypertextClass, epv),
			       &tinfo,
			       "SpiAccessibleHypertext");
  }

  return type;
}

static void
spi_hypertext_class_init (SpiHypertextClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_SpiHyperspi_text__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = spi_hypertext_finalize;

  /* Initialize epv table */

  epv->getNLinks = impl_getNLinks;
  epv->getLink = impl_getLink;
  epv->getLinkIndex = impl_getLinkIndex;
}

static void
spi_hypertext_init (SpiHypertext *hypertext)
{
}

static void
spi_hypertext_finalize (GObject *obj)
{
  SpiHypertext *hypertext = SPI_HYPERTEXT(obj);
  g_object_unref (hypertext->atko);
  hypertext->atko = NULL;
  parent_class->finalize (obj);
}

SpiHypertext *
spi_hypertext_interface_new (AtkObject *obj)
{
  SpiHypertext *new_hypertext = 
    SPI_HYPERTEXT(g_object_new (SPI_HYPERTEXT_TYPE, NULL));
  new_hypertext->atko = obj;
  g_object_ref (obj);
  return new_hypertext;
}



static CORBA_long
impl_getNLinks (PortableServer_Servant _servant,
		CORBA_Environment * ev)
{
  SpiHypertext *hypertext = SPI_HYPERTEXT(bonobo_object_from_servant(_servant));
  return (CORBA_long)
    atk_hypertext_get_n_links (ATK_HYPERTEXT(hypertext->atko));
}



static Accessibility_SpiHyperlink
impl_getLink (PortableServer_Servant _servant,
	      const CORBA_long linkIndex,
					  CORBA_Environment * ev)
{
  AtkHyperlink *link;
  SpiHypertext *hypertext = SPI_HYPERTEXT(bonobo_object_from_servant(_servant));
  Accessibility_SpiHyperlink rv;
  
  link = atk_hypertext_get_link (ATK_HYPERTEXT(hypertext->atko),
				 (gint) linkIndex);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(spi_hyperlink_interface_new(ATK_OBJECT(link))));
  return rv;
}



static CORBA_long
impl_getLinkIndex (PortableServer_Servant _servant,
		   const CORBA_long characterIndex,
				  CORBA_Environment * ev)
{
  SpiHypertext *hypertext = SPI_HYPERTEXT(bonobo_object_from_servant(_servant));
  return (CORBA_long)
    atk_hypertext_get_link_index (ATK_HYPERTEXT(hypertext->atko),
				  (gint) characterIndex);
}

