
lib_LTLIBRARIES = \
	libteststateset.la	\
	libtestrelation.la

libatk = $(top_builddir)/atk/libatk.la

INCLUDES = -I$(top_srcdir)	\
	-DG_DISABLE_DEPRECATED	\
	@DEP_CFLAGS@
DEPS = \
	$(libatk)

libteststateset_la_SOURCES = teststateset.c
libteststateset_la_LDFLAGS = -avoid-version	\
	-module

libtestrelation_la_SOURCES = testrelation.c
libtestrelation_la_LDFLAGS = -avoid-version	\
	-module
