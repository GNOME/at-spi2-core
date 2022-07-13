# Pytest will pick up this module automatically when running just "pytest".
#
# Each test_*() function gets passed test fixtures, which are defined
# in conftest.py.  So, a function "def test_foo(bar)" will get a bar()
# fixture created for it.

PROPERTIES_IFACE = 'org.freedesktop.DBus.Properties'
ACCESSIBLE_IFACE = 'org.a11y.atspi.Accessible'

def get_property(proxy, iface_name, prop_name):
    return proxy.Get(iface_name, prop_name, dbus_interface=PROPERTIES_IFACE)

def test_accessible_iface_properties(registry, session_manager):
    values = [
        ('Name',        'main'),
        ('Description', ''),
    ]

    for prop_name, expected in values:
        assert get_property(registry, ACCESSIBLE_IFACE, prop_name) == expected

def test_registry_root_has_null_parent(registry, session_manager):
    assert get_property(registry, ACCESSIBLE_IFACE, 'Parent') == ('', '/org/a11y/atspi/null')

def test_empty_registry_has_zero_children(registry, session_manager):
    assert get_property(registry, ACCESSIBLE_IFACE, 'ChildCount') == 0
