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
