#Copyright (C) 2008 Codethink Ltd

#his program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

import dbus

from weakref import ref
from dbus.proxies import Interface, ProxyObject

ATSPI_ACCESSIBLE = 'org.freedesktop.atspi.Accessible'
ATSPI_ACCESSIBLE_TREE = 'org.freedesktop.atspi.Tree'

class AccessibleObjectDoesNotExist(Exception):
	pass

class AccessibleInterfaceNotSupported(Exception):
	pass

class _CacheData(object):
	__slots__ = [
			'parent',
			'interfaces',
			'children',
			'role',
			'name',
			'description',
			'proxy'
		]

	def __init__(self, data):
		self.update(data)
		self.proxy = None

	def update(self, data):
		#Don't cache the path here, used as lookup in cache object dict.
		(path,
		self.parent,
		self.children,
		self.interfaces,
		self.name,
		self.role,
		self.description) = data

class AccessibleCache(object):
	"""
	Caches data for a collection of accessible object proxies.

	The cache only holds a weak reference to any D-Bus proxies,
	they are created on demand.
	"""

	_UPDATE_SIGNAL = 'updateTree'

	def __init__(self, connection, busName, objectStorePath):
		"""
		Creates a cache for accessible object data.

		All accessible object proxies are created and accessed through this cache.

		connection - DBus connection.
		busName - DBus bus name where accessible tree resides.
		objectStorePath - Path where the accessible tree can be accessed.
		"""
		storeObject = connection.get_object(busName, objectStorePath)

		self._connection = connection
		self._busName = busName
		self._accessibleStore = dbus.Interface(storeObject, ATSPI_ACCESSIBLE_TREE)
		self._objects = {}
		#TODO are we caching the root accessible or not?
		#Do we need a roundtrip to access this?
		#Does the root accessible ever change?
		#Probably not as the root accessible is the 'Application'
		self._root = self._accessibleStore.getRoot()

		self._updateObjects(self._accessibleStore.getTree())

		#Connect to update signal
		self._signalMatch = self._accessibleStore.connect_to_signal(self._UPDATE_SIGNAL,
									    self._updateHandler)

	def getRootAccessible(self):
		"""
		Gets the accessible object at the root of the tree.
		"""
		return self.getAccessible(self._root)

	def getAccessible(self, path):
		"""
		Gets a D-Bus proxy object for the given object path.

		path - The D-Bus path of the remote object.
		"""
		if path in self._objects:
			cachedata = self._objects[path]
			proxy = None
			if cachedata.proxy is not None:
				proxy = cachedata.proxy()
			if proxy is None:
				proxy = AccessibleObjectProxy(self,
							cachedata,
							self._connection,
							self._busName,
							path)
				cachedata.proxy = ref(proxy)
			return proxy
		else:
			raise AccessibleObjectDoesNotExist, "D-Bus reference not found in cache"

	def _updateObjects(self, objects):
		"""
		Updates the object cache from an
		array of accessible object cache data.
		"""
		for data in objects:
			#First element is the object path.
			path = data[0]
			if path in self._objects:
				cachedata = self._objects[path]
				cachedata.update(data)
			else:
				self._objects[path] = _CacheData(data)

	def _removeObjects(self, paths):
		"""
		Removes the object data from the cache.
		"""
		for path in paths:
			del(self._objects[path])

	def _updateHandler(self, updates):
		objects, paths = updates
		self._removeObjects(paths)
		self._updateObjects(objects)

class AccessibleObjectProxy(ProxyObject):
	"""
	A D-Bus proxy for a remote object that implements one or more of the AT-SPI
	Accessibility interfaces.
	"""

	def __init__(self, cache, data, connection, busName, path):
		"""
		Create an accessible object.

		cache - The accessible cache that this object is owned by.
		connection - The D-Bus connection
		busName - The location on the bus where the remote object is located.
		path - The object path of the remote object.
		"""
		ProxyObject.__init__(self, connection, busName, path, introspect=False)

		self._cache = cache
		self._path = path
	
	@property
	def _data(self):
		try:
			data = self._cache._objects[self._path]
		except KeyError:
			raise AccessibleObjectDoesNotExist(
				'Cache data cannot be found for path %s' % (self._path,))
		return data

	@property
	def path(self):
		return self._path

	@property
	def parent(self):
		return self._cache.getAccessible(self._data.parent)

	@property
	def numChildren(self):
		return len(self._data.children)

	def getChild(self, index):
		return self._cache.getAccessible(self._data.children[index])

	@property
	def interfaces(self):
		return self._data.interfaces

	def getInterface(self, interface):
		if interface in self._data.interfaces:
			return Interface(self, interface)
		else:
			raise AccessibleInterfaceNotSupported(
				"%s not supported by accessible object at path %s"
				% (interface, self.path))

	@property
	def name(self):
		return self._data.name

	@property
	def role(self):
		return self._data.role

	@property
	def description(self):
		return self._data.description
