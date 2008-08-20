import dbus
import gobject
import os.path

from xml.dom import minidom
import os

from pasytest import PasyTest as _PasyTest

import pyatspi
from pyatspi import Accessible

ATSPI_LAYER_WIDGET = 3
ATSPI_LAYER_MDI = 4
ATSPI_LAYER_WINDOW = 7

extents_expected = [(0,0,30,20), (40,30,30,40), (0,0,70,70)]
sizes_expected = [(30,20), (30,40), (70,70)]
positions_expected = [(0,0), (40,30), (0,0)]
layers_expected = [ATSPI_LAYER_WINDOW, ATSPI_LAYER_WIDGET, ATSPI_LAYER_MDI]
zorders_expected = [-100, 100]

class ComponentTest(_PasyTest):

	__tests__ = ["setup",
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
		_PasyTest.__init__(self, "Accessible", False)
		self._bus = bus
		self._path = path

	def setup(self, test):
		self._cache = pyatspi.TestApplicationCache(self._bus, self._path)

	def test_contains(self, test):
		pass

	def test_getAccessibleAtPoint(self, test):
		pass

	def test_getExtents(self, test):
		root = self._cache.root
		one = root.getChildAtIndex(0)
		two = root.getChildAtIndex(1)

		comps = [one.queryComponent(),
			 two.queryComponent(),
			 root.queryComponent(),]
 
		for expected, comp in zip(extents_expected, comps):
			extents = comp.getExtents(0)
			test.assertEqual(extents, expected, 
					 "Extents not correct. Expected (%d, %d, %d, %d), Recieved (%d, %d, %d, %d)"
					 % (expected[0], expected[1], expected[2], expected[3], 
						extents[0], extents[1], extents[2], extents[3]))

	def test_getPosition(self, test):
		pass
		root = self._cache.root
		one = root.getChildAtIndex(0)
		two = root.getChildAtIndex(1)

		comps = [one.queryComponent(),
			 two.queryComponent(),
			 root.queryComponent(),]
 
		for expected, comp in zip(positions_expected, comps):
			position = comp.getPosition(0)
			test.assertEqual(position, expected, 
					 "Position not correct. Expected (%d, %d) Recieved (%d, %d)"
					 % (expected[0], expected[1], position[0], position[1]))

	def test_getSize(self, test):
		root = self._cache.root
		one = root.getChildAtIndex(0)
		two = root.getChildAtIndex(1)

		comps = [one.queryComponent(),
			 two.queryComponent(),
			 root.queryComponent(),]
 
		for expected, comp in zip(sizes_expected, comps):
			size = comp.getSize()
			test.assertEqual(size, expected, 
					 "Size not correct. Expected (%d, %d) Recieved (%d, %d)"
					 % (expected[0], expected[1], size[0], size[1]))

	def test_getLayer(self, test):
		root = self._cache.root
		one = root.getChildAtIndex(0)
		two = root.getChildAtIndex(1)

		comps = [one.queryComponent(),
			 two.queryComponent(),
			 root.queryComponent(),]
 
		for expected, comp in zip(layers_expected, comps):
			layer = comp.getLayer()
			test.assertEqual(layer, expected, 
					 "Layer not correct. Expected %d, Recieved %d"
					 % (layer, expected))

	def test_getMDIZOrder(self, test):
		root = self._cache.root
		one = root.getChildAtIndex(0)
		two = root.getChildAtIndex(1)

		comps = [two.queryComponent(),
			 root.queryComponent(),]
 
		for expected, comp in zip(zorders_expected, comps):
			mdizo = comp.getMDIZOrder()
			test.assertEqual(mdizo, expected, 
					 "ZOrder not correct. Expected %d, Recieved %d"
					 % (expected, mdizo))

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
