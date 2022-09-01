# Pytest will pick up this module automatically when running just "pytest".
#
# Each test_*() function gets passed test fixtures, which are defined
# in conftest.py.  So, a function "def test_foo(bar)" will get a bar()
# fixture created for it.

import pytest
import dbus

from utils import get_property, check_unknown_property_yields_error

APPLICATION_IFACE = 'org.a11y.atspi.Application'

def test_application_iface_properties(registry_root, session_manager):
    values = [
        ('ToolkitName',  'at-spi-registry'),
        ('Version',      '2.0'),
    ]

    for prop_name, expected in values:
        assert get_property(registry_root, APPLICATION_IFACE, prop_name) == expected

def test_unknown_property_yields_error(registry_root, session_manager):
    check_unknown_property_yields_error(registry_root, APPLICATION_IFACE)
