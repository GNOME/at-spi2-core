import pytest
import dbus
import xml.etree.ElementTree as ElementTree

INTROSPECTABLE_IFACE = 'org.freedesktop.DBus.Introspectable'

# obj: a dbus proxy object to introspect
# expected_ifaces: sequence of interface names as strings
def check_object_supports_interfaces(obj, expected_ifaces):
    xml_str = str(obj.Introspect(dbus_interface=INTROSPECTABLE_IFACE))
    root = ElementTree.fromstring(xml_str)
    assert root.tag == 'node'

    interface_elements = root.findall('./interface')
    iface_names = map(lambda e: e.attrib['name'], interface_elements)
    iface_names = list(iface_names)
    iface_names.sort()

    expected_ifaces.sort()

    assert iface_names == expected_ifaces

# Test that the root object at /org/a11y/atspi/accessible/root advertises the correct interfaces
def test_introspect_root(registry_root, session_manager):
    check_object_supports_interfaces(
        registry_root,
        ['org.a11y.atspi.Accessible',
         'org.a11y.atspi.Component',
         'org.a11y.atspi.Socket',
         ]
    )

# Test that the registry object at /org/a11y/atspi/registry advertises the correct interfaces
def test_introspect_registry(registry_registry, session_manager):
    check_object_supports_interfaces(
        registry_registry,
        ['org.a11y.atspi.Registry']
    )
