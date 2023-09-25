AT-SPI 3 roadmap
================

Note: this document may be obsolete. See also new-protocol.rst.

This document is a place to collect changes that we should consider for a
future version of the API.

- Remove ATSPI_COORD_TYPE_SCREEN. It cannot be supported under Wayland.

- Remove the legacy device event controller API. AtspiDevice should handle
  things related to the keyboard and mouse.

- Eliminate the registry daemon. The new device API no longer uses it for
  key grabs (except for the existing legacy back end), and the mouse could
  similarly be handled in-process. Aside from the device event controller,
  the registry daemon currently keeps a list of applications and registerd
  events, but querying applications could be done with a DBus signal that
  applications could then respond to, and applications could similarly
  query consumers for a list of events that they care about.

- The DBus API for sending events should be reviewed. All events in the
  `xml/Event.xml` file return items with the same "siiva{sv}" signature
  when it is not necessary. Take for example "StateChanged", which only
  uses the first two variables: a string of the state which has been
  changed, and a signed integer that may be either 1 or 0 depending on
  if the state has been added or removed. For one thing, it is clear that
  the second argument for "StateChanged" should be "b" for bool, and
  secondly the arguments "iva{sv}" do not seem to be used at all for this
  event, and subsequently could be dropped. This is only the example of
  one event with issues like this. Nearly every event has a similar case
  of some unused fields, or fields that don't *exactly* match the
  semantics of their type sent over DBus.

- ChildCount and GetChildAtIndex could have unsigned arguments (#106).

- For the Collection interface, roles are signed, but they are unsigned
  elsewhere (#108).

- For the Collection interface, states are signed, but they are unsigned
  elsewhere (#109).

- The AtspiEventListener API is hard to use and also needs to be reworked.
