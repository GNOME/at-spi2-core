import testutil

import dbus
import gobject
import os.path
import coretest 
from dbus.mainloop.glib import DBusGMainLoop

from accessible_cache import AccessibleCache

from xml.dom import minidom
import os

def createNode(accessible, parentElement):
	e = minidom.Element("accessible")

	e.attributes["name"] = accessible.name
	e.attributes["role"] = str(int(accessible.role))
	e.attributes["description"] = accessible.description

	for i in range(0, accessible.numChildren):
		createNode(accessible.getChild(i), e)

	parentElement.appendChild(e)

class AccessibleTestCase(coretest.CoreTestCase):
	def runTest(self):
		self._app = testutil.runTestApp("libobjectapp.so", self._name)
		self._loop.run()

	def post_application_test(self):
		root = self._cache.getRootAccessible()

		doc = minidom.Document()
		createNode(root, doc)
		answer = doc.toprettyxml()

		correct = os.path.join(os.environ["TEST_DATA_DIRECTORY"],
					"object-test-stage1-results.xml")
		file = open(correct)
		cstring = file.read()
		
		#import difflib
		#print ''.join(difflib.unified_diff(answer.splitlines(), cstring.splitlines()))
		
		self.assertEqual(answer, cstring, "Object tree not passed correctly")
