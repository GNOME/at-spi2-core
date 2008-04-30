import dbus
import dbus.service

TREE_UPDATE_ACCESSIBLE = 0
TREE_REMOVE_ACCESSIBLE = 1

class AccessibleTree(dbus.service.Object):
	"""
	The Accessible tree provides the interface,
	for accessing all the accessible objects
	available on a particular application.
	"""

	def __init__(self, bus, path):
		"""
		Parameters:

		bus - The D-Bus bus object to use
		path - The object path this interface should use
		"""
		dbus.service.Object.__init__(self, bus, path)
		self._toSend = {}
		self._objects = {}
		self._root = '/'

	@dbus.service.method(dbus_interface='org.freedesktop.atspi.Tree',
			     out_signature='o')
	def getRoot(self):
		return self._root

	@dbus.service.method(dbus_interface='org.freedesktop.atspi.Tree',
			     out_signature='a(qooaoassus)')
	def getTree(self):
		wireObjects = []
		for object in self._objects.values():
			wireObjects.append((TREE_UPDATE_ACCESSIBLE,) + object)
		return wireObjects

	@dbus.service.signal(dbus_interface='org.freedesktop.atspi.Tree',
			     signature='a(qooaoassus)')
	def updateTree(self, values):
		#There are some locking issues here.
		#Need to make sure that updates are not missed.
		oldSend = self._toSend.values()
		self._toSend = {}
		return oldSend

	def updateObject(self, path, object):
		self._objects[path] = object
		self._toSend[path] = (TREE_UPDATE_ACCESSIBLE,) + object
		
	def removeObject(self, path):
		self._toSend[path] = (TREE_REMOVE_ACCESSIBLE,) + self._objects[path]
		del(self._objects[path])

	def setRoot(self, root):
		self._root = root
