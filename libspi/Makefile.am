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
                        Accessible.h       \
                        Application.h      \
                        Desktop.h          \
                        Event.h            \
                        Registry.h         \
                        Relation.h         \
                        Role.h             \
                        State.h

Accessible_IDLOUT = Accessible.h Accessible-stubs.c Accessible-skels.c Accessible-common.c
Application_IDLOUT = Application.h Application-stubs.c Application-skels.c Application-common.c
Desktop_IDLOUT = Desktop.h Desktop-stubs.c Desktop-skels.c Desktop-common.c
Event_IDLOUT = Event.h Event-stubs.c Event-skels.c Event-common.c
Registry_IDLOUT = Registry.h Registry-stubs.c Registry-skels.c Registry-common.c
Relation_IDLOUT = Relation.h Relation-stubs.c Relation-skels.c Relation-common.c
Role_IDLOUT = Role.h Role-stubs.c Role-skels.c Role-common.c
State_IDLOUT = State.h State-stubs.c State-skels.c State-common.c

BUILT_SOURCES = $(Accessible_IDLOUT)     \
                $(Application_IDLOUT)    \
                $(Desktop_IDLOUT)        \
                $(Event_IDLOUT)          \
                $(Registry_IDLOUT)	 \
                $(Relation_IDLOUT)       \
                $(Role_IDLOUT)           \
                $(State_IDLOUT)

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
                    $(BUILT_SOURCES)

IDLFLAGS = -I$(OAF_IDL_DIR) -I$(LIBBONOBO_IDL_DIR) -I$(top_srcdir)/idl --noimodule

$(Accessible_IDLOUT): $(top_srcdir)/idl/Accessible.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_ACCESSIBLE_COMPILATION $(IDLFLAGS) $(top_srcdir)/idl/Accessible.idl

$(Application_IDLOUT): $(top_srcdir)/idl/Application.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_APPLICATION_COMPILATION $(IDLFLAGS) $(top_srcdir)/idl/Application.idl

$(Desktop_IDLOUT): $(top_srcdir)/idl/Desktop.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_DESKTOP_COMPILATION $(IDLFLAGS) $(top_srcdir)/idl/Desktop.idl

$(Event_IDLOUT): $(top_srcdir)/idl/Event.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_EVENT_COMPILATION $(IDLFLAGS) $(top_srcdir)/idl/Event.idl

$(Registry_IDLOUT): $(top_srcdir)/idl/Registry.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_REGISTRY_COMPILATION $(IDLFLAGS) $(top_srcdir)/idl/Registry.idl

$(Relation_IDLOUT): $(top_srcdir)/idl/Relation.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_RELATION_COMPILATION $(IDLFLAGS) $(top_srcdir)/idl/Relation.idl

$(Role_IDLOUT): $(top_srcdir)/idl/Role.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_ROLE_COMPILATION $(IDLFLAGS) $(top_srcdir)/idl/Role.idl

$(State_IDLOUT): $(top_srcdir)/idl/State.idl $(ORBIT_IDL)
	$(ORBIT_IDL) -D__ACCESSIBILITY_STATE_COMPILATION $(IDLFLAGS) $(top_srcdir)/idl/State.idl
