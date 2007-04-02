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

#include <cspi/spi.h>
#include <cspi/spi-private.h>

/**
 * AccessibleCollection_ref:
 * @obj: a pointer to the #AccessibleCollection object on which to operate.
 *
 * Increment the reference count for an #AccessibleCollection object.
 **/

void
AccessibleCollection_ref (AccessibleCollection *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleCollection_unref:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Decrement the reference count for an #AccessibleCollection object.
 **/

void
AccessibleCollection_unref (AccessibleCollection *obj)
{
  cspi_object_unref (obj);
}

/**
 * AccessibleCollection_createMatchRule:
 * @obj: a pointer to the #Accessible object on which to operate.
 * @states: An #AccessibleStateSet whose states are to be compared with the objects in the collection.
 * @statematchtype A MatchType which determines the match requirements for the 'states' criterion.
 * @attributes: An #AccessibleAttributeSet of attributes which should present 
 * in members of a matching set. 
 * @attributematchtype: A MatchType which determines the match requirements for the 'attributes' criterion.
 * @roles: An #AcceessibleRoleSet specifying the #AccessibleRole values which are to be considered among the match criteria.
 * @rolematchtype: A MatchType which determines the match requirements for the 'roles' criterion.
 * @interfaces: A string which represents one or more IDL interface definitions which should be implemented by matching objects.
 * @interfacematchtype: A MatchType which determines the match requirements for the 'interfaces' criterion.
 * @invert: If TRUE, the resulting MatchRule will return objects which do NOT match the specified criteria, 
 * instead of those which do; i.e. this parameter "inverts" the sense of the resulting query.
 *
 * Returns an #AccesibleMatchRule object which encapsulates the specified criteria.
 **/

AccessibleMatchRule *
AccessibleCollection_createMatchRule (AccessibleCollection *obj,
				      AccessibleStateSet *states,
				      AccessibleCollectionMatchType statematchtype,
				      AccessibleAttributeSet *attributes,
				      AccessibleCollectionMatchType attributematchtype,
				      AccessibleRoleSet *roles,
				      AccessibleCollectionMatchType rolematchtype,
				      char *interfaces,
				      AccessibleCollectionMatchType interfacematchtype,
				      long int invert)
{

     Accessibility_AttributeSet *attributeset;
     Accessibility_RoleSet *roleset;
     Accessibility_MatchRule retval;
     int i;

     cspi_return_val_if_fail (obj != NULL, NULL);

     attributeset = CORBA_sequence_CORBA_string__alloc ();
     attributeset->_length = attributes->len;
     attributeset->_buffer = CORBA_sequence_CORBA_string_allocbuf (attributeset->_length);

     for (i = 0; i < attributeset->_length; i++)
	  attributeset->_buffer [i] =  CORBA_string_dup (attributes->attributes [i]);

     CORBA_sequence_set_release (attributeset, TRUE);

     roleset =  Accessibility_RoleSet__alloc ();
     roleset->_length = roles->len;
     roleset->_buffer =  Accessibility_RoleSet_allocbuf (roleset->_length);
					
     for (i = 0; i < roleset->_length; i++)
	    roleset->_buffer [i] = roles->roles [i];

     retval =  Accessibility_Collection_createMatchRule (CSPI_OBJREF (obj),
							 states,
							 statematchtype,
							 attributeset, 
							 attributematchtype,
							 roleset, 
							 rolematchtype,
							 interfaces,
							 interfacematchtype,
							 invert,
							 cspi_ev ());

     cspi_return_val_if_ev ("createMatchRule", NULL);

     return cspi_object_add (retval);
    

}

/**
 * AccessibleCollection_freeMatchRule:
 * @obj: a pointer to the #Accessible object on which to operate.
 * @matchrule: a pointer to the #AccessibleMatchRule to free
 *
 * Frees the resources associated with the specified #AccessibleMatchRule
 **/

void
AccessibleCollection_freeMatchRule (AccessibleCollection *obj,
				    AccessibleMatchRule  *matchrule)
{

     cspi_return_if_fail (obj != NULL);
     
     Accessibility_Collection_freeMatchRule (CSPI_OBJREF (obj),
					     CSPI_OBJREF (matchrule),
					     cspi_ev ());

      cspi_return_if_ev ("freeMatchRule");
}
