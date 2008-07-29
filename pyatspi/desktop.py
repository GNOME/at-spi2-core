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

from base import BaseProxy

from accessible import Accessible

class Desktop(Accessible):
    """
    The desktop class implements the 
    accessible interface, but uses a different
    method to access its chilren and to obtain cached
    data. 

    This is because application caches are separate
    and the children of the desktop are applications.
    The child access must get the root accessible
    of the application cache that is accessed.

    The data is accessed from the desktop cache.
    """

	@property
	def _data(self):
		try:
			data = self._cache._objects[self._path]
		except KeyError, ReferenceError:
			raise AccessibleObjectNoLongerExists, 'Cache data cannot be found for path %s' % (self._path,)
		return data
    
    def getApplication(self, *args, **kwargs):
        """
        Get the containing Application for this object.
        @return the Application instance to which this object belongs.
        """
        func = self.get_dbus_method("getApplication")
        return func(*args, **kwargs)
    
    def getAttributes(self, *args, **kwargs):
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
        @return : an AttributeSet encapsulating any "attribute values"
        currently defined for the object.
        """
        func = self.get_dbus_method("getAttributes")
        return func(*args, **kwargs)
    
    def getChildAtIndex(self, *args, **kwargs):
        """
        Get the accessible child of this object at index. 
        @param : index
        an in parameter indicating which child is requested (zero-indexed).
        @return : the 'nth' Accessible child of this object.
        """
        func = self.get_dbus_method("getChildAtIndex")
        return func(*args, **kwargs)
    
    def getIndexInParent(self, *args, **kwargs):
        """
        Get the index of this object in its parent's child list. 
        @return : a long integer indicating this object's index in the
        parent's list.
        """
        func = self.get_dbus_method("getIndexInParent")
        return func(*args, **kwargs)
    
    def getLocalizedRoleName(self, *args, **kwargs):
        """
        Get a string indicating the type of UI role played by this object,
        translated to the current locale.
        @return : a UTF-8 string indicating the type of UI role played
        by this object.
        """
        func = self.get_dbus_method("getLocalizedRoleName")
        return func(*args, **kwargs)
    
    def getRelationSet(self, *args, **kwargs):
        """
        Get a set defining this object's relationship to other accessible
        objects. 
        @return : a RelationSet defining this object's relationships.
        """
        func = self.get_dbus_method("getRelationSet")
        return func(*args, **kwargs)
    
    def getRole(self, *args, **kwargs):
        """
        Get the Role indicating the type of UI role played by this object.
        @return : a Role indicating the type of UI role played by this
        object.
        """
        func = self.get_dbus_method("getRole")
        return func(*args, **kwargs)
    
    def getRoleName(self, *args, **kwargs):
        """
        Get a string indicating the type of UI role played by this object.
        @return : a UTF-8 string indicating the type of UI role played
        by this object.
        """
        func = self.get_dbus_method("getRoleName")
        return func(*args, **kwargs)
    
    def getState(self, *args, **kwargs):
        """
        Get the current state of the object as a StateSet. 
        @return : a StateSet encapsulating the currently true states
        of the object.
        """
        func = self.get_dbus_method("getState")
        return func(*args, **kwargs)
    
    def isEqual(self, *args, **kwargs):
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
        func = self.get_dbus_method("isEqual")
        return func(*args, **kwargs)
    
    def unimplemented(self, *args, **kwargs):
        """
        /cond future expansion
        """
        func = self.get_dbus_method("unimplemented")
        return func(*args, **kwargs)
    
    def get_childCount(self):
        self._pgetter(self._dbus_interface, "childCount")
    def set_childCount(self, value):
        self._psetter(self._dbus_interface, "childCount", value)
    _childCountDoc = \
        """
        childCount: the number of children contained by this object.
        """
    childCount = property(fget=get_childCount, fset=set_childCount, doc=_childCountDoc)
    
    def get_description(self):
        self._pgetter(self._dbus_interface, "description")
    def set_description(self, value):
        self._psetter(self._dbus_interface, "description", value)
    _descriptionDoc = \
        """
        a string describing the object in more detail than name.
        """
    description = property(fget=get_description, fset=set_description, doc=_descriptionDoc)
    
    def get_name(self):
        self._pgetter(self._dbus_interface, "name")
    def set_name(self, value):
        self._psetter(self._dbus_interface, "name", value)
    _nameDoc = \
        """
        a (short) string representing the object's name.
        """
    name = property(fget=get_name, fset=set_name, doc=_nameDoc)
    
    def get_parent(self):
        self._pgetter(self._dbus_interface, "parent")
    def set_parent(self, value):
        self._psetter(self._dbus_interface, "parent", value)
    _parentDoc = \
        """
        an Accessible object which is this object's containing object.
        """
    parent = property(fget=get_parent, fset=set_parent, doc=_parentDoc)
