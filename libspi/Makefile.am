lib_LTLIBRARIES = libspi.la

INCLUDES = -I $(top_srcdir)           \
           -I $(top_builddir)         \
           $(LIBSPI_CFLAGS)

LDFLAGS = $(LIBSPI_LIBS) -lXtst @LT_VERSION_INFO@

DEBUG_CFLAGS=-DSPI_DEBUG

CFLAGS += $(DEBUG_CFLAGS)

libspiincludedir = $(includedir)/at-spi-1.0/libspi

libspiinclude_HEADERS = Accessibility.h \
			accessible.h \
			accessibleeventlistener.h \
			action.h \
                        application.h \
			component.h \
                        desktop.h \
			deviceeventcontroller.h	\
			editabletext.h\
			hyperlink.h\
			hypertext.h\
			image.h \
			keystrokelistener.h \
			keymasks.h \
			libspi.h \
                        listener.h \
                        registry.h \
			relation.h \
			selection.h \
			table.h \
			text.h \
			value.h

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

BUILT_SOURCES = $(IDL_OUT) Accessibility-imodule.c
CLEANFILES+=$(IDL_OUT) Accessibility-imodule.c

libspi_la_SOURCES = accessible.c         \
                    accessible.h         \
                    action.c\
		    action.h\
		    application.c        \
                    application.h        \
		    component.c		 \
		    component.h		 \
                    desktop.c            \
                    desktop.h            \
		    deviceeventcontroller.c \
		    deviceeventcontroller.h \
		    editabletext.c\
		    editabletext.h\
		    hyperlink.c\
		    hyperlink.h\
		    hypertext.c\
		    hypertext.h\
		    image.c\
		    image.h\
		    keystrokelistener.c\
		    keystrokelistener.h\
		    relation.c\
		    relation.h\
		    selection.c\
		    selection.h\
		    table.c\
		    table.h\
		    text.c\
		    text.h\
		    value.c\
		    value.h\
                    listener.c           \
                    listener.h           \
		    accessibleeventlistener.c	\
		    accessibleeventlistener.h	\
                    registry.c           \
                    registry.h           \
		    keymasks.h		\
                    $(IDL_OUT)

IDLFLAGS = -I$(BONOBO_ACTIVATION_IDL_DIR) \
	   -I$(LIBBONOBO_IDL_DIR)         \
	   -I$(top_srcdir)/idl            \
	   --add-imodule

$(IDL_OUT) Accessibility-imodule.c : $(IDL) $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_COMPILATION__ $(IDLFLAGS) $(top_srcdir)/idl/Accessibility.idl

orbittypelibdir = $(libdir)/orbit-2.0
orbittypelib_LTLIBRARIES = Accessibility_module.la

Accessibility_module_la_LDFLAGS = \
	-export-dynamic -module
Accessibility_module_la_SOURCES = \
	Accessibility-imodule.c
