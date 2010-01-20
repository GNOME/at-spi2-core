busconfigdir=$(sysconfdir)/at-spi2
busconfig_DATA=\
	accessibility.conf			

atspidbusdir=$(bindir)
atspidbus_SCRIPTS=\
	at-spi-dbus-bus

default_sessiondir = $(sysconfdir)/xdg/autostart
default_session_DATA = at-spi-dbus-bus.desktop

EXTRA_DIST= \
            accessibility.conf \
            at-spi-dbus-bus.desktop.in

%.desktop: %.desktop.in Makefile.am
	sed -e "s,\@bindir\@,$(EXPANDED_BINDIR)," < $< > $@

CLEANFILES= \
	at-spi-dbus-bus.desktop
