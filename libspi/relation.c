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

/* relation.c : implements the Relation interface */

#include <config.h>
#include <stdio.h>
#include <libspi/relation.h>

/* Static function declarations */

static void
spi_relation_class_init (SpiRelationClass *klass);
static void
spi_relation_init (SpiRelation *relation);
static void
spi_relation_finalize (GObject *obj);
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
spi_relation_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (SpiRelationClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) spi_relation_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (SpiRelation),
      0, /* n preallocs */
      (GInstanceInitFunc) spi_relation_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_TYPE_OBJECT,
			       POA_Accessibility_Relation__init,
			       NULL,
			       G_STRUCT_OFFSET (SpiRelationClass, epv),
			       &tinfo,
			       "SpiAccessibleRelation");
  }

  return type;
}

static void
spi_relation_class_init (SpiRelationClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Relation__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = spi_relation_finalize;

  epv->getRelationType  = NULL; /* TODO: finish me! */
  epv->getNTargets      = NULL;
  epv->getTarget        = NULL;
}

static void
spi_relation_init (SpiRelation *relation)
{
}

static void
spi_relation_finalize (GObject *obj)
{
  SpiRelation *relation = SPI_RELATION(obj);
  g_object_unref (relation->relation);
  relation->relation = NULL;
  parent_class->finalize (obj);
}

SpiRelation *
spi_relation_new (AtkRelation *obj)
{
  SpiRelation *new_relation = 
    SPI_RELATION (g_object_new (SPI_RELATION_TYPE, NULL));
  new_relation->relation = obj;
  g_object_ref (obj);
  return new_relation;
}

