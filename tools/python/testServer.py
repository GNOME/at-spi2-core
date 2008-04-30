import sys
from dbus.mainloop.glib import DBusGMainLoop

import gobject
import dbus

from xml.dom import minidom

from AccessibleTree import AccessibleTree

DBusGMainLoop(set_as_default=True)

def getChildrenByName(node, name):
	return [child for child in node.childNodes if child.nodeType == child.ELEMENT_NODE and child.nodeName == name]

def registerObject(tree, parent, node):
	reference = node.getAttribute('reference').encode('ASCII')
	name = node.getAttribute('name').encode('ASCII')
	role = node.getAttribute('role').encode('ASCII')
	description = node.getAttribute('description').encode('ASCII')

	interfacen = getChildrenByName(node, 'interface')
	interfaces = [itf.getAttribute('name').encode('ASCII') for itf in interfacen]

	childrenn = getChildrenByName(node, 'accessible')
	children = [cld.getAttribute('reference').encode('ASCII') for cld in childrenn]

	object = (reference, parent, children, interfaces, name, int(role), description)
	tree.updateObject(reference, object)

	for child in childrenn:
		registerObject(tree, reference, child)

def main(argv):
	filename = argv[1]
	bus = dbus.SessionBus()

	loop = gobject.MainLoop()

	bus.request_name('test.atspi.tree', 0)

	tree = AccessibleTree(bus, '/org/freedesktop/atspi/tree')
	doc = minidom.parse(filename)
	node = doc.firstChild
	tree.setRoot(node.getAttribute('reference').encode('ASCII'))

	registerObject(tree, '/', node)

	try:
		loop.run()
	except KeyboardInterrupt:
		loop.quit()

if __name__ == '__main__':
	sys.exit(main(sys.argv))
