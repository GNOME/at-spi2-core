CLEANFILES=

busconfigdir = $(datadir)/defaults/at-spi2
busconfig_DATA = accessibility.conf

libexec_PROGRAMS = at-spi-bus-launcher
at_spi_bus_launcher_SOURCES = at-spi-bus-launcher.c
at_spi_bus_launcher_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\" \
                               -DDATADIR=\"$(datadir)\" \
                               -DDBUS_DAEMON=\"$(DBUS_DAEMON)\"
at_spi_bus_launcher_CFLAGS = $(GIO_CFLAGS)
at_spi_bus_launcher_LDADD = $(GIO_LIBS) $(X_LIBS)

default_sessiondir = $(sysconfdir)/xdg/autostart
default_session_DATA = at-spi-dbus-bus.desktop

substitutions = "s,@libexecdir[@],$(libexecdir),"
at-spi-dbus-bus.desktop: at-spi-dbus-bus.desktop.in
	sed -e$ $(substitutions)  $< > $@.tmp && mv $@.tmp $@
EXTRA_DIST = at-spi-dbus-bus.desktop.in
CLEANFILES += at-spi-dbus-bus.desktop

dbusservicedir=$(DBUS_SERVICES_DIR)
dbusservice_DATA = org.a11y.Bus.service

org.a11y.Bus.service: org.a11y.Bus.service.in
	sed -e $(substitutions) $< > $@.tmp && mv $@.tmp $@

systemd_userdir = $(prefix)/lib/systemd/user
systemd_user_DATA = at-spi-dbus-bus.service
at-spi-dbus-bus.service: at-spi-dbus-bus.service.in Makefile
	sed -e $(substitutions) $< > $@.tmp && mv $@.tmp $@

accessibility.conf: accessibility.conf.in
	sed -e "s|[@]DATADIR[@]|$(datadir)|" $(srcdir)/$@.in > $@

EXTRA_DIST += org.a11y.Bus.service.in \
	at-spi-dbus-bus.service.in \
	accessibility.conf.in

DISTCLEANFILES = org.a11y.Bus.service \
	$(systemd_user_DATA) \
	accessibility.conf

-include $(top_srcdir)/git.mk
