import dbus
import gobject
import os.path

from xml.dom import minidom
import os

from pasytest import PasyTest as _PasyTest

import pyatspi

class ActionTest(_PasyTest):

	__tests__ = ["setup",
		     "test_nActions",
		     "test_getDescription",
		     "test_getName",
		     "test_doAction",
		     "test_getKeyBinding",
		     "teardown",
		     ]

	def __init__(self, bus, path):
		_PasyTest.__init__(self, "Action", False)
		self._bus = bus
		self._path = path

	def setup(self, test):
		self._registry = pyatspi.Registry()
		self._desktop = self._registry.getDesktop(0)

	def test_nActions(self, test):
		root = self._desktop[0]
		root = root.queryAction()
		nact = root.nActions
		test.assertEqual(nact, 10, "nActions expected %d, recieved %d" % (10, nact))

	def test_getName(self, test):
		root = self._desktop[0]
		root = root.queryAction()
		name = root.getName(0)
		test.assertEqual(name, "First action", "Name expected %s, recieved %s" % ("First action", name))
		name = root.getName(1)
		test.assertEqual(name, "Action", "Name expected %s, recieved %s" % ("Action", name))

	def test_getDescription(self, test):
		root = self._desktop[0]
		root = root.queryAction()
		description = root.getDescription(0)
		expected = "First action performed"
		test.assertEqual(description, expected, "Description expected %s, recieved %s" % (expected, description))
		description = root.getDescription(1)
		expected = "Description of action"
		test.assertEqual(description, expected, "Description expected %s, recieved %s" % (expected, description))

	def test_doAction(self, test):
		root = self._desktop[0]
		root = root.queryAction()
		#TODO have event emitted to check action has been performed
		for i in range(0, root.nActions):
			root.doAction(i)

	def test_getKeyBinding(self, test):
		root = self._desktop[0]
		root = root.queryAction()
		for i in range(0, root.nActions):
			keybinding = root.getKeyBinding(i)
			expected = "%s" % (i,)
			test.assertEqual(keybinding, expected,
					 "Keybinding expected %s, recieved %s" % (expected, keybinding))

	def teardown(self, test):
		pass
