check_PROGRAMS = test-application
check_LTLIBRARIES = libnoopapp.la libobjectapp.la

test_application_CFLAGS = $(DBUS_GLIB_CFLAGS)	\
			  $(ATK_CFLAGS)		\
			  $(GMODULE_CFLAGS)	\
		       	  -I$(top_srcdir)

test_application_LDADD = $(DBUS_GLIB_LIBS)	\
		         $(GMODULE_LIBS)	\
		         $(ATK_LIBS)

test_application_SOURCES = test-application.c


TEST_APP_CFLAGS = $(ATK_CFLAGS) $(GMODULE_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/tests/dummyatk/
TEST_APP_LDFLAGS = -no-undefined -module -avoid-version -rpath /a/fake/path
TEST_APP_LIBADD = $(ATK_LIBS) $(GMODULE_LIBS) $(top_builddir)/tests/dummyatk/libdummyatk.la

libnoopapp_la_CFLAGS = $(TEST_APP_CFLAGS)
libnoopapp_la_LDFLAGS = $(TEST_APP_LDFLAGS)
libnoopapp_la_LIBADD = $(TEST_APP_LIBADD)
libnoopapp_la_SOURCES = noop-app.c

libobjectapp_la_CFLAGS = $(TEST_APP_CFLAGS)
libobjectapp_la_LDFLAGS = $(TEST_APP_LDFLAGS)
libobjectapp_la_LIBADD = $(TEST_APP_LIBADD)
libobjectapp_la_SOURCES = object-app.c
