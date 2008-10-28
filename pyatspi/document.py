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
           "Document",
          ]

#------------------------------------------------------------------------------

class Document(BaseProxy):
        """
        Primarily a 'tagging' interface which indicates the start of
        document content in the Accessibility hierarchy. Accessible objects
        below the node implementing Document are normally assumed to
        be part of the document content. Attributes of Document are those
        attributes associated with the document as a whole. Objects that
        implement Document are normally expected to implement Collection
        as well.
        """

        def getAttributeValue(self, *args, **kwargs):
                """
                Gets the value of a single attribute, if specified for the document
                as a whole.
                @param : attributename
                a string indicating the name of a specific attribute (name-value
                pair) being queried.
                @return a string corresponding to the value of the specified
                attribute, or an empty string if the attribute is unspecified
                for the object.
                """
                func = self.get_dbus_method("getAttributeValue")
                return func(*args, **kwargs)

        def getAttributes(self, *args, **kwargs):
                """
                Gets all attributes specified for a document as a whole. For
                attributes which change within the document content, see Accessibility::Text::getAttributes
                instead.
                @return an AttributeSet containing the attributes of the document,
                as name-value pairs.
                """
                func = self.get_dbus_method("getAttributes")
                return func(*args, **kwargs)

        def getLocale(self, *args, **kwargs):
                """
                Gets the locale associated with the document's content. e.g.
                the locale for LOCALE_TYPE_MESSAGES.
                @return a string compliant with the POSIX standard for locale
                description.
                """
                func = self.get_dbus_method("getLocale")
                return func(*args, **kwargs)

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(interfaces.ATSPI_DOCUMENT, Document)

#END----------------------------------------------------------------------------
