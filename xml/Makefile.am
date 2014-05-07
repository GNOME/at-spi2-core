EXTRA_DIST = \
	Accessibility.xml \
	Accessible.xml \
	Action.xml \
	Application.xml \
	Cache.xml \
	Collection.xml \
	Component.xml \
	create-introspection.sh \
	DeviceEventController.xml \
	DeviceEventListener.xml \
	Document.xml \
	EditableText.xml \
	Event.xml \
	Hyperlink.xml \
	Hypertext.xml \
	Image.xml \
	Registry.xml \
	remove-annotations.xsl \
	Selection.xml \
	Socket.xml \
	Table.xml \
	Text.xml \
	Value.xml \
	versioned-introspection.py

CLEANFILES = \
	introspection.c \
	introspection.h \
	Processed.xml

#BUILT_SOURCES = spec.xml
#CLEANFILES += spec.xml

XML_SPEC = $(filter %.xml,$(EXTRA_DIST))

spec.xml: $(XML_SPEC)
	xsltproc --xinclude $(top_srcdir)/tools/identity.xsl Accessibility.xml >spec.xml


-include $(top_srcdir)/git.mk
