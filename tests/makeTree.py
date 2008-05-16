import sys
import pyatspi
import uuid

from xml.dom import minidom

INTERFACES = [
"Accessible",
"Desktop",
"Image",
"StreamableContent",
"Action",
"Document",
"Table",
"Application",
"EditableText",
"MatchRule",
"Text",
"Collection",
"Hyperlink",
"Value",
"Component",
"Hypertext",
"Selection",
]

def getChild(accessible, i):
	try:
		child = accessible.getChildAtIndex(i)
	except LookupError:
		child = None
	return child

def createNode(accessible, parentRef, parentElement):
	e = minidom.Element("accessible")
	reference = '/' + str(uuid.uuid4()).replace('-', '')

	e.attributes["reference"] = reference
	e.attributes["parent"] = parentRef
	e.attributes["name"] = accessible.name
	e.attributes["role"] = str(accessible.getRole())
	e.attributes["description"] = accessible.description

	for i in INTERFACES:
		query = getattr(accessible, "query" + i)
		try:
			query()
			itf = minidom.Element("interface")
			itf.attributes["name"] = i
			e.appendChild(itf)
		except NotImplementedError:
			pass
		except LookupError:
			pass

	try:
		count = accessible.childCount
	except LookupError:
		count = 0
	
	for i in range(count):
		child = getChild(accessible, i)
		if child is not None:
			createNode(child, reference, e)

	parentElement.appendChild(e)

def main(argv):
	filename = argv[1]
	doc = minidom.Document()
	desk = pyatspi.Registry.getDesktop(0)
	createNode(desk, '/', doc)
	
	file = open(filename, 'w')
	file.write(doc.toprettyxml())
	file.close()

if __name__ == "__main__":
	sys.exit(main(sys.argv))
