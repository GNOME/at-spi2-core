#Copyright (C) 2008 Codethink Ltd

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License version 2 as published by the Free Software Foundation.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU Lesser General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

import interfaces
from base import BaseProxy
from factory import create_accessible, add_accessible_class
from stateset import StateSet, _marshal_state_set
from relation import _marshal_relation_set

__all__ = [
	   "Accessible",
	  ]

#------------------------------------------------------------------------------

class Accessible(BaseProxy):
    """
    The base interface which is implemented by all accessible objects.
    All objects support interfaces for querying their contained
    'children' and position in the accessible-object hierarchy,
    whether or not they actually have children.
    """
    
    def getApplication(self):
        """
        Get the containing Application for this object.
        @return the Application instance to which this object belongs.
        """
	application_root = self._cache[self._app_name]._get_root()
	#TODO Set the desktop object as the parent of this.
	return create_accessible(self._cache,
			 	 self._app_name,
				 application_root,
				 interfaces.ATSPI_APPLICATION,
				 connection=self._cache._connection)
    
    def getAttributes(self):
        """
        Get a list of properties applied to this object as a whole, as
        an AttributeSet consisting of name-value pairs. As such these
        attributes may be considered weakly-typed properties or annotations,
        as distinct from the strongly-typed interface instance data declared
        using the IDL "attribute" keyword.
        Not all objects have explicit "name-value pair" AttributeSet
        properties.
        Attribute names and values may have any UTF-8 string value, however
        where possible, in order to facilitate consistent use and exposure
        of "attribute" properties by applications and AT clients, attribute
        names and values should chosen from a publicly-specified namespace
        where appropriate.
        Where possible, the names and values in the name-value pairs
        should be chosen from well-established attribute namespaces using
        standard semantics. For example, attributes of Accessible objects
        corresponding to XHTML content elements should correspond to
        attribute names and values specified in the w3c XHTML specification,
        at http://www.w3.org/TR/xhtml2, where such values are not already
        exposed via a more strongly-typed aspect of the AT-SPI API. Metadata
        names and values should be chosen from the 'Dublin Core' Metadata
        namespace using Dublin Core semantics: http://dublincore.org/dcregistry/
        Similarly, relevant structural metadata should be exposed using
        attribute names and values chosen from the CSS2 and WICD specification:
        http://www.w3.org/TR/1998/REC-CSS2-19980512 WICD (http://www.w3.org/TR/2005/WD-WICD-20051121/).

        @return : An AttributeSet encapsulating any "attribute values"
        currently defined for the object. An attribute set is a list of strings
	with each string comprising an name-value pair format 'name:value'.
        """
        func = self.get_dbus_method("getAttributes")
        return func()
    
    def getChildAtIndex(self, index):
        """
        Get the accessible child of this object at index. 
        @param : index
        an in parameter indicating which child is requested (zero-indexed).
        @return : the 'nth' Accessible child of this object.
        """
	path = self.cached_data.children[index]
	return create_accessible(self._cache,
			 	 self._app_name,
				 path,
				 interfaces.ATSPI_ACCESSIBLE,
				 connection=self._cache._connection)
    
    def getIndexInParent(self):
        """
        Get the index of this object in its parent's child list. 
        @return : a long integer indicating this object's index in the
        parent's list.
        """
	for i in range(0, self.parent.childCount):
		child = self.parent.getChildAtIndex(i)
		if self.isEqual(child):
			return i
	raise AccessibleObjectNoLongerExists("Child not found within parent")
    
    def getLocalizedRoleName(self):
        """
        Get a string indicating the type of UI role played by this object,
        translated to the current locale.
        @return : a UTF-8 string indicating the type of UI role played
        by this object.
        """
        func = self.get_dbus_method("getLocalizedRoleName")
        return func()
    
    def getRelationSet(self):
        """
        Get a set defining this object's relationship to other accessible
        objects. 
        @return : a RelationSet defining this object's relationships.
        """
        func = self.get_dbus_method("getRelationSet")
        relation_set = func()
        return _marshal_relation_set(self._cache, self._dbus_object, self._app_name, relation_set)
    
    def getRole(self):
        """
        Get the Role indicating the type of UI role played by this object.
        @return : a Role indicating the type of UI role played by this
        object.
        """
        return self.cached_data.role
    
    def getRoleName(self):
        """
        Get a string indicating the type of UI role played by this object.
        @return : a UTF-8 string indicating the type of UI role played
        by this object.
        """
        func = self.get_dbus_method("getRoleName")
        return func()
    
    def getState(self):
        """
        Get the current state of the object as a StateSet. 
        @return : a StateSet encapsulating the currently true states
        of the object.
        """
        func = self.get_dbus_method("getState")
        bitfield = func()
	return _marshal_state_set(bitfield)
    
    def isEqual(self, accessible):
        """
        Determine whether an Accessible refers to the same object as
        another. This method should be used rather than brute-force comparison
        of object references (i.e. "by-value" comparison), as two object
        references may have different apparent values yet refer to the
        same object.
        @param : obj
        an Accessible object reference to compare to 
        @return : a boolean indicating whether the two object references
        point to the same object.
        """
        return  (self._app_name == accessible._app_name) and \
		(self._acc_path == accessible._acc_path)	
    
    def get_childCount(self):
        return len(self.cached_data.children)
    _childCountDoc = \
        """
        childCount: the number of children contained by this object.
        """
    childCount = property(fget=get_childCount, doc=_childCountDoc)
    
    def get_description(self):
        return self.cached_data.description
    _descriptionDoc = \
        """
        a string describing the object in more detail than name.
        """
    description = property(fget=get_description, doc=_descriptionDoc)
    
    def get_name(self):
        return self.cached_data.name
    _nameDoc = \
        """
        a (short) string representing the object's name.
        """
    name = property(fget=get_name, doc=_nameDoc)
    
    def get_parent(self):
	if self._parent:
		return self._parent
	else:
		return create_accessible(self._cache,
				 	 self._app_name,
					 self.cached_data.parent,
					 interfaces.ATSPI_ACCESSIBLE,
				 	 connection=self._cache._connection)

    _parentDoc = \
        """
        an Accessible object which is this object's containing object.
        """
    parent = property(fget=get_parent, doc=_parentDoc)

# Register the Accessible class with the accessible factory.
add_accessible_class(interfaces.ATSPI_ACCESSIBLE, Accessible)

#END----------------------------------------------------------------------------
