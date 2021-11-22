# Launcher for the accessibility bus

The communications mechanism for accessibility does not run through the user's session
DBus; it runs in a separate bus just for accessibility purposes.  The accessibility
interfaces for DBus are very chatty; using a separate bus prevents the main session bus
from getting too much traffic.

Throughout this document we will distinguish between the **session bus** and the
**accessibility bus**.

## Who launches the accessibility bus?

This source directory `bus` contains a little daemon, `at-spi-bus-launcher`, which
launches the **accessibility bus** and manages its lifetime according to the user's
session.

The **accessibility bus** is just a separate instance of `dbus-daemon` or equivalent, like
`dbus-broker`.  That bus allows communication using the accessibility interfaces defined
in the `xml` directory in this repository.  It also has the accessibility registry —
`registryd` in this repository, which claims the name `org.a11y.atspi.Registry` in that
bus.

## When does the accessibility bus get launched?

When a normal application starts up, it will want to find the **accesibility bus**, and
then contact the accessibility registry in that bus (`registryd` in this repository) to
inform the world that they are up and running.  Finding the accessibility bus can then be
done on demand for normal applications, via the `GetAddress` method described below.

However, a screen reader is special: it needs to start up automatically during login, and
immediatelly tell the accessibility registry (... via the **accessibility bus**) that it
is running.  If you need a screen reader to use your computer, you cannot easily launch it
by hand if there is no screen reader already running!

That is, if a screen reader is turned on — and we assume it will start up turned on for
future sessions — we need to launch the **accessibility bus** unconditionally, not on
demand, at session startup.  This is why `at-spi-dbus-bus.desktop`, described below, is an
[XDG autostart][xdg-autostart] file which runs `at-spi-bus-launcher --launch-immediately`,
but only if a certain GSettings key is turned on.

In summary, `at-spi-bus-launcher` will launch the **accessibility bus** under two situations:

* On demand via the `GetAddress` method; see below.

* Shortly after `at-spi-bus-launcher` starts up, if the gsettings key
  `org.gnome.desktop.interface toolkit-accessibility` is set to true, due to the
  `at-spi-dbus-bus.desktop` XDG autostart file.

* The gsettings key `org.gnome.desktop.a11y.applications screen-reader-enabled` is set to true.

## Contents of this `bus` directory

This `bus` source directory has a configuration file for the `dbus-daemon` which will run
as the **accessibility bus**, and a helper daemon called `at-spi-bus-launcher`, which actually
starts that bus and makes its address visible to the user's session.  The files are as follows:

* `accessibility.conf.in` - template for the configuration for the accessibility bus,
  which gets installed in `$(datadir)/defaults/at-spi2/accessibility.conf`.

* `at-spi-bus-launcher.c` - See [`at-spi-bus-launcher`](#at-spi-bus-launcher) below.

* `at-spi-dbus-bus.service.in` - template for a systemd user service to start `at-spi-bus-launcher`.

* `org.a11y.Bus.service.in` - template for a DBus user service to start `at-spi-bus-launcher`.

* `at-spi-dbus-bus.desktop.in` - template for a XDG autostart file to start
  `at-spi-bus-launcher` at session startup, only if the `org.gnome.desktop.interface
  toolkit-accessibility` GSettings key is turned on.

* `00-at-spi` - script to set the `AT_SPI_BUS` property on the X root window, for
  Wayland-based systems where XWayland is started on demand.  That X window property is an
  alternative way of finding the address of the **accessibility bus**.

## at-spi-bus-launcher

This is a tiny daemon which registers a service in the normal **session bus**, and which
can then be used to query the address of the actual **accessibility bus**.  The daemon
claims ownership of the `org.a11y.Bus` name in the **session bus**, and exports a
single object, `/org/a11y/bus`, with two interfaces:

* `org.a11y.Bus` - has a single `GetAddress` method, which returns the address of the
  actual **accessibility bus**.  Accessibility clients must use this address when creating
  their initial DBus connection.

* `org.a11y.Status` - has properties to query whether the **accessibility bus** is enabled
  and whether a screen reader is running.

`at-spi-bus-launcher` starts the separate `dbus-daemon` (or `dbus-broker` equivalent) for
the **accessibility bus** on demand.  The following actions can cause it to launch:

* Calling the `GetAddress` method.  The daemon gets launched and queried for its address;
  the method returns that.  This is the normal way to start the accessibility bus.

* If `at-spi-bus-launcher` was run with the `--launch-immediately` option, the
  accessibility bus launches as soon as `at-spi-bus-launcher` is able to claim ownership
  of the `org.a11y.Bus` name in the session bus.  This is intended for session startup.

[xdg-autostart]: https://specifications.freedesktop.org/autostart-spec/autostart-spec-latest.html
