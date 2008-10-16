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
from accessible import Accessible

__all__ = [
                   "Application",
                  ]

#------------------------------------------------------------------------------

class Application(Accessible):
        """
        An interface identifying an object which is the root of the user
        interface Accessible hierarchy associated with a running application.
        Children of Application are typically, but not exclusively, top-level
        windows.
        """

        def getLocale(self, *args, **kwargs):
                """
                Gets the locale in which the application is currently operating.
                For the current message locale, use lctype LOCALE_TYPE_MESSAGES.
                @param : lctype
                The LocaleType for which the locale is queried. 
                @return a string compliant with the POSIX standard for locale
                description.
                """
                func = self.get_dbus_method("getLocale")
                return func(*args, **kwargs)

        def pause(self, *args, **kwargs):
                """
                Request that the application temporarily stop sending events.
                In most cases this should pause the application's main event
                loop.
                @return : true if the request succeeded, false otherwise.
                """
                func = self.get_dbus_method("pause")
                return func(*args, **kwargs)

        def registerObjectEventListener(self, *args, **kwargs):
                """
                registerObjectEventListener: 
                @param : listener
                an EventListener object which will receive the requested events
                @param : eventName
                a UTF-8 string indicating the type of (toolkit-specific) event
                being requested. Register with this application toolkit for "Accessibility::Accessible"
                event notifications.
                """
                func = self.get_dbus_method("registerObjectEventListener")
                return func(*args, **kwargs)

        def registerToolkitEventListener(self, *args, **kwargs):
                """
                @param : listener
                an EventListener object which will receive the requested events
                from the application's toolkits via toolit 'bridges' 
                @param : eventName
                a UTF-8 string indicating the type of (toolkit-specific) event
                being requested. Not all applications can generate toolkit events
                of a given type.
                Register with this application's toolkit for "toolkit-specific"
                event notifications.
                """
                func = self.get_dbus_method("registerToolkitEventListener")
                return func(*args, **kwargs)

        def resume(self, *args, **kwargs):
                """
                Request that the application resume sending events.
                @return : True if the request succeeded, False otherwise.
                """
                func = self.get_dbus_method("resume")
                return func(*args, **kwargs)

        def get_id(self):
                return self._pgetter(self._dbus_interface, "id")
        def set_id(self, value):
                self._psetter(self._dbus_interface, "id", value)
        _idDoc = \
                """
                The application instance's unique ID as assigned by the registry.
                """
        id = property(fget=get_id, fset=set_id, doc=_idDoc)

        def get_toolkitName(self):
                return self._pgetter(self._dbus_interface, "toolkitName")
        def set_toolkitName(self, value):
                self._psetter(self._dbus_interface, "toolkitName", value)
        _toolkitNameDoc = \
                """
                A string indicating the type of user interface toolkit which
                is used by the application.
                """
        toolkitName = property(fget=get_toolkitName, fset=set_toolkitName, doc=_toolkitNameDoc)

        def get_version(self):
                return self._pgetter(self._dbus_interface, "version")
        def set_version(self, value):
                self._psetter(self._dbus_interface, "version", value)
        _versionDoc = \
                """
                A string indicating the version number of the application's accessibility
                bridge implementation.
                """
        version = property(fget=get_version, fset=set_version, doc=_versionDoc)

# ATTENTION - Register the Application class with the accessible factory.
add_accessible_class(interfaces.ATSPI_APPLICATION, Application)

#END----------------------------------------------------------------------------
