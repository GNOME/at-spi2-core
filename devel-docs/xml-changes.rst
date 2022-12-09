Changes to the DBus XML interfaces
==================================

The `XML descriptions of DBus interfaces
<https://gitlab.gnome.org/GNOME/at-spi2-core/-/tree/main/xml>`_ and
the actual C code that implements them have gone out of sync in the
past.  During the process of `auditing the XML and C code
<https://gitlab.gnome.org/GNOME/at-spi2-core/-/issues/75>`_ to ensure
they match each other, changes have been needed in the XML
definitions.

The idea is that the C code nominally *works*, and we hope to make the
XML descriptions of the DBus interfaces actually match reality.  This
is so that other projects that consume those interfaces can assume
that the XML is correct and documented, instead of having to dig into
the accessibility code.

This document is a list of changes that we've made to the XML
interfaces since FIXME: date/version.  If you generate code based on
the XML, keep an eye on this document.

The changes here are about **method signatures**.  Changes to the
documentation are not listed, since they do not affect generated code.

* 9096b760 - 2022/12/09 - DeviceEventController.xml: fix the signature
  of the ``types`` argument in the ``RegisterKeystrokeListener``
  method; it's ``u`` now as it is a bitmask, not an array.

* 
