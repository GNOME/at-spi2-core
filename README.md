# AT-SPI2-ATK - implementation of the ATK interfaces in terms of the libatspi2 API

What is at-spi2-atk?  First let's introduce two of its related modules:

[ATK] is the Accessibility Toolkit, a set of GObject interfaces that can
be implemented to communicate with assistive technologies (ATs).

[at-spi2-core] is the Assistive Technology Service Provider Interface, which
provides two things: a set of DBus interfaces for accessibility, and a
C language binding to use those interfaces.

Applications that provide accessibility through the ATK interfaces
need a way to translate those interfaces to AT-SPI2 DBus calls.  This
module, **at-spi2-atk**, provides that translation bridge.  Several
things use at-spi2-atk:

* GTK3 calls ATK directly and assumes that at-spi2-atk is backing it.

* GTK2 loads a module at runtime to do the same (`atk-adaptor/gtk-2.0` in this module).

* Chromium.

* Gnome-shell.

* Any applications that use ATK directly, like Mozilla Firefox, will
  need at-spi2-atk backing them.
  
The version control repository and bug tracker are at https://gitlab.gnome.org/GNOME/at-spi2-atk

## Summary of this repository's contents

* `atk-adaptor` - Bridges at-spi and the ATK APIs.  GTK3 and earlier,
and applications that use ATK like gnome-shell and Chromium, do not
use the at-spi DBus interfaces directly, so they go through ATK, then
libatk-bridge (contained here), libatspi, and then finally DBus to the
accessibility registry daemon.  Yes, this is too much layering!  In contrast,
GTK4 talks the at-spi DBus interface directly to the registry.

* `tests` - End-to-end tests between a mock user of ATK and a mock
assistive technology (AT).  The tests simulate communication between a
real ATK-enabled application and an AT like a screen reader, via the
registry daemon.


* `droute` - Utilities for registering objects with a D-Bus connection
and for routing messages to the implementing object.


[ATK]: https://gitlab.gnome.org/GNOME/atk/
[at-spi2-core]: https://gitlab.gnome.org/GNOME/at-spi2-core/

