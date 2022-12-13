Changes to the DBus XML interfaces
==================================

The `XML descriptions of DBus interfaces
<https://gitlab.gnome.org/GNOME/at-spi2-core/-/tree/main/xml>`_ and
the actual C code that implements them have gone out of sync in the
past.  During the process of `auditing the XML and C code
<https://gitlab.gnome.org/GNOME/at-spi2-core/-/issues/75>`_ to ensure
they match each other, changes have been needed in the XML
definitions.

**Assumptions for toolkits:** Regretfully, both the in-tree XML files
and the introspected XML you get from DBus have been out of sync with
the C code.  We are fixing this so that they always stay in sync:

* Make the XML files the single source of truth...

* By making sure the XML matches the C code, which is the ground truth
  before this auditing project began...

* And by making sure that the introspected XML matches the XML files
  in-tree.

* Along the way, document all the XML interfaces properly.

This document is a list of changes that we've made to the XML
interfaces since 2022/Jun (starting with version 2.45.90).  If you
generate code based on the XML, keep an eye on this document.

The changes here are about **method signatures**.  Changes to the
documentation are not listed, since they do not affect generated code.
The changes are listed in reverse chronological order (newest first).

* add8abce - 2022/12/09 - ``DeviceEventListener.xml`` - Remove unused signals
  ``DeviceListenerRegistered`` and ``DeviceListenerDeregistered``.

* 9096b760 - 2022/12/09 - ``DeviceEventController.xml`` -  fix the signature
  of the ``types`` argument in the ``RegisterKeystrokeListener``
  method; it's ``u`` now as it is a bitmask, not an array.

* 55934a59 - 2022/12/05 - ``DeviceEventController.xml`` - remove unused method
  ``DeregisterDeviceEventListener``.

* 9b96a257 - 2022/12/05 - ``DeviceEventController.xml`` - remove unused method
  ``RegisterDeviceEventListener``.

* cb10a4c9 - 2022/12/05 - ``DeviceEventController.xml`` - remove unused method
  ``GetDeviceEventListeners``.

* f0781465 - 2022/08/26 - ``Event.xml`` - Fix the signature of all
  signals.  See https://gitlab.gnome.org/GNOME/at-spi2-core/-/issues/74

* 01d624b1 - 2022/08/11 - ``Application.xml`` - document the ``GetLocale`` method as not
  used.

* 26835da29 - 2022/08/04 - ``Event.xml`` - Add an ``Announcement`` signal to the
  ``org.a11y.atspi.Event.Object`` interface.

* 0412902f - 2022/07/01 - ``DeviceEventListener.xml``: add missing signals
  ``KeystrokeListenerRegistered``, ``KeystrokeListenerDeregistered``

* ae6eb3bd - 2022/07/01 - ``DeviceEventController.xml``: add missing
  methods ``GetKeystrokeListeners``.
