# Process this file with automake to create Makefile.in.

SUBDIRS=atk tests docs po

EXTRA_DIST = 			\
	atk-uninstalled.pc.in	\
	atk.pc.in		\
	atk.spec.in		\
	atk.spec		\
	atk-zip.sh.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = atk.pc

# This is a version of the automake-1.4 distcheck rule modified
# to pass --enable-gtk-doc to ./configure
#
mydistcheck: dist
	-rm -rf $(distdir)
	GZIP=$(GZIP_ENV) $(TAR) zxf $(distdir).tar.gz
	mkdir $(distdir)/=build
	mkdir $(distdir)/=inst
	dc_install_base=`cd $(distdir)/=inst && pwd`; \
	cd $(distdir)/=build \
	  && ../configure --srcdir=.. --prefix=$$dc_install_base --enable-gtk-doc \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) dvi \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && $(MAKE) $(AM_MAKEFLAGS) installcheck \
	  && $(MAKE) $(AM_MAKEFLAGS) dist
	-rm -rf $(distdir)
	@banner="$(distdir).tar.gz is ready for distribution"; \
	dashes=`echo "$$banner" | sed s/./=/g`; \
	echo "$$dashes"; \
	echo "$$banner"; \
	echo "$$dashes"
