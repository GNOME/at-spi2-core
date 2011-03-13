busconfigdir = $(sysconfdir)/at-spi2
busconfig_DATA = accessibility.conf

atspidbusdir = $(libexecdir)
atspidbus_SCRIPTS = at-spi-dbus-bus

default_sessiondir = $(sysconfdir)/xdg/autostart
default_session_DATA = at-spi-dbus-bus.desktop

at-spi-dbus-bus.desktop: at-spi-dbus-bus.desktop.in
	sed -e "s,@libexecdir[@],$(libexecdir)," $< > $@.tmp && mv $@.tmp $@

EXTRA_DIST= \
	accessibility.conf \
	at-spi-dbus-bus.in \
	at-spi-dbus-bus.desktop.in

CLEANFILES= \
	at-spi-dbus-bus.desktop \
	at-spi-dbus-bus
