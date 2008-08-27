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
from base import BaseProxy, Enum
from factory import add_accessible_class

__all__ = [
	   "Selector",
	   "Command",
	   "CommandListener",
	  ]

#------------------------------------------------------------------------------

class Command(list):
    def __new__(cls, name, id):
        list.__new__(cls, (name, id))
    def __init__(self, name, id):
        list.__init__(self, (name, id))
    
    def _get_name(self):
        return self[0]
    def _set_name(self, val):
        self[0] = val
    name = property(fget=_get_name, fset=_set_name)
    def _get_id(self):
        return self[1]
    def _set_id(self, val):
        self[1] = val
    id = property(fget=_get_id, fset=_set_id)

#------------------------------------------------------------------------------

class CommandListener(BaseProxy):
    """
    An interface which should be implemented by assistive technologies
    or other clients of the Selector interface, over which notifications
    to the list of available commands is made. The notifyCommands()
    method of the client is then called by the Selector instance.
    """
    def notifyCommands(self, *args, **kwargs):
        """
        Notify the CommandListener instance of changes to the currently
        available commands, by sending the current CommandList.
        @param : commands
        The newly-available list of Command objects which may be invoked
        by the listener.
        """
        func = self.get_dbus_method("notifyCommands")
        return func(*args, **kwargs)

#------------------------------------------------------------------------------

class Selector(BaseProxy):
    """
    This interface is intended for use by assistive technologies
    and related user-agents. Via this interface, an assistive technology
    or user agent may expose a series of choices or selections in
    textual form, which can be activated on demand by a client of
    the Selector interface.
    Examples of the use of this interface include voice-command and
    remote-control applications, in which the user interaction is
    wholly or partly delegated by the implementor to an external
    agent.
    """
    
    def activateCommand(self, *args, **kwargs):
        """
        Request that the Selector invoke the specified Command. 
        @param : cmd
        the Command to activate/invoke. 
        @return a CommandResult indicating whether the request was honored,
        and the reason for failure if the Command could not be activated
        or invoked.
        """
        func = self.get_dbus_method("activateCommand")
        return func(*args, **kwargs)
    
    def deregisterChangeListener(self, *args, **kwargs):
        """
        Tell the Selector instance to cease notifying the specified CommandListener
        of changes to the command list. 
        @param : listener
        the CommandListener to remove from the notification list.
        """
        func = self.get_dbus_method("deregisterChangeListener")
        return func(*args, **kwargs)
    
    def getCommands(self, *args, **kwargs):
        """
        Query the Selector for the current CommandList.
        @return the currently available CommandList
        """
        func = self.get_dbus_method("getCommands")
        return func(*args, **kwargs)
    
    def refreshCommands(self, *args, **kwargs):
        """
        Ask the Selector to re-calculate its CommandList. 
        @return TRUE if the CommandList changed.
        """
        func = self.get_dbus_method("refreshCommands")
        return func(*args, **kwargs)
    
    def registerChangeListener(self, *args, **kwargs):
        """
        Register a :CommandListener instance for notification of changes
        to the command set. 
        @param : listener
        the CommandListener to be notified of changes.
        """
        func = self.get_dbus_method("registerChangeListener")
        return func(*args, **kwargs)
    
    def replaceCommands(self, *args, **kwargs):
        """
        @return TRUE if the replacement request was successful, FALSE
        if the request could not be honored.
        """
        func = self.get_dbus_method("replaceCommands")
        return func(*args, **kwargs)
    
    def get_supportsReplace(self):
        self._pgetter(self._dbus_interface, "supportsReplace")
    def set_supportsReplace(self, value):
        self._psetter(self._dbus_interface, "supportsReplace", value)
    _supportsReplaceDoc = \
        """
        This attribute is TRUE if this Selector allows its CommandList
        to be specified by the client
        """
    supportsReplace = property(fget=get_supportsReplace, fset=set_supportsReplace, doc=_supportsReplaceDoc)

    class CommandResult(Enum):
        """
        A code returned by a call to activateCommand, indicating the
        result of the activation request.
        """
        _enum_lookup = {
            0:'COMMAND_RESULT_INVALID',
            1:'COMMAND_RESULT_SUCCESS',
            2:'COMMAND_RESULT_FAILED',
            3:'COMMAND_RESULT_OBSOLETE',
            4:'COMMAND_RESULT_LAST_DEFINED',
        }
    
    COMMAND_RESULT_FAILED = CommandResult(2)
    
    COMMAND_RESULT_INVALID = CommandResult(0)
    
    COMMAND_RESULT_LAST_DEFINED = CommandResult(4)
    
    COMMAND_RESULT_OBSOLETE = CommandResult(3)
    
    COMMAND_RESULT_SUCCESS = CommandResult(1)

# ATTENTION - Register the Application class with the accessible factory.
add_accessible_class(interfaces.ATSPI_SELECTOR, Selector)

#END----------------------------------------------------------------------------
