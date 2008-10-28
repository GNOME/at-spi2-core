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
from factory import accessible_factory

__all__ = [
           "Hyperlink",
          ]

#------------------------------------------------------------------------------

class Hyperlink(BaseProxy):
        """
        Instances of Hyperlink are returned by Hypertext objects, and
        are the means by which end users and clients interact with linked,
        and in some cases embedded, content. Hyperlinks may have multiple
        "anchors", where an anchor corresponds to a reference to a particular
        resource with a corresponding resource identified (URI). Hyperlinks
        may be queried for their URIs, or queried for the objects corresponding
        to their anchors. The objects thus obtained are instances of
        Accessible, and may be queried, and manipulated via the Action
        interface.
        """

        def getObject(self, *args, **kwargs):
                """
                Gets the i'th object, (where i is an integer between 0 and Hyperlink::numAnchors
                - 1, inclusive) associated with a Hyperlink. The objects returned
                are usually actionable (i.e. they should implement Accessibility::Action),
                and the available actions often include "open", "bookmark", "save
                link as", etc. They may also implement Accessibility::StreamableContent,
                although clients can normally use getURI to obtain a resource
                locator via which the object's data may be accessed.
                @return an Accessible object instance representing the Hyperlink's
                ith anchor, or through which the content associated with the
                ith anchor can be accessed.
                """
                func = self.get_dbus_method("getObject")
                return func(*args, **kwargs)

        def getURI(self, *args, **kwargs):
                """
                Obtain a resource locator ('URI') which can be used to access
                the content to which this link "points" or is connected. 
                @return a string corresponding to the URI of the Hyperlink's
                'ith' anchor, if one exists, or a NIL string otherwise.
                """
                func = self.get_dbus_method("getURI")
                return func(*args, **kwargs)

        def isValid(self, *args, **kwargs):
                """
                Check the hyperlink to see if a connection to its backing content
                can be established, or if its URI is valid. 
                @return True if the object's content is available, or False if
                the hyperlink's URI is invalid, or a connection to the resource
                can not be established.
                """
                func = self.get_dbus_method("isValid")
                return func(*args, **kwargs)

        def get_endIndex(self):
                return self._pgetter(self._dbus_interface, "endIndex")
        def set_endIndex(self, value):
                self._psetter(self._dbus_interface, "endIndex", value)
        _endIndexDoc = \
                """
                the ending offset within the containing Hypertext content with
                which this Hyperlink is associated; that is, the offset of the
                first element past the range within the Hypertext associated
                with this Hyperlink.
                """
        endIndex = property(fget=get_endIndex, fset=set_endIndex, doc=_endIndexDoc)

        def get_nAnchors(self):
                return self._pgetter(self._dbus_interface, "nAnchors")
        def set_nAnchors(self, value):
                self._psetter(self._dbus_interface, "nAnchors", value)
        _nAnchorsDoc = \
                """
                the number of separate anchors associated with this Hyperlink
                """
        nAnchors = property(fget=get_nAnchors, fset=set_nAnchors, doc=_nAnchorsDoc)

        def get_startIndex(self):
                return self._pgetter(self._dbus_interface, "startIndex")
        def set_startIndex(self, value):
                self._psetter(self._dbus_interface, "startIndex", value)
        _startIndexDoc = \
                """
                the starting offset within the containing Hypertext content with
                which this Hyperlink is associated
                """
        startIndex = property(fget=get_startIndex, fset=set_startIndex, doc=_startIndexDoc)

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(interfaces.ATSPI_HYPERLINK, Hyperlink)

#END----------------------------------------------------------------------------
