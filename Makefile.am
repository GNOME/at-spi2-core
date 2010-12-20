SUBDIRS=dbind droute common atk-adaptor po schemas

schemas_in_files = at-spi2.schemas.in
if RELOCATE
schemasdir       = $(GCONF_SCHEMA_FILE_DIR)
schemas_DATA     = $(schemas_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@
endif

gtk_modulesdir = $(libdir)/gnome-settings-daemon-3.0/gtk-modules/
gtk_modules_DATA = at-spi2-atk.desktop

if GCONF_SCHEMAS_INSTALL
install-data-local:
		if test -z "$(DESTDIR)" ; then \
			for p in $(schemas_DATA) ; do \
				GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/$$p ; \
			done \
		fi
else
install-data-local:
endif

EXTRA_DIST = \
	$(schemas_in_files) \
	$(gtk_modules_DATA)


CLEANFILES = \
	$(schemas_DATA)
