# Pytest will pick up this module automatically when running just "pytest".
#
# Each test_*() function gets passed test fixtures, which are defined
# in conftest.py.  So, a function "def test_foo(bar)" will get a bar()
# fixture created for it.

import pytest
import dbus

from utils import get_property, check_unknown_property_yields_error

ACCESSIBLE_IFACE = 'org.a11y.atspi.Accessible'

ATSPI_ROLE_DESKTOP_FRAME = 14 # see atspi-constants.h

def test_accessible_iface_properties(registry_root, session_manager):
    values = [
        ('Name',        'main'),
        ('Description', ''),
        ('Parent',      ('', '/org/a11y/atspi/null')),
        ('ChildCount',  0),
    ]

    for prop_name, expected in values:
        assert get_property(registry_root, ACCESSIBLE_IFACE, prop_name) == expected

def test_unknown_property_yields_error(registry_root, session_manager):
    check_unknown_property_yields_error(registry_root, ACCESSIBLE_IFACE)

def test_root_get_interfaces(registry_root, session_manager):
    ifaces = registry_root.GetInterfaces(dbus_interface=ACCESSIBLE_IFACE)
    assert ifaces.signature == 's'
    assert 'org.a11y.atspi.Accessible' in ifaces
    assert 'org.a11y.atspi.Application' in ifaces
    assert 'org.a11y.atspi.Component' in ifaces
    assert 'org.a11y.atspi.Socket' in ifaces

def test_root_get_index_in_parent(registry_root, session_manager):
    # The registry root is always index 0
    assert registry_root.GetIndexInParent(dbus_interface=ACCESSIBLE_IFACE) == 0

def test_root_get_relation_set(registry_root, session_manager):
    # The registry root has an empty relation set
    assert len(registry_root.GetRelationSet(dbus_interface=ACCESSIBLE_IFACE)) == 0

def test_root_get_role(registry_root, session_manager):
    # Hardcoded to ATSPI_ROLE_DESKTOP_FRAME
    assert registry_root.GetRole(dbus_interface=ACCESSIBLE_IFACE) == ATSPI_ROLE_DESKTOP_FRAME

