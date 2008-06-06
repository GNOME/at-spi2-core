import unittest
import testutil

import dbus
import gobject
import os.path
from dbus.mainloop.glib import DBusGMainLoop

from accessible_cache import AccessibleCache

from xml.dom import minidom

def createNode(accessible, parentElement):
	e = minidom.Element("accessible")

	e.attributes["name"] = accessible.name
	e.attributes["role"] = str(int(accessible.role))
	e.attributes["description"] = accessible.description

	for i in range(0, accessible.numChildren):
		createNode(accessible.getChild(i), e)

	parentElement.appendChild(e)

class AccessibleObjectTestCase(unittest.TestCase):
	def setUp(self):
		DBusGMainLoop(set_as_default=True)
		self._app = testutil.runTestApp("libobjectapp.so")

		self._bus = dbus.SessionBus()
		self._loop = gobject.MainLoop()
		self._cache = AccessibleCache(self._bus, testutil.busname, testutil.objectpath)

	def tearDown(self):
		del(self._bus)
		del(self._loop)
		del(self._cache)
		#TODO Shut down the test application.
		del(self._app)

	def runTest(self):
		root = self._cache.getRootAccessible()

		doc = minidom.Document()
		createNode(root, doc)
		answer = doc.toprettyxml()

		correct = os.path.join(testutil.testdata, "object-test-stage1-results.xml")
		file = open(correct)
		cstring = file.read()
		
		#import difflib
		#print ''.join(difflib.unified_diff(answer.splitlines(), cstring.splitlines()))
		
		self.assertEqual(answer, cstring, "Object tree not passed correctly")
