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
           "Selection",
          ]

#------------------------------------------------------------------------------

class Selection(Accessible):
        """
        An interface which indicates that an object exposes a 'selection'
        model, allowing the selection of one or more of its children.
        Read-only Selection instances are possible, in which case the
        interface is used to programmatically determine the selected-ness
        of its children. A selected child has State::STATE_SELECTED,
        and a child which may hypothetically be selected (though possibly
        not programmatically selectable) has State::STATE_SELECTABLE.
        """

        def clearSelection(self):
                """
                Attempt to clear all selections (i.e. deselect all children)
                of a Selection. Not all Selection implementations allow the removal
                of all selections.
                @return True if the selections were successfully cleared, False
                otherwise.
                """
                func = self.get_dbus_method("clearSelection", dbus_interface=ATSPI_SELECTION)
                return func()

        def deselectChild(self, childIndex):
                """
                Remove a child from the selected children list of a Selection,
                if the child is currently selected.
                @param : childIndex
                a long integer (the zero offset index into the Accessible object's
                list of children) indicating which child of the Selection is
                to be selected.
                @return True if the child was successfully selected, False otherwise.
                """
                func = self.get_dbus_method("deselectChild", dbus_interface=ATSPI_SELECTION)
                return func(childIndex)

        def deselectSelectedChild(self, index):
                """
                Remove a child to the selected children list of a Selection.
                @param : selectedChildIndex
                a long integer indicating which of the selected children of the
                Selection is to be deselected. The index is a zero-offset index
                into the 'selected child list', not a zero-offset index into
                the list of all children of the Selection.
                @return True if the child was successfully deselected, False
                otherwise.
                """
                func = self.get_dbus_method("deselectSelectedChild", dbus_interface=ATSPI_SELECTION)
                return func(index)

        def getSelectedChild(self, index):
                """
                Get the i-th selected Accessible child of a Selection. 
                @param : selectedChildIndex
                a long integer indicating which of the selected children of an
                object is being requested. 
                @return a pointer to a selected Accessible child object, specified
                by selectedChildIndex.
                """
                func = self.get_dbus_method("getSelectedChild", dbus_interface=ATSPI_SELECTION)
                return self._cache.create_accessible(self._app_name, func(index),
                                                     interfaces.ATSPI_ACCESSIBLE)

        def isChildSelected(self, index):
                """
                Determine whether a particular child of an Selection implementor
                is currently selected. Note that childIndex is the zero-offset
                index into the standard Accessible container's list of children.
                @param : childIndex
                an index into the Selection's list of children.
                @return True if the specified child is currently selected, False
                otherwise.
                """
                func = self.get_dbus_method("isChildSelected", dbus_interface=ATSPI_SELECTION)
                return func(index)

        def selectAll(self):
                """
                Attempt to select all of the children of a Selection implementor.
                Not all Selection implementors support this operation (for instance,
                implementations which support only "single selection" do not
                support this operation).
                @return True if successful, False otherwise.
                """
                func = self.get_dbus_method("selectAll", dbus_interface=ATSPI_SELECTION)
                return func()

        def selectChild(self, index):
                """
                Add a child to the selected children list of a Selection. 
                @param : childIndex
                a long integer indicating which child of the Selection is to
                be selected.
                @return True if the child was successfully selected, False otherwise.
                """
                func = self.get_dbus_method("selectChild", dbus_interface=ATSPI_SELECTION)
                return func(index)

        def get_nSelectedChildren(self):
                return dbus.Int32(self._pgetter(self._dbus_interface, "nSelectedChildren"))
        _nSelectedChildrenDoc = \
                """
                The number of children of a Selection implementor which are currently
                selected.
                """
        nSelectedChildren = property(fget=get_nSelectedChildren, doc=_nSelectedChildrenDoc)

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(ATSPI_SELECTION, Selection)

#END----------------------------------------------------------------------------
