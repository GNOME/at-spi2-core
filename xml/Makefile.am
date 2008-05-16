XML= \
	Accessibility.xml \
	Accessibility_Accessible.xml \
	Accessibility_Action.xml \
	Accessibility_Application.xml \
	Accessibility_Component.xml \
	Accessibility_Document.xml \
	Accessibility_EditableText.xml \
	Accessibility_Event.xml \
	Accessibility_Hyperlink.xml \
	Accessibility_Hypertext.xml \
	Accessibility_Image.xml \
	Accessibility_LoginHelper.xml \
	Accessibility_Registry.xml \
	Accessibility_Role.xml \
	Accessibility_Selection.xml \
	Accessibility_Selector.xml \
	Accessibility_State.xml \
	Accessibility_StreamableContent.xml \
	Accessibility_Table.xml \
	Accessibility_Text.xml \
	Accessibility_Value.xml

spec.xml: $(XML)
	xsltproc --xinclude $(top_srcdir)/tools/identity.xsl $< >$@

EXTRA_DIST = $(XML)

BUILT_SOURCES = spec.xml
CLEANFILES = spec.xml
