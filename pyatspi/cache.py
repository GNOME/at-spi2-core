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

from base import AccessibleObjectNoLongerExists
from factory import interfaceFactory

ATSPI_DEVICE_EVENT_CONTROLLER = 'org.freedesktop.atspi.DeviceEventController'
ATSPI_DEVICE_EVENT_LISTENER = 'org.freedesktop.atspi.DeviceEventListener'
ATSPI_REGISTRY = 'org.freedesktop.atspi.Registry'
ATSPI_TREE = 'org.freedesktop.atspi.Tree'

#------------------------------------------------------------------------------

class _CacheData(object):
	__slots__ =	[
			'parent',
			'interfaces',
			'children',
			'role',
			'name',
			'description',
			]

	def __init__(self, data):
		self.update(data)

	def update(self, data):
		#Don't cache the path here, used as lookup in cache object dict.
		(path,
		self.parent,
		self.children,
		self.interfaces,
		self.name,
		self.role,
		self.description) = data

#------------------------------------------------------------------------------

class ApplicationCache(object):
	"""
	Caches the bus names of all applications.

	Makes calls and recieves updates from the registry
	daemon to keep the cache up to date.
	"""

	_UPDATE_SIGNAL = 'updateApplications'
	_REGISTRY_PATH = '/org/freedesktop/atspi/registry'

	def __init__(self, connection, busName):
		"""
		Create a cache for application caches.
		BusName -> ApplicationCache
		"""
		self._connection = connection
		self._busName = busName

		registryObject = connection.get_object(busName, self._REGISTRY_PATH)
		registryInterface = dbus.Interface(registryObject, ATSPI_REGISTRY)

		self._applications = {}

		self._updateApplications(registryInterface.getApplications())

		#Connect to update signal
		self._signalMatch = self.registryInterface.connect_to_signal(self._UPDATE_SIGNAL,
									     self._updateHandler)

	def _updateApplications(self, names):
		"""
		Updates the application cache from an
		array of dbus bus names.
		"""
		for name in names:
			if name not in self._applications:
				self._applications[name] = AccessibleCache(self._connection,
									self._busName,
									self._treePath)

	def _removeApplications(self, names):
		"""
		Removes application caches given by names.
		"""
		for name in names:
			del(self._applications[name])

	def _updateHandler(self, updates):
		update, remove = updates
		self._removeApplications(update)
		self._updateApplications(remove)

#------------------------------------------------------------------------------

class AccessibleCache(object):
	"""
	Caches data for a collection of accessible object proxies.

	Makes calls and recieves updates from applications
	to update the cache of accessible object data.
	"""

	_UPDATE_SIGNAL = 'updateTree'
	_TREE_PATH = '/org/freedesktop/atspi/tree'


	def __init__(self, connection, busName):
		"""
		Creates a cache for accessible object data.

		All accessible object proxies are created and accessed through this cache.

		connection - DBus connection.
		busName - DBus bus name where accessible tree resides.
		"""
		self._connection = connection
		self._busName = busName

		treeObject = connection.get_object(busName, self._TREE_PATH)
		treeInterface = dbus.Interface(treeObject, ATSPI_TREE)

		self._objects = {}

		self._root = treeObject.getRoot()
		self._updateObjects(treeInterface.getTree())

		#Connect to update signal
		self._signalMatch = treeInterface.connect_to_signal(self._UPDATE_SIGNAL,
								    self._updateHandler)

	def getRootAccessible(self):
		"""
		Gets the accessible object at the root of the tree.
		"""
		return self.getAccessible(self._root)

	def getAccessible(self, path):
		"""
		Gets a D-Bus proxy object for the given object path.
		The object that is returned implements the accessible
		interface.

		path - The D-Bus path of the remote object.
		"""
		if path in self._objects:
                	proxy = self._connection.get_object(self._busName, path, introspect=False)
			return interfaceFactory(proxy, self, self._busName, path, ATSPI_ACCESSIBLE)
		else:
			raise AccessibleObjectNoLongerExists, "D-Bus reference not found in cache"

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
		update, remove = updates
		self._removeObjects(update)
		self._updateObjects(remove)

#END---------------------------------------------------------------------------
