SUBDIRS=droute atk-adaptor

if ATSPI_TESTS

SUBDIRS += tests

endif

gtk_modulesdir = $(libdir)/gnome-settings-daemon-3.0/gtk-modules/
gtk_modules_DATA = at-spi2-atk.desktop

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = atk-bridge-2.0.pc

EXTRA_DIST = $(gtk_modules_DATA) atk-bridge-2.0.pc.in \
	meson.build \
	meson_options.txt

DISTCHECK_CONFIGURE_FLAGS = --with-tests=yes
