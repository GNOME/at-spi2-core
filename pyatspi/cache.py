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

class BaseCache(object):
	"""
	Base object for the Desktop, Accessible and Application caches.

	Abstracts common initialization.
	"""


	def __init__(self, connection, bus_name):
		"""
		Creates a cache.

		connection - DBus connection.
		busName    - Name of DBus connection where cache interface resides.
		"""
		self._connection = connection
		self._bus_name = bus_name

		obj = connection.get_object(bus_name, self._PATH)
		itf = dbus.Interface(obj, self._INTERFACE)

		self._objects = {}

		getMethod = itf.get_dbus_method(self._GET_METHOD)
		self._updateObjects(getMethod)

		self._signalMatch = itf.connect_to_signal(self._UPDATE_SIGNAL, self._updateHandler)

	def _updateHandler(self, updates):
		update, remove = updates
		self._removeObjects(update)
		self._updateObjects(remove)

	def _updateObjects(self, objects):
		for data in objects:
			#First element is the object path.
			path = data[0]
			if path in self._objects:
				cachedata = self._objects[path]
				cachedata.update(data)
			else:
				self._objects[path] = _CacheData(data)

	def _removeObjects(self, paths):
		for path in paths:
			del(self._objects[path])


	def getAccessible(self, path, interface, dbus_object=None):
		"""
		Gets a client side proxy for the accessible object found
		at the path.

		path - The D-Bus path of the remote object.
		interface - The interface that the accessible object should support.
		dbus_object=None - The D-Bus proxy object backing this interface.
		"""
		if path in self._objects:
			if not dbus_object:
                		dbus_object = self._connection.get_object(self._bus_name, path, introspect=False)
			return interfaceFactory(proxy, self, path, interface)
		else:
			raise AccessibleObjectNoLongerExists, "D-Bus reference not found in cache"

#------------------------------------------------------------------------------

class AccessibleCache(BaseCache):
	"""
	There is one accessible cache per application.
	For each application the accessible cache stores
	data on every accessible object within the app.

	It also acts as the factory for creating client
	side proxies for these accessible objects.
	"""

	_PATH = '/org/freedesktop/atspi/tree'
	_INTERFACE = 'org.freedesktop.atspi.Tree'
	_GET_METHOD = 'getTree'
	_UPDATE_SIGNAL = 'updateTree'

	def __init__(self, connection, bus_name):
		BaseCache.__init__(self, connection, bus_name)

		obj = connection.get_object(_self.bus_name, self._PATH)
		itf = dbus.Interface(obj, self._INTERFACE)

		self._root = itf.getRoot()

	def getRootAccessible(self):
		"""
		Gets the accessible object at the root of the tree.
		"""
		return self.getAccessible(self._root)

#------------------------------------------------------------------------------

class DesktopCache(BaseCache):
	"""
	Cache of desktop objects obtained from the registry.

	The desktop interface on the registry object is the
	same as that of the general tree interface on the
	applications.

	The difference is that the children data refers to
	bus names of the applications rather than the object
	paths of particular accessible objects within an application.
	"""

	_PATH = '/org/freedesktop/atspi/registry'
	_INTERFACE = 'org.freedesktop.atspi.Registry'
	_GET_METHOD = 'getDesktops'
	_UPDATE_SIGNAL = 'updateDesktops'
	
	def __init__(self, connection, bus_name):
		self._app_cache = ApplicationCache(connection, bus_name)

	def getApplication(self, name):
		try:
			self._app_cache[name].getRootAccessible()
		except KeyError:
			raise AccessibleObjectNoLongerExists("Application no longer exists")

#------------------------------------------------------------------------------

class ApplicationCache(object):
	"""
	Holds a mapping of bus names of each accessible application
	to the applications accessible cache.

	Makes calls and recieves updates from the registry
	daemon to keep the cache up to date.
	"""

	_PATH = '/org/freedesktop/atspi/registry'
	_INTERFACE = 'org.freedesktop.atspi.Registry'
	_GET_METHOD = 'getApplications'
	_UPDATE_SIGNAL = 'updateApplications'

	def _updateApplications(self, names):
		for name in names:
			if name not in self._applications:
				self._applications[name] = AccessibleCache(self._connection,
									self._busName,
									self._treePath)

	def _removeApplications(self, names):
		for name in names:
			del(self._applications[name])

#END---------------------------------------------------------------------------
