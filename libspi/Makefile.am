lib_LTLIBRARIES = libspi.la

INCLUDES = -I$(top_srcdir)	\
           -I$(top_builddir)	\
           $(LIBSPI_CFLAGS)

LDFLAGS = @LDFLAGS@ $(LIBSPI_LIBS) $(XTST_LIBS) @LT_VERSION_INFO@

#DEBUG_CFLAGS=-DSPI_DEBUG

CFLAGS = @CFLAGS@ $(DEBUG_CFLAGS)

libspiincludedir = $(includedir)/at-spi-1.0/libspi

libspiinclude_HEADERS =		\
	Accessibility.h		\
	accessible.h		\
	eventlistener.h		\
	action.h		\
	application.h		\
	base.h			\
	component.h		\
	editabletext.h		\
	hyperlink.h		\
	hypertext.h		\
	image.h			\
	devicelistener.h	\
	keystrokelistener.h	\
	keymasks.h		\
	libspi.h		\
	listener.h		\
	relation.h		\
	remoteobject.h		\
	selection.h		\
	stateset.h		\
	table.h			\
	text.h			\
	value.h

IDL_OUT = Accessibility.h Accessibility-stubs.c Accessibility-skels.c Accessibility-common.c

IDL_DEPS = \
	$(top_srcdir)/idl/Accessibility.idl			\
	$(top_srcdir)/idl/Accessibility_Accessible.idl		\
	$(top_srcdir)/idl/Accessibility_Action.idl		\
	$(top_srcdir)/idl/Accessibility_Application.idl		\
	$(top_srcdir)/idl/Accessibility_Component.idl		\
	$(top_srcdir)/idl/Accessibility_Desktop.idl		\
	$(top_srcdir)/idl/Accessibility_EditableText.idl	\
	$(top_srcdir)/idl/Accessibility_Event.idl		\
	$(top_srcdir)/idl/Accessibility_Hyperlink.idl		\
	$(top_srcdir)/idl/Accessibility_Hypertext.idl		\
	$(top_srcdir)/idl/Accessibility_Image.idl		\
	$(top_srcdir)/idl/Accessibility_Registry.idl		\
	$(top_srcdir)/idl/Accessibility_Relation.idl		\
	$(top_srcdir)/idl/Accessibility_Role.idl		\
	$(top_srcdir)/idl/Accessibility_Selection.idl		\
	$(top_srcdir)/idl/Accessibility_State.idl		\
	$(top_srcdir)/idl/Accessibility_StreamableContent.idl	\
	$(top_srcdir)/idl/Accessibility_Table.idl		\
	$(top_srcdir)/idl/Accessibility_Text.idl		\
	$(top_srcdir)/idl/Accessibility_Value.idl


BUILT_SOURCES = $(IDL_OUT) Accessibility-imodule.c
CLEANFILES+= $(IDL_OUT) Accessibility-imodule.c

libspi_la_SOURCES =		\
	accessible.c		\
	action.c		\
	application.c		\
	base.c			\
	component.c		\
	editabletext.c		\
	hyperlink.c		\
	hypertext.c		\
	image.c			\
	devicelistener.c	\
	relation.c		\
	remoteobject.c		\
	selection.c		\
	spi-private.h		\
	stateset.c		\
	table.c			\
	text.c			\
	util.c			\
	value.c			\
	listener.c		\
	eventlistener.c		\
	$(IDL_OUT)

$(top_srcdir)/libspi/accessible.c $(top_srcdir)/libspi/base.h $(top_srcdir)/libspi/keystrokelistener.h $(top_srcdir)/libspi/libspi.h $(top_srcdir)/libspi/listener.h $(top_srcdir)/libspi/remoteobject.h : Accessibility.h

IDLFLAGS = -I$(BONOBO_ACTIVATION_IDL_DIR) \
	   -I$(LIBBONOBO_IDL_DIR)         \
	   -I$(top_srcdir)/idl            \
	   --add-imodule

$(IDL_OUT) Accessibility-imodule.c : $(IDL_DEPS) $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_COMPILATION__ $(IDLFLAGS) $(top_srcdir)/idl/Accessibility.idl

orbittypelibdir = $(libdir)/orbit-2.0
orbittypelib_LTLIBRARIES = Accessibility_module.la

Accessibility_module_la_LDFLAGS = \
	-export-dynamic -module -avoid-version
Accessibility_module_la_SOURCES = \
	Accessibility-imodule.c

DONT_DIST_SOURCE = $(IDL_OUT) Accessibility-imodule.c

dist-hook:
	for file in $(DONT_DIST_SOURCE) ; do \
	    rm -f $(distdir)/$$file ; \
	done
