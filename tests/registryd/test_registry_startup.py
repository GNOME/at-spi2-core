PROPERTIES_IFACE = 'org.freedesktop.DBus.Properties'
ACCESSIBLE_IFACE = 'org.a11y.atspi.Accessible'

def test_accessible_iface_properties(registry, session_manager):
    val = registry.Get(ACCESSIBLE_IFACE, 'Name', dbus_interface=PROPERTIES_IFACE)
    assert str(val) == 'main'
