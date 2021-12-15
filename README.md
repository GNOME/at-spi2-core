# Assistive Technology Service Provider Interface (AT-SPI)

This repository contains the [DBus][DBus] interface definitions for AT-SPI, the Assistive
Technology Service Provider Interface — the core of an accessibility stack for free
software systems.  It also contains the basic daemons of the accessibility stack.

The version control repository and bug tracker are at https://gitlab.gnome.org/GNOME/at-spi2-core/

The code in this repository is not intended for application programmers.  To write
accessible applications, look into [ATK][ATK] or your programming language's bindings for
the `xml` DBus interfaces mentioned below.

While this module started within the [GNOME][GNOME] project's umbrella, it is not used
only in GNOME.  Other sources of relevant information about AT-SPI and Accessibility
include:

* [GNOME Accessibility wiki][gnome-a11y-wiki]
* [KDE Accessibility wiki][kde-a11y-wiki]
* [Accessibility documentation for GNOME users][docs-users]


## Summary of this repository's contents

* `xml` - DBus interfaces for accessibility, described in DBus's XML introspection format.
  Ideally, your programming language's implementation of DBus makes use of these files to
  generate callable bindings.
  
* `bus` - Launcher for the session's accessibility bus; see its [README.md](bus/README.md)
  for details.

* `registryd` - Daemon that keeps track of accessible applications in the user's session,
  and lets them talk to each other and to assistive technologies (ATs) like screen
  readers.

* `atspi` - Hand-written binding for the `xml` DBus interface above, for use from C with
  [GObject][GObject].  This is not normally what you would use; use a language-specific
  binding instead.  This module is for use mainly by [`at-spi2-atk`][at-spi2-atk].

* `dbind` - DBus utilities for use by `atspi` above.  `atspi` was written before the more
  modern C bindings like [GDBusConnection][GDBus] were available, so there is a lot of
  hand-written IPC here.

* Documentation for the Gitlab [Continuous Integration pipeline](devel-docs/gitlab-ci.md).

## Historical note

Versions 1.x of AT-SPI were based on [CORBA][CORBA] for inter-process communication (IPC),
using GNOME's ORBit implementation thereof.  During the GNOME 2 and 3 release series,
CORBA was phased out in favor of [DBus][DBus], a more modern IPC mechanism.

The original CORBA interfaces for AT-SPI were based on Java's implementation of
accessibility.  Later, these CORBA interfaces were translated to DBus.  This is why the
interfaces sometimes have a 1990s feeling to them.

The project was started with a D-Bus performance review, the results of which are available
on the GNOME wiki. Keep in mind that the D-Bus AT-SPI design documents on this page have
not been kept up to date.

        https://wiki.gnome.org/Accessibility/Documentation/GNOME2/ATSPI2-Investigation


[CORBA]: https://en.wikipedia.org/wiki/Common_Object_Request_Broker_Architecture
[DBus]: https://www.freedesktop.org/wiki/Software/dbus/
[GObject]: https://docs.gtk.org/gobject/
[at-spi2-atk]: https://gitlab.gnome.org/GNOME/at-spi2-atk
[GDBus]: https://docs.gtk.org/gio/class.DBusConnection.html
[ATK]: https://gitlab.gnome.org/GNOME/atk/
[GNOME]: https://www.gnome.org
[docs-users]: https://help.gnome.org/users/gnome-help/stable/a11y.html
[gnome-a11y-wiki]: https://wiki.gnome.org/Accessibility
[kde-a11y-wiki]: https://community.kde.org/Accessibility
