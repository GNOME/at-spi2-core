lib_LTLIBRARIES = libloginhelper.la

LDADD = $(X_LIBS) $(LOGIN_HELPER_LIBS) 

INCLUDES = -I$(top_srcdir)          \
           -I$(top_builddir)        \
	   $(WARN_CFLAGS)	\
           $(REGISTRYD_CFLAGS)

IDL_OUT = Accessibility_LoginHelper.h Accessibility_LoginHelper-stubs.c Accessibility_LoginHelper-skels.c Accessibility_LoginHelper-common.c

IDL_DEPS = \
	$(top_srcdir)/idl/Accessibility_LoginHelper.idl

BUILT_SOURCES = $(IDL_OUT) Accessibility_LoginHelper-imodule.c
CLEANFILES = $(IDL_OUT) Accessibility_LoginHelper-imodule.c

libloginhelperincludedir = $(includedir)/at-spi-1.0/login-helper

libloginhelper_la_LDFLAGS = -no-undefined
libloginhelper_la_LIBADD = $(REGISTRYD_LIBS)

libloginhelperinclude_HEADERS = \
	Accessibility_LoginHelper.h	\
	login-helper.h			

libloginhelper_la_SOURCES =		\
	$(IDL_OUT)		\
	login-helper.c

IDLFLAGS = -I$(BONOBO_ACTIVATION_IDL_DIR) \
	   -I$(LIBBONOBO_IDL_DIR)         \
	   -I$(top_srcdir)/idl            \
	   --add-imodule

$(IDL_OUT) Accessibility_LoginHelper-imodule.c : $(IDL_DEPS) $(ORBIT_IDL)
	$(ORBIT_IDL) $(IDLFLAGS) $(top_srcdir)/idl/Accessibility_LoginHelper.idl

orbittypelibdir = $(libdir)/orbit-2.0
orbittypelib_LTLIBRARIES = Accessibility_LoginHelper_module.la

Accessibility_LoginHelper_module_la_LDFLAGS = \
	-export-dynamic -module -avoid-version -no-undefined
Accessibility_LoginHelper_module_la_LIBADD = \
	$(ORBIT_LIBS)
Accessibility_LoginHelper_module_la_SOURCES = \
	Accessibility_LoginHelper-imodule.c

DONT_DIST_SOURCE = $(IDL_OUT) Accessibility_LoginHelper-imodule.c

dist-hook:
	for file in $(DONT_DIST_SOURCE) ; do \
	    rm -f $(distdir)/$$file ; \
	done

