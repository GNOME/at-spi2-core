SUBDIRS = dummyatk

noinst_PROGRAMS = atk-test app-test
TESTS = atk-test
lib_LTLIBRARIES =libxmlloader.la libtestutils.la

atk_test_CFLAGS = $(DBUS_CFLAGS) \
                  $(GLIB_CFLAGS) \
                  $(ATSPI_CFLAGS) \
                  -I$(top_builddir)\
                  -I$(top_srcdir)/tests/dummyatk/ \
                  -I$(top_srcdir)/tests/dummyatk/.libs \
                  -I$(top_srcdir)/tests/dummyatk/.deps \
                  -I$(top_srcdir)

atk_test_LDFLAGS  = libxmlloader.la \
                    libtestutils.la \
                    $(GLIB_LIBS) \
                    $(ATSPI_LIBS) \
                    $(top_srcdir)/tests/dummyatk/libdummyatk.la

atk_test_CPPFLAGS = -DTESTS_BUILD_DIR=\"$(top_builddir)/tests\" \
                    -DTESTS_SRC_DIR=\"$(top_srcdir)/tests\" \
                    -DTESTS_DATA_DIR=\"$(top_srcdir)/tests/data\"

atk_test_SOURCES = atk_suite.c \
                   atk_test_accessible.c \
                   atk_test_action.c \
                   atk_test_component.c

app_test_CFLAGS = -I$(top_builddir) \
                   $(GLIB_CFLAGS) \
                   $(ATK_CFLAGS) \
                   -I$(top_srcdir)/tests/dummyatk \
                   -I$(top_srcdir)/atk-adaptor

app_test_LDFLAGS = libxmlloader.la \
                   libtestutils.la \
                   $(GLIB_LIBS) \
                   $(ATK_LIBS) \
                   $(top_srcdir)/tests/dummyatk/libdummyatk.la \
                   $(top_srcdir)/atk-adaptor/libatk-bridge-2.0.la

app_test_SOURCES = test-application.c

libxmlloader_la_CFLAGS = $(GLIB_CFLAGS) \
                         $(GOBJ_CFLAGS)  \
                         $(XML_CFLAGS) \
                         $(ATK_CFLAGS) \
                         -I$(top_srcdir)/tests/dummyatk/

libxmlloader_la_LIBADD = $(GLIB_LIBS) \
                         $(XML_LIBS) \
                         $(GOBJ_LIBS) \
                         $(top_srcdir)/tests/dummyatk/libdummyatk.la \
                         $(ATK_LIBS)

libxmlloader_la_SOURCES = atk-object-xml-loader.c\
                          atk-object-xml-loader.h

libtestutils_la_CFLAGS = -I$(top_builddir) \
                         $(GLIB_CFLAGS) \
                         $(ATSPI_CFLAGS)

libtestutils_la_LIBADD = $(GLIB_LIBS) \
                         $(ATSPI_LIBS)

libtestutils_la_SOURCES = atk_test_util.c \
                          atk_test_util.h
