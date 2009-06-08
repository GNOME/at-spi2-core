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

from interfaces import ATSPI_ACCESSIBLE, ATSPI_APPLICATION
from base import BaseProxy, Enum
from factory import accessible_factory
from state import StateSet, _marshal_state_set
from relation import _marshal_relation_set
from role import Role, ROLE_NAMES

__all__ = [
           "LOCALE_TYPE",
           "LOCALE_TYPE_COLLATE",
           "LOCALE_TYPE_CTYPE",
           "LOCALE_TYPE_MESSAGES",
           "LOCALE_TYPE_MONETARY",
           "LOCALE_TYPE_NUMERIC",
           "LOCALE_TYPE_TIME",
           "BoundingBox",
           "Accessible",
          ]

#------------------------------------------------------------------------------

class LOCALE_TYPE(Enum):
    _enum_lookup = {
        0:'LOCALE_TYPE_MESSAGES',
        1:'LOCALE_TYPE_COLLATE',
        2:'LOCALE_TYPE_CTYPE',
        3:'LOCALE_TYPE_MONETARY',
        4:'LOCALE_TYPE_NUMERIC',
        5:'LOCALE_TYPE_TIME',
    }

LOCALE_TYPE_COLLATE = LOCALE_TYPE(1)
LOCALE_TYPE_CTYPE = LOCALE_TYPE(2)
LOCALE_TYPE_MESSAGES = LOCALE_TYPE(0)
LOCALE_TYPE_MONETARY = LOCALE_TYPE(3)
LOCALE_TYPE_NUMERIC = LOCALE_TYPE(4)
LOCALE_TYPE_TIME = LOCALE_TYPE(5)

#------------------------------------------------------------------------------

class BoundingBox(list):
    def __new__(cls, x, y, width, height):
        return list.__new__(cls, (x, y, width, height))
    def __init__(self, x, y, width, height):
        list.__init__(self, (x, y, width, height))

    def __str__(self):
        return ("(%d, %d, %d, %d)" % (self.x, self.y, self.width, self.height))

    def _get_x(self):
        return self[0]
    def _set_x(self, val):
        self[0] = val
    x = property(fget=_get_x, fset=_set_x)
    def _get_y(self):
        return self[1]
    def _set_y(self, val):
        self[1] = val
    y = property(fget=_get_y, fset=_set_y)
    def _get_width(self):
        return self[2]
    def _set_width(self, val):
        self[2] = val
    width = property(fget=_get_width, fset=_set_width)
    def _get_height(self):
        return self[3]
    def _set_height(self, val):
        self[3] = val
    height = property(fget=_get_height, fset=_set_height)

#------------------------------------------------------------------------------

class Accessible(BaseProxy):
    """
    The base interface which is implemented by all accessible objects.
    All objects support interfaces for querying their contained
    'children' and position in the accessible-object hierarchy,
    whether or not they actually have children.
    """

    _relation_set = None

    def __nonzero__(self):
            return True

    def __len__(self):
            return self.getChildCount()

    def __getitem__(self, index):
            return self.getChildAtIndex(index)

    def getApplication(self):
        """
        Get the containing Application for this object.
        @return the Application instance to which this object belongs.
        """
        return self._cache.create_application(self._app_name)

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
        func = self.get_dbus_method("getAttributes", dbus_interface=ATSPI_ACCESSIBLE)
        return [key + ':' + value for key, value in func().iteritems()]

    def getChildAtIndex(self, index):
        """
        Get the accessible child of this object at index. 
        @param : index
        an in parameter indicating which child is requested (zero-indexed).
        @return : the 'nth' Accessible child of this object.
        """
        path = self.cached_data.children[index]
        return self._cache.create_accessible(self._app_name, path, ATSPI_ACCESSIBLE)

    def getIndexInParent(self):
        """
        Get the index of this object in its parent's child list.
        @return : a long integer indicating this object's index in the
        parent's list.
        """
        if self.parent == None:
                return -1
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
        func = self.get_dbus_method("getLocalizedRoleName", dbus_interface=ATSPI_ACCESSIBLE)
        return func()

    def getRelationSet(self):
        """
        Get a set defining this object's relationship to other accessible
        objects. 
        @return : a RelationSet defining this object's relationships.
        """
        if self._relation_set:
                return self._relation_set
        else:
                func = self.get_dbus_method("getRelationSet", dbus_interface=ATSPI_ACCESSIBLE)
                relation_set = func()
                self._relation_set = _marshal_relation_set(self._cache, self._app_name, relation_set)
                return self._relation_set

    def getRole(self):
        """
        Get the Role indicating the type of UI role played by this object.
        @return : a Role indicating the type of UI role played by this
        object.
        """
        return Role(self.cached_data.role)

    def getRoleName(self):
        """
        Get a string indicating the type of UI role played by this object.
        @return : a UTF-8 string indicating the type of UI role played
        by this object.
        """
        """
        func = self.get_dbus_method("getRoleName", dbus_interface=ATSPI_ACCESSIBLE)
        return func()
        """
        return ROLE_NAMES[self.cached_data.role]

    def getState(self):
        """
        Get the current state of the object as a StateSet. 
        @return : a StateSet encapsulating the currently true states
        of the object.
        """
        return _marshal_state_set(self.cached_data.state)

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

    getChildCount = get_childCount

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
        return self._cache.create_accessible(self._app_name,
                                             self.cached_data.parent,
                                             ATSPI_ACCESSIBLE)

    _parentDoc = \
        """
        an Accessible object which is this object's containing object.
        """
    parent = property(fget=get_parent, doc=_parentDoc)

    def refresh(self):
            self._relation_set = None
            self._cache.application_cache[self._app_name]._refresh()

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(ATSPI_ACCESSIBLE, Accessible)

#END----------------------------------------------------------------------------
