/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2007 IBM Corp.
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

/* collection.c: implements the Collection interface */

#include <config.h>
#include <stdio.h>
#include <bonobo/bonobo-exception.h>
#include <libspi/collection.h>
#include <libspi/matchrule.h> 
#include <libspi/spi-private.h>

typedef struct _MatchRulePrivate MatchRulePrivate;

struct _MatchRulePrivate {

     Accessibility_StateSet states;
     Accessibility_Collection_MatchType statematchtype;
     Accessibility_AttributeSet  *attributes;
     Accessibility_Collection_MatchType attributematchtype;
     Accessibility_RoleSet *roles;
     Accessibility_Collection_MatchType rolematchtype;
     CORBA_char *interfaces;
     Accessibility_Collection_MatchType interfacematchtype;
     gboolean invert;

};

#define MATCH_RULE_PRIVATE(o)\
     G_TYPE_INSTANCE_GET_PRIVATE((o), SPI_MATCHRULE_TYPE, MatchRulePrivate)


SpiCollection *
spi_collection_interface_new ()
{

     SpiCollection *new_collection = g_object_new (SPI_COLLECTION_TYPE, NULL);

     spi_base_construct_default (SPI_BASE (new_collection));

     return new_collection;

}

/*
static SpiCollection *
get_collection_from_servant (PortableServer_Servant servant)
{
     SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

     g_return_val_if_fail (object, NULL);
     
     return SPI_COLLECTION (object->gobj);
				
}
*/

static Accessibility_MatchRule 
impl_createMatchRule (PortableServer_Servant servant,
		      const Accessibility_StateSet states,
		      const Accessibility_Collection_MatchType statematchtype,
		      const Accessibility_AttributeSet *attributes,
		      const Accessibility_Collection_MatchType attributematchtype,
		      const Accessibility_RoleSet *roles,
		      const Accessibility_Collection_MatchType rolematchtype,
		      const CORBA_char *interfaces,
		      const Accessibility_Collection_MatchType interfacematchtype,
		      const CORBA_boolean invert,
		      CORBA_Environment *ev){

     Accessibility_MatchRule  retval = NULL;

     SpiMatchrule *matchrule = spi_matchrule_interface_new ();
     MatchRulePrivate *mrp   = MATCH_RULE_PRIVATE (matchrule);
     int i;

     mrp->states  = states;
     mrp->statematchtype = statematchtype;

/* attributes */

     mrp->attributes = CORBA_sequence_CORBA_string__alloc ();
     mrp->attributes->_maximum = attributes->_maximum;
     mrp->attributes->_length = attributes->_length;
     mrp->attributes->_buffer = CORBA_sequence_CORBA_string_allocbuf (attributes->_length);

     for (i = 0; i < mrp->attributes->_length; i++)
	  mrp->attributes->_buffer [i]= CORBA_string_dup (attributes->_buffer [i]);
    
     CORBA_sequence_set_release (mrp->attributes, TRUE);
     mrp->attributematchtype = attributematchtype;

/* roles */

     mrp->roles = Accessibility_RoleSet__alloc ();
     mrp->roles->_maximum = roles->_maximum;
     mrp->roles->_length = roles->_length;
     mrp->roles->_buffer = Accessibility_RoleSet_allocbuf (roles->_length);

     for (i = 0; i < roles->_length; i++)
	  mrp->roles->_buffer [i] = roles->_buffer [i];
     
     CORBA_sequence_set_release (mrp->roles, TRUE);
     mrp->rolematchtype = rolematchtype;

/* interfaces */
     
     mrp->interfaces = CORBA_string_dup (interfaces);
     mrp->interfacematchtype = interfacematchtype;

     mrp->invert = invert;
     
     retval = CORBA_Object_duplicate (BONOBO_OBJREF (matchrule), ev);

     return retval;

}

static void impl_freeMatchRule (PortableServer_Servant servant,
				Accessibility_MatchRule matchrule,     
				CORBA_Environment *ev){

     SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));
     SpiMatchrule *spimatchrule;

     MatchRulePrivate *mrp;  

     spimatchrule = SPI_MATCHRULE (object->gobj);
     mrp =  MATCH_RULE_PRIVATE (spimatchrule);;
     
     CORBA_free (mrp->attributes);
     CORBA_free (mrp->roles);
     CORBA_free (mrp->interfaces);     
     
     g_free (spimatchrule);

}
static void
spi_collection_class_init (SpiCollectionClass *klass)
{

    POA_Accessibility_Collection__epv *epv  = &klass->epv;

    /*    epv->isAncestorOf = impl_isAncestorOf; */
    
   epv->createMatchRule =  impl_createMatchRule;
   epv->freeMatchRule = impl_freeMatchRule;
   
   /*
     epv->getChildren = impl_getChildren;
     epv->getPreviousChildren = impl_getPreviousChildren;
     epv->getNextChildren = impl_getNextChildren;
     epv->getActiveDescendant = impl_getActiveDescendant;
   */

   g_type_class_add_private (klass, sizeof (MatchRulePrivate));
}

static void
spi_collection_init (SpiCollection *collection)
{
}

BONOBO_TYPE_FUNC_FULL (SpiCollection,
		       Accessibility_Collection,
		       SPI_TYPE_BASE,
		       spi_collection)

