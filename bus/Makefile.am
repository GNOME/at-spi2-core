busconfigdir = $(sysconfdir)/at-spi2
busconfig_DATA = accessibility.conf			

atspidbusdir = $(bindir)
atspidbus_SCRIPTS = at-spi-dbus-bus

default_sessiondir = $(sysconfdir)/xdg/autostart
default_session_DATA = at-spi-dbus-bus.desktop

do_subst = sed -e 's,[@]prefix[@],${prefix},g' \
               -e 's,[@]sysconfdir[@],${sysconfdir},g' \
               -e 's,[@]DBUS_DAEMON[@],@DBUS_DAEMON@,g'

at-spi-dbus-bus:	at-spi-dbus-bus.in Makefile
	$(do_subst) $(srcdir)/$@.in > $@

EXTRA_DIST= \
	accessibility.conf \
	at-spi-dbus-bus.in \
	at-spi-dbus-bus.desktop

CLEANFILES= \
	at-spi-dbus-bus
