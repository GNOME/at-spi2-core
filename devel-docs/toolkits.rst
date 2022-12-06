Accessibility implementations in different toolkits
===================================================

Toolkits that use the DBus APIs directly
----------------------------------------

GTK4
~~~~

Sources:
```gtk4/gtk/a11y`` <https://gitlab.gnome.org/GNOME/gtk/-/tree/main/gtk/a11y>`__

Qt5
---

Sources:
```qtbase/src/gui/accessible/linux`` <https://github.com/qt/qtbase/tree/dev/src/gui/accessible/linux>`__

Note: Qt’s implementation of
`Cache.GetItems <https://github.com/qt/qtbase/blob/dev/src/gui/accessible/linux/qspi_struct_marshallers.cpp#L41-L71>`__
uses the old signature ``a((so)(so)(so)a(so)assusau)`` instead of the
preferred/new ``a((so)(so)(so)iiassusau)``.

WebKit
------

Sources:
```WebKit/Source/WebCore/accessibility/atspi`` <https://github.com/WebKit/WebKit/tree/main/Source/WebCore/accessibility/atspi>`__

Toolkits that use ATK
---------------------

GTK3
----

Sources:
```gtk3/gtk/a11y`` <https://gitlab.gnome.org/GNOME/gtk/-/tree/gtk-3-24/gtk/a11y>`__

gnome-shell / St / via clutter’s cally
--------------------------------------

Sources:
```mutter/clutter/clutter/cally`` <https://gitlab.gnome.org/GNOME/mutter/-/tree/main/clutter/clutter/cally>`__

Mozilla Firefox
---------------

Sources:
```gecko-dev/accessible/atk`` <https://github.com/mozilla/gecko-dev/tree/master/accessible/atk>`__

Chromium
--------

Uses both ATK and libatspi?

Sources: \*
```chromium/ui/accessibility/platform/*auralinux*`` <https://github.com/chromium/chromium/tree/main/ui/accessibility/platform>`__
(atk) \*
```chromium/ui/accessibility/platform/inspect/*auralinux*`` <https://github.com/chromium/chromium/tree/main/ui/accessibility/platform/inspect>`__
(atspi) \*
```chromium/content/browser/accessibility/*auralinux*`` <https://github.com/chromium/chromium/tree/main/content/browser/accessibility/>`__
(atspi and atk)

LibreOffice
-----------

Sources:
```LibreOffice/core/vcl/unx/gtk3/a11y`` <https://github.com/LibreOffice/core/tree/master/vcl/unx/gtk3/a11y>`__

Java Swing - via java-atk-wrapper
---------------------------------

Sources:
```java-atk-wrapper`` <https://gitlab.gnome.org/GNOME/java-atk-wrapper>`__
