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


SpiRelation *
spi_relation_new (AtkRelation *obj)
{
  SpiRelation *new_relation = g_object_new (SPI_RELATION_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_relation), ATK_OBJECT (obj));

  return new_relation;
}


static void
spi_relation_class_init (SpiRelationClass *klass)
{
  POA_Accessibility_Relation__epv *epv = &klass->epv;

  epv->getRelationType  = NULL; /* TODO: finish me! */
  epv->getNTargets      = NULL;
  epv->getTarget        = NULL;
}


static void
spi_relation_init (SpiRelation *relation)
{
}


BONOBO_TYPE_FUNC_FULL (SpiRelation,
		       Accessibility_Relation,
		       SPI_TYPE_BASE,
		       spi_relation);
