## Process this file with automake to produce Makefile.in

doxygen_configfile = Doxyfile

EXTRA_DIST = Doxyfile README html

html/index.html:
	-rm -rf html
	-doxygen $(doxygen_configfile) \
	2> doxygen-warnings.txt | tee doxygen-output.txt && cat doxygen-warnings.txt

referencetopdir = $(datadir)/doc/at-spi-$(VERSION)/reference
referencedir = $(referencetopdir)/html

install-reference: html/index.html
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(referencedir)
	@-dir='$(<D)'; for p in $$dir/*.html $$dir/*.css $$dir/*.png; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " $(INSTALL_DATA) $$p $(DESTDIR)$(referencedir)/$$f"; \
	  $(INSTALL_DATA) $$p $(DESTDIR)$(referencedir)/$$f; \
	done

uninstall-reference: html/index.html
	@$(NORMAL_UNINSTALL)
	@-dir='$(<D)'; for p in $$dir/*.html $$dir/*.css $$dir/*.png; do \
	  f="`echo $$p | sed -e 's|^.*/||'`"; \
	  echo " rm -f $(DESTDIR)$(referencedir)/$$f"; \
	  rm -f $(DESTDIR)$(referencedir)/$$f; \
	done

all-local: html/index.html

install-data-local: install-reference

uninstall-local: uninstall-reference

maintainer-clean-local: doc-clean

.PHONY: doc-clean install-reference uninstall-reference html-clean

# Direct all the doxygen console output to a text file, so we can read it later:
# In doxywizard, disable EXTRACT_ALL to get warnings about undocumented methods.
doxygen-warnings: html-clean html/index.html
