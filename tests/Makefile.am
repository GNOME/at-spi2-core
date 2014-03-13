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

noinst_PROGRAMS = testdocument \
		  testrole  \
		  testrelation  \
		  teststateset  \
		  testvalue

LDADD = $(libatk) $(DEP_LIBS)

testdocument_SOURCES = testdocument.c
testrole_SOURCES = testrole.c
testrelation_SOURCES = testrelation.c
teststateset_SOURCES = teststateset.c
testvalue_SOURCES = testvalue.c

testatk_vc.mak: testatk_vc.makin $(top_srcdir)/build/atk_msvc_files.py $(top_srcdir)/build/msvcfiles.py
	$(PYTHON) $(top_srcdir)/build/atk_msvc_files.py -t nmake-exe

EXTRA_DIST = testatk_vc.makin testatk_vc.mak

DISTCLEANFILES = testatk_vc.mak
