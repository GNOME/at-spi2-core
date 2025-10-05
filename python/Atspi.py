# AT-SPI - Assistive Technology Service Provider Interface
#
# Copyright 2025 SUSE LLC.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.

from ..overrides import override
from ..module import get_introspection_module

Atspi = get_introspection_module('Atspi')

__all__ = []


class Accessible(Atspi.Accessible):

    def __bool__(self):
        return True

    def __eq__(a, b):
        try:
            return a.app.bus_name == b.app.bus_name and a.path == b.path
        except:
            return False

    def __getitem__(self, i):
        len = self.get_child_count()
        if i < 0:
                i = self + i
        if i < 0 or i >= len:
                raise IndexError
        return self.get_child_at_index(i)

    def __hash__(self):
        if hasattr(self, "_hashval"):
            return self._hashval
        if self.app is not None:
            self._hashval = hash(self.app.bus_name + self.path)
        else:
            self._hashval = super.__hash__(self)
        return self._hashval

    def __len__(self):
        try:
            count = self.get_child_count()
        except:
            return 0
        if count < 0:
            return 0
        return count

    def __ne__(a, b):
        try:
            return a.app.bus_name != b.app.bus_name or a.path != b.path
        except:
            return True

    def __nonzero__(self):
        return True

Accessible = override(Accessible)
__all__.append('Accessible')
