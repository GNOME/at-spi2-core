XML_SPEC= \
	Accessibility.xml \
	org.freedesktop.atspi.Accessible.xml \
	org.freedesktop.atspi.Action.xml \
	org.freedesktop.atspi.Application.xml \
	org.freedesktop.atspi.Component.xml \
	org.freedesktop.atspi.Desktop.xml \
	org.freedesktop.atspi.Document.xml \
	org.freedesktop.atspi.EditableText.xml \
	org.freedesktop.atspi.Hyperlink.xml \
	org.freedesktop.atspi.Hypertext.xml \
	org.freedesktop.atspi.Image.xml \
	org.freedesktop.atspi.LoginHelper.xml \
	Accessibility_Registry_Common.xml \
	org.freedesktop.atspi.Registry.xml \
	org.freedesktop.atspi.DeviceEventController.xml \
	org.freedesktop.atspi.DeviceEventListener.xml \
	Accessibility_Relation.xml \
	Accessibility_Role.xml \
	org.freedesktop.atspi.Selection.xml \
	org.freedesktop.atspi.Selector.xml \
	Accessibility_State.xml \
	org.freedesktop.atspi.StreamableContent.xml \
	org.freedesktop.atspi.Table.xml \
	org.freedesktop.atspi.Text.xml \
	org.freedesktop.atspi.Tree.xml \
	org.freedesktop.atspi.Value.xml

spec.xml: $(XML_SPEC)
	xsltproc --xinclude $(top_srcdir)/tools/identity.xsl $< >$@

# I'm only adding spec.xml because it is needed to build other files that
# are currently being distributed. -MPG
EXTRA_DIST = $(XML_SPEC) spec.xml

BUILT_SOURCES = spec.xml
CLEANFILES = spec.xml

INTROSPECT_FILES = $(addprefix introspection/,$(patsubst %.xml,%,$(filter org.freedesktop.atspi%,$(XML_SPEC))))

$(INTROSPECT_FILES): introspection/%: %.xml ../tools/spec-to-introspect.xsl
	@mkdir -p introspection
	xsltproc ../tools/spec-to-introspect.xsl $< |sed -e 's@xmlns:tp="http://telepathy\.freedesktop\.org/wiki/DbusSpec.extensions-v0"@@g' >$@

introspectdir = $(pkgdatadir)/$(DEFAULT_ATSPI_INTROSPECTION_PATH)
dist_introspect_DATA = $(INTROSPECT_FILES)
