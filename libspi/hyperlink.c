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
 * This pulls the definition of the hyperlink bonobo object
 */
#include "hyperlink.h"

/*
 * Static function declarations
 */

static void
hyperlink_class_init (HyperlinkClass *klass);
static void
hyperlink_init (Hyperlink *hyperlink);
static void
hyperlink_finalize (GObject *obj);
static CORBA_string
impl_getURI (PortableServer_Servant _servant,
	     const CORBA_long i, CORBA_Environment * ev);
static CORBA_short
impl__get_n_anchors (PortableServer_Servant _servant,
		     CORBA_Environment * ev);
static CORBA_long
impl__get_startIndex (PortableServer_Servant _servant,
		      CORBA_Environment * ev);
static CORBA_long
impl__get_endIndex (PortableServer_Servant _servant,
		    CORBA_Environment * ev);
static Accessibility_Accessible
impl_getObject (PortableServer_Servant _servant,
		const CORBA_long i,
		CORBA_Environment * ev);
static CORBA_boolean
impl_isValid (PortableServer_Servant _servant,
	      CORBA_Environment * ev);

static GObjectClass *parent_class;

GType
hyperlink_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (HyperlinkClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) hyperlink_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (Hyperlink),
      0, /* n preallocs */
      (GInstanceInitFunc) hyperlink_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_Hyperlink__init,
			       NULL,
			       G_STRUCT_OFFSET (HyperlinkClass, epv),
			       &tinfo,
			       "AccessibleHyperlink");
  }

  return type;
}

static void
hyperlink_class_init (HyperlinkClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Hyperlink__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = hyperlink_finalize;

  /* Initialize epv table */

  epv->_get_n_anchors = impl__get_n_anchors;
  epv->getURI = impl_getURI;
  epv->_get_startIndex = impl__get_startIndex;
  epv->_get_endIndex = impl__get_endIndex;
  epv->getObject = impl_getObject;
  epv->isValid = impl_isValid;
}

static void
hyperlink_init (Hyperlink *hyperlink)
{
}

static void
hyperlink_finalize (GObject *obj)
{
  Hyperlink *hyperlink = HYPERLINK(obj);
  g_object_unref (hyperlink->atko);
  hyperlink->atko = NULL;
  parent_class->finalize (obj);
}

Hyperlink *
hyperlink_interface_new (AtkObject *obj)
{
  Hyperlink *new_hyperlink = 
    HYPERLINK(g_object_new (HYPERLINK_TYPE, NULL));
  new_hyperlink->atko = obj;
  g_object_ref (obj);
  return new_hyperlink;
}



static CORBA_short
impl__get_n_anchors (PortableServer_Servant _servant,
		     CORBA_Environment * ev)
{
  Hyperlink *link = HYPERLINK(bonobo_object_from_servant(_servant));
  return (CORBA_short) atk_hyperlink_get_n_anchors (ATK_HYPERLINK(link->atko));
}



static CORBA_long
impl__get_startIndex (PortableServer_Servant _servant,
		      CORBA_Environment * ev)
{
  Hyperlink *link = HYPERLINK(bonobo_object_from_servant(_servant));
  return (CORBA_long) atk_hyperlink_get_start_index (ATK_HYPERLINK(link->atko));
}



static CORBA_long
impl__get_endIndex (PortableServer_Servant _servant,
		    CORBA_Environment * ev)
{
  Hyperlink *link = HYPERLINK(bonobo_object_from_servant(_servant));
  return (CORBA_long) atk_hyperlink_get_end_index (ATK_HYPERLINK(link->atko));
}



static CORBA_string
impl_getURI (PortableServer_Servant _servant,
  const CORBA_long i, CORBA_Environment * ev)
{
  Hyperlink *link = HYPERLINK(bonobo_object_from_servant(_servant));
  gchar *uri;
  CORBA_char *rv;
  uri = atk_hyperlink_get_uri (ATK_HYPERLINK(link->atko), (gint) i);
  rv = CORBA_string_dup (uri);
  g_free (uri);
  return rv;
} 



static Accessibility_Accessible
impl_getObject (PortableServer_Servant _servant,
		const CORBA_long i,
		CORBA_Environment * ev)
{
  Hyperlink *link = HYPERLINK(bonobo_object_from_servant(_servant));
  AtkObject *atk_object;
  Accessibility_Accessible rv;
  atk_object = atk_hyperlink_get_object (ATK_HYPERLINK(link->atko), (gint) i);
  rv = bonobo_object_corba_objref (BONOBO_OBJECT(accessible_new(atk_object)));
  return rv;
}



static CORBA_boolean
impl_isValid (PortableServer_Servant _servant,
	      CORBA_Environment * ev)
{
  Hyperlink *link = HYPERLINK(bonobo_object_from_servant(_servant));
  return (CORBA_boolean) atk_hyperlink_is_valid (ATK_HYPERLINK(link->atko));
}


