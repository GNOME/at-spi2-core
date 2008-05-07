import dbus
import dbus.service

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
		self._toRemove = []
		self._objects = {}
		self._root = '/'

	@dbus.service.method(dbus_interface='org.freedesktop.atspi.Tree',
			     out_signature='o')
	def getRoot(self):
		return self._root

	@dbus.service.method(dbus_interface='org.freedesktop.atspi.Tree',
			     out_signature='a(ooaoassus)')
	def getTree(self):
		return self._objects.values()

	@dbus.service.signal(dbus_interface='org.freedesktop.atspi.Tree',
			     signature=('a(ooaoassus)ao'))
	def updateTree(self, objects, paths):
		#There are some locking issues here.
		#Need to make sure that updates are not missed.
		oldSend = self._toSend.values()
		self._toSend = {}
		oldRemove = self._toRemove
		self._toRemove = []
		return (oldSend, oldRemove)

	def updateObject(self, path, object):
		self._objects[path] = object
		self._toSend[path] = object
		
	def removeObject(self, path):
		if path in self._objects:
			self._toRemove.append(path)
			del(self._objects[path])

	def setRoot(self, root):
		self._root = root
