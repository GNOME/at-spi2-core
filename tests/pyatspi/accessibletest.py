import dbus
import gobject
import os.path

from xml.dom import minidom
import os

from pasytest import PasyTest as _PasyTest

import pyatspi

def _createNode(accessible, parentElement):
	e = minidom.Element("accessible")

	e.attributes["name"] = accessible.name
	e.attributes["role"] = str(int(accessible.getRole()))
	e.attributes["description"] = accessible.description

	for i in range(0, accessible.childCount):
		_createNode(accessible.getChildAtIndex(i), e)

	parentElement.appendChild(e)

class AccessibleTest(_PasyTest):

	__tests__ = ["setup", "tree"]

	def __init__(self, bus, path):
		_PasyTest.__init__(self, "Accessible", False)
		self._bus = bus
		self._path = path

	def setup(self):
		self._cache = pyatspi.TestApplicationCache(self._bus, self._path)

	def tree(self):
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
		root = self._cache.root

		doc = minidom.Document()
		_createNode(root, doc)
		answer = doc.toprettyxml()

		correct = os.path.join(os.environ["TEST_DATA_DIRECTORY"],
					"object-test-stage1-results.xml")
		file = open(correct)
		cstring = file.read()
		
		self.assertEqual(answer, cstring, "Object tree not passed correctly")
