import dbus

class AccessibleObjectDoesNotExist(Exception):
	def __init__(self, path):
		self.path = path
	
	def __str__(self):
		return "Object %s does not exist" % (self.path)

class AccessibleTreeCache():
	"""
	Caches a collection of Accessible Objects.
	"""

	_TREE_INTERFACE = 'org.freedesktop.atspi.Tree'
	_UPDATE_SIGNAL = 'updateTree'

	def __init__(self, connection, busName, objectStorePath):
		"""
		Creates a cache for accessible objects.

		All accessible objects are created and accessed through this cache.

		Parameters:
		connection - DBus connection.
		busName - DBus bus name where accessible tree resides.
		objectStorePath - Path where the accessible tree can be accessed.
		"""
		storeObject = connection.get_object(busName, objectStorePath)

		self._busName = busName
		self._accessibleStore = dbus.Interface(storeObject, self._TREE_INTERFACE)
		self._objects = {}
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

	def getAccessible(self, objectPath):
		"""
		Gets the accessible object for the given object path.
		"""
		if objectPath in self._objects:
			return self._objects[objectPath]
		else:
			raise AccessibleObjectDoesNotExist(objectPath)

	def _updateObjects(self, objects):
		"""
		Updates the object cache from an
		array of wire format Accessible objects.
		"""
		for object in objects:
			(path,
			 parent,
			 children,
			 interfaces,
			 name, 
			 role,
			 description) = object
			if path in self._objects:
				self._objects[path].update(path,
							   parent,
							   children, 
							   interfaces, 
							   name, 
							   role, 
							   description)
			else:
				acc = AccessibleObjectProxy(self,
							    self._busName,
							    path,
							    parent,
							    children,
							    interfaces,
							    name,
							    role,
							    description)
				self._objects[path] = acc;

	def _removeObjects(self, paths):
		for path in paths:
			#Probably want to set object as invalid first
			del(self._objects[path])

	def _updateHandler(self, updates):
		objects, paths = updates
		self._removeObjects(paths)
		self._updateObjects(objects)

class AccessibleObjectProxy():
	"""
	A D-Bus proxy for an element that implements one or more of the AT-SPI
	Accessibility interfaces.
	"""

	def __init__(self, cache, bus,
		     path, parent,
		     children, interfaces, 
		     name, role, description):
		"""
		Create an accessible object.

		Parameters:

		cache - The accessible cache that this object is owned by.
		bus - Bus name where proxied object can be located.
		path - The D-Bus object path of object proxied by this object.
		parent - The parent accessible or '/' if the root object.
		children - List of child accessible objects.
		interfaces - List of interfaces supported by this object.
		name - Name of the accessible.
		role - The accessibles role.
		description - Description of the accessible
		"""
		self._cache = cache
		self.bus = bus
		self.update(path, parent, children, interfaces, name, role, description)

	def update(self, path, parent,
		   children, interfaces, 
		   name, role, description):
		"""
		update an accessible object.

		Parameters:

		path - The D-Bus object path of object proxied by this object.
		parent - The parent accessible or '/' if the root object.
		children - List of child accessible objects.
		interfaces - List of interfaces supported by this object.
		name - Name of the accessible.
		role - The accessibles role.
		description - Description of the accessible
		"""
		self.path = path
		self.parent = parent
		self.children = children
		self.interfaces = interfaces
		self.name = name
		self.role = role
		self.description = description

	def getChildren(self):
		return [self._cache.getAccessible(path) for path in self.children]
