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

/* hypertext.c : implements the HyperText interface */

#include <config.h>
#include <stdio.h>
#include <libspi/hyperlink.h>
#include <libspi/hypertext.h>

/* Static function declarations */

static GObjectClass *parent_class;

static void
spi_hypertext_finalize (GObject *obj)
{
  parent_class->finalize (obj);
}

SpiHypertext *
spi_hypertext_interface_new (AtkObject *obj)
{
  SpiHypertext *new_hypertext = g_object_new (SPI_HYPERTEXT_TYPE, NULL);
  (SPI_TEXT (new_hypertext))->atko = obj;
  g_object_ref (obj);
  return new_hypertext;
}



static CORBA_long
impl_getNLinks (PortableServer_Servant _servant,
		CORBA_Environment * ev)
{
  SpiHypertext *hypertext = SPI_HYPERTEXT (bonobo_object_from_servant(_servant));
  return (CORBA_long)
    atk_hypertext_get_n_links (ATK_HYPERTEXT ((SPI_TEXT (hypertext))->atko));
}



static Accessibility_Hyperlink
impl_getLink (PortableServer_Servant servant,
	      const CORBA_long       linkIndex,
	      CORBA_Environment     *ev)
{
  AtkHyperlink *link;
  SpiHypertext *hypertext;
  Accessibility_Hyperlink rv;
  
  hypertext = SPI_HYPERTEXT (bonobo_object_from_servant (servant));

  link = atk_hypertext_get_link (
	  ATK_HYPERTEXT ((SPI_TEXT (hypertext))->atko), linkIndex);

  rv = bonobo_object_corba_objref (BONOBO_OBJECT (
	  spi_hyperlink_new (ATK_OBJECT (link))));

  return rv;
}



static CORBA_long
impl_getLinkIndex (PortableServer_Servant _servant,
		   const CORBA_long characterIndex,
				  CORBA_Environment * ev)
{
  SpiHypertext *hypertext = SPI_HYPERTEXT(bonobo_object_from_servant(_servant));
  return (CORBA_long)
    atk_hypertext_get_link_index (ATK_HYPERTEXT ((SPI_TEXT (hypertext))->atko),
				  (gint) characterIndex);
}

static void
spi_hypertext_class_init (SpiHypertextClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Hypertext__epv *epv = &klass->epv;
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

BONOBO_TYPE_FUNC_FULL (SpiHypertext,
		       Accessibility_Hypertext,
		       BONOBO_TYPE_OBJECT,
		       spi_hypertext);
