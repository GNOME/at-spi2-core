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
from base import Enum
from accessible import Accessible
from factory import accessible_factory

__all__ = [
           "LoginHelper",
          ]

#------------------------------------------------------------------------------

class LoginHelper(Accessible):
        """
        An interface for use by assistive technologies by which they
        can access system information and services on a 'need to know'
        basis while the screen is locked, during user authentication,
        or during other sensitive operations.         
        This interface is intended for use by assistive technologies
        and related user-enabling services, and by applications and utilities
        which may wish to restrict access to certain system devices and
        services during security-sensitive states, e.g. when the screen
        is locked or during authentication into some secure service.
        Such 'applications' (for instance, screen lock dialogs and security-enabled
        web browsers) use the LoginHelper client interfaces, and the
        bonobo-activation query service, to query for assistive technologies
        which advertise the LoginHelper service. The client then queries
        these assistive technologies for their device I/O requirements,
        via the getDeviceReqs call. The client may then issue the advisory
        request setSafe (TRUE), which requests that the LoginHelper -implementing
        service make a best-effort attempt to make itself more secure
        (for instance, an onscreen keyboard might turn off word prediction,
        and a screenreader may turn off keyboard echo via speech). The
        return value of setSafe is an advisory indication of whether
        this attempt was successful (no specific guarantees are implied).
        Once the 'security sensitive' state is exited, the client should
        call setSafe (FALSE).
        The return values from getDeviceReqs inform the client of which
        services the LoginHelper service (e. g. assistive technology)
        needs in order to do its job. The client may use this information
        to loosen any restrictions on access which it may currently have
        in place (for instance, keyboard grabs, etc.). If it does not
        do so, the likely outcome is that the end-user will experience
        loss of access to the system.
        """

        def getDeviceReqs(self, *args, **kwargs):
                """
                getDeviceReqs:
                Query a LoginHelper for the types of device I/O it requires,
                in order to do its job. For instance, a LoginHelper which needs
                to receive keyboard events will include Accessibility_LoginHelper_CORE_KEYBOARD
                in this list.
                @return : A sequence of LoginHelper_DeviceReq indicating the
                device I/O required in order to facilitate end-user access to
                the system.
                """
                func = self.get_dbus_method("getDeviceReqs", dbus_interface=ATSPI_LOGIN_HELPER)
                return func(*args, **kwargs)

        def getRaiseWindows(self, *args, **kwargs):
                """
                getRaiseWindows:
                Get a list of window IDs that need raising on login.
                @return : a sequence containing window IDS for toplevels which
                need to be raised/made visible during user authentication, in
                order for the LoginHelper to facilitate end-user access to the
                system.
                """
                func = self.get_dbus_method("getRaiseWindows", dbus_interface=ATSPI_LOGIN_HELPER)
                return func(*args, **kwargs)

        def setSafe(self, *args, **kwargs):
                """
                setSafe: 
                @param : safe_mode
                TRUE if the client is requesting that 'safe mode' be initiated,
                FALSE if the client is advising that 'safe mode' may be exited,
                i.e. normal operation may be resumed.
                Request a LoginHelper to enter "safe" mode, or inform LoginHelper
                that "safe" mode may be exited. If safe_mode is TRUE, but the
                return value is FALSE, the requesting client may wish to deny
                services to the LoginHelper, for instance avoid raising its toplevels.
                The return value is purely advisory, and no guarantees are intended
                about what the implementing LoginHelper will do to improve security
                when in "safe" mode.
                @return : whether the LoginHelper is now "safe" or not.
                """
                func = self.get_dbus_method("setSafe", dbus_interface=ATSPI_LOGIN_HELPER)
                return func(*args, **kwargs)

        class DeviceReq(Enum):
                _enum_lookup = {
                        0:'GUI_EVENTS',
                        1:'CORE_KEYBOARD',
                        2:'CORE_POINTER',
                        3:'EXT_INPUT',
                        4:'POST_WINDOWS',
                        5:'AUDIO_OUT',
                        6:'AUDIO_IN',
                        7:'NETWORK',
                        8:'LOCALHOST',
                        9:'SERIAL_OUT',
                        10:'SERIAL_IN',
                }

        AUDIO_IN = DeviceReq(6)
        AUDIO_OUT = DeviceReq(5)
        CORE_KEYBOARD = DeviceReq(1)
        CORE_POINTER = DeviceReq(2)
        EXT_INPUT = DeviceReq(3)
        GUI_EVENTS = DeviceReq(0)
        LOCALHOST = DeviceReq(8)
        NETWORK = DeviceReq(7)
        POST_WINDOWS = DeviceReq(4)
        SERIAL_IN = DeviceReq(10)
        SERIAL_OUT = DeviceReq(9)

        class WindowInfo(list):
                def __new__(cls, winID):
                        list.__new__(cls, (winID))
                def __init__(self, winID):
                        list.__init__(self, (winID))

                def _get_winID(self):
                        return self[0]
                def _set_winID(self, val):
                        self[0] = val
                winID = property(fget=_get_winID, fset=_set_winID)

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(ATSPI_LOGIN_HELPER, LoginHelper)

#END----------------------------------------------------------------------------
