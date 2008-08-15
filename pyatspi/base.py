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

import dbus as _dbus
from dbus.proxies import Interface as _Interface

from dbus.exceptions import *
from factory import create_accessible, add_accessible_class

import constants

class AccessibleObjectNoLongerExists(Exception):
	pass

#------------------------------------------------------------------------------

class _Enum(int):
	def __str__(self):
		return self._enum_lookup(int(self))

#------------------------------------------------------------------------------


class _BaseProxyMeta(type):
	def __init__(cls, *args, **kwargs):
		type.__init__(cls, *args, **kwargs)

		queryable_interfaces = { 
			'Accessible':constants.ATSPI_ACCESSIBLE,
			'Action':constants.ATSPI_ACTION,
			'Application':constants.ATSPI_APPLICATION,
			'Collection':constants.ATSPI_COLLECTION,
			'Component':constants.ATSPI_COMPONENT,
			'Desktop':constants.ATSPI_DESKTOP,
			'Document':constants.ATSPI_DOCUMENT,
			'EditableText':constants.ATSPI_EDITABLE_TEXT,
			'Hypertext':constants.ATSPI_HYPERTEXT,
			'Hyperlink':constants.ATSPI_HYPERLINK,
			'Image':constants.ATSPI_IMAGE,
			'Selection':constants.ATSPI_SELECTION,
			'StreamableContent':constants.ATSPI_STREAMABLE_CONTENT,
			'Table':constants.ATSPI_TABLE,
			'Text':constants.ATSPI_TEXT,
			'Value':constants.ATSPI_VALUE,
		}

		for interface in queryable_interfaces.keys():
			name = 'query%s' % interface
    			def new_query(self, object):
				return self.queryInterface(object, queryable_interfaces[interface])
			setattr(cls, name, new_query) 

#------------------------------------------------------------------------------

class _BaseProxy(_Interface):
	"""
	The base D-Bus proxy for a remote object that implements one or more
	of the AT-SPI interfaces.
	"""

	__metaclass__ = _BaseProxyMeta

	def __init__(self, cache, app_name, acc_path, parent, interface, dbus_object=None, connection=None):
		"""
		Create a D-Bus Proxy for an ATSPI interface.

		cache - ApplicationCache, where the cached data for the accessible can be obtained.
		app_name - D-Bus bus name of the application this accessible belongs to.
		acc_path - D-Bus object path of the server side accessible object.
		parent - Parent accessible.
		interface - D-Bus interface of the object. Used to decide which accessible class to instanciate.
		dbus_object(kwarg) - The D-Bus proxy object used by the accessible for D-Bus method calls.
		"""
		self._cache = cache
		self._app_name = app_name
		self._acc_path = acc_path
		self._parent = parent

		if not dbus_object:
			dbus_object = connection.get_object(self._app_name, self._acc_path, introspect=False)
		self._dbus_object = dbus_object

		_Interface.__init__(self, self._dbus_object, interface)

		self._pgetter = self.get_dbus_method("Get", dbus_interface="org.freedesktop.DBus.Properties")
		self._psetter = self.get_dbus_method("Set", dbus_interface="org.freedesktop.DBus.Properties")

	def __getattr__(self, *args, **kwargs):
		method =  _Interface.__getattr__(self, *args, **kwargs)

		def dbus_method_func(*args, **kwargs):
			# TODO Need to throw an AccessibleObjectNoLongerExists exception
			# on D-Bus error of the same type.
			try:
				method(*args, **kwargs)
			except UnknownMethodException, e:
				raise NotImplementedError(e)
			except DBusException, e:
				raise LookupError(e)

		return dbus_method_func

	@property
	def cached_data(self):
		try:
			return self._cache[self._app_name][self._acc_path]
		except KeyError:
			raise AccessibleObjectNoLongerExists, \
				'Cache data cannot be found for path %s in app %s' % (self._acc_path, self._app_name)

	@property
	def interfaces(self):
		return self._data.interfaces

	def queryInterface(self, interface):
		"""
		Gets a different accessible interface for this object
		or raises a NotImplemented error if the given interface
		is not supported.
		"""
		if interface in self._data.interfaces:
			return create_accessible(self._cache,
					 	 self._app_name,
						 self._acc_path,
						 self._parent,
						 interface,
						 dbus_object=self._dbus_object)
		else:
			raise NotImplementedError(
				"%s not supported by accessible object at path %s"
				% (interface, self.path))

#------------------------------------------------------------------------------

class Accessible(_BaseProxy):
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
	application_root = self._cache.get_application_root(self._app_name)
	return create_accessible(self._cache,
			 	 self._app_name,
				 application_root,
				 constants.NULL_BUS_NAME,
				 constants.NULL_OBJECT_PATH,
				 constants.ATSPI_ACCESSIBLE,
				 dbus_object=self._dbus_object)
    
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
				 self,
				 constants.ATSPI_ACCESSIBLE,
				 dbus_object=self._dbus_object)
    
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
    
    def getRole(self):
        """
        Get the Role indicating the type of UI role played by this object.
        @return : a Role indicating the type of UI role played by this
        object.
        """
        return self.cached_data.role
    
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
    
    def unimplemented(self, *args, **kwargs):
        """
        /cond future expansion
        """
        func = self.get_dbus_method("unimplemented")
        return func(*args, **kwargs)
    
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
	# The parent attribute is part of the base proxy
	return self._parent
    _parentDoc = \
        """
        an Accessible object which is this object's containing object.
        """
    parent = property(fget=get_parent, doc=_parentDoc)

# ATTENTION - Register the Accessible class with the accessible factory.
add_accessible_class(constants.ATSPI_ACCESSIBLE, Accessible)

#END----------------------------------------------------------------------------
