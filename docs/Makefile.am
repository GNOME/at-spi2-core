## Process this file with automake to produce Makefile.in

EXTRA_DIST = Makefile.am atk-api.sgml atk-docs.sgml atk-sections.txt

# The name of the module.
DOC_MODULE=atk

# The top-level SGML file.
DOC_MAIN_SGML_FILE=atk-docs.sgml

# The directory containing the source code (if it contains documentation).
DOC_SOURCE_DIR=../atk


TARGET_DIR=$(HTML_DIR)/$(DOC_MODULE)

scan:
	gtkdoc-scan --module=$(DOC_MODULE) --source-dir=$(DOC_SOURCE_DIR) --ignore-headers="libgnomeP.h gnome-i18nP.h gnome-triggersP.h gnome-magic.h vroot.h"

templates: scan
	gtkdoc-mktmpl --module=$(DOC_MODULE)

sgml:
	gtkdoc-mkdb --module=$(DOC_MODULE) --source-dir=$(DOC_SOURCE_DIR)

html:
	if test ! -d html ; then mkdir html ; fi
	-cd html && gtkdoc-mkhtml $(DOC_MODULE) ../$(DOC_MAIN_SGML_FILE)

clean-local:
	rm -f *~ *.bak *.hierarchy *.signals *.args *-unused.txt

maintainer-clean-local: clean-local
	rm -rf sgml html $(DOC_MODULE)-decl-list.txt $(DOC_MODULE)-decl.txt 

install-data-local:
	install -d -m 0755 $(TARGET_DIR)
	install -m 0644 html/*.html $(TARGET_DIR)
	install -m 0644 html/index.sgml $(TARGET_DIR)
	gtkdoc-fixxref --module=$(DOC_MODULE) --html-dir=$(HTML_DIR)

#
# Require gtk-doc when making dist
#
if ENABLE_GTK_DOC
dist-check-gtkdoc:
else
dist-check-gtkdoc:
	@echo "*** gtk-doc must be installed and enabled in order to make dist"
	@false
endif

dist-hook: dist-check-gtkdoc dist-hook-local
	mkdir $(distdir)/tmpl
	mkdir $(distdir)/sgml
	mkdir $(distdir)/html
	-cp $(srcdir)/tmpl/*.sgml $(distdir)/tmpl
	-cp $(srcdir)/sgml/*.sgml $(distdir)/sgml
	-cp $(srcdir)/html/index.sgml $(distdir)/html
	-cp $(srcdir)/html/*.html $(srcdir)/html/*.css $(distdir)/html

	images=$(HTML_IMAGES) ;			\
	for i in $$images ; do			\
	  cp $(srcdir)/$$i $(distdir)/html ;  \
	done

.PHONY : html sgml templates scan dist-hook-local
