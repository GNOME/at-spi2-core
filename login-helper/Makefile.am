lib_LTLIBRARIES = libloginhelper.la

LDADD = $(X_LIBS) $(LOGIN_HELPER_LIBS) 

INCLUDES = -I$(top_srcdir)          \
           -I$(top_builddir)        \
	   $(WARN_CFLAGS)	\
           $(DBUS_GLIB_CFLAGS)

libloginhelperincludedir = $(includedir)/at-spi-1.0/login-helper

libloginhelper_la_LDFLAGS = -no-undefined
libloginhelper_la_LIBADD = \
	$(top_builddir)/spi-common/libspicommon.la \
	$(top_builddir)/droute/libdroute.la \
	$(X_LIBS)

libloginhelperinclude_HEADERS = \
	login-helper.h			

libloginhelper_la_SOURCES =		\
	login-helper.c
