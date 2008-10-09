import dbus
import gobject
import os.path

from xml.dom import minidom
import os

from pasytest import PasyTest as _PasyTest

import pyatspi

class DesktopTest(_PasyTest):

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
		     "test_contains",
		     "test_getAccessibleAtPoint",
		     "test_getExtents",
		     "test_getPosition",
		     "test_getSize",
		     "test_getLayer",
		     "test_getMDIZOrder",
		     "test_grabFocus",
		     "test_registerFocusHandler",
		     "test_deregisterFocusHandler",
		     "test_getAlpha",
		     "teardown",
		     ]

	def __init__(self, bus, path):
		_PasyTest.__init__(self, "Desktop", False)
		self._bus = bus
		self._path = path

	def setup(self, test):
		self._registry = pyatspi.Registry()
		self._desktop = self._registry.getDesktop(0)

	def test_name(self, test):
		name = self._desktop.name
		test.assertEqual(name, "main", "Expected name - \"main\". Recieved - \"%s\"" % (name,))

	def test_getChildAtIndex(self, test):
		root = self._desktop.getChildAtIndex(0)
		test.assertEqual(root.name, "main",
					 "Expected name - \"main\". Recieved - \"%s\"" % (root.name,))
		
	def test_isEqual(self, test):
		if not self._desktop.isEqual(self._desktop):
			test.fail("Same accessible found unequal to self")

		root = self._desktop.getChildAtIndex(0)
		if root.isEqual(self._desktop):
			test.fail("Different accessibles found equal")
		if self._desktop.isEqual(root):
			test.fail("Different accessibles found equal")

	def test_getApplication(self, test):
		test.assertEqual(self._desktop.getApplication(), None,
				"Expected application - \"None\". Recieved - \"%s\"" % (self._desktop.getApplication(),))

	def test_getAttributes(self, test):
		test.assertEqual(self._desktop.getAttributes(), [],
				"Expected attributes - \"[]\". Recieved - \"%s\"" % (self._desktop.getAttributes(),))

	def test_parent(self, test):
		test.assertEqual(self._desktop.parent, None,
				"Expected parent - \"None\". Recieved - \"%s\"" % (self._desktop.parent,))

	def test_getIndexInParent(self, test):
		test.assertEqual(self._desktop.getIndexInParent(), -1,
				"Expected index - \"-1\". Recieved - \"%d\"" % (self._desktop.getIndexInParent(),))

	def test_getLocalizedRoleName(self, test):
		ans = "unknown"
		res = self._desktop.getLocalizedRoleName()
		test.assertEqual(ans, res,
				 "Expected LocalizedRoleName - \"%s\". Recieved - \"%s\"" % (ans, res,))

	def test_getRelationSet(self, test):
		rset = self._desktop.getRelationSet()
		test.assertEqual(rset, [],
				 "Expected relation set - \"[]\". Recieved - \"%s\"" % (rset,))

	def test_getRole(self, test):
		test.assertEqual(self._desktop.getRole(), pyatspi.ROLE_UNKNOWN,
				 "Expected role - \"ROLE_UNKNOWN\". Recieved - \"%s\"" % (self._desktop.getRole(),))

	def test_getRoleName(self, test):
		ans = "unknown"
		res = self._desktop.getRoleName()
		test.assertEqual(ans, res,
				 "Expected RoleName - \"%s\". Recieved - \"%s\"" % (ans, res,))

	def test_getState(self, test):
		state = self._desktop.getState()
		res = pyatspi.StateSet()
		if not state.equals(res):
			test.fail("Desktop state set is not empty")

	def test_childCount(self, test):
		test.assertEqual(self._desktop.childCount, 1,
				 "Expected childCount - \"1\". Recieved - \"%d\"" % (self._desktop.childCount,))

	def test_description(self, test):
		description = ""
		test.assertEqual(self._desktop.description, description,
			"Expected description - \"%s\". Recieved - \"%s\"" % (description, self._desktop.description,))

	def test_contains(self, test):
		pass

	def test_getAccessibleAtPoint(self, test):
		pass

	def test_getExtents(self, test):
		comp = self._desktop.queryComponent()
 
		extents = comp.getExtents(0)
		expected = pyatspi.BoundingBox(*(0,0,1024, 768))
		test.assertEqual(extents, expected, 
				 "Extents not correct. Expected (%d, %d, %d, %d), Recieved (%d, %d, %d, %d)"
				 % (expected[0], expected[1], expected[2], expected[3], 
					extents[0], extents[1], extents[2], extents[3]))

	def test_getPosition(self, test):
		comp = self._desktop.queryComponent()

		position = comp.getPosition(0)
		test.assertEqual(position, (0,0), 
				 "Position not correct. Expected (%d, %d) Recieved (%d, %d)"
				 % (0, 0, position[0], position[1]))

	def test_getSize(self, test):
		comp = self._desktop.queryComponent()
 
		size = comp.getSize()
		test.assertEqual(size, (1024, 768), 
				 "Size not correct. Expected (%d, %d) Recieved (%d, %d)"
				 % (1024, 768, size[0], size[1]))

	def test_getLayer(self, test):
		comp = self._desktop.queryComponent()
 
		layer = comp.getLayer()
		test.assertEqual(layer, pyatspi.LAYER_WIDGET, 
				 "Layer not correct. Expected %d, Recieved %d"
				 % (layer, pyatspi.LAYER_WIDGET))

	def test_getMDIZOrder(self, test):
		comp = self._desktop.queryComponent()
 
		mdizo = comp.getMDIZOrder()
		test.assertEqual(mdizo, 0, 
				 "ZOrder not correct. Expected %d, Recieved %d"
				 % (0, mdizo))

	def test_grabFocus(self, test):
		pass

	def test_registerFocusHandler(self, test):
		pass

	def test_deregisterFocusHandler(self, test):
		pass

	def test_getAlpha(self, test):
		pass

	def teardown(self, test):
		pass
