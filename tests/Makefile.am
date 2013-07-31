if PLATFORM_WIN32
no_undefined = -no-undefined
else
no_undefined =
endif

libatk = $(top_builddir)/atk/libatk-$(ATK_API_VERSION).la

AM_CPPFLAGS = -I$(top_srcdir)	\
	-DG_DISABLE_DEPRECATED	\
	$(DEP_CFLAGS)
DEPS = \
	$(libatk)

noinst_PROGRAMS = testrole  \
		  testrelation  \
		  teststateset

LDADD = $(libatk) $(DEP_LIBS)

testrole_SOURCES = testrole.c
testrelation_SOURCES = testrelation.c
teststateset_SOURCES = teststateset.c
