import pytest
import dbus
import xml.etree.ElementTree as ElementTree

INTROSPECTABLE_IFACE = 'org.freedesktop.DBus.Introspectable'

def test_introspect(registry_root, session_manager):
    xml_str = str(registry_root.Introspect(dbus_interface=INTROSPECTABLE_IFACE))
    root = ElementTree.fromstring(xml_str)
    assert root.tag == 'node'

    interface_elements = root.findall('./interface')
    iface_names = map(lambda e: e.attrib['name'], interface_elements)
    iface_names = list(iface_names)
    iface_names.sort()

    assert iface_names == ['org.a11y.atspi.Accessible',
                           'org.a11y.atspi.Component',
                           'org.a11y.atspi.Socket',
                           ]
