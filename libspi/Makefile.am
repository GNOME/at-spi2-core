lib_LTLIBRARIES = libspi.la

INCLUDES = -I $(top_srcdir)           \
           -I $(top_builddir)         \
           -I $(top_srcdir)/libspi    \
           -I $(top_builddir)/libspi  \
           $(LIBSPI_CFLAGS)

LDFLAGS = $(LIBSPI_LIBS) @LT_VERSION_INFO@

DEBUG_CFLAGS=-DSPI_DEBUG

CFLAGS += $(DEBUG_CFLAGS)

libspiincludedir = $(includedir)/libspi

libspiinclude_HEADERS = accessible.h       \
                        application.h      \
                        atksimpleobject.h  \
                        desktop.h          \
                        listener.h         \
                        registry.h         \
                        Accessibility.h

IDL_OUT = Accessibility.h Accessibility-stubs.c Accessibility-skels.c Accessibility-common.c

IDL = \
	$(top_srcdir)/idl/Accessibility.idl \
	$(top_srcdir)/idl/Accessible.idl    \
	$(top_srcdir)/idl/Application.idl   \
	$(top_srcdir)/idl/Desktop.idl       \
	$(top_srcdir)/idl/Event.idl         \
	$(top_srcdir)/idl/Registry.idl	    \
	$(top_srcdir)/idl/Relation.idl      \
	$(top_srcdir)/idl/Role.idl          \
	$(top_srcdir)/idl/State.idl

BUILT_SOURCES = $(IDL_OUT)
CLEANFILES+=$(IDL_OUT)

libspi_la_SOURCES = accessible.c         \
                    accessible.h         \
                    application.c        \
                    application.h        \
                    atksimpleobject.c    \
                    atksimpleobject.h    \
                    desktop.c            \
                    desktop.h            \
                    listener.c           \
                    listener.h           \
                    registry.c           \
                    registry.h           \
                    $(IDL_OUT)

IDLFLAGS = -I$(OAF_IDL_DIR) -I$(LIBBONOBO_IDL_DIR) -I$(top_srcdir)/idl

$(IDL_OUT) : $(IDL) $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_COMPILATION__ $(IDLFLAGS) $(top_srcdir)/idl/Accessibility.idl

Accessibility-imodule.c : $(IDL) $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_COMPILATION__ $(IDLFLAGS) --imodule $(top_srcdir)/idl/Accessibility.idl

orbittypelibdir = $(libdir)/orbit
orbittypelib_LTLIBRARIES = Accessibility_module.la

Accessibility_module_la_LDFLAGS = \
	-export-dynamic -module
Accessibility_module_la_SOURCES = \
	Accessibility-imodule.c
