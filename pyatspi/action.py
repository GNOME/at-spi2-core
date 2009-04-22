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
           "Action",
          ]

#------------------------------------------------------------------------------

class Action(Accessible):
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

        def getActions(self):
                """
                getActions:
                Retrieves all the actions at once.  
                @return : an array of an array of strings in the form
                [[name, description, keybinding], ...]
                """
                func = self.get_dbus_method("getActions", dbus_interface=ATSPI_ACTION)
                return func()

        def doAction(self, index):
                """
                doAction: 
                @param : index
                the 0-based index of the action to perform.
                Causes the object to perform the specified action.
                @return : a boolean indicating success or failure.
                """
                func = self.get_dbus_method("doAction", dbus_interface=ATSPI_ACTION)
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
                func = self.get_dbus_method("getDescription", dbus_interface=ATSPI_ACTION)
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
                func = self.get_dbus_method("getKeyBinding", dbus_interface=ATSPI_ACTION)
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
                func = self.get_dbus_method("getName", dbus_interface=ATSPI_ACTION)
                return func(index)

        def get_nActions(self):
                return dbus.Int32(self._pgetter(self._dbus_interface, "nActions"))
        _nActionsDoc = \
                """
                nActions: a long containing the number of actions this object
                supports.
                """
        nActions = property(fget=get_nActions, doc=_nActionsDoc)

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(ATSPI_ACTION, Action)

#END----------------------------------------------------------------------------
