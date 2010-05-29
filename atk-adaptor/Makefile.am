SUBDIRS=adaptors gtk-2.0 gtk-3.0

if RELOCATE
    gtkpathdir = $(libdir)/gtk-2.0/modules/at-spi-dbus
gnomeautostartdir = $(datadir)/gnome/autostart
gnomeautostart_DATA = atk-bridge.desktop
endif

EXTRA_DIST = atk-bridge.desktop.in \
	Makefile.include

CLEANFILES = atk-bridge.desktop

%.desktop: %.desktop.in Makefile.am
	sed -e "s,\@gtkpathdir\@,$(gtkpathdir)," \
	< $< > $@
