AT-SPI 3 roadmap
================

This document is a place to collect changes that we should consider for a
future version of the API.

-  Remove ATSPI_COORD_TYPE_SCREEN. It cannot be supported under Wayland.

-  Remove the legacy device event controller API. AtspiDevice should handle
   things related to the keyboard and mouse.

-  Eliminate the registry daemon. The new device API no longer uses it for
   key grabs (except for the existing legacy back end), and the mouse could
   similarly be handled in-process. Aside from the device event controller,
   the registry daemon currently keeps a list of applications and registerd
   events, but querying applications could be done with a DBus signal that
   applications could then respond to, and applications could similarly
   query consumers for a list of events that they care about.

-  The DBus API for sending events should be reviewed. TODO: flesh this out.

-  The AtspiEventListener API is hard to use and also needs to be reworked.
