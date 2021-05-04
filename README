D-Bus AT-SPI
------------

This version of at-spi is a major break from version 1.x.
It has been completely rewritten to use D-Bus rather than
ORBIT / CORBA for its transport protocol.

An outdated page including instructions for testing, project status and
TODO items is at:

        https://wiki.linuxfoundation.org/accessibility/atk/at-spi/at-spi_on_d-bus

The mailing list used for general questions is:

        https://lists.linuxfoundation.org/mailman/listinfo/accessibility-atspi

For bug reports, feature requests, patches or enhancements please use:

        https://gitlab.gnome.org/GNOME/at-spi2-core/

A git repository with the latest development code is available at:

	https://gitlab.gnome.org/GNOME/at-spi2-core/

More information
----------------

The project was started with a D-Bus performance review
the results of which are available on the GNOME wiki. Keep in
mind that the D-Bus AT-SPI design documents on this page
have not been kept up to date.

        https://wiki.gnome.org/Accessibility/Documentation/GNOME2/ATSPI2-Investigation

Other sources of relevant information about AT-SPI and Accessibility
include:

        https://wiki.gnome.org/Accessibility
        https://community.kde.org/Accessibility


Contents of this package
------------------------

This package includes the protocol definitions for the new D-Bus
at-spi.

Also included is the daemon necessary for forwarding device events
and registering accessible applicaitions.


Directory structure
-------------------

The directories within this package are arranged as follows:

    xml 

        This directory contains XML documents describing
        the D-Bus protocol in the format used for D-Bus introspection.

    idl

        The D-Bus specification in an idl-like format. This is likely not
        parseable by any existing tools, is not entirely up-to-date, and may
        by removed in a future release.

    registryd

        The registry daemon code. The registry daemon
        keeps a register of accessible applications and presents
        this to clients (ATs).
        It is also responsible for delivering device events.

    dbind

        Library to ease making D-Bus method calls, contains
        marshalling code to convert function arguments
        and a provided D-Bus signature into a D-Bus message.

        Used by libatspi.

    atspi

        C library for use by ATs. Wraps the various D-Bus calls, provides
        an interface for listening to events, and caches some information about
        accessible objects. Also contains some functions used by at-spi2-atk.

    bus

        A server that sits on the session bus and provides an interface
        allowing applications to find the accessibility bus daemon, launching
        it as needed. The accessibility bus is separate from the session bus
        because it may in fact span user sessions if a user, for instance,
        runs an application that escalates to run as root. The accessibility
        bus is thus tied to the X session rather than the D-Bus session.

    doc

        Contains infrastructure for creating libatspi documentation.

    test

        Contains files that may be useful for testing AT-SPI.

   m4

        Some macros used for building the module.

    po

        Infrastructure used for translation.
