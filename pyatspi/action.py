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
from factory import create_accessible, add_accessible_class

__all__ = [
	   "Action",
	  ]

#------------------------------------------------------------------------------

class Action(BaseProxy):
    """
    An interface through which a user-actionable user interface component
    can be manipulated. Components which react to mouse or keyboard
    input from the user, (with the exception of pure text entry fields
    with no other function), should implement this interface. Typical
    actions include "click", "press", "release" (for instance for
    buttons), "menu" (for objects which have context menus invokable
    from mouse or keyboard), "open" for icons representing files
    folders, and others.
    """
    
    def doAction(self, index):
        """
        doAction: 
        @param : index
        the 0-based index of the action to perform.
        Causes the object to perform the specified action.
        @return : a boolean indicating success or failure.
        """
        func = self.get_dbus_method("doAction")
        return func(index)
    
    def getDescription(self, index):
        """
        getDescription: 
        @param : index
        the index of the action for which a description is desired.
        Get the description of the specified action. The description
        of an action may provide information about the result of action
        invocation, unlike the action name. 
        @return : a string containing the description of the specified
        action.
        """
        func = self.get_dbus_method("getDescription")
        return func(index)
    
    def getKeyBinding(self, index):
        """
        getKeyBinding: 
        @param : index
        the 0-based index of the action for which a key binding is requested.
        Get the key binding associated with a specific action.
        @return : a string containing the key binding for the specified
        action, or an empty string ("") if none exists.
        """
        func = self.get_dbus_method("getKeyBinding")
        return func(index)
    
    def getName(self, index):
        """
        getName: 
        @param : index
        the index of the action whose name is requested.
        Get the name of the specified action. Action names generally
        describe the user action, i.e. "click" or "press", rather then
        the result of invoking the action.
        @return : a string containing the name of the specified action.
        """
        func = self.get_dbus_method("getName")
        return func(index)
    
    def get_nActions(self):
        return self._pgetter(self._dbus_interface, "nActions")
    def set_nActions(self, value):
        self._psetter(self._dbus_interface, "nActions", value)
    _nActionsDoc = \
        """
        nActions: a long containing the number of actions this object
        supports.
        """
    nActions = property(fget=get_nActions, fset=set_nActions, doc=_nActionsDoc)

# Register the Accessible class with the accessible factory.
add_accessible_class(interfaces.ATSPI_ACTION, Action)

#END----------------------------------------------------------------------------
