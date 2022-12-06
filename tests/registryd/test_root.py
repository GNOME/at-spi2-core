import pytest
import dbus

from utils import check_unknown_property_yields_error

def test_root_property_on_invalid_interface(registry_root, session_manager):
    check_unknown_property_yields_error(registry_root, 'this.is.not.a.supported.interface')
