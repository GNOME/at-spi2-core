gsettings_SCHEMAS = org.a11y.atspi.gschema.xml

org.a11y.atspi.gschema.xml: org.a11y.atspi.gschema.xml.in Makefile
	$(AM_V_GEN) sed -e "s|@libdir[@]|$(libdir)|" $< > $@

CLEANFILES = org.a11y.atspi.gschema.xml

@GSETTINGS_RULES@

EXTRA_DIST = \
	$(gsettings_SCHEMAS) \
	$(patsubst %,%.in,$(gsettings_SCHEMAS))
