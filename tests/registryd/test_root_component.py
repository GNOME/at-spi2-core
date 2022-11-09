import pytest
import dbus

COMPONENT_IFACE = 'org.a11y.atspi.Component'

COORD_TYPE_WINDOW = 1
LAYER_WIDGET = 3

def test_contains(registry_root, session_manager):
    assert registry_root.Contains(0, 0, COORD_TYPE_WINDOW, dbus_interface=COMPONENT_IFACE) == False

def test_get_accessible_at_point(registry_root, session_manager):
    (name, path) = registry_root.GetAccessibleAtPoint(0, 0, COORD_TYPE_WINDOW, dbus_interface=COMPONENT_IFACE)
    assert path == '/org/a11y/atspi/null'

def test_get_extents(registry_root, session_manager):
    assert registry_root.GetExtents(COORD_TYPE_WINDOW, dbus_interface=COMPONENT_IFACE) == (0, 0, 1024, 768)

def test_get_position(registry_root, session_manager):
    assert registry_root.GetPosition(COORD_TYPE_WINDOW, dbus_interface=COMPONENT_IFACE) == (0, 0)

def test_get_size(registry_root, session_manager):
    assert registry_root.GetSize(dbus_interface=COMPONENT_IFACE) == (1024, 768)

def test_get_layer(registry_root, session_manager):
    assert registry_root.GetLayer(dbus_interface=COMPONENT_IFACE) == LAYER_WIDGET

def test_get_mdi_z_order(registry_root, session_manager):
    assert registry_root.GetMDIZOrder(dbus_interface=COMPONENT_IFACE) == 0

def test_grab_focus(registry_root, session_manager):
    assert registry_root.GrabFocus(dbus_interface=COMPONENT_IFACE) == False

def test_get_alpha(registry_root, session_manager):
    assert registry_root.GetAlpha(dbus_interface=COMPONENT_IFACE) == 1.0
