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

import interfaces
from base import BaseProxy
from factory import add_accessible_class

__all__ = [
           "Selection",
          ]

#------------------------------------------------------------------------------

class Selection(BaseProxy):
        """
        An interface which indicates that an object exposes a 'selection'
        model, allowing the selection of one or more of its children.
        Read-only Selection instances are possible, in which case the
        interface is used to programmatically determine the selected-ness
        of its children. A selected child has State::STATE_SELECTED,
        and a child which may hypothetically be selected (though possibly
        not programmatically selectable) has State::STATE_SELECTABLE.
        """

        def clearSelection(self, *args, **kwargs):
                """
                Attempt to clear all selections (i.e. deselect all children)
                of a Selection. Not all Selection implementations allow the removal
                of all selections.
                @return True if the selections were successfully cleared, False
                otherwise.
                """
                func = self.get_dbus_method("clearSelection")
                return func(*args, **kwargs)

        def deselectChild(self, *args, **kwargs):
                """
                Remove a child from the selected children list of a Selection,
                if the child is currently selected.
                @param : childIndex
                a long integer (the zero offset index into the Accessible object's
                list of children) indicating which child of the Selection is
                to be selected.
                @return True if the child was successfully selected, False otherwise.
                """
                func = self.get_dbus_method("deselectChild")
                return func(*args, **kwargs)

        def deselectSelectedChild(self, *args, **kwargs):
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
                func = self.get_dbus_method("deselectSelectedChild")
                return func(*args, **kwargs)

        def getSelectedChild(self, *args, **kwargs):
                """
                Get the i-th selected Accessible child of a Selection. 
                @param : selectedChildIndex
                a long integer indicating which of the selected children of an
                object is being requested. 
                @return a pointer to a selected Accessible child object, specified
                by selectedChildIndex.
                """
                func = self.get_dbus_method("getSelectedChild")
                return func(*args, **kwargs)

        def isChildSelected(self, *args, **kwargs):
                """
                Determine whether a particular child of an Selection implementor
                is currently selected. Note that childIndex is the zero-offset
                index into the standard Accessible container's list of children.
                @param : childIndex
                an index into the Selection's list of children.
                @return True if the specified child is currently selected, False
                otherwise.
                """
                func = self.get_dbus_method("isChildSelected")
                return func(*args, **kwargs)

        def selectAll(self, *args, **kwargs):
                """
                Attempt to select all of the children of a Selection implementor.
                Not all Selection implementors support this operation (for instance,
                implementations which support only "single selection" do not
                support this operation).
                @return True if successful, False otherwise.
                """
                func = self.get_dbus_method("selectAll")
                return func(*args, **kwargs)

        def selectChild(self, *args, **kwargs):
                """
                Add a child to the selected children list of a Selection. 
                @param : childIndex
                a long integer indicating which child of the Selection is to
                be selected.
                @return True if the child was successfully selected, False otherwise.
                """
                func = self.get_dbus_method("selectChild")
                return func(*args, **kwargs)

        def get_nSelectedChildren(self):
                return self._pgetter(self._dbus_interface, "nSelectedChildren")
        def set_nSelectedChildren(self, value):
                self._psetter(self._dbus_interface, "nSelectedChildren", value)
        _nSelectedChildrenDoc = \
                """
                The number of children of a Selection implementor which are currently
                selected.
                """
        nSelectedChildren = property(fget=get_nSelectedChildren, fset=set_nSelectedChildren, doc=_nSelectedChildrenDoc)

# ATTENTION - Register the Application class with the accessible factory.
add_accessible_class(interfaces.ATSPI_SELECTION, Selection)

#END----------------------------------------------------------------------------
