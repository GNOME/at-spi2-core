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

/* relation.c : implements the Relation interface */

#include <config.h>
#include <stdio.h>
#include <libspi/accessible.h>
#include <libspi/relation.h>


static gboolean
spi_init_relation_type_table (Accessibility_RelationType *types)
{
  gint i;

  for (i = 0; i < ATK_RELATION_LAST_DEFINED; i++)
    types[i] = Accessibility_RELATION_NULL;

  types[ATK_RELATION_CONTROLLED_BY] = Accessibility_RELATION_CONTROLLED_BY;
  types[ATK_RELATION_CONTROLLER_FOR] = Accessibility_RELATION_CONTROLLER_FOR;
  types[ATK_RELATION_LABEL_FOR] = Accessibility_RELATION_LABEL_FOR;
  types[ATK_RELATION_LABELLED_BY] = Accessibility_RELATION_LABELLED_BY;
  types[ATK_RELATION_MEMBER_OF] = Accessibility_RELATION_MEMBER_OF;
  types[ATK_RELATION_NODE_CHILD_OF] = Accessibility_RELATION_NODE_CHILD_OF;
  types[ATK_RELATION_FLOWS_TO] = Accessibility_RELATION_FLOWS_TO;
  types[ATK_RELATION_FLOWS_FROM] = Accessibility_RELATION_FLOWS_FROM;
  types[ATK_RELATION_SUBWINDOW_OF] = Accessibility_RELATION_SUBWINDOW_OF;
  types[ATK_RELATION_EMBEDS] = Accessibility_RELATION_EMBEDS;
  types[ATK_RELATION_EMBEDDED_BY] = Accessibility_RELATION_EMBEDDED_BY;
  types[ATK_RELATION_POPUP_FOR] = Accessibility_RELATION_POPUP_FOR;
  types[ATK_RELATION_PARENT_WINDOW_OF] = Accessibility_RELATION_PARENT_WINDOW_OF;
  return TRUE;
}



static Accessibility_RelationType
spi_relation_type_from_atk_relation_type (AtkRelationType type)
{
  static gboolean is_initialized = FALSE;
  static Accessibility_RelationType spi_relation_type_table [ATK_RELATION_LAST_DEFINED];
  Accessibility_RelationType spi_type;

  if (!is_initialized)
    is_initialized = spi_init_relation_type_table (spi_relation_type_table);	   

  if (type > ATK_RELATION_NULL && type < ATK_RELATION_LAST_DEFINED)
    spi_type = spi_relation_type_table[type];
  else
    spi_type = Accessibility_RELATION_EXTENDED;
  return spi_type;
}



static AtkRelation *
get_relation_from_servant (PortableServer_Servant servant)
{
  SpiBase *base = SPI_BASE (bonobo_object_from_servant(servant));

  g_return_val_if_fail (base, NULL);
  return  ATK_RELATION(base->gobj);
}



static Accessibility_RelationType
impl_getRelationType (PortableServer_Servant servant,
		      CORBA_Environment * ev)
{
  AtkRelation *relation = get_relation_from_servant (servant);
  AtkRelationType type;

  g_return_val_if_fail (relation != NULL, 0);
  type = atk_relation_get_relation_type (relation);
  return spi_relation_type_from_atk_relation_type (type);
}


static CORBA_short
impl_getNTargets (PortableServer_Servant servant,
		  CORBA_Environment * ev)
{
  AtkRelation *relation = get_relation_from_servant(servant);
  g_return_val_if_fail (relation != NULL, 0);

  return relation->target ? relation->target->len : 0;
}


static CORBA_Object
impl_getTarget (PortableServer_Servant servant,
		const CORBA_short index,
		CORBA_Environment * ev)
{
  AtkObject *atk_object;
  AtkRelation *relation = get_relation_from_servant (servant);
  g_return_val_if_fail (relation, NULL);

  if (!relation->target ||
      index < 0 ||
      index >= relation->target->len)
    return CORBA_OBJECT_NIL;

  atk_object = g_ptr_array_index (relation->target, index);
  if (!atk_object)
    return CORBA_OBJECT_NIL;

  return spi_accessible_new_return (atk_object, FALSE, ev);
}


SpiRelation *
spi_relation_new (AtkRelation *obj)
{
  SpiRelation *new_relation = g_object_new (SPI_RELATION_TYPE, NULL);
  spi_base_construct (SPI_BASE (new_relation), G_OBJECT (obj));
  return new_relation;
}


static void
spi_relation_class_init (SpiRelationClass *klass)
{
  POA_Accessibility_Relation__epv *epv = &klass->epv;

  epv->getRelationType  = impl_getRelationType;  
  epv->getNTargets      = impl_getNTargets;
  epv->getTarget        = impl_getTarget;
}


static void
spi_relation_init (SpiRelation *relation)
{
}


BONOBO_TYPE_FUNC_FULL (SpiRelation,
		       Accessibility_Relation,
		       SPI_TYPE_BASE,
		       spi_relation)
