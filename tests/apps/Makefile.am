check_PROGRAMS = test-application
check_LTLIBRARIES = libnoopapp.la \
		    libaccessibleapp.la \
		    libcomponentapp.la \
		    libactionapp.la \
		    librelationapp.la

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

libaccessibleapp_la_CFLAGS = $(TEST_APP_CFLAGS) $(LIB_XML_CFLAGS)
libaccessibleapp_la_LDFLAGS = $(TEST_APP_LDFLAGS)
libaccessibleapp_la_LIBADD = $(TEST_APP_LIBADD) $(LIB_XML_LIBS)
libaccessibleapp_la_SOURCES = accessible-app.c atk-object-xml-loader.c atk-object-xml-loader.h

libcomponentapp_la_CFLAGS = $(TEST_APP_CFLAGS)
libcomponentapp_la_LDFLAGS = $(TEST_APP_LDFLAGS)
libcomponentapp_la_LIBADD = $(TEST_APP_LIBADD)
libcomponentapp_la_SOURCES = component-app.c

libactionapp_la_CFLAGS = $(TEST_APP_CFLAGS)
libactionapp_la_LDFLAGS = $(TEST_APP_LDFLAGS)
libactionapp_la_LIBADD = $(TEST_APP_LIBADD)
libactionapp_la_SOURCES = action-app.c

librelationapp_la_CFLAGS = $(TEST_APP_CFLAGS)
librelationapp_la_LDFLAGS = $(TEST_APP_LDFLAGS)
librelationapp_la_LIBADD = $(TEST_APP_LIBADD)
librelationapp_la_SOURCES = relation-app.c
