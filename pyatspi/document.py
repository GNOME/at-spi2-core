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

__all__ = [
           "Document",
          ]

#------------------------------------------------------------------------------

class Document(Accessible):
        """
        Primarily a 'tagging' interface which indicates the start of
        document content in the Accessibility hierarchy. Accessible objects
        below the node implementing Document are normally assumed to
        be part of the document content. Attributes of Document are those
        attributes associated with the document as a whole. Objects that
        implement Document are normally expected to implement Collection
        as well.
        """

        def getAttributeValue(self, key):
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
                func = self.get_dbus_method("getAttributeValue", dbus_interface=ATSPI_DOCUMENT)
                return func(key)

        def getAttributes(self):
                """
                Gets all attributes specified for a document as a whole. For
                attributes which change within the document content, see Accessibility::Text::getAttributes
                instead.
                @return an AttributeSet containing the attributes of the document,
                as name-value pairs.
                """
                func = self.get_dbus_method("getAttributes", dbus_interface=ATSPI_DOCUMENT)
                return [key + ':' + value for key, value in func().values()]

        def getLocale(self):
                """
                Gets the locale associated with the document's content. e.g.
                the locale for LOCALE_TYPE_MESSAGES.
                @return a string compliant with the POSIX standard for locale
                description.
                """
                func = self.get_dbus_method("getLocale", dbus_interface=ATSPI_DOCUMENT)
                return func()

# Register the accessible class with the factory.
accessible_factory.register_accessible_class(ATSPI_DOCUMENT, Document)

#END----------------------------------------------------------------------------
