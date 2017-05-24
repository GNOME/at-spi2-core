CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =

libexec_PROGRAMS = at-spi-bus-launcher
at_spi_bus_launcher_SOURCES = at-spi-bus-launcher.c
at_spi_bus_launcher_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\" \
                               -DDATADIR=\"$(datadir)\" \
                               -DDBUS_DAEMON=\"$(DBUS_DAEMON)\"
at_spi_bus_launcher_CFLAGS = $(GIO_CFLAGS)
at_spi_bus_launcher_LDADD = $(GIO_LIBS) $(X11_LIBS)

substitutions = "s,@libexecdir[@],$(libexecdir),"

default_sessiondir = $(sysconfdir)/xdg/autostart
default_session_DATA = at-spi-dbus-bus.desktop
at-spi-dbus-bus.desktop: at-spi-dbus-bus.desktop.in
	$(AM_V_GEN) $(SED) -e$ $(substitutions)  $< > $@.tmp && mv $@.tmp $@

EXTRA_DIST += at-spi-dbus-bus.desktop.in
CLEANFILES += at-spi-dbus-bus.desktop

dbusservicedir = $(DBUS_SERVICES_DIR)
dbusservice_DATA = org.a11y.Bus.service
org.a11y.Bus.service: org.a11y.Bus.service.in
	$(AM_V_GEN) $(SED) -e $(substitutions) $< > $@.tmp && mv $@.tmp $@

EXTRA_DIST += org.a11y.Bus.service.in
CLEANFILES += org.a11y.Bus.service

systemd_userdir = $(prefix)/lib/systemd/user
systemd_user_DATA = at-spi-dbus-bus.service
at-spi-dbus-bus.service: at-spi-dbus-bus.service.in Makefile
	$(AM_V_GEN) $(SED) -e $(substitutions) $< > $@.tmp && mv $@.tmp $@

EXTRA_DIST += at-spi-dbus-bus.service.in
CLEANFILES += at-spi-dbus-bus.service

busconfigdir = $(datadir)/defaults/at-spi2
busconfig_DATA = accessibility.conf
accessibility.conf: accessibility.conf.in
	$(AM_V_GEN) $(SED) -e "s|[@]DATADIR[@]|$(datadir)|" $(srcdir)/$@.in > $@

EXTRA_DIST += accessibility.conf.in
CLEANFILES += accessibility.conf

-include $(top_srcdir)/git.mk
