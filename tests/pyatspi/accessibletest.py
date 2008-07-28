import dbus
import gobject
import os.path

from xml.dom import minidom
import os

from pasytest import PasyTestSuite

def createNode(accessible, parentElement):
	e = minidom.Element("accessible")

	e.attributes["name"] = accessible.name
	e.attributes["role"] = str(int(accessible.role))
	e.attributes["description"] = accessible.description

	for i in range(0, accessible.numChildren):
		createNode(accessible.getChild(i), e)

	parentElement.appendChild(e)

class TreeTestSuite(PasyTestSuite):

	__tests__ = ["accessibleTree"]

	def __init__(self, bus, name):
		PasyTestSuite.__init__(self, "Tree")
		self._cache = getAccessibleCache(bus, name)

	def accessibleTree(test):
		root = self._cache.getRootAccessible()

		doc = minidom.Document()
		createNode(root, doc)
		answer = doc.toprettyxml()

		correct = os.path.join(os.environ["TEST_DATA_DIRECTORY"],
					"object-test-stage1-results.xml")
		file = open(correct)
		cstring = file.read()
		
		test.assertEqual(answer, cstring, "Object tree not passed correctly")

		test.win()
