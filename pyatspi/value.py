#Copyright (C) 2008 Codethink Ltd

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License version 2 as published by the Free Software Foundation.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU Lesser General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

from interfaces import *
from accessible import Accessible
from factory import accessible_factory

import dbus

__all__ = [
           "Value",
          ]

#------------------------------------------------------------------------------

class Value(Accessible):
        """
        An interface supporting controls which allow a one-dimensional,
        scalar quantity to be modified or which reflect a scalar quantity.
        (If STATE_EDITABLE is not present, the valuator is treated as
        "read only".
        """

        def get_currentValue(self):
                return dbus.Double(self._pgetter(self._dbus_interface, "currentValue"))
        def set_currentValue(self, value):
                self._psetter(self._dbus_interface, "currentValue", dbus.Double(value, variant_level=1))
        _currentValueDoc = \
                """
                The current value of the valuator.
                """
        currentValue = property(fget=get_currentValue, fset=set_currentValue, doc=_currentValueDoc)

        def get_maximumValue(self):
                return dbus.Double(self._pgetter(self._dbus_interface, "maximumValue"))
        _maximumValueDoc = \
                """
                The maximum value allowed by this valuator.
                """
        maximumValue = property(fget=get_maximumValue, doc=_maximumValueDoc)

        def get_minimumIncrement(self):
                return dbus.Double(self._pgetter(self._dbus_interface, "minimumIncrement"))
        _minimumIncrementDoc = \
                """
                The smallest incremental change which this valuator allows. If
                0, the incremental changes to the valuator are limited only by
                the precision of a double precision value on the platform.
                """
        minimumIncrement = property(fget=get_minimumIncrement, doc=_minimumIncrementDoc)

        def get_minimumValue(self):
                return dbus.Double(self._pgetter(self._dbus_interface, "minimumValue"))
        _minimumValueDoc = \
                """
                The minimum value allowed by this valuator.
                """
        minimumValue = property(fget=get_minimumValue, doc=_minimumValueDoc)

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(ATSPI_VALUE, Value)

#END----------------------------------------------------------------------------
