import sys
import gobject
import dbus
from dbus.mainloop.glib import DBusGMainLoop

from xml.dom import minidom

from AccessibleTreeCache import AccessibleTreeCache, AccessibleObjectProxy

DBusGMainLoop(set_as_default=True)

def createNode(accessible, parentRef, parentElement):
	e = minidom.Element("accessible")

	e.attributes["reference"] = accessible.path
	e.attributes["parent"] = accessible.parent
	e.attributes["name"] = accessible.name
	e.attributes["role"] = str(int(accessible.role))
	e.attributes["description"] = accessible.description

	for i in accessible.interfaces:
		itf = minidom.Element("interface")
		itf.attributes["name"] = i
		e.appendChild(itf)

	for c in accessible.getChildren():
		createNode(c, accessible.path, e)

	parentElement.appendChild(e)

def main(argv):
	filename = argv[1]
	bus = dbus.SessionBus()
	
	loop = gobject.MainLoop()

	cache = AccessibleTreeCache(bus, 'test.atspi.tree', '/org/freedesktop/atspi/tree')
	root = cache.getRootAccessible()

	doc = minidom.Document()
	createNode(root, '/', doc)
	
	file = open(filename, 'w')
	file.write(doc.toprettyxml())
	file.close()

if __name__ == '__main__':
	sys.exit(main(sys.argv))
