import testutil

import dbus
import gobject
import os.path
import coretest
from dbus.mainloop.glib import DBusGMainLoop

from accessible_cache import AccessibleCache
from accessible_cache import ATSPI_COMPONENT

from xml.dom import minidom

ATSPI_LAYER_WIDGET = 3
ATSPI_LAYER_MDI = 4
ATSPI_LAYER_WINDOW = 7

extents_expected = [(0,0,30,20), (40,30,30,40), (0,0,70,70)]
layers_expected = [ATSPI_LAYER_WINDOW, ATSPI_LAYER_WIDGET, ATSPI_LAYER_MDI]
zorders_expected = [0, -100, 100]

def supportsInterface(accessible, interface):
	for itf in accessible.interfaces:
		if itf == interface:
			return True
	return False

class ComponentTestCase(coretest.CoreTestCase):
	def runTest(self):
		self._app = testutil.runTestApp("libcomponentapp.so", self._name)
		self._loop.run()

	def post_application_test(self):
		#----------------------------------------
		comps = [None, None, None]
		comps[2] = self._cache.getRootAccessible()

		self.assertEqual(comps[2].numChildren, 2,
		"""
		Number of child components = %d
		Correct number of components = 2
		""" % comps[2].numChildren)
		#----------------------------------------
		comps[0] = comps[2].getChild(0)
		comps[1] = comps[2].getChild(1)

		for comp in comps:
			self.assert_(supportsInterface(comp, ATSPI_COMPONENT),
				"""
				An accessible object provided does not support the
				component interface.
				""")
		#----------------------------------------
		for (expected, comp) in zip(extents_expected, comps):
			extents = comp.getExtents(dbus.types.UInt32(0))
			self.assertEquals(extents, expected,
					"""
					Extents of component do not match.
					Expected: %s
					Recieved: %s
					""" % (str(expected), str(extents)))
		#----------------------------------------
		for (expected, comp) in zip(layers_expected, comps):
			layer = comp.getLayer()
			self.assertEquals(layer, expected,
					"""
					Layer of component does not match.
					Expected: %s
					Recieved: %s
					""" % (str(expected), str(layer)))
		#----------------------------------------
		#There is no defined value for the result when the layer is not WINDOW or MDI
		#for (expected, comp) in zip(zorders_expected, [comps[0], comps[2]]):
		#	zorder = comp.getMDIZOrder()
		#	print zorder, expected
		#	self.assertEquals(layer, expected,
		#			"""
		#			ZOrder of component does not match.
		#			Expected: %s
		#			Recieved: %s
		#			""" % (str(expected), str(zorder)))
		#----------------------------------------
