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

from weakref import proxy

from dbus.proxies import Interface, ProxyObject
from dbus.exceptions import *

class AccessibleObjectNoLongerExists(Exception):
	pass

#------------------------------------------------------------------------------

class Enum(int):
	def __str__(self):
		return self._enum_lookup(int(self))

#------------------------------------------------------------------------------

class BaseProxy(Interface):
	"""
	The base D-Bus proxy for a remote object that implements one or more
	of the AT-SPI interfaces.

	This class must be further specialised as the cache stores are different
	for Desktop objects and other Accessible objects.
	"""
	
	def __new__(cls, *args, **kwargs):
		Interface.__new__(cls, *args, **kwargs)

		queryable_interfaces = { 
			'Accessible':ATSPI_ACCESSIBLE,
			'Action':ATSPI_ACTION,
			'Application':ATSPI_APPLICATION,
			'Collection':ATSPI_COLLECTION,
			'Component':ATSPI_COMPONENT,
			'Desktop':ATSPI_DESKTOP,
			'Document':ATSPI_DOCUMENT,
			'EditableText':ATSPI_EDITABLE_TEXT,
			'Hypertext':ATSPI_HYPERTEXT,
			'Hyperlink':ATSPI_HYPERLINK,
			'Image':ATSPI_IMAGE,
			'Selection':ATSPI_SELECTION,
			'StreamableContent':ATSPI_STREAMABLE_CONTENT,
			'Table':ATSPI_TABLE,
			'Text':ATSPI_TEXT,
			'Value':ATSPI_VALUE,
		}

		for interface in queryable_interfaces.keys():
			name = 'query%s' % interface
    			def new_query(self, object):
				return self.queryInterface(object, queryable_interfaces[interface])
			setattr(cls, name, new_query) 

	def __init__(self, obj, cache, path, interface):
		"""
		Create a D-Bus Proxy for an ATSPI interface.

		obj - The D-Bus proxy object this interface uses for D-Bus calls.
		cache - Cache storing data for this object.
		path - The object path of the remote object.
		interface - The name of the ATSPI interface that this proxy implements.
		"""
		Interface.__init__(self, obj, interface)

		self._cobj = ref(cache)
		self._path = path

		self._pgetter = self.get_dbus_method("Get", dbus_interface="org.freedesktop.DBus.Properties")
		self._psetter = self.get_dbus_method("Set", dbus_interface="org.freedesktop.DBus.Properties")

	def __getattr__(self, *args, **kwargs):
		"""
		The __getattr__ function must return the D-Bus method wrapped in a
		method to translate exceptions.
		"""
		# Need to throw an AccessibleObjectNoLongerExists exception
		# on D-Bus error of the same type.
		try:
			return Interface.__getattr__(self, *args, **kwargs)
		except UnknownMethodException, e:
			raise NotImplementedError(e)
		except DBusException, e:
			raise LookupError(e)

	@property
	def _cache(self):
		c = self._cobj()
		if not c:
			raise AccessibleObjectNoLongerExits("Application has been removed")

	@property
	def _data(self):
		try:
			data = self._cache._objects[self._path]
		except KeyError:
			raise AccessibleObjectNoLongerExists, 'Cache data cannot be found for path %s' % (self._path,)
		return data

	@property
	def interfaces(self):
		return self._data.interfaces

	def queryInterface(self, interface):
		if interface in self._data.interfaces:
				return self._cache.proxyFactory(self._path, interface, dbus_obj=self._obj)
		else:
			raise NotImplementedError(
				"%s not supported by accessible object at path %s"
				% (interface, self.path))

#END----------------------------------------------------------------------------
