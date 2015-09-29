SUBDIRS = data dummyatk

noinst_PROGRAMS = atk-test app-test
TESTS = atk-test
lib_LTLIBRARIES =libxmlloader.la libtestutils.la

atk_test_CFLAGS = $(DBUS_CFLAGS) \
                  $(GLIB_CFLAGS) \
                  $(ATSPI_CFLAGS) \
                  -I$(top_builddir)\
                  -I$(top_srcdir)/tests/dummyatk/ \
                  -I$(top_builddir)/tests/dummyatk/.libs \
                  -I$(top_builddir)/tests/dummyatk/.deps \
                  -I$(top_srcdir) \
                  -Wall

atk_test_LDADD = libxmlloader.la \
                 libtestutils.la \
                 $(GLIB_LIBS) \
                 $(ATSPI_LIBS) \
                 $(top_builddir)/tests/dummyatk/libdummyatk.la

atk_test_CPPFLAGS = -DTESTS_BUILD_DIR=\"$(top_builddir)/tests\" \
                    -DTESTS_SRC_DIR=\"$(top_srcdir)/tests\" \
                    -DTESTS_DATA_DIR=\"$(top_srcdir)/tests/data\"

atk_test_SOURCES = atk_suite.c \
                   atk_suite.h \
                   atk_test_accessible.c \
                   atk_test_action.c \
                   atk_test_component.c \
                   atk_test_collection.c \
                   atk_test_editable_text.c \
                   atk_test_document.c \
                   atk_test_hyperlink.c \
                   atk_test_hypertext.c \
                   atk_test_image.c \
                   atk_test_selection.c \
                   atk_test_state_set.c \
                   atk_test_table.c \
                   atk_test_table_cell.c \
                   atk_test_text.c \
                   atk_test_value.c

app_test_CFLAGS = -I$(top_builddir) \
                   $(GLIB_CFLAGS) \
                   $(ATK_CFLAGS) \
                   -I$(top_srcdir)/tests/dummyatk \
                   -I$(top_srcdir)/atk-adaptor \
                   -Wall

app_test_LDADD = libxmlloader.la \
                 libtestutils.la \
                 $(GLIB_LIBS) \
                 $(ATK_LIBS) \
                 $(top_builddir)/tests/dummyatk/libdummyatk.la \
                 $(top_builddir)/atk-adaptor/libatk-bridge-2.0.la

app_test_SOURCES = test-application.c

libxmlloader_la_CFLAGS = $(GLIB_CFLAGS) \
                         $(GOBJ_CFLAGS)  \
                         $(XML_CFLAGS) \
                         $(ATK_CFLAGS) \
                         -I$(top_srcdir)/tests/dummyatk/ \
                         -Wall

libxmlloader_la_LIBADD = $(GLIB_LIBS) \
                         $(XML_LIBS) \
                         $(GOBJ_LIBS) \
                         $(top_builddir)/tests/dummyatk/libdummyatk.la \
                         $(ATK_LIBS)

libxmlloader_la_SOURCES = atk-object-xml-loader.c\
                          atk-object-xml-loader.h

libtestutils_la_CFLAGS = -I$(top_builddir) \
                         $(GLIB_CFLAGS) \
                         $(ATSPI_CFLAGS) \
                         -Wall

libtestutils_la_LIBADD = $(GLIB_LIBS) \
                         $(ATSPI_LIBS)

libtestutils_la_SOURCES = atk_test_util.c \
                          atk_test_util.h
