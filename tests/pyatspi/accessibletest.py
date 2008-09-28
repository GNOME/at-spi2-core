import dbus
import gobject
import os.path

from xml.dom import minidom
import os

from pasytest import PasyTest as _PasyTest

import pyatspi
from pyatspi import StateSet

st = [pyatspi.STATE_MULTI_LINE,
      pyatspi.STATE_MODAL,
      pyatspi.STATE_INDETERMINATE,
      pyatspi.STATE_SUPPORTS_AUTOCOMPLETION,
      pyatspi.STATE_VERTICAL,]

def _createNode(accessible, parentElement):
	e = minidom.Element("accessible")

	e.attributes["name"] = accessible.name
	e.attributes["role"] = str(int(accessible.getRole()))
	e.attributes["description"] = accessible.description

	for i in range(0, accessible.childCount):
		_createNode(accessible.getChildAtIndex(i), e)

	parentElement.appendChild(e)

class AccessibleTest(_PasyTest):

	__tests__ = ["setup",
		     "test_name",
		     "test_getChildAtIndex",
		     "test_isEqual",
		     "test_getApplication",
		     "test_getAttributes",
		     "test_parent",
		     "test_getIndexInParent",
		     "test_getLocalizedRoleName",
		     "test_getRelationSet",
		     "test_getRole",
		     "test_getRoleName",
		     "test_getState",
		     "test_childCount",
		     "test_description",
		     "test_tree",
		     "teardown",
		     ]

	def __init__(self, bus, path):
		_PasyTest.__init__(self, "Accessible", False)
		self._bus = bus
		self._path = path

	def setup(self, test):
		self._registry = pyatspi.registry.Registry(self._path)
		self._desktop = self._registry.getDesktop(0)

	def test_name(self, test):
		root = self._desktop.getChildAtIndex(0)
		test.assertEqual(root.name, "main", "Expected name - \"main\". Recieved - \"%s\"" % (root.name,))

	def test_getChildAtIndex(self, test):
		root = self._desktop.getChildAtIndex(0)
		a = root.getChildAtIndex(0)
		test.assertEqual(a.name, "gnome-settings-daemon",
					 "Expected name - \"gnome-settings-daemon\". Recieved - \"%s\"" % (a.name,))
		b = root.getChildAtIndex(1)
		test.assertEqual(b.name, "gnome-panel",
					 "Expected name - \"gnome-panel\". Recieved - \"%s\"" % (b.name,))
		c = root.getChildAtIndex(2)
		test.assertEqual(c.name, "nautilus",
					 "Expected name - \"nautilus\". Recieved - \"%s\"" % (c.name,))
		
	def test_isEqual(self, test):
		root = self._desktop.getChildAtIndex(0)

		a = root.getChildAtIndex(1)
		if not a.isEqual(a):
			test.fail("Same accessible found unequal to self")

		b = root.getChildAtIndex(1)
		if not a.isEqual(b):
			test.fail("Similar accessibles found unequal")
		if not b.isEqual(a):
			test.fail("Similar accessibles found unequal")

		c = root.getChildAtIndex(2)
		if c.isEqual(a):
			test.fail("Different accessibles found equal")
		if a.isEqual(c):
			test.fail("Different accessibles found equal")

	def test_getApplication(self, test):
		root = self._desktop.getChildAtIndex(0)
		application = root.getApplication()
		if not root.isEqual(application):
			test.fail("Root accessible does not provide itself as its Application")

		a = root.getChildAtIndex(1)
		application = a.getApplication()
		if not root.isEqual(application):
			test.fail("Child accessible does not provide the root as its Application")


	def test_getAttributes(self, test):
		root = self._desktop.getChildAtIndex(0)
		attr = root.getAttributes()
		res = ["foo:bar", "baz:qux", "quux:corge"]
		test.assertEqual(attr, res, "Attributes expected %s, recieved %s" % (attr, res))


	def test_parent(self, test):
		root = self._desktop.getChildAtIndex(0)

		a = root.getChildAtIndex(1)
		pa = a.parent
		if not root.isEqual(pa):
			test.fail("Child does not correctly report its parent")

	def test_getIndexInParent(self, test):
		root = self._desktop.getChildAtIndex(0)

		for i in range(0, root.childCount):
			child = root.getChildAtIndex(i)
			test.assertEqual(i, child.getIndexInParent(), "Childs index in parent reported incorrectly")

	def test_getLocalizedRoleName(self, test):
		root = self._desktop.getChildAtIndex(0)

		ans = "window"
		res = root.getLocalizedRoleName()
		test.assertEqual(ans, res,
				 "Expected LocalizedRoleName - \"%s\". Recieved - \"%s\"" % (ans, res,))

		a = root.getChildAtIndex(1)
		a = a.getChildAtIndex(0)
		ans = "html container"
		res = a.getLocalizedRoleName()
		test.assertEqual(ans, res,
				 "Expected LocalizedRoleName - \"%s\". Recieved - \"%s\"" % (ans, res,))

	def test_getRelationSet(self, test):
		root = self._desktop.getChildAtIndex(0)
		# Complete test of Relation interface is separate
		rset = root.getRelationSet()

	def test_getRole(self, test):
		root = self._desktop.getChildAtIndex(0)
		test.assertEqual(root.getRole(), 69,
				 "Expected role - \"69\". Recieved - \"%d\"" % (root.getRole(),))

	def test_getRoleName(self, test):
		root = self._desktop.getChildAtIndex(0)

		ans = "window"
		res = root.getRoleName()
		test.assertEqual(ans, res,
				 "Expected roleName - \"%s\". Recieved - \"%s\"" % (ans, res,))

		a = root.getChildAtIndex(1)
		a = a.getChildAtIndex(0)
		ans = "html container"
		res = a.getRoleName()
		test.assertEqual(ans, res,
				 "Expected roleName - \"%s\". Recieved - \"%s\"" % (ans, res,))

	def test_getState(self, test):
		root = self._desktop.getChildAtIndex(0)
		state = root.getState()
		res = StateSet(*st)
		if not res.equals(state):
			test.fail("States not reported correctly")

	def test_childCount(self, test):
		root = self._desktop.getChildAtIndex(0)
		test.assertEqual(root.childCount, 11,
				 "Expected role - \"11\". Recieved - \"%d\"" % (root.childCount,))

	def test_description(self, test):
		root = self._desktop.getChildAtIndex(0)
		description = "The main accessible object, root of the accessible tree"
		test.assertEqual(root.description, description,
				 "Expected description - \"%s\". Recieved - \"%s\"" % (description, root.description,))

	def test_tree(self, test):
		"""
		This is a mild stress test for the 
		methods:

		getChildAtIndex
		
		And the attributes:

		name
		description

		It checks a tree of these values is correctly
		passed from Application to AT.
		"""
		root = self._desktop.getChildAtIndex(0)

		doc = minidom.Document()
		_createNode(root, doc)
		answer = doc.toprettyxml()

		correct = os.path.join(os.environ["TEST_DATA_DIRECTORY"],
					"accessible-test-results.xml")
		file = open(correct)
		cstring = file.read()
		
		test.assertEqual(answer, cstring, "Object tree not passed correctly")

	def teardown(self, test):
		pass
